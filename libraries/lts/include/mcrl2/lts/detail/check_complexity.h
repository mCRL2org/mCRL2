// Author(s): David N. Jansen, Radboud Universiteit, Nijmegen, The Netherlands
//
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/// \file lts/detail/check_complexity.h
///
/// \brief helper macro for time complexity checks during test runs
///
/// \details We use the macro in this group to check whether the overall time
/// complexity fits in O(m log n).  Although it is difficult to test this in
/// general because of the constant factor in the definition of the O()
/// notation, it is often possible to give a (rather tight) upper bound on the
/// number of iterations of most loops.
///
/// \author David N. Jansen, Radboud Universiteit, Nijmegen, The Netherlands

#ifndef _COUNT_ITERATIONS_H
#define _COUNT_ITERATIONS_H

#ifndef NDEBUG
    #include <iostream>  // for cerr
    #include <limits>    // used in ilog2() for Clang and g++
#endif
#include <cstddef>       // for size_t
#include <cassert>
#include <map>


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

#ifndef NDEBUG

/// \fn ilog2(<integer type> i)
/// \brief calculate floor(log_2(i))
/// \details Because some compilers provide a more efficient implementation
/// (using a single, simple assembly instruction) than the standard loop (as
/// shown near the end), we distinguish them:
#if defined(__clang__) || defined(__GNUG__) // Clang, GNU g++

static inline unsigned ilog2(unsigned long long i)
                                                __attribute__((const, unused));
static inline unsigned ilog2(unsigned long i) __attribute__((const, unused));
static inline unsigned ilog2(unsigned i) __attribute__((const, unused));
static inline unsigned ilog2(unsigned long long i)
{
    return std::numeric_limits<unsigned long long>::digits - 1 -
                                                            __builtin_clzll(i);
}
static inline unsigned ilog2(unsigned long i)
{
    return std::numeric_limits<unsigned long>::digits - 1 - __builtin_clzl(i);
}
static inline unsigned ilog2(unsigned i)
{
    return std::numeric_limits<unsigned>::digits - 1 - __builtin_clz(i);
}

#elif defined(_MSC_VER) // Microsoft Visual Studio

static inline unsigned long ilog2(unsigned long i)
{
    unsigned long result;
    _BitScanReverse(&result, i);
    return result;
}
static inline unsigned long ilog2(unsigned __int64 i)
{
    unsigned long result;
    _BitScanReverse64(&result, i);
    return result;
}

#else

// compiler not detected.  Use a general implementation of ilog2() (which may
// be slower).
#warn "I am using a slow ilog2() implementation because I cannot recognise the compiler."

// The code used in ilog2() is inspired by the Bit Twiddling Hacks collected by
// Sean Eron Anderson.  See http://graphics.stanford.edu/~seander/bithacks.html

static inline unsigned ilog2(size_t i)
{
    register unsigned char r = 0;
    assert(std::numeric_limits<state_type>::digits <= 64);
    if (i > 0xFFFFFFFF)  {  i >>= 32; r  = 32;  }
    if (i >     0xFFFF)  {  i >>= 16; r |= 16;  }
    if (i >       0xFF)  {  i >>=  8; r |=  8;  }
    if (i >        0xF)  {  i >>=  4; r |=  4;  }
    if (i >        0x3)  {  i >>=  2; r |=  2;  }
                                      r |= (i >> 1);
    return r;
}

#endif // compiler version detection


class check_complexity
{
private:
    struct single_counter {
        size_t count;
        unsigned short percentage;
    };
    static std::map<const char*, single_counter> counters;
public:
    class iteration_type  {
    private:
        size_t value;

        // private constructor to disallow user to construct new instances.
        iteration_type(size_t value_)  :value(value_)  {  }

        // private assignment operators to disallow user to assign other values
        void operator=(size_t value_)  {  value = value_;  }
        void operator=(const iteration_type& other)  {  value = other.value;  }

        friend class check_complexity;
    public:
        // public copy constructor to allow user to copy existing instances.
        iteration_type(const iteration_type& other)  :value(other.value)  {  }
    };
    static iteration_type n;
    static iteration_type m;
    static iteration_type n_log_n;
    static iteration_type m_log_n;
    static iteration_type primary_m_log_n;
    static iteration_type secondary_m_log_n;
    static iteration_type n_m;

    static void init(state_type n_, trans_type m_)
    {
        assert(n_ <= m_);
        counters.clear();
        n = n_;
        m = m_;
        n_log_n = n_ * ilog2(n_);
        m_log_n = m_ * ilog2(n_);
        primary_m_log_n = m_ * ilog2(n_) * 5 / 2; // I am unsure about this bound.
        secondary_m_log_n = m_ * ilog2(n_) * 5 / 2 + m_; // I am unsure about this bound.
        n_m = n_ * m_;
    }

    static void count(const char* id, size_t counter, const iteration_type max)
    {
        struct single_counter& ctr = counters[id];
        ctr.count += counter;
        if (ctr.count > max.value)
        {
            std::cerr << "Error: counter \"" << id << "\" too large: "
                            "maximum allowed value is " << max.value << "\n";
            exit(EXIT_FAILURE);
        }
        // assert(ctr.count <= (SIZE_MAX - max.value) / 200);
        ctr.percentage = (ctr.count * 200 + max.value) / max.value >> 1;
    }

    static void stats()
    {
        std::cout << "Statistics for LTS with " << n.value << " states and "
                                            << m.value << " transitions:\n";
        for (std::map<const char*, single_counter>::const_iterator iter =
                            counters.begin(); counters.end() != iter; ++iter)
        {
            const struct single_counter& ctr = iter->second;
            std::cout << "Counter \"" << iter->first << "\" reached value "
                << ctr.count << " (" << ctr.percentage << "% of maximum)\n";
        }
    }
};

#else // ifndef NDEBUG

#warn "Time complexity will not be checked."

class check_complexity
{
public:
    class iteration_type  {
    private:
        // private constructor to disallow user to construct new instances.
        iteration_type()  {  }

        // private assignment operators to disallow user to assign other values
        void operator=(size_t)  {  }
        void operator=(const iteration_type&)  {  }
    public:
        // public copy constructor to allow user to copy existing instances.
        iteration_type(iteration_type&)  {  }
    };
    static const iteration_type n;
    static const iteration_type m;
    static const iteration_type n_log_n;
    static const iteration_type m_log_n;
    static const iteration_type n_m;

    static void init(state_type, trans_type)  {  }

    static void count(const char*, size_t, const iteration_type)  {  }

    static void stats()  {  }
};

#endif // ifndef NDEBUG

} // end namespace bisim_gjkw
} // end namespace detail
} // end namespace lts
} // end namespace mcrl2

#endif // ifndef _COUNT_ITERATIONS_H
