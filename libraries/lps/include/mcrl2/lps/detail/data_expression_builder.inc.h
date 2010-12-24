// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/include/mcrl2/lps/detail/data_expression_builder.inc.h
/// \brief The content of this file is included in other header
/// files, to prevent duplication.

//--- start generated code ---//
lps::action operator()(const lps::action& x)
{
  static_cast<Derived&>(*this).enter(x);  
  lps::action result = lps::action(x.label(), static_cast<Derived&>(*this)(x.arguments()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

void operator()(lps::deadlock& x)
{
  static_cast<Derived&>(*this).enter(x);  
  x.time() = static_cast<Derived&>(*this)(x.time());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(lps::multi_action& x)
{
  static_cast<Derived&>(*this).enter(x);  
  x.actions() = static_cast<Derived&>(*this)(x.actions());
  x.time() = static_cast<Derived&>(*this)(x.time());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(lps::deadlock_summand& x)
{
  static_cast<Derived&>(*this).enter(x);  
  x.summation_variables() = static_cast<Derived&>(*this)(x.summation_variables());
  x.condition() = static_cast<Derived&>(*this)(x.condition());
  static_cast<Derived&>(*this)(x.deadlock());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(lps::action_summand& x)
{
  static_cast<Derived&>(*this).enter(x);  
  x.summation_variables() = static_cast<Derived&>(*this)(x.summation_variables());
  x.condition() = static_cast<Derived&>(*this)(x.condition());
  static_cast<Derived&>(*this)(x.multi_action());
  x.assignments() = static_cast<Derived&>(*this)(x.assignments());
  static_cast<Derived&>(*this).leave(x);
}

lps::process_initializer operator()(const lps::process_initializer& x)
{
  static_cast<Derived&>(*this).enter(x);  
  lps::process_initializer result = lps::process_initializer(static_cast<Derived&>(*this)(x.assignments()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

void operator()(lps::linear_process& x)
{
  static_cast<Derived&>(*this).enter(x);  
  x.process_parameters() = static_cast<Derived&>(*this)(x.process_parameters());
  static_cast<Derived&>(*this)(x.deadlock_summands());
  static_cast<Derived&>(*this)(x.action_summands());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(lps::specification& x)
{
  static_cast<Derived&>(*this).enter(x);  
  static_cast<Derived&>(*this)(x.global_variables());
  static_cast<Derived&>(*this)(x.process());
  x.initial_process() = static_cast<Derived&>(*this)(x.initial_process());
  static_cast<Derived&>(*this).leave(x);
}
//--- end generated code ---//
