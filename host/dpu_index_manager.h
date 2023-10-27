#ifndef __PRE_LOAD_INDEX_H
#define __PRE_LOAD_INDEX_H

#include "dpu_set_context.h"
#include "dpu_set_pool.h"
#include "../util/param.h"
#define MAX_NUM_PRE_LOAD_PRIMARY_INDEX 64

typedef struct _dpu_index_manager
{
    pthread_mutex_t lock;
    volatile global_nr_pre_load_index;
    dpu_set_context *map[MAX_NUM_PRE_LOAD_PRIMARY_INDEX];
} dpu_index_manager;

void dpu_index_manager_init();
PRIMARY_INDEX_ID dpu_index_manager_create_index();
dpu_set_context *dpu_index_manager_get_dpu_set(PRIMARY_INDEX_ID id);

#endif