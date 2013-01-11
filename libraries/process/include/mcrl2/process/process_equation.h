// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/process_equation.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_PROCESS_EQUATION_H
#define MCRL2_PROCESS_PROCESS_EQUATION_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/process/process_expression.h"
#include "mcrl2/process/process_identifier.h"

namespace mcrl2
{

namespace process
{

/// \brief Process equation
// <ProcEqn> ::= ProcEqn(<ProcVarId>, <DataVarId>*, <ProcExpr>)
class process_equation: public atermpp::aterm_appl
{
  public:
    /// \brief Constructor.
    process_equation()
      : atermpp::aterm_appl(core::detail::constructProcEqn())
    {}

    /// \brief Constructor.
    /// \param term A term
    process_equation(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_term_ProcEqn(*this));
    }

    /// \brief Constructor.
    /// \param term A term
    explicit process_equation(const atermpp::aterm &term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_term_ProcEqn(*this));
    }

    /// \brief Constructor.
    /// \param name A process identifier
    /// \param formal_parameters A sequence of data variables
    /// \param expression A process expression
    process_equation(process_identifier name, data::variable_list formal_parameters, process_expression expression)
      : atermpp::aterm_appl(core::detail::gsMakeProcEqn(
                              name,
                              // atermpp::term_list<data::variable>(formal_parameters.begin(), formal_parameters.end()),
                              formal_parameters,
                              expression))
    {}

    /// \brief Returns the name of the process equation
    /// \return The name of the process equation
    process_identifier identifier() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Returns the formal parameters of the equation
    /// \return The formal parameters of the equation
    data::variable_list formal_parameters() const
    {
      return data::variable_list((*this)[1]);
      /* return data::variable_list(
               atermpp::term_list_iterator<data::variable>(atermpp::list_arg2(*this)),
               atermpp::term_list_iterator<data::variable>()); */
    }

    /// \brief Returns the expression of the process equation
    /// \return The expression of the process equation
    const process_expression& expression() const
    {
      return atermpp::aterm_cast<process_expression>(atermpp::arg3(*this));
    }
};

/// \brief Read-only singly linked list of process equations
typedef atermpp::term_list<process_equation> process_equation_list;

/// \brief vector of process equations
typedef std::vector<process_equation> process_equation_vector;

// template function overloads
std::string pp(const process_equation& x);
std::string pp(const process_equation_list& x);
std::string pp(const process_equation_vector& x);
void normalize_sorts(process_equation_vector& x, const data::data_specification& dataspec);
std::set<data::sort_expression> find_sort_expressions(const process::process_equation_vector& x);

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_PROCESS_EQUATION_H
