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

/// \brief Returns the intersection of a list and a set of variables.
/// \param x A sequence of data variables.
/// \param y A set of data variables.
/// \return The intersection of two sets.
inline
variable_list set_intersection(const variable_list& x, const std::set<variable>& y)
{
  return variable_list(x.begin(), 
                       x.end(), 
                       [](const variable& v){ return v; }, // Identity transformer.
                       [&y](const variable& v){ return y.contains(v); });
}

/// \brief Returns a list of variables equal to the first list, not containing the elements in the second.
/// \param x A list of data variables.
/// \param y A set of data variables.
/// \return The difference of two sets.
inline
variable_list set_difference(const variable_list& x, const std::set<variable>& y)
{
  return variable_list(x.begin(), 
                       x.end(), 
                       [](const variable& v){ return v; }, // Identity transformer.
                       [&y](const variable& v){ return !y.contains(v); });
}

} // namespace mcrl2::data::detail

#endif // MCRL2_DATA_DETAIL_DATA_SEQUENCE_ALGORITHM_H
