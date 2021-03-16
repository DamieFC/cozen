#ifndef MEMUTIL_H
#define MEMUTIL_H
#include <stddef.h>
#include <stdint.h>

void memset8(uint8_t *src, uint8_t data, size_t count);
void memset64(uint64_t *src, uint64_t data, size_t count);
void bitmap_free_region(uint8_t *bitmap, size_t start, size_t limit);

#endif
