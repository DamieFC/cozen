#ifndef SMP_H
#define SMP_H

#include <boot/stivale2.h>
#include <stdbool.h>
#include <stdint.h>

#define CPU_GET_VENDOR_ID 0x0
#define CPU_GET_FEATURES 0x1
#define CPU_FEAT_HYPERVISOR 31
#define CPU_GET_HYPERVISOR_NAME 0x40000000

typedef struct
{
    char *vendor;
    bool is_hypervisor;
    char *hypervisor_name;
    struct stivale2_struct_tag_smp *smp_info;
} CPUInfo;

extern CPUInfo *cpu_info;

void CPU_init(struct stivale2_struct_tag_smp *smp_info);

#endif
