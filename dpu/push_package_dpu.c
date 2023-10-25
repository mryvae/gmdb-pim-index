#include "push_package_dpu.h"

coo_matrix_dpu push_package_dpu_coo_get()
{
    if (global_package->coo_offset < 0)
    {
        // printf("error: push_package_coo_get coo hasn't been initialized!\n");
        return NULL;
    }
    coo_matrix_dpu matrix = (coo_matrix_dpu)((global_package->buf + global_package->coo_offset));
    return matrix;
}

void push_package_dpu_coo_dump()
{
    coo_matrix_dpu coo = push_package_dpu_coo_get();
    printf("matrix coo: \n");
    if (coo)
    {
        coo_matrix_dpu_dump(coo);
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