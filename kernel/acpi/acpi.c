/*
BSD 3-Clause License

Copyright (c) 2021, Smart6502
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
#include "acpi.h"
#include <devices/video/vbe.h>
#include <libk/logging.h>
#include <stdbool.h>
ACPI_Info *acpi_info = {0};

bool ACPI_do_checksum(struct SDT_desc *sdt)
{
    unsigned char sum = 0;
    uint32_t i;

    for (i = 0; i < sdt->length; i++)
        sum += ((char *)sdt)[i];

    return (sum == 0) ? true : false;
}

void *ACPI_find_table(struct RSDT *rsdt, struct XSDT *xsdt, char *sig)
{
    int i, entries;
    if (acpi_info->version == 1)
        entries = (rsdt->sdt.length - sizeof(rsdt->sdt)) / 4;
    else
        entries = (xsdt->sdt.length - sizeof(xsdt->sdt)) / 8;

    struct SDT_desc *h;
    for (i = 0; i < entries; i++)
    {
        if (acpi_info->version == 1)
        {
            h = (struct SDT_desc *)(uintptr_t)rsdt->sdt_pointer[i];
        }
        else
        {
            h = (struct SDT_desc *)(uintptr_t)xsdt->sdt_pointer[i];
        }

        if (h->signature[0] == sig[0] && h->signature[1] == sig[1] && h->signature[2] == sig[2] && h->signature[3] == sig[3])
        {
            if (ACPI_do_checksum(h))
                return (void *)h;
            else
            {
                log(INFO, "Found %s but checksum was invalid", sig);
                return 0;
            }
        }
    }
    return 0;
}

void ACPI_init(uint64_t rsdp_location)
{
    module("ACPI");

    struct RSDP *rsdp = (struct RSDP *)(uintptr_t)rsdp_location;
    struct RSDT *rsdt = NULL;
    struct XSDT *xsdt = NULL;
    struct FADT *fadt = NULL;

    if (rsdp->revision == 0)
    {
        acpi_info->version = 1;
        rsdt = (struct RSDT *)(uintptr_t)rsdp->rsdt_addr;
        if (!ACPI_do_checksum((struct SDT_desc *)rsdt))
        {
            log(INFO, "RSDT checksum failed");
            return;
        }
    }

    else if (rsdp->revision <= 2)
    {
        acpi_info->version = rsdp->revision;
        xsdt = (struct XSDT *)(uintptr_t)rsdp->xsdt_addr;
        if (!ACPI_do_checksum((struct SDT_desc *)xsdt))
        {
            log(INFO, "XSDT checksum failed");
            return;
        }
    }

    fadt = ACPI_find_table(rsdt, xsdt, "FACP");
    log(INFO, "Detected ACPI version %d", acpi_info->version);
    VBE_putf("OEM: %s", rsdp->oem_id);

    acpi_info->rsdt = rsdt;
    acpi_info->xsdt = xsdt;
    acpi_info->fadt = fadt;

    log(INFO, "ACPI initialized!");
}
