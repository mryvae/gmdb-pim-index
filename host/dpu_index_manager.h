#ifndef __PRE_LOAD_INDEX_H
#define __PRE_LOAD_INDEX_H

#include "dpu_set_context.h"
#include "dpu_set_pool.h"
#include "../util/param.h"
#include "hash_map.h"
#define MAX_NUM_PRE_LOAD_PRIMARY_INDEX 64

#define DPU_INDEX_MANAGER_OK 0
#define DPU_INDEX_MANAGER_ERR 1

typedef struct _dpu_index_manager
{
    pthread_mutex_t lock;
    volatile int global_nr_pre_load_index;

    hash_map *dpu_set_map;
} dpu_index_manager;

void dpu_index_manager_init(void);
int dpu_index_manager_create_index(PRIMARY_INDEX_ID id);
dpu_set_context *dpu_index_manager_get_dpu_set(PRIMARY_INDEX_ID id);

#endif