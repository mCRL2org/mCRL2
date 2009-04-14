// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/assignment_list_substitution.h
/// \brief The class data_assignment.

#ifndef MCRL2_NEWDATA_DATA_ASSIGNMENT_H
#define MCRL2_NEWDATA_DATA_ASSIGNMENT_H

#include <utility>
#include "mcrl2/core/substitution_function.h"
#include "mcrl2/new_data/variable.h"
#include "mcrl2/new_data/data_expression.h"

namespace mcrl2 {

namespace new_data {

///////////////////////////////////////////////////////////////////////////////
// assignment_list_substitution
/// \brief Sequence of data assignments.
// A linear search is done in the list of assignments.
// Note that a data_assigment_list doesn't allow for an efficient implementation.
struct assignment_list_substitution: public core::substitution_function<variable, data_expression>
{
  /// \brief An assignment list.
  const assignment_list& m_assignments;

  /// \cond INTERNAL_DOCS
  struct compare_assignment_lhs
  {
    variable m_variable;

    compare_assignment_lhs(const variable& variable)
      : m_variable(variable)
    {}

    /// \brief Function call operator
    /// \param a An assignment to a data variable
    /// \return The function result
    bool operator()(const assignment& a) const
    {
      return m_variable == a.lhs();
    }
  };

  struct assignment_list_substitution_helper
  {
    const assignment_list& l;

    assignment_list_substitution_helper(const assignment_list& l_)
      : l(l_)
    {}

    /// \brief Function call operator
    /// \param t A term
    /// \return The function result
    std::pair<atermpp::aterm_appl, bool> operator()(atermpp::aterm_appl t) const
    {
      if (!data_expression(t).is_variable())
      {
        return std::pair<atermpp::aterm_appl, bool>(t, true); // continue the recursion
      }
      assignment_list::const_iterator i = std::find_if(l.begin(), l.end(), compare_assignment_lhs(t));
      if (i == l.end())
      {
        return std::pair<atermpp::aterm_appl, bool>(t, false); // don't continue the recursion
      }
      else
      {
        return std::pair<atermpp::aterm_appl, bool>(i->rhs(), false); // don't continue the recursion
      }
    }
  };
  /// \endcond

  /// \brief Constructor.
  /// \param assignments A sequence of assignments to data variables
  assignment_list_substitution(const assignment_list& assignments)
    : m_assignments(assignments)
  {}

  /// \brief Applies the assignments to the term t and returns the result.
  /// \param t A term
  /// \return The application of the assignments to the term.
  data_expression operator()(data_expression t) const
  {
    return partial_replace(t, assignment_list_substitution_helper(m_assignments));
  }

  private:
    assignment_list_substitution& operator=(const assignment_list_substitution&)
    {
      return *this;
    }
};

} // namespace data

} // namespace mcrl2

#endif

