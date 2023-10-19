#include "primary_index_dpu.h"
#include "global_mutex.h"

static inline uint32_t hash_function(uint32_t key)
{
    key += ~(key << 15);
    key ^= (key >> 10);
    key += (key << 3);
    key ^= (key >> 6);
    key += ~(key << 11);
    key ^= (key >> 16);
    return key;
}

void primary_index_dpu_init(primary_index_dpu *pid, __mram_ptr primary_index_entry *buckets_addr, uint32_t buckets_size)
{
    pid->buckets_size = next_power(buckets_size);
    pid->sizemask = pid->buckets_size - 1;
    pid->buckets = buckets_addr;
    pid->used = 0;
}

void primary_index_dpu_init_buckets(primary_index_dpu *pid, uint32_t tasklet_id)
{
    int start = BLOCK_LOW(tasklet_id, NR_SLAVE_TASKLETS, pid->buckets_size);
    int end = BLOCK_HIGH(tasklet_id, NR_SLAVE_TASKLETS, pid->buckets_size);

    __dma_aligned primary_index_entry primary_index_entry_init = {.key = -1, .val = 0, .next = NULL};
    int i;
    for (i = start; i <= end; i++)
    {
        mram_write((void *)&primary_index_entry_init, (__mram_ptr void *)(pid->buckets + i), sizeof(primary_index_entry));
    }
}

int primary_index_dpu_insert(primary_index_dpu *pid, uint32_t key, uint64_t val, block_mram_allocator *allocator)
{
    int32_t bucket_id = hash_function(key) & pid->sizemask, flag = 0;
    // printf("bucket_matrix_slice_dpu_insert_elem: row: %d, col: %d, bucket_id: %d\n", elem->row, elem->col, bucket_id);
    // printf("bucket_matrix_slice_dpu_insert_elem: bmsd->sizemask: %d, bmsd->size: %d\n", bmsd->sizemask, bmsd->buckets_size);
    __dma_aligned primary_index_entry entry_buffer;
    __mram_ptr primary_index_entry *cur = pid->buckets + bucket_id;
    buckets_mutex_lock(bucket_id);
    mram_read((__mram_ptr void *)(pid->buckets + bucket_id), (void *)&entry_buffer, sizeof(primary_index_entry));
    if (entry_buffer.key < 0)
    {
        entry_buffer.key = key;
        entry_buffer.val = val;
        entry_buffer.next = NULL;
        mram_write((void *)&entry_buffer, (__mram_ptr void *)(cur), sizeof(primary_index_entry));
    }
    else
    {
        __mram_ptr primary_index_entry *entry_buffer_next = entry_buffer.next;
        // mutex_lock(col_index_entry_allocator_mutex);
        entry_buffer.next = (__mram_ptr primary_index_entry *)block_mram_allocator_alloc(allocator);
        // mutex_unlock(col_index_entry_allocator_mutex);

        mram_write((void *)&entry_buffer, (__mram_ptr void *)(cur), sizeof(primary_index_entry));
        cur = entry_buffer.next;

        entry_buffer.key = key;
        entry_buffer.val = val;
        entry_buffer.next = entry_buffer_next;
        mram_write((void *)&entry_buffer, (__mram_ptr void *)(cur), sizeof(primary_index_entry));
    }
    pid->used++;
    buckets_mutex_unlock(bucket_id);
    return PRIMARY_INDEX_OK;
}

int primary_index_dpu_delete(primary_index_dpu *pid, uint32_t key, block_mram_allocator *allocator)
{
    int32_t bucket_id = hash_function(key) & pid->sizemask, flag = 0;
    __dma_aligned primary_index_entry entry_buffer;
    __mram_ptr primary_index_entry *last_ptr = pid->buckets + bucket_id;
    primary_index_entry last;
    buckets_mutex_lock(bucket_id);
    mram_read((__mram_ptr void *)(pid->buckets + bucket_id), (void *)&entry_buffer, sizeof(primary_index_entry));

    if (entry_buffer.key == key)
    {
        entry_buffer.key = -1;
        flag = 1;
        if (entry_buffer.next)
        {
            mram_read((__mram_ptr void *)(entry_buffer.next), (void *)&entry_buffer, sizeof(primary_index_entry));
        }

        mram_write((void *)&entry_buffer, (__mram_ptr void *)(pid->buckets + bucket_id), sizeof(primary_index_entry));
    }
    else if (entry_buffer.key >= 0)
    {
        while (entry_buffer.next)
        {
            last = entry_buffer;
            mram_read((__mram_ptr void *)(entry_buffer.next), (void *)&entry_buffer, sizeof(primary_index_entry));
            if (entry_buffer.key == key)
            {
                // mutex_lock(col_index_entry_allocator_mutex);
                block_mram_allocator_free(allocator, last.next);
                // mutex_unlock(col_index_entry_allocator_mutex);
                last.next = entry_buffer.next;
                mram_write((void *)&last, (__mram_ptr void *)last_ptr, sizeof(primary_index_entry));
                flag = 1;
                break;
            }
            last_ptr = last.next;
        }
    }
    if (flag)
    {
        pid->used--;
    }
    buckets_mutex_unlock(bucket_id);
    return flag ? PRIMARY_INDEX_OK : PRIMARY_INDEX_ERR;
}

__mram_ptr primary_index_entry *primary_index_dpu_lookup(const primary_index_dpu *pid, uint32_t key)
{
    int32_t bucket_id = hash_function(key) & pid->sizemask, flag = 0;
    __dma_aligned primary_index_entry entry_buffer;
    __mram_ptr primary_index_entry *last = pid->buckets + bucket_id, *res = NULL;
    buckets_mutex_lock(bucket_id);
    mram_read((__mram_ptr void *)(pid->buckets + bucket_id), (void *)&entry_buffer, sizeof(primary_index_entry));
    while (entry_buffer.key != key)
    {
        // printf("entry_buffer.row: %d\n", entry_buffer.row);
        last = entry_buffer.next;
        if (entry_buffer.next)
        {
            mram_read((__mram_ptr void *)(entry_buffer.next), (void *)&entry_buffer, sizeof(primary_index_entry));
        }
        else
        {
            break;
        }
    }
    if (entry_buffer.key == key)
    {
        res = last;
    }
    buckets_mutex_unlock(bucket_id);
    return res;
}

void primary_index_entry_dump(primary_index_entry *entry_buffer)
{
    printf("key: %d, val: %ld\n", entry_buffer->key, entry_buffer->val);
}