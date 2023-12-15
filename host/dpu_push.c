#include "dpu_push.h"

static inline unsigned int _hash_function_int(unsigned int key)
{
    key += ~(key << 15);
    key ^= (key >> 10);
    key += (key << 3);
    key ^= (key >> 6);
    key += ~(key << 11);
    key ^= (key >> 16);
    return key;
}

static inline unsigned int _hash_function(char *buf, int len)
{
    unsigned int hash = 5381;

    while (len--)
        hash = ((hash << 5) + hash) + (*buf++); /* hash * 33 + c */
    return _hash_function_int(hash);
    // XXH32_hash_t const seed = 0;
    // XXH32_hash_t hash = XXH32(buf, len, seed);
    // return (unsigned int)hash;
}

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

void dpu_push_build_package_primary_index_insert(dpu_push_info *push_info, PRIMARY_INDEX_ID id, char **keys, uint32_t key_len, uint64_t *vals, uint32_t batch_size)
{
    uint32_t nr_dpus = push_info->nr_dpus;
    int32_t dpu_id;

    push_package *packages = push_info->packages;
    kv_elem *kv_data[NR_DPUS];
    uint32_t nnz[NR_DPUS];
    uint32_t capility[NR_DPUS];
    query_param *query;

    dpu_push_info_reset(push_info);

    for (int i = 0; i < NR_DPUS; i++)
    {
        query = push_package_query_get(&(packages[i]));
        query->type = PRIMARY_INDEX_INSERT;
        query->primary_index_id = id;
        query->key_len = key_len;
    }

    kv_set kvs;
    for (int i = 0; i < NR_DPUS; i++)
    {
        kvs = push_package_kv_init(&(packages[i]));
        if (kvs)
        {
            kv_data[i] = kvs->data;
            nnz[i] = 0;
            capility[i] = kvs->capacity;
        }
        else
        {
            printf("error: dpu_push_build_package_primary_index_insert\n");
            return;
        }
    }
    for (int i = 0; i < batch_size; i++)
    {
        dpu_id = _hash_function(keys[i], key_len) % NR_DPUS;
        memcpy(kv_data[dpu_id][nnz[dpu_id]].key, keys[i], key_len);
        kv_data[dpu_id][nnz[dpu_id]].val = vals[i];
        nnz[dpu_id]++;
    }

    for (int i = 0; i < NR_DPUS; i++)
    {
        kvs = push_package_kv_get(&(packages[i]));
        if (kvs)
        {
            kvs->nnz = nnz[i];
        }
    }
}

void dpu_push_build_package_primary_index_delete(dpu_push_info *push_info, PRIMARY_INDEX_ID id, char **keys, uint32_t key_len, uint32_t batch_size)
{
    uint32_t nr_dpus = push_info->nr_dpus;
    int32_t dpu_id;

    push_package *packages = push_info->packages;
    kv_elem *kv_data[NR_DPUS];
    uint32_t nnz[NR_DPUS];
    uint32_t capility[NR_DPUS];
    query_param *query;

    dpu_push_info_reset(push_info);

    for (int i = 0; i < NR_DPUS; i++)
    {
        query = push_package_query_get(&(packages[i]));
        query->type = PRIMARY_INDEX_DELETE;
        query->primary_index_id = id;
        query->key_len = key_len;
    }

    kv_set kvs;
    for (int i = 0; i < NR_DPUS; i++)
    {
        kvs = push_package_kv_init(&(packages[i]));
        if (kvs)
        {
            kv_data[i] = kvs->data;
            nnz[i] = 0;
            capility[i] = kvs->capacity;
        }
        else
        {
            printf("error: dpu_push_build_package_primary_index_insert\n");
            return;
        }
    }
    for (int i = 0; i < batch_size; i++)
    {
        dpu_id = _hash_function(keys[i], key_len) % NR_DPUS;
        memcpy(kv_data[dpu_id][nnz[dpu_id]].key, keys[i], key_len);
        nnz[dpu_id]++;
    }

    for (int i = 0; i < NR_DPUS; i++)
    {
        kvs = push_package_kv_get(&(packages[i]));
        if (kvs)
        {
            kvs->nnz = nnz[i];
        }
    }
}

void dpu_push_build_package_primary_index_lookup(dpu_push_info *push_info, PRIMARY_INDEX_ID id, char **keys, uint32_t key_len, uint32_t batch_size)
{
    uint32_t nr_dpus = push_info->nr_dpus;
    int32_t dpu_id;

    push_package *packages = push_info->packages;
    kv_elem *kv_data[NR_DPUS];
    uint32_t nnz[NR_DPUS];
    uint32_t capility[NR_DPUS];
    query_param *query;

    dpu_push_info_reset(push_info);

    for (int i = 0; i < NR_DPUS; i++)
    {
        query = push_package_query_get(&(packages[i]));
        query->type = PRIMARY_INDEX_LOOKUP;
        query->primary_index_id = id;
        query->key_len = key_len;
    }

    kv_set kvs;
    for (int i = 0; i < NR_DPUS; i++)
    {
        kvs = push_package_kv_init(&(packages[i]));
        if (kvs)
        {
            kv_data[i] = kvs->data;
            nnz[i] = 0;
            capility[i] = kvs->capacity;
        }
        else
        {
            printf("error: dpu_push_build_package_primary_index_lookup\n");
            return;
        }
    }
    for (int i = 0; i < batch_size; i++)
    {
        dpu_id = _hash_function(keys[i], key_len) % NR_DPUS;
        kv_data[dpu_id][nnz[dpu_id]].key_id = i;
        memcpy(kv_data[dpu_id][nnz[dpu_id]].key, keys[i], key_len);
        nnz[dpu_id]++;
    }

    for (int i = 0; i < NR_DPUS; i++)
    {
        printf("%d ", nnz[i]);
        kvs = push_package_kv_get(&(packages[i]));
        if (kvs)
        {
            kvs->nnz = nnz[i];
        }
    }
    printf("\n");
}

void dpu_push_package(dpu_push_info *push_info, XDPI interface)
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

    uint8_t *buffers[NR_DPUS];
    for (int i = 0; i < NR_DPUS; i++)
    {
        buffers[i] = &(packages[i]);
    }
    SendToPIM(interface, buffers, DPU_MRAM_HEAP_POINTER_NAME, PUSH_PACKAGE_OFFSET, align8(max_package_size), 0);
    
    // struct dpu_set_t dpu;
    // uint32_t each_dpu;

    // DPU_FOREACH(dpu_set, dpu, each_dpu)
    // {
    //     DPU_ASSERT(dpu_prepare_xfer(dpu, &(packages[each_dpu])));
    // }
    // DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, PUSH_PACKAGE_OFFSET,
    //                          align8(max_package_size), DPU_XFER_DEFAULT));

    // push_package_int_steam_dump(&(push_info->packages[0]));
}
