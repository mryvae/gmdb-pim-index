#include "dpu_push.h"

dpu_push_info *dpu_push_info_init(dpu_push_info *push_info, uint32_t nr_dpus)
{
    if (!push_info)
    {
        return push_info;
    }

    push_info->nr_dpus = nr_dpus;
    push_info->packages = malloc(sizeof(push_package) * nr_dpus);

    for (uint32_t i = 0; i < nr_dpus; i++)
    {
        push_package_init(&(push_info->packages[i]));
    }
    return push_info;
}

dpu_push_info *dpu_push_info_reset(dpu_push_info *push_info)
{
    if (!push_info)
    {
        return push_info;
    }

    uint32_t nr_dpus = push_info->nr_dpus;
    for (uint32_t i = 0; i < nr_dpus; i++)
    {
        push_package_init(&(push_info->packages[i]));
    }
    return push_info;
}

void dpu_push_info_free(dpu_push_info *push_info)
{
    if (!push_info)
    {
        return;
    }

    free(push_info->packages);
}

void dpu_push_build_package_primary_index_create(dpu_push_info *push_info, PRIMARY_INDEX_ID id)
{
    dpu_push_info_reset(push_info);
    push_package *packages = push_info->packages;
    for (int i = 0; i < NR_DPUS; i++)
    {
        query_param *query = push_package_query_get(&(packages[i]));
        query->type += PRIMARY_INDEX_CREATE;
        query->primary_index_id = id;
    }
}

void dpu_push_build_package_primary_index_insert(dpu_push_info *push_info, PRIMARY_INDEX_ID id, uint32_t *keys, uint64_t *vals, uint32_t batch_size)
{
    uint32_t nr_dpus = push_info->nr_dpus;
    int32_t dpu_id;

    push_package *packages = push_info->packages;
    coo_matrix_v_elem *coo_data[NR_DPUS];
    uint32_t nnz[NR_DPUS];
    uint32_t capility[NR_DPUS];
    query_param *query;

    dpu_push_info_reset(push_info);

    for (int i = 0; i < NR_DPUS; i++)
    {
        query = push_package_query_get(&(packages[i]));
        query->type = PRIMARY_INDEX_INSERT;
        query->primary_index_id = id;
    }

    coo_matrix_v coo_kvs;
    for (int i = 0; i < NR_DPUS; i++)
    {
        coo_kvs = push_package_coo_init(&(packages[i]));
        if (coo_kvs)
        {
            coo_data[i] = coo_kvs->data;
            nnz[i] = 0;
            capility[i] = coo_kvs->capacity;
        }
        else
        {
            printf("error: dpu_push_build_package_primary_index_insert\n");
            return;
        }
    }
    for (int i = 0; i < batch_size; i++)
    {
        dpu_id = keys[i] % NR_DPUS;
        coo_data[dpu_id][nnz[dpu_id]].row = keys[i];
        coo_data[dpu_id][nnz[dpu_id]].val = vals[i];
        nnz[dpu_id]++;
    }

    for (int i = 0; i < NR_DPUS; i++)
    {
        coo_kvs = push_package_coo_get(&(packages[i]));
        if (coo_kvs)
        {
            coo_kvs->nnz = nnz[i];
        }
    }
}

void dpu_push_build_package_primary_index_lookup(dpu_push_info *push_info, PRIMARY_INDEX_ID id, uint32_t *keys, uint32_t batch_size)
{
    uint32_t nr_dpus = push_info->nr_dpus;
    int32_t dpu_id;

    push_package *packages = push_info->packages;
    coo_matrix_elem *coo_data[NR_DPUS];
    uint32_t nnz[NR_DPUS];
    uint32_t capility[NR_DPUS];
    query_param *query;

    dpu_push_info_reset(push_info);

    for (int i = 0; i < NR_DPUS; i++)
    {
        query = push_package_query_get(&(packages[i]));
        query->type = PRIMARY_INDEX_LOOKUP;
        query->primary_index_id = id;
    }

    coo_matrix coo_keys;
    for (int i = 0; i < NR_DPUS; i++)
    {
        coo_keys = push_package_coo_init(&(packages[i]));
        if (coo_keys)
        {
            coo_data[i] = coo_keys->data;
            nnz[i] = 0;
            capility[i] = coo_keys->capacity;
        }
        else
        {
            printf("error: dpu_push_build_package_primary_index_lookup\n");
            return;
        }
    }
    for (int i = 0; i < batch_size; i++)
    {
        dpu_id = keys[i] % NR_DPUS;
        coo_data[dpu_id][nnz[dpu_id]].row = i;
        coo_data[dpu_id][nnz[dpu_id]].col = keys[i];
        nnz[dpu_id]++;
    }

    for (int i = 0; i < NR_DPUS; i++)
    {
        coo_keys = push_package_coo_get(&(packages[i]));
        if (coo_keys)
        {
            coo_keys->nnz = nnz[i];
        }
    }
}

void dpu_push_package(dpu_push_info *push_info, struct dpu_set_t dpu_set)
{
    push_package *packages = push_info->packages;
    uint32_t nr_dpus = push_info->nr_dpus;
    uint32_t max_package_size = 0;
    uint32_t package_size;

    for (int i = 0; i < nr_dpus; i++)
    {
        package_size = push_package_size(&(packages[i]));
        if (package_size > max_package_size)
        {
            max_package_size = package_size;
        }
    }
    printf("max_package_size: %u\n", max_package_size);
    struct dpu_set_t dpu;
    uint32_t each_dpu;

    DPU_FOREACH(dpu_set, dpu, each_dpu)
    {
        DPU_ASSERT(dpu_prepare_xfer(dpu, &(packages[each_dpu])));
    }
    DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, PUSH_PACKAGE_OFFSET,
                             align8(max_package_size), DPU_XFER_DEFAULT));

    // push_package_int_steam_dump(&(push_info->packages[0]));
}
