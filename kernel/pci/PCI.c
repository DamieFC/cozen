/*
BSD 3-Clause License

Copyright (c) 2021, Abbix, Smart6502
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "PCI.h"
#include "pci_id.h"
#include <devices/ahci/ahci.h>
#include <libk/logging.h>
PCIDevice *pci_devices;

uint32_t PCI_raw_read(uint8_t bus, uint8_t device, uint8_t function,
                      uint8_t reg)
{
    IO_outl(0xCF8, (1 << 31) | ((uint32_t)bus << 16) |
                       (((uint32_t)device & 31) << 11) |
                       (((uint32_t)function & 7) << 8) | (reg & 0xFC));
    return IO_inl(0xCFC);
}

uint32_t PCI_read_dword(PCIDevice *device, uint8_t reg)
{
    uint32_t bus32 = (uint32_t)device->bus;
    uint32_t device32 = (uint32_t)device->device;
    uint32_t function32 = (uint32_t)device->function;

    uint32_t target = 0x80000000 | (bus32 << 16) | (device32 << 11) |
                      ((function32) << 8) | (reg & 0xFC);

    IO_outl(0xcf8, target);

    return IO_inl(0xCFC);
}

void PCI_write_dword(PCIDevice *device, uint8_t reg, uint32_t data)
{
    uint32_t bus32 = (uint32_t)device->bus;
    uint32_t device32 = (uint32_t)device->device & 31;
    uint32_t function32 = (uint32_t)device->function & 7;

    uint32_t target = (1 << 31) | (bus32 << 16) | (device32 << 11) |
                      (function32 << 8) | (reg & 0xFC);

    IO_outl(0xCF8, target);
    IO_outl(0xCFC, data);
}

#define get_vendor(bus, device, function) \
    (uint16_t)(PCI_raw_read(bus, device, function, 0))

#define get_device_id(bus, device, function) \
    (uint16_t)(((uint32_t)PCI_raw_read(bus, device, function, 0)) >> 16)

#define get_class(bus, device, function) \
    (uint8_t)(PCI_raw_read(bus, device, function, 0x8) >> 24)

#define get_subclass(bus, device, function) \
    (uint8_t)(PCI_raw_read(bus, device, function, 0x8) >> 16)

#define get_prog_if(bus, device, function) \
    (uint8_t)(PCI_raw_read(bus, device, function, 0x8) >> 8)

#define get_header_type(bus, device, function) \
    (uint8_t)(PCI_raw_read(bus, device, function, 0xC) >> 16)

#define get_secondary_bus(device) (uint8_t)(PCI_read_dword(device, 0x18) >> 8)

uint8_t is_bridge(PCIDevice *device)
{
    if ((get_header_type(device->bus, device->device, device->function) &
         ~(0x80)) != 0x1)
        return 0;
    if (get_class(device->bus, device->device, device->function) != 0x6)
        return 0;
    if (get_subclass(device->bus, device->device, device->function) != 0x4)
        return 0;

    return 1;
}

int PCI_get_bar(PCIDevice *device, PCIBar *bar, size_t num)
{
    if ((get_header_type(device->bus, device->device, device->function) &
         ~(0x80)) != 0)
        return 1;

    /* FIXME: Add IO/mem and 64-bit bars */
    size_t offset = 0x10 + num * 4;

    bar->base = PCI_read_dword(device, offset);

    PCI_write_dword(device, offset, 0xFFFFFFFF);
    bar->size = ~(PCI_read_dword(device, offset)) + 1;
    PCI_write_dword(device, offset, bar->base);
    return 0;
}

uint64_t device_count = 0;

void PCI_add_device(uint8_t bus, uint8_t device, uint8_t function)
{
    PCIDevice ndev = {.bus = bus,
                      .device = device,
                      .function = function,
                      .class = get_class(bus, device, function),
                      .subclass = get_subclass(bus, device, function),
                      .prog_if = get_prog_if(bus, device, function),
                      .vendor_id = get_vendor(bus, device, function),
                      .device_id = get_device_id(bus, device, function)};
    device_count++;
    pci_devices[device_count] = ndev;
}

/* FIXME: First device is not valid */

void PCI_scan_bus(uint8_t bus)
{
    uint8_t device, function;
    for (device = 0; device < 32; device++)
    {
        if (get_vendor(bus, device, 0) != 0xFFFF)
        {
            if (get_header_type(bus, device, 0) & 0x80)
            {
                for (function = 0; function < 9; function++)
                    if (get_vendor(bus, device, function) != 0xFFFF)
                        PCI_add_device(bus, device, function);
            }
            else
            {
                PCI_add_device(bus, device, 0);
            }

            if (is_bridge(&pci_devices[device]))
            {
                PCI_scan_bus(get_secondary_bus(&pci_devices[device]));
            }
        }
    }
}

void PCI_init(void)
{
    module("PCI");
    PCI_scan_bus(0);

    uint64_t device;

    for (device = 0; device < device_count; device++)
    {
        module("PCI");
        log(INFO, "Found device with vendor %x, device id: %x",
            pci_devices[device].vendor_id, pci_devices[device].device_id);

        VBE_putf("[PCI] 00:%x.%d %s: %s %s", device, pci_devices[device].function,
                 PCI_id_to_string(&pci_devices[device]),
                 PCI_vendor_to_string(&pci_devices[device]),
                 PCI_device_id_to_string(&pci_devices[device]));

        switch (pci_devices[device].class)
        {
        case 1:
            switch (pci_devices[device].subclass)
            {
            case 6:
                AHCI_init(&pci_devices[device]);
                break;
            }
            break;
        }
    }
}
