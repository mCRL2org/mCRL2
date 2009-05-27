// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file remove_parameters.h
/// \brief Add your file description here.

#ifndef MCRL2_LPS_DETAIL_REMOVE_PARAMETERS_H
#define MCRL2_LPS_DETAIL_REMOVE_PARAMETERS_H

#include <algorithm>
#include <set>
#include <vector>
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/data/detail/convert.h"
#include "mcrl2/lps/specification.h"

namespace mcrl2 {

namespace lps {

namespace detail {

/// \brief Removes the parameters in to_be_removed from l.
/// \param l A sequence of data variables
/// \param to_be_removed A set of variables, not necessarily elements of l
/// \return The removal result
inline
data::variable_list remove_parameters(const data::variable_list& l, const std::set<data::variable>& to_be_removed)
{
  std::vector<data::variable> result;
  for (data::variable_list::const_iterator i = l.begin(); i != l.end(); ++i)
  {
    if (to_be_removed.find(*i) == to_be_removed.end())
    {
      result.push_back(*i);
    }
  }
  return data::variable_list(result.begin(), result.end());
}

/// \brief Removes the parameters in to_be_removed from l.
/// \param l A sequence of assignments to data variables
/// \param to_be_removed A subset of parameters of l
/// \return The removal result
inline
data::assignment_list remove_parameters(const data::assignment_list& l, const std::set<data::variable>& to_be_removed)
{
  std::vector<data::assignment> a(l.begin(), l.end());
  a.erase(std::remove_if(a.begin(), a.end(), data::detail::has_left_hand_side_in(to_be_removed)), a.end());
  return data::assignment_list(a.begin(), a.end());
}

/// \brief Removes the parameters in to_be_removed from p.
/// \param p A linear process
/// \param to_be_removed A subset of the process parameters of p
inline
void remove_parameters(linear_process& p, const std::set<data::variable>& to_be_removed)
{
  // remove process parameters
  p.process_parameters() = remove_parameters(p.process_parameters(), to_be_removed);

  // remove parameters from assignment lists in summands
  for (action_summand_vector::iterator i = p.action_summands().begin(); i != p.action_summands().end(); ++i)
  {
    i->assignments() = remove_parameters(i->assignments(), to_be_removed);
  }

  // remove free variables
  p.free_variables() = remove_parameters(p.free_variables(), to_be_removed);
}

/// \brief Removes the parameters in to_be_removed from spec.
/// \param spec A linear process specification
/// \param to_be_removed A set of data variables
inline
void remove_parameters(specification& spec, const std::set<data::variable>& to_be_removed)
{
  spec.initial_process() = process_initializer(spec.initial_process().free_variables(), remove_parameters(spec.initial_process().assignments(), to_be_removed));
  remove_parameters(spec.process(), to_be_removed);
}

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_REMOVE_PARAMETERS_H
