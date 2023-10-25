#include "primary_index_host.h"

void primary_index_create(dpu_set_context *set, PRIMARY_INDEX_ID id)
{
    dpu_push_build_package_primary_index_create(&(set->push_info), id);
    dpu_push_package(&(set->push_info), set->dpu_set);
    dpu_set_context_dpu_run(set);
}

void primary_index_batch_insert(dpu_set_context *set, PRIMARY_INDEX_ID id, uint32_t *keys, uint64_t *vals, uint32_t batch_size)
{
    dpu_push_build_package_primary_index_insert(&(set->push_info), id, keys, vals, batch_size);
    dpu_push_package(&(set->push_info), set->dpu_set);
    dpu_set_context_dpu_run(set);
    // dpu_set_context_log_read(set);
}

void primary_index_batch_lookup(dpu_set_context *set, PRIMARY_INDEX_ID id, uint32_t *keys, uint32_t batch_size, uint64_t **vals)
{
    dpu_push_build_package_primary_index_lookup(&(set->push_info), id, keys, batch_size);
    dpu_push_package(&(set->push_info), set->dpu_set);
    dpu_set_context_dpu_run(set);
    // dpu_set_context_log_read(set);
    dpu_pull_coo_metadatas(&(set->pull_info), set->dpu_set);
    dpu_pull_coo_results(&(set->pull_info), set->dpu_set);
    dpu_pull_merge_results(&(set->pull_info), vals, batch_size);
}
