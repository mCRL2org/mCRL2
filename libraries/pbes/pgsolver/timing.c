#include "timing.h"
#include <sys/time.h>
#include <stdlib.h>

static double time_limit;

static struct timeval tv_start;
static double time_secs_waited = 0;
static double time_pause_start = -1;

static struct timeval tv_now()
{
    struct timeval tv;
    (void)gettimeofday(&tv, NULL);
    return tv;
}

static double now()
{
    struct timeval tv = tv_now();
    return (tv.tv_sec - tv_start.tv_sec) + 1e-6*(tv.tv_usec - tv_start.tv_usec);
}

void time_initialize(double tl)
{
    time_limit = tl;
    tv_start = tv_now();
}

double time_used()
{
    return now() - time_secs_waited;
}

void time_pause()
{
    time_pause_start = now();
}

double time_resume()
{
    if (time_pause_start == -1) return 0;

    double waited = now() - time_pause_start;
    time_secs_waited += waited;
    time_pause_start = -1;

    return waited;
}
