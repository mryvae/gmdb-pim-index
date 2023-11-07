#include "test_case.h"

typedef struct _test_key
{
    char buf[16];
} test_key;

void test_primary_index()
{
    dpu_set_context *dpu_set;
    uint32_t batch_size = 64 * 1024;
    uint32_t kv_num = 1024 * 1024;
    test_key *keys_t = calloc(kv_num, sizeof(test_key));
    uint64_t *vals = malloc(kv_num * sizeof(uint64_t));
    char **keys = malloc(kv_num * sizeof(char *));
    uint32_t primary_index_id = 0;
    uint64_t *result_vals = malloc(batch_size * sizeof(uint64_t));
    struct timeval begin, end, mid;

    dpu_set = dpu_set_context_create();

    // generate kvs
    for (int i = 0; i < kv_num; i++)
    {
        itoa_t(i, keys_t[i].buf);
        keys[i] = keys_t[i].buf;
        vals[i] = i * 100;
    }

    primary_index_create(dpu_set, primary_index_id);
    for (int i = 0; i < kv_num; i = i + batch_size)
    {
        primary_index_batch_insert(dpu_set, primary_index_id, keys + i, 9, vals + i, batch_size);
    }

    gettimeofday(&begin, NULL);
    primary_index_batch_lookup(dpu_set, primary_index_id, keys, 9, batch_size, result_vals);
    gettimeofday(&end, NULL);
    printf("sum time: %d ns\n", (end.tv_sec * 1000000 + end.tv_usec) - (begin.tv_sec * 1000000 + begin.tv_usec));
    for (int i = 0; i < 10; i++)
    {
        printf("%ld\n", result_vals[i]);
    }

    primary_index_id++;
    for (int i = 0; i < kv_num; i++)
    {
        vals[i] = i * 100 + 1;
    }
    primary_index_create(dpu_set, primary_index_id);
    for (int i = 0; i < kv_num; i = i + batch_size)
    {
        primary_index_batch_insert(dpu_set, primary_index_id, keys + i, 9, vals + i, batch_size);
    }

    gettimeofday(&begin, NULL);
    primary_index_batch_lookup(dpu_set, primary_index_id, keys, 9, batch_size, result_vals);
    for (int i = 0; i < 10; i++)
    {
        printf("%ld\n", result_vals[i]);
    }
    primary_index_batch_delete(dpu_set, primary_index_id, keys + 4, 9, batch_size);
    primary_index_batch_lookup(dpu_set, primary_index_id, keys, 9, batch_size, result_vals);
    gettimeofday(&end, NULL);
    printf("sum time: %d ns\n", (end.tv_sec * 1000000 + end.tv_usec) - (begin.tv_sec * 1000000 + begin.tv_usec));
    for (int i = 0; i < 10; i++)
    {
        printf("%ld\n", result_vals[i]);
    }
}

void test_dpu_index_manager()
{
    uint32_t batch_size = 64 * 1024;
    uint32_t kv_num = 1024 * 1024;
    test_key *keys_t = calloc(kv_num, sizeof(test_key));
    uint64_t *vals = malloc(kv_num * sizeof(uint64_t));
    char **keys = malloc(kv_num * sizeof(char *));
    uint32_t primary_index_id;
    uint64_t *result_vals = malloc(batch_size * sizeof(uint64_t));
    dpu_index_manager_init();

    for (int k = 0; k < 60; k++)
    {
        // generate kvs
        for (int i = 0; i < kv_num; i++)
        {
            itoa_t(i, keys_t[i].buf);
            keys[i] = keys_t[i].buf;
            vals[i] = i * 100 + k;
        }
        primary_index_id = k;
        primary_index_create_v1(primary_index_id);
        printf("primary_index_id: %d\n", primary_index_id);
        for (int i = 0; i < kv_num; i = i + batch_size)
        {
            primary_index_batch_insert_v1(primary_index_id, keys + i, 9, vals + i, batch_size);
        }

        primary_index_batch_lookup_v1(primary_index_id, keys, 9, batch_size, result_vals);
        for (int i = 0; i < 10; i++)
        {
            printf("%ld\n", result_vals[i]);
        }
        primary_index_batch_lookup_v1(primary_index_id, keys, 9, batch_size, result_vals);
        for (int i = 0; i < 10; i++)
        {
            printf("%ld\n", result_vals[i]);
        }
    }

    for (int k = 0; k < 60; k++)
    {
        primary_index_id = k;

        primary_index_batch_lookup_v1(primary_index_id, keys, 9, batch_size, result_vals);
        for (int i = 0; i < 10; i++)
        {
            printf("%ld\n", result_vals[i]);
        }
    }
}