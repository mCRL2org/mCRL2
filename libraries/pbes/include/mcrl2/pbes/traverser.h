// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/traverser.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_TRAVERSER_H
#define MCRL2_PBES_TRAVERSER_H

#include "mcrl2/pbes/pbes.h"

namespace mcrl2::pbes_system
{

/// \brief Traversal class for pbes_expressions. Used as a base class for pbes_expression_traverser.
template <typename Derived>
struct pbes_expression_traverser_base: public core::traverser<Derived>
{
  using super = core::traverser<Derived>;
  using super::apply;
  using super::enter;
  using super::leave;

  void apply(pbes_expression& result, const data::data_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    result=x;
  }
};

//--- start generated add_traverser_sort_expressions code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_sort_expressions: public Traverser<Derived>
{
  using super = Traverser<Derived>;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const pbes_system::propositional_variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.parameters());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::pbes_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variable());
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::pbes& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.global_variables());
    static_cast<Derived&>(*this).apply(x.equations());
    static_cast<Derived&>(*this).apply(x.initial_state());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::propositional_variable_instantiation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.parameters());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::pbes_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (data::is_variable(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::variable>(x));
    }
    else if (data::is_untyped_data_parameter(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_data_parameter>(x));
    }
    else if (pbes_system::is_propositional_variable_instantiation(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::propositional_variable_instantiation>(x));
    }
    else if (pbes_system::is_not(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::not_>(x));
    }
    else if (pbes_system::is_and(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::and_>(x));
    }
    else if (pbes_system::is_or(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::or_>(x));
    }
    else if (pbes_system::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::imp>(x));
    }
    else if (pbes_system::is_forall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::forall>(x));
    }
    else if (pbes_system::is_exists(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::exists>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \\brief Traverser class
template <typename Derived>
struct sort_expression_traverser: public add_traverser_sort_expressions<data::sort_expression_traverser, Derived>
{
};
//--- end generated add_traverser_sort_expressions code ---//

//--- start generated add_traverser_data_expressions code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_data_expressions: public Traverser<Derived>
{
  using super = Traverser<Derived>;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const pbes_system::pbes_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::pbes& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.equations());
    static_cast<Derived&>(*this).apply(x.initial_state());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::propositional_variable_instantiation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.parameters());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::pbes_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (data::is_variable(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::variable>(x));
    }
    else if (data::is_untyped_data_parameter(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_data_parameter>(x));
    }
    else if (pbes_system::is_propositional_variable_instantiation(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::propositional_variable_instantiation>(x));
    }
    else if (pbes_system::is_not(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::not_>(x));
    }
    else if (pbes_system::is_and(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::and_>(x));
    }
    else if (pbes_system::is_or(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::or_>(x));
    }
    else if (pbes_system::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::imp>(x));
    }
    else if (pbes_system::is_forall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::forall>(x));
    }
    else if (pbes_system::is_exists(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::exists>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \\brief Traverser class
template <typename Derived>
struct data_expression_traverser: public add_traverser_data_expressions<data::data_expression_traverser, Derived>
{
};
//--- end generated add_traverser_data_expressions code ---//

//--- start generated add_traverser_pbes_expressions code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_pbes_expressions: public Traverser<Derived>
{
  using super = Traverser<Derived>;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const pbes_system::pbes_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::pbes& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.equations());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::propositional_variable_instantiation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::pbes_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (data::is_variable(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::variable>(x));
    }
    else if (data::is_untyped_data_parameter(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_data_parameter>(x));
    }
    else if (pbes_system::is_propositional_variable_instantiation(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::propositional_variable_instantiation>(x));
    }
    else if (pbes_system::is_not(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::not_>(x));
    }
    else if (pbes_system::is_and(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::and_>(x));
    }
    else if (pbes_system::is_or(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::or_>(x));
    }
    else if (pbes_system::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::imp>(x));
    }
    else if (pbes_system::is_forall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::forall>(x));
    }
    else if (pbes_system::is_exists(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::exists>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \\brief Traverser class
template <typename Derived>
struct pbes_expression_traverser: public add_traverser_pbes_expressions<pbes_system::pbes_expression_traverser_base, Derived>
{
};
//--- end generated add_traverser_pbes_expressions code ---//

//--- start generated add_traverser_variables code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_variables: public Traverser<Derived>
{
  using super = Traverser<Derived>;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const pbes_system::propositional_variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.parameters());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::pbes_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variable());
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::pbes& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.global_variables());
    static_cast<Derived&>(*this).apply(x.equations());
    static_cast<Derived&>(*this).apply(x.initial_state());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::propositional_variable_instantiation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.parameters());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::pbes_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (data::is_variable(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::variable>(x));
    }
    else if (data::is_untyped_data_parameter(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_data_parameter>(x));
    }
    else if (pbes_system::is_propositional_variable_instantiation(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::propositional_variable_instantiation>(x));
    }
    else if (pbes_system::is_not(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::not_>(x));
    }
    else if (pbes_system::is_and(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::and_>(x));
    }
    else if (pbes_system::is_or(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::or_>(x));
    }
    else if (pbes_system::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::imp>(x));
    }
    else if (pbes_system::is_forall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::forall>(x));
    }
    else if (pbes_system::is_exists(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::exists>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \\brief Traverser class
template <typename Derived>
struct variable_traverser: public add_traverser_variables<data::variable_traverser, Derived>
{
};
//--- end generated add_traverser_variables code ---//

//--- start generated add_traverser_identifier_strings code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_identifier_strings: public Traverser<Derived>
{
  using super = Traverser<Derived>;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const pbes_system::propositional_variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.name());
    static_cast<Derived&>(*this).apply(x.parameters());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::pbes_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variable());
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::pbes& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.global_variables());
    static_cast<Derived&>(*this).apply(x.equations());
    static_cast<Derived&>(*this).apply(x.initial_state());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::propositional_variable_instantiation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.name());
    static_cast<Derived&>(*this).apply(x.parameters());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pbes_system::pbes_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (data::is_variable(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::variable>(x));
    }
    else if (data::is_untyped_data_parameter(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_data_parameter>(x));
    }
    else if (pbes_system::is_propositional_variable_instantiation(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::propositional_variable_instantiation>(x));
    }
    else if (pbes_system::is_not(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::not_>(x));
    }
    else if (pbes_system::is_and(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::and_>(x));
    }
    else if (pbes_system::is_or(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::or_>(x));
    }
    else if (pbes_system::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::imp>(x));
    }
    else if (pbes_system::is_forall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::forall>(x));
    }
    else if (pbes_system::is_exists(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pbes_system::exists>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \\brief Traverser class
template <typename Derived>
struct identifier_string_traverser: public add_traverser_identifier_strings<data::identifier_string_traverser, Derived>
{
};
//--- end generated add_traverser_identifier_strings code ---//

} // namespace mcrl2::pbes_system

#endif // MCRL2_PBES_TRAVERSER_H
