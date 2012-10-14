// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/communication_expression.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_COMMUNICATION_EXPRESSION_H
#define MCRL2_PROCESS_COMMUNICATION_EXPRESSION_H

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/process/action_name_multiset.h"

namespace mcrl2
{

namespace process
{

//<CommExpr>     ::= CommExpr(<MultActName>, <StringOrNil>)
/// \brief Communication expression
class communication_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Constructor.
    communication_expression()
      : atermpp::aterm_appl(core::detail::constructCommExpr())
    {}

    /// \brief Constructor.
    /// \param term A term
    communication_expression(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_term_CommExpr(m_term));
    }

    /// \brief Constructor.
    /// \param term A term
    explicit communication_expression(const atermpp::aterm &term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_term_CommExpr(m_term));
    }

    /// \brief Constructor.
    communication_expression(action_name_multiset action_name, core::identifier_string name)
      : atermpp::aterm_appl(core::detail::gsMakeCommExpr(action_name, name))
    {}

    /// \brief Returns the name of the action
    /// \return The name of the action
    const action_name_multiset &action_name() const
    {
      return atermpp::aterm_cast<action_name_multiset>(atermpp::arg1(*this));
    }

    /// \brief Returns the name of the communication expression
    /// \return The name of the communication expression
    const core::identifier_string &name() const
    {
      return atermpp::aterm_cast<core::identifier_string>(atermpp::arg2(*this));
    }
};

/// \brief Read-only singly linked list of communication expressions
typedef atermpp::term_list<communication_expression> communication_expression_list;

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_COMMUNICATION_EXPRESSION_H
