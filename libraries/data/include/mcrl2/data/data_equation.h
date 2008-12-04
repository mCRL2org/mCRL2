// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
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

/// \brief Conditional data equation.
///
/// The equality holds if
/// the condition evaluates to true. A declaration of variables
/// that can be used in the expressions is included. The condition
/// is optional. In case there is no condition, it has the value
/// nil.
///
//<DataEqn>      ::= DataEqn(<DataVarId>*, <DataExprOrNil>,
//                     <DataExpr>, <DataExpr>)
class data_equation: public atermpp::aterm_appl
{
  protected:

    /// \brief The variables that are used in the equation.
    data_variable_list m_variables;

    /// \brief The condition of the equation.
    data_expression m_condition;

    /// \brief The left hand side of the equation.
    data_expression m_lhs;

    /// \brief The right hand side of the equation.
    data_expression m_rhs;

  public:

    /// \brief An iterator for the variable sequence.
    typedef data_variable_list::iterator variable_iterator;

    /// \brief Constructor.
    data_equation()
      : atermpp::aterm_appl(core::detail::constructDataEqn())
    {}

    /// \brief Constructor.
    /// \param t A term.
    data_equation(atermpp::aterm_appl t)
     : atermpp::aterm_appl(t)
    {
      assert(core::detail::check_rule_DataEqn(m_term));
      atermpp::aterm_appl::iterator i = t.begin();
      m_variables = data_variable_list(*i++);
      m_condition = data_expression(*i++);
      m_lhs       = data_expression(*i++);
      m_rhs       = data_expression(*i);
      assert(data_expr::is_nil(m_condition) || data_expr::is_bool(m_condition));
    }

    /// \brief Constructor.
    /// \param variables The variables that are used in the equation.
    /// \param condition The condition of the equation.
    /// \param lhs The left hand side of the equation.
    /// \param rhs The right hand side of the equation.
    data_equation(data_variable_list variables,
                  data_expression    condition,
                  data_expression    lhs,
                  data_expression    rhs
                 )
     : atermpp::aterm_appl(core::detail::gsMakeDataEqn(variables, condition, lhs, rhs)),
       m_variables(variables),
       m_condition(condition),
       m_lhs(lhs),
       m_rhs(rhs)
    {
      assert(data_expr::is_nil(m_condition) || data_expr::is_bool(m_condition));
    }

    /// \brief Returns the variables of the equation.
    /// \return The variables of the equation.
    data_variable_list variables() const
    {
      return m_variables;
    }

    /// \brief Returns the condition of the equation.
    /// \return The condition of the equation.
    data_expression condition() const
    {
      return m_condition;
    }

    /// \brief Returns the left hand side of the equation.
    /// \return The left hand side of the equation.
    data_expression lhs() const
    {
      return m_lhs;
    }

    /// \brief Returns the right hand side of the equation.
    /// \return The right hand side of the equation.
    data_expression rhs() const
    {
      return m_rhs;
    }

    /// \brief Applies a substitution to this data equation and returns the result.
    /// The Substitution object must supply the method atermpp::aterm operator()(atermpp::aterm).
    /// \param f A substitution function.
    /// \return The application of the substitution to the equation.
    template <typename Substitution>
    data_equation substitute(Substitution f) const
    {
      return data_equation(f(atermpp::aterm(*this)));
    }

    /// \brief Returns true if
    /// <ul>
    /// <li>the types of the left and right hand side are equal</li>
    /// </ul>
    /// \return True if the equation is well typed.
    bool is_well_typed() const
    {
      // check 1)
      if (m_lhs.sort() != m_rhs.sort())
      {
        std::cerr << "data_equation::is_well_typed() failed: the left and right hand sides " << mcrl2::core::pp(m_lhs) << " and " << mcrl2::core::pp(m_rhs) << " have different types." << std::endl;
        return false;
      }

      return true;
    }
};

/// \brief singly linked list of data equations
///
typedef atermpp::term_list<data_equation> data_equation_list;

/// \brief Returns true if the term t is a data equation
/// \param t A term.
/// \return True if the term is a data equation.
inline
bool is_data_equation(atermpp::aterm_appl t)
{
  return core::detail::gsIsDataEqn(t);
}

} // namespace data

} // namespace mcrl2

/// \cond INTERNAL_DOCS
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::data::data_equation)
/// \endcond

#endif // MCRL2_DATA_DATA_EQUATION_H
