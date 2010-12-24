// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/detail/translate_user_notation_builder.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_DETAIL_TRANSLATE_USER_NOTATION_BUILDER_H
#define MCRL2_PROCESS_DETAIL_TRANSLATE_USER_NOTATION_BUILDER_H

#include "mcrl2/lps/detail/translate_user_notation_builder.h"
#include "mcrl2/process/builder.h"
#include "mcrl2/process/process_specification.h"

namespace mcrl2 {

namespace process {

namespace detail {

  template <typename Derived>
  class translate_user_notation_builder: public lps::detail::translate_user_notation_builder<Derived>
  {
    public:
      typedef lps::detail::translate_user_notation_builder<Derived> super;
  
      using super::enter;
      using super::leave;
      using super::operator();

#include "mcrl2/process/detail/data_expression_builder.inc.h"
  };

  /**
   * Adapts the parse tree from the format after type checking to the
   * format used internally as part of data expressions.
   **/
  inline
  void translate_user_notation(process_specification& x)
  {
    core::apply_builder<translate_user_notation_builder> builder;
    builder(x);
  }

} // namespace detail

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_DETAIL_TRANSLATE_USER_NOTATION_BUILDER_H
