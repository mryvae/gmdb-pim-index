#ifndef _OP_PRIMARY_INDEX_LOOKUP_H
#define _OP_PRIMARY_INDEX_LOOKUP_H

#include "global_var.h"
#include "coo_matrix_dpu.h"

void primary_index_lookup_prepare();
void primary_index_lookup_tasklets_run();
void primary_index_lookup_merge();

#endif