#include "dpu_timer.h"

int64_t usec(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (((int64_t)tv.tv_sec) * 1000000) + tv.tv_usec;
}

void dpu_timer_init(dpu_timer *timer)
{
    timer->load_balance_interval = 0;
    timer->push_interval = 0;
    timer->run_interval = 0;
    timer->pull_interval = 0;
    timer->merge_interval = 0;
    timer->sum_interval = 0;
}

void dpu_timer_reset(dpu_timer *timer)
{
    dpu_timer_init(timer);
}

void dpu_timer_dump(dpu_timer *timer)
{
    printf("load_balance_interval: %ld, push_interval: %ld, run_interval: %ld, ",
            timer->load_balance_interval, timer->push_interval, timer->run_interval);

    printf("pull_interval: %ld, merge_interval: %ld\n sum_interval: %ld\n",
            timer->pull_interval, timer->merge_interval, timer->sum_interval);
}