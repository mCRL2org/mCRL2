// Author(s): Wieger Wesselink
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
#include "mcrl2/data/data.h"

namespace lps {

namespace detail {

  /// Create a list containing the left hand sides of the initial assignments.
  ///
  inline
  data_variable_list compute_initial_variables(data_assignment_list assignments)
  {
    std::vector<data_variable> variables; // protection not needed
    variables.reserve(assignments.size());
    for (data_assignment_list::iterator i = assignments.begin(); i != assignments.end(); ++i)
    {
      variables.push_back(i->lhs());
    }
    return data_variable_list(variables.begin(), variables.end());
  }
  
  /// Create a list containing the right hand sides of the initial assignments.
  ///
  inline
  data_expression_list compute_initial_state(data_assignment_list assignments)
  {
    std::vector<data_expression> expressions; // protection not needed
    expressions.reserve(assignments.size());
    for (data_assignment_list::iterator i = assignments.begin(); i != assignments.end(); ++i)
    {
      expressions.push_back(i->rhs());
    }
    return data_expression_list(expressions.begin(), expressions.end());
  }

  /// Create assignments for the initial state.
  ///
  inline
  data_assignment_list compute_initial_assignments(data_variable_list variables, data_expression_list initial_state)
  {
    std::vector<data_assignment> assignments; // protection not needed
    assignments.reserve(variables.size());
    data_expression_list::iterator j = initial_state.begin();
    for (data_variable_list::iterator i = variables.begin(); i != variables.end(); ++i, ++j)
    {
      assignments.push_back(data_assignment(*i, *j));
    }
    return data_assignment_list(assignments.begin(), assignments.end());
  }

} // namespace detail

} // namespace lps

#endif // MCRL2_LPS_DETAIL_SPECIFICATION_UTILITY_H
