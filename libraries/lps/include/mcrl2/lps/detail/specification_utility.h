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
#include "mcrl2/data/data.h"

namespace mcrl2 {

namespace lps {

namespace detail {

  /// \brief Returns the left hand sides of the assignments.
  /// \param assignments A sequence of assignments to data variables
  /// \return The left hand sides of the assignments.
  inline
  data::data_variable_list compute_initial_variables(data::data_assignment_list assignments)
  {
    std::vector<data::data_variable> variables; // protection not needed
    variables.reserve(assignments.size());
    for (data::data_assignment_list::iterator i = assignments.begin(); i != assignments.end(); ++i)
    {
      variables.push_back(i->lhs());
    }
    return data::data_variable_list(variables.begin(), variables.end());
  }

  /// \brief Returns the right hand sides of the assignments.
  /// \param assignments A sequence of assignments to data variables
  /// \return The right hand sides of the assignments.
  inline
  data::data_expression_list compute_initial_state(data::data_assignment_list assignments)
  {
    std::vector<data::data_expression> expressions; // protection not needed
    expressions.reserve(assignments.size());
    for (data::data_assignment_list::iterator i = assignments.begin(); i != assignments.end(); ++i)
    {
      expressions.push_back(i->rhs());
    }
    return data::data_expression_list(expressions.begin(), expressions.end());
  }

  /// \brief Returns assignments for the initial state.
  /// \param variables A sequence of data variables
  /// \param initial_state A sequence of data expressions
  /// \return Assignments for the initial state.
  inline
  data::data_assignment_list compute_initial_assignments(data::data_variable_list variables, data::data_expression_list initial_state)
  {
    std::vector<data::data_assignment> assignments; // protection not needed
    assignments.reserve(variables.size());
    data::data_expression_list::iterator j = initial_state.begin();
    for (data::data_variable_list::iterator i = variables.begin(); i != variables.end(); ++i, ++j)
    {
      assignments.push_back(data::data_assignment(*i, *j));
    }
    return data::data_assignment_list(assignments.begin(), assignments.end());
  }

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_SPECIFICATION_UTILITY_H
