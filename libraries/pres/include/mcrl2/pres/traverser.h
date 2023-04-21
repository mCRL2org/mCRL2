// Author(s): Jan Friso Groote. Based on traverser.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/traverser.h
/// \brief A traverser framework for pres's. 

#ifndef MCRL2_PRES_TRAVERSER_H
#define MCRL2_PRES_TRAVERSER_H

#include "mcrl2/pres/pres.h"

namespace mcrl2
{

namespace pres_system
{

/// \brief Traversal class for pres_expressions. Used as a base class for pres_expression_traverser.
template <typename Derived>
struct pres_expression_traverser_base: public core::traverser<Derived>
{
  typedef core::traverser<Derived> super;
  using super::apply;
  using super::enter;
  using super::leave;

  void apply(pres_expression& result, const data::data_expression& x)
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
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const pres_system::pres_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::pres& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.global_variables());
    static_cast<Derived&>(*this).apply(x.equations());
    static_cast<Derived&>(*this).apply(x.initial_state());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::propositional_variable_instantiation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.parameters());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::minus& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::plus& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::multiply& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::minall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::maxall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::sum& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::pres_expression& x)
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
    else if (pres_system::is_propositional_variable_instantiation(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::propositional_variable_instantiation>(x));
    }
    else if (pres_system::is_minus(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::minus>(x));
    }
    else if (pres_system::is_and(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::and_>(x));
    }
    else if (pres_system::is_or(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::or_>(x));
    }
    else if (pres_system::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::imp>(x));
    }
    else if (pres_system::is_plus(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::plus>(x));
    }
    else if (pres_system::is_multiply(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::multiply>(x));
    }
    else if (pres_system::is_minall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::minall>(x));
    }
    else if (pres_system::is_maxall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::maxall>(x));
    }
    else if (pres_system::is_sum(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::sum>(x));
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
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const pres_system::pres_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::pres& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.equations());
    static_cast<Derived&>(*this).apply(x.initial_state());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::propositional_variable_instantiation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.parameters());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::minus& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::plus& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::multiply& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::minall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::maxall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::sum& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::pres_expression& x)
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
    else if (pres_system::is_propositional_variable_instantiation(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::propositional_variable_instantiation>(x));
    }
    else if (pres_system::is_minus(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::minus>(x));
    }
    else if (pres_system::is_and(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::and_>(x));
    }
    else if (pres_system::is_or(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::or_>(x));
    }
    else if (pres_system::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::imp>(x));
    }
    else if (pres_system::is_plus(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::plus>(x));
    }
    else if (pres_system::is_multiply(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::multiply>(x));
    }
    else if (pres_system::is_minall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::minall>(x));
    }
    else if (pres_system::is_maxall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::maxall>(x));
    }
    else if (pres_system::is_sum(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::sum>(x));
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

//--- start generated add_traverser_pres_expressions code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_pres_expressions: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const pres_system::pres_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::pres& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.equations());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::propositional_variable_instantiation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::minus& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::plus& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::multiply& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::minall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::maxall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::sum& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::pres_expression& x)
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
    else if (pres_system::is_propositional_variable_instantiation(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::propositional_variable_instantiation>(x));
    }
    else if (pres_system::is_minus(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::minus>(x));
    }
    else if (pres_system::is_and(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::and_>(x));
    }
    else if (pres_system::is_or(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::or_>(x));
    }
    else if (pres_system::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::imp>(x));
    }
    else if (pres_system::is_plus(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::plus>(x));
    }
    else if (pres_system::is_multiply(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::multiply>(x));
    }
    else if (pres_system::is_minall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::minall>(x));
    }
    else if (pres_system::is_maxall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::maxall>(x));
    }
    else if (pres_system::is_sum(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::sum>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \\brief Traverser class
template <typename Derived>
struct pres_expression_traverser: public add_traverser_pres_expressions<pres_system::pres_expression_traverser_base, Derived>
{
};
//--- end generated add_traverser_pres_expressions code ---//

//--- start generated add_traverser_variables code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_variables: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const pres_system::pres_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::pres& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.global_variables());
    static_cast<Derived&>(*this).apply(x.equations());
    static_cast<Derived&>(*this).apply(x.initial_state());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::propositional_variable_instantiation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.parameters());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::minus& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::plus& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::multiply& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::minall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::maxall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::sum& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::pres_expression& x)
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
    else if (pres_system::is_propositional_variable_instantiation(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::propositional_variable_instantiation>(x));
    }
    else if (pres_system::is_minus(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::minus>(x));
    }
    else if (pres_system::is_and(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::and_>(x));
    }
    else if (pres_system::is_or(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::or_>(x));
    }
    else if (pres_system::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::imp>(x));
    }
    else if (pres_system::is_plus(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::plus>(x));
    }
    else if (pres_system::is_multiply(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::multiply>(x));
    }
    else if (pres_system::is_minall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::minall>(x));
    }
    else if (pres_system::is_maxall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::maxall>(x));
    }
    else if (pres_system::is_sum(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::sum>(x));
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
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const pres_system::pres_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::pres& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.global_variables());
    static_cast<Derived&>(*this).apply(x.equations());
    static_cast<Derived&>(*this).apply(x.initial_state());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::propositional_variable_instantiation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.name());
    static_cast<Derived&>(*this).apply(x.parameters());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::minus& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::plus& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::multiply& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::minall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::maxall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::sum& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::pres_expression& x)
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
    else if (pres_system::is_propositional_variable_instantiation(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::propositional_variable_instantiation>(x));
    }
    else if (pres_system::is_minus(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::minus>(x));
    }
    else if (pres_system::is_and(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::and_>(x));
    }
    else if (pres_system::is_or(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::or_>(x));
    }
    else if (pres_system::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::imp>(x));
    }
    else if (pres_system::is_plus(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::plus>(x));
    }
    else if (pres_system::is_multiply(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::multiply>(x));
    }
    else if (pres_system::is_minall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::minall>(x));
    }
    else if (pres_system::is_maxall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::maxall>(x));
    }
    else if (pres_system::is_sum(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::sum>(x));
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

} // namespace pres_system

} // namespace mcrl2

#endif // MCRL2_PRES_TRAVERSER_H
