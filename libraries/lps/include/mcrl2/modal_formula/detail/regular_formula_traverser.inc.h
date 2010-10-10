// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/detail/regular_formula_traverser.inc.h
/// \brief add your file description here.

//--- start generated code ---//
void operator()(const regular_formulas::nil& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const regular_formulas::seq& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.left());
  static_cast<Derived&>(*this)(x.right());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const regular_formulas::alt& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.left());
  static_cast<Derived&>(*this)(x.right());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const regular_formulas::trans& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const regular_formulas::trans_or_nil& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const regular_formulas::regular_formula& x)
{
  static_cast<Derived&>(*this).enter(x);
  if (is_nil(x)) { static_cast<Derived&>(*this)(regular_formulas::nil(atermpp::aterm_appl(x))); }
  else if (is_seq(x)) { static_cast<Derived&>(*this)(regular_formulas::seq(atermpp::aterm_appl(x))); }
  else if (is_alt(x)) { static_cast<Derived&>(*this)(regular_formulas::alt(atermpp::aterm_appl(x))); }
  else if (is_trans(x)) { static_cast<Derived&>(*this)(regular_formulas::trans(atermpp::aterm_appl(x))); }
  else if (is_trans_or_nil(x)) { static_cast<Derived&>(*this)(regular_formulas::trans_or_nil(atermpp::aterm_appl(x))); }
  static_cast<Derived&>(*this).leave(x);
}
//--- end generated code ---//
