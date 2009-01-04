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
#include "mcrl2/lps/specification.h"

namespace mcrl2 {

namespace lps {

namespace detail {

/// \brief Removes the parameters in to_be_removed from l.
/// \param l A sequence of assignments to data variables
/// \param to_be_removed A subset of parameters of l
/// \return The removal result
inline
data::data_assignment_list remove_parameters(const data::data_assignment_list& l, const std::set<data::data_variable>& to_be_removed)
{
  std::vector<data::data_assignment> a(l.begin(), l.end());
  a.erase(std::remove_if(a.begin(), a.end(), data::detail::has_left_hand_side_in(to_be_removed)), a.end());
  return data::data_assignment_list(a.begin(), a.end()); 
}

/// \brief Removes the parameters in to_be_removed from p.
/// \param p A linear process
/// \param to_be_removed A subset of the process parameters of p
/// \return The removal result
inline
linear_process remove_parameters(const linear_process& p, const std::set<data::data_variable>& to_be_removed)
{
  atermpp::vector<data::data_variable> v(p.process_parameters().begin(), p.process_parameters().end());
  atermpp::vector<summand> s(p.summands().begin(), p.summands().end());

  for (std::set<data::data_variable>::const_iterator i = to_be_removed.begin(); i != to_be_removed.end(); ++i)
  {
    v.erase(std::remove(v.begin(), v.end(), *i), v.end());
  }

  for (std::vector<summand>::iterator i = s.begin(); i != s.end(); ++i)
  {
    *i = set_assignments(*i, remove_parameters(i->assignments(), to_be_removed));
  }
  
  data::data_variable_list new_process_parameters(v.begin(), v.end());
  summand_list new_summands(s.begin(), s.end());
  linear_process result = set_process_parameters(p, new_process_parameters);
  result = set_summands(result, new_summands);

  return result;
}

/// \brief Removes the parameters in to_be_removed from spec.
/// \param spec A linear process specification
/// \param to_be_removed A set of data variables
/// \return The removal result
inline
specification remove_parameters(const specification& spec, const std::set<data::data_variable>& to_be_removed)
{
  process_initializer new_initial_state(spec.initial_process().free_variables(), remove_parameters(spec.initial_process().assignments(), to_be_removed));
  linear_process p = remove_parameters(spec.process(), to_be_removed);
  specification result = set_lps(spec, p);
  result = set_initial_process(result, new_initial_state);
  return result;
}

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_REMOVE_PARAMETERS_H
