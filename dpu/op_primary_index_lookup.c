#include "op_primary_index_lookup.h"
#include "cache.h"
#include <seqread.h>

void primary_index_lookup_prepare()
{
    global_primary_index = NULL;
    for (int j = 0; j < global_num_pre_load_primary_index; j++)
    {
        if (primary_index_id == global_prestored_primary_index[j].index_id)
        {
            global_primary_index = &(global_prestored_primary_index[j]);
            global_coo_A_nnz = global_coo_A->nnz;
            break;
        }
    }
    coo_matrix_v_dpu_init(global_coo_result, COO_RESULT_SIZE / (sizeof(coo_matrix_v_elem)));
}

void primary_index_lookup_tasklets_run()
{
    uint32_t tasklet_id = me();
    if (tasklet_id == 0)
        return;

    coo_matrix_elem *current_elem;
    __mram_ptr coo_matrix_elem *start;
    int32_t nnz_start;
    int32_t nnz_tasklet;
    uint32_t nnz_handled = 0;
    __dma_aligned coo_matrix_v_elem result_cache[RESULT_CACHE_SIZE];
    uint32_t result_cache_nnz = 0;
    seqreader_buffer_t local_cache = seqread_alloc();
    seqreader_t sr;
    __mram_ptr primary_index_entry *target;
    __dma_aligned primary_index_entry entry_buffer;
    uint64_t val;

    if (global_primary_index)
    {
        nnz_start = BLOCK_LOW(tasklet_id - 1, NR_SLAVE_TASKLETS, global_plus_nnz);
        nnz_tasklet = BLOCK_SIZE(tasklet_id - 1, NR_SLAVE_TASKLETS, global_plus_nnz);

        if (nnz_tasklet > 0)
        {
            start = (__mram_ptr coo_matrix_elem *)(&(global_coo_A->data[nnz_start]));
            current_elem = seqread_init(local_cache, start, &sr);

        handle_elem:
            nnz_handled++;
            // printf("primary_index_dpu_lookup, key: %d\n", current_elem->col);
            target = primary_index_dpu_lookup(global_primary_index, current_elem->col);
            if (target)
            {
                mram_read(target, (void *)&entry_buffer, sizeof(primary_index_entry));
                val = entry_buffer.val;
                // printf("find it, val: %ld\n", val);
            }
            else
            {
                val = 0;
            }
            // printf("RESULT_CACHE_ADD_ELEM, row: %d, val: %ld\n", current_elem->row, val);
            RESULT_CACHE_ADD_ELEM(result_cache, current_elem->row, val, result_cache_nnz);
            if (result_cache_nnz >= RESULT_CACHE_SIZE)
            {
                // last jump, write it to global_coo_result, need lock
                result_cache_mutex_lock();
                write_result_cache(global_coo_result, result_cache, RESULT_CACHE_SIZE);
                result_cache_mutex_unlock();
                result_cache_nnz = 0;
            }
        }
        while (nnz_handled < nnz_tasklet)
        {
            current_elem = seqread_get(current_elem, sizeof(coo_matrix_elem), &sr);
            goto handle_elem;
        }

        if (result_cache_nnz > 0)
        {
            // last jump, write it to global_coo_result, need lock
            result_cache_mutex_lock();
            write_result_cache(global_coo_result, result_cache, result_cache_nnz);
            result_cache_mutex_unlock();
            result_cache_nnz = 0;
        }
    }
}

void primary_index_lookup_merge()
{
    global_pull_package_metadata->result_size = global_coo_result->nnz;
}