/*
 * Copyright 2026 Tom van Dijk, Formal Methods and Tools, University of Twente
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#if defined(__GLIBC__) && !defined(_GNU_SOURCE)
    #define _GNU_SOURCE
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <sylvan_platform.h>

#if SYLVAN_USE_MMAP && !defined(_WIN32)
    #include <sys/mman.h> // for mmap
#elif defined(_WIN32)
    #include <malloc.h>
#endif

#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <windows.h>
    #include <intrin.h>
#endif

#if defined(__APPLE__)
    #include <sys/sysctl.h>
#endif

#if !SYLVAN_MSVC
    #include <pthread.h>
    #include <unistd.h>
#endif

static_assert((SYLVAN_CACHE_LINE_SIZE& (SYLVAN_CACHE_LINE_SIZE - 1)) == 0,
    "SYLVAN_CACHE_LINE_SIZE must be power of two");

void*
sylvan_alloc_aligned(size_t size)
{
    if (size == 0) return NULL;

#if SYLVAN_USE_MMAP
    // Use virtual memory, either using mmap or with VirtualAlloc
#if defined(_WIN32)
    return VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#else 
    void* p = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return (p == MAP_FAILED) ? NULL : p;
#endif

#elif defined(__MINGW32__)
    void* res = __mingw_aligned_malloc(size, SYLVAN_CACHE_LINE_SIZE);
    if (res != NULL) memset(res, 0, size);
    return res;

#elif defined(_MSC_VER) || defined(__MINGW64_VERSION_MAJOR)
    void* res = _aligned_malloc(size, SYLVAN_CACHE_LINE_SIZE);
    if (res != NULL) memset(res, 0, size);
    return res;

#else
    void* res = NULL;
    /* posix_memalign is widely available on Linux/macOS/MSYS2 */
    if (posix_memalign(&res, SYLVAN_CACHE_LINE_SIZE, size) != 0) return NULL;
    memset(res, 0, size);
    return res;
#endif
}


void
sylvan_free_aligned(void* ptr, size_t size)
{
    if (!ptr) return;

#if SYLVAN_USE_MMAP
#if defined(_WIN32)
    (void)size;
    (void)VirtualFree(ptr, 0, MEM_RELEASE);
#else 
    (void)munmap(ptr, size);
#endif

#elif defined(__MINGW32__)
    (void)size;
    __mingw_aligned_free(ptr);

#elif defined(_MSC_VER) || defined(__MINGW64_VERSION_MAJOR)
    (void)size;
    _aligned_free(ptr);

#else
    (void)size;
    free(ptr);
#endif
}


void
sylvan_clear_aligned(void* ptr, size_t size)
{
    if (!ptr || size == 0) return;

#if SYLVAN_USE_MMAP && defined(_WIN32)
    if (VirtualFree(ptr, size, MEM_DECOMMIT)) {
        if (VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) == ptr) return;
        abort();
    }
#elif SYLVAN_USE_MMAP
    void* res = mmap(ptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
    if (res != MAP_FAILED) return;
#endif

    memset(ptr, 0, size);
}

