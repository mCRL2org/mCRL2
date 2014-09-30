// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file general_utilities.h
/// \brief Some general mathematical functions not provided by standard libraries.

#ifndef MCRL2_MATH_UTILITIES_H
#define MCRL2_MATH_UTILITIES_H

#include <cassert>
#include <cmath>
#include <cstdlib>

namespace mcrl2
{
namespace utilities
{

// Compute base 2 logarithm of n, by checking which is the leftmost
// bit that has been set.

inline
size_t ceil_log2(size_t n)
{
  assert(n>0);
  size_t result = 0;
  while(n != 0)
  {
    n = n >> 1;
    ++result;
  }
  return result;
}

// Calculate n^m for numbers n,m of type size_t
inline 
size_t power_size_t(const size_t n_in, const size_t m_in)
{
  size_t result=1;
  size_t n=n_in;
  size_t m=m_in;
  while (m>0)  // Invariant: result*n^m=n_in^m_in;
  {
    if (m % 2==1)
    {
      result=result*n;
    }
    n=n*n;
    m=m/2;
  }
  return result;
}

} // utilities
} // mcrl2


#endif // MCRL2_MATH_UTILITIES_H

