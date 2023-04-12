#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <lace.h>

// simple workload

long sfib(int n)
{
    if (n<2) return n;
    return sfib(n-2) + sfib(n-1);
}

TASK_1(int, pfib, int, n)
{
    if (n<2) return n;
    int m,k;
    SPAWN(pfib, n-1);
    k = CALL(pfib, n-2);
    m = SYNC(pfib);
    return m+k;
}

double wctime() 
{
    struct timespec tv;
    clock_gettime(CLOCK_MONOTONIC, &tv);
    return (tv.tv_sec + 1E-9 * tv.tv_nsec);
}

void
runtests(int n_workers)
{
    // first run sfib a few times
    for (int i=0; i<10; i++) sfib(35);

    // Initialize the Lace framework for <n_workers> workers.
    lace_start(n_workers, 0);

    double time = 0;

    for (int i=0; i<10; i++) {
        RUN(pfib, 35);
        double before = wctime();
        lace_suspend();
        time += wctime() - before;
        for (int i=0; i<10; i++) sfib(30);
        before = wctime();
        lace_resume();
        time += wctime() - before;
        RUN(pfib, 35);
    }

    printf("Time suspend + resume avg: %f sec\n", time/10);

    lace_stop();
}

int
main (int argc, char *argv[])
{
    int n_workers = 0; // automatically detect number of workers

    if (argc > 1) {
        n_workers = atoi(argv[1]);
    }

    lace_set_verbosity(1);

    for (int i=0; i<=n_workers; i++) {
        runtests(i);
    }

    return 0;
}
