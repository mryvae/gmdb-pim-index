#ifndef _BITMAP_H
#define _BITMAP_H

#include <stdint.h>
#include <stdio.h>
#include <mram.h>
#include <alloc.h>

#define BITMAP_OK 0
#define BITMAP_ERR 1

#define BITMAP_SIZE_DEFAULT ((uint32_t)(4 * 1024 * 1024 / 32)) // 4M/64B = 64K blocks
#define MAP_CACHE_SIZE 32
#define BITMAP_SIZE_MAX ((uint32_t)(16 * 1024 * 1024 / 32)) // bitmap manages at most `BITMAP_SIZE_DEFAULT` blocks

typedef struct _bitmap_head
{
    // the size of bitmap
    uint32_t size;
    // the first available bit's index
    uint32_t first_avail;
    // if full is 1, there is no available bit in the bitmap
    int32_t full;
    // just for align(8)
    int32_t nonsense;
} bitmap_head;

struct _bitmap
{
    bitmap_head bitmap_head;
    // the size of map
    uint32_t map_size;
    int32_t nonsense;
    // map[map_size]
    int32_t map[];
};

typedef __mram_ptr struct _bitmap *bitmap_dpu;

typedef struct _bitmap_cache
{
    bitmap_head bitmap_head;
    // map_cache[0, MAP_CACHE_SIZE-1] caches bitmap_dpu->map[map_offset, map_offset + MAP_CACHE_SIZE]
    int32_t map_offset;
    // if modified is 1, need to write back map_cache[MAP_CACHE_SIZE] before caching the next part of bitmap_dpu->map
    int32_t modified;
    bitmap_dpu bitmap;
    uint32_t map_cache_size;
    // cache a part of bitmap_dpu->map
    int32_t map_cache[MAP_CACHE_SIZE];
} bitmap_cache;

// initialize bitmap_dpu
int bitmap_dpu_init(bitmap_dpu bpd, uint32_t size);
// obtain the total storage size(metadata+array) of a bitmap with `bitmap_size` bits
int bitmap_dpu_size(uint32_t bitmap_size);
// initialize bitmap_cache
int bitmap_cache_init(bitmap_cache *cache, bitmap_dpu bpd);
// set the i-th bit of the bitmap to 1
int bitmap_cache_set(bitmap_cache *cache, uint32_t i);
// set the i-th bit of the bitmap to 0
int bitmap_cache_clear(bitmap_cache *cache, uint32_t i);
// write back bitmap_head and map_cache[MAP_CACHE_SIZE]
int bitmap_cache_save(bitmap_cache *cache);

void bitmap_cache_dumpdata(bitmap_cache *cache);
void bitmap_dpu_dumpdata(bitmap_dpu bpd, uint32_t offset, uint32_t size);

#endif /* _BITMAP_H */