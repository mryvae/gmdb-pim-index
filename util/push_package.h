#ifndef __PUSH_PACKAGE_H
#define __PUSH_PACKAGE_H
#include "coo_matrix.h"
#include "param.h"

#define MAX_PUSH_BUF_SIZE (1024 * 64 * 8 * 4) // 64KB

typedef struct _push_package_head
{
    int32_t query_offset;
    int32_t coo_offset;
    uint32_t used_memory;
    uint32_t nonsense;
} push_package_head;

typedef struct _pull_package_metadata
{
    uint32_t result_size;
    uint32_t bucket_hit;
    uint32_t bucket_miss;
    uint32_t nonsense;
} pull_package_metadata;

typedef struct _push_package
{
    int32_t query_offset;
    int32_t coo_offset;
    uint32_t used_memory;
    uint32_t nonsense;
    char buf[MAX_PUSH_BUF_SIZE];
} push_package;

push_package *push_package_init(push_package *package);
query_param *push_package_query_get(push_package *package);
coo_matrix push_package_coo_init(push_package *package);
coo_matrix push_package_coo_get(push_package *package);
uint32_t push_package_size(push_package *package);
void push_package_int_steam_dump(push_package *package);

#endif