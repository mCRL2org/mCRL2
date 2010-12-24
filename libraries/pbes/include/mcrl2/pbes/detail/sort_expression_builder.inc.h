// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/include/mcrl2/pbes/detail/sort_expression_builder.inc.h
/// \brief The content of this file is included in other header
/// files, to prevent duplication.

//--- start generated code ---//
pbes_system::propositional_variable operator()(const pbes_system::propositional_variable& x)
{
  static_cast<Derived&>(*this).enter(x);  
  pbes_system::propositional_variable result = pbes_system::propositional_variable(x.name(), static_cast<Derived&>(*this)(x.parameters()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

void operator()(pbes_system::pbes_equation& x)
{
  static_cast<Derived&>(*this).enter(x);  
  x.variable() = static_cast<Derived&>(*this)(x.variable());
  x.formula() = static_cast<Derived&>(*this)(x.formula());
  static_cast<Derived&>(*this).leave(x);
}

template <typename Container>
void operator()(pbes_system::pbes<Container>& x)
{
  static_cast<Derived&>(*this).enter(x);  
  static_cast<Derived&>(*this)(x.equations());
  static_cast<Derived&>(*this)(x.global_variables());
  x.initial_state() = static_cast<Derived&>(*this)(x.initial_state());
  static_cast<Derived&>(*this).leave(x);
}

pbes_system::pbes_expression operator()(const pbes_system::propositional_variable_instantiation& x)
{
  static_cast<Derived&>(*this).enter(x);  
  pbes_system::pbes_expression result = pbes_system::propositional_variable_instantiation(x.name(), static_cast<Derived&>(*this)(x.parameters()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

pbes_system::pbes_expression operator()(const pbes_system::true_& x)
{
  static_cast<Derived&>(*this).enter(x);  
  // skip
  static_cast<Derived&>(*this).leave(x);
  return x;
}

pbes_system::pbes_expression operator()(const pbes_system::false_& x)
{
  static_cast<Derived&>(*this).enter(x);  
  // skip
  static_cast<Derived&>(*this).leave(x);
  return x;
}

pbes_system::pbes_expression operator()(const pbes_system::not_& x)
{
  static_cast<Derived&>(*this).enter(x);  
  pbes_system::pbes_expression result = pbes_system::not_(static_cast<Derived&>(*this)(x.operand()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

pbes_system::pbes_expression operator()(const pbes_system::and_& x)
{
  static_cast<Derived&>(*this).enter(x);  
  pbes_system::pbes_expression result = pbes_system::and_(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

pbes_system::pbes_expression operator()(const pbes_system::or_& x)
{
  static_cast<Derived&>(*this).enter(x);  
  pbes_system::pbes_expression result = pbes_system::or_(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

pbes_system::pbes_expression operator()(const pbes_system::imp& x)
{
  static_cast<Derived&>(*this).enter(x);  
  pbes_system::pbes_expression result = pbes_system::imp(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

pbes_system::pbes_expression operator()(const pbes_system::forall& x)
{
  static_cast<Derived&>(*this).enter(x);  
  pbes_system::pbes_expression result = pbes_system::forall(static_cast<Derived&>(*this)(x.variables()), static_cast<Derived&>(*this)(x.body()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

pbes_system::pbes_expression operator()(const pbes_system::exists& x)
{
  static_cast<Derived&>(*this).enter(x);  
  pbes_system::pbes_expression result = pbes_system::exists(static_cast<Derived&>(*this)(x.variables()), static_cast<Derived&>(*this)(x.body()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

pbes_system::pbes_expression operator()(const pbes_system::pbes_expression& x)
{
  static_cast<Derived&>(*this).enter(x);  
  pbes_system::pbes_expression result;
  if (data::is_data_expression(x)) { result = static_cast<Derived&>(*this)(data::data_expression(atermpp::aterm_appl(x))); }
  else if (pbes_system::is_propositional_variable_instantiation(x)) { result = static_cast<Derived&>(*this)(pbes_system::propositional_variable_instantiation(atermpp::aterm_appl(x))); }
  else if (pbes_system::is_true(x)) { result = static_cast<Derived&>(*this)(pbes_system::true_(atermpp::aterm_appl(x))); }
  else if (pbes_system::is_false(x)) { result = static_cast<Derived&>(*this)(pbes_system::false_(atermpp::aterm_appl(x))); }
  else if (pbes_system::is_not(x)) { result = static_cast<Derived&>(*this)(pbes_system::not_(atermpp::aterm_appl(x))); }
  else if (pbes_system::is_and(x)) { result = static_cast<Derived&>(*this)(pbes_system::and_(atermpp::aterm_appl(x))); }
  else if (pbes_system::is_or(x)) { result = static_cast<Derived&>(*this)(pbes_system::or_(atermpp::aterm_appl(x))); }
  else if (pbes_system::is_imp(x)) { result = static_cast<Derived&>(*this)(pbes_system::imp(atermpp::aterm_appl(x))); }
  else if (pbes_system::is_forall(x)) { result = static_cast<Derived&>(*this)(pbes_system::forall(atermpp::aterm_appl(x))); }
  else if (pbes_system::is_exists(x)) { result = static_cast<Derived&>(*this)(pbes_system::exists(atermpp::aterm_appl(x))); }
  static_cast<Derived&>(*this).leave(x);
  return result;
}
//--- end generated code ---//
