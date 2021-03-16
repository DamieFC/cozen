#ifndef COMMON_H
#define COMMON_H

#define is_bit_set(var, pos) \
    (((uint64_t)var) & (1ULL << (pos)))

#define bitmap_set_bit(bitmap, location) \
    (bitmap)[(location) / 8] |= (1 << ((location) % 8))

#define bitmap_clear_bit(bitmap, location) \
    (bitmap)[(location) / 8] &= ~(1 << ((location) % 8))

#define roundup(a, b) (((a) + ((b)-1)) / (b))

#define convert_to_mb(bytes) (bytes / 1048576)
#define convert_to_kb(bytes) (bytes / 1024)

#endif
