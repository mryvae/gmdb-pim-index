#ifndef _BUCKETS_MUTEX_H
#define _BUCKETS_MUTEX_H

#include "mram_memory.h"
#include <mutex_pool.h>

void buckets_mutex_lock(uint32_t bucket_id);
void buckets_mutex_unlock(uint32_t bucket_id);

void allocator_mutex_32_lock();
void allocator_mutex_32_unlock();
void allocator_mutex_16_lock();
void allocator_mutex_16_unlock();
void result_cache_mutex_lock();
void result_cache_mutex_unlock();
void inter_cache_mutex_lock();
void inter_cache_mutex_unlock();

#endif