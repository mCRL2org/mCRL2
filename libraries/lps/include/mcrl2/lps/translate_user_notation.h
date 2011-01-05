// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/translate_user_notation.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_TRANSLATE_USER_NOTATION_H
#define MCRL2_LPS_TRANSLATE_USER_NOTATION_H

#include "mcrl2/data/translate_user_notation.h"
#include "mcrl2/lps/specification.h"

namespace mcrl2 {

namespace lps {

namespace detail {

  template <typename Derived>
  class translate_user_notation_builder: public data::detail::translate_user_notation_builder<Derived>
  {
    public:
      typedef data::detail::translate_user_notation_builder<Derived> super;
  
      using super::enter;
      using super::leave;
      using super::operator();

#include "mcrl2/lps/detail/data_expression_builder.inc.h"
  };

} // namespace detail

  template <typename T>
  void translate_user_notation(T& x,
                               typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
  )
  {
    core::make_apply_builder<detail::translate_user_notation_builder>()(x);
  }

  template <typename T>
  T translate_user_notation(const T& x,
                            typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
  )
  {
    return core::make_apply_builder<detail::translate_user_notation_builder>()(x);
  }

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_TRANSLATE_USER_NOTATION_H
