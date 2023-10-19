#ifndef __COO_MATRIX_H
#define __COO_MATRIX_H
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define COO_MATRIX_OK 0
#define COO_MATRIX_ERR 1
typedef uint32_t CooType;
#define COO_INT32 1 // in C: bool
#define COO_INT64 2 // in C: bool

typedef struct _coo_matrix_elem
{
    int32_t row;
    int32_t col;
} coo_matrix_elem;

typedef struct _coo_matrix_v_elem
{
    int32_t row;
    int32_t nonsense; // for data align 8
    uint64_t val;
} coo_matrix_v_elem;

struct _coo_matrix
{
    CooType type;
    uint32_t nnz;
    uint32_t capacity;
    uint32_t nonsense;      // for data align 8
    coo_matrix_elem data[]; // [(i,j,value),(i,j,value),....]
};

struct _coo_matrix_v
{
    CooType type;
    uint32_t nnz;
    uint32_t capacity;
    uint16_t minus_nnz;
    uint16_t plus_nnz;
    coo_matrix_v_elem data[]; // [(i,j,value),(i,j,value),....]
};

typedef struct _coo_matrix_head
{
    CooType type;
    uint32_t nnz;
    uint32_t capacity;
    uint32_t nonsense; // for data align 8
} coo_matrix_head;

typedef struct _coo_matrix *coo_matrix;
typedef struct _coo_matrix_v *coo_matrix_v;

typedef struct _coo_matrix_iterator
{
    uint32_t index;
    coo_matrix coo;
} coo_matrix_iterator;

typedef struct _coo_matrix_v_iterator
{
    uint32_t index;
    coo_matrix_v coo;
} coo_matrix_v_iterator;

void coo_matrix_clear(coo_matrix coo);
coo_matrix coo_matrix_create(uint32_t capacity);
coo_matrix coo_matrix_reset(coo_matrix coo, uint32_t capacity);
void coo_matrix_disorder(coo_matrix coo);
void coo_matrix_disorder_v1(coo_matrix coo);
coo_matrix_v coo_matrix_v_create(uint32_t capacity);
void coo_matrix_v_clear(coo_matrix_v coo);
coo_matrix coo_matrix_init(coo_matrix coo, uint32_t capacity);
coo_matrix_v coo_matrix_v_init(coo_matrix_v coo, uint32_t capacity);
int coo_matrix_insert(coo_matrix coo, int32_t row, int32_t col);
int coo_matrix_insert_elems(coo_matrix coo, coo_matrix_elem *elems, uint32_t num);
int coo_matrix_v_insert(coo_matrix_v coo, int32_t row, uint64_t val);
coo_matrix_iterator *coo_matrix_iterator_init(coo_matrix_iterator *it, coo_matrix coo);
coo_matrix_v_iterator *coo_matrix_v_iterator_init(coo_matrix_v_iterator *it, coo_matrix_v coo);
coo_matrix_elem *coo_matrix_next_elem(coo_matrix_iterator *it);
coo_matrix_v_elem *coo_matrix_v_next_elem(coo_matrix_v_iterator *it);
void coo_matrix_dump(const coo_matrix coo);
void coo_matrix_dump_head(const coo_matrix coo, uint32_t num);
void coo_matrix_v_dump(const coo_matrix_v coo);
void coo_matrix_test();
coo_matrix coo_matrix_merge(const coo_matrix *coo_results, uint32_t num);
coo_matrix coo_matrix_add(coo_matrix A, coo_matrix B);
coo_matrix coo_matrix_sub(coo_matrix A, coo_matrix B);

#endif