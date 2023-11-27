#include "cache.h"

void write_result_cache(coo_matrix_v_dpu coo, coo_matrix_v_elem *result_cache, uint32_t size)
{
    mram_write(result_cache, (__mram_ptr coo_matrix_v_elem *)(&(coo->data[coo->nnz])), sizeof(coo_matrix_v_elem) * size);
    coo->nnz += size;
}