#ifndef _OP_PRIMARY_INDEX_DELETE_H
#define _OP_PRIMARY_INDEX_DELETE_H

#include "global_var.h"
#include "coo_matrix_dpu.h"
#include "primary_index_dpu.h"

void op_primary_index_delete_prepare();
void op_primary_index_delete_tasklets_run();
void op_primary_index_delete_merge();

#endif