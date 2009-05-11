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
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/process/process_expression.h"
#include "mcrl2/process/process_identifier.h"

namespace mcrl2 {

namespace process {

  /// \brief Process equation
  // <ProcEqn> ::= ProcEqn(<DataVarId>*, <ProcVarId>, <DataVarId>*, <ProcExpr>)
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
        assert(core::detail::check_term_ProcEqn(m_term));
      }

      /// \brief Constructor.
      /// \param variables1 A sequence of data variables
      /// \param name A process identifier
      /// \param variables2 A sequence of data variables
      /// \param expression A process expression
      process_equation(data::variable_list variables1, process_identifier name, data::variable_list variables2, process_expression expression)
        : atermpp::aterm_appl(core::detail::gsMakeProcEqn(
                                atermpp::term_list<data::variable>(variables1.begin(), variables1.end()),
                                name,
                                atermpp::term_list<data::variable>(variables2.begin(), variables2.end()),
                                expression))
      {}

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
      data::variable_list variables1() const
      {
        using namespace atermpp;
        return data::variable_list(
          atermpp::term_list_iterator<data::variable>(list_arg1(*this)),
          atermpp::term_list_iterator<data::variable>());
      }

      /// \brief Returns the name of the process equation
      /// \return The name of the process equation
      process_identifier name() const
      {
        using namespace atermpp;
        return arg2(*this);
      }

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
      data::variable_list variables2() const
      {
        using namespace atermpp;
        return data::variable_list(
          atermpp::term_list_iterator<data::variable>(list_arg3(*this)),
          atermpp::term_list_iterator<data::variable>());
      }

      /// \brief Returns the expression of the process equation
      /// \return The expression of the process equation
      process_expression expression() const
      {
        using namespace atermpp;
        return arg4(*this);
      }
  };

  /// \brief Traverses the process equation, and writes all sort expressions
  /// that are encountered to the output range [dest, ...).
  /// \param eq A process equation
  /// \param dest An output iterator
  template <typename OutIter>
  void traverse_sort_expressions(const process_equation& eq, OutIter dest)
  {
    data::traverse_sort_expressions(eq.variables1(), dest);
    data::traverse_sort_expressions(eq.variables2(), dest);
  }

  /// \brief Read-only singly linked list of process equations
  typedef atermpp::term_list<process_equation> process_equation_list;

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_PROCESS_EQUATION_H
