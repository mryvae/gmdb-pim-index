#ifndef __HASH_MAP_H
#define __HASH_MAP_H

#include <stdint.h>
#include <malloc.h>
#include "../util/util.h"

#define BUCKETS_INITIAL_SIZE (128)

#define HASH_MAP_OK 0
#define HASH_MAP_ERR 1

typedef struct _entry
{
    int32_t key;
    uint64_t val;
    int32_t flag;
    struct _entry *next;
} entry;

typedef struct _hash_map
{
    uint32_t buckets_size;
    uint32_t sizemask;
    uint32_t used;
    entry *buckets;
} hash_map;

hash_map *hash_map_new(uint32_t buckets_size);
int hash_map_insert(hash_map *map,int32_t key,uint64_t val);
entry *hash_map_find(hash_map *map, int32_t key);

void hash_map_test(void);

#endif