#include "dpu_set_context.h"

static DPU_SET_ID dpu_set_allocated_num = 0;

dpu_set_context *dpu_set_context_create()
{
    dpu_set_context *set;
    set = malloc(sizeof(dpu_set_context));
    if (!set)
    {
        return NULL;
    }
    set->interface = newDirectPIMInterface();
    allocate(set->interface, 1, DPU_BINARY);
    set->dpu_set_id = dpu_set_allocated_num;
    set->nr_pre_load_index = 0;
    dpu_set_allocated_num++;
    pthread_mutex_init(&(set->lock), NULL);

    // DPU_ASSERT(dpu_alloc(NR_DPUS, NULL, &(set->dpu_set)));
    // DPU_ASSERT(dpu_load(set->dpu_set, DPU_BINARY, NULL));
    set->nr_dpus = NR_DPUS;
    set->nr_slave_tasklets = NR_SLAVE_TASKLETS;
    dpu_push_info_init(&(set->push_info), NR_DPUS);
    dpu_pull_info_init(&(set->pull_info), NR_DPUS);
    dpu_timer_init(&(set->timer));

    return set;
}

void dpu_set_context_free(dpu_set_context *set)
{
    if (!set)
    {
        return;
    }

    deallocate(set->interface);
    // DPU_ASSERT(dpu_free(set->dpu_set));
    dpu_push_info_free(&(set->push_info));
    dpu_pull_info_free(&(set->pull_info));
    free(set);
}

void dpu_set_context_dpu_run(dpu_set_context *set)
{
    Launch(set->interface, 0);
    // DPU_ASSERT(dpu_launch(set->dpu_set, DPU_SYNCHRONOUS));
    // dpu_set_context_log_read(set);
}

void dpu_set_context_log_read(dpu_set_context *set)
{
    PrintLog(set->interface);
    // struct dpu_set_t dpu;
    // DPU_FOREACH(set->dpu_set, dpu)
    // {
    //     DPU_ASSERT(dpu_log_read(dpu, stdout));
    // }
}