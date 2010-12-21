// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/detail/traverser.inc.h
/// \brief The content of this file is included in other header
/// files, to prevent duplication.

//--- start generated code ---//
void operator()(const bes::boolean_equation& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.symbol());
  static_cast<Derived&>(*this)(x.variable());
  static_cast<Derived&>(*this)(x.formula());
  static_cast<Derived&>(*this).leave(x);
}

template <typename Container>
void operator()(const bes::boolean_equation_system<Container>& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.equations());
  static_cast<Derived&>(*this)(x.initial_state());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const bes::true_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const bes::false_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const bes::not_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const bes::and_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.left());
  static_cast<Derived&>(*this)(x.right());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const bes::or_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.left());
  static_cast<Derived&>(*this)(x.right());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const bes::imp& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.left());
  static_cast<Derived&>(*this)(x.right());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const bes::boolean_variable& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.name());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const bes::boolean_expression& x)
{
  static_cast<Derived&>(*this).enter(x);
  if (bes::is_true(x)) { static_cast<Derived&>(*this)(bes::true_(atermpp::aterm_appl(x))); }
  else if (bes::is_false(x)) { static_cast<Derived&>(*this)(bes::false_(atermpp::aterm_appl(x))); }
  else if (bes::is_not(x)) { static_cast<Derived&>(*this)(bes::not_(atermpp::aterm_appl(x))); }
  else if (bes::is_and(x)) { static_cast<Derived&>(*this)(bes::and_(atermpp::aterm_appl(x))); }
  else if (bes::is_or(x)) { static_cast<Derived&>(*this)(bes::or_(atermpp::aterm_appl(x))); }
  else if (bes::is_imp(x)) { static_cast<Derived&>(*this)(bes::imp(atermpp::aterm_appl(x))); }
  else if (bes::is_boolean_variable(x)) { static_cast<Derived&>(*this)(bes::boolean_variable(atermpp::aterm_appl(x))); }
  static_cast<Derived&>(*this).leave(x);
}
//--- end generated code ---//
