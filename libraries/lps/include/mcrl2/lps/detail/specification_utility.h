// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file specification_utility.h
/// \brief Add your file description here.

#ifndef MCRL2_LPS_DETAIL_SPECIFICATION_UTILITY_H
#define MCRL2_LPS_DETAIL_SPECIFICATION_UTILITY_H

#include <vector>
#include "mcrl2/old_data/data.h"

namespace mcrl2 {

namespace lps {

namespace detail {

  /// Create a list containing the left hand sides of the initial assignments.
  ///
  inline
  old_data::data_variable_list compute_initial_variables(old_data::data_assignment_list assignments)
  {
    std::vector<old_data::data_variable> variables; // protection not needed
    variables.reserve(assignments.size());
    for (old_data::data_assignment_list::iterator i = assignments.begin(); i != assignments.end(); ++i)
    {
      variables.push_back(i->lhs());
    }
    return old_data::data_variable_list(variables.begin(), variables.end());
  }
  
  /// Create a list containing the right hand sides of the initial assignments.
  ///
  inline
  old_data::data_expression_list compute_initial_state(old_data::data_assignment_list assignments)
  {
    std::vector<old_data::data_expression> expressions; // protection not needed
    expressions.reserve(assignments.size());
    for (old_data::data_assignment_list::iterator i = assignments.begin(); i != assignments.end(); ++i)
    {
      expressions.push_back(i->rhs());
    }
    return old_data::data_expression_list(expressions.begin(), expressions.end());
  }

  /// Create assignments for the initial state.
  ///
  inline
  old_data::data_assignment_list compute_initial_assignments(old_data::data_variable_list variables, old_data::data_expression_list initial_state)
  {
    std::vector<old_data::data_assignment> assignments; // protection not needed
    assignments.reserve(variables.size());
    old_data::data_expression_list::iterator j = initial_state.begin();
    for (old_data::data_variable_list::iterator i = variables.begin(); i != variables.end(); ++i, ++j)
    {
      assignments.push_back(old_data::data_assignment(*i, *j));
    }
    return old_data::data_assignment_list(assignments.begin(), assignments.end());
  }

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_SPECIFICATION_UTILITY_H
