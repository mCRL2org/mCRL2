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
/// \brief helper class for time complexity checks during test runs
///
/// \details We use the class in this group to check whether the overall time
/// complexity fits in O(m log n).  Although it is difficult to test this in
/// general because of the constant factor in the definition of the O()
/// notation, it is often possible to give a (rather tight) upper bound on the
/// number of iterations of most loops.
///
/// \author David N. Jansen, Radboud Universiteit, Nijmegen, The Netherlands

#ifndef _COUNT_ITERATIONS_H
#define _COUNT_ITERATIONS_H

#include <cstddef>       // for size_t
#include <cassert>
#include <cmath>         // for log2()
#include "mcrl2/utilities/logger.h"


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

class check_complexity
{
  private:
    struct single_counter 
    {
        size_t count;
        unsigned short percentage;
    };
    static std::map<const char*, single_counter> counters;
  public:
    class iteration_type  
    {
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
        iteration_type operator+(const iteration_type& other) const
        {
            return iteration_type(value + other.value);
        }
        iteration_type operator*(size_t other) const
        {
            return iteration_type(value * other);
        }
    };
    static iteration_type n;
    static iteration_type m;
    static iteration_type n_log_n;
    static iteration_type m_log_n;

    static void init(state_type n_, trans_type m_)
    {
        assert(n_ > 0);
        // assert(m_ > 0);
        counters.clear();
        n = n_;
        m = m_;
        n_log_n = size_t(n_ * log2((double) n_)) / 2;
        m_log_n = m_ * (unsigned) log2((double) n_);
    }

    static void count(const char* id, size_t counter,const iteration_type& max)
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
        ctr.percentage = (unsigned short)
                              ((ctr.count * 200 + max.value) / max.value >> 1);
    }

    static void stats()
    {
        mCRL2log(log::debug, "check_complexity") << "Statistics for LTS with "
                  << n.value << " states and " << m.value << " transitions:\n";
        for (std::map<const char*, single_counter>::const_iterator iter =
                              counters.begin(); counters.end() != iter; ++iter)
        {
            const struct single_counter& ctr = iter->second;
            mCRL2log(log::debug, "check_complexity") << "Counter \""
                             << iter->first << "\" reached value " << ctr.count
                                << " (" << ctr.percentage << "% of maximum)\n";
        }
    }
};

#else // ifndef NDEBUG

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
        iteration_type(const iteration_type&)  {  }
        const iteration_type& operator+(const iteration_type&) const
        {
            return *this;
        }
        const iteration_type& operator*(size_t) const
        {
            return *this;
        }
    };
    static const iteration_type n;
    static const iteration_type m;
    static const iteration_type n_log_n;
    static const iteration_type m_log_n;

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
