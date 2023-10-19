#include "push_package.h"

push_package *push_package_init(push_package *package)
{
    package->query_offset = 0;
    query_param *query = (query_param *)(package->buf + package->query_offset);
    query->type = 0;
    package->coo_offset = -1;
    package->used_memory = sizeof(query_param);
}

query_param *push_package_query_get(push_package *package)
{
    query_param *query = (query_param *)(package->buf + package->query_offset);
    return query;
}

coo_matrix push_package_coo_init(push_package *package)
{
    query_param *query = (query_param *)(package->buf + package->query_offset);
    if (query->type == PRIMARY_INDEX_LOOKUP)
    {
        package->coo_offset = package->used_memory;
        coo_matrix coo = (coo_matrix)(package->buf + package->coo_offset);

        // initial coo_matrix
        int capacity = (MAX_PUSH_BUF_SIZE - package->used_memory - sizeof(struct _coo_matrix)) / sizeof(coo_matrix_elem);
        if (capacity < 0)
        {
            printf("error: push_package_plus_init package capacity is no enough!\n");
            return NULL;
        }
        coo_matrix_init(coo, capacity);
        return coo;
    }
    else
    {
        package->coo_offset = package->used_memory;
        coo_matrix_v coo_v = (coo_matrix_v)(package->buf + package->used_memory);

        // initial coo_matrix
        int capacity = (MAX_PUSH_BUF_SIZE - package->used_memory - sizeof(struct _coo_matrix_v)) / sizeof(coo_matrix_v_elem);
        if (capacity < 0)
        {
            printf("error: push_package_plus_init package capacity is no enough!\n");
            return NULL;
        }
        coo_matrix_v_init(coo_v, capacity);
        return coo_v;
    }
}

coo_matrix push_package_coo_get(push_package *package)
{
    if (package->coo_offset < 0)
    {
        // printf("error: push_package_coo_get coo hasn't been initialized!\n");
        return NULL;
    }
    coo_matrix matrix = (coo_matrix)((package->buf + package->coo_offset));
    return matrix;
}

uint32_t push_package_size(push_package *package)
{
    coo_matrix matrix = (coo_matrix)((package->buf + package->coo_offset));
    if (matrix->type == COO_INT32)
    {
        package->used_memory += sizeof(struct _coo_matrix) + matrix->nnz * sizeof(coo_matrix_elem);
    }
    else
    {
        matrix = (coo_matrix_v)matrix;
        package->used_memory += sizeof(struct _coo_matrix_v) + matrix->nnz * sizeof(coo_matrix_v_elem);
    }
    uint32_t size = package->used_memory;
    size += sizeof(push_package_head);
    return size;
}

void push_package_int_steam_dump(push_package *package)
{
    uint32_t size = push_package_size(package);
    int *p = (int *)package;
    for (int i = 0; i < size / sizeof(int); i++)
    {
        printf("%d ", p[i]);
    }
    printf("\n");
}