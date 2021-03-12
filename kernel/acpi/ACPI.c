/*
BSD 3-Clause License

Copyright (c) 2021, Smart6502 (Xenon6502)
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
#include "ACPI.h"
#include "tables.h"
#include <libk/logging.h>
#include <libk/module.h>
ACPI_info *acpi_info;

uint8_t do_checksum(struct RSDT_desc *root_sdt)
{
	unsigned char sum = 0;
	uint32_t i;

	for (i = 0; i < root_sdt->length; i++)
	{
		sum += ((char*)root_sdt)[i];
	}

	if (sum == 0)
		return 0;
	return 1;
}

void ACPI_init(uint64_t rsdp_location)
{
    module("ACPI");

    log(INFO, "Found RSDP at 0x%x", rsdp_location);
    struct RSDP_desc *rsdp = (struct RSDP_desc *)(uintptr_t)rsdp_location;
    acpi_info->rsdp = rsdp;

    if (rsdp->revision == 0)
        acpi_info->version = 1;
    else if (rsdp->revision <= 2)
        acpi_info->version = rsdp->revision;

    log(INFO, "Detected ACPI version %d", acpi_info->version);
    log(INFO, "OEM name: %s", rsdp->oem_id);

    log(INFO, "Found RSDT at 0x%x", rsdp->rsdt_addr);
    struct RSDT_desc *rsdt = (struct RSDT_desc *)(uintptr_t)rsdp->rsdt_addr;
    if (!do_checksum(rsdt))
	    log(INFO, "RSDT checksum is OK");
    else return;
    log(INFO, "OEM revision: %d", rsdt->creator_revision);
}
