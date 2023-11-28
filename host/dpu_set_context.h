#ifndef _DPU_SET_CONTEXT_H
#define _DPU_SET_CONTEXT_H
#include <dpu.h>
#include <assert.h>
#include <stdint.h>
#include <pthread.h>
#include "dpu_info.h"
#include "dpu_push.h"
#include "dpu_pull.h"
#include "dpu_timer.h"
#include "../upmemInterface/upmem_direct_c.h"

#define DEFAULT_COO_RESULTS_CAPACITY 1024
#define MAX_NUM_PRE_LOAD_PRIMARY_INDEX_PER_DPU_SET 16

#define AVAILABLE 1
#define UNAVAILABLE 0

typedef int DPU_SET_ID;

typedef struct _dpu_set_context
{
    pthread_mutex_t lock;
    volatile int nr_pre_load_index;
    DPU_SET_ID dpu_set_id;
    struct dpu_set_t dpu_set;
    // XDPI interface;
    uint32_t nr_dpus;
    uint32_t nr_slave_tasklets;
    dpu_push_info push_info;
    dpu_pull_info pull_info;
    dpu_timer timer;
} dpu_set_context;

// dpu_set_context
dpu_set_context *dpu_set_context_create(void);
void dpu_set_context_free(dpu_set_context *set);
void dpu_set_context_dpu_run(dpu_set_context *set);
void dpu_set_context_log_read(dpu_set_context *set);

#endif