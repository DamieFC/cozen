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
#include "smp.h"
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

void cpuid_string(int code, uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d)
{
    __asm__ volatile("cpuid"
                     : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
                     : "a"(code));
}

char *CPU_get_vendor_name(char buf[13])
{
    uint32_t a, b, c, d;
    cpuid_string(CPU_GET_VENDOR_ID, &a, &b, &c, &d);
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

char *CPU_get_hypervisor_name(char buf[13])
{
    uint32_t a, b, c, d;
    cpuid_string(CPU_GET_HYPERVISOR_NAME, &a, &b, &c, &d);
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

bool CPU_is_hypervisor(void)
{
    uint32_t a, c, d;
    cpuid(CPU_GET_FEATURES, &a, &c, &d);
    if (is_bit_set(c, CPU_FEAT_HYPERVISOR))
        return true;
    else
        return false;
}

void CPU_init(struct stivale2_struct_tag_smp *smp_info)
{
    module("CPU");

    char buf[13];
    cpu_info->vendor = CPU_get_vendor_name(buf);
    cpu_info->is_hypervisor = CPU_is_hypervisor();
    cpu_info->smp_info = smp_info;
    log(INFO, "CPU count: %d", cpu_info->smp_info->cpu_count);
    if (cpu_info->vendor)
        log(INFO, "CPU vendor: %s", buf);
    else
        log(INFO, "Unknown CPU vendor");

    char hbuf[13];
    if (cpu_info->is_hypervisor)
    {
        cpu_info->hypervisor_name = CPU_get_hypervisor_name(hbuf);
        log(INFO, "Running on hypervisor %s", cpu_info->hypervisor_name);
    }
}
