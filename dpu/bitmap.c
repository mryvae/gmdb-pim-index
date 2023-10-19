#include "bitmap.h"
#include "../util/util.h"

#define BITMAP_INIT_BUFFER_SIZE 16
#define INT_BITS 32
#define MASK 0x1F

int bitmap_dpu_size(uint32_t bitmap_size)
{
    uint32_t size = sizeof(struct _bitmap);
    // printf("bitmap metadata size: %u\n", size);
    uint32_t map_size = bitmap_size / INT_BITS;
    if (map_size % INT_BITS)
    {
        map_size++;
    }
    // printf("bitmap map size: %u\n", map_size);

    size += sizeof(int32_t) * map_size;
    return size;
}

int bitmap_dpu_init(bitmap_dpu bpd, uint32_t size)
{
    // size: #blocks == #bits in the bitmap
    if (size > BITMAP_SIZE_MAX)
    {
        printf("error: bitmap_dpu_init size exceeds\n");
        return BITMAP_ERR;
    }

    __dma_aligned bitmap_head bitmap_head_buffer;
    bitmap_head_buffer.first_avail = 0;
    bitmap_head_buffer.full = 0;
    bitmap_head_buffer.size = size;
    mram_write(&bitmap_head_buffer, bpd, sizeof(bitmap_head));

    uint32_t map_size = size / INT_BITS;
    if (size % INT_BITS)
    {
        map_size++;
    }
    bpd->map_size = map_size;
    __dma_aligned int32_t bitmap_init_buffer[BITMAP_INIT_BUFFER_SIZE] = {0};
    uint32_t initialized = 0;
    __mram_ptr int32_t *map = bpd->map;
    // printf("mram bitmap->map addr: %p (align 8?)\n", map);
    while (initialized < map_size)
    {
        mram_write(&bitmap_init_buffer, &(map[initialized]), sizeof(int32_t) * BITMAP_INIT_BUFFER_SIZE);
        initialized += BITMAP_INIT_BUFFER_SIZE;
    }

    return BITMAP_OK;
}

int bitmap_cache_init(bitmap_cache *cache, bitmap_dpu bpd)
{
    mram_read(&(bpd->bitmap_head), &(cache->bitmap_head), sizeof(bitmap_head));
    cache->map_offset = 0;
    cache->modified = 0;
    cache->bitmap = bpd;
    cache->map_cache_size = MAP_CACHE_SIZE;
    mram_read(&(bpd->map[cache->map_offset]), cache->map_cache, MAP_CACHE_SIZE * sizeof(int32_t));
    return BITMAP_OK;
}

/*
    obtain the target index in map_cache[MAP_CACHE_SIZE]
    if necessary, cache the corresponding part of bitmap_dpu->map
*/
static inline int bitmap_cache_hit(bitmap_cache *cache, uint32_t i)
{
    uint32_t index = i >> 5;
    if (index < cache->map_offset || index >= cache->map_offset + cache->map_cache_size)
    {
        if (cache->modified)
        {
            mram_write(cache->map_cache, &(cache->bitmap->map[cache->map_offset]), MAP_CACHE_SIZE * sizeof(int32_t));
            cache->modified = 0;
        }
        cache->map_offset = (index / MAP_CACHE_SIZE) * MAP_CACHE_SIZE;
        mram_read(&(cache->bitmap->map[cache->map_offset]), cache->map_cache, MAP_CACHE_SIZE * sizeof(int32_t));
    }
    return index - cache->map_offset;
}

static inline int bitmap_cache_test(bitmap_cache *cache, uint32_t i)
{
    uint32_t index = bitmap_cache_hit(cache, i);
    return cache->map_cache[index] & (1 << (i & MASK));
}

int bitmap_cache_set(bitmap_cache *cache, uint32_t i)
{
    if (i >= cache->bitmap_head.size)
        return BITMAP_ERR;

    uint32_t index = bitmap_cache_hit(cache, i);
    cache->map_cache[index] |= (1 << (i & MASK));
    cache->modified = 1;

    if (cache->bitmap_head.first_avail != i)
        return BITMAP_OK;

    cache->bitmap_head.first_avail = cache->bitmap_head.size;
    while (i < cache->bitmap_head.size)
    {
        if (bitmap_cache_test(cache, i) == 0)
        {
            cache->bitmap_head.first_avail = i;
            break;
        }
        i++;
    }
    if (cache->bitmap_head.first_avail == cache->bitmap_head.size)
    {
        cache->bitmap_head.full = 1;
    }
    return BITMAP_OK;
}

int bitmap_cache_clear(bitmap_cache *cache, uint32_t i)
{
    if (i >= cache->bitmap_head.size)
        return BITMAP_ERR;

    uint32_t index = bitmap_cache_hit(cache, i);
    
    if(!((cache->map_cache[index]) & (1 << (i & MASK))))
        return BITMAP_ERR; // not set, no need to clear

    cache->map_cache[index] &= ~(1 << (i & MASK));
    cache->modified = 1;
    cache->bitmap_head.full = 0;

    if (i < cache->bitmap_head.first_avail)
    {
        cache->bitmap_head.first_avail = i;
    }
    return BITMAP_OK;
}

int bitmap_cache_save(bitmap_cache *cache)
{
    mram_write(&(cache->bitmap_head), &(cache->bitmap->bitmap_head), sizeof(bitmap_head));
    mram_write(cache->map_cache, &(cache->bitmap->map[cache->map_offset]), MAP_CACHE_SIZE * sizeof(int32_t));
    cache->modified = 0;
    return BITMAP_OK;
}

void bitmap_dpu_dumpdata(bitmap_dpu bpd, uint32_t offset, uint32_t size)
{
    uint32_t row_num = 4;
    printf("[BITMAP] bitmap data dump, offset = %u\n", offset*INT_BITS);
    for(int i = 0; i < size; i+=row_num){
        if(size - i < 4) row_num = size - i;
        printf("   %4u - %4u ", i*INT_BITS, (i+row_num)*INT_BITS - 1);
        for(int j = 0; j < row_num; j++){
            printf("%08x ", bpd->map[offset+i+j]);
        }
        printf("\n");
    }
    printf("[BITMAP] bitmap data dump end\n");
}

void bitmap_cache_dumpdata(bitmap_cache *cache)
{
    uint32_t row_num = 4;
    printf("[BMCACHE] cache data dump\n");
    for(int i = 0; i < cache->map_cache_size; i+=row_num){
        if(cache->map_cache_size - i < 4) row_num = cache->map_cache_size - i;
        printf("   %4u - %4u ", i*INT_BITS, (i+row_num)*INT_BITS - 1);
        for(int j = 0; j < row_num; j++){
            printf("%08x ", cache->map_cache[i+j]);
        }
        printf("\n");
    }
    printf("[BMCACHE] cache data dump end\n");
}