// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/data_expression_builder.inc.h
/// \brief The content of this file is included in other header
/// files, to prevent duplication.


//--- PBES expression classes

pbes_expression operator()(const propositional_variable_instantiation& x)
{
  static_cast<Derived&>(*this).enter(x);
  pbes_expression result = propositional_variable_instantiation(x.name(), static_cast<Derived&>(*this)(x.parameters()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

pbes_expression operator()(const forall& x)
{
  static_cast<Derived&>(*this).enter(x);
  pbes_expression result = forall(static_cast<Derived&>(*this)(x.variables()), static_cast<Derived&>(*this)(x.body()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

pbes_expression operator()(const exists& x)
{
  static_cast<Derived&>(*this).enter(x);
  pbes_expression result = exists(static_cast<Derived&>(*this)(x.variables()), static_cast<Derived&>(*this)(x.body()));
  static_cast<Derived&>(*this).leave(x);
  return result;
}

//--- other PBES classes

propositional_variable operator()(const propositional_variable& x)
{
  return propositional_variable(x.name(), static_cast<Derived&>(*this)(x.parameters()));
}

void operator()(pbes_equation& x)
{
  x.variable() = static_cast<Derived&>(*this)(x.variable());
  x.formula() = static_cast<Derived&>(*this)(x.formula());
}

template <typename Container>
void operator()(pbes<Container>& x)
{
  static_cast<Derived&>(*this)(x.equations());
  static_cast<Derived&>(*this)(x.global_variables());
  x.initial_state() = static_cast<Derived&>(*this)(x.initial_state());
}     
