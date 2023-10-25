#ifndef _MRAM_MEMORY_H
#define _MRAM_MEMORY_H
#include <mram.h>

#define NR_SLAVE_TASKLETS (NR_TASKLETS - 1)

#define PUSH_PACKAGE_OFFSET 0x0
#define COO_RESULT_OFFSET 0x200000
#define COO_RESULT_SIZE 0x200000
#define PULL_PACKAGE_METADATA_OFFSET (COO_RESULT_OFFSET + COO_RESULT_SIZE)
#define PULL_PACKAGE_METADATA_SIZE 0x000020

#define MAX_NUM_PRE_LOAD_PRIMARY_INDEX 16

#define PRIMARY_INDEX_SPACE_ADDR0 ((__mram_ptr void *)(0x2000000))
#define PRIMARY_INDEX_SPACE_ADDR1 ((__mram_ptr void *)(0x2100000))
#define PRIMARY_INDEX_SPACE_ADDR2 ((__mram_ptr void *)(0x2200000))
#define PRIMARY_INDEX_SPACE_ADDR3 ((__mram_ptr void *)(0x2300000))
#define PRIMARY_INDEX_SPACE_ADDR4 ((__mram_ptr void *)(0x2400000))
#define PRIMARY_INDEX_SPACE_ADDR5 ((__mram_ptr void *)(0x2500000))
#define PRIMARY_INDEX_SPACE_ADDR6 ((__mram_ptr void *)(0x2600000))
#define PRIMARY_INDEX_SPACE_ADDR7 ((__mram_ptr void *)(0x2700000))
#define PRIMARY_INDEX_SPACE_ADDR8 ((__mram_ptr void *)(0x2800000))
#define PRIMARY_INDEX_SPACE_ADDR9 ((__mram_ptr void *)(0x2900000))
#define PRIMARY_INDEX_SPACE_ADDR10 ((__mram_ptr void *)(0x2a00000))
#define PRIMARY_INDEX_SPACE_ADDR11 ((__mram_ptr void *)(0x2b00000))
#define PRIMARY_INDEX_SPACE_ADDR12 ((__mram_ptr void *)(0x2c00000))
#define PRIMARY_INDEX_SPACE_ADDR13 ((__mram_ptr void *)(0x2d00000))
#define PRIMARY_INDEX_SPACE_ADDR14 ((__mram_ptr void *)(0x2e00000))
#define PRIMARY_INDEX_SPACE_ADDR15 ((__mram_ptr void *)(0x2f00000))

#define INDEX_ENTRY_BLOCKS_SPACE_ADDR ((__mram_ptr void *)(0x3000000))
#define INDEX_ENTRY_BLOCKS_SIZE (0x800000)

#endif