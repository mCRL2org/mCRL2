Lace [![CI](https://github.com/trolando/lace/actions/workflows/ci.yml/badge.svg)](https://github.com/trolando/lace/actions/workflows/ci.yml)
======
Lace is a C framework for fine-grained fork-join parallelism intended for scientific computations on multi-core computers.

```c
TASK_1(int, fibonacci, int, n) {
    if(n < 2) return n;
    SPAWN(fibonacci, n-1);
    int a = CALL(fibonacci, n-2);
    int b = SYNC(fibonacci);
    return a+b;
}

int main(int argc, char **argv)
{
    int n_workers = 4;
    lace_start(n_workers, 0);
    int result = RUN(fibonacci, 42);
    printf("fibonacci(42) = %d\n", result);
    lace_stop();
}
```

## Description


Feature | Description
---------|------------
Low overhead | Lace uses a **scalable** double-ended queue for its implementation of work-stealing, which is **wait-free** for the thread spawning tasks and **lock-free** for the threads stealing tasks. The design of the datastructure minimizes interaction between CPUs.
Suspending | Lace threads can be manually suspended when the framework is not used to reduce CPU usage. This is used when part of a computation is not parallelized, since Lace workers busy-wait for work.
Interrupting | Lace threads can be (cooperatively) interrupted to execute another task first. This is for example used by [Sylvan](https://github.com/trolando/sylvan) to perform garbage collection.

Lace is licensed with the Apache 2.0 license.

The main repository of Lace is https://github.com/trolando/lace.
The main author of Lace is Tom van Dijk who can be reached via <tom@tvandijk.nl>.
Please let us know if you use Lace in your projects and if you need
features that are currently not implemented in Lace.

## Dependencies

Lace requires a modern compiler supporting C11.
Optionally, Lace can use hwloc (`libhwloc-dev`) to pin workers and allocate memory on the correct CPUs/memory domains on NUMA systems.

## Building Lace

It is recommended to build Lace in a separate build directory:
```bash
mkdir build
cd build
cmake ..
make
```

Lace is typically used as a subproject, for example using the FetchContent or ExternalProject feature of CMake.

Lace can be configured with the following CMake settings:
Setting | Description
--------|------------
`LACE_BUILD_TESTS` | Build the testing programs (not when subproject)
`LACE_BUILD_BENCHMARKS` | Build the included set of benchmark programs (not when subproject)
`LACE_USE_MMAP` | Use `mmap` to allocate memory instead of `posix_memalign`
`LACE_USE_HWLOC` | Use the `hwloc` library to pin threads to CPUs
`LACE_COUNT_TASKS` | Let Lace record the number of executed tasks
`LACE_COUNT_STEALS` | Let Lace count how often tasks were stolen
`LACE_COUNT_SPLITS` | Let Lace count how often the queue split point was moved
`LACE_PIE_TIMES` | Let Lace record precise overhead times

Ideally, `LACE_USE_MMAP` is set to let Lace allocate a large amount of virtual memory for the task queues instead of real memory. Real memory is only allocated by the OS when required, thus in most use cases this minimizes the memory overhead of Lace. If `LACE_USE_MMAP` is not set, then real memory is allocated using `posix_memalign`, and a more conservative queue size should be chosen when invoking `lace_start`.

There are two versions of Lace:
- The standard version `lace` consisting of `lace.h` and `lace.c` uses 64 bytes per task and supports at most 6 parameters per task.
- The extended version `lace14` consisting of `lace14.h` and `lace14.c` uses 128 bytes per task and supports at most 14 parameters per task.

## Using Lace

### Starting and stopping Lace
Start the Lace framework using the `lace_start(unsigned int n_workers, size_t dqsize)` method.
This creates `n_workers` new threads that will immediately start busy-waiting for work. Each threads will allocate its own task queue for `dqsize` tasks. The entire queue is preallocated, requiring 64 bytes per tasks (or 128 bytes for `lace14`).
* When `n_workers` is set to 0, Lace automatically detects the maximum number of workers for the system using `lace_get_pu_count()`.
* When `dqsize` is set to 0, the default is used, which is currently 100000 tasks.

Use `lace_stop()` to stop the framework, terminating all workers.

Lace workers busy-wait for tasks to steal, increasing the CPU load to 100%.
Use `lace_suspend` and `lace_resume` from non-Lace threads to temporarily stop the work-stealing framework.

Calls to `lace_start`, `lace_suspend`, and `lace_resume` do not incur much overhead.
Suspending and resuming typically requires at most 1-2 ms.

### Defining tasks

Lace tasks are defined using the `TASK_n` macro, where `n` is the number of parameters.
For example, `TASK_1(int, fib, int, n) { ... }` defines a Lace task with an int return value and one parameter of type int and variable name n.
Declaration and implementation can be separated using the `TASK_DECL_n` and `TASK_IMPL_n` macros.
To declare tasks with no return value, use the `VOID_TASK_n` macros, for example, `VOID_TASK_1(do_something, int, n)`.

From Lace tasks (running in a Lace thread):
- Use `SPAWN` to create a task and `SYNC` to obtain the result (if stolen) or execute the task (if not stolen)
- Use `CALL` to directly execute a task without putting it in the queue
- Use `DROP` instead of `SYNC` to not execute a task (unless already stolen)

From external methods (not running in a Lace thread):
- Use `RUN` to offer the task to the Lace framework. This method halts until the task is fully executed

See the `benchmarks` directory for examples.

### Interrupting

Lace offers two methods to interrupt currently running tasks and run something else:
- the `NEWFRAME` macro, e.g. `NEWFRAME(fib, 40)` macro halts current tasks and offers the `fib` method to the framework.
- the `TOGETHER` macro halts current tasks and lets **all Lace workers** execute a copy of the given task.

The `TOGETHER` macro is useful to initialize thread-local variables on each worker.

Interrupting is cooperative. Lace checks for interrupting tasks when stealing work, i.e., during `SYNC` or when idle.
Large tasks can use the `YIELD_NEWFRAME()` macro to manually check for interrupting tasks.

Lace offers the `lace_barrier` method to let all Lace workers synchronize.
Typically used in Lace tasks created using the `TOGETHER` macro.

### Support for C++

There is currently no direct support for C++ classes, but class methods can be parallelized via C helper functions.

## Benchmarking Lace

Lace comes with a number of example programs, which can be used to test the performance of Lace.
Many of these benchmark programs have been obtained from benchmark collections of other frameworks such as Cilk, Wool, and Nowa.
After building Lace with `LACE_BUILD_BENCHMARKS` set to `ON`, the `build/benchmarks` directory contains the benchmarks programs, as well as the `bench.py` Python script that runs the benchmarks.

Workloads such as `matmul` and `queens` are easy to load balance.
The `fib` workload has a very high number of nearly empty tasks and is therefore a stress test on the overhead of the framework, but is not very representative for real world workloads.
The `uts t3l` is a more challenging workload as it offers a unpredictable tree search.
See for further details the academic publications on Lace mentioned below.

## Academic publications

The following two academic publications are directly related to Lace.

T. van Dijk (2016) [Sylvan: Multi-core Decision Diagrams](http://dx.doi.org/10.3990/1.9789036541602). PhD Thesis.

T. van Dijk and J.C. van de Pol (2014) [Lace: Non-blocking Split Deque for Work-Stealing](http://dx.doi.org/10.1007/978-3-319-14313-2_18). In: Euro-Par 2014: Parallel Processing Workshops. LNCS 8806, Springer.
