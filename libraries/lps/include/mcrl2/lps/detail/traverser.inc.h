// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/traverser.inc
/// \brief The content of this file is included in other header
/// files, to prevent duplication.

//--- start generated code ---//
void operator()(const action_label& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.name());
  static_cast<Derived&>(*this)(x.sorts());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const action& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.label());
  static_cast<Derived&>(*this)(x.arguments());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const deadlock& x)
{
  static_cast<Derived&>(*this).enter(x);
  if (x.has_time()) static_cast<Derived&>(*this)(x.time());;
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const multi_action& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.actions());
  if (x.has_time()) static_cast<Derived&>(*this)(x.time());;
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const deadlock_summand& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.summation_variables());
  static_cast<Derived&>(*this)(x.condition());
  static_cast<Derived&>(*this)(x.deadlock());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const action_summand& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.summation_variables());
  static_cast<Derived&>(*this)(x.condition());
  static_cast<Derived&>(*this)(x.multi_action());
  static_cast<Derived&>(*this)(x.assignments());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const process_initializer& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.assignments());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const linear_process& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.process_parameters());
  static_cast<Derived&>(*this)(x.deadlock_summands());
  static_cast<Derived&>(*this)(x.action_summands());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const specification& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.action_labels());
  static_cast<Derived&>(*this)(x.global_variables());
  static_cast<Derived&>(*this)(x.process());
  static_cast<Derived&>(*this)(x.initial_process());
  static_cast<Derived&>(*this).leave(x);
}
//--- end generated code ---//
