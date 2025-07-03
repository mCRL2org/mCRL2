// Author(s): Jeroen Keiren and Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/real_utilities.h
/// \brief Contains a number of auxiliary functions to recognize reals. 


#ifndef MCRL2_DATA_REAL_UTILITIES_H
#define MCRL2_DATA_REAL_UTILITIES_H

#include "mcrl2/data/standard_numbers_utility.h"

namespace mcrl2::data::sort_real
{

inline data_expression& real_zero()
{
  static data_expression real_zero=sort_real::real_("0");
  return real_zero;
}

inline data_expression& real_one()
{
  static data_expression real_one=sort_real::real_("1");
  return real_one;
}

inline bool is_zero(const atermpp::aterm& e)
{
  return (e==real_zero());
}

inline bool is_one(const atermpp::aterm& e)
{
  return (e==real_one());
}

/// \brief Functions that returns true if e is a closed real number larger than zero.
inline bool is_larger_zero(const atermpp::aterm& e)
{
  if (sort_real::is_creal_application(e))
  { 
    const application& ea=atermpp::down_cast<application>(e);
    return sort_int::is_cint_application(ea[0]) && 
           sort_nat::is_natural_constant(atermpp::down_cast<application>(ea[0])[0]) && 
           e!=real_zero();
  }
  return false;
}


} // namespace real




#endif // MCRL2_DATA_REAL_UTILITIES_H


