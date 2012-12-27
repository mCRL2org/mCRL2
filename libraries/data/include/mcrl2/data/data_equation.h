// Author(s): Jeroen Keiren
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

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/atermpp/container_utility.h"

namespace mcrl2
{

namespace data
{

// predeclare
namespace sort_bool
{
function_symbol const& true_();
}

/// \brief data data_equation.
///
class data_equation: public atermpp::aterm_appl
{
  public:
    /// \brief Constructor.
    ///
    data_equation()
      : atermpp::aterm_appl(core::detail::constructDataEqn())
    {}

    /// \brief Constructor
    ///
    /// \param[in] a An aterm adhering to the internal format.
    data_equation(const aterm& a)
      : atermpp::aterm_appl(a)
    { }

    /// \brief Constructor
    ///
    /// \param[in] variables The free variables of the data_equation.
    /// \param[in] condition The condition of the data_equation.
    /// \param[in] lhs The left hand side of the data_equation.
    /// \param[in] rhs The right hand side of the data_equation.
    template < typename Container >
    data_equation(const Container& variables,
                  const data_expression& condition,
                  const data_expression& lhs,
                  const data_expression& rhs,
                  typename atermpp::detail::enable_if_container< Container, variable >::type* = 0)
      : atermpp::aterm_appl(core::detail::gsMakeDataEqn(
                              variable_list(variables.begin(),variables.end()), condition, lhs, rhs))
    {}

    /// \brief Constructor
    ///
    /// \param[in] variables The free variables of the data_equation.
    /// \param[in] lhs The left hand side of the data_equation.
    /// \param[in] rhs The right hand side of the data_equation.
    /// \post this is the data equation representing the input, with
    ///       condition true
    template < typename Container >
    data_equation(const Container& variables,
                  const data_expression& lhs,
                  const data_expression& rhs,
                  typename atermpp::detail::enable_if_container< Container, variable >::type* = 0)
      : atermpp::aterm_appl(core::detail::gsMakeDataEqn(
                              variable_list(variables.begin(),variables.end()), sort_bool::true_(), lhs, rhs))
    {}

    /// \brief Constructor
    ///
    /// \param[in] condition The condition of the data equation.
    /// \param[in] lhs The left hand side of the data equation.
    /// \param[in] rhs The right hand side of the data equation.
    /// \post this is the data equations representing the input, without
    ///       variables, and condition true
    data_equation(const data_expression& condition,
                  const data_expression& lhs,
                  const data_expression& rhs)
      : atermpp::aterm_appl(core::detail::gsMakeDataEqn(
                              variable_list(), condition, lhs, rhs))
    {}

    /// \brief Constructor
    ///
    /// \param[in] lhs The left hand side of the data equation.
    /// \param[in] rhs The right hand side of the data equation.
    /// \post this is the data equations representing the input, without
    ///       variables, and condition true
    data_equation(const data_expression& lhs,
                  const data_expression& rhs)
      : atermpp::aterm_appl(core::detail::gsMakeDataEqn(
                              variable_list(), sort_bool::true_(), lhs, rhs))
    {}

    /// \brief Returns the variables of the data equation.
    variable_list variables() const
    {
      return variable_list(atermpp::list_arg1(*this));
    }

    /// \brief Returns the condition of the data equation.
    data_expression condition() const
    {
      return data_expression(atermpp::arg2(*this));
    }

    /// \brief Returns the left hand side of the data equation.
    data_expression lhs() const
    {
      return data_expression(atermpp::arg3(*this));
    }

    /// \brief Returns the right hand side of the data equation.
    data_expression rhs() const
    {
      return data_expression(atermpp::arg4(*this));
    }

}; // class data_equation

/// \brief list of data_equations
typedef atermpp::term_list< data_equation >    data_equation_list;

/// \brief list of data_equations
typedef std::vector< data_equation >       data_equation_vector;

// template function overloads
std::string pp(const data_equation& x);
std::string pp(const data_equation_list& x);
std::string pp(const data_equation_vector& x);
data::data_equation translate_user_notation(const data::data_equation& x);
std::set<data::sort_expression> find_sort_expressions(const data::data_equation& x);
std::set<data::function_symbol> find_function_symbols(const data::data_equation& x);

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DATA_EQUATION_H

