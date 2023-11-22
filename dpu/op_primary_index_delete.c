#include "op_primary_index_delete.h"
#include <seqread.h>

void op_primary_index_delete_prepare()
{
    global_primary_index = NULL;
    for (int j = 0; j < global_num_pre_load_primary_index; j++)
    {
        if (primary_index_id == global_prestored_primary_index[j].index_id)
        {
            global_primary_index = &(global_prestored_primary_index[j]);
            break;
        }
    }
}

void op_primary_index_delete_tasklets_run()
{
    uint32_t tasklet_id = me();
    if (tasklet_id == 0)
        return;

    kv_elem *current_elem;
    seqreader_buffer_t local_cache = seqread_alloc();
    seqreader_t sr;
    uint32_t nnz_handled = 0;
    uint32_t minus_start;
    uint32_t minus_nnz;
    __mram_ptr kv_elem *start;

    if (global_primary_index)
    {
        minus_start = BLOCK_LOW(tasklet_id - 1, NR_SLAVE_TASKLETS, global_kvsd_nnz);
        minus_nnz = BLOCK_SIZE(tasklet_id - 1, NR_SLAVE_TASKLETS, global_kvsd_nnz);
        // printf("tasklet_id: %u, NR_SLAVE_TASKLETS: %u, plus__nnz: %u, delta_start: %u, delta_nnz: %u\n",
        //        tasklet_id, NR_SLAVE_TASKLETS, global_bucket_matrix_slice_B_plus_minus_nnz[i], delta_start, delta_nnz);
        if (minus_nnz > 0)
        {
            start = (__mram_ptr kv_elem *)(&(global_kvsd->data[minus_start]));
            current_elem = seqread_init(local_cache, start, &sr);

        handle_elem:
            nnz_handled++;
            primary_index_dpu_delete(global_primary_index, current_elem->key, global_key_len, &global_index_mram_allocator);
        }
        while (nnz_handled < minus_nnz)
        {
            current_elem = seqread_get(current_elem, sizeof(kv_elem), &sr);
            goto handle_elem;
        }
    }
}

void op_primary_index_delete_merge()
{
    // nothing
}