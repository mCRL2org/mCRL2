// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/traverser.inc
/// \brief The content of this file is included in other header
/// files, to prevent duplication.

//--- start generated code ---//
void operator()(const pbes_system::fixpoint_symbol& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const pbes_system::propositional_variable& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.name());
  static_cast<Derived&>(*this)(x.parameters());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const pbes_system::pbes_equation& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.symbol());
  static_cast<Derived&>(*this)(x.variable());
  static_cast<Derived&>(*this)(x.formula());
  static_cast<Derived&>(*this).leave(x);
}

template <typename Container>
void operator()(const pbes_system::pbes<Container>& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.data());
  static_cast<Derived&>(*this)(x.equations());
  static_cast<Derived&>(*this)(x.global_variables());
  static_cast<Derived&>(*this)(x.initial_state());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const pbes_system::propositional_variable_instantiation& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.name());
  static_cast<Derived&>(*this)(x.parameters());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const pbes_system::true_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const pbes_system::false_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const pbes_system::not_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.operand());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const pbes_system::and_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.left());
  static_cast<Derived&>(*this)(x.right());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const pbes_system::or_& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.left());
  static_cast<Derived&>(*this)(x.right());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const pbes_system::imp& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.left());
  static_cast<Derived&>(*this)(x.right());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const pbes_system::forall& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.variables());
  static_cast<Derived&>(*this)(x.body());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const pbes_system::exists& x)
{
  static_cast<Derived&>(*this).enter(x);
  static_cast<Derived&>(*this)(x.variables());
  static_cast<Derived&>(*this)(x.body());
  static_cast<Derived&>(*this).leave(x);
}

void operator()(const pbes_system::pbes_expression& x)
{
  static_cast<Derived&>(*this).enter(x);
  if (pbes_system::is_propositional_variable_instantiation(x)) { static_cast<Derived&>(*this)(pbes_system::propositional_variable_instantiation(atermpp::aterm_appl(x))); }
  else if (pbes_system::is_true(x)) { static_cast<Derived&>(*this)(pbes_system::true_(atermpp::aterm_appl(x))); }
  else if (pbes_system::is_false(x)) { static_cast<Derived&>(*this)(pbes_system::false_(atermpp::aterm_appl(x))); }
  else if (pbes_system::is_not(x)) { static_cast<Derived&>(*this)(pbes_system::not_(atermpp::aterm_appl(x))); }
  else if (pbes_system::is_and(x)) { static_cast<Derived&>(*this)(pbes_system::and_(atermpp::aterm_appl(x))); }
  else if (pbes_system::is_or(x)) { static_cast<Derived&>(*this)(pbes_system::or_(atermpp::aterm_appl(x))); }
  else if (pbes_system::is_imp(x)) { static_cast<Derived&>(*this)(pbes_system::imp(atermpp::aterm_appl(x))); }
  else if (pbes_system::is_forall(x)) { static_cast<Derived&>(*this)(pbes_system::forall(atermpp::aterm_appl(x))); }
  else if (pbes_system::is_exists(x)) { static_cast<Derived&>(*this)(pbes_system::exists(atermpp::aterm_appl(x))); }
  else if (data::is_data_expression(x)) { static_cast<Derived&>(*this)(data::data_expression(atermpp::aterm_appl(x))); }
  static_cast<Derived&>(*this).leave(x);
}
//--- end generated code ---//
