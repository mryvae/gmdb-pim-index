#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <defs.h>
#include <stdint.h>
#include <mram.h>
#include <alloc.h>
#include "../../util/param.h"
#include "../../util/push_package.h"
#include "coo_matrix_dpu.h"
#include "linear_mram_alloc.h"
#include "mram_memory.h"
#include "primary_index_dpu.h"

extern __mram_ptr push_package *global_package;
extern __mram_ptr query_param *global_query_parameter;

extern __mram_ptr pull_package_metadata *global_pull_package_metadata;

extern kv_set_dpu global_kvsd;
extern uint32_t global_kvsd_nnz;

extern PRIMARY_INDEX_ID primary_index_id;
extern uint32_t global_key_len;
extern primary_index_dpu *global_primary_index;

extern __host uint32_t global_num_pre_load_primary_index;
extern __host primary_index_dpu global_prestored_primary_index[MAX_NUM_PRE_LOAD_PRIMARY_INDEX];
extern __mram_ptr void *global_prestored_primary_index_space_addr[MAX_NUM_PRE_LOAD_PRIMARY_INDEX];

extern __host linear_mram_allocator global_index_mram_allocator;
extern uint32_t global_index_mram_allocator_initial_flag;

extern coo_matrix_v_dpu global_coo_result;

#endif