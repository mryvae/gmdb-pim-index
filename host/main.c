#include "test_case.h"
#include "../upmemInterface/upmem_direct_c.h"
#include "dpu_info.h"

int main()
{
    // XDPI interface;
    // allocate(interface, 1, DPU_BINARY);
    // test_primary_index();
    test_dpu_index_manager();
    return 0;
}