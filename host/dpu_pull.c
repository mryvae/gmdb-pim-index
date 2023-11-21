#include "dpu_pull.h"

dpu_pull_info *dpu_pull_info_init(dpu_pull_info *pull_info, uint32_t nr_dpus)
{
    if (!pull_info)
    {
        return NULL;
    }

    pull_info->nr_dpus = nr_dpus;
    for (int i = 0; i < NR_DPUS; i++)
    {
        pull_info->pull_bufs[i] = malloc(sizeof(dpu_pull_buf));
    }

    for (int i = 0; i < NR_DPUS; i++)
    {
        pull_info->coo_metadatas[i] = pull_info->pull_bufs[i]->buf;
    }

    for (int i = 0; i < NR_DPUS; i++)
    {
        pull_info->coo_results[i] = pull_info->pull_bufs[i]->buf + PULL_PACKAGE_METADATA_SIZE;
    }

    pull_info->coo_results_capacity = (sizeof(dpu_pull_buf) - PULL_PACKAGE_METADATA_SIZE - sizeof(struct _coo_matrix_v)) / sizeof(coo_matrix_v_elem);
    for (int i = 0; i < NR_DPUS; i++)
    {
        coo_matrix_v_init(pull_info->coo_results[i], pull_info->coo_results_capacity);
    }
    return pull_info;
}

void dpu_pull_info_free(dpu_pull_info *pull_info)
{
    if (!pull_info)
        return;
    int i;
    for (i = 0; i < NR_DPUS; i++)
    {
        free(pull_info->pull_bufs[i]);
    }
}

void dpu_pull_coo_metadatas(dpu_pull_info *pull_info, struct dpu_set_t dpu_set)
{
    // uint8_t *buffers[NR_DPUS];
    // for (int i = 0; i < NR_DPUS; i++)
    // {
    //     buffers[i] = pull_info->coo_metadatas[i];
    // }
    // ReceiveFromPIM(interface, buffers, DPU_MRAM_HEAP_POINTER_NAME, PULL_PACKAGE_METADATA_OFFSET, sizeof(pull_package_metadata), 0);

    struct dpu_set_t dpu;
    uint32_t each_dpu;
    DPU_FOREACH(dpu_set, dpu, each_dpu)
    {
        DPU_ASSERT(dpu_prepare_xfer(dpu, pull_info->coo_metadatas[each_dpu]));
    }
    DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, DPU_MRAM_HEAP_POINTER_NAME, PULL_PACKAGE_METADATA_OFFSET, sizeof(pull_package_metadata), DPU_XFER_DEFAULT));
}

void dpu_pull_coo_results(dpu_pull_info *pull_info, struct dpu_set_t dpu_set)
{
    struct dpu_set_t dpu;
    uint32_t each_dpu, length, max_capacity = 0;

    for (int i = 0; i < NR_DPUS; i++)
    {
        if (max_capacity < pull_info->coo_metadatas[i]->result_size)
        {
            max_capacity = pull_info->coo_metadatas[i]->result_size;
        }
    }
    // printf("per_coo_results_capacity: %d,max_capacity: %d\n", pull_info->per_coo_results_capacity, max_capacity);
    if (pull_info->coo_results_capacity < max_capacity)
    {
        printf("pull buf exceeds!\n");
        return;
    }

    // uint8_t *buffers[NR_DPUS];
    // for (int i = 0; i < NR_DPUS; i++)
    // {
    //     buffers[i] = pull_info->coo_results[i];
    // }
    // ReceiveFromPIM(interface, buffers, DPU_MRAM_HEAP_POINTER_NAME, COO_RESULT_OFFSET, align8(length), 0);

    DPU_FOREACH(dpu_set, dpu, each_dpu)
    {
        DPU_ASSERT(dpu_prepare_xfer(dpu, pull_info->coo_results[each_dpu]));
    }
    length = sizeof(struct _coo_matrix_v) + sizeof(coo_matrix_v_elem) * max_capacity;
    DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, DPU_MRAM_HEAP_POINTER_NAME, COO_RESULT_OFFSET, align8(length), DPU_XFER_DEFAULT));
}

void dpu_pull_merge_results(dpu_pull_info *pull_info, uint64_t *vals, uint32_t batch_size)
{
    coo_matrix_v coo_result;
    for (int i = 0; i < NR_DPUS; i++)
    {
        coo_result = pull_info->coo_results[i];
        for (int j = 0; j < coo_result->nnz; j++)
        {
            // printf("row:%d, col:%ld\n", coo_result->data[j].row, coo_result->data[j].val);
            vals[coo_result->data[j].row] = coo_result->data[j].val;
        }
    }
}