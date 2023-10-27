#ifndef _DPU_TIMER_H
#define _DPU_TIMER_H

#include <stdint.h>
#include <stddef.h>
#include <sys/time.h>
#include <stdlib.h>

int64_t usec(void);

typedef struct _dpu_timer
{
    int64_t load_balance_interval;
    int64_t push_interval;
    int64_t run_interval;
    int64_t pull_interval;
    int64_t merge_interval;
    int64_t sum_interval;
} dpu_timer;

void dpu_timer_init(dpu_timer *timer);
void dpu_timer_reset(dpu_timer *timer);

#endif