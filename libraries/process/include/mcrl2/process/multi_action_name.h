// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/multi_action_name.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_MULTI_ACTION_NAME_H
#define MCRL2_PROCESS_MULTI_ACTION_NAME_H

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/detail/constructors.h"

namespace mcrl2 {

namespace process {

  //<MultActName>  ::= MultActName(<String>+)
  /// \brief Multi-action name
  class multi_action_name: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      multi_action_name(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(core::detail::check_term_MultActName(m_term));
      }

      multi_action_name(core::identifier_string_list names)
        : atermpp::aterm_appl(core::detail::gsMakeMultActName(names))
      {}

      /// \brief Returns the names of the multi-action
      /// \return The names of the multi-action
      core::identifier_string_list names() const
      {
        using namespace atermpp;
        return list_arg1(arg1(*this));
      }
  };

  /// \brief Read-only singly linked list of multi_action_name expressions
  typedef atermpp::term_list<multi_action_name> multi_action_name_list;

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_MULTI_ACTION_NAME_H
