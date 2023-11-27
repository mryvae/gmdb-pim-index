#include "block_mram_alloc.h"
#include "../../util/util.h"

void block_mram_allocator_init(block_mram_allocator *allocator, __mram_ptr void *start, uint32_t mram_size,
                               uint32_t block_size, void (*mutex_lock)(), void (*mutex_unlock)())
{
    allocator->block_size = next_power(block_size);
    uint32_t bitmap_size = bitmap_dpu_size(mram_size / allocator->block_size);
    // printf("space for bitmap: %u bytes\n", bitmap_size);
    if (mram_size < bitmap_size)
    {
        // printf("error: block_mram_allocator_init\n");
        return;
    }
    allocator->start = (__mram_ptr void *)(start + align8(bitmap_size));
    allocator->size = ((mram_size - align8(bitmap_size)) / allocator->block_size);
    allocator->avail = allocator->size;
    allocator->mutex_lock = mutex_lock;
    allocator->mutex_unlock = mutex_unlock;

    allocator->bpd = (bitmap_dpu)(start);
    bitmap_dpu_init(allocator->bpd, allocator->size);
    bitmap_cache_init(&(allocator->cache), allocator->bpd);
}

void block_mram_allocator_save(block_mram_allocator *allocator)
{
    bitmap_cache_save(&(allocator->cache));
}

__mram_ptr void *block_mram_allocator_alloc(block_mram_allocator *allocator)
{
    allocator->mutex_lock();
    if (allocator->avail <= 0)
    {
        allocator->mutex_unlock();
        return NULL;
    }
    block_id avail = allocator->cache.bitmap_head.first_avail;
    bitmap_cache_set(&(allocator->cache), avail);
    allocator->avail--;
    allocator->mutex_unlock();
    return block_mram_allocator_block_addr(allocator, avail);
}

void block_mram_allocator_free(block_mram_allocator *allocator, __mram_ptr void *addr)
{
    allocator->mutex_lock();
    block_id id = block_mram_allocator_block_id(allocator, addr);
    block_mram_allocator_block_free(allocator, id);
    allocator->mutex_unlock();
}

void block_mram_allocator_block_free(block_mram_allocator *allocator, block_id id)
{
    if (id >= 0)
    {
        if (bitmap_cache_clear(&(allocator->cache), id) == BITMAP_OK)
        {
            allocator->avail++;
        }
    }
}

__mram_ptr void *block_mram_allocator_block_addr(block_mram_allocator *allocator, block_id id)
{
    if (id < 0 || id >= allocator->size)
        return (__mram_ptr void *)NULL;

    return (__mram_ptr void *)(allocator->start + id * allocator->block_size);
}

block_id block_mram_allocator_block_id(block_mram_allocator *allocator, __mram_ptr void *addr)
{
    return (addr - allocator->start) / allocator->block_size;
}

void block_mram_allocator_dump(block_mram_allocator *allocator, int dump_cache_data)
{
    printf("\n");
    printf("[ALLOCATOR] start_addr: %p , #blocks: %u , #available blocks: %u\n",
           allocator->start, allocator->size, allocator->avail);
    printf("[BITMAP]  #bits: %u, first_avail: %u, full: %u\n", allocator->bpd->bitmap_head.size,
           allocator->bpd->bitmap_head.first_avail, allocator->bpd->bitmap_head.full);
    printf("[BITMAP]  map_size: %u, map_addr: %p\n", allocator->bpd->map_size, allocator->bpd->map);
    printf("[BMCACHE] #bits: %u, first_avail: %u, full: %u\n", allocator->cache.bitmap_head.size,
           allocator->cache.bitmap_head.first_avail, allocator->cache.bitmap_head.full);
    printf("[BMCACHE] offset: %u, modified: %u, bitmap_addr: %p\n", allocator->cache.map_offset,
           allocator->cache.modified, allocator->cache.bitmap);
    if (dump_cache_data)
        bitmap_cache_dumpdata(&allocator->cache);
    printf("\n");
}