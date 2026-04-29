/*
 * Copyright 2013-2016 Formal Methods and Tools, University of Twente
 * Copyright 2016-2017 Tom van Dijk, Johannes Kepler University Linz
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
 */


#if defined(__GLIBC__)
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#endif

#include <lace14.h>

#include <errno.h> // for errno
#include <inttypes.h> // for PRIu64
#include <stddef.h> // for size_t
#include <stdio.h>  // for fprintf
#include <stdlib.h> // for memalign, malloc
#include <string.h> // for memset
#include <time.h> // for clock_gettime

#if LACE_MSVC
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <processthreadsapi.h>
#include <process.h>
#include <synchapi.h>
#include <profileapi.h>
#include <malloc.h>
#else
#include <sched.h>
#include <unistd.h>
#include <pthread.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h> // still get windows...
#else
#include <sys/resource.h> // for getrlimit
#endif
#endif

#ifdef __STDC_NO_ATOMICS__
#if LACE_MSVC
#error "C atomic support is not enabled; try /std:c11 and /experimental:c11atomics"
#else
#error "C atomic support is not enabled"
#endif
#endif

#include <stdatomic.h>

#if LACE_USE_HWLOC
#include <hwloc.h>

 /**
  * HWLOC information
  */
static hwloc_topology_t topo;
static hwloc_cpuset_t* cpusets;
static unsigned int n_nodes, n_cores, n_pus;
#else
static unsigned int n_pus;
#endif

#if !defined(_WIN32)
#include <sys/mman.h>
#ifndef MAP_NORESERVE
#define MAP_NORESERVE 0
#endif
#endif

/**
 * Little helper to get cache line size
 */
#if LACE_MSVC
size_t get_cache_line_size(void)
{
    DWORD bytes = 0;
    GetLogicalProcessorInformation(NULL, &bytes);
    if (bytes == 0) return 64;

    SYSTEM_LOGICAL_PROCESSOR_INFORMATION* buf =
        (SYSTEM_LOGICAL_PROCESSOR_INFORMATION*)malloc(bytes);
    if (!buf) return 64;

    if (!GetLogicalProcessorInformation(buf, &bytes)) {
        free(buf);
        return 64;
    }

    size_t line = 0;
    DWORD count = bytes / (DWORD)sizeof(*buf);
    for (DWORD i = 0; i < count; i++) {
        if (buf[i].Relationship == RelationCache &&
            buf[i].Cache.Level == 1 &&
            buf[i].Cache.LineSize != 0) {
            line = (size_t)buf[i].Cache.LineSize;
            break;
        }
    }

    free(buf);
    return line ? line : 64;
}

#elif defined(__APPLE__)
#include <sys/sysctl.h>

static size_t get_cache_line_size(void)
{
    size_t line = 0;
    size_t sz = sizeof(line);
    if (sysctlbyname("hw.cachelinesize", &line, &sz, NULL, 0) == 0 && line != 0)
        return line;
    return 64;
}

#elif defined(__linux__)
static size_t get_cache_line_size(void)
{
    long line = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
    return (line > 0) ? (size_t)line : 64;
}

#else
static size_t get_cache_line_size(void)
{
    return 64;
}
#endif

static size_t cache_line_size;

/**
 * Thread handles
 */
#if !LACE_MSVC
static pthread_t* handles = NULL;
#else
static HANDLE* handles = NULL;
#endif

/**
 * Worker thread program stacks
 */
#if LACE_USE_HWLOC || !defined(_WIN32)
static void** worker_stacks = NULL;
static size_t worker_stack_size = 0;
static size_t worker_stack_page_size = 0;
#endif

/**
 * (public) Worker data
 */
static Worker **workers = NULL;

/**
 * Size of the task deque
 */
static size_t dqsize = 1048576; // 1M entries, virtual only, should be sufficient... (famous last words)

/**
 * Size of the program stack of each Lace worker
 */
static size_t stacksize = 0;

/**
 * Verbosity flag, set with lace_set_verbosity
 */
static int verbosity = 0;

/**
 * Number of workers
 */
static unsigned int n_workers = 0;

/**
 * Datastructure of the task deque etc for each worker.
 * - first public cachelines (accessible via global "workers" variable)
 * - then private cachelines
 * - then the deque array
 */
#if LACE_MSVC
#pragma warning(push)
#pragma warning(disable: 4324)
#endif

typedef struct {
    Worker worker_public;
    alignas(LACE_PADDING_TARGET) WorkerP worker_private;
    alignas(LACE_PADDING_TARGET) Task deque[];
} worker_data;

#if LACE_MSVC
#pragma warning(pop)
#endif

/**
 * (Secret) holds pointers to the memory block allocated for each worker
 */
static worker_data **workers_memory = NULL;

/**
 * Number of bytes allocated for each worker's worker data.
 */
static size_t workers_memory_size = 0;

/**
 * (Secret) holds pointer to private Worker data, just for stats collection at end
 */
static WorkerP **workers_p;

/**
 * Flag to signal all workers to quit.
 */
static atomic_int lace_quits = 0;

/**
 * Flag whether lace is running
 */
static int is_running = 0;

/**
 * Thread-specific mechanism to access current worker data
 */
LACE_TLS WorkerP* current_worker = NULL;

/**
 * Global newframe variable used for the implementation of NEWFRAME and TOGETHER
 */
lace_newframe_t lace_newframe;

/**
 * Retrieve whether we are running as a Lace worker
 */
int
lace_is_worker(void)
{
    return lace_get_worker() != NULL ? 1 : 0;
}

/**
 * Get the private Worker data of the current thread
 */
WorkerP*
lace_get_worker(void)
{
    return current_worker;
}

/**
 * Find the head of the task deque, using the given private Worker data
 */
Task*
lace_get_head(WorkerP *self)
{
    Task *dq = self->dq;

    /* First check the first tasks linearly */
    if (atomic_load_explicit(&dq[0].thief, memory_order_relaxed) == 0) return dq;
    if (atomic_load_explicit(&dq[1].thief, memory_order_relaxed) == 0) return dq+1;
    if (atomic_load_explicit(&dq[2].thief, memory_order_relaxed) == 0) return dq+2;
    if (atomic_load_explicit(&dq[3].thief, memory_order_relaxed) == 0) return dq+3;

    /* Then fast search for a low/high bound using powers of 2: 4, 8, 16... */
    ptrdiff_t low = 2;
    ptrdiff_t high = self->end - self->dq;

    for (;;) {
        if (low*2 >= high) {
            break;
        } else if (atomic_load_explicit(&dq[low*2].thief, memory_order_relaxed) == 0) {
            high=low*2;
            break;
        } else {
            low*=2;
        }
    }

    /* Finally zoom in using binary search */
    while (low < high) {
        ptrdiff_t mid = low + (high-low)/2;
        if (atomic_load_explicit(&dq[mid].thief, memory_order_relaxed) == 0) high = mid;
        else low = mid + 1;
    }

    return dq+low;
}

/**
 * Get the number of workers
 */
unsigned int
lace_workers(void)
{
    return n_workers;
}

/**
 * Get the default stack size (or 0 for automatically determine)
 */
size_t
lace_get_stacksize(void)
{
    return stacksize;
}


/**
 * If we are collecting PIE times, then we need some helper functions.
 */
static uint64_t count_at_start;
static uint64_t us_elapsed_timer;

static inline uint64_t lace_now_us(void)
{
#if defined(_WIN32)
    /* Windows: QueryPerformanceCounter -> microseconds */
    LARGE_INTEGER t, f;
    QueryPerformanceCounter(&t);
    QueryPerformanceFrequency(&f);
    /* avoid overflow: (t * 1e6) / f  */
    return (uint64_t)(((uint64_t)t.QuadPart * 1000000ULL) / (uint64_t)f.QuadPart);

#elif defined(__APPLE__)
    return lace_macos_now_ns() / 1000ULL;

#else
    /* POSIX: clock_gettime -> microseconds */
    struct timespec ts_now;
#if defined(CLOCK_MONOTONIC_RAW)
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts_now);
#else
    clock_gettime(CLOCK_MONOTONIC, &ts_now);
#endif
    return (uint64_t)ts_now.tv_sec * 1000000ULL + (uint64_t)ts_now.tv_nsec / 1000ULL;
#endif
}

static void us_elapsed_start(void)
{
    us_elapsed_timer = lace_now_us();
}

LACE_UNUSED
static unsigned long long us_elapsed(void)
{
    return (unsigned long long)(lace_now_us() - us_elapsed_timer);
}

/**
 * Lace barrier implementation, that synchronizes on all workers.
 */
#if LACE_MSVC
#pragma warning(push)
#pragma warning(disable: 4324)
#endif

typedef struct {
    LACE_ALIGN(LACE_PADDING_TARGET) atomic_int count;
    LACE_ALIGN(LACE_PADDING_TARGET) atomic_int leaving;
    LACE_ALIGN(LACE_PADDING_TARGET) atomic_int wait;
} barrier_t;

#if LACE_MSVC
#pragma warning(pop)
#endif

static barrier_t lace_bar;

/**
 * Enter the Lace barrier and wait until all workers have entered the Lace barrier.
 */
void
lace_barrier(void)
{
    int wait = atomic_load_explicit(&lace_bar.wait, memory_order_relaxed);
    if ((int)n_workers == 1 + atomic_fetch_add_explicit(&lace_bar.count, 1, memory_order_acq_rel)) {
        // This thread is the last to arrive (the leader)
        atomic_store_explicit(&lace_bar.count, 0, memory_order_relaxed);
        atomic_store_explicit(&lace_bar.leaving, (int)n_workers, memory_order_relaxed);
        atomic_fetch_xor_explicit(&lace_bar.wait, 1, memory_order_acq_rel);
    }
    else {
        // Wait until leader flips the wait value
        while (atomic_load_explicit(&lace_bar.wait, memory_order_acquire) == wait) {
            // possibly pause/yield
        }
    }
    // Needed for lace_barrier_destroy to observe all threads have exited
    atomic_fetch_sub_explicit(&lace_bar.leaving, 1, memory_order_release);
}

/**
 * Initialize the Lace barrier
 */
static void
lace_barrier_init(void)
{
    atomic_init(&lace_bar.count, 0);
    atomic_init(&lace_bar.leaving, 0);
    atomic_init(&lace_bar.wait, 0);
}

/**
 * Destroy the Lace barrier (just wait until all are exited)
 */
static void
lace_barrier_destroy(void)
{
    while (atomic_load_explicit(&lace_bar.leaving, memory_order_acquire) > 0) {
        // possibly pause/yield
    }
}

/**
 * For debugging purposes, check if memory is allocated on the correct memory nodes.
 */
static void LACE_UNUSED
lace_check_memory(void)
{
#if LACE_USE_HWLOC
    // get our current worker
    WorkerP *w = lace_get_worker();
    void* mem = workers_memory[w->worker];

    // get pinned PUs
    hwloc_cpuset_t cpuset = hwloc_bitmap_alloc();
    hwloc_get_cpubind(topo, cpuset, HWLOC_CPUBIND_THREAD);

    // get nodes of pinned PUs
    hwloc_nodeset_t cpunodes = hwloc_bitmap_alloc();
    hwloc_cpuset_to_nodeset(topo, cpuset, cpunodes);

    // get location of memory
    hwloc_nodeset_t memlocation = hwloc_bitmap_alloc();
    hwloc_get_area_memlocation(topo, mem, sizeof(worker_data), memlocation, HWLOC_MEMBIND_BYNODESET);

    // check if CPU and node are on the same place
    if (!hwloc_bitmap_isincluded(memlocation, cpunodes)) {
        fprintf(stdout, "Lace warning: Lace thread not on same memory domain as data!\n");

        char *strp, *strp2, *strp3;
        hwloc_bitmap_list_asprintf(&strp, cpuset);
        hwloc_bitmap_list_asprintf(&strp2, cpunodes);
        hwloc_bitmap_list_asprintf(&strp3, memlocation);
        fprintf(stdout, "Worker %d is pinned on PUs %s, node %s; memory is pinned on node %s\n", w->worker, strp, strp2, strp3);
        free(strp);
        free(strp2);
        free(strp3);
    }

    // free allocated memory
    hwloc_bitmap_free(cpuset);
    hwloc_bitmap_free(cpunodes);
    hwloc_bitmap_free(memlocation);
#endif
}

static void
lace_pin_worker(void)
{
#if LACE_USE_HWLOC
    // Get the worker id
    unsigned int worker = lace_get_worker()->worker;

    // Pin the thread
    if (hwloc_set_cpubind(topo, cpusets[worker], HWLOC_CPUBIND_THREAD) != 0) {
        fprintf(stderr, "Lace warning: hwloc_set_cpubind failed!\n");
    }

    // Grab nodeset for this cpuset
    hwloc_bitmap_t nodeset = hwloc_bitmap_alloc();
    if (hwloc_cpuset_to_nodeset(topo, cpusets[worker], nodeset) != 0) {
        fprintf(stderr, "Lace error: Unable to convert cpuset to nodeset!\n");
    }

    // Pin the memory area
    if (hwloc_set_area_membind(topo, workers_memory[worker], workers_memory_size, nodeset, HWLOC_MEMBIND_BIND, HWLOC_MEMBIND_STRICT | HWLOC_MEMBIND_MIGRATE | HWLOC_MEMBIND_BYNODESET) != 0) {
        fprintf(stderr, "Lace error: Unable to bind worker memory to node!\n");
    }

    // Free allocated memory
    hwloc_bitmap_free(nodeset);

    // Check if everything is on the correct node
    lace_check_memory();
#endif
}

static inline uint64_t lace_splitmix64(uint64_t* x)
{
    uint64_t z = (*x += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
}

static inline void lace_rng_seed(WorkerP* w, uint64_t seed)
{
    uint64_t x = seed ? seed : 0x123456789abcdefULL;
    w->rng.s0 = lace_splitmix64(&x);
    w->rng.s1 = lace_splitmix64(&x);
    if ((w->rng.s0 | w->rng.s1) == 0) w->rng.s1 = 1;
}

LACE_NO_SANITIZE_THREAD
static void
lace_init_worker(unsigned int worker)
{
    // Allocate our memory
#if defined(_WIN32)
    workers_memory[worker] = (worker_data*)VirtualAlloc(NULL, workers_memory_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (workers_memory[worker] == NULL) {
        fprintf(stderr, "Lace error: Unable to allocate VirtualAlloc memory for the Lace worker!\n");
        exit(1);
    }
#else
    workers_memory[worker] = mmap(NULL, workers_memory_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
    if (workers_memory[worker] == MAP_FAILED) {
        fprintf(stderr, "Lace error: Unable to allocate mmapped memory for the Lace worker!\n");
        exit(1);
    }
#endif

    // Set pointers
    Worker *wt = workers[worker] = &workers_memory[worker]->worker_public;
    WorkerP *w = workers_p[worker] = &workers_memory[worker]->worker_private;
    w->dq = workers_memory[worker]->deque;
    current_worker = w;

    // Initialize public worker data
    wt->dq = w->dq;
    atomic_store_explicit(&wt->ts.v, 0, memory_order_relaxed);
    wt->allstolen = 0;
    wt->movesplit = 0;

    // Initialize private worker data
    w->_public = wt;
    w->end = w->dq + dqsize;
    w->split = w->dq;
    w->allstolen = 0;
    w->worker = (uint16_t)worker;

    uint64_t seed = ((uint64_t)(uint32_t)rand() << 32) ^ (uint32_t)rand();
    lace_rng_seed(w, seed);

#if LACE_COUNT_EVENTS
    // Initialize counters
    { int k; for (k=0; k<CTR_MAX; k++) w->ctr[k] = 0; }
#endif

#if LACE_PIE_TIMES
    w->time = lace_gethrtime();
    w->level = 0;
#endif
}

/**
 * "External" task management
 */

 /**
  * Global "external" task
  */
typedef struct ext_lace_task {
    Task* task;
    lace_sem_t sem;
} ExtTask;

#define LACE_EXT_SLOTS 64
static _Atomic(ExtTask*)external_tasks[LACE_EXT_SLOTS];
static atomic_int external_task_count = 0;

void
lace_run_task(Task* task)
{
    // check if we are really not in a Lace thread
    WorkerP* self = lace_get_worker();
    if (self != 0) {
        task->f(self, lace_get_head(self), task);
        return;
    }

    ExtTask et;
    et.task = task;
    atomic_store_explicit(&et.task->thief, 0, memory_order_relaxed);
    if (lace_sem_init(&et.sem, 0) != 0) {
        fprintf(stderr, "Lace error: unable to create semaphore for external task!\n");
        exit(1);
    }

    // Push into any empty slot
    while (1) {
        for (int i = 0; i < LACE_EXT_SLOTS; i++) {
            ExtTask* expected = NULL;
            if (atomic_compare_exchange_weak_explicit(&external_tasks[i],
                &expected, &et, memory_order_release, memory_order_relaxed)) {
                atomic_fetch_add_explicit(&external_task_count, 1, memory_order_release);
                goto pushed;
            }
        }
        lace_sleep_us(1);
    }
pushed:

    lace_sem_wait(&et.sem);
    lace_sem_destroy(&et.sem);
}

static inline void
lace_steal_external(WorkerP* self, Task* head)
{
    for (int i = 0; i < LACE_EXT_SLOTS; i++) {
        ExtTask* stolen = atomic_exchange_explicit(&external_tasks[i], NULL, memory_order_acquire);
        if (stolen != NULL) {
            // execute task
            atomic_fetch_sub_explicit(&external_task_count, 1, memory_order_relaxed);
            atomic_store_explicit(&stolen->task->thief, self->_public, memory_order_relaxed);
            lace_time_event(self, 1);
            stolen->task->f(self, head, stolen->task);
            lace_time_event(self, 2);
            atomic_store_explicit(&stolen->task->thief, THIEF_COMPLETED, memory_order_relaxed);
            lace_sem_post(&stolen->sem);
            lace_time_event(self, 8);
            return;
        }
    }
}

/**
 * (Try to) steal and execute a task from a random worker.
 */
TASK(void, lace_steal_random)
{
    YIELD_NEWFRAME();

    if (LACE_UNLIKELY(atomic_load_explicit(&external_task_count, memory_order_acquire) > 0)) {
        lace_steal_external(__lace_worker, __lace_dq_head);
    } else if (n_workers > 1) {
        Worker* victim = workers[(__lace_worker->worker + 1U + (lace_rng(__lace_worker) % (n_workers - 1))) % n_workers];

        PR_COUNTSTEALS(__lace_worker, CTR_steal_tries);
        Worker *res = lace_steal(__lace_worker, __lace_dq_head, victim);
        if (res == LACE_STOLEN) {
            PR_COUNTSTEALS(__lace_worker, CTR_steals);
        } else if (res == LACE_BUSY) {
            PR_COUNTSTEALS(__lace_worker, CTR_steal_busy);
        }
    }
}

/**
 * Main Lace worker implementation.
 * Steal from random victims until "quit" is set.
 */
TASK(void, lace_steal_loop, atomic_int*, quit)
{
    // Determine who I am
    const int worker_id = __lace_worker->worker;

    // Prepare self, victim
    Worker ** const self = &workers[worker_id];
    Worker ** victim = self;

#if LACE_PIE_TIMES
    __lace_worker->time = lace_gethrtime();
#endif

    unsigned int n = n_workers;
#if LACE_BACKOFF
    unsigned int backoff = 0;
#endif

    while (1) {
#if LACE_BACKOFF
        backoff++;
#endif
        if (n > 1) {
            victim = workers + ((lace_rng(__lace_worker) % (n - 1)) + (uint64_t)worker_id + 1) % n;

            PR_COUNTSTEALS(__lace_worker, CTR_steal_tries);
            Worker* res = lace_steal(__lace_worker, __lace_dq_head , *victim);
            if (res == LACE_STOLEN) {
                PR_COUNTSTEALS(__lace_worker, CTR_steals);
#if LACE_BACKOFF
                backoff = 0;
#endif
            }
            else if (res == LACE_BUSY) {
                PR_COUNTSTEALS(__lace_worker, CTR_steal_busy);
#if LACE_BACKOFF
                backoff = 0;
#endif
            }
            else { // LACE_NOWORK
            }
        }

        YIELD_NEWFRAME();

        if (LACE_UNLIKELY(atomic_load_explicit(&external_task_count, memory_order_acquire) > 0)) {
            lace_steal_external(__lace_worker, __lace_dq_head);
#if LACE_BACKOFF
            backoff = 0;
#endif
            continue;
        }

#if LACE_BACKOFF
        if (backoff > 1000) { // only back off after 1000 attempts
            int64_t delay_us = ((int64_t)1) << ((backoff - 1000) / 50);
            if (delay_us > 1000) delay_us = 1000; // cap at 1ms
#if LACE_PIE_TIMES
            uint64_t prev = lace_gethrtime();
#endif
            lace_sleep_us(delay_us);
#if LACE_PIE_TIMES
            PR_ADD(__lace_worker, CTR_backoff, lace_gethrtime() - prev);
#endif
        }
#endif

        if (atomic_load_explicit(quit, memory_order_relaxed)) break;
    }
}

/**
 * Initialize the current thread as a Lace thread, and perform work-stealing
 * as worker <worker> until lace_exit() is called.
 */
static void*
lace_worker_thread(void* arg)
{
    unsigned int worker = (unsigned int)(size_t)arg;

    // Initialize data structures
    lace_init_worker(worker);

    // Pin CPU
    lace_pin_worker();

    // Run the steal loop
    WorkerP *__lace_worker = lace_get_worker();
    Task *__lace_dq_head = lace_get_head(__lace_worker);
    lace_steal_loop_WORK(__lace_worker, __lace_dq_head, &lace_quits);

    // Time worker exit event
    lace_time_event(__lace_worker, 9);

    return NULL;
}

#if LACE_MSVC
static unsigned __stdcall lace_worker_thread_win(void* arg)
{
    lace_worker_thread(arg);   // ignore return value
    return 0;
}
#endif

/**
 * Set the verbosity of Lace.
 */
void
lace_set_verbosity(int level)
{
    verbosity = level;
}

/**
 * Set the program stack size of Lace threads
 */
void
lace_set_stacksize(size_t new_stacksize)
{
    stacksize = new_stacksize;
}

unsigned int
lace_get_pu_count(void)
{
    unsigned int N;
#if defined(_WIN32)
    DWORD_PTR processMask = 0, systemMask = 0;
    if (GetProcessAffinityMask(GetCurrentProcess(), &processMask, &systemMask) && processMask) {
        unsigned count = 0;
        DWORD_PTR m = processMask;
        while (m) { m &= (m - 1); count++; }      // popcount
        N = count ? count : 1;
    }
    else {
        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
        N = (unsigned)sysinfo.dwNumberOfProcessors;
    }

#elif defined(HAVE_SCHED_GETAFFINITY) && HAVE_SCHED_GETAFFINITY
    cpu_set_t cs;
    CPU_ZERO(&cs);
    if (sched_getaffinity(0, sizeof(cs), &cs) == 0) {
        N = (unsigned)CPU_COUNT(&cs);
    }
    else {
        N = (unsigned)sysconf(_SC_NPROCESSORS_ONLN);
    }

#elif defined(__linux__) || defined(__APPLE__) || defined(__unix__)
    N = (unsigned)sysconf(_SC_NPROCESSORS_ONLN);

#else
    N = 1;
#endif
    return N;
}

/**
 * Initialize Lace for work-stealing with <n> workers, where
 * each worker gets a task deque with <dqsize> elements.
 */
void
lace_start(unsigned int _n_workers, size_t dequesize)
{
#if LACE_USE_HWLOC
    // Initialize topology and information about cpus
    hwloc_topology_init(&topo);
    hwloc_topology_load(topo);

    n_nodes = (unsigned int)hwloc_get_nbobjs_by_type(topo, HWLOC_OBJ_NODE);
    n_cores = (unsigned int)hwloc_get_nbobjs_by_type(topo, HWLOC_OBJ_CORE);

    // Allowed CPUs for this process/thread
    hwloc_cpuset_t allowed = hwloc_bitmap_alloc();
    if (allowed && hwloc_get_cpubind(topo, allowed, HWLOC_CPUBIND_PROCESS) == 0) {
        int cnt = hwloc_bitmap_weight(allowed);
        n_pus = (cnt > 0) ? (unsigned)cnt : (unsigned)hwloc_get_nbobjs_by_type(topo, HWLOC_OBJ_PU);
    }
    else {
        n_pus = (unsigned)hwloc_get_nbobjs_by_type(topo, HWLOC_OBJ_PU);
    }
    if (allowed) hwloc_bitmap_free(allowed); 
#else
    n_pus = lace_get_pu_count();
#endif

    // Check task size vs cache line size
    cache_line_size = get_cache_line_size();
    size_t task_size = sizeof(Task);
    if (cache_line_size % task_size != 0 && task_size % cache_line_size != 0) {
        // typical values of task_size: 32, 64, 128...
        // lace14.h on 32-bit: 32 bytes; lace14.h on 64-bit: 64 bytes
        // lace14.h on 32-bit: 64 bytes; lace14.h on 64-bit: 128 bytes
        // typical values of cache_line_size: 32, 64, 128...
        // for example some ARM chips have a 32-byte cache line size
        // and some modern ARM chips have a 128-byte cache line size like Apple's CPUs
        fprintf(stderr, "Lace warning: task size %zu and cache line size %zu may be unaligned!\n",
            task_size, cache_line_size);
    }

    // Initialize globals
    n_workers = _n_workers == 0 ? n_pus : _n_workers;
    dqsize = dequesize > 0 ? dequesize : 1048576;
    atomic_store_explicit(&lace_quits, 0, memory_order_relaxed);

#if LACE_USE_HWLOC
    // Distribute workers over cores.
    // It tries to first use all cores, before using multiple PUs per core, avoiding hyperthreading.
    cpusets = malloc(n_workers * sizeof(*cpusets));
    // one way of doing this is just with hwloc_distrib, but this is suboptimal
    // for (int i=0; i<n_workers; i++) cpusets[i] = hwloc_bitmap_alloc();
    // hwloc_obj_t root = hwloc_get_root_obj(topo);
    // hwloc_distrib(topo, &root, 1, cpusets, n_workers, INT_MAX, 0);

    {
        unsigned int i=0;
        hwloc_obj_t cores[n_cores];
        {
            hwloc_obj_t core = NULL;
            while ((core = hwloc_get_next_obj_by_type(topo, HWLOC_OBJ_CORE, core)) != NULL) {
                cores[i++] = core;
            }
        }

        i = 0;
        unsigned int j=0, k=0;
        // i is index of worker, j is index of cpu, k is how many PUs per core we have used
        while (i < n_workers) {
            if (j < n_cores && k < (unsigned)hwloc_bitmap_weight(cores[j]->cpuset)) {
                cpusets[i] = cores[j]->cpuset;
                // grab the kth in cpuset
                // turns out this is slightly slower than just pinning to all threads
                // int idx = hwloc_bitmap_first(cores[j]->cpuset);
                // for (int kk=1; kk<k; kk++) idx = hwloc_bitmap_next(cores[j]->cpuset, idx);
                // hwloc_obj_t pu = hwloc_get_pu_obj_by_os_index(topo, idx);
                // cpusets[i] = pu->cpuset;
                i++;
                j++;
            } else {
                k++;
                for (j=0; j<n_cores; j++) {
                    if (k < (unsigned)hwloc_bitmap_weight(cores[j]->cpuset)) break;
                }
                if (j == n_cores) {
                    j = 0;
                    k = 0;
                }
            }
        }
    }
#endif

    // Initialize Lace barrier
    lace_barrier_init();

    // Allocate array with all workers
    // first make sure that the amount to allocate (n_workers times pointer) is a multiple of cache_line_size
    size_t to_allocate = n_workers * sizeof(void*);
    to_allocate = (to_allocate+cache_line_size-1) & (~(cache_line_size-1));
#if defined(_MSC_VER) || defined(__MINGW64_VERSION_MAJOR)
    workers = _aligned_malloc(to_allocate, cache_line_size);
    workers_p = _aligned_malloc(to_allocate, cache_line_size);
    workers_memory = _aligned_malloc(to_allocate, cache_line_size);
#elif defined(__MINGW32__)
    workers = __mingw_aligned_malloc(to_allocate, cache_line_size);
    workers_p = __mingw_aligned_malloc(to_allocate, cache_line_size);
    workers_memory = __mingw_aligned_malloc(to_allocate, cache_line_size);
#else
    workers = aligned_alloc(cache_line_size, to_allocate);
    workers_p = aligned_alloc(cache_line_size, to_allocate);
    workers_memory = aligned_alloc(cache_line_size, to_allocate);
#endif
    if (workers == 0 || workers_p == 0 || workers_memory == 0) {
        fprintf(stderr, "Lace error: unable to allocate memory for the workers!\n");
        exit(1);
    }

    // Ensure worker array is set to 0 initially
    memset(workers, 0, n_workers*sizeof(Worker*));

    // Compute memory size for each worker
    workers_memory_size = sizeof(worker_data) + sizeof(Task) * dqsize;

#if LACE_MSVC
    // Windows MSVC: stack size is passed to thread creation directly.
    if (stacksize == 0) stacksize = 16777216;
    if (stacksize < 16777216) stacksize = 16777216;
    if (stacksize > 67108864) stacksize = 67108864;
#else
    // Prepare structures for thread creation
    pthread_attr_t worker_attr;
    if (pthread_attr_init(&worker_attr) != 0) {
        fprintf(stderr, "Lace error: unable to initialize thread attributes!\n");
        exit(1);
    }

    // Compute the stack size
    if (stacksize == 0) {
#ifndef _WIN32
        struct rlimit lim;
        if (getrlimit(RLIMIT_STACK, &lim) == 0) stacksize = (size_t)lim.rlim_cur;
        if (stacksize > 67108864) stacksize = 67108864;
#endif
    }
    if (stacksize < 16777216) stacksize = 16777216;
    if (stacksize > 67108864) stacksize = 67108864;

#if LACE_USE_HWLOC
    // Use hwloc to allocate stacks bound to the correct NUMA node
    {
        long ps = sysconf(_SC_PAGESIZE);
        size_t page_size = (ps > 0) ? (size_t)ps : 4096;
        stacksize = (stacksize + page_size - 1) & ~(page_size - 1);
        worker_stack_size = stacksize + page_size;
        worker_stack_page_size = page_size;

        worker_stacks = malloc(n_workers * sizeof(void*));
        if (worker_stacks == NULL) {
            fprintf(stderr, "Lace error: unable to allocate stack pointer array!\n");
            exit(1);
        }

        for (unsigned int i = 0; i < n_workers; i++) {
            hwloc_bitmap_t nodeset = hwloc_bitmap_alloc();
            hwloc_cpuset_to_nodeset(topo, cpusets[i], nodeset);

            void* stack = hwloc_alloc_membind(topo, worker_stack_size,
                nodeset,
                HWLOC_MEMBIND_BIND,
                HWLOC_MEMBIND_BYNODESET);
            hwloc_bitmap_free(nodeset);

            if (stack == NULL) {
                fprintf(stderr, "Lace error: unable to allocate stack for worker %u!\n", i);
                exit(1);
            }

            // Guard page at the low end (stacks grow downward)
#if defined(_WIN32)
            DWORD old_protect;
            if (!VirtualProtect(stack, page_size, PAGE_NOACCESS, &old_protect)) {
#else
            if (mprotect(stack, page_size, PROT_NONE) != 0) {
#endif
                fprintf(stderr, "Lace error: unable to set guard page for worker %u: %s\n",
                    i, strerror(errno));
                exit(1);
            }
            worker_stacks[i] = stack;
            }
        }
#elif !defined(_WIN32)
    // Use mmap so first-touch places pages on the right NUMA node after pinning
    {
        long ps = sysconf(_SC_PAGESIZE);
        size_t page_size = (ps > 0) ? (size_t)ps : 4096;
        stacksize = (stacksize + page_size - 1) & ~(page_size - 1);
        worker_stack_size = stacksize + page_size;
        worker_stack_page_size = page_size;

        worker_stacks = malloc(n_workers * sizeof(void*));
        if (worker_stacks == NULL) {
            fprintf(stderr, "Lace error: unable to allocate stack pointer array!\n");
            exit(1);
        }

        for (unsigned int i = 0; i < n_workers; i++) {
            void* stack = mmap(NULL, worker_stack_size,
                PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE,
                -1, 0);
            if (stack == MAP_FAILED) {
                fprintf(stderr, "Lace error: unable to mmap stack for worker %u: %s\n",
                    i, strerror(errno));
                exit(1);
            }
            if (mprotect(stack, page_size, PROT_NONE) != 0) {
                fprintf(stderr, "Lace error: unable to set guard page for worker %u: %s\n",
                    i, strerror(errno));
                exit(1);
            }
            worker_stacks[i] = stack;
        }
    }
#else
    // _WIN32 without HWLOC: just set stack size, let the OS handle it
    if (pthread_attr_setstacksize(&worker_attr, stacksize) != 0) {
        fprintf(stderr, "Lace error: unable to set stack size to %zu bytes!\n", stacksize);
        exit(1);
    }
#endif
#endif /* !LACE_MSVC */

    if (verbosity) {
#if LACE_USE_HWLOC
        fprintf(stdout, "Lace startup: %u nodes, %u cores, %u logical processors, %d workers.\n", n_nodes, n_cores, n_pus, n_workers);
        // Print resulting CPU sets
        if (verbosity != 0) {
            for (unsigned int i = 0; i < n_workers; ++i) {
                int id;
                hwloc_bitmap_foreach_begin(id, cpusets[i]);
                hwloc_obj_t pu = hwloc_get_pu_obj_by_os_index(topo, (unsigned int)id);
                // find the core
                if (pu != 0) {
                    hwloc_obj_t core = hwloc_get_ancestor_obj_by_type(topo, HWLOC_OBJ_CORE, pu);
                    if (core != 0) {
                        printf("Lace startup: will pin worker thread %u to pu %u (on core %u)\n", i, pu->logical_index, core->logical_index);
                    }
                }
                hwloc_bitmap_foreach_end();
            }
        }
#else
        fprintf(stdout, "Lace startup: %u available cores, %d workers.\n", n_pus, n_workers);
#endif
    }

    // Prepare lace_init structure
    atomic_store_explicit(&lace_newframe.t, NULL, memory_order_relaxed);

    // Initialize counters for pie times
    us_elapsed_start();
    count_at_start = lace_gethrtime();

    /* Report startup if verbose */
    if (verbosity) {
        fprintf(stdout, "Lace startup: creating %d worker threads with program stack %zu bytes.\n", n_workers, stacksize);
    }

#if !LACE_MSVC
    handles = (pthread_t*)malloc(n_workers * sizeof(*handles));
#else
    handles = (HANDLE*)malloc(n_workers * sizeof(*handles));
#endif

    /* Spawn all workers */
    for (unsigned int i = 0; i < n_workers; i++) {
#if !LACE_MSVC
#if LACE_USE_HWLOC || !defined(_WIN32)
        if (pthread_attr_setstack(&worker_attr,
            (char*)worker_stacks[i] + worker_stack_page_size,
            worker_stack_size - worker_stack_page_size) != 0) {
            fprintf(stderr, "Lace error: unable to set stack for worker %u: %s\n",
                i, strerror(errno));
            exit(1);
        }
#endif
        int rc = pthread_create(&handles[i], &worker_attr, lace_worker_thread, (void*)(size_t)i);
        if (rc != 0) {
            fprintf(stderr, "Lace error: unable to create worker thread %u: %s\n", i, strerror(rc));
            exit(1);
        }
#else
        unsigned thread_id;
        handles[i] = (HANDLE)_beginthreadex(
            NULL, (unsigned)stacksize, lace_worker_thread_win,
            (void*)(size_t)i, 0, &thread_id);
        if (handles[i] == 0) {
            fprintf(stderr, "Lace error: failed to create worker thread %u\n", i);
            exit(1);
        }
#endif
    }

#if !LACE_MSVC
    pthread_attr_destroy(&worker_attr);
#endif

	is_running = 1;
}


int lace_is_running(void)
{
    return is_running;
}


#if LACE_COUNT_EVENTS
static uint64_t ctr_all[CTR_MAX];
#endif

/**
 * Reset the counters of Lace.
 */
void
lace_count_reset(void)
{
#if LACE_COUNT_EVENTS
    unsigned int i;
    size_t j;

    for (i = 0; i < n_workers; i++) {
        for (j = 0; j < CTR_MAX; j++) {
            workers_p[i]->ctr[j] = 0;
        }
    }

#if LACE_PIE_TIMES
    for (i = 0; i < n_workers; i++) {
        workers_p[i]->time = lace_gethrtime();
        if (i != 0) workers_p[i]->level = 0;
    }

    us_elapsed_start();
    count_at_start = lace_gethrtime();
#endif
#endif
}

/**
 * Report counters to the given file.
 */
void
lace_count_report_file(FILE* file)
{
#if LACE_COUNT_EVENTS
    unsigned int i;
    size_t j;

    for (j = 0; j < CTR_MAX; j++) ctr_all[j] = 0;
    for (i = 0; i < n_workers; i++) {
        uint64_t* wctr = workers_p[i]->ctr;
        for (j = 0; j < CTR_MAX; j++) {
            ctr_all[j] += wctr[j];
        }
    }

#if LACE_COUNT_TASKS
    for (i = 0; i < n_workers; i++) {
        fprintf(file, "Tasks (%u): %" PRIu64 "\n", i, workers_p[i]->ctr[CTR_tasks]);
    }
    fprintf(file, "Tasks (sum): %" PRIu64 "\n", ctr_all[CTR_tasks]);
    fprintf(file, "\n");
#endif

#if LACE_COUNT_STEALS
    for (i = 0; i < n_workers; i++) {
        fprintf(file, "Steals (%u): %" PRIu64 " good/%" PRIu64 " busy of %" PRIu64 " tries; leaps: %" PRIu64 " good/%" PRIu64 " busy of %" PRIu64 " tries\n", i,
            workers_p[i]->ctr[CTR_steals], workers_p[i]->ctr[CTR_steal_busy],
            workers_p[i]->ctr[CTR_steal_tries], workers_p[i]->ctr[CTR_leaps],
            workers_p[i]->ctr[CTR_leap_busy], workers_p[i]->ctr[CTR_leap_tries]);
    }
    fprintf(file, "Steals (sum): %" PRIu64 " good/%" PRIu64 " busy of %" PRIu64 " tries; leaps: %" PRIu64 " good/%" PRIu64 " busy of %" PRIu64 " tries\n",
        ctr_all[CTR_steals], ctr_all[CTR_steal_busy],
        ctr_all[CTR_steal_tries], ctr_all[CTR_leaps],
        ctr_all[CTR_leap_busy], ctr_all[CTR_leap_tries]);
    fprintf(file, "\n");
#endif

#if LACE_COUNT_STEALS && LACE_COUNT_TASKS
    for (i = 0; i < n_workers; i++) {
        if ((workers_p[i]->ctr[CTR_steals] + workers_p[i]->ctr[CTR_leaps]) > 0) {
            fprintf(file, "Tasks per steal (%u): %" PRIu64 "\n", i,
                workers_p[i]->ctr[CTR_tasks] / (workers_p[i]->ctr[CTR_steals] + workers_p[i]->ctr[CTR_leaps]));
        }
    }
    if ((ctr_all[CTR_steals] + ctr_all[CTR_leaps]) > 0) {
        fprintf(file, "Tasks per steal (sum): %" PRIu64 "\n", ctr_all[CTR_tasks] / (ctr_all[CTR_steals] + ctr_all[CTR_leaps]));
    }
    fprintf(file, "\n");
#endif

#if LACE_COUNT_SPLITS
    for (i = 0; i < n_workers; i++) {
        fprintf(file, "Splits (%u): %" PRIu64 " shrinks, %" PRIu64 " grows, %" PRIu64 " outgoing requests\n", i,
            workers_p[i]->ctr[CTR_split_shrink], workers_p[i]->ctr[CTR_split_grow], workers_p[i]->ctr[CTR_split_req]);
    }
    fprintf(file, "Splits (sum): %" PRIu64 " shrinks, %" PRIu64 " grows, %" PRIu64 " outgoing requests\n",
        ctr_all[CTR_split_shrink], ctr_all[CTR_split_grow], ctr_all[CTR_split_req]);
    fprintf(file, "\n");
#endif

#if LACE_PIE_TIMES
    uint64_t count_at_end = lace_gethrtime();
    double ms = (double)us_elapsed() / 1000.0;
    double count_per_ms = (double)(count_at_end - count_at_start) / ms;
    double dcpm = (double)count_per_ms;

    uint64_t sum_count;
    sum_count = ctr_all[CTR_init] + ctr_all[CTR_wapp] + ctr_all[CTR_lapp] + ctr_all[CTR_wsteal] + ctr_all[CTR_lsteal]
        + ctr_all[CTR_close] + ctr_all[CTR_wstealsucc] + ctr_all[CTR_lstealsucc] + ctr_all[CTR_wsignal]
        + ctr_all[CTR_lsignal];

    fprintf(file, "Measured clock (tick) frequency: %.2f GHz\n", count_per_ms / 1000000.0);
    fprintf(file, "Aggregated time per pie slice, total time: %.2f CPU seconds\n\n", (double)sum_count / (1000 * dcpm));

    for (i = 0; i < n_workers; i++) {
        fprintf(file, "Startup time (%d):    %10.2f ms\n", i, (double)workers_p[i]->ctr[CTR_init] / dcpm);
        fprintf(file, "Steal work (%d):      %10.2f ms\n", i, (double)workers_p[i]->ctr[CTR_wapp] / dcpm);
        fprintf(file, "Leap work (%d):       %10.2f ms\n", i, (double)workers_p[i]->ctr[CTR_lapp] / dcpm);
        fprintf(file, "Steal overhead (%d):  %10.2f ms\n", i, (double)(workers_p[i]->ctr[CTR_wstealsucc] + workers_p[i]->ctr[CTR_wsignal]) / dcpm);
        fprintf(file, "Leap overhead (%d):   %10.2f ms\n", i, (double)(workers_p[i]->ctr[CTR_lstealsucc] + workers_p[i]->ctr[CTR_lsignal]) / dcpm);
        fprintf(file, "Steal search (%d):    %10.2f ms\n", i, (double)(workers_p[i]->ctr[CTR_wsteal] - workers_p[i]->ctr[CTR_wstealsucc] - workers_p[i]->ctr[CTR_wsignal]) / dcpm);
        fprintf(file, "Leap search (%d):     %10.2f ms\n", i, (double)(workers_p[i]->ctr[CTR_lsteal] - workers_p[i]->ctr[CTR_lstealsucc] - workers_p[i]->ctr[CTR_lsignal]) / dcpm);
        fprintf(file, "Backoff time (%d):    %10.2f ms\n", i, (double)workers_p[i]->ctr[CTR_backoff] / dcpm);
        fprintf(file, "Exit time (%d):       %10.2f ms\n", i, (double)workers_p[i]->ctr[CTR_close] / dcpm);
        fprintf(file, "\n");
    }

    fprintf(file, "Startup time (sum):    %10.2f ms\n", (double)ctr_all[CTR_init] / dcpm);
    fprintf(file, "Steal work (sum):      %10.2f ms\n", (double)ctr_all[CTR_wapp] / dcpm);
    fprintf(file, "Leap work (sum):       %10.2f ms\n", (double)ctr_all[CTR_lapp] / dcpm);
    fprintf(file, "Steal overhead (sum):  %10.2f ms\n", (double)(ctr_all[CTR_wstealsucc] + ctr_all[CTR_wsignal]) / dcpm);
    fprintf(file, "Leap overhead (sum):   %10.2f ms\n", (double)(ctr_all[CTR_lstealsucc] + ctr_all[CTR_lsignal]) / dcpm);
    fprintf(file, "Steal search (sum):    %10.2f ms\n", (double)(ctr_all[CTR_wsteal] - ctr_all[CTR_wstealsucc] - ctr_all[CTR_wsignal]) / dcpm);
    fprintf(file, "Leap search (sum):     %10.2f ms\n", (double)(ctr_all[CTR_lsteal] - ctr_all[CTR_lstealsucc] - ctr_all[CTR_lsignal]) / dcpm);
    fprintf(file, "Backoff time (sum):    %10.2f ms\n", (double)ctr_all[CTR_backoff] / dcpm);
    fprintf(file, "Exit time (sum):       %10.2f ms\n", (double)ctr_all[CTR_close] / dcpm);
    fprintf(file, "\n");
#endif
#endif
    (void)file;
    return;
}

/**
 * End Lace. All Workers are signaled to quit.
 * This function waits until all threads are done, then returns.
 */
void lace_stop(void)
{
    atomic_store_explicit(&lace_quits, 1, memory_order_relaxed);

    for (unsigned int i = 0; i < n_workers; i++) {
#if !LACE_MSVC
        pthread_join(handles[i], NULL);
#else
        WaitForSingleObject(handles[i], INFINITE);
        CloseHandle(handles[i]);
#endif
    }

    free(handles);

#if LACE_USE_HWLOC
    if (worker_stacks != NULL) {
        for (unsigned int i = 0; i < n_workers; i++) {
            hwloc_free(topo, worker_stacks[i], worker_stack_size);
        }
        free(worker_stacks);
        worker_stacks = NULL;
    }
#elif !defined(_WIN32)
    if (worker_stacks != NULL) {
        for (unsigned int i = 0; i < n_workers; i++) {
            munmap(worker_stacks[i], worker_stack_size);
        }
        free(worker_stacks);
        worker_stacks = NULL;
    }
#endif

#if LACE_COUNT_EVENTS
    lace_count_report_file(stdout);
#endif

    // finally, destroy the barriers
    lace_barrier_destroy();

    for (unsigned int i = 0; i < n_workers; i++) {
#if defined(_WIN32)
        VirtualFree(workers_memory[i], 0, MEM_RELEASE);
#else
        munmap(workers_memory[i], workers_memory_size);
#endif
    }

#if defined(_MSC_VER) || defined(__MINGW64_VERSION_MAJOR)
    _aligned_free(workers);
    _aligned_free(workers_p);
    _aligned_free(workers_memory);
#elif defined(__MINGW32__)
    __mingw_aligned_free(workers);
    __mingw_aligned_free(workers_p);
    __mingw_aligned_free(workers_memory);
#else
    free(workers);
    free(workers_p);
    free(workers_memory);
#endif

    workers = 0;
    workers_p = 0;
    workers_memory = 0;

    is_running = 0;
}

/**
 * Execute the given <root> task in a new frame (synchronizing with all Lace threads)
 * 1) Creates a new frame
 * 2) LACE BARRIER
 * 3) Execute the <root> task
 * 4) LACE BARRIER
 * 5) Restore the old frame
 */
static void
lace_exec_in_new_frame(WorkerP *__lace_worker, Task *__lace_dq_head, Task *root)
{
    TailSplitNA old;
    uint8_t old_as;

    // save old tail, split, allstolen and initiate new frame
    {
        Worker *wt = __lace_worker->_public;

        old_as = wt->allstolen;
        wt->allstolen = 1;
        old.ts.split = atomic_load_explicit(&wt->ts.ts.split, memory_order_relaxed);
        atomic_store_explicit(&wt->ts.ts.split, 0, memory_order_relaxed);
        atomic_thread_fence(memory_order_seq_cst);
        old.ts.tail = atomic_load_explicit(&wt->ts.ts.tail, memory_order_relaxed);

        TailSplitNA ts_new;
        ts_new.ts.tail = (uint32_t)(__lace_dq_head - __lace_worker->dq);
        ts_new.ts.split = (uint32_t)(__lace_dq_head - __lace_worker->dq);
        atomic_store_explicit(&wt->ts.v, ts_new.v, memory_order_relaxed);

        __lace_worker->split = __lace_dq_head;
        __lace_worker->allstolen = 1;
    }

    // wait until all workers are ready
    lace_barrier();

    // execute task
    root->f(__lace_worker, __lace_dq_head, root);

    // wait until all workers are back (else they may steal from previous frame)
    lace_barrier();

    // restore tail, split, allstolen
    {
        Worker *wt = __lace_worker->_public;
        wt->allstolen = old_as;
        atomic_store_explicit(&wt->ts.v, old.v, memory_order_relaxed);
        __lace_worker->split = __lace_worker->dq + old.ts.split;
        __lace_worker->allstolen = old_as;
    }
}

/**
 * This method is called when there is a new frame (NEWFRAME or TOGETHER)
 * Each Lace worker executes lace_yield to execute the task in a new frame.
 */
void
lace_yield(WorkerP *__lace_worker, Task *__lace_dq_head)
{
    // make a local copy of the task
    Task _t;
    memcpy(&_t, atomic_load_explicit(&lace_newframe.t, memory_order_relaxed), sizeof(Task));

    // wait until all workers have made a local copy
    lace_barrier();

    lace_exec_in_new_frame(__lace_worker, __lace_dq_head, &_t);
}

/**
 * Root task for the TOGETHER method.
 * Ensures after executing, to steal random tasks until done.
 */
TASK(void, lace_together_root, Task*, t, atomic_int*, finished)
{
    // run the root task
    t->f(__lace_worker, __lace_dq_head, t);

    // signal out completion
    atomic_fetch_sub_explicit(finished, 1, memory_order_relaxed);

    // while threads aren't done, steal randomly
    while (atomic_load_explicit(finished, memory_order_relaxed) != 0) STEAL_RANDOM();
}

TASK(void, lace_wrap_together, Task*, task)
{
    /* synchronization integer (decrease by 1 when done...) */
    atomic_int done = (int)n_workers;

    /* wrap task in lace_together_root */
    Task _t2;
    TD_lace_together_root *t2 = (TD_lace_together_root *)&_t2;
    t2->f = lace_together_root_WRAP;
    atomic_store_explicit(&t2->thief, THIEF_TASK, memory_order_relaxed);
    t2->d.args.arg_1 = task;
    t2->d.args.arg_2 = &done;
    atomic_thread_fence(memory_order_release); // no StoreStore reordering

    /* now try to be the one who sets it! */
    while (1) {
        Task *expected = 0;
        if (atomic_compare_exchange_weak(&lace_newframe.t, &expected, &_t2)) break;
        lace_yield(__lace_worker, __lace_dq_head);
    }

    // wait until other workers have made a local copy
    lace_barrier();

    // reset the newframe struct
    atomic_store_explicit(&lace_newframe.t, NULL, memory_order_relaxed);

    lace_exec_in_new_frame(__lace_worker, __lace_dq_head, &_t2);
}

TASK(void, lace_newframe_root, Task*, t, atomic_int*, done)
{
    t->f(__lace_worker, __lace_dq_head, t);
    atomic_thread_fence(memory_order_release); // no StoreStore reordering
    atomic_store_explicit(done, 1, memory_order_relaxed);
}

TASK(void, lace_wrap_newframe, Task*, task)
{
    /* synchronization integer (set to 1 when done...) */
    atomic_int done = 0;

    /* create the lace_steal_loop task for the other workers */
    Task _s;
    TD_lace_steal_loop *s = (TD_lace_steal_loop *)&_s;
    s->f = &lace_steal_loop_WRAP;
    atomic_store_explicit(&s->thief, THIEF_TASK, memory_order_relaxed);
    s->d.args.arg_1 = &done;
    atomic_thread_fence(memory_order_release); // no StoreStore reordering

    /* now try to be the one who sets it! */
    while (1) {
        Task *expected = 0;
        if (atomic_compare_exchange_weak(&lace_newframe.t, &expected, &_s)) break;
        lace_yield(__lace_worker, __lace_dq_head);
    }

    // wait until other workers have made a local copy
    lace_barrier();

    // reset the newframe struct, then wrap and run ours
    atomic_store_explicit(&lace_newframe.t, NULL, memory_order_relaxed);

    /* wrap task in lace_newframe_root */
    Task _t2;
    TD_lace_newframe_root *t2 = (TD_lace_newframe_root *)&_t2;
    t2->f = lace_newframe_root_WRAP;
    atomic_store_explicit(&t2->thief, THIEF_TASK, memory_order_relaxed);
    t2->d.args.arg_1 = task;
    t2->d.args.arg_2 = &done;

    lace_exec_in_new_frame(__lace_worker, __lace_dq_head, &_t2);
}

void
lace_run_together(Task *t)
{
    WorkerP* self = lace_get_worker();
    if (self != 0) {
        lace_wrap_together_CALL(self, lace_get_head(self), t);
    } else {
        RUN(lace_wrap_together, t);
    }
}

void
lace_run_newframe(Task *t)
{
    WorkerP* self = lace_get_worker();
    if (self != 0) {
        lace_wrap_newframe_CALL(self, lace_get_head(self), t);
    } else {
        RUN(lace_wrap_newframe, t);
    }
}

/**
 * Called by _SPAWN functions when the Task stack is full.
 */
void
lace_abort_stack_overflow(void)
{
    fprintf(stderr, "Lace fatal error: Task stack overflow! Aborting.\n");
    exit(-1);
}

#if defined(_WIN32)
void lace_sleep_us(int64_t microseconds)
{
    if (microseconds <= 0) return;

    // Cache QPC frequency once
    static LARGE_INTEGER qpc_freq;
    static LONG qpc_init = 0;
    if (qpc_init == 0) {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        qpc_freq = f;
        InterlockedExchange(&qpc_init, 1);
    }

    // For very short waits, a spin is usually better than Sleep()/timers.
    const uint32_t SPIN_THRESHOLD_US = 300;

    if (microseconds > SPIN_THRESHOLD_US) {
        // Reuse a per-thread waitable timer to avoid Create/Close per call
        static LACE_TLS HANDLE tls_timer = NULL;

        if (tls_timer == NULL) {
            tls_timer = CreateWaitableTimerExW(
                NULL, NULL,
                0, /* optionally CREATE_WAITABLE_TIMER_HIGH_RESOLUTION if you want to try */
                TIMER_MODIFY_STATE | SYNCHRONIZE
            );
        }

        if (tls_timer) {
            // Clamp to avoid signed overflow in 100ns units
            uint64_t us = (uint64_t)microseconds;
            if (us > (uint64_t)(INT64_MAX / 10)) us = (uint64_t)(INT64_MAX / 10);

            LARGE_INTEGER due;
            due.QuadPart = -(LONGLONG)(10ULL * us); // relative, 100ns units
            if (SetWaitableTimer(tls_timer, &due, 0, NULL, NULL, FALSE)) {
                WaitForSingleObject(tls_timer, INFINITE);
                return;
            }
        }

        // Fallback
        Sleep((DWORD)((microseconds + 999) / 1000));
        return;
    }

    // Spin-wait using QPC for sub-ms delays
    LARGE_INTEGER start, now;
    QueryPerformanceCounter(&start);

    const int64_t target_ticks =
        ((int64_t)microseconds * (int64_t)qpc_freq.QuadPart) / 1000000LL;

    const int64_t deadline = start.QuadPart + target_ticks;

    do {
        YieldProcessor();
        QueryPerformanceCounter(&now);
    } while (now.QuadPart < deadline);
}
#endif
