#ifndef _OP_PRIMARY_INDEX_INSERT_H
#define _OP_PRIMARY_INDEX_INSERT_H

#include "global.h"
#include "coo_matrix_dpu.h"
#include "primary_index_dpu.h"

void op_primary_index_insert_prepare();
void op_primary_index_insert_tasklets_run();
void op_primary_index_insert_merge();

#endif