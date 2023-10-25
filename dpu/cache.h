#ifndef _CACHE_H
#define _CACHE_H

#include "coo_matrix_dpu.h"

#define RESULT_CACHE_SIZE 16

#define RESULT_CACHE_ADD_ELEM(result_cache, elem_row, elem_val, result_index) \
    result_cache[result_index].row = elem_row;                                \
    result_cache[result_index].val = elem_val;                                \
    result_index++;

void write_result_cache(coo_matrix_v_dpu coo, coo_matrix_v_elem *result_cache, uint32_t size);

#endif