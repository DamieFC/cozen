/*
BSD 3-Clause License

Copyright (c) 2021, Abbix
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
#include "ahci.h"
#include <libk/logging.h>
#include <system/common.h>

int AHCI_check_port_type(HBA_port *port)
{
    if ((uint8_t)((port->ssts) & 0b111) != 0x3)
        return 0;
    else if ((uint8_t)(((port->ssts) >> 8) & 0b111) != 0x1)
        return 0;

    switch (port->sig)
    {
    case SIG_SATA_ATA:
        return SATA_ATA;

    case SIG_SATA_ATAPI:
        return SATA_ATAPI;

    case SIG_SATA_PM:
        return SATA_PM;

    default:
        return 0;
    }
}

void AHCI_init(PCIDevice *ahci_dev)
{
    module("AHCI");
    switch (ahci_dev->prog_if)
    {
    case 0:
        log(INFO, "Detected a vendor specific interface (ur pc is stupid)");
        return;
    case 1:
        log(INFO, "Detected a AHCI 1.0 compatible device");
        break;
    case 2:
        log(INFO, "Detected a serial storage bus");
        return;
    } 

    volatile HBA_mem *hba_mem = (volatile HBA_mem *)(uint64_t)(ahci_dev->BAR[4]);
    int i;

    for (i = 0; i < 32; i++)
    {
        if (hba_mem->pi & (1 << i))
        {
            volatile HBA_port *port_regs = (volatile HBA_port *)&hba_mem->ports[i];
            int port_type = AHCI_check_port_type(port_regs);

            switch (port_type)
            {
            case SATA_ATA:
                log(INFO, "SATA drive found on port %d", i);
                break;

            case SATA_ATAPI:
                log(INFO, "Enclosure management bridge found on port %d", i);
                break;

            case SATA_PM:
                log(INFO, "Port multiplier found on port %d", i);
                break;

            default:
                break;
            }
        }
    }
}


