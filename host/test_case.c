#include "test_case.h"

void test_primary_index()
{
    dpu_set_context *dpu_set;
    uint32_t batch_size = 64 * 1024;
    uint32_t kv_num = 1024 * 1024;
    uint32_t *keys = malloc(kv_num * sizeof(uint32_t));
    uint64_t *vals = malloc(kv_num * sizeof(uint64_t));
    uint32_t primary_index_id = 0;
    uint64_t *result_vals;
    struct timeval begin, end, mid;

    dpu_set = dpu_set_context_create();

    // generate kvs
    for (int i = 0; i < kv_num; i++)
    {
        keys[i] = i;
        vals[i] = i * 100;
    }

    primary_index_create(dpu_set, primary_index_id);
    for (int i = 0; i < kv_num; i = i + batch_size)
    {
        primary_index_batch_insert(dpu_set, primary_index_id, keys + i, vals + i, batch_size);
    }

    gettimeofday(&begin, NULL);
    primary_index_batch_lookup(dpu_set, primary_index_id, keys, batch_size, &result_vals);
    gettimeofday(&end, NULL);
    printf("sum time: %d ns\n", (end.tv_sec * 1000000 + end.tv_usec) - (begin.tv_sec * 1000000 + begin.tv_usec));
    for (int i = 0; i < 10; i++)
    {
        printf("%ld\n", result_vals[i]);
    }
}