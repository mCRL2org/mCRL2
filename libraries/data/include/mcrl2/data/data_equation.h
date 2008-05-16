// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/data_equation.h
/// \brief The class data_equation.

#ifndef MCRL2_DATA_DATA_EQUATION_H
#define MCRL2_DATA_DATA_EQUATION_H

#include <cassert>
#include <string>
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/data/data_variable.h"
#include "mcrl2/core/detail/soundness_checks.h"

namespace mcrl2 {

namespace data {

using atermpp::aterm_appl;
using atermpp::term_list;
using atermpp::aterm;

/// \brief A conditional data equation. The equality holds if
/// the condition evaluates to true. A declaration of variables
/// that can be used in the expressions is included. The condition
/// is optional. In case there is no condition, it has the value
/// nil.
///
//<DataEqn>      ::= DataEqn(<DataVarId>*, <DataExprOrNil>,
//                     <DataExpr>, <DataExpr>)
class data_equation: public aterm_appl
{
  protected:
    data_variable_list m_variables;
    data_expression m_condition;
    data_expression m_lhs;
    data_expression m_rhs;

  public:
    typedef data_variable_list::iterator variable_iterator;

    /// Constructor.
    ///             
    data_equation()
      : aterm_appl(core::detail::constructDataEqn())
    {}

    /// Constructor.
    ///             
    data_equation(aterm_appl t)
     : aterm_appl(t)
    {
      assert(core::detail::check_rule_DataEqn(m_term));
      aterm_appl::iterator i = t.begin();
      m_variables = data_variable_list(*i++);
      m_condition = data_expression(*i++);
      m_lhs       = data_expression(*i++);
      m_rhs       = data_expression(*i);
      assert(data_expr::is_nil(m_condition) || data_expr::is_bool(m_condition));
    } 

    /// Constructor.
    ///             
    data_equation(data_variable_list variables,
                  data_expression    condition,
                  data_expression    lhs,
                  data_expression    rhs
                 )
     : aterm_appl(core::detail::gsMakeDataEqn(variables, condition, lhs, rhs)),
       m_variables(variables),
       m_condition(condition),
       m_lhs(lhs),
       m_rhs(rhs)     
    {
      assert(data_expr::is_nil(m_condition) || data_expr::is_bool(m_condition));
    }

    /// Returns the variables of the equation.
    ///
    data_variable_list variables() const
    {
      return m_variables;
    }

    /// Returns the condition of the equation.
    ///
    data_expression condition() const
    {
      return m_condition;
    }

    /// Returns the left hand side of the equation.
    ///
    data_expression lhs() const
    {
      return m_lhs;
    }

    /// Returns the right hand side of the equation.
    ///
    data_expression rhs() const
    {
      return m_rhs;
    }

    /// Applies a substitution to this data equation and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    data_equation substitute(Substitution f) const
    {
      return data_equation(f(aterm(*this)));
    }
    
    /// Returns true if
    /// <ul>
    /// <li>the types of the left and right hand side are equal</li>
    /// </ul>
    ///
    bool is_well_typed() const
    {
      // check 1)
      if (m_lhs.sort() != m_rhs.sort())
      {
        std::cerr << "data_equation::is_well_typed() failed: the left and right hand sides " << pp(m_lhs) << " and " << pp(m_rhs) << " have different types." << std::endl;
        return false;
      }
      
      return true;
    }   
};

/// \brief singly linked list of data equations
///
typedef term_list<data_equation> data_equation_list;

/// \brief Returns true if the term t is a data equation
inline
bool is_data_equation(aterm_appl t)
{
  return core::detail::gsIsDataEqn(t);
}

} // namespace data

} // namespace mcrl2

/// \cond INTERNAL_DOCS
namespace atermpp
{
using mcrl2::data::data_equation;

template<>
struct aterm_traits<data_equation>
{
  typedef ATermAppl aterm_type;
  static void protect(data_equation t)   { t.protect(); }
  static void unprotect(data_equation t) { t.unprotect(); }
  static void mark(data_equation t)      { t.mark(); }
  static ATerm term(data_equation t)     { return t.term(); }
  static ATerm* ptr(data_equation& t)    { return &t.term(); }
};

} // namespace atermpp
/// \endcond

#endif // MCRL2_DATA_DATA_EQUATION_H
