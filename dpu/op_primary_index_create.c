#include "op_primary_index_create.h"

void primary_index_create_prepare()
{
    global_primary_index = &(global_prestored_primary_index[global_num_pre_load_primary_index]);
    primary_index_dpu_init(global_primary_index, (__mram_ptr primary_index_entry *)(global_prestored_primary_index_space_addr[global_num_pre_load_primary_index]),
                           primary_index_id, PRIMARY_INDEX_BUCKETS_INITIAL_SIZE);
    global_num_pre_load_primary_index++;
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
}