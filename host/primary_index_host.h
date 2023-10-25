#ifndef __PRIMARY_INDEX_HOST_H
#define __PRIMARY_INDEX_HOST_H
#include <stdint.h>
#include "../util/param.h"
#include "dpu_set_context.h"

void primary_index_create(dpu_set_context *set, PRIMARY_INDEX_ID id);
void primary_index_batch_insert(dpu_set_context *set, PRIMARY_INDEX_ID id, uint32_t *keys, uint64_t *vals, uint32_t batch_size);
void primary_index_batch_delete(dpu_set_context *set, PRIMARY_INDEX_ID id, uint32_t *keys, uint32_t batch_size);
void primary_index_batch_lookup(dpu_set_context *set, PRIMARY_INDEX_ID id, uint32_t *keys, uint32_t batch_size, uint64_t **vals);

#endif