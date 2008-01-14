// Author(s): Wieger Wesselink
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

namespace lps {

using atermpp::aterm_appl;
using atermpp::term_list;
using atermpp::aterm;

/// \brief data_assignment is an assignment of a data expression to a data variable.
///
class data_assignment: public aterm_appl
{
  protected:
    data_variable   m_lhs;         // left hand side of the assignment
    data_expression m_rhs;         // right hand side of the assignment

  public:
    /// Constructor.
    ///             
    data_assignment()
      : aterm_appl(detail::constructPBExpr())
    {}

    /// Constructor.
    ///             
    data_assignment(aterm_appl t)
     : aterm_appl(t)
    {
      assert(detail::check_rule_DataVarIdInit(m_term));
      aterm_appl::iterator i = t.begin();
      m_lhs = data_variable(*i++);
      m_rhs = data_expression(*i);
    }

    /// Constructor.
    ///             
    data_assignment(data_variable lhs, data_expression rhs)
     : 
       aterm_appl(gsMakeDataVarIdInit(lhs, rhs)),
       m_lhs(lhs),
       m_rhs(rhs)
    {
    }

    /// Returns true if the sorts of the left and right hand side are equal.
    ///
    bool is_well_typed() const
    {
      bool result = m_lhs.sort() == m_rhs.sort();
      if (!result)
      {
        std::cerr << "data_assignment::is_well_typed() failed: the left and right hand sides " << pp(m_lhs) << " and " << pp(m_rhs) << " have different sorts." << std::endl;
        return false;
      }
      return true;
    }

    /// Applies the assignment to t and returns the result.
    ///
    aterm operator()(aterm t) const
    {
      return atermpp::replace(t, aterm(m_lhs), aterm(m_rhs));
    }

    /// Returns the left hand side of the assignment.
    ///
    data_variable lhs() const
    {
      return m_lhs;
    }

    /// Returns the right hand side of the assignment.
    ///
    data_expression rhs() const
    {
      return m_rhs;
    }
};

///////////////////////////////////////////////////////////////////////////////
// data_assignment_list
/// \brief singly linked list of data assignments
///
typedef term_list<data_assignment> data_assignment_list;

/// \brief Returns true if the term t is a data assignment
inline
bool is_data_assignment(aterm_appl t)
{
  return gsIsDataVarIdInit(t);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Makes a data_assignment_list from lhs and rhs
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
/// \brief Utility class for applying a sequence of data assignments
/// Can be used in the replace algorithms of the atermpp library.
//
// A linear search is done in the list of assignments.
// Note that a data_assigment_list doesn't allow for an efficient implementation.
struct assignment_list_substitution
{
  const data_assignment_list& m_assignments;

  /// \cond INTERNAL_DOCS
  struct compare_assignment_lhs
  {
    data_variable m_variable;
  
    compare_assignment_lhs(const data_variable& variable)
      : m_variable(variable)
    {}
    
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
    
    std::pair<aterm_appl, bool> operator()(aterm_appl t) const
    {
      if (!is_data_variable(t))
      {
        return std::pair<aterm_appl, bool>(t, true); // continue the recursion
      }
      data_assignment_list::iterator i = std::find_if(l.begin(), l.end(), compare_assignment_lhs(t));
      if (i == l.end())
      {
        return std::pair<aterm_appl, bool>(t, false); // don't continue the recursion
      }
      else
      {
        return std::pair<aterm_appl, bool>(i->rhs(), false); // don't continue the recursion
      }
    }
  };
  /// \endcond
  
  /// Constructor.
  ///
  assignment_list_substitution(const data_assignment_list& assignments)
    : m_assignments(assignments)
  {}
  
  /// Applies the assignments to the term t and returns the result.
  ///
  aterm operator()(aterm t) const
  {
    return partial_replace(t, assignment_list_substitution_helper(m_assignments));
  }

  private:
    assignment_list_substitution& operator=(const assignment_list_substitution&)
    {
      return *this;
    }
};

} // namespace lps

/// \cond INTERNAL_DOCS
namespace atermpp
{
using lps::data_assignment;

template<>
struct aterm_traits<data_assignment>
{
  typedef ATermAppl aterm_type;
  static void protect(data_assignment t)   { t.protect(); }
  static void unprotect(data_assignment t) { t.unprotect(); }
  static void mark(data_assignment t)      { t.mark(); }
  static ATerm term(data_assignment t)     { return t.term(); }
  static ATerm* ptr(data_assignment& t)    { return &t.term(); }
};
/// \endcond

} // namespace atermpp

#endif // MCRL2_DATA_DATA_ASSIGNMENT_H
