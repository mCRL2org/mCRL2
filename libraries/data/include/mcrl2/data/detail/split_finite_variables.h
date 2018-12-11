// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/split_finite_variables.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_DETAIL_SPLIT_FINITE_VARIABLES_H
#define MCRL2_DATA_DETAIL_SPLIT_FINITE_VARIABLES_H

#include "mcrl2/data/data_specification.h"

namespace mcrl2 {

namespace data {

namespace detail {

/// \brief Computes the subset with variables of finite sort and infinite.
// TODO: this should be done more efficiently, by avoiding aterm lists
/// \param variables A sequence of data variables
/// \param data A data specification
/// \param finite_variables A sequence of data variables
/// \param infinite_variables A sequence of data variables
inline
void split_finite_variables(data::variable_list variables, const data::data_specification& data, data::variable_list& finite_variables, data::variable_list& infinite_variables)
{
  std::vector<data::variable> finite;
  std::vector<data::variable> infinite;
  for (const data::variable& v: variables)
  {
    if (data.is_certainly_finite(v.sort()))
    {
      finite.push_back(v);
    }
    else
    {
      infinite.push_back(v);
    }
  }
  finite_variables = data::variable_list(finite.begin(), finite.end());
  infinite_variables = data::variable_list(infinite.begin(), infinite.end());
}

template <typename Rewriter>
inline
void split_enumerable_variables(data::variable_list variables, const data::data_specification& data, const Rewriter& rewr, data::variable_list& enumerable_variables, data::variable_list& non_enumerable_variables)
{
  std::vector<data::variable> enumerable;
  std::vector<data::variable> non_enumerable;
  for (const data::variable& v: variables)
  {
    if (is_enumerable(data, rewr, v.sort()))
    {
      enumerable.push_back(v);
    }
    else
    {
      non_enumerable.push_back(v);
    }
  }
  enumerable_variables = data::variable_list(enumerable.begin(), enumerable.end());
  non_enumerable_variables = data::variable_list(non_enumerable.begin(), non_enumerable.end());
}

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_SPLIT_FINITE_VARIABLES_H
