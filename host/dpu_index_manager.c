#include "dpu_index_manager.h"

static dpu_index_manager *_dpu_index_manager = NULL;

void dpu_index_manager_init()
{
    dpu_set_pool_init(4);
    _dpu_index_manager = malloc(sizeof(dpu_index_manager));
    _dpu_index_manager->global_nr_pre_load_index = 0;
    pthread_mutex_init(&(_dpu_index_manager->lock), NULL);
}

PRIMARY_INDEX_ID dpu_index_manager_create_index()
{
    PRIMARY_INDEX_ID id = -1;
    dpu_set_context *dpu_set = NULL;

    pthread_mutex_lock(&(_dpu_index_manager->lock));
    dpu_set = dpu_set_apply();
    if (dpu_set)
    {
        if (_dpu_index_manager->global_nr_pre_load_index < MAX_NUM_PRE_LOAD_PRIMARY_INDEX)
        {
            id = _dpu_index_manager->global_nr_pre_load_index;
            _dpu_index_manager->global_nr_pre_load_index++;

            _dpu_index_manager->map[id] = dpu_set;
        }
    }
    pthread_mutex_unlock(&(_dpu_index_manager->lock));
    return id;
}

dpu_set_context *dpu_index_manager_get_dpu_set(PRIMARY_INDEX_ID id)
{
    dpu_set_context *dpu_set = NULL;
    pthread_mutex_lock(&(_dpu_index_manager->lock));
    if (id >= 0 && id < _dpu_index_manager->global_nr_pre_load_index)
    {
        dpu_set = _dpu_index_manager->map[id];
    }
    pthread_mutex_unlock(&(_dpu_index_manager->lock));
    return dpu_set;
}