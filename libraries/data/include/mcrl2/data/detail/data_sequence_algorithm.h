// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/data_sequence_algorithm.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_DETAIL_DATA_SEQUENCE_ALGORITHM_H
#define MCRL2_DATA_DETAIL_DATA_SEQUENCE_ALGORITHM_H

#include "mcrl2/data/variable.h"

namespace mcrl2::data::detail
{

/// \brief Returns the intersection of two unordered sets, that are stored in ATerm lists.
/// \param x A sequence of data variables
/// \param y A sequence of data variables
/// \return The intersection of two sets.
inline
variable_list set_intersection(const variable_list& x, const variable_list& y)
{
  if (x == y)
  {
    return x;
  }

  std::vector<variable> result;
  for (const variable& v: x)
  {
    if (std::find(y.begin(), y.end(), v) != y.end())
    {
      result.push_back(v);
    }
  }
  return variable_list(result.begin(), result.end());
}

/// \brief Returns the difference of two unordered sets, that are stored in aterm lists.
/// \param x A sequence of data variables
/// \param y A sequence of data variables
/// \return The difference of two sets.
inline
variable_list set_difference(const variable_list& x, const variable_list& y)
{
  if (x == y)
  {
    return variable_list();
  }

  std::vector<variable> result;
  for (const variable& v: x)
  {
    if (std::find(y.begin(), y.end(), v) == y.end())
    {
      result.push_back(v);
    }
  }
  return variable_list(result.begin(), result.end());
}

} // namespace mcrl2::data::detail

#endif // MCRL2_DATA_DETAIL_DATA_SEQUENCE_ALGORITHM_H
