#include "push_package_dpu.h"

kv_set_dpu push_package_dpu_kv_get()
{
    if (global_package->kv_offset < 0)
    {
        // printf("error: push_package_coo_get coo hasn't been initialized!\n");
        return NULL;
    }
    kv_set_dpu kvsd = (kv_set_dpu)((global_package->buf + global_package->kv_offset));
    return kvsd;
}

void push_package_dpu_kv_dump()
{
    kv_set_dpu kvsd = push_package_dpu_kv_get();
    printf("kv_set_dpu: \n");
    if (kvsd)
    {
        kv_set_dpu_dump(kvsd);
    }
}

void push_package_dpu_int_steam_dump()
{
    uint32_t size = 224;
    __mram_ptr int *p = (__mram_ptr int *)global_package;
    for (int i = 0; i < size / sizeof(int); i++)
    {
        printf("%d ", p[i]);
    }
    printf("\n");
}