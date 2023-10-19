#include "coo_matrix_dpu.h"

coo_matrix_dpu coo_matrix_dpu_init(coo_matrix_dpu coo, uint32_t capacity)
{
    if (!coo)
    {
        // printf("error: coo_matrix_init: NULL\n");
        return coo;
    }
    coo->type = COO_INT32;
    coo->nnz = 0;
    coo->capacity = capacity;
    return coo;
}

coo_matrix_v_dpu coo_matrix_v_dpu_init(coo_matrix_v_dpu coo, uint32_t capacity)
{
    if (!coo)
    {
        // printf("error: coo_matrix_init: NULL\n");
        return coo;
    }
    coo->type = COO_INT64;
    coo->nnz = 0;
    coo->capacity = capacity;
    coo->minus_nnz = 0;
    coo->plus_nnz = 0;
    return coo;
}

coo_matrix_dpu_iterator *coo_matrix_dpu_iterator_init(coo_matrix_dpu_iterator *it, coo_matrix_dpu coo)
{
    it->index = 0;
    it->coo = coo;
    return it;
}

coo_matrix_v_dpu_iterator *coo_matrix_v_dpu_iterator_init(coo_matrix_v_dpu_iterator *it, coo_matrix_v_dpu coo)
{
    it->index = 0;
    it->coo = coo;
    return it;
}

__mram_ptr coo_matrix_elem *coo_matrix_dpu_next_elem(coo_matrix_dpu_iterator *it)
{
    __mram_ptr coo_matrix_elem *elem;
    if (it->coo == NULL || it->index >= it->coo->nnz)
    {
        return (__mram_ptr coo_matrix_elem *)NULL;
    }
    elem = &(it->coo->data[it->index]);
    it->index++;
    return elem;
}

__mram_ptr coo_matrix_v_elem *coo_matrix_v_dpu_next_elem(coo_matrix_v_dpu_iterator *it)
{
    __mram_ptr coo_matrix_v_elem *elem;
    if (it->coo == NULL || it->index >= it->coo->nnz)
    {
        return (__mram_ptr coo_matrix_v_elem *)NULL;
    }
    elem = &(it->coo->data[it->index]);
    it->index++;
    return elem;
}

int coo_matrix_dpu_insert(coo_matrix_dpu coo, uint32_t row, uint32_t col)
{
    if (coo->nnz >= coo->capacity)
        return COO_MATRIX_ERR;
    coo->data[coo->nnz].row = row;
    coo->data[coo->nnz].col = col;
    coo->nnz++;
    return COO_MATRIX_OK;
}

int coo_matrix_dpu_insert_elem(coo_matrix_dpu coo, coo_matrix_elem *elem)
{
    if (coo->nnz >= coo->capacity)
        return COO_MATRIX_ERR;
    coo->data[coo->nnz] = *elem;
    coo->nnz++;
    return COO_MATRIX_OK;
}

void coo_matrix_dpu_dump(const coo_matrix_dpu coo)
{
    coo_matrix_dpu_iterator it;
    coo_matrix_dpu_iterator_init(&it, coo);
    __mram_ptr coo_matrix_elem *elem = coo_matrix_dpu_next_elem(&it);
    printf("-------------------\n");
    printf("COO_Matrix_DPU:\n");
    while (elem)
    {
        printf("row:%d, col:%d\n", elem->row, elem->col);
        elem = coo_matrix_dpu_next_elem(&it);
    }
    printf("-------------------\n");
}

void coo_matrix_v_dpu_dump(const coo_matrix_v_dpu coo)
{
    coo_matrix_v_dpu_iterator it;
    coo_matrix_v_dpu_iterator_init(&it, coo);
    __mram_ptr coo_matrix_v_elem *elem = coo_matrix_v_dpu_next_elem(&it);
    printf("-------------------\n");
    printf("COO_Matrix_DPU:\n");
    while (elem)
    {
        printf("row:%d, val:%ld\n", elem->row, elem->val);
        elem = coo_matrix_v_dpu_next_elem(&it);
    }
    printf("-------------------\n");
}