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

#pragma once
#ifndef __SYLVAN_PLATFORM_H__
#define __SYLVAN_PLATFORM_H__

// Determine if we're on MSVC or not

#if defined(_MSC_VER) && !defined(__clang__)
#define SYLVAN_MSVC 1
#else
#define SYLVAN_MSVC 0
#endif

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include <sylvan_config.h>

/*
// Commented out for now, shouldn't this be handled by a bridge header?
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
*/

// Check endianness

#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__)
    #if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
    #error "Sylvan requires a little-endian target."
    #endif
#elif defined(_WIN32)
  // Windows targets supported by MSVC/MinGW are little-endian
#else
  // Unknown compiler/platform: fail fast rather than guess
    #error "Cannot determine endianness; Sylvan requires little-endian."
#endif

// Portable macros for several keywords and for likely/unlikely

#if SYLVAN_MSVC
    #define SYLVAN_UNUSED
    #define SYLVAN_NOINLINE __declspec(noinline)
    #define SYLVAN_NORETURN __declspec(noreturn)
    #define SYLVAN_ALIGN(N) __declspec(align(N))
    #define SYLVAN_LIKELY(x)   (x)
    #define SYLVAN_UNLIKELY(x) (x)

#elif defined(__GNUC__) || defined(__clang__)
    #define SYLVAN_UNUSED __attribute__((unused))
    #define SYLVAN_NOINLINE __attribute__((noinline))
    #define SYLVAN_NORETURN __attribute__((noreturn))
    #define SYLVAN_ALIGN(N) __attribute__((aligned(N)))
    #define SYLVAN_LIKELY(x)   __builtin_expect(!!(x), 1)
    #define SYLVAN_UNLIKELY(x) __builtin_expect(!!(x), 0)

#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L)
    #define SYLVAN_UNUSED [[maybe_unused]]
    #define SYLVAN_NOINLINE
    #define SYLVAN_NORETURN [[noreturn]]
    #define SYLVAN_ALIGN(N) alignas(N)
    #define SYLVAN_LIKELY(x)   (x)
    #define SYLVAN_UNLIKELY(x) (x)

#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
    #define SYLVAN_UNUSED
    #define SYLVAN_NOINLINE
    #define SYLVAN_NORETURN _Noreturn
    #define SYLVAN_ALIGN(N) _Alignas(N)
    #define SYLVAN_LIKELY(x)   (x)
    #define SYLVAN_UNLIKELY(x) (x)

#else
    #define SYLVAN_UNUSED
    #define SYLVAN_NOINLINE
    #define SYLVAN_NORETURN
    #define SYLVAN_ALIGN(N)
    #define SYLVAN_LIKELY(x)   (x)
    #define SYLVAN_UNLIKELY(x) (x)
#endif

// Thread local storage

#if defined(__cplusplus)
    #define SYLVAN_TLS thread_local
#elif SYLVAN_MSVC
    #define SYLVAN_TLS __declspec(thread)
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L)
    #define SYLVAN_TLS thread_local
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
    #define SYLVAN_TLS _Thread_local
#elif defined(__GNUC__) || defined(__clang__)
    #define SYLVAN_TLS __thread
#else
    #error "No thread-local storage qualifier available"
#endif

// __builtin_ctz and __builtin_ctzll
#if SYLVAN_MSVC
    static inline unsigned int ctz_uint32(uint32_t x) { unsigned long i; return _BitScanForward(&i, x) ? (unsigned int)i : 32; }
    static inline unsigned int ctz_uint64(uint64_t x) { unsigned long i; return _BitScanForward64(&i, x) ? (unsigned int)i : 64; }
    static inline unsigned int clz_uint32(uint32_t x) { unsigned long i; return _BitScanReverse(&i, x) ? 31U - (unsigned int)i : 32; }
    static inline unsigned int clz_uint64(uint64_t x) { unsigned long i; return _BitScanReverse64(&i, x) ? 63U - (unsigned int)i : 64; }
#elif defined(__GNUC__) || defined(__clang__)
    static inline unsigned int ctz_uint32(uint32_t x) { return x ? (unsigned int)__builtin_ctz(x) : 32; }
    static inline unsigned int ctz_uint64(uint64_t x) { return x ? (unsigned int)__builtin_ctzll(x) : 64; }
    static inline unsigned int clz_uint32(uint32_t x) { return x ? (unsigned int)__builtin_clz(x) : 32; }
    static inline unsigned int clz_uint64(uint64_t x) { return x ? (unsigned int)__builtin_clzll(x) : 64; }
#else
    static inline unsigned int ctz_uint32(uint32_t x) {
        if (x == 0) return 32;
        unsigned int n = 0;
        while ((x & 1U) == 0) { ++n; x >>= 1; }
        return n;
    }
    static inline unsigned int ctz_uint64(uint64_t x) {
        if (x == 0) return 64;
        unsigned int n = 0;
        while ((x & 1U) == 0) { ++n; x >>= 1; }
        return n;
    }
    static inline unsigned int clz_uint32(uint32_t x) {
        if (x == 0) return 32;
        unsigned int n = 0;
        while ((x & 0x80000000U) == 0) { ++n; x <<= 1; }
        return n;
    }
    static inline unsigned int clz_uint64(uint64_t x) {
        if (x == 0) return 64;
        unsigned int n = 0;
        while ((x & 0x8000000000000000ULL) == 0) { ++n; x <<= 1; }
        return n;
    }
#endif

// __builtin_popcountll

#if defined(__GNUC__) || defined(__clang__)
    static inline unsigned int popcnt_uint64(uint64_t x)
    {
        return (unsigned int)__builtin_popcountll(x);
    }

#else
    static inline unsigned int popcnt_uint64(uint64_t x)
    {
        x = x - ((x >> 1) & UINT64_C(0x5555555555555555));
        x = (x & UINT64_C(0x3333333333333333)) + ((x >> 2) & UINT64_C(0x3333333333333333));
        x = (x + (x >> 4)) & UINT64_C(0x0f0f0f0f0f0f0f0f);
        return (unsigned int)((x * UINT64_C(0x0101010101010101)) >> 56);
    }
#endif

// Allocate aligned memory (virtual memory if SYLVAN_USE_MMAP is set)

#ifdef __cplusplus
namespace sylvan {
#endif

void* sylvan_alloc_aligned(size_t size);

void sylvan_free_aligned(void* ptr, size_t size);

void sylvan_clear_aligned(void* ptr, size_t size);

#ifdef __cplusplus
} /* namespace */
#endif

#endif
