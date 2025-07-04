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



namespace mcrl2::pres_system
{

/// \brief Traversal class for pres_expressions. Used as a base class for pres_expression_traverser.
template <typename Derived>
struct pres_expression_traverser_base: public core::traverser<Derived>
{
  using super = core::traverser<Derived>;
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
  using super = Traverser<Derived>;
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

  void apply(const pres_system::const_multiply& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::const_multiply_alt& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::infimum& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::supremum& x)
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

  void apply(const pres_system::eqinf& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::eqninf& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::condsm& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.arg1());
    static_cast<Derived&>(*this).apply(x.arg2());
    static_cast<Derived&>(*this).apply(x.arg3());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::condeq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.arg1());
    static_cast<Derived&>(*this).apply(x.arg2());
    static_cast<Derived&>(*this).apply(x.arg3());
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
    else if (pres_system::is_const_multiply(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::const_multiply>(x));
    }
    else if (pres_system::is_const_multiply_alt(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::const_multiply_alt>(x));
    }
    else if (pres_system::is_infimum(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::infimum>(x));
    }
    else if (pres_system::is_supremum(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::supremum>(x));
    }
    else if (pres_system::is_sum(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::sum>(x));
    }
    else if (pres_system::is_eqinf(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::eqinf>(x));
    }
    else if (pres_system::is_eqninf(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::eqninf>(x));
    }
    else if (pres_system::is_condsm(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::condsm>(x));
    }
    else if (pres_system::is_condeq(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::condeq>(x));
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

  void apply(const pres_system::const_multiply& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::const_multiply_alt& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::infimum& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::supremum& x)
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

  void apply(const pres_system::eqinf& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::eqninf& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::condsm& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.arg1());
    static_cast<Derived&>(*this).apply(x.arg2());
    static_cast<Derived&>(*this).apply(x.arg3());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::condeq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.arg1());
    static_cast<Derived&>(*this).apply(x.arg2());
    static_cast<Derived&>(*this).apply(x.arg3());
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
    else if (pres_system::is_const_multiply(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::const_multiply>(x));
    }
    else if (pres_system::is_const_multiply_alt(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::const_multiply_alt>(x));
    }
    else if (pres_system::is_infimum(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::infimum>(x));
    }
    else if (pres_system::is_supremum(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::supremum>(x));
    }
    else if (pres_system::is_sum(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::sum>(x));
    }
    else if (pres_system::is_eqinf(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::eqinf>(x));
    }
    else if (pres_system::is_eqninf(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::eqninf>(x));
    }
    else if (pres_system::is_condsm(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::condsm>(x));
    }
    else if (pres_system::is_condeq(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::condeq>(x));
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
  using super = Traverser<Derived>;
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

  void apply(const pres_system::const_multiply& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::const_multiply_alt& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::infimum& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::supremum& x)
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

  void apply(const pres_system::eqinf& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::eqninf& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::condsm& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.arg1());
    static_cast<Derived&>(*this).apply(x.arg2());
    static_cast<Derived&>(*this).apply(x.arg3());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::condeq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.arg1());
    static_cast<Derived&>(*this).apply(x.arg2());
    static_cast<Derived&>(*this).apply(x.arg3());
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
    else if (pres_system::is_const_multiply(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::const_multiply>(x));
    }
    else if (pres_system::is_const_multiply_alt(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::const_multiply_alt>(x));
    }
    else if (pres_system::is_infimum(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::infimum>(x));
    }
    else if (pres_system::is_supremum(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::supremum>(x));
    }
    else if (pres_system::is_sum(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::sum>(x));
    }
    else if (pres_system::is_eqinf(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::eqinf>(x));
    }
    else if (pres_system::is_eqninf(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::eqninf>(x));
    }
    else if (pres_system::is_condsm(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::condsm>(x));
    }
    else if (pres_system::is_condeq(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::condeq>(x));
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
  using super = Traverser<Derived>;
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

  void apply(const pres_system::const_multiply& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::const_multiply_alt& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::infimum& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::supremum& x)
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

  void apply(const pres_system::eqinf& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::eqninf& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::condsm& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.arg1());
    static_cast<Derived&>(*this).apply(x.arg2());
    static_cast<Derived&>(*this).apply(x.arg3());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::condeq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.arg1());
    static_cast<Derived&>(*this).apply(x.arg2());
    static_cast<Derived&>(*this).apply(x.arg3());
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
    else if (pres_system::is_const_multiply(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::const_multiply>(x));
    }
    else if (pres_system::is_const_multiply_alt(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::const_multiply_alt>(x));
    }
    else if (pres_system::is_infimum(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::infimum>(x));
    }
    else if (pres_system::is_supremum(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::supremum>(x));
    }
    else if (pres_system::is_sum(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::sum>(x));
    }
    else if (pres_system::is_eqinf(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::eqinf>(x));
    }
    else if (pres_system::is_eqninf(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::eqninf>(x));
    }
    else if (pres_system::is_condsm(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::condsm>(x));
    }
    else if (pres_system::is_condeq(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::condeq>(x));
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

  void apply(const pres_system::const_multiply& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::const_multiply_alt& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::infimum& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::supremum& x)
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

  void apply(const pres_system::eqinf& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::eqninf& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::condsm& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.arg1());
    static_cast<Derived&>(*this).apply(x.arg2());
    static_cast<Derived&>(*this).apply(x.arg3());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const pres_system::condeq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.arg1());
    static_cast<Derived&>(*this).apply(x.arg2());
    static_cast<Derived&>(*this).apply(x.arg3());
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
    else if (pres_system::is_const_multiply(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::const_multiply>(x));
    }
    else if (pres_system::is_const_multiply_alt(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::const_multiply_alt>(x));
    }
    else if (pres_system::is_infimum(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::infimum>(x));
    }
    else if (pres_system::is_supremum(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::supremum>(x));
    }
    else if (pres_system::is_sum(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::sum>(x));
    }
    else if (pres_system::is_eqinf(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::eqinf>(x));
    }
    else if (pres_system::is_eqninf(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::eqninf>(x));
    }
    else if (pres_system::is_condsm(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::condsm>(x));
    }
    else if (pres_system::is_condeq(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<pres_system::condeq>(x));
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

} // namespace mcrl2::pres_system



#endif // MCRL2_PRES_TRAVERSER_H
