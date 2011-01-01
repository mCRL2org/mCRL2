// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/translate_user_notation_builder.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_TRANSLATE_USER_NOTATION_BUILDER_H
#define MCRL2_PBES_DETAIL_TRANSLATE_USER_NOTATION_BUILDER_H

#include "mcrl2/core/builder.h"
#include "mcrl2/data/detail/translate_user_notation_builder.h"
#include "mcrl2/pbes/pbes.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

  template <typename Derived>
  class translate_user_notation_builder: public data::detail::translate_user_notation_builder<Derived>
  {
    public:
      typedef data::detail::translate_user_notation_builder<Derived> super;
  
      using super::enter;
      using super::leave;
      using super::operator();

#include "mcrl2/pbes/detail/data_expression_builder.inc.h"
  };

  /**
   * Adapts the parse tree from the format after type checking to the
   * format used internally as part of data expressions.
   **/
  template <typename Container>
  void translate_user_notation(pbes<Container>& x)
  {
    return core::make_apply_builder<translate_user_notation_builder>()(x);
  }

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_TRANSLATE_USER_NOTATION_BUILDER_H
