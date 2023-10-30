#include "primary_index_host.h"

void primary_index_create(dpu_set_context *set, PRIMARY_INDEX_ID id)
{
    pthread_mutex_lock(&(set->lock));
    dpu_push_build_package_primary_index_create(&(set->push_info), id);
    dpu_push_package(&(set->push_info), set->dpu_set);
    dpu_set_context_dpu_run(set);
    pthread_mutex_unlock(&(set->lock));
}

void primary_index_batch_insert(dpu_set_context *set, PRIMARY_INDEX_ID id, char **keys, uint32_t key_len, uint64_t *vals, uint32_t batch_size)
{
    int64_t begin, last, cur;
    pthread_mutex_lock(&(set->lock));
    dpu_timer_reset(&(set->timer));
    begin = usec();
    last = begin;
    cur = begin;
    dpu_push_build_package_primary_index_insert(&(set->push_info), id, keys, key_len, vals, batch_size);
    last = cur;
    cur = usec();
    set->timer.load_balance_interval += cur - last;
    dpu_push_package(&(set->push_info), set->dpu_set);
    last = cur;
    cur = usec();
    set->timer.push_interval += cur - last;
    dpu_set_context_dpu_run(set);
    last = cur;
    cur = usec();
    set->timer.run_interval += cur - last;
    last = cur;
    cur = usec();
    set->timer.pull_interval += cur - last;
    last = cur;
    cur = usec();
    set->timer.merge_interval += cur - last;
    set->timer.sum_interval += cur - begin;
    // dpu_set_context_log_read(set);
    pthread_mutex_unlock(&(set->lock));
}

void primary_index_batch_lookup(dpu_set_context *set, PRIMARY_INDEX_ID id, char **keys, uint32_t key_len, uint32_t batch_size, uint64_t *vals)
{
    int64_t begin, last, cur;
    pthread_mutex_lock(&(set->lock));
    dpu_timer_reset(&(set->timer));
    begin = usec();
    last = begin;
    cur = begin;
    dpu_push_build_package_primary_index_lookup(&(set->push_info), id, keys, key_len, batch_size);
    last = cur;
    cur = usec();
    set->timer.load_balance_interval += cur - last;
    dpu_push_package(&(set->push_info), set->dpu_set);
    last = cur;
    cur = usec();
    set->timer.push_interval += cur - last;
    dpu_set_context_dpu_run(set);
    last = cur;
    cur = usec();
    set->timer.run_interval += cur - last;
    // dpu_set_context_log_read(set);
    dpu_pull_coo_metadatas(&(set->pull_info), set->dpu_set);
    dpu_pull_coo_results(&(set->pull_info), set->dpu_set);
    last = cur;
    cur = usec();
    set->timer.pull_interval += cur - last;
    dpu_pull_merge_results(&(set->pull_info), vals, batch_size);
    last = cur;
    cur = usec();
    set->timer.merge_interval += cur - last;
    set->timer.sum_interval += cur - begin;
    dpu_timer_dump(&(set->timer));
    pthread_mutex_unlock(&(set->lock));
}

void primary_index_create_v1(PRIMARY_INDEX_ID id)
{
    if (dpu_index_manager_create_index(id) == DPU_INDEX_MANAGER_ERR)
    {
        return;
    }
    printf("id: %d\n", id);
    dpu_set_context *dpu_set;
    dpu_set = dpu_index_manager_get_dpu_set(id);
    if (dpu_set)
    {
        primary_index_create(dpu_set, id);
    }
}

void primary_index_batch_insert_v1(PRIMARY_INDEX_ID id, char **keys, uint32_t key_len, uint64_t *vals, uint32_t batch_size)
{
    dpu_set_context *dpu_set;
    dpu_set = dpu_index_manager_get_dpu_set(id);
    if (dpu_set)
    {
        primary_index_batch_insert(dpu_set, id, keys, key_len, vals, batch_size);
    }
}

void primary_index_batch_lookup_v1(PRIMARY_INDEX_ID id, char **keys, uint32_t key_len, uint32_t batch_size, uint64_t *vals)
{
    dpu_set_context *dpu_set;
    dpu_set = dpu_index_manager_get_dpu_set(id);

    if (dpu_set)
    {
        primary_index_batch_lookup(dpu_set, id, keys, key_len, batch_size, vals);
    }
}
