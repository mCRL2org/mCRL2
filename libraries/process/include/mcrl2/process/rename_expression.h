// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/rename_expression.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_RENAME_EXPRESSION_H
#define MCRL2_PROCESS_RENAME_EXPRESSION_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/detail/constructors.h"

namespace mcrl2 {

namespace process {

  /// \brief Rename expression
  //<RenameExpr>   ::= RenameExpr(<String>, <String>)
  class rename_expression: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      rename_expression()
        : atermpp::aterm_appl(core::detail::constructRenameExpr())
      {}

      /// \brief Constructor.
      /// \param term A term
      rename_expression(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(core::detail::check_term_RenameExpr(m_term));
      }

      /// \brief Constructor.
      rename_expression(core::identifier_string source, core::identifier_string target)
        : atermpp::aterm_appl(core::detail::gsMakeRenameExpr(source, target))
      {}

      /// \brief Returns the source of the rename rule
      /// \return The source of the rename rule
      core::identifier_string source() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      /// \brief Returns the target of the rename rule
      /// \return The target of the rename rule
      core::identifier_string target() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  /// \brief Read-only singly linked list of rename expressions
  typedef atermpp::term_list<rename_expression> rename_expression_list;

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_RENAME_EXPRESSION_H
