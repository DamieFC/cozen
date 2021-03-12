#include "cpu.h"
#include <libk/logging.h>

void cpuid(int code, uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d)
{
	__asm__ volatile ("cpuid"
			  : "=a" (*a), "=b" (*b), "=c" (*c), "=d" (*d)
			  : "a" (code));
}

char *CPU_get_vendor_name(char buf[13])
{
  uint32_t a, b, c, d;
  cpuid(0, &a, &b, &c, &d);
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

