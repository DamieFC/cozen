#include "pmm.h"
#include <libk/logging.h>
#include <libk/memutils.h>
#include <system/common.h>

static uint8_t *pages_bitmap;
size_t phys_mem;
static size_t pages_bitmap_size; /*, last_page = 0; */

void PMM_init(struct stivale2_struct_tag_memmap *memory_map)
{
    module("PMM");

    log(INFO, "Mmap address: 0x%x", (uint64_t)memory_map);

    size_t entry_index = 0;
    pages_bitmap_size = (size_t)(phys_mem / PAGE_SIZE / 8);
    struct stivale2_mmap_entry entry;

    log(INFO, "Physical memory: %d MB", phys_mem / 1048576);
    log(INFO, "Bitmap size: %d", pages_bitmap_size);

    for (entry_index = 0; entry_index < memory_map->entries; entry_index++)
    {
        entry = memory_map->memmap[entry_index];

        log(INFO, "Entry: %d [ %x -> %x ] :: size %d KB :: type %d",
            entry_index,
            entry.base,
            entry.base + entry.length,
            entry.length / 1024,
            entry.type);

        if (entry.type == STIVALE2_MMAP_USABLE && entry.length >= pages_bitmap_size)
        {
            pages_bitmap = (uint8_t *)(entry.base + MEM_OFFSET);
            memset64((uint64_t *)pages_bitmap, 0xff, pages_bitmap_size);

            log(INFO, "Bitmap is using entry %d", entry_index);
            break;
        }
    }

    for (entry_index = 0; entry_index < memory_map->entries; entry_index++)
    {
        entry = memory_map->memmap[entry_index];
        if (entry.type == 1)
            bitmap_free_region(pages_bitmap, entry.base / PAGE_SIZE, entry.length / PAGE_SIZE);
    }

    log(INFO, "PMM initialized!");
}
