#ifndef PMM_H
#define PMM_H
#include <boot/boot.h>
#include <boot/stivale2.h>
#include <stddef.h>
#include <system/common.h>

#define MEM_USABLE 1

#define PAGE_SIZE 0x1000

extern size_t phys_mem;
void PMM_init(struct stivale2_struct_tag_memmap *memory_map);
void *PMM_alloc(uint64_t count);

#endif
