#ifndef __PARAM_H
#define __PARAM_H
#include <stdint.h>

typedef uint32_t PRIMARY_INDEX_ID;
typedef uint32_t opType;
#define PRIMARY_INDEX_LOOKUP 1 
#define PRIMARY_INDEX_INSERT 2 
#define PRIMARY_INDEX_DELETE 3 
#define PRIMARY_INDEX_CREATE 4

typedef struct query_param
{
    opType type;
    PRIMARY_INDEX_ID primary_index_id;
} query_param;

#endif