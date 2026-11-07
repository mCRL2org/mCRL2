#! /bin/bash

nparams=$1
tasksize=$2

# Extract version from CMakeLists.txt
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
CMAKE_FILE="$SCRIPT_DIR/../CMakeLists.txt"
LACE_VERSION=$(sed -n '/^project(/,/)/{ s/.*VERSION \([0-9]*\.[0-9]*\.[0-9]*\).*/\1/p; }' "$CMAKE_FILE")
LACE_MAJOR=${LACE_VERSION%%.*}
_rest=${LACE_VERSION#*.}
LACE_MINOR=${_rest%%.*}
LACE_PATCH=${_rest#*.}

# Copyright notice:
echo "/* 
 * Copyright 2013-2016 Formal Methods and Tools, University of Twente
 * Copyright 2016-2017 Tom van Dijk, Johannes Kepler University Linz
 * Copyright 2019-2026 Tom van Dijk, Formal Methods and Tools, University of Twente
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */"

echo '
#pragma once

// Lace version
#define LACE_VERSION_MAJOR '$LACE_MAJOR'
#define LACE_VERSION_MINOR '$LACE_MINOR'
#define LACE_VERSION_PATCH '$LACE_PATCH'

#if defined(_MSC_VER) && !defined(__clang__)
    #define LACE_MSVC 1
#else
    #define LACE_MSVC 0
#endif

// Platform configuration
#include <lace_config.h>

// Standard includes
#include <assert.h> // for static_assert
#include <errno.h>
#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#if LACE_MSVC
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <windows.h>
    #undef NEWFRAME // otherwise we cannot use NEWFRAME as a macro name, which is needed for the NEWFRAME() macro
    #include <intrin.h>
#else
    #include <pthread.h>
    #include <unistd.h>
#endif

#if defined(__APPLE__)
  #include <time.h>
  #include <Availability.h>
  #include <TargetConditionals.h>
  #include <mach/mach_time.h>
#endif

#ifndef __cplusplus
    #include <stdatomic.h>
#else
    // Even though we are not really intending to support C++...
    // Compatibility with C11
    #include <atomic>
    #define _Atomic(T) std::atomic<T>
    using std::memory_order_relaxed;
    using std::memory_order_acquire;
    using std::memory_order_release;
    using std::memory_order_seq_cst;
#endif

/**
 * Portable macros
 */

#if LACE_MSVC
    #define LACE_UNUSED
    #define LACE_NOINLINE __declspec(noinline)
    #define LACE_COLD
    #define LACE_NORETURN __declspec(noreturn)
    #define LACE_ALIGN(N) __declspec(align(N))
    #define LACE_LIKELY(x)   (x)
    #define LACE_UNLIKELY(x) (x)

#elif defined(__GNUC__) || defined(__clang__)
    #define LACE_UNUSED __attribute__((unused))
    #define LACE_NOINLINE __attribute__((noinline))
    #define LACE_COLD __attribute__((cold))
    #if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
        #define LACE_NORETURN _Noreturn
        #define LACE_ALIGN(N) _Alignas(N)
    #else
        #define LACE_NORETURN __attribute__((noreturn))
        #define LACE_ALIGN(N) __attribute__((aligned(N)))
    #endif
    #define LACE_LIKELY(x)   __builtin_expect(!!(x), 1)
    #define LACE_UNLIKELY(x) __builtin_expect(!!(x), 0)

#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
    #define LACE_UNUSED
    #define LACE_NOINLINE
    #define LACE_COLD
    #define LACE_NORETURN _Noreturn
    #define LACE_ALIGN(N) _Alignas(N)
    #define LACE_LIKELY(x)   (x)
    #define LACE_UNLIKELY(x) (x)

#else
    #define LACE_UNUSED
    #define LACE_NOINLINE
    #define LACE_COLD
    #define LACE_NORETURN
    #define LACE_ALIGN(N)
    #define LACE_LIKELY(x)   (x)
    #define LACE_UNLIKELY(x) (x)
#endif

#if LACE_MSVC
    #define LACE_TLS __declspec(thread)
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
    #define LACE_TLS _Thread_local
#elif defined(__GNUC__) || defined(__clang__)
    #define LACE_TLS __thread
#else
    #error "No thread-local storage qualifier available"
#endif

/**
 * Portable semaphore abstraction
 */

#if LACE_MSVC
    #include <limits.h>

    typedef HANDLE lace_sem_t;

    static inline int lace_sem_init(lace_sem_t* sem, unsigned value)
    {
        *sem = CreateSemaphoreA(NULL, (LONG)value, LONG_MAX, NULL);
        return (*sem == NULL) ? -1 : 0;
    }

    static inline int lace_sem_destroy(lace_sem_t* sem)
    {
        int ok = CloseHandle(*sem) ? 0 : -1;
        *sem = NULL;
        return ok;
    }

    static inline int lace_sem_post(lace_sem_t* sem)
    {
        return ReleaseSemaphore(*sem, 1, NULL) ? 0 : -1;
    }

    static inline int lace_sem_wait(lace_sem_t* sem)
    {
        DWORD r = WaitForSingleObject(*sem, INFINITE);
        return (r == WAIT_OBJECT_0) ? 0 : -1;
    }

    static inline int lace_sem_trywait(lace_sem_t* sem)
    {
        DWORD r = WaitForSingleObject(*sem, 0);
        if (r == WAIT_OBJECT_0) return 0;
        if (r == WAIT_TIMEOUT) { errno = EAGAIN; return -1; }
        return -1;
    }

#elif defined(__APPLE__)
    #include <dispatch/dispatch.h>

    typedef dispatch_semaphore_t lace_sem_t;

    static inline int lace_sem_init(lace_sem_t* s, unsigned value)
    {
        *s = dispatch_semaphore_create((long)value);
        return (*s == NULL) ? -1 : 0;
    }

    static inline int lace_sem_wait(lace_sem_t* s)
    {
        dispatch_semaphore_wait(*s, DISPATCH_TIME_FOREVER);
        return 0;
    }

    static inline int lace_sem_trywait(lace_sem_t* s)
    {
        long r = dispatch_semaphore_wait(*s, DISPATCH_TIME_NOW);
        if (r == 0) return 0;
        errno = EAGAIN;
        return -1;
    }

    static inline int lace_sem_post(lace_sem_t* s)
    {
        dispatch_semaphore_signal(*s);
        return 0;
    }

    static inline int lace_sem_destroy(lace_sem_t* s)
    {
        /* See note: usually fine to leak until process exit for runtime globals. */
        *s = NULL;
        return 0;
    }

#else
    #include <semaphore.h>

    typedef sem_t lace_sem_t;

    static inline int lace_sem_init(lace_sem_t* sem, unsigned value) { return sem_init(sem, 0, value); }
    static inline int lace_sem_wait(lace_sem_t* sem) { return sem_wait(sem); }
    static inline int lace_sem_trywait(lace_sem_t* sem) { return sem_trywait(sem); }
    static inline int lace_sem_post(lace_sem_t* sem) { return sem_post(sem); }
    static inline int lace_sem_destroy(lace_sem_t* sem) { return sem_destroy(sem); }
#endif

/**
 * Portable mutex and condition abstraction
 */

#if LACE_MSVC
    typedef CRITICAL_SECTION lace_mutex_t;
    typedef CONDITION_VARIABLE lace_cond_t;

    static inline void lace_mutex_init(lace_mutex_t* m) { InitializeCriticalSection(m); }
    static inline void lace_mutex_destroy(lace_mutex_t* m) { DeleteCriticalSection(m); }
    static inline void lace_mutex_lock(lace_mutex_t* m) { EnterCriticalSection(m); }
    static inline void lace_mutex_unlock(lace_mutex_t* m) { LeaveCriticalSection(m); }

    static inline void lace_cond_init(lace_cond_t* c) { InitializeConditionVariable(c); }
    static inline void lace_cond_destroy(lace_cond_t* c) { (void)c; } // no-op on Windows
    static inline void lace_cond_signal(lace_cond_t* c) { WakeConditionVariable(c); }
    static inline void lace_cond_broadcast(lace_cond_t* c) { WakeAllConditionVariable(c); }
    static inline void lace_cond_wait(lace_cond_t* c, lace_mutex_t* m) { SleepConditionVariableCS(c, m, INFINITE); }
#else
    typedef pthread_mutex_t lace_mutex_t;
    typedef pthread_cond_t lace_cond_t;

    static inline void lace_mutex_init(lace_mutex_t* m) { pthread_mutex_init(m, NULL); }
    static inline void lace_mutex_destroy(lace_mutex_t* m) { pthread_mutex_destroy(m); }
    static inline void lace_mutex_lock(lace_mutex_t* m) { pthread_mutex_lock(m); }
    static inline void lace_mutex_unlock(lace_mutex_t* m) { pthread_mutex_unlock(m); }

    static inline void lace_cond_init(lace_cond_t* c) { pthread_cond_init(c, NULL); }
    static inline void lace_cond_destroy(lace_cond_t* c) { pthread_cond_destroy(c); }
    static inline void lace_cond_signal(lace_cond_t* c) { pthread_cond_signal(c); }
    static inline void lace_cond_broadcast(lace_cond_t* c) { pthread_cond_broadcast(c); }
    static inline void lace_cond_wait(lace_cond_t* c, lace_mutex_t* m) { pthread_cond_wait(c, m); }
#endif

#if defined(__has_feature)
    #if __has_feature(thread_sanitizer)
        #define LACE_NO_SANITIZE_THREAD __attribute__((no_sanitize("thread")))
    #else
        #define LACE_NO_SANITIZE_THREAD
    #endif
#else
    #define LACE_NO_SANITIZE_THREAD
#endif

#if LACE_MSVC
    #include <malloc.h>
    #define LACE_ALLOCA(sz) _alloca(sz)
#else
    #if defined(__has_include)
        #if __has_include(<alloca.h>)
            #include <alloca.h>
        #endif
    #else
        #include <alloca.h>
    #endif
    #define LACE_ALLOCA(sz) alloca(sz)
#endif

/**
 * Portable sleep
 */
 
#if defined(_WIN32)
    void lace_sleep_us(int64_t microseconds);
#else
    #include <time.h>
    static inline void lace_sleep_us(int64_t microseconds) {
        if (microseconds <= 0) return;
        struct timespec ts;
        ts.tv_sec = (time_t)(microseconds / 1000000);
        ts.tv_nsec = (long)((microseconds % 1000000) * 1000);
        nanosleep(&ts, NULL);
    }
#endif

// Architecture configuration

// We add padding to some datastructures in order to avoid false sharing.
// We just overapproximate the size of cache lines. On some modern machines,
// cache lines are 128 bytes, so we pick that.
// If needed, this can be overridden with -DLACE_PADDING_TARGET=256 for example
// if targetting architectures that have even larger cache line sizes.
#ifndef LACE_PADDING_TARGET
#define LACE_PADDING_TARGET 128
#endif

/* The size is in bytes. That includes the common fields, so that leaves a little less space for the
   task and parameters. Typically tasksize is 64 for lace.h and 128 for lace14.h. If the size of a
   pointer is 32/64 bits (4/8 bytes) then this leaves 56/48 bytes for parameters of the task and the
   return value. */
#ifndef LACE_TASKSIZE
#define LACE_TASKSIZE ('$tasksize')
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Using Lace.
 *
 * Optionally set the verbosity level with lace_set_verbosity.
 * Optionally set the default program stack size of each worker thread with lace_set_stacksize.
 *
 * Then call lace_start to start Lace workers.
 * - lace_start(n_workers, deque_size);
 *   set both parameters to 0 for reasonable defaults, using all available cores.
 *
 * After this, you can run tasks using the RUN(...)
 */

/**
 * Type definitions used in the functions below.
 * - WorkerP contains the (private) Worker data
 * - Task contains a single Task
 */
typedef struct _WorkerP WorkerP;
typedef struct _Task Task;

/**
 * The macro LACE_TYPEDEF_CB(typedefname, taskname, parametertypes) defines
 * a Task for use as a callback function.
 */
#define LACE_TYPEDEF_CB(t, f, ...) typedef t (*f)(WorkerP *, Task *, ##__VA_ARGS__);
 
/**
 * Set verbosity level (0 = no startup messages, 1 = startup messages)
 * Default level: 0
 */
void lace_set_verbosity(int level);

/**
 * Pinning modes for lace_set_pinning().
 */
typedef enum lace_pin_mode {
    LACE_PIN_OFF = 0,
    LACE_PIN_AUTO = 1,
    LACE_PIN_ALL = 2,
    LACE_PIN_CPU = 3,
    LACE_PIN_L3 = 4,
    LACE_PIN_NUMA = 5,
    LACE_PIN_CORE = 6,
    LACE_PIN_L2 = 7,
} lace_pin_mode;

/**
 * Choose what each worker may run on, within the affinity mask of the thread
 * that calls lace_start() (as taskset, cgroups or a job scheduler set it).
 *
 * - LACE_PIN_AUTO (the default): LACE_PIN_CPU if the workers fill the cores
 *   of the mask, else LACE_PIN_ALL. A program with as many workers as cores
 *   owns them, and one CPU per worker is fastest; one that leaves cores free
 *   may share the machine, and pinning to fixed CPUs would make two such
 *   programs share the same cores.
 * - LACE_PIN_CPU: one hardware thread per worker, one of every physical core
 *   before any second hardware thread of a core.
 * - LACE_PIN_CORE: one physical core per worker (its hardware threads).
 * - LACE_PIN_L2, LACE_PIN_L3, LACE_PIN_NUMA: the cores that share an L2
 *   cache, an L3 cache or a NUMA node. The workers are spread over these
 *   groups in proportion to their number of cores.
 * - LACE_PIN_ALL: any physical core of the mask, one hardware thread each.
 * - LACE_PIN_OFF: no pinning.
 *
 * With more workers than places (hardware threads for LACE_PIN_CPU, cores
 * otherwise) Lace does not pin. Without this call, the environment variable
 * LACE_PIN chooses: off, auto, all, cpu, core, l2, l3, numa, or list:CPUS for
 * worker i on the i-th CPU of a list such as list:0,2,4-7 (see
 * lace_set_pin_cpus). Linux, FreeBSD and Windows only (macOS does not pin),
 * and not with LACE_USE_HWLOC, which pins in its own way. Call before
 * lace_start().
 *
 * @param mode  The pinning mode (default LACE_PIN_AUTO)
 * @see lace_set_pin_cpus
 */
void lace_set_pinning(lace_pin_mode mode);

/**
 * Pin worker i to cpus[i % n] (manual pinning).
 *
 * CPUs outside the affinity mask are left out. Overrides lace_set_pinning()
 * and LACE_PIN; n = 0 undoes it. Call before lace_start().
 *
 * @param cpus  CPU numbers
 * @param n     Number of entries in @p cpus
 * @see lace_set_pinning
 */
void lace_set_pin_cpus(const int *cpus, unsigned int n);
 
/**
 * Set the program stack size of Lace worker threads, at least 16 MB.
 * Call before lace_start. The default (0) is a 1 GB reservation on 64-bit
 * systems where stack pages are committed on first touch (not with
 * LACE_DONT_OVERCOMMIT, not on Windows except MSVC), otherwise the stack
 * limit of the calling thread clamped to 16-64 MB. If the 1 GB stacks
 * cannot be reserved (for example under ulimit -v), Lace falls back to
 * the latter.
 */
void lace_set_stacksize(size_t stacksize);
 
/**
 * Get the program stack size of Lace worker threads.
 * If this returns 0, it uses the default. After lace_start, this is the
 * stack size that the workers use.
 */
size_t lace_get_stacksize(void);

/**
 * Set the per-worker scratch arena reservation size in bytes.
 * Call before lace_start(). The default is 1 GiB on 64-bit systems
 * and 16 MiB on 32-bit systems. If the 64-bit default cannot be
 * reserved (for example under ulimit -v), Lace falls back to 16 MiB.
 * A size set here is used as given: if it cannot be reserved,
 * lace_start() exits with an error. Pass 0 to disable the scratch arena.
 */
void lace_set_scratch_size(size_t size);

/**
 * Set the per-worker scratch committed band in bytes (default 1 MiB).
 * Call before lace_start(). The band is the hysteresis margin above
 * the current top that absorbs push/pop oscillation without
 * commit/decommit syscalls.
 */
void lace_set_scratch_band(size_t size);
 
/**
 * Get the number of available PUs (hardware threads)
 */
unsigned int lace_get_pu_count(void);

/**
 * Start Lace with <n_workers> workers and a a task deque size of <dqsize> per worker.
 * If <n_workers> is set to 0, automatically detects available cores.
 * If <dqsize> is est to 0, uses a reasonable default value.
 * With LACE_BACKOFF (the default), idle workers soon sleep, and an idle Lace
 * uses almost no CPU. On Linux, FreeBSD and Windows, workers are pinned as
 * lace_set_pinning() chooses (by default LACE_PIN_AUTO); with LACE_USE_HWLOC,
 * hwloc pins them to CPU cores.
 */
void lace_start(unsigned int n_workers, size_t dqsize);

/**
 * Stop Lace.
 * Call this method from outside Lace threads.
 */
void lace_stop(void);

/**
 * Give the memory that idle workers hold back to the operating system.
 *
 * A deep computation leaves physical pages behind in the worker stacks, task
 * deques and scratch arenas, and they stay resident until lace_stop(). This
 * makes every worker, at its next idle moment, release the pages of its stack
 * below its idle loop, of its deque past the first page, and of its scratch
 * arena past the first band. The reservations stay, so later work just faults
 * pages in again. Returns when all workers are done, so it waits for running
 * tasks to finish.
 *
 * Must be called from a thread that is not a Lace worker; it returns at once
 * otherwise. On Windows only the scratch arenas are released.
 */
void lace_release_memory(void);

/**
 * Check whether the Lace runtime is currently active.
 * Returns 1 if Lace is running, 0 otherwise.
 */
int lace_is_running(void);

/**
 * Steal a random task.
 * Only use this from inside a Lace task.
 */
#define lace_steal_random() CALL(lace_steal_random)
void lace_steal_random_CALL(WorkerP*, Task*);

/**
 * Enter the Lace barrier. (all active workers must enter it before we can continue)
 * Only run this from inside a Lace task.
 */
void lace_barrier(void);

/**
 * Retrieve the number of Lace workers
 */
unsigned int lace_workers(void);

/**
 * Retrieve whether we are running in a Lace worker. Returns 1 if this is the case, 0 otherwise.
 */
int lace_is_worker(void);

/**
 * Retrieve the current worker data.
 * Only run this from inside a Lace task.
 * (Used by LACE_VARS)
 */
WorkerP *lace_get_worker(void);

/**
 * Retrieve the current head of the deque of the worker.
 * (Used by LACE_VARS)
 */
Task *lace_get_head(WorkerP *);

/**
 * Helper function to call from outside Lace threads.
 * This helper function is used by the _RUN methods for the RUN() macro.
 */
void lace_run_task(Task *task);

/**
 * Helper function to start a new task execution (task frame) on a given task.
 * This helper function is used by the _NEWFRAME methods for the NEWFRAME() macro
 * Only when the task is done, do workers continue with the previous task frame.
 */
void lace_run_newframe(Task *task);

/**
 * Helper function to make all run a given task together.
 * This helper function is used by the _TOGETHER methods for the TOGETHER() macro
 * They all start the task in a lace_barrier and complete it with a lace barrier.
 * Meaning they all start together, and all end together.
 */
void lace_run_together(Task *task);

/**
 * Helper macros
 */
#define LACE_PASTE_(a, b) a ## b
#define LACE_PASTE(a, b)  LACE_PASTE_(a, b)

#define LACE_NARG(...) LACE_NARG_(__VA_ARGS__ __VA_OPT__(,) \
    30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)
#define LACE_NARG_( \
    _1,_2,_3,_4,_5,_6,_7,_8,_9,_10, \
    _11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,N,...) N

#define LACE_SECOND_(a, b, ...) b
#define LACE_PROBE() ~, 1
#define LACE_IS_PROBE(...) LACE_SECOND_(__VA_ARGS__, 0, ~)
#define LACE_IS_VOID(T) LACE_IS_PROBE(LACE_PASTE(LACE_IS_VOID_HELPER_, T))
#define LACE_IS_VOID_HELPER_void LACE_PROBE()

#define LACE_ARITY(n) LACE_ARITY_I(n)
#define LACE_ARITY_I(n) LACE_ARITY_##n
#define LACE_ARITY_2  0
#define LACE_ARITY_4  1
#define LACE_ARITY_6  2
#define LACE_ARITY_8  3
#define LACE_ARITY_10 4
#define LACE_ARITY_12 5
#define LACE_ARITY_14 6
#define LACE_ARITY_16 7
#define LACE_ARITY_18 8
#define LACE_ARITY_20 9
#define LACE_ARITY_22 10
#define LACE_ARITY_24 11
#define LACE_ARITY_26 12
#define LACE_ARITY_28 13
#define LACE_ARITY_30 14

#define LACE_VARITY(n) LACE_VARITY_I(n)
#define LACE_VARITY_I(n) LACE_VARITY_##n
#define LACE_VARITY_1  0
#define LACE_VARITY_3  1
#define LACE_VARITY_5  2
#define LACE_VARITY_7  3
#define LACE_VARITY_9  4
#define LACE_VARITY_11 5
#define LACE_VARITY_13 6
#define LACE_VARITY_15 7
#define LACE_VARITY_17 8
#define LACE_VARITY_19 9
#define LACE_VARITY_21 10
#define LACE_VARITY_23 11
#define LACE_VARITY_25 12
#define LACE_VARITY_27 13
#define LACE_VARITY_29 14

#define TASK(RTYPE, ...) LACE_PASTE(LACE_TASK_V_, LACE_IS_VOID(RTYPE))(RTYPE, __VA_ARGS__)
#define LACE_TASK_V_0(RTYPE, ...) \
    LACE_PASTE(TASK_, LACE_ARITY(LACE_NARG(RTYPE, __VA_ARGS__)))(RTYPE, __VA_ARGS__)
#define LACE_TASK_V_1(RTYPE, ...) \
    LACE_PASTE(VOID_TASK_, LACE_VARITY(LACE_NARG(__VA_ARGS__)))(__VA_ARGS__)

/**
 * Create a pointer to a Tasks main function.
 */
// #define TASK(f)           ( f##_CALL )

/**
 * Call a Tasks implementation (adds Lace variables to call)
 */
#define WRAP(...) \
    LACE_PASTE(WRAP_, LACE_NARG(__VA_ARGS__))(__VA_ARGS__)

/* 1 total argument = function only = 0 task arguments */
#define WRAP_1(f) \
    ( f((WorkerP *)__lace_worker, (Task *)__lace_dq_head) )

#define WRAP_2(f, a1) \
    ( f((WorkerP *)__lace_worker, (Task *)__lace_dq_head, a1) )

#define WRAP_3(f, a1, a2) \
    ( f((WorkerP *)__lace_worker, (Task *)__lace_dq_head, a1, a2) )

#define WRAP_4(f, a1, a2, a3) \
    ( f((WorkerP *)__lace_worker, (Task *)__lace_dq_head, a1, a2, a3) )

#define WRAP_5(f, a1, a2, a3, a4) \
    ( f((WorkerP *)__lace_worker, (Task *)__lace_dq_head, a1, a2, a3, a4) )

#define WRAP_6(f, a1, a2, a3, a4, a5) \
    ( f((WorkerP *)__lace_worker, (Task *)__lace_dq_head, a1, a2, a3, a4, a5) )

#define WRAP_7(f, a1, a2, a3, a4, a5, a6) \
    ( f((WorkerP *)__lace_worker, (Task *)__lace_dq_head, a1, a2, a3, a4, a5, a6) )

#define WRAP_8(f, a1, a2, a3, a4, a5, a6, a7) \
    ( f((WorkerP *)__lace_worker, (Task *)__lace_dq_head, a1, a2, a3, a4, a5, a6, a7) )

#define WRAP_9(f, a1, a2, a3, a4, a5, a6, a7, a8) \
    ( f((WorkerP *)__lace_worker, (Task *)__lace_dq_head, a1, a2, a3, a4, a5, a6, a7, a8) )

#define WRAP_10(f, a1, a2, a3, a4, a5, a6, a7, a8, a9) \
    ( f((WorkerP *)__lace_worker, (Task *)__lace_dq_head, a1, a2, a3, a4, a5, a6, a7, a8, a9) )

#define WRAP_11(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) \
    ( f((WorkerP *)__lace_worker, (Task *)__lace_dq_head, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) )

#define WRAP_12(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) \
    ( f((WorkerP *)__lace_worker, (Task *)__lace_dq_head, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) )

#define WRAP_13(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) \
    ( f((WorkerP *)__lace_worker, (Task *)__lace_dq_head, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) )

#define WRAP_14(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) \
    ( f((WorkerP *)__lace_worker, (Task *)__lace_dq_head, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) )

#define WRAP_15(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) \
    ( f((WorkerP *)__lace_worker, (Task *)__lace_dq_head, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) )

/**
 * Sync a task.
 */
#define SYNC(f)           ( __lace_dq_head--, WRAP(f##_SYNC) )

/**
 * Sync a task, but if the task is not stolen, then do not execute it.
 */
#define DROP()            ( __lace_dq_head--, WRAP(lace_drop) )

/**
 * Spawn a task.
 */
#define SPAWN(f, ...)     ( WRAP(f##_SPAWN, ##__VA_ARGS__), __lace_dq_head++ )

/**
 * Directly execute a task from inside a Lace thread.
 */
#define CALL(...) \
    LACE_PASTE(CALL_, LACE_NARG(__VA_ARGS__))(__VA_ARGS__)

/* 1 total argument = function name only = 0 task arguments */
#define CALL_1(f) \
    ( WRAP_1(f##_CALL) )

#define CALL_2(f, a1) \
    ( WRAP_2(f##_CALL, a1) )

#define CALL_3(f, a1, a2) \
    ( WRAP_3(f##_CALL, a1, a2) )

#define CALL_4(f, a1, a2, a3) \
    ( WRAP_4(f##_CALL, a1, a2, a3) )

#define CALL_5(f, a1, a2, a3, a4) \
    ( WRAP_5(f##_CALL, a1, a2, a3, a4) )

#define CALL_6(f, a1, a2, a3, a4, a5) \
    ( WRAP_6(f##_CALL, a1, a2, a3, a4, a5) )

#define CALL_7(f, a1, a2, a3, a4, a5, a6) \
    ( WRAP_7(f##_CALL, a1, a2, a3, a4, a5, a6) )

#define CALL_8(f, a1, a2, a3, a4, a5, a6, a7) \
    ( WRAP_8(f##_CALL, a1, a2, a3, a4, a5, a6, a7) )

#define CALL_9(f, a1, a2, a3, a4, a5, a6, a7, a8) \
    ( WRAP_9(f##_CALL, a1, a2, a3, a4, a5, a6, a7, a8) )

#define CALL_10(f, a1, a2, a3, a4, a5, a6, a7, a8, a9) \
    ( WRAP_10(f##_CALL, a1, a2, a3, a4, a5, a6, a7, a8, a9) )

#define CALL_11(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) \
    ( WRAP_11(f##_CALL, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) )

#define CALL_12(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) \
    ( WRAP_12(f##_CALL, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) )

#define CALL_13(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) \
    ( WRAP_13(f##_CALL, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) )

#define CALL_14(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) \
    ( WRAP_14(f##_CALL, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) )

#define CALL_15(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) \
    ( WRAP_15(f##_CALL, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) )

/**
 * Directly execute a task from outside Lace threads.
 */
#define RUN(f, ...)       ( f##_RUN ( __VA_ARGS__ ) )

/**
 * Signal all workers to interrupt their current tasks and instead perform (a personal copy of) the given task.
 */
#define TOGETHER(f, ...)  ( f##_TOGETHER ( __VA_ARGS__) )

/**
 * Signal all workers to interrupt their current tasks and help the current thread with the given task.
 */
#define NEWFRAME(f, ...)  ( f##_NEWFRAME ( __VA_ARGS__) )

/**
 * (Try to) steal a task from a random worker.
 */
#define STEAL_RANDOM()    ( CALL(lace_steal_random) )

/**
 * Get the current worker id.
 */
#define LACE_WORKER_ID    ( __lace_worker->worker )

/**
 * Initialize local variables __lace_worker and __lace_dq_head which are required for most Lace functionality.
 * This only works inside a Lace thread.
 */
#define LACE_VARS WorkerP * LACE_UNUSED __lace_worker = lace_get_worker(); Task * LACE_UNUSED __lace_dq_head = lace_get_head(__lace_worker);

/**
 * Check if current tasks must be interrupted, and if so, interrupt.
 *
 * lace_yield is out of line and cold: YIELD_NEWFRAME checks lace_newframe
 * inline and only calls it when there is an interruption, so its task copy
 * and barrier stay out of the stack frames of its callers.
 */
LACE_NOINLINE LACE_COLD void lace_yield(WorkerP *__lace_worker, Task *__lace_dq_head);
#define YIELD_NEWFRAME() { if (LACE_UNLIKELY(atomic_load_explicit(&lace_newframe.t, memory_order_relaxed) != NULL)) { atomic_thread_fence(memory_order_acquire); lace_yield(__lace_worker, __lace_dq_head); } }

/**
 * True if the given task is stolen, False otherwise.
 */
#define TASK_IS_STOLEN(t) ((size_t)atomic_load_explicit(&(t)->thief, memory_order_relaxed) > 1)
 
/**
 * True if the given task is completed, False otherwise.
 */
#define TASK_IS_COMPLETED(t) ((size_t)atomic_load_explicit(&(t)->thief, memory_order_relaxed) == 2)

/**
 * Retrieves a pointer to the result of the given task.
 */
#define TASK_RESULT(t) (&t->d[0])

/**
 * Compute a random number, thread-local (so scalable).
 * Uses xoroshiro128** via lace_rng().
 */
#define LACE_TRNG lace_rng(__lace_worker)

/* Some flags that influence Lace behavior */

#ifndef LACE_COUNT_EVENTS
#define LACE_COUNT_EVENTS (LACE_PIE_TIMES || LACE_COUNT_TASKS || LACE_COUNT_STEALS || LACE_COUNT_SPLITS)
#endif

/**
 * Now follows the implementation of Lace
 */

/* The size of a pointer, 8 bytes on a 64-bit architecture */
#define P_SZ (sizeof(void *))

#define PAD(x,b) ( ( (b) - ((x)%(b)) ) & ((b)-1) ) /* b must be power of 2 */
#define ROUND(x,b) ( (x) + PAD( (x), (b) ) )

static inline uint64_t lace_macos_now_ns(void)
{
#if defined(__APPLE__)
    #if defined(CLOCK_UPTIME_RAW)
        return (uint64_t)clock_gettime_nsec_np(CLOCK_UPTIME_RAW);
    #else
        static mach_timebase_info_data_t tb;
        if (tb.denom == 0) mach_timebase_info(&tb);
        uint64_t t = (uint64_t)mach_absolute_time();
        return (t * (uint64_t)tb.numer) / (uint64_t)tb.denom;
    #endif
#else
    return 0;
#endif
}

/* High resolution timer */
static inline uint64_t lace_gethrtime(void)
{
#if (defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_X64))
    #if defined(_MSC_VER) && !defined(__clang__)
        return (uint64_t)__rdtsc();   // not __rdtscp: some (virtual) CPUs lack RDTSCP
    #elif defined(__clang__) || defined(__GNUC__)
        #if defined(__RDTSCP__)
            unsigned lo, hi, aux;
            __asm__ __volatile__("rdtscp" : "=a"(lo), "=d"(hi), "=c"(aux) :: "memory");
            return ((uint64_t)hi << 32) | lo;
        #else
            unsigned lo, hi;
            __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi) :: "memory");
            return ((uint64_t)hi << 32) | lo;
        #endif
    #else
        /* unknown compiler */
    #endif
#elif defined(_WIN32)
    LARGE_INTEGER t;
    QueryPerformanceCounter(&t);
    return (uint64_t)t.QuadPart;
#elif defined(__APPLE__)
    return lace_macos_now_ns();
#else
    struct timespec ts;
#if defined(CLOCK_MONOTONIC_RAW)
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
#else
    clock_gettime(CLOCK_MONOTONIC, &ts);
#endif
    return (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t)ts.tv_nsec;
#endif
}

void lace_count_reset(void);
void lace_count_report_file(FILE *file);
static inline LACE_UNUSED void lace_count_report(void) { lace_count_report_file(stdout); }

#if LACE_COUNT_TASKS
#define PR_COUNTTASK(s) PR_INC(s,CTR_tasks)
#else
#define PR_COUNTTASK(s) /* Empty */
#endif

#if LACE_COUNT_STEALS
#define PR_COUNTSTEALS(s,i) PR_INC(s,i)
#else
#define PR_COUNTSTEALS(s,i) /* Empty */
#endif

#if LACE_COUNT_SPLITS
#define PR_COUNTSPLITS(s,i) PR_INC(s,i)
#else
#define PR_COUNTSPLITS(s,i) /* Empty */
#endif

#if LACE_COUNT_EVENTS
#define PR_ADD(s,i,k) ( ((s)->ctr[i])+=k )
#else
#define PR_ADD(s,i,k) /* Empty */
#endif
#define PR_INC(s,i) PR_ADD(s,i,1)

typedef enum {
#if LACE_COUNT_TASKS
    CTR_tasks,       /* Number of tasks spawned */
#endif
#if LACE_COUNT_STEALS
    CTR_steal_tries, /* Number of steal attempts */
    CTR_leap_tries,  /* Number of leap attempts */
    CTR_steals,      /* Number of succesful steals */
    CTR_leaps,       /* Number of succesful leaps */
    CTR_steal_busy,  /* Number of steal busies */
    CTR_leap_busy,   /* Number of leap busies */
    CTR_leap_strays, /* Leaps that stole a task unrelated to the awaited one */
    CTR_leap_trans,  /* Successful leaps from a worker that the thief waits for */
#endif
#if LACE_COUNT_SPLITS
    CTR_split_grow,  /* Number of split right */
    CTR_split_shrink,/* Number of split left */
    CTR_split_req,   /* Number of split requests */
#endif
    CTR_fast_sync,   /* Number of fast syncs */
    CTR_slow_sync,   /* Number of slow syncs */
#if LACE_PIE_TIMES
    CTR_init,        /* Timer for initialization */
    CTR_close,       /* Timer for shutdown */
    CTR_wapp,        /* Timer for application code (steal) */
    CTR_lapp,        /* Timer for application code (leap) */
    CTR_wsteal,      /* Timer for steal code (steal) */
    CTR_lsteal,      /* Timer for steal code (leap) */
    CTR_wstealsucc,  /* Timer for succesful steal code (steal) */
    CTR_lstealsucc,  /* Timer for succesful steal code (leap) */
    CTR_wsignal,     /* Timer for signal after work (steal) */
    CTR_lsignal,     /* Timer for signal after work (leap) */
    CTR_backoff,     /* Timer for backoff */
#endif    
    CTR_MAX
} CTR_index;

#define THIEF_EMPTY     ((struct _Worker*)0x0)
#define THIEF_TASK      ((struct _Worker*)0x1)
#define THIEF_COMPLETED ((struct _Worker*)0x2)

#define TASK_COMMON_FIELDS(type)                                   \
    void (*f)(struct _WorkerP *, struct _Task *, struct _Task *);  \
    _Atomic(struct _Worker*) thief;

struct _Worker; // forward declaration
struct __lace_common_fields_only { TASK_COMMON_FIELDS(_Task) };
#define LACE_COMMON_FIELD_SIZE sizeof(struct __lace_common_fields_only)

static_assert((LACE_COMMON_FIELD_SIZE % P_SZ) == 0, "LACE_COMMON_FIELD_SIZE is not a multiple of P_SZ");

typedef struct _Task {
    TASK_COMMON_FIELDS(_Task)
    char d[LACE_TASKSIZE-sizeof(void*)-sizeof(struct _Worker*)];
} Task;

static_assert(LACE_PADDING_TARGET % 32 == 0, "LACE_PADDING_TARGET must be a multiple of 32");
static_assert(sizeof(Task) == '$tasksize', "A Lace task should be '$tasksize' bytes.");

typedef union {
    struct {
        _Atomic(uint32_t) tail;
        _Atomic(uint32_t) split;
    } ts;
    LACE_ALIGN(8) _Atomic(uint64_t) v;
} TailSplit;
 
typedef union {
    struct {
        uint32_t tail;
        uint32_t split;
    } ts;
    uint64_t v;
} TailSplitNA;
 
static_assert(sizeof(TailSplit) == 8, "TailSplit size should be 8 bytes");
static_assert(sizeof(TailSplitNA) == 8, "TailSplit size should be 8 bytes");

typedef struct _Worker {
    Task *dq;
    TailSplit ts;
    /* While this worker leapfrogs: the thief of the task it waits for (the
     * innermost wait), else NULL. Written on entering and leaving a leapfrog;
     * read by leapfrogging workers whose own thief has nothing to steal. It
     * shares the cache line that a steal attempt reads anyway. */
    _Atomic(struct _Worker*) waiting_on;
    uint8_t allstolen;

    char pad1[PAD(2*P_SZ+sizeof(TailSplit)+1, LACE_PADDING_TARGET)];

    uint8_t movesplit;
} Worker;

typedef struct { uint64_t s0, s1; } lace_rng_state;

typedef struct _WorkerP {
    Task *dq;                   // same as dq
    Task *split;                // same as dq+ts.ts.split
    Task *end;                  // dq+dq_size
    Worker *_public;            // pointer to public Worker struct

    /* Scratch arena fast-path fields: hot, kept adjacent to deque fields. */
    char *scratch_top;          // current top of scratch arena (next byte)
    char *scratch_committed_end; // one past end of committed scratch (fast-path bound)

    lace_rng_state rng;         // my random seed (for lace_rng)
    uint32_t seed;              // my random seed (for lace_steal_random)
    uint16_t worker;            // what is my worker id?
    uint8_t allstolen;          // my allstolen

    /* Scratch arena slow-path fields: only touched on grow/trim/teardown. */
    char *scratch_base;          // base of scratch VM reservation (NULL if disabled)
    char *scratch_reserve_end;   // one past end of VM reservation
    int scratch_leak_reported;   // 1 if a leak warning has been emitted for this worker

#if LACE_COUNT_EVENTS
    uint64_t ctr[CTR_MAX];      // counters
    uint64_t time;
    int level;
#endif
#if LACE_COUNT_STEALS
    Task *leap_task;            // awaited task during a leap attempt (for CTR_leap_strays)
#endif
} WorkerP;

/**
 * The public part of a worker. Lace allocates the public struct of a worker
 * right before its private struct (worker_data in lace.c, which aligns the
 * private struct to LACE_PADDING_TARGET), so the hot paths (SPAWN, SYNC,
 * DROP) find it at a constant offset instead of loading w->_public first.
 */
static inline Worker* lace_worker_public_of(WorkerP *w)
{
    return (Worker*)(void*)((char*)w - ROUND(sizeof(Worker), LACE_PADDING_TARGET));
}

/**
 * Scratch arena public API.
 *
 * Use the save/alloc/restore pattern: lace_scratch_mark to record
 * the current top, lace_scratch_alloc to obtain bytes, and
 * lace_scratch_reset to release them before the enclosing task body
 * returns. The arena follows a strict LIFO discipline.
 *
 * Inside a TASK body, the convenience macros LACE_SCRATCH_ALLOC etc.
 * use the implicit __lace_worker pointer; the underlying functions
 * take an explicit WorkerP* for use outside task bodies.
 *
 * If a task forgets to reset, Lace detects the leak when the worker
 * next enters deep backoff, prints a one-line warning, and resets
 * the arena automatically.
 */

/** Slow-path commit-and-allocate; called by lace_scratch_alloc on grow. */
void *lace_scratch_grow(WorkerP *lw, size_t aligned_size);

/** Allocate size bytes from the scratch arena. Aligned to 16 bytes. */
static inline void *lace_scratch_alloc(WorkerP *lw, size_t size)
{
    size = (size + 15) & ~(size_t)15;
    char *result = lw->scratch_top;
    char *new_top = result + size;
    if (LACE_UNLIKELY(new_top > lw->scratch_committed_end)) {
        return lace_scratch_grow(lw, size);
    }
    lw->scratch_top = new_top;
    return result;
}

/** Save current arena top. Pair with lace_scratch_reset before return. */
static inline void *lace_scratch_mark(WorkerP *lw)
{
    return lw->scratch_top;
}

/** Restore arena top to a previously saved mark. */
static inline void lace_scratch_reset(WorkerP *lw, void *mark)
{
    lw->scratch_top = (char *)mark;
}

/* Convenience macros for use inside TASK bodies. */
#define LACE_SCRATCH_ALLOC(size)    lace_scratch_alloc(__lace_worker, (size))
#define LACE_SCRATCH_MARK()         lace_scratch_mark(__lace_worker)
#define LACE_SCRATCH_RESET(mark)    lace_scratch_reset(__lace_worker, (mark))

/**
 * Allocate an array of `count` elements of `type` from the scratch arena
 * and return a properly-typed pointer (no manual cast needed).
 * Equivalent to (type *)LACE_SCRATCH_ALLOC((count) * sizeof(type)).
 * `count` is evaluated exactly once.
 */
#define LACE_SCRATCH_ARRAY(type, count) \
    ((type *)lace_scratch_alloc(__lace_worker, (size_t)(count) * sizeof(type)))

#define LACE_STOLEN      ((Worker*)0)
#define LACE_BUSY        ((Worker*)1)
#define LACE_NOWORK      ((Worker*)2)
#define LACE_STOLEN_LAST ((Worker*)3)

LACE_NORETURN void lace_abort_stack_overflow(void);

typedef struct
{
    _Atomic(Task*) t;
    char pad[LACE_PADDING_TARGET-sizeof(Task *)];
} lace_newframe_t;

extern lace_newframe_t lace_newframe;

/**
 * Random number generator (xoroshiro128**)
 */
static inline uint64_t lace_rotl64(uint64_t x, int k)
{
    return (x << k) | (x >> (64 - k));
}
 
static inline LACE_UNUSED uint64_t lace_rng(WorkerP* w)
{
    uint64_t s0 = w->rng.s0;
    uint64_t s1 = w->rng.s1;
    uint64_t result = lace_rotl64(s0 * 5ULL, 7) * 9ULL;
    s1 ^= s0;
    w->rng.s0 = lace_rotl64(s0, 24) ^ s1 ^ (s1 << 16);
    w->rng.s1 = lace_rotl64(s1, 37);
    return result;
}

/**
 * Make all tasks of the current worker shared.
 */
#define LACE_MAKE_ALL_SHARED() lace_make_all_shared(__lace_worker, __lace_dq_head)
static inline LACE_UNUSED
void lace_make_all_shared(WorkerP *w, Task *__lace_dq_head)
{
    if (w->split != __lace_dq_head) {
        w->split = __lace_dq_head;
        atomic_store_explicit(&w->_public->ts.ts.split,
            (uint32_t)(__lace_dq_head - w->dq), memory_order_release);
    }
}

/**
 * PIE time events
 */
#if LACE_PIE_TIMES
static LACE_UNUSED void lace_time_event(WorkerP *w, int event)
{
    uint64_t now = lace_gethrtime(),
             prev = w->time;
 
    switch (event) {
        case 1 :
            if (w->level == 0) {
                PR_ADD(w, CTR_init, now - prev);
                w->level = 1;
            } else if (w->level == 1) {
                PR_ADD(w, CTR_wsteal, now - prev);
                PR_ADD(w, CTR_wstealsucc, now - prev);
            } else {
                PR_ADD(w, CTR_lsteal, now - prev);
                PR_ADD(w, CTR_lstealsucc, now - prev);
            }
            break;
        case 2 :
            if (w->level == 1) {
                PR_ADD(w, CTR_wapp, now - prev);
            } else {
                PR_ADD(w, CTR_lapp, now - prev);
            }
            break;
        case 3 :
            if (w->level == 1) {
                PR_ADD(w, CTR_wapp, now - prev);
            } else {
                PR_ADD(w, CTR_lapp, now - prev);
            }
            w->level++;
            break;
        case 4 :
            if (w->level == 1) {
                fprintf(stderr, "This should not happen, level = %d\n", w->level);
            } else {
                PR_ADD(w, CTR_lsteal, now - prev);
            }
            w->level--;
            break;
        case 7 :
            if (w->level == 0) {
                PR_ADD(w, CTR_init, now - prev);
            } else if (w->level == 1) {
                PR_ADD(w, CTR_wsteal, now - prev);
            } else {
                PR_ADD(w, CTR_lsteal, now - prev);
            }
            break;
        case 8 :
            if (w->level == 1) {
                PR_ADD(w, CTR_wsignal, now - prev);
                PR_ADD(w, CTR_wsteal, now - prev);
            } else {
                PR_ADD(w, CTR_lsignal, now - prev);
                PR_ADD(w, CTR_lsteal, now - prev);
            }
            break;
        case 9 :
            if (w->level == 0) {
                PR_ADD(w, CTR_init, now - prev);
            } else {
                PR_ADD(w, CTR_close, now - prev);
            }
            break;
        default: return;
    }
 
    w->time = now;
}
#else
#define lace_time_event(w, e) /* Empty */
#endif

/**
 * Core work-stealing functions
 */

LACE_NO_SANITIZE_THREAD
static int
lace_shrink_shared(WorkerP *w)
{
    Worker *wt = w->_public;
    TailSplitNA ts; /* Use non-atomic version to emit better code */
    ts.v = atomic_load_explicit(&wt->ts.v, memory_order_relaxed); /* Force in 1 memory read */
    uint32_t tail = ts.ts.tail;
    uint32_t split = ts.ts.split;
 
    if (tail != split) {
        uint32_t newsplit = (tail + split)/2;
        atomic_store_explicit(&wt->ts.ts.split, newsplit, memory_order_relaxed);
        atomic_thread_fence(memory_order_seq_cst); /* prevent StoreLoad reordering */
        tail = atomic_load_explicit(&wt->ts.ts.tail, memory_order_relaxed);
        if (tail != split) {
            if (LACE_UNLIKELY(tail > newsplit)) {
                newsplit = (tail + split) / 2;
                atomic_store_explicit(&wt->ts.ts.split, newsplit, memory_order_relaxed);
            }
            w->split = w->dq + newsplit;
            PR_COUNTSPLITS(w, CTR_split_shrink);
            return 0;
        } else {
            w->split = w->dq + split;
        }
    }
 
    wt->allstolen = 1;
    w->allstolen = 1;
    return 1;
}

/**
 * Wait for the stolen task at __lace_dq_head and pop it (lace.c). While its
 * thief works on it, steal from the thief, or from the workers that the
 * thief waits for: they all work on parts of the awaited task.
 */
void lace_leapfrog(WorkerP *__lace_worker, Task *__lace_dq_head);

static LACE_NOINLINE
void lace_drop_slow(WorkerP *w, Task *__dq_head)
{
    if ((w->allstolen) || (w->split > __dq_head && lace_shrink_shared(w))) lace_leapfrog(w, __dq_head);
}

static inline LACE_UNUSED
void lace_drop(WorkerP *w, Task *__dq_head)
{
    if (LACE_LIKELY(0 == lace_worker_public_of(w)->movesplit)) {
        if (LACE_LIKELY(w->split <= __dq_head)) {
            return;
        }
    }
    lace_drop_slow(w, __dq_head);
}

'
#
# Create macros for each arity
#

for(( r = 0; r <= $nparams; r++ )) do

# Extend various argument lists
if ((r)); then
  MACRO_ARGS="$MACRO_ARGS, ATYPE_$r, ARG_$r"
  DECL_ARGS="$DECL_ARGS, ATYPE_$r"
  TASK_FIELDS="$TASK_FIELDS ATYPE_$r arg_$r;"
  TASK_INIT="$TASK_INIT t->d.args.arg_$r = arg_$r;"
  TASK_GET_FROM_t="$TASK_GET_FROM_t, t->d.args.arg_$r"
  CALL_ARGS="$CALL_ARGS, arg_$r"
  FUN_ARGS="$FUN_ARGS, ATYPE_$r arg_$r"
  WORK_ARGS="$WORK_ARGS, ATYPE_$r ARG_$r"
  ARGS_STRUCT="struct { $TASK_FIELDS } args;"
fi

FUN_ARGS_NC=${FUN_ARGS:2}
FUN_ARGS_NC=${FUN_ARGS_NC:-void}

echo
echo "// Task macros for tasks of arity $r"
echo

# Create a void and a non-void version
for isvoid in 0 1; do
if (( isvoid==0 )); then
  DEF_MACRO="#define TASK_$r(RTYPE, NAME$MACRO_ARGS) \
             TASK_DECL_$r(RTYPE, NAME$DECL_ARGS) TASK_IMPL_$r(RTYPE, NAME$MACRO_ARGS)"
  DECL_MACRO="#define TASK_DECL_$r(RTYPE, NAME$DECL_ARGS)"
  IMPL_MACRO="#define TASK_IMPL_$r(RTYPE, NAME$MACRO_ARGS)"
  RTYPE="RTYPE"
  RES_FIELD="$RTYPE res;"
  SAVE_RVAL="t->d.res ="
  RETURN_RES="((TD_##NAME *)t)->d.res"
  UNION="union { $ARGS_STRUCT $RTYPE res; } d;"
  SS_RETURN="return "
  SS_RETURN2=""
else
  DEF_MACRO="#define VOID_TASK_$r(NAME$MACRO_ARGS) \
             VOID_TASK_DECL_$r(NAME$DECL_ARGS) VOID_TASK_IMPL_$r(NAME$MACRO_ARGS)"
  DECL_MACRO="#define VOID_TASK_DECL_$r(NAME$DECL_ARGS)"
  IMPL_MACRO="#define VOID_TASK_IMPL_$r(NAME$MACRO_ARGS)"
  RTYPE="void"
  SAVE_RVAL=""
  RETURN_RES=""
  if ((r)); then UNION="union { $ARGS_STRUCT } d;"; else UNION=""; fi
  SS_RETURN=""
  SS_RETURN2="return;"
fi

# Write down the macro for the task declaration
(\
echo "$DECL_MACRO

typedef struct _TD_##NAME {
  TASK_COMMON_FIELDS(_TD_##NAME)
  $UNION
} TD_##NAME;

static_assert(sizeof(TD_##NAME) <= sizeof(Task), \"TD_\" #NAME \" is too large to fit in the Task struct!\");

void NAME##_WRAP(WorkerP *, Task *, Task *);
$RTYPE NAME##_CALL(WorkerP *, Task * $FUN_ARGS);
static inline $RTYPE NAME##_SYNC(WorkerP *, Task *);
static $RTYPE NAME##_SYNC_SLOW(WorkerP *, Task *);

static inline LACE_UNUSED LACE_NO_SANITIZE_THREAD
void NAME##_SPAWN(WorkerP *w, Task *__dq_head $FUN_ARGS)
{
    PR_COUNTTASK(w);
 
    TD_##NAME *t;
    TailSplitNA ts;
    uint32_t head, split, newsplit;
 
    if (__dq_head == w->end) lace_abort_stack_overflow();
 
    t = (TD_##NAME *)__dq_head;
    t->f = &NAME##_WRAP;
    atomic_store_explicit(&t->thief, THIEF_TASK, memory_order_relaxed);
    $TASK_INIT
    /* The task is in the private part of the deque, where thieves cannot take
     * it. It becomes stealable only when the split moves above it: those
     * stores (here, in lace_make_all_shared and in SYNC) are release stores,
     * so a thief that steals it sees it written. No fence here. */
 
    Worker *wt = lace_worker_public_of(w);
    if (LACE_UNLIKELY(w->allstolen)) {
        if (wt->movesplit) wt->movesplit = 0;
        head = (uint32_t)(__dq_head - w->dq);
        ts.ts.tail = head;
        ts.ts.split = head + 1;
        atomic_store_explicit(&wt->ts.v, ts.v, memory_order_release);
        wt->allstolen = 0;
        w->split = __dq_head+1;
        w->allstolen = 0;
    } else if (LACE_UNLIKELY(wt->movesplit)) {
        head = (uint32_t)(__dq_head - w->dq);
        split = (uint32_t)(w->split - w->dq);
        newsplit = (split + head + 2)/2;
        atomic_store_explicit(&wt->ts.ts.split, newsplit, memory_order_release);
        w->split = w->dq + newsplit;
        wt->movesplit = 0;
        PR_COUNTSPLITS(w, CTR_split_grow);
    }
}

static inline LACE_UNUSED LACE_NO_SANITIZE_THREAD
$RTYPE NAME##_NEWFRAME($FUN_ARGS_NC)
{
    Task _t;
    TD_##NAME *t = (TD_##NAME *)&_t;
    t->f = &NAME##_WRAP;
    atomic_store_explicit(&t->thief, THIEF_TASK, memory_order_relaxed);
    $TASK_INIT
    lace_run_newframe(&_t);
    return $RETURN_RES;
}
 
static inline LACE_UNUSED LACE_NO_SANITIZE_THREAD
void NAME##_TOGETHER($FUN_ARGS_NC)
{
    Task _t;
    TD_##NAME *t = (TD_##NAME *)&_t;
    t->f = &NAME##_WRAP;
    atomic_store_explicit(&t->thief, THIEF_TASK, memory_order_relaxed);
    $TASK_INIT
    lace_run_together(&_t);
}

static inline LACE_UNUSED LACE_NO_SANITIZE_THREAD
$RTYPE NAME##_RUN($FUN_ARGS_NC)
{
    Task _t;
    TD_##NAME *t = (TD_##NAME *)&_t;
    t->f = &NAME##_WRAP;
    atomic_store_explicit(&t->thief, THIEF_TASK, memory_order_relaxed);
    $TASK_INIT
    lace_run_task(&_t);
    return $RETURN_RES;
}

static LACE_NOINLINE LACE_NO_SANITIZE_THREAD
$RTYPE NAME##_SYNC_SLOW(WorkerP *w, Task *__dq_head)
{
    TD_##NAME *t;
 
    if ((w->allstolen) || (w->split > __dq_head && lace_shrink_shared(w))) {
        lace_leapfrog(w, __dq_head);
        t = (TD_##NAME *)__dq_head;
        return $RETURN_RES;
    }
 
    Worker *wt = w->_public;
    if (wt->movesplit) {
        Task *t_s = w->split;
        ptrdiff_t diff = __dq_head - t_s;
        diff = (diff + 1) / 2;
        Task* newsplit = t_s + diff;
        w->split = newsplit;
        atomic_store_explicit(&wt->ts.ts.split, (uint32_t)(newsplit - w->dq), memory_order_release);
        wt->movesplit = 0;
        PR_COUNTSPLITS(w, CTR_split_grow);
    }
 
    t = (TD_##NAME *)__dq_head;
    atomic_store_explicit(&t->thief, THIEF_EMPTY, memory_order_relaxed);
    ${SS_RETURN}NAME##_CALL(w, __dq_head $TASK_GET_FROM_t);
}

static inline LACE_UNUSED LACE_NO_SANITIZE_THREAD
$RTYPE NAME##_SYNC(WorkerP *w, Task *__dq_head)
{
    /* assert (__dq_head > 0); */  /* Commented out because we assume contract */
 
    if (LACE_LIKELY(0 == lace_worker_public_of(w)->movesplit)) {
        if (LACE_LIKELY(w->split <= __dq_head)) {
            TD_##NAME *t = (TD_##NAME *)__dq_head;
            atomic_store_explicit(&t->thief, THIEF_EMPTY, memory_order_relaxed);
            ${SS_RETURN}NAME##_CALL(w, __dq_head $TASK_GET_FROM_t);
            ${SS_RETURN2}
        }
    }
 
    ${SS_RETURN}NAME##_SYNC_SLOW(w, __dq_head);
}

"\
) | awk '{printf "%-86s\\\n", $0 }'

echo ""

(\
echo "$IMPL_MACRO
LACE_NO_SANITIZE_THREAD
void NAME##_WRAP(WorkerP *w, Task *__dq_head, Task *task)
{
    TD_##NAME *t = (TD_##NAME*)task;
    (void)t;
    $SAVE_RVAL NAME##_CALL(w, __dq_head $TASK_GET_FROM_t);
}

static inline
$RTYPE NAME##_WORK(WorkerP *__lace_worker, Task *__lace_dq_head $DECL_ARGS);

/* NAME##_WORK is inlined in NAME##_CALL and the parameter __lace_in_task will disappear */
LACE_NO_SANITIZE_THREAD
$RTYPE NAME##_CALL(WorkerP *w, Task *__dq_head $FUN_ARGS)
{
    ${SS_RETURN}NAME##_WORK(w, __dq_head $CALL_ARGS);
}

static inline
$RTYPE NAME##_WORK(LACE_UNUSED WorkerP *__lace_worker, LACE_UNUSED Task *__lace_dq_head $WORK_ARGS)" \
) | awk '{printf "%-86s\\\n", $0 }'

echo ""

echo $DEF_MACRO

echo ""

done

done

echo "

#ifdef __cplusplus
}
#endif /* __cplusplus */
"
