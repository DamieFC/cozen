#ifndef CPU_H
#define CPU_H
#include <stdint.h>

#define CPU_GET_VENDOR_ID 0x0
#define CPU_GET_FEATURES 0x1
#define CPU_FEAT_HYPERVISOR 31

char *CPU_get_vendor_name();
uint8_t CPU_is_hypervisor();
void CPU_init();

typedef struct CPUInfo
{
    char *vendor;
    uint8_t hypervisor;
    char *hypervisor_vendor;
} CPUInfo;

#endif
