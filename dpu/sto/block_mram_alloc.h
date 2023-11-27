#ifndef _BLOCK_MRAM_ALLOC_H
#define _BLOCK_MRAM_ALLOC_H
#include <stdint.h>
#include <mram.h>
#include "bitmap.h"
typedef int32_t block_id;

typedef struct _block_mram_allocator
{
    bitmap_cache cache;
    bitmap_dpu bpd;
    __mram_ptr void *start;
    uint32_t size;
    uint32_t avail;
    uint32_t block_size;
    void (*mutex_lock)();
    void (*mutex_unlock)();
} block_mram_allocator;

// initialize the block_mram_allocator
void block_mram_allocator_init(block_mram_allocator *allocator, __mram_ptr void *start, uint32_t mram_size,
                               uint32_t block_size, void (*mutex_lock)(), void (*mutex_unlock)());
// save bitmap_cache
void block_mram_allocator_save(block_mram_allocator *allocator);
// allocate a new block
__mram_ptr void *block_mram_allocator_alloc(block_mram_allocator *allocator);
// free a block
void block_mram_allocator_free(block_mram_allocator *allocator, __mram_ptr void *addr);

void block_mram_allocator_block_free(block_mram_allocator *allocator, block_id id);

// obtain the address of a block
inline __mram_ptr void *block_mram_allocator_block_addr(block_mram_allocator *allocator, block_id id);

inline block_id block_mram_allocator_block_id(block_mram_allocator *allocator, __mram_ptr void *addr);

void block_mram_allocator_test(block_mram_allocator *allocator);

void block_mram_allocator_dump(block_mram_allocator *allocator, int dump_cache_data);

#endif