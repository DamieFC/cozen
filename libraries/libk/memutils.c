#include "memutils.h"
#include <system/common.h>

void memset8(uint8_t *src, uint8_t data, size_t count)
{
    size_t i;
    for (i = 0; i < count; i++)
        *src++ = data;
}

void memset64(uint64_t *src, uint64_t data, size_t count)
{
    size_t i;
    for (i = 0; i < count; i++)
        *src++ = data;
}

void bitmap_free_region(uint8_t *bitmap, size_t start, size_t limit)
{
    size_t i;
    for (i = start; i < start + limit; i++)
        bitmap_clear_bit(bitmap, i);
}
