// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/process_initialization.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_PROCESS_INITIALIZATION_H
#define MCRL2_PROCESS_PROCESS_INITIALIZATION_H

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/process/process_expression.h"

namespace mcrl2 {

namespace process {

  /// \brief Process initialization
  //<ProcInit>     ::= ProcessInit(<DataVarId>*, <ProcExpr>)
  class process_initialization: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      process_initialization()
        : atermpp::aterm_appl(core::detail::constructProcessInit())
      {}

      /// \brief Constructor.
      /// \param term A term
      process_initialization(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(core::detail::check_term_ProcessInit(m_term));
      }

      process_initialization(data::variable_list variables, process_expression expression)
        : atermpp::aterm_appl(core::detail::gsMakeProcessInit(
              atermpp::term_list<data::variable>(variables.begin(), variables.end()), expression))
      {}

      /// \brief Returns the free variables of the process initialization
      /// \return The free variables of the process initialization
      data::variable_list global_variables() const
      {
        using namespace atermpp;
        return data::variable_list(
          atermpp::term_list_iterator<data::variable>(list_arg1(*this)),
          atermpp::term_list_iterator<data::variable>());
      }

      /// \brief Returns the expression of the process initialization
      /// \return The expression of the process initialization
      process_expression expression() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_PROCESS_INITIALIZATION_H
