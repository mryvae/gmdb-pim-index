#include "primary_index_dpu.h"
#include "global_mutex.h"
#include "global_var.h"

static inline _wram_memcpy(char *dest, char *src, uint32_t len)
{
    for (int i = 0; i < len; i++)
    {
        dest[i] = src[i];
    }
    dest[len] = '\0';
}

static inline uint32_t _hash_function(char *buf, uint32_t len)
{
    uint32_t hash = 3917;

    while (len--)
        hash = ((hash << 5) + hash) + (*buf++); /* hash * 33 + c */
    return hash;
}

static inline int _key_compare(char *key1, uint32_t key1_len, char *key2, uint32_t key2_len)
{
    if (key1_len != key2_len)
    {
        return 0;
    }
    for (int i = 0; i < key1_len; i++)
    {
        if (key1[i] != key2[i])
        {
            return 0;
        }
    }
    return 1;
}

void primary_index_dpu_allocator_init(__mram_ptr void *start, size_t size)
{
    if (!global_index_mram_allocator_initial_flag)
    {
        linear_mram_allocator_initial(&global_index_mram_allocator, start, size,
                                      linear_allocator_mutex_lock, linear_allocator_mutex_unlock);
        global_index_mram_allocator_initial_flag = 1;
    }
}

void primary_index_dpu_space_init(__mram_ptr void *start, size_t size)
{
    if (!global_primary_index_space_initial_flag)
    {
        global_primary_index_space_addr = start;
        global_primary_index_space_size = size;
        global_primary_index_space_initial_flag = 1;
    }
}

primary_index_dpu *primary_index_dpu_create(PRIMARY_INDEX_ID index_id)
{
    if (global_num_pre_load_primary_index >= PRIMARY_INDEX_MAX_NUM)
    {
        return NULL;
    }
    primary_index_dpu *pid = &(global_prestored_primary_index[global_num_pre_load_primary_index]);
    pid->index_id = index_id;
    pid->buckets_size = next_power(global_primary_index_space_size / PRIMARY_INDEX_MAX_NUM / sizeof(primary_index_entry) + 1) / 2;
    pid->sizemask = pid->buckets_size - 1;
    pid->buckets = (__mram_ptr primary_index_entry *)(global_primary_index_space_addr + global_num_pre_load_primary_index * global_primary_index_space_size / PRIMARY_INDEX_MAX_NUM);
    pid->used = 0;
    global_num_pre_load_primary_index++;
    return pid;
}

void primary_index_dpu_init_buckets(primary_index_dpu *pid, uint32_t tasklet_id)
{
    if (!pid)
    {
        return;
    }
    int start = BLOCK_LOW(tasklet_id, NR_SLAVE_TASKLETS, pid->buckets_size);
    int end = BLOCK_HIGH(tasklet_id, NR_SLAVE_TASKLETS, pid->buckets_size);

    __dma_aligned primary_index_entry primary_index_entry_init = {.val = 0, .key_len = 0, .next = NULL};
    int i;
    for (i = start; i <= end; i++)
    {
        mram_write((void *)&primary_index_entry_init, (__mram_ptr void *)(pid->buckets + i), sizeof(primary_index_entry));
    }
}

primary_index_dpu *primary_index_dpu_check(uint32_t index_id)
{
    primary_index_dpu *index = NULL;
    for (int j = 0; j < global_num_pre_load_primary_index; j++)
    {
        if (index_id == global_prestored_primary_index[j].index_id)
        {
            index = &(global_prestored_primary_index[j]);
            break;
        }
    }
    return index;
}

int primary_index_dpu_insert(primary_index_dpu *pid, char *key, uint32_t key_len, uint64_t val)
{
    int32_t bucket_id = _hash_function(key, key_len) & pid->sizemask, flag = 0;
    // printf("bucket_matrix_slice_dpu_insert_elem: row: %d, col: %d, bucket_id: %d\n", elem->row, elem->col, bucket_id);
    // printf("bucket_matrix_slice_dpu_insert_elem: bmsd->sizemask: %d, bmsd->size: %d\n", bmsd->sizemask, bmsd->buckets_size);
    __dma_aligned primary_index_entry entry_buffer;
    __mram_ptr primary_index_entry *cur = pid->buckets + bucket_id;
    buckets_mutex_lock(bucket_id);
    mram_read((__mram_ptr void *)(pid->buckets + bucket_id), (void *)&entry_buffer, sizeof(primary_index_entry));
    if (entry_buffer.key_len == 0)
    {
        _wram_memcpy(entry_buffer.key, key, key_len);
        entry_buffer.key_len = key_len;
        entry_buffer.val = val;
        entry_buffer.next = NULL;
        mram_write((void *)&entry_buffer, (__mram_ptr void *)(cur), sizeof(primary_index_entry));
    }
    else
    {
        __mram_ptr primary_index_entry *entry_buffer_next = entry_buffer.next;
        entry_buffer.next = (__mram_ptr primary_index_entry *)linear_mram_alloc(&global_index_mram_allocator, sizeof(primary_index_entry));

        mram_write((void *)&entry_buffer, (__mram_ptr void *)(cur), sizeof(primary_index_entry));
        cur = entry_buffer.next;

        _wram_memcpy(entry_buffer.key, key, key_len);
        entry_buffer.key_len = key_len;
        entry_buffer.val = val;
        entry_buffer.next = entry_buffer_next;
        mram_write((void *)&entry_buffer, (__mram_ptr void *)(cur), sizeof(primary_index_entry));
    }
    pid->used++;
    buckets_mutex_unlock(bucket_id);
    return PRIMARY_INDEX_OK;
}

int primary_index_dpu_delete(primary_index_dpu *pid, char *key, uint32_t key_len)
{
    int32_t bucket_id = _hash_function(key, key_len) & pid->sizemask, flag = 0;
    __dma_aligned primary_index_entry entry_buffer;
    __mram_ptr primary_index_entry *last_ptr = pid->buckets + bucket_id;
    primary_index_entry last;
    buckets_mutex_lock(bucket_id);
    mram_read((__mram_ptr void *)(pid->buckets + bucket_id), (void *)&entry_buffer, sizeof(primary_index_entry));

    if (_key_compare(entry_buffer.key, entry_buffer.key_len, key, key_len))
    {
        entry_buffer.key_len = 0;
        flag = 1;
        if (entry_buffer.next)
        {
            mram_read((__mram_ptr void *)(entry_buffer.next), (void *)&entry_buffer, sizeof(primary_index_entry));
        }

        mram_write((void *)&entry_buffer, (__mram_ptr void *)(pid->buckets + bucket_id), sizeof(primary_index_entry));
    }
    else
    {
        while (entry_buffer.next)
        {
            last = entry_buffer;
            mram_read((__mram_ptr void *)(entry_buffer.next), (void *)&entry_buffer, sizeof(primary_index_entry));
            if (_key_compare(entry_buffer.key, entry_buffer.key_len, key, key_len))
            {
                linear_mram_free(&global_index_mram_allocator, last.next);
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

__mram_ptr primary_index_entry *primary_index_dpu_lookup(const primary_index_dpu *pid, char *key, uint32_t key_len)
{
    int32_t bucket_id = _hash_function(key, key_len) & pid->sizemask, flag = 0;
    // printf("bucket_id: %d\n", bucket_id);
    __dma_aligned primary_index_entry entry_buffer;
    __mram_ptr primary_index_entry *last = pid->buckets + bucket_id, *res = NULL;
    buckets_mutex_lock(bucket_id);
    mram_read((__mram_ptr void *)(pid->buckets + bucket_id), (void *)&entry_buffer, sizeof(primary_index_entry));
    while (!_key_compare(entry_buffer.key, entry_buffer.key_len, key, key_len))
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
    if (_key_compare(entry_buffer.key, entry_buffer.key_len, key, key_len))
    {
        res = last;
    }
    buckets_mutex_unlock(bucket_id);
    return res;
}

void primary_index_dpu_update_with_entry_addr(const primary_index_dpu *pid, __mram_ptr primary_index_entry *entry, metadata new_val_meta, uint64_t new_val)
{
    __dma_aligned primary_index_entry entry_buffer;
    mram_read((__mram_ptr void *)entry, (void *)&entry_buffer, sizeof(primary_index_entry));
    int32_t bucket_id = _hash_function(entry_buffer.key, entry_buffer.key_len) & pid->sizemask;
    buckets_mutex_lock(bucket_id);
    entry_buffer.val = new_val;
    entry_buffer.val_meta = new_val_meta;
    mram_write((void *)&entry_buffer, (__mram_ptr void *)entry, sizeof(primary_index_entry));
    buckets_mutex_unlock(bucket_id);
}

__mram_ptr primary_index_entry *primary_index_dpu_get_or_insert(primary_index_dpu *pid, char *key, uint32_t key_len, metadata val_meta, uint64_t val)
{
    int32_t bucket_id = _hash_function(key, key_len) & pid->sizemask;
    // printf("bucket_id: %d\n", bucket_id);
    __dma_aligned primary_index_entry entry_buffer;
    __mram_ptr primary_index_entry *last = pid->buckets + bucket_id, *res = NULL;
    buckets_mutex_lock(bucket_id);
    mram_read((__mram_ptr void *)(pid->buckets + bucket_id), (void *)&entry_buffer, sizeof(primary_index_entry));
    while (!_key_compare(entry_buffer.key, entry_buffer.key_len, key, key_len))
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

    if (_key_compare(entry_buffer.key, entry_buffer.key_len, key, key_len))
    {
        res = last;
    }
    else
    {
        // insert (key ,val)
        __mram_ptr primary_index_entry *cur = pid->buckets + bucket_id;
        mram_read((__mram_ptr void *)(pid->buckets + bucket_id), (void *)&entry_buffer, sizeof(primary_index_entry));
        if (entry_buffer.key_len == 0)
        {
            _wram_memcpy(entry_buffer.key, key, key_len);
            entry_buffer.key_len = key_len;
            entry_buffer.val = val;
            entry_buffer.val_meta = val_meta;
            entry_buffer.next = NULL;
            mram_write((void *)&entry_buffer, (__mram_ptr void *)(cur), sizeof(primary_index_entry));
        }
        else
        {
            __mram_ptr primary_index_entry *entry_buffer_next = entry_buffer.next;
            entry_buffer.next = (__mram_ptr primary_index_entry *)linear_mram_alloc(&global_index_mram_allocator, sizeof(primary_index_entry));

            mram_write((void *)&entry_buffer, (__mram_ptr void *)(cur), sizeof(primary_index_entry));
            cur = entry_buffer.next;

            _wram_memcpy(entry_buffer.key, key, key_len);
            entry_buffer.key_len = key_len;
            entry_buffer.val = val;
            entry_buffer.val_meta = val_meta;
            entry_buffer.next = entry_buffer_next;
            mram_write((void *)&entry_buffer, (__mram_ptr void *)(cur), sizeof(primary_index_entry));
        }
        pid->used++;
    }
    buckets_mutex_unlock(bucket_id);
    return res;
}

static void primary_index_entry_dump(__mram_ptr primary_index_entry *entry)
{
    __dma_aligned primary_index_entry entry_buffer;
    mram_read((__mram_ptr void *)entry, (void *)&entry_buffer, sizeof(primary_index_entry));
    printf("key: %s, type: %u, id: %u, val: %ld\n", entry_buffer.key, entry_buffer.val_meta.type,
           entry_buffer.val_meta.id, entry_buffer.val);
}

void primary_index_dpu_test(primary_index_dpu *pid)
{
    int key_len = 3;
    char key1[16] = "111";
    metadata val_meta = {2, 110};
    metadata new_val_meta = {2, 100};
    uint64_t val1 = 1;
    char key2[16] = "222";
    uint64_t val2 = 2;
    char key3[16] = "333";
    uint64_t val3 = 3;
    char key4[16] = "444";
    uint64_t val4 = 4;
    char key_empty[16] = "555";
    __mram_ptr primary_index_entry *entry;

    primary_index_dpu_get_or_insert(pid, key1, key_len, val_meta, val1);
    primary_index_dpu_get_or_insert(pid, key2, key_len, val_meta, val2);
    primary_index_dpu_get_or_insert(pid, key3, key_len, val_meta, val3);
    primary_index_dpu_get_or_insert(pid, key4, key_len, val_meta, val4);

    entry = primary_index_dpu_get_or_insert(pid, key2, key_len, val_meta, 0);
    if (entry)
    {
        primary_index_entry_dump(entry);
    }
    entry = primary_index_dpu_get_or_insert(pid, key3, key_len, val_meta, 0);
    if (entry)
    {
        primary_index_entry_dump(entry);
    }
    primary_index_dpu_update_with_entry_addr(pid, entry, new_val_meta, 0);
    entry = primary_index_dpu_get_or_insert(pid, key3, key_len, val_meta, 0);
    if (entry)
    {
        primary_index_entry_dump(entry);
    }
    entry = primary_index_dpu_get_or_insert(pid, key2, key_len, val_meta, 0);
    if (entry)
    {
        primary_index_entry_dump(entry);
    }
    primary_index_dpu_update_with_entry_addr(pid, entry, new_val_meta, 0);
    entry = primary_index_dpu_get_or_insert(pid, key2, key_len, val_meta, 0);
    if (entry)
    {
        primary_index_entry_dump(entry);
    }
}