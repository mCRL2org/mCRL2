// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/process_identifier.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_PROCESS_IDENTIFIER_H
#define MCRL2_PROCESS_PROCESS_IDENTIFIER_H

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/process/action_name_multiset.h"

namespace mcrl2 {

namespace process {

  /// \brief Process identifier
  //<ProcVarId>    ::= ProcVarId(<String>, <SortExpr>*)
  class process_identifier: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      process_identifier()
        : atermpp::aterm_appl(core::detail::constructProcVarId())
      {}

      /// \brief Constructor.
      /// \param term A term
      process_identifier(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(core::detail::check_term_ProcVarId(m_term));
      }

      /// \brief Constructor.
      process_identifier(core::identifier_string name, data::sort_expression_list sorts)
        : atermpp::aterm_appl(core::detail::gsMakeProcVarId(name, atermpp::term_list<data::sort_expression>(sorts.begin(), sorts.end())))
      {}

      /// \brief Returns the name of the process identifier
      /// \return The name of the process identifier
      core::identifier_string name() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      /// \brief Returns the sorts of the process identifier
      /// \return The sorts of the process identifier
      data::sort_expression_list sorts() const
      {
        using namespace atermpp;
        return data::sort_expression_list(
          atermpp::term_list_iterator<data::sort_expression>(list_arg2(*this)),
          atermpp::term_list_iterator<data::sort_expression>());
      }
  };

  /// \brief Traverses the process identifier, and writes all sort expressions
  /// that are encountered to the output range [dest, ...).
  /// \param pi A process identifier
  /// \param dest An output iterator
  template <typename OutIter>
  void traverse_sort_expressions(const process_identifier& pi, OutIter dest)
  {
    data::traverse_sort_expressions(pi.sorts(), dest);
  }

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_PROCESS_IDENTIFIER_H
