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

    PCIBar devbar;
    if (PCI_get_bar(ahci_dev, &devbar, 5) == 1)
    {
        log(INFO, "Failed getting AHCI device BAR5");
        return;
    }

    volatile HBA_mem *hba_mem = (volatile HBA_mem *)(uint64_t)(devbar.base);
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
