// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/detail/regular_formula_builder.inc
/// \brief The content of this file is included in other header
/// files, to prevent duplication.

//--- start generated code ---//
regular_formula operator()(const nil& x)
{
  static_cast<Derived&>(*this).enter(x);
  regular_formula result = nil();
  static_cast<Derived&>(*this).leave(x);
  return result;
}

regular_formula operator()(const seq& x)
{
  static_cast<Derived&>(*this).enter(x);
  regular_formula result = seq(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

regular_formula operator()(const alt& x)
{
  static_cast<Derived&>(*this).enter(x);
  regular_formula result = alt(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

regular_formula operator()(const trans& x)
{
  static_cast<Derived&>(*this).enter(x);
  regular_formula result = trans(static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

regular_formula operator()(const trans_or_nil& x)
{
  static_cast<Derived&>(*this).enter(x);
  regular_formula result = trans_or_nil(static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

regular_formula operator()(const regular_formula& x)
{
  static_cast<Derived&>(*this).enter(x);
  regular_formula result;
  if (is_nil(x)) { result = static_cast<Derived&>(*this)(nil(atermpp::aterm_appl(x))); }
  else if (is_seq(x)) { result = static_cast<Derived&>(*this)(seq(atermpp::aterm_appl(x))); }
  else if (is_alt(x)) { result = static_cast<Derived&>(*this)(alt(atermpp::aterm_appl(x))); }
  else if (is_trans(x)) { result = static_cast<Derived&>(*this)(trans(atermpp::aterm_appl(x))); }
  else if (is_trans_or_nil(x)) { result = static_cast<Derived&>(*this)(trans_or_nil(atermpp::aterm_appl(x))); }
  static_cast<Derived&>(*this).leave(x);
  return result;
}
//--- end generated code ---//
