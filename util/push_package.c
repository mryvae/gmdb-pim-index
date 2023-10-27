#include "push_package.h"

push_package *push_package_init(push_package *package)
{
    package->query_offset = 0;
    query_param *query = (query_param *)(package->buf + package->query_offset);
    query->type = 0;
    package->kv_offset = -1;
    package->used_memory = sizeof(query_param);
    return package;
}

query_param *push_package_query_get(push_package *package)
{
    query_param *query = (query_param *)(package->buf + package->query_offset);
    return query;
}

kv_set push_package_kv_init(push_package *package)
{
    query_param *query = (query_param *)(package->buf + package->query_offset);
    package->kv_offset = package->used_memory;
    kv_set kv = (kv_set)(package->buf + package->kv_offset);

    // initial coo_matrix
    int capacity = (MAX_PUSH_BUF_SIZE - package->used_memory - sizeof(struct _kv_set)) / sizeof(kv_elem);
    if (capacity < 0)
    {
        printf("error: push_package_plus_init package capacity is no enough!\n");
        return NULL;
    }
    kv_set_init(kv, capacity);
    return kv;
}

kv_set push_package_kv_get(push_package *package)
{
    if (package->kv_offset < 0)
    {
        // printf("error: push_package_coo_get coo hasn't been initialized!\n");
        return NULL;
    }
    kv_set kv = (kv_set)((package->buf + package->kv_offset));
    return kv;
}

uint32_t push_package_size(push_package *package)
{
    kv_set kv = (kv_set)((package->buf + package->kv_offset));
    package->used_memory += sizeof(struct _kv_set) + kv->nnz * sizeof(kv_elem);
    uint32_t size = package->used_memory;
    size += sizeof(push_package_head);
    return size;
}

void push_package_int_steam_dump(push_package *package)
{
    uint32_t size = package->used_memory + sizeof(push_package_head);
    int *p = (int *)package;
    for (int i = 0; i < size / sizeof(int); i++)
    {
        printf("%d ", p[i]);
    }
    printf("\n");
}