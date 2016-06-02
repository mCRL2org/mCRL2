// Author(s): David N. Jansen, Radboud Universiteit, Nijmegen, The Netherlands
//
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/// \file lts/detail/count_iterations.h
///
/// \brief helper macro for time complexity checks during test runs
///
/// \details We use the macro in this group to check whether the overall time
/// complexity fits in O(m log n).  Although it is difficult to test this in
/// general because of the constant factor in the definition of the O()
/// notation, it is often possible to give a (rather tight) upper bound on the
/// number of iterations of most loops.  This bound is checked by the macro
/// `MAX_ITERATIONS`.
///
/// \author David N. Jansen, Radboud Universiteit, Nijmegen, The Netherlands

#ifndef _COUNT_ITERATIONS_H
#define _COUNT_ITERATIONS_H

#ifndef NDEBUG
    #include <limits>    // used in ilog2() for Clang and g++
#endif
#include <cstddef>       // for size_t
#include <cassert>

#include <boost/preprocessor/cat.hpp>



#ifndef NDEBUG

namespace mcrl2
{
namespace lts
{
namespace detail
{

/// type used to store state (numbers and) counts
typedef size_t state_type;

/// type used to store transition (numbers and) counts
typedef size_t trans_type;


namespace bisim_gjkw
{

/// These global variables serve to provide the number of states and
/// transitions, respectively. They are used by `MAX_ITERATIONS`.
extern state_type _count_iterations_n;
extern trans_type _count_iterations_m;

} // end namespace bisim_gjkw
} // end namespace detail
} // end namespace lts
} // end namespace mcrl2


/// \def _count_iterations_ASSERT
/// \brief first expands its macro argument and then generates an assert()
/// \details This internal macro is similar to `BOOST_PP_CAT`.  It is used
/// to generate a nicer error message if an assertion fails.
#define _count_iterations_ASSERT(a,b) _count_iterations_ASSERT_helper(a,b)
#define _count_iterations_ASSERT_helper(a,b) assert(a ## b)

/// \def MAX_ITERATIONS
/// \brief asserts that a loop is not executed too often
/// \details The macro counts the number of iterations and asserts that this
/// number does not exceed the stated upper bound.  In stating the upper bound,
/// `n` (for the number of states) and `m` (for the number of transitions) can
/// be used.  Also the function `ilog2` is accessible.  If the upper bound is
/// exceeded, an assertion failure causes a runtime error.
/// \param count The number that should be added to the loop iteration count
///              (typically 1, but other values are possible, e. g. to model
///              that the run time of a function call is not constant).
/// \param max   The maximum allowed number of iterations. Variables `n` and
///              `m` and function `ilog2` can be used.
#if defined(__clang__) || defined(__GNUG__) // Clang, GNU g++

#define MAX_ITERATIONS(count,max)                                             \
    do                                                                        \
    {                                                                         \
        static size_t BOOST_PP_CAT(iterations_in_line_, __LINE__) = 0;        \
        BOOST_PP_CAT(iterations_in_line_, __LINE__) += (count);               \
        const struct /* functor for ilog2, simulates a nested function */ {   \
            unsigned operator() (unsigned long long i) const                  \
            {                                                                 \
                return std::numeric_limits<unsigned long long>::digits - 1 -  \
                                                        __builtin_clzll(i);   \
            }                                                                 \
            unsigned operator() (unsigned long i) const                       \
            {                                                                 \
                return std::numeric_limits<unsigned long>::digits - 1 -       \
                                                        __builtin_clzl(i);    \
            }                                                                 \
            unsigned operator() (unsigned i) const                            \
            {                                                                 \
                return std::numeric_limits<unsigned>::digits - 1 -            \
                                                        __builtin_clz(i);     \
            }                                                                 \
        } ilog2 = {};                                           (void) ilog2; \
        const state_type n = bisim_gjkw::_count_iterations_n;   (void) n;     \
        const trans_type m = bisim_gjkw::_count_iterations_m;   (void) m;     \
        _count_iterations_ASSERT(iterations_in_line_, __LINE__ <= (max));     \
    }                                                                         \
    while (0)

#elif defined(_MSC_VER) // Microsoft Visual Studio

#define MAX_ITERATIONS(count,max)                                             \
    do                                                                        \
    {                                                                         \
        static size_t BOOST_PP_CAT(iterations_in_line_, __LINE__) = 0;        \
        BOOST_PP_CAT(iterations_in_line_, __LINE__) += (count);               \
        const struct /* functor for ilog2, simulates a nested function */ {   \
            unsigned operator() (unsigned long i) const                       \
            {                                                                 \
                unsigned long result;                                         \
                _BitScanReverse(&result, i);                                  \
                return result;                                                \
            }                                                                 \
            unsigned operator() (unsigned __int64 i) const                    \
            {                                                                 \
                unsigned long result;                                         \
                _BitScanReverse64(&result, i);                                \
                return result;                                                \
            }                                                                 \
        } ilog2 = {};                                           (void) ilog2; \
        const state_type n = bisim_gjkw::_count_iterations_n;   (void) n;     \
        const trans_type m = bisim_gjkw::_count_iterations_m;   (void) m;     \
        _count_iterations_ASSERT(iterations_in_line_, __LINE__ <= (max));     \
    }                                                                         \
    while (0)

#else

// compiler not detected.  Use a general implementation of ilog2() (which may
// be slower).
#warn "I am using a slow ilog2() implementation because I cannot recognise the compiler."

// The code used in ilog2() is inspired by the Bit Twiddling Hacks collected by
// Sean Eron Anderson.  See http://graphics.stanford.edu/~seander/bithacks.html

#define MAX_ITERATIONS(count,max)                                             \
    do                                                                        \
    {                                                                         \
        static size_t BOOST_PP_CAT(iterations_in_line_, __LINE__) = 0;        \
        BOOST_PP_CAT(iterations_in_line_, __LINE__) += (count);               \
        const struct /* functor for ilog2, simulates a nested function */ {   \
            unsigned operator() (state_type i) const                          \
            {                                                                 \
                register unsigned r = 0;                                      \
                assert(std::numeric_limits<state_type>::digits <= 64);        \
                if (i > 0xFFFFFFFF)  {  i >>= 32; r  = 32;  }                 \
                if (i >     0xFFFF)  {  i >>= 16; r |= 16;  }                 \
                if (i >       0xFF)  {  i >>=  8; r |=  8;  }                 \
                if (i >        0xF)  {  i >>=  4; r |=  4;  }                 \
                if (i >        0x3)  {  i >>=  2; r |=  2;  }                 \
                                                  r |= (i >> 1);              \
                return r;                                                     \
            }                                                                 \
        } ilog2 = {};                                           (void) ilog2; \
        const state_type n = bisim_gjkw::_count_iterations_n;   (void) n;     \
        const trans_type m = bisim_gjkw::_count_iterations_m;   (void) m;     \
        _count_iterations_ASSERT(iterations_in_line_, __LINE__ <= (max));     \
    }                                                                         \
    while (0)

#endif // compiler version detection

#define INIT_MAX_ITERATIONS_STATES(n)                                         \
    do {  bisim_gjkw::_count_iterations_n = (n);  } while (0)
#define INIT_MAX_ITERATIONS_TRANSITIONS(m)                                    \
    do {  bisim_gjkw::_count_iterations_m = (m);  } while (0)

#else // ifndef NDEBUG

#define MAX_ITERATIONS(count,max)           do {  /* do nothing */  } while (0)
#define INIT_MAX_ITERATIONS_STATES(n)       do {  /* do nothing */  } while (0)
#define INIT_MAX_ITERATIONS_TRANSITIONS(m)  do {  /* do nothing */  } while (0)

#endif // ifndef NDEBUG

#endif // ifndef _COUNT_ITERATIONS_H
