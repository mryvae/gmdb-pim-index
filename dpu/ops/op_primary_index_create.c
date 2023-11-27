#include "op_primary_index_create.h"

void primary_index_create_prepare()
{
    global_primary_index = primary_index_dpu_create(primary_index_id, PRIMARY_INDEX_BUCKETS_INITIAL_SIZE);
}

void primary_index_create_tasklets_run()
{
    uint32_t tasklet_id = me();
    if (tasklet_id == 0)
        return;

    primary_index_dpu_init_buckets(global_primary_index, tasklet_id - 1);
}

void primary_index_create_merge()
{
    // nothing
    // primary_index_dpu_test(global_primary_index, &global_index_mram_allocator);
    // printf("DPU_MRAM_HEAP_POINTER: %p\n", DPU_MRAM_HEAP_POINTER);
}