// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/translate_user_notation.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_TRANSLATE_USER_NOTATION_H
#define MCRL2_MODAL_FORMULA_TRANSLATE_USER_NOTATION_H

#include "mcrl2/lps/translate_user_notation.h"
#include "mcrl2/modal_formula/action_formula.h"
#include "mcrl2/modal_formula/regular_formula.h"
#include "mcrl2/modal_formula/state_formula.h"

namespace mcrl2 {

namespace action_formulas {

namespace detail {

  template <typename Derived>
  struct translate_user_notation_builder: public lps::detail::translate_user_notation_builder<Derived>
  {
    typedef lps::detail::translate_user_notation_builder<Derived> super; 
    using super::enter;
    using super::leave;
    using super::operator();
#include "mcrl2/modal_formula/detail/action_formula_data_expression_builder.inc.h"
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

} // namespace action_formulas

namespace regular_formulas {

namespace detail {

  template <typename Derived>
  struct translate_user_notation_builder: public action_formulas::detail::translate_user_notation_builder<Derived>
  {
    typedef action_formulas::detail::translate_user_notation_builder<Derived> super;
    using super::enter;
    using super::leave;
    using super::operator();
#include "mcrl2/modal_formula/detail/regular_formula_data_expression_builder.inc.h"
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

} // namespace regular_formulas

namespace state_formulas {

namespace detail {

  template <typename Derived>
  struct translate_user_notation_builder: public regular_formulas::detail::translate_user_notation_builder<Derived>
  {
    typedef regular_formulas::detail::translate_user_notation_builder<Derived> super;
    using super::enter;
    using super::leave;
    using super::operator();
#include "mcrl2/modal_formula/detail/state_formula_data_expression_builder.inc.h"
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

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_TRANSLATE_USER_NOTATION_H
