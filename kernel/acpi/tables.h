/*
BSD 3-Clause License

Copyright (c) 2021, Smart6502, Abbix
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
#ifndef TABLES_H
#define TABLES_H
#include <stdint.h>

struct RSDP
{
    char signature[8];
    uint8_t checksum;
    char oem_id[6];
    uint8_t revision;
    uint32_t rsdt_addr;

    uint32_t length;
    uint64_t xsdt_addr;
    uint8_t extended_checksum;
    uint8_t reserved;
} __attribute__((packed));

struct SDT_desc
{
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} __attribute__((packed));

struct RSDT
{
    struct SDT_desc sdt;
    uint32_t sdt_pointer[];
} __attribute__((packed));

struct XSDT
{
    struct SDT_desc sdt;
    uint64_t sdt_pointer[];
} __attribute__((packed));

struct Generic_address_structure
{
    uint8_t AddressSpace;
    uint8_t BitWidth;
    uint8_t BitOffset;
    uint8_t AccessSize;
    uint64_t Address;
} __attribute__((packed));

struct FADT
{
    struct SDT_desc sdt;
    uint32_t firmware_ctrl;
    uint32_t dsdt;

    /* Only used in ACPI 1.0 */
    uint8_t reserved;

    uint8_t preferred_power_management_profile;
    uint16_t sci_interrupt;
    uint32_t smi_command_port;
    uint8_t acpi_enable;
    uint8_t acpi_disable;
    uint8_t S4BIOS_REQ;
    uint8_t PSTATE_control;
    uint32_t PM1a_event_block;
    uint32_t PM1b_event_block;
    uint32_t PM1a_control_block;
    uint32_t PM1b_control_block;
    uint32_t PM2_control_block;
    uint32_t PM_timer_block;
    uint32_t GPE0_block;
    uint32_t GPE1_block;
    uint8_t PM1_event_length;
    uint8_t PM1_control_length;
    uint8_t PM2_control_length;
    uint8_t PM_timer_length;
    uint8_t GPE0_length;
    uint8_t GPE1_length;
    uint8_t GPE1_base;
    uint8_t CState_control;
    uint16_t worst_C2_latency;
    uint16_t worst_C3_latency;
    uint16_t flush_size;
    uint16_t flush_stride;
    uint8_t duty_offset;
    uint8_t duty_width;
    uint8_t day_alarm;
    uint8_t month_alarm;
    uint8_t century;

    /* Reserved in ACPI 1.0 used in 2.0 */
    uint16_t boot_architecture_flags;

    uint8_t reserved2;
    uint32_t flags;

    /* 12 byte struct */
    struct Generic_address_structure ResetReg;

    uint8_t reset_value;
    uint8_t reserved3[3];

    /* ACPI 2.0 (64-bit) */
    uint64_t X_firmware_control;
    uint64_t X_dsdt;

    struct Generic_address_structure X_PM1a_event_block;
    struct Generic_address_structure X_PM1b_event_block;
    struct Generic_address_structure X_PM1a_control_block;
    struct Generic_address_structure X_PM1b_control_block;
    struct Generic_address_structure X_PM2_control_block;
    struct Generic_address_structure X_PM_timer_block;
    struct Generic_address_structure X_GPE0_block;
    struct Generic_address_structure X_GPE1_block;
} __attribute__((packed));

struct MCFG_config_entry
{
    uint64_t base_address;
    uint16_t segment_group_num;
    uint8_t start_bus_num;
    uint8_t end_bus_num;
    uint32_t reserved;
} __attribute__((packed));

struct MCFG
{
    struct SDT_desc sdt;
    uint64_t reserved;
    struct MCFG_config_entry entries[];
} __attribute__((packed));

#endif
