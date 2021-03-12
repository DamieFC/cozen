#include "CPU.h"
#include "common.h"
#include <libk/logging.h>
CPUInfo *cpu_info;

void cpuid(uint32_t code, uint32_t *a, uint32_t *c, uint32_t *d)
{
    __asm__ volatile("cpuid"
                     : "=a"(*a), "=c"(*c), "=d"(*d)
                     : "a"(code)
                     : "ebx");
}

void cpuid_string(int code, uint32_t *a, uint32_t *b, uint32_t *c,
                  uint32_t *d)
{
    __asm__ volatile("cpuid"
                     : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
                     : "a"(code));
}

char *CPU_get_vendor_name(char buf[13])
{
    uint32_t a, b, c, d;
    cpuid_string(0, &a, &b, &c, &d);
    char *ebx = (char *)&b;
    char *ecx = (char *)&c;
    char *edx = (char *)&d;
    buf[0] = ebx[0];
    buf[1] = ebx[1];
    buf[2] = ebx[2];
    buf[3] = ebx[3];
    buf[4] = edx[0];
    buf[5] = edx[1];
    buf[6] = edx[2];
    buf[7] = edx[3];
    buf[8] = ecx[0];
    buf[9] = ecx[1];
    buf[10] = ecx[2];
    buf[11] = ecx[3];
    buf[12] = 0;
    return buf;
}

char *CPU_get_hypervisor_name(char buf[13]) {
	uint32_t a, b, c, d;
	cpuid_string(0x40000000, &a, &b, &c, &d);
	char *ebx = (char *)&b;
	char *ecx = (char *)&c;
	char *edx = (char *)&d;
	buf[0] = ebx[0];
	buf[1] = ebx[1];
	buf[2] = ebx[2];
	buf[3] = ebx[3];
	buf[4] = ecx[0];
	buf[5] = ecx[1];
	buf[6] = ecx[2];
	buf[7] = ecx[3];
	buf[8] = edx[0];
	buf[9] = edx[1];
	buf[10] = edx[2];
	buf[11] = edx[3];
	buf[12] = 0;
	return buf;
}

uint8_t CPU_is_hypervisor()
{
    uint32_t a, c, d;
    cpuid(CPU_GET_FEATURES, &a, &c, &d);
    if (!BIT_IS_SET(c, CPU_FEAT_HYPERVISOR))
        return 0;
    else
        return 1;
}

void CPU_init()
{
    module("CPU");
    char vbuf[13];
    cpu_info->vendor = CPU_get_vendor_name(vbuf);
    cpu_info->hypervisor = CPU_is_hypervisor();

    log(INFO, "CPU Vendor: %s", cpu_info->vendor);
    if (cpu_info->hypervisor)
    {
	char hbuf[13];
	cpu_info->hypervisor_vendor = CPU_get_hypervisor_name(hbuf);
        log(INFO, "Hypervisor detected: %s", cpu_info->hypervisor_vendor);
    }
}
