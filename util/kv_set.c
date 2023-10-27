#include "kv_set.h"

kv_set kv_set_init(kv_set set, uint32_t capacity)
{
    if (!set)
    {
        printf("error: kv_set_init: NULL\n");
        return set;
    }
    set->nnz = 0;
    set->capacity = capacity;
    return set;
}