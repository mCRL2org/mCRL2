// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/detail/traverser.inc
/// \brief The content of this file is included in other header
/// files, to prevent duplication.

//--- start generated code ---//
void operator()(const true_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const false_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const not_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const and_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.left());
  static_cast<Derived&>(*this)(x.right());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const or_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.left());
  static_cast<Derived&>(*this)(x.right());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const imp& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.left());
  static_cast<Derived&>(*this)(x.right());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const forall& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.variables());
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const exists& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.variables());
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const must& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.formula());
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const may& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.formula());
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const yaled& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const yaled_timed& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.time_stamp());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const delay& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const delay_timed& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.time_stamp());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const variable& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.name());
  static_cast<Derived&>(*this)(x.arguments());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const nu& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.name());
  static_cast<Derived&>(*this)(x.assignments());
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const mu& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.name());
  static_cast<Derived&>(*this)(x.assignments());
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}
//--- end generated code ---//
