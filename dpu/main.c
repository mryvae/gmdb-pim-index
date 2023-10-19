#include <barrier.h>
#include "global.h"
#include "perfcounter.h"
#include "push_package_dpu.h"
#include "../util/util.h"
#include "global_mutex.h"
#include "op_primary_index_create.h"
#include "op_primary_index_delete.h"
#include "op_primary_index_insert.h"
#include "op_primary_index_lookup.h"

BARRIER_INIT(barrier1, NR_TASKLETS);
BARRIER_INIT(barrier2, NR_TASKLETS);

void initial()
{
    mem_reset();

    // do other things
}

#define PRIMARY_INDEX_LOOKUP 1 
#define PRIMARY_INDEX_INSERT 2 
#define PRIMARY_INDEX_DELETE 3 
#define PRIMARY_INDEX_CREATE 4

int master()
{
    initial();
    opType type = global_query_parameter->type;

    if (type == PRIMARY_INDEX_LOOKUP)
    {
        // printf("ENTER INTO MATRIX_CREATE opreate\n");
        primary_index_lookup_prepare();
        barrier_wait(&barrier1);
        barrier_wait(&barrier2);
        primary_index_lookup_merge();
    }

    if (type == PRIMARY_INDEX_INSERT)
    {
        // printf("ENTER INTO MATRIX_CREATE opreate\n");
        op_primary_index_insert_prepare();
        barrier_wait(&barrier1);
        barrier_wait(&barrier2);
        op_primary_index_insert_merge();
    }

    if (type == PRIMARY_INDEX_DELETE)
    {
        // printf("ENTER INTO MATRIX_SUB opreate\n");
        op_primary_index_delete_prepare();
        barrier_wait(&barrier1);
        barrier_wait(&barrier2);
        op_primary_index_delete_merge();
    }

    if (type == PRIMARY_INDEX_CREATE)
    {
        // printf("master ENTER INTO MATRIX_ADD opreate\n");
        primary_index_create_prepare();
        barrier_wait(&barrier1);
        barrier_wait(&barrier2);
        primary_index_create_merge();
    }
    return 0;
}

/* Tasklets wait for the initialization to complete, then compute their checksum. */
int slave()
{
    opType type = global_query_parameter->type;

    if (type == PRIMARY_INDEX_LOOKUP)
    {
        barrier_wait(&barrier1);
        primary_index_lookup_tasklets_run();
        barrier_wait(&barrier2);
    }

    if (type == PRIMARY_INDEX_INSERT)
    {
        barrier_wait(&barrier1);
        op_primary_index_insert_tasklets_run();
        barrier_wait(&barrier2);
    }

    if (type == PRIMARY_INDEX_DELETE)
    {
        // printf("ENTER INTO MATRIX_SUB opreate\n");
        barrier_wait(&barrier1);
        op_primary_index_delete_tasklets_run();
        barrier_wait(&barrier2);
    }

    if (type == PRIMARY_INDEX_CREATE)
    {
        // printf("slave ENTER INTO MATRIX_ADD opreate\n");
        barrier_wait(&barrier1);
        primary_index_create_tasklets_run();
        barrier_wait(&barrier2);
    }
    return 0;
}

int main()
{
    return me() == 0 ? master() : slave();
}