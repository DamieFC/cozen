#ifndef CPU_H
#define CPU_H
#pragma once
#include <stdint.h>

#define CPU_GET_VENDOR_ID 0x0
#define CPU_GET_FEATURES 0x1
#define CPU_FEAT_HYPERVISOR 31

typedef struct
{
    char *vendor;
    uint8_t hypervisor;
    char *hypervisor_vendor;
} CPUInfo;

char *CPU_get_vendor_name(char buf[13]);
char *CPU_get_hypervisor_name(char buf[13]);
char *CPU_get_model();
uint8_t CPU_is_hypervisor();
void CPU_init();

#endif
