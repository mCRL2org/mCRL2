#include <stdio.h>
#include <stdlib.h>

#include <lace.h>

VOID_TASK_DECL_1(test_together, int);
VOID_TASK_DECL_1(test_newframe, int);

VOID_TASK_IMPL_1(test_together, int, depth)
{
    if (depth != 0) {
        SPAWN(test_together, depth-1);
        SPAWN(test_together, depth-1);
        SPAWN(test_together, depth-1);
        SPAWN(test_together, depth-1);
        NEWFRAME(test_newframe, depth-1);
        SYNC(test_together);
        SYNC(test_together);
        SYNC(test_together);
        SYNC(test_together);
    }
}

VOID_TASK_IMPL_1(test_newframe, int, depth)
{
    if (depth != 0) {
        SPAWN(test_newframe, depth-1);
        SPAWN(test_newframe, depth-1);
        SPAWN(test_newframe, depth-1);
        SPAWN(test_newframe, depth-1);
        TOGETHER(test_together, depth-1);
        SYNC(test_newframe);
        SYNC(test_newframe);
        SYNC(test_newframe);
        SYNC(test_newframe);
    }
}

VOID_TASK_0(test_something)
{
    printf("running from worker %d\n", LACE_WORKER_ID);
}

VOID_TASK_1(_main, void*, arg)
{
    fprintf(stdout, "Testing TOGETHER and NEWFRAME with %u workers...\n", lace_workers());

    for (int i=0; i<10; i++) {
        NEWFRAME(test_newframe, 5);
        TOGETHER(test_together, 5);
    }

    RUN(test_something);

    // We didn't use arg
    (void)arg;
}

void
runtests(int n_workers)
{
    // Initialize the Lace framework for <n_workers> workers.
    lace_start(n_workers, 0);

    printf("Newframe:\n");
    NEWFRAME(test_something);

    printf("Together:\n");
    TOGETHER(test_something);

    lace_suspend();
    lace_resume();

    // Spawn and start all worker pthreads; suspends current thread until done.
    printf("Running (10x):\n");
    for (int i=0; i<10; i++) {
        printf("%d: ", i);
        RUN(test_something);
    }

    // Spawn and start all worker pthreads; suspends current thread until done.
    printf("Recursive test\n");
    RUN(_main, NULL);

    // The lace_startup command also exits Lace after _main is completed.
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

    for (int i=0; i<10; i++) {
        runtests(n_workers);
    }

    return 0;
}
