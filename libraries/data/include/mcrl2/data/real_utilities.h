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

#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/substitutions/map_substitution.h"

namespace mcrl2
{
namespace data
{
namespace sort_real
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

} // namespace real
} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_REAL_UTILITIES_H


