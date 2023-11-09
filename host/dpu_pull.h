#ifndef _DPU_PULL_H
#define _DPU_PULL_H
#include "../util/coo_matrix.h"
#include "../util/util.h"
#include "../util/push_package.h"
#include "../upmemInterface/upmem_direct_c.h"
#include "dpu_info.h"
#include <dpu.h>

#define COO_RESULT_OFFSET 0x200000
#define COO_RESULT_SIZE 0x200000
#define PULL_PACKAGE_METADATA_OFFSET (COO_RESULT_OFFSET + COO_RESULT_SIZE)
#define PULL_PACKAGE_METADATA_SIZE 0x000020

#define MAX_PULL_BUF_SIZE (1024 * 128)
#define MIN_PULL_SIZE (2 * 1024)

typedef struct _dpu_pull_buf
{
    char buf[MAX_PULL_BUF_SIZE];
} dpu_pull_buf;

typedef struct _dpu_pull_info
{
    uint32_t nr_dpus;
    dpu_pull_buf *pull_bufs[NR_DPUS];
    coo_matrix_v coo_results[NR_DPUS];
    uint32_t coo_results_capacity;
    pull_package_metadata *coo_metadatas[NR_DPUS];
} dpu_pull_info;

// dpu_pull_info
dpu_pull_info *dpu_pull_info_init(dpu_pull_info *pull_info, uint32_t nr_dpus);
void dpu_pull_info_free(dpu_pull_info *pull_info);
void dpu_pull_coo_metadatas(dpu_pull_info *pull_info, XDPI interface);
void dpu_pull_coo_results(dpu_pull_info *pull_info, XDPI interface);
void dpu_pull_merge_results(dpu_pull_info *pull_info, uint64_t *vals, uint32_t batch_size);

#endif