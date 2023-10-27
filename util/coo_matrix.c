#include "coo_matrix.h"
#include <string.h>

void coo_matrix_clear(coo_matrix coo)
{
    coo->nnz = 0;
}

void coo_matrix_v_clear(coo_matrix_v coo)
{
    coo->nnz = 0;
}

coo_matrix coo_matrix_create(uint32_t capacity)
{
    coo_matrix coo;
    coo = (coo_matrix)malloc(sizeof(struct _coo_matrix) + sizeof(coo_matrix_elem) * capacity);
    coo->type = COO_INT32;
    coo->nnz = 0;
    coo->capacity = capacity;
    return coo;
}

coo_matrix coo_matrix_reset(coo_matrix coo, uint32_t capacity)
{
    coo_matrix new;
    if (coo->capacity < capacity)
    {
        free(coo);
        new = (coo_matrix)malloc(sizeof(struct _coo_matrix) + sizeof(coo_matrix_elem) * capacity);
        new->type = COO_INT32;
        new->nnz = 0;
        new->capacity = capacity;
        return new;
    }
    coo->type = COO_INT32;
    coo->nnz = 0;
    return coo;
}

void coo_matrix_disorder(coo_matrix coo)
{
    for (int i = 0; i < coo->nnz; i++)
    {
        int m = rand() % coo->nnz;
        int n = rand() % coo->nnz;
        coo_matrix_elem t = coo->data[m];
        coo->data[m] = coo->data[n];
        coo->data[n] = t;
    }
}

void coo_matrix_disorder_v1(coo_matrix coo)
{
    for (int i = 0; i < coo->nnz; i++)
    {
        int m = i;
        int n = rand() % coo->nnz;
        coo_matrix_elem t = coo->data[m];
        coo->data[m] = coo->data[n];
        coo->data[n] = t;
    }
}

coo_matrix_v coo_matrix_v_create(uint32_t capacity)
{
    coo_matrix_v coo;
    coo = (coo_matrix_v)malloc(sizeof(struct _coo_matrix_v) + sizeof(coo_matrix_v_elem) * capacity);
    coo->type = COO_INT64;
    coo->nnz = 0;
    coo->capacity = capacity;
    return coo;
}

coo_matrix coo_matrix_init(coo_matrix coo, uint32_t capacity)
{
    if (!coo)
    {
        printf("error: coo_matrix_init: NULL\n");
        return coo;
    }
    coo->type = COO_INT32;
    coo->nnz = 0;
    coo->capacity = capacity;
    return coo;
}

coo_matrix_v coo_matrix_v_init(coo_matrix_v coo, uint32_t capacity)
{
    if (!coo)
    {
        printf("error: coo_matrix_v_init: NULL\n");
        return coo;
    }
    coo->type = COO_INT64;
    coo->nnz = 0;
    coo->capacity = capacity;
    return coo;
}

int coo_matrix_insert(coo_matrix coo, int32_t row, int32_t col)
{
    if (coo->nnz >= coo->capacity)
        return COO_MATRIX_ERR;

    uint32_t nnz = coo->nnz;
    coo->data[nnz].row = row;
    coo->data[nnz].col = col;
    coo->nnz++;
    return COO_MATRIX_OK;
}

int coo_matrix_insert_elems(coo_matrix coo, coo_matrix_elem *elems, uint32_t num)
{
    if (coo->nnz + num > coo->capacity)
        return COO_MATRIX_ERR;

    uint32_t nnz = coo->nnz;
    coo_matrix_elem *coo_data = coo->data;
    for (int i = 0; i < num; i++)
    {
        coo_data[coo->nnz].row = elems[i].row;
        coo_data[coo->nnz].col = elems[i].col;
        coo->nnz++;
    }
    // memcpy(&(coo->data[nnz]), elems, num * sizeof(coo_matrix_elem));
    // coo->nnz += num;
    return COO_MATRIX_OK;
}

int coo_matrix_v_insert(coo_matrix_v coo, int32_t row, uint64_t val)
{
    if (coo->nnz >= coo->capacity)
        return COO_MATRIX_ERR;

    uint32_t nnz = coo->nnz;
    coo->data[nnz].row = row;
    coo->data[nnz].val = val;
    coo->nnz++;
    return COO_MATRIX_OK;
}

coo_matrix_iterator *coo_matrix_iterator_init(coo_matrix_iterator *it, coo_matrix coo)
{
    it->index = 0;
    it->coo = coo;
    return it;
}

coo_matrix_v_iterator *coo_matrix_v_iterator_init(coo_matrix_v_iterator *it, coo_matrix_v coo)
{
    it->index = 0;
    it->coo = coo;
    return it;
}

coo_matrix_elem *coo_matrix_next_elem(coo_matrix_iterator *it)
{
    coo_matrix_elem *elem;
    if (it->index >= it->coo->nnz)
    {
        return NULL;
    }
    elem = &(it->coo->data[it->index]);
    it->index++;
    return elem;
}

coo_matrix_v_elem *coo_matrix_v_next_elem(coo_matrix_v_iterator *it)
{
    coo_matrix_v_elem *elem;
    if (it->index >= it->coo->nnz)
    {
        return NULL;
    }
    elem = &(it->coo->data[it->index]);
    it->index++;
    return elem;
}

void coo_matrix_dump(const coo_matrix coo)
{
    if (!coo)
    {
        printf("error: coo_matrix_dump NULL\n");
        return;
    }
    coo_matrix_iterator it;
    coo_matrix_iterator_init(&it, coo);
    coo_matrix_elem *elem = coo_matrix_next_elem(&it);
    printf("-------------------\n");
    printf("COO_Matrix:\n");
    while (elem)
    {
        printf("row:%d,col:%d\n", elem->row, elem->col);
        elem = coo_matrix_next_elem(&it);
    }
    printf("nnz: %d\n", coo->nnz);
    printf("-------------------\n");
}

void coo_matrix_dump_head(const coo_matrix coo, uint32_t num)
{
    printf("nnz: %d\n", coo->nnz);
    if (coo->nnz < num)
    {
        coo_matrix_dump(coo);
    }
    for (int i = 0; i < num; i++)
    {
        printf("row: %d, col: %d\n", coo->data[i].row, coo->data[i].col);
    }
}

void coo_matrix_v_dump(const coo_matrix_v coo)
{
    if (!coo)
    {
        printf("error: coo_matrix_v_dump NULL\n");
        return;
    }
    coo_matrix_v_iterator it;
    coo_matrix_v_iterator_init(&it, coo);
    coo_matrix_v_elem *elem = coo_matrix_v_next_elem(&it);
    printf("-------------------\n");
    printf("COO_Matrix:\n");
    while (elem)
    {
        printf("row:%d, val:%ld\n", elem->row, elem->val);
        elem = coo_matrix_v_next_elem(&it);
    }
    printf("-------------------\n");
}

coo_matrix coo_matrix_merge(const coo_matrix *coo_results, uint32_t num)
{
    coo_matrix result;

    uint32_t capacity = 0;
    CooType type;
    if (num > 0)
    {
        type = coo_results[0]->type;
    }
    else
    {
        return NULL;
    }
    int i;
    for (i = 0; i < num; i++)
    {
        if (coo_results[i])
        {
            capacity += coo_results[i]->nnz;
        }
    }
    result = coo_matrix_create(capacity);

    // for efficiency, not use coo_matrix_iterator to traversal coo_matrix
    coo_matrix_elem *src;
    coo_matrix_elem *dst;
    size_t size;
    for (i = 0; i < num; i++)
    {
        if (coo_results[i])
        {
            src = coo_results[i]->data;
            dst = result->data + result->nnz;
            size = sizeof(struct _coo_matrix_elem) * coo_results[i]->nnz;
            if (size > 0)
            {
                memcpy(dst, src, size);
                result->nnz += coo_results[i]->nnz;
            }
        }
    }
    return result;
}

coo_matrix coo_matrix_add(coo_matrix A, coo_matrix B)
{
    coo_matrix res = coo_matrix_create(B->capacity + A->capacity);
    int i = 0, j = 0;
    while (i < A->nnz || j < B->nnz)
    {
        if (i < A->nnz && j < B->nnz)
        {
            if (A->data[i].row < B->data[j].row)
            {
                coo_matrix_insert(res, A->data[i].row, A->data[i].col);
                i++;
            }
            else if (A->data[i].row == B->data[j].row && A->data[i].col < B->data[j].col)
            {
                coo_matrix_insert(res, A->data[i].row, A->data[i].col);
                i++;
            }
            else
            {
                coo_matrix_insert(res, B->data[j].row, B->data[j].col);
                j++;
            }
        }
        else if (j >= B->nnz)
        {
            for (; i < A->nnz; i++)
                coo_matrix_insert(res, A->data[i].row, A->data[i].col);
        }
        else if (i >= A->nnz)
        {
            for (; j < B->nnz; j++)
                coo_matrix_insert(res, B->data[j].row, B->data[j].col);
        }
    }
    return res;
}

coo_matrix coo_matrix_sub(coo_matrix A, coo_matrix B)
{
    coo_matrix res = coo_matrix_create(B->capacity);
    int i = 0, j = 0;
    for (; j < B->nnz; j++)
    {
        if (i < A->nnz && A->data[i].row == B->data[j].row && A->data[i].col == B->data[j].col)
        {
            i++;
            continue;
        }
        coo_matrix_insert(res, B->data[j].row, B->data[j].col);
    }
    return res;
}