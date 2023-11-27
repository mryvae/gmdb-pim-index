#include "global_var.h"

// initial information
__mram_ptr push_package *global_package = (__mram_ptr push_package *)(DPU_MRAM_HEAP_POINTER + PUSH_PACKAGE_OFFSET);
__mram_ptr query_param *global_query_parameter = (__mram_ptr query_param *)(DPU_MRAM_HEAP_POINTER + PUSH_PACKAGE_OFFSET + sizeof(push_package_head));

__mram_ptr pull_package_metadata *global_pull_package_metadata = (__mram_ptr pull_package_metadata *)(DPU_MRAM_HEAP_POINTER + PULL_PACKAGE_METADATA_OFFSET);

kv_set_dpu global_kvsd;
uint32_t global_kvsd_nnz;

PRIMARY_INDEX_ID primary_index_id;
uint32_t global_key_len;
primary_index_dpu *global_primary_index;

__host uint32_t global_num_pre_load_primary_index = 0;
__host primary_index_dpu global_prestored_primary_index[PRIMARY_INDEX_MAX_NUM];
__mram_ptr void *global_primary_index_space_addr;
size_t global_primary_index_space_size;
uint32_t global_primary_index_space_initial_flag = 0;

__host linear_mram_allocator global_index_mram_allocator;
uint32_t global_index_mram_allocator_initial_flag = 0;

coo_matrix_v_dpu global_coo_result = (coo_matrix_v_dpu)(DPU_MRAM_HEAP_POINTER + COO_RESULT_OFFSET);
