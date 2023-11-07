#ifndef _DPU_PUSH_H
#define _DPU_PUSH_H

#include <dpu.h>
#include <assert.h>
#include "dpu_info.h"
#include "../util/param.h"
#include "../util/coo_matrix.h"
#include "../util/push_package.h"

#define PUSH_PACKAGE_OFFSET 0x0

typedef struct _dpu_push_info
{
    uint32_t nr_dpus;
    push_package *packages;
} dpu_push_info;

// dpu_push_info
dpu_push_info *dpu_push_info_init(dpu_push_info *push_info, uint32_t nr_dpus);
dpu_push_info *dpu_push_info_reset(dpu_push_info *push_info);
void dpu_push_info_free(dpu_push_info *push_info);

// operations
void dpu_push_build_package_primary_index_create(dpu_push_info *push_info, PRIMARY_INDEX_ID id);
void dpu_push_build_package_primary_index_insert(dpu_push_info *push_info, PRIMARY_INDEX_ID id, char **keys, uint32_t key_len, uint64_t *vals, uint32_t batch_size);
void dpu_push_build_package_primary_index_delete(dpu_push_info *push_info, PRIMARY_INDEX_ID id, char **keys, uint32_t key_len, uint32_t batch_size);
void dpu_push_build_package_primary_index_lookup(dpu_push_info *push_info, PRIMARY_INDEX_ID id, char **keys, uint32_t key_len, uint32_t batch_size);
void dpu_push_package(dpu_push_info *push_info, struct dpu_set_t dpu_set);

#endif