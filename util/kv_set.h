#ifndef __KV_SET_H
#define __KV_SET_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct _kv_elem
{
    char key[16];
    union
    {
        int32_t key_id;
        uint64_t val;
    };
} kv_elem;

struct _kv_set
{
    uint32_t nnz;
    uint32_t capacity;
    kv_elem data[];
};

typedef struct _kv_set *kv_set;

kv_set kv_set_init(kv_set set, uint32_t capacity);

#endif