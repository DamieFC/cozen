/* 
 * SPDX-License-Identifier: MIT
 *
 * MIT License
 *
 * Copyright (c) 2021 Abb1x, Smart6502
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
*/

#include "PCI.h"
#include "pci_id.h"
#include <libk/logging.h>
PCIDevice *pci_devices;

uint32_t PCI_raw_read(uint8_t bus, uint8_t device, uint8_t function,
                      uint8_t reg) {
  IO_outl(0xCF8, (1 << 31) | ((uint32_t)bus << 16) |
                     (((uint32_t)device & 31) << 11) |
                     (((uint32_t)function & 7) << 8) | (reg & 0xFC));
  return IO_inl(0xCFC);
}

uint32_t PCI_read_dword(PCIDevice *device, uint8_t reg) {
  uint32_t bus32 = (uint32_t)device->bus;
  uint32_t device32 = (uint32_t)device->device;
  uint32_t function32 = (uint32_t)device->function;

  uint32_t target = 0x80000000 | (bus32 << 16) | (device32 << 11) |
                    ((function32) << 8) | (reg & 0xFC);

  IO_outl(0xcf8, target);

  return IO_inl(0xCFC);
}

void PCI_write_dword(PCIDevice *device, uint8_t reg, uint32_t data) {
  uint32_t bus32 = (uint32_t)device->bus;
  uint32_t device32 = (uint32_t)device->device & 31;
  uint32_t function32 = (uint32_t)device->function & 7;

  uint32_t target = (1 << 31) | (bus32 << 16) | (device32 << 11) |
                    (function32 << 8) | (reg & 0xFC);

  IO_outl(0xCF8, target);
  IO_outl(0xCFC, data);
}

#define get_vendor(bus, device, function)                                      \
  (uint16_t)(PCI_raw_read(bus, device, function, 0))

#define get_device_id(bus, device, function)                                   \
  (uint16_t)(((uint32_t)PCI_raw_read(bus, device, function, 0)) >> 16)

#define get_class(bus, device, function)                                       \
  (uint8_t)(PCI_raw_read(bus, device, function, 0x8) >> 24)

#define get_subclass(bus, device, function)                                    \
  (uint8_t)(PCI_raw_read(bus, device, function, 0x8) >> 16)

#define get_header_type(bus, device, function)                                 \
  (uint8_t)(PCI_raw_read(bus, device, function, 0xC) >> 16)

#define get_secondary_bus(device) (uint8_t)(PCI_read_dword(device, 0x18) >> 8)

uint8_t is_bridge(PCIDevice *device) {
  if ((get_header_type(device->bus, device->device, device->function) &
       ~(0x80)) != 0x1)
    return 0;
  if (get_class(device->bus, device->device, device->function) != 0x6)
    return 0;
  if (get_subclass(device->bus, device->device, device->function) != 0x4)
    return 0;

  return 1;
}

/* Getting BAR */
uint8_t PCI_get_bar(PCIDevice *device, PCIBar *bar, size_t num) {
  if ((get_header_type(device->bus, device->device, device->function) &
       ~(0x80)) != 0)
    return 1;

  /* FIXME: Add IO/mem and 64-bit bars detection */
  size_t offset = 0x10 + num * 4;

  bar->base = PCI_read_dword(device, offset);

  PCI_write_dword(device, offset, 0xFFFFFFFF);
  bar->size = ~(PCI_read_dword(device, offset)) + 1;
  PCI_write_dword(device, offset, bar->base);
  return 0;
}

uint64_t current_count = 0;

void PCI_scan_device(PCIDevice *dev, uint8_t bus, uint8_t device,
                     uint8_t function) {
  PCIDevice ndev = {.bus = bus,
                    .device = device,
                    .function = function,
                    .class = get_class(bus, device, function),
                    .subclass = get_subclass(bus, device, function),
                    .vendor_id = get_vendor(bus, device, function),
                    .device_id = get_device_id(bus, device, function)};
  *dev = ndev;
}

void PCI_scan_fn(PCIDevice *dev, uint8_t bus, uint8_t device) {
  uint8_t function = 0;
  if (get_header_type(bus, device, 0) & 0x80) {
    for (function = 1; function < 8; function++)
      if (get_vendor(bus, device, function) != 0xFFFF)
        /* FIXME: Implement vector arrays and stuff for multiple functions */
        PCI_scan_device(dev, bus, device, function);
  } else
    PCI_scan_device(dev, bus, device, function);
}

void PCI_scan_bus(uint8_t bus) {
  log(INFO, "Checking bus %x", (uint32_t)bus);

  uint8_t device;

  for (device = 0; device < 32; device++) {
    if (get_vendor(bus, device, 0) != 0xFFFF) {
      current_count++;
      /* Multifunction devices */
      PCI_scan_fn(&pci_devices[device], bus, device);

      if (is_bridge(&pci_devices[device])) /* If it's a bridge */
      {
        PCI_scan_bus(get_secondary_bus(&pci_devices[device]));
      }
    }
  }
}

void PCI_init() {
  module("PCI");

  PCI_scan_bus(0);

  uint64_t device;

  for (device = 0; device < current_count; device++) {
    log(INFO, "Found device with vendor %x, device id: %x",
        pci_devices[device].vendor_id, pci_devices[device].device_id);

    VBE_putf("[PCI] 00:%x.%d %s: %s %s", device, pci_devices[device].function,
             PCI_id_to_string(&pci_devices[device]),
             PCI_vendor_to_string(&pci_devices[device]),
             PCI_device_id_to_string(&pci_devices[device]));
  }
}
