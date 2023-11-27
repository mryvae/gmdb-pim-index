#ifndef __COO_Matrix_DPU_H
#define __COO_Matrix_DPU_H
#include <mram.h>
#include <alloc.h>
#include "../../util/coo_matrix.h"
#include "../../util/kv_set.h"

typedef __mram_ptr struct _coo_matrix *coo_matrix_dpu;
typedef __mram_ptr struct _coo_matrix_v *coo_matrix_v_dpu;
typedef __mram_ptr struct _kv_set *kv_set_dpu;

typedef struct _coo_matrix_dpu_set
{
    uint32_t size;
    uint32_t capacity;
    __mram_ptr coo_matrix_dpu *coo_set;
} coo_matrix_dpu_set;

typedef struct _coo_matrix_dpu_iterator
{
    uint32_t index;
    coo_matrix_dpu coo;
} coo_matrix_dpu_iterator;

typedef struct _coo_matrix_v_dpu_iterator
{
    uint32_t index;
    coo_matrix_v_dpu coo;
} coo_matrix_v_dpu_iterator;

// coo_matrix_dpu api
coo_matrix_dpu coo_matrix_dpu_init(coo_matrix_dpu coo, uint32_t capacity);
int coo_matrix_dpu_insert(coo_matrix_dpu coo, uint32_t row, uint32_t col);
int coo_matrix_dpu_insert_elem(coo_matrix_dpu coo, coo_matrix_elem *elem);
void coo_matrix_dpu_dump(const coo_matrix_dpu coo);

// coo_matrix_v_dpu api
coo_matrix_v_dpu coo_matrix_v_dpu_init(coo_matrix_v_dpu coo, uint32_t capacity);
void coo_matrix_v_dpu_dump(const coo_matrix_v_dpu coo);

// coo_matrix_dpu_iterator api
coo_matrix_dpu_iterator *coo_matrix_dpu_iterator_init(coo_matrix_dpu_iterator *it, coo_matrix_dpu coo);
__mram_ptr coo_matrix_elem *coo_matrix_dpu_next_elem(coo_matrix_dpu_iterator *it);

// coo_matrix_v_dpu_iterator api
coo_matrix_v_dpu_iterator *coo_matrix_v_dpu_iterator_init(coo_matrix_v_dpu_iterator *it, coo_matrix_v_dpu coo);
__mram_ptr coo_matrix_v_elem *coo_matrix_v_dpu_next_elem(coo_matrix_v_dpu_iterator *it);

void kv_set_dpu_dump(const kv_set_dpu kvsd);

#endif