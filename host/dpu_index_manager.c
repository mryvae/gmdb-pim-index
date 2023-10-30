#include "dpu_index_manager.h"

static dpu_index_manager *_dpu_index_manager = NULL;

void dpu_index_manager_init()
{
    dpu_set_pool_init(4);
    _dpu_index_manager = malloc(sizeof(dpu_index_manager));
    _dpu_index_manager->global_nr_pre_load_index = 0;
    _dpu_index_manager->dpu_set_map = hash_map_new(128);
    pthread_mutex_init(&(_dpu_index_manager->lock), NULL);
}

int dpu_index_manager_create_index(PRIMARY_INDEX_ID id)
{
    dpu_set_context *dpu_set = NULL;
    int ret = DPU_INDEX_MANAGER_ERR;

    pthread_mutex_lock(&(_dpu_index_manager->lock));
    dpu_set = dpu_set_apply();
    if (dpu_set)
    {
        if (_dpu_index_manager->global_nr_pre_load_index < MAX_NUM_PRE_LOAD_PRIMARY_INDEX)
        {
            if (hash_map_insert(_dpu_index_manager->dpu_set_map, id, dpu_set) == HASH_MAP_OK)
            {
                _dpu_index_manager->global_nr_pre_load_index++;
                ret = DPU_INDEX_MANAGER_OK;
            }
        }
    }
    pthread_mutex_unlock(&(_dpu_index_manager->lock));
    return ret;
}

dpu_set_context *dpu_index_manager_get_dpu_set(PRIMARY_INDEX_ID id)
{
    dpu_set_context *dpu_set = NULL;
    entry *en;
    pthread_mutex_lock(&(_dpu_index_manager->lock));
    en = hash_map_find(_dpu_index_manager->dpu_set_map, id);
    if (en)
    {
        dpu_set = (dpu_set_context *)(en->val);
    }
    pthread_mutex_unlock(&(_dpu_index_manager->lock));
    return dpu_set;
}