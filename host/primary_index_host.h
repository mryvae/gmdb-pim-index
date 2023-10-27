#ifndef __PRIMARY_INDEX_HOST_H
#define __PRIMARY_INDEX_HOST_H
#include <stdint.h>
#include "../util/param.h"
#include "dpu_set_context.h"
#include "dpu_index_manager.h"

void primary_index_create(dpu_set_context *set, PRIMARY_INDEX_ID id);
void primary_index_batch_insert(dpu_set_context *set, PRIMARY_INDEX_ID id, char **keys, uint32_t key_len, uint64_t *vals, uint32_t batch_size);
void primary_index_batch_lookup(dpu_set_context *set, PRIMARY_INDEX_ID id, char **keys, uint32_t key_len, uint32_t batch_size, uint64_t **vals);

PRIMARY_INDEX_ID primary_index_create_v1();
void primary_index_batch_insert_v1(PRIMARY_INDEX_ID id, char **keys, uint32_t key_len, uint64_t *vals, uint32_t batch_size);
void primary_index_batch_lookup_v1(PRIMARY_INDEX_ID id, char **keys, uint32_t key_len, uint32_t batch_size, uint64_t **vals);

#endif