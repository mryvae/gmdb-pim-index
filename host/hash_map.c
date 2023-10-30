#include "hash_map.h"

static inline uint32_t hash_function(uint32_t key)
{
    /*
        other hash function is also acceptable
    */
    key += ~(key << 15);
    key ^= (key >> 10);
    key += (key << 3);
    key ^= (key >> 6);
    key += ~(key << 11);
    key ^= (key >> 16);
    return key;
}

static void entry_init(entry *entry_ptr)
{
    entry_ptr->key = -1;
    entry_ptr->next = NULL;
    entry_ptr->val = 0;
    entry_ptr->flag = 0;
}

hash_map *hash_map_new(uint32_t buckets_size)
{
    hash_map *map = malloc(sizeof(hash_map));
    map->used = 0;
    map->buckets_size = next_power(buckets_size);
    map->buckets = malloc(sizeof(entry) * map->buckets_size);
    for (int i = 0; i < map->buckets_size; i++)
    {
        entry_init(&(map->buckets[i]));
    }
    map->sizemask = map->buckets_size - 1;
    return map;
}

int hash_map_insert(hash_map *map, int32_t key, uint64_t val)
{
    entry *en = hash_map_find(map, key);
    if (en)
    {
        return HASH_MAP_ERR;
    }
    int32_t bucket_id = hash_function(key) & map->sizemask, flag = 0;

    entry *cur = map->buckets + bucket_id;

    if (!cur->flag)
    {
        cur->key = key;
        cur->val = val;
        cur->flag = 1;
        cur->next = NULL;
    }
    else
    {
        entry *new_entry = malloc(sizeof(entry));
        new_entry->key = key;
        new_entry->val = val;
        new_entry->flag = 1;
        new_entry->next = cur->next;
        cur->next = new_entry;
    }
    map->used++;
    return HASH_MAP_OK;
}
entry *hash_map_find(hash_map *map, int32_t key)
{
    int32_t bucket_id = hash_function(key) & map->sizemask;

    entry *last = map->buckets + bucket_id, *res = NULL;

    while (last && last->flag && last->key != key)
    {
        last = last->next;
    }

    if (last && last->flag && last->key == key)
    {
        res = last;
    }

    return res;
}

void hash_map_test()
{
    hash_map *map = hash_map_new(128);
    int32_t key;
    uint64_t val;
    entry *en;

    key = 0, val = 0;
    hash_map_insert(map, key, val);

    key = 1, val = 1;
    hash_map_insert(map, key, val);

    key = 2, val = 2;
    hash_map_insert(map, key, val);
    key = 3, val = 3;
    hash_map_insert(map, key, val);

    key = 2, val = 100;
    hash_map_insert(map, key, val);
    key = 3, val = 100;
    hash_map_insert(map, key, val);

    for (int i = 0; i < 5; i++)
    {
        en = hash_map_find(map, i);
        if (en)
        {
            printf("key: %d, val: %ld\n", en->key, en->val);
        }
    }
}