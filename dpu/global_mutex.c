#include "global_mutex.h"

MUTEX_POOL_INIT(buckets_mutex_pool, 2 * NR_SLAVE_TASKLETS);

MUTEX_INIT(linear_allocator_mutex);

MUTEX_INIT(result_cache_mutex);

MUTEX_INIT(inter_cache_mutex);

void buckets_mutex_lock(uint32_t bucket_id)
{
    mutex_pool_lock(&buckets_mutex_pool, bucket_id);
}

void buckets_mutex_unlock(uint32_t bucket_id)
{
    mutex_pool_unlock(&buckets_mutex_pool, bucket_id);
}

void linear_allocator_mutex_lock()
{
    mutex_lock(linear_allocator_mutex);
}

void linear_allocator_mutex_unlock()
{
    mutex_unlock(linear_allocator_mutex);
}

void result_cache_mutex_lock()
{
    mutex_lock(result_cache_mutex);
}

void result_cache_mutex_unlock()
{
    mutex_unlock(result_cache_mutex);
}

void inter_cache_mutex_lock()
{
    mutex_lock(inter_cache_mutex);
}

void inter_cache_mutex_unlock()
{
    mutex_unlock(inter_cache_mutex);
}