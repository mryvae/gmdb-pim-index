#ifndef __PRIMARY_INDEX_HOST_H
#define __PRIMARY_INDEX_HOST_H
#include <stdint.h>
#include "../util/param.h"

PRIMARY_INDEX_ID primary_index_create();

void primary_index_batch_insert(PRIMARY_INDEX_ID id, uint32_t *keys, uint64_t *vals, uint32_t batch_size);
void primary_index_batch_delete(PRIMARY_INDEX_ID id, uint32_t *keys, uint32_t batch_size);
void primary_index_batch_lookup(PRIMARY_INDEX_ID id, uint32_t *keys, uint32_t batch_size, uint64_t **vals);

#endif