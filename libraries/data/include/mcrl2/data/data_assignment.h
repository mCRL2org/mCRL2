// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/data_assignment.h
/// \brief The class data_assignment.

#ifndef MCRL2_DATA_DATA_ASSIGNMENT_H
#define MCRL2_DATA_DATA_ASSIGNMENT_H

#include <algorithm>
#include <cassert>
#include <string>
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/atermpp/algorithm.h"          // replace
#include "mcrl2/data/data_variable.h"
#include "mcrl2/core/detail/soundness_checks.h"

namespace mcrl2 {

namespace data {

/// \brief Assignment of a data expression to a data variable.
class data_assignment: public atermpp::aterm_appl
{
  protected:

    /// \brief Left hand side of the assignment
    data_variable   m_lhs;

    /// \brief Right hand side of the assignment
    data_expression m_rhs;

  public:

    /// \brief Constructor.
    data_assignment()
      : atermpp::aterm_appl(core::detail::constructPBExpr())
    {}

    /// \brief Constructor.
    /// \param t A term
    data_assignment(atermpp::aterm_appl t)
     : atermpp::aterm_appl(t)
    {
      assert(core::detail::check_rule_DataVarIdInit(m_term));
      atermpp::aterm_appl::iterator i = t.begin();
      m_lhs = data_variable(*i++);
      m_rhs = data_expression(*i);
    }

    /// \brief Constructor.
    /// \param lhs A
    /// \param rhs A data expression
    data_assignment(data_variable lhs, data_expression rhs)
     :
       atermpp::aterm_appl(core::detail::gsMakeDataVarIdInit(lhs, rhs)),
       m_lhs(lhs),
       m_rhs(rhs)
    {
    }

    /// \brief Returns true if the sorts of the left and right hand side are equal.
    /// \return True if the assignement is well typed.
    bool is_well_typed() const
    {
      bool result = m_lhs.sort() == m_rhs.sort();
      if (!result)
      {
        std::cerr << "data_assignment::is_well_typed() failed: the left and right hand sides " << mcrl2::core::pp(m_lhs) << " and " << mcrl2::core::pp(m_rhs) << " have different sorts." << std::endl;
        return false;
      }
      return true;
    }

    /// \brief Applies the assignment to t and returns the result.
    /// \param t A term.
    /// \return The application of the assignment to the term.
    atermpp::aterm operator()(atermpp::aterm t) const
    {
      return atermpp::replace(t, atermpp::aterm(m_lhs), atermpp::aterm(m_rhs));
    }

    /// \brief Returns the left hand side of the assignment.
    /// \return The left hand side of the assignment.
    data_variable lhs() const
    {
      return m_lhs;
    }

    /// \brief Returns the right hand side of the assignment.
    /// \return The right hand side of the assignment.
    data_expression rhs() const
    {
      return m_rhs;
    }
};

///////////////////////////////////////////////////////////////////////////////
// data_assignment_list
/// \brief Read-only singly linked list of data assignments
typedef atermpp::term_list<data_assignment> data_assignment_list;

/// \brief Returns true if the term t is a data assignment
/// \param t A term
/// \return RETURN_DESCRIPTION
inline
bool is_data_assignment(atermpp::aterm_appl t)
{
  return core::detail::gsIsDataVarIdInit(t);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Makes a data_assignment_list from lhs and rhs
/// \param lhs A sequence of data variables
/// \param rhs A sequence of data expressions
/// \return The corresponding assigment list.
inline
data_assignment_list make_assignment_list(data_variable_list lhs, data_expression_list rhs)
{
  assert(lhs.size() == rhs.size());
  data_assignment_list result;
  data_variable_list::iterator i = lhs.begin();
  data_expression_list::iterator j = rhs.begin();
  for ( ; i != lhs.end(); ++i, ++j)
  {
    result = push_front(result, data_assignment(*i, *j));
  }
  return atermpp::reverse(result);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Returns the right hand sides of the assignments
/// \param l A sequence of assignments to data variables
/// \return The right hand sides of the assignments.
inline
data_expression_list data_assignment_expressions(data_assignment_list l)
{
  data_expression_list result;
  for (data_assignment_list::iterator i = l.begin(); i != l.end(); ++i)
  {
    result = push_front(result, i->rhs());
  }
  return atermpp::reverse(result);
}

///////////////////////////////////////////////////////////////////////////////
// assignment_list_substitution
/// \brief Sequence of data assignments.
// A linear search is done in the list of assignments.
// Note that a data_assigment_list doesn't allow for an efficient implementation.
struct assignment_list_substitution
{
  /// \brief An assignment list.
  const data_assignment_list& m_assignments;

  /// \cond INTERNAL_DOCS
  struct compare_assignment_lhs
  {
    data_variable m_variable;

    compare_assignment_lhs(const data_variable& variable)
      : m_variable(variable)
    {}

    /// \brief FUNCTION_DESCRIPTION
    /// \param a An assignment to a data variable
    /// \return RETURN_DESCRIPTION
    bool operator()(const data_assignment& a) const
    {
      return m_variable == a.lhs();
    }
  };

  struct assignment_list_substitution_helper
  {
    const data_assignment_list& l;

    assignment_list_substitution_helper(const data_assignment_list& l_)
      : l(l_)
    {}

    /// \brief FUNCTION_DESCRIPTION
    /// \param t A term
    /// \return RETURN_DESCRIPTION
    std::pair<atermpp::aterm_appl, bool> operator()(atermpp::aterm_appl t) const
    {
      if (!is_data_variable(t))
      {
        return std::pair<atermpp::aterm_appl, bool>(t, true); // continue the recursion
      }
      data_assignment_list::iterator i = std::find_if(l.begin(), l.end(), compare_assignment_lhs(t));
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
  assignment_list_substitution(const data_assignment_list& assignments)
    : m_assignments(assignments)
  {}

  /// \brief Applies the assignments to the term t and returns the result.
  /// \param t A term.
  /// \return The application of the assignments to the term.
  atermpp::aterm operator()(atermpp::aterm t) const
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

/// \cond INTERNAL_DOCS
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::data::data_assignment)
/// \endcond

#endif // MCRL2_DATA_DATA_ASSIGNMENT_H
