#include "dpu_set_pool.h"
#include <stdio.h>
#include <pthread.h>
typedef struct dpu_set_pool_
{
    dpu_set_context **dpu_set_info;
    int num_dpu_set;
} dpu_set_pool_;

typedef struct dpu_set_pool_ *dpu_set_pool;

static dpu_set_pool _dpu_set_pool = NULL; // dpu_set_pool

/* Initialise dpu_set_pool */
int dpu_set_pool_init(int num_dpu_set)
{

    if (num_dpu_set < 0)
    {
        num_dpu_set = 0;
    }

    /* Make new dpu_set_pool */
    _dpu_set_pool = malloc(sizeof(dpu_set_pool_));
    if (_dpu_set_pool == NULL)
    {
        fprintf(stderr, "dpu_set_pool_init(): Could not allocate memory for dpu_set_pool\n");
        return 0;
    }

    _dpu_set_pool->dpu_set_info = (dpu_set_context **)malloc(num_dpu_set * sizeof(dpu_set_context *));
    if (_dpu_set_pool->dpu_set_info == NULL)
    {
        fprintf(stderr, "dpu_set_pool_init(): Could not allocate memory for dpu_set_context\n");
        return 0;
    }
    _dpu_set_pool->num_dpu_set = num_dpu_set;

    /* dpu_set_context init */
    int i;
    for (i = 0; i < num_dpu_set; i++)
    {
        _dpu_set_pool->dpu_set_info[i] = dpu_set_context_create();
    }
    return num_dpu_set;
}

dpu_set_context *dpu_set_apply()
{
    // 申请一个dpu_set 资源
    dpu_set_context *dpu_set_avail = NULL;
    for (int i = 0; i < _dpu_set_pool->num_dpu_set; i++)
    {
        pthread_mutex_lock(&(_dpu_set_pool->dpu_set_info[i]->lock));
        if (_dpu_set_pool->dpu_set_info[i]->nr_pre_load_index < MAX_NUM_PRE_LOAD_PRIMARY_INDEX_PER_DPU_SET)
        {
            dpu_set_avail = _dpu_set_pool->dpu_set_info[i];
            dpu_set_avail->nr_pre_load_index++;
        }
        pthread_mutex_unlock(&(_dpu_set_pool->dpu_set_info[i]->lock));
        if (dpu_set_avail)
        {
            break;
        }
    }
    return dpu_set_avail;
}

void dpu_set_pool_destroy()
{
    for (int i = 0; i < _dpu_set_pool->num_dpu_set; i++)
    {
        dpu_set_context_free(_dpu_set_pool->dpu_set_info[i]);
    }
    free(_dpu_set_pool);
}