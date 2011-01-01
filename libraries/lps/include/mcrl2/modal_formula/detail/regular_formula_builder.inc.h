// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/include/mcrl2/modal_formula/detail/regular_formula_builder.inc.h
/// \brief The content of this file is included in other header
/// files, to prevent duplication.

//--- start generated code ---//
regular_formulas::regular_formula operator()(const regular_formulas::nil& x)
{
  static_cast<Derived&>(*this).enter(x);  
  // skip
  static_cast<Derived&>(*this).leave(x);
  return x;
}

regular_formulas::regular_formula operator()(const regular_formulas::seq& x)
{
  static_cast<Derived&>(*this).enter(x);  
  regular_formulas::regular_formula result = regular_formulas::seq(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

regular_formulas::regular_formula operator()(const regular_formulas::alt& x)
{
  static_cast<Derived&>(*this).enter(x);  
  regular_formulas::regular_formula result = regular_formulas::alt(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

regular_formulas::regular_formula operator()(const regular_formulas::trans& x)
{
  static_cast<Derived&>(*this).enter(x);  
  regular_formulas::regular_formula result = regular_formulas::trans(static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

regular_formulas::regular_formula operator()(const regular_formulas::trans_or_nil& x)
{
  static_cast<Derived&>(*this).enter(x);  
  regular_formulas::regular_formula result = regular_formulas::trans_or_nil(static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

regular_formulas::regular_formula operator()(const regular_formulas::regular_formula& x)
{
  static_cast<Derived&>(*this).enter(x);  
  regular_formulas::regular_formula result;
  if (regular_formulas::is_nil(x)) { result = static_cast<Derived&>(*this)(regular_formulas::nil(atermpp::aterm_appl(x))); }
  else if (regular_formulas::is_seq(x)) { result = static_cast<Derived&>(*this)(regular_formulas::seq(atermpp::aterm_appl(x))); }
  else if (regular_formulas::is_alt(x)) { result = static_cast<Derived&>(*this)(regular_formulas::alt(atermpp::aterm_appl(x))); }
  else if (regular_formulas::is_trans(x)) { result = static_cast<Derived&>(*this)(regular_formulas::trans(atermpp::aterm_appl(x))); }
  else if (regular_formulas::is_trans_or_nil(x)) { result = static_cast<Derived&>(*this)(regular_formulas::trans_or_nil(atermpp::aterm_appl(x))); }
  static_cast<Derived&>(*this).leave(x);
  return result;
}
//--- end generated code ---//
