#ifndef _OP_PRIMARY_INDEX_CREATE_H
#define _OP_PRIMARY_INDEX_CREATE_H

#include "global.h"

void primary_index_create_prepare();
void primary_index_create_tasklets_run();
void primary_index_create_merge();

#endif