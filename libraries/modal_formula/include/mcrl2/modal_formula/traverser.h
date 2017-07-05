// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/traverser.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_TRAVERSER_H
#define MCRL2_MODAL_FORMULA_TRAVERSER_H

#include "mcrl2/lps/traverser.h"
#include "mcrl2/modal_formula/action_formula.h"
#include "mcrl2/modal_formula/regular_formula.h"
#include "mcrl2/modal_formula/state_formula_specification.h"

namespace mcrl2
{

namespace action_formulas
{

/// \brief Base class for action_formula_traverser.
template <typename Derived>
struct action_formula_traverser_base: public core::traverser<Derived>
{
  typedef core::traverser<Derived> super;
  using super::apply;
  using super::enter;
  using super::leave;

  void apply(const data::data_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }
};

//--- start generated action_formulas::add_traverser_sort_expressions code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_sort_expressions: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const action_formulas::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::at& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).apply(x.time_stamp());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.actions());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::action_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (action_formulas::is_true(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::true_>(x));
    }
    else if (action_formulas::is_false(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::false_>(x));
    }
    else if (action_formulas::is_not(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::not_>(x));
    }
    else if (action_formulas::is_and(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::and_>(x));
    }
    else if (action_formulas::is_or(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::or_>(x));
    }
    else if (action_formulas::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::imp>(x));
    }
    else if (action_formulas::is_forall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::forall>(x));
    }
    else if (action_formulas::is_exists(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::exists>(x));
    }
    else if (action_formulas::is_at(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::at>(x));
    }
    else if (action_formulas::is_multi_action(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::multi_action>(x));
    }
    else if (process::is_untyped_multi_action(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::untyped_multi_action>(x));
    }
    else if (data::is_untyped_data_parameter(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_data_parameter>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct sort_expression_traverser: public add_traverser_sort_expressions<lps::sort_expression_traverser, Derived>
{
  typedef add_traverser_sort_expressions<lps::sort_expression_traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
};
//--- end generated action_formulas::add_traverser_sort_expressions code ---//

//--- start generated action_formulas::add_traverser_data_expressions code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_data_expressions: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const action_formulas::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::at& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).apply(x.time_stamp());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.actions());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::action_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (action_formulas::is_true(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::true_>(x));
    }
    else if (action_formulas::is_false(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::false_>(x));
    }
    else if (action_formulas::is_not(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::not_>(x));
    }
    else if (action_formulas::is_and(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::and_>(x));
    }
    else if (action_formulas::is_or(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::or_>(x));
    }
    else if (action_formulas::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::imp>(x));
    }
    else if (action_formulas::is_forall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::forall>(x));
    }
    else if (action_formulas::is_exists(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::exists>(x));
    }
    else if (action_formulas::is_at(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::at>(x));
    }
    else if (action_formulas::is_multi_action(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::multi_action>(x));
    }
    else if (process::is_untyped_multi_action(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::untyped_multi_action>(x));
    }
    else if (data::is_untyped_data_parameter(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_data_parameter>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct data_expression_traverser: public add_traverser_data_expressions<lps::data_expression_traverser, Derived>
{
  typedef add_traverser_data_expressions<lps::data_expression_traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
};
//--- end generated action_formulas::add_traverser_data_expressions code ---//

//--- start generated action_formulas::add_traverser_action_formula_expressions code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_action_formula_expressions: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const action_formulas::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::at& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::action_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (action_formulas::is_true(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::true_>(x));
    }
    else if (action_formulas::is_false(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::false_>(x));
    }
    else if (action_formulas::is_not(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::not_>(x));
    }
    else if (action_formulas::is_and(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::and_>(x));
    }
    else if (action_formulas::is_or(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::or_>(x));
    }
    else if (action_formulas::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::imp>(x));
    }
    else if (action_formulas::is_forall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::forall>(x));
    }
    else if (action_formulas::is_exists(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::exists>(x));
    }
    else if (action_formulas::is_at(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::at>(x));
    }
    else if (action_formulas::is_multi_action(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::multi_action>(x));
    }
    else if (process::is_untyped_multi_action(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::untyped_multi_action>(x));
    }
    else if (data::is_untyped_data_parameter(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_data_parameter>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct action_formula_traverser: public add_traverser_action_formula_expressions<action_formulas::action_formula_traverser_base, Derived>
{
  typedef add_traverser_action_formula_expressions<action_formulas::action_formula_traverser_base, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
};
//--- end generated action_formulas::add_traverser_action_formula_expressions code ---//

//--- start generated action_formulas::add_traverser_variables code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_variables: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const action_formulas::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::at& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).apply(x.time_stamp());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.actions());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::action_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (action_formulas::is_true(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::true_>(x));
    }
    else if (action_formulas::is_false(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::false_>(x));
    }
    else if (action_formulas::is_not(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::not_>(x));
    }
    else if (action_formulas::is_and(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::and_>(x));
    }
    else if (action_formulas::is_or(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::or_>(x));
    }
    else if (action_formulas::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::imp>(x));
    }
    else if (action_formulas::is_forall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::forall>(x));
    }
    else if (action_formulas::is_exists(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::exists>(x));
    }
    else if (action_formulas::is_at(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::at>(x));
    }
    else if (action_formulas::is_multi_action(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::multi_action>(x));
    }
    else if (process::is_untyped_multi_action(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::untyped_multi_action>(x));
    }
    else if (data::is_untyped_data_parameter(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_data_parameter>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct variable_traverser: public add_traverser_variables<lps::variable_traverser, Derived>
{
  typedef add_traverser_variables<lps::variable_traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
};
//--- end generated action_formulas::add_traverser_variables code ---//

//--- start generated action_formulas::add_traverser_identifier_strings code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_identifier_strings: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const action_formulas::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::at& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).apply(x.time_stamp());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.actions());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::action_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (action_formulas::is_true(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::true_>(x));
    }
    else if (action_formulas::is_false(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::false_>(x));
    }
    else if (action_formulas::is_not(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::not_>(x));
    }
    else if (action_formulas::is_and(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::and_>(x));
    }
    else if (action_formulas::is_or(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::or_>(x));
    }
    else if (action_formulas::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::imp>(x));
    }
    else if (action_formulas::is_forall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::forall>(x));
    }
    else if (action_formulas::is_exists(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::exists>(x));
    }
    else if (action_formulas::is_at(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::at>(x));
    }
    else if (action_formulas::is_multi_action(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::multi_action>(x));
    }
    else if (process::is_untyped_multi_action(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::untyped_multi_action>(x));
    }
    else if (data::is_untyped_data_parameter(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_data_parameter>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct identifier_string_traverser: public add_traverser_identifier_strings<lps::identifier_string_traverser, Derived>
{
  typedef add_traverser_identifier_strings<lps::identifier_string_traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
};
//--- end generated action_formulas::add_traverser_identifier_strings code ---//

} // namespace action_formulas

namespace regular_formulas
{

/// \brief Traversal class for regular_formula_traverser. Used as a base class for pbes_expression_traverser.
template <typename Derived>
struct regular_formula_traverser_base: public core::traverser<Derived>
{
  typedef core::traverser<Derived> super;
  using super::apply;
  using super::enter;
  using super::leave;

  void apply(const data::data_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const action_formulas::action_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }
};

//--- start generated regular_formulas::add_traverser_sort_expressions code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_sort_expressions: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const regular_formulas::seq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const regular_formulas::alt& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const regular_formulas::trans& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const regular_formulas::trans_or_nil& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const regular_formulas::untyped_regular_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const regular_formulas::regular_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (action_formulas::is_action_formula(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::action_formula>(x));
    }
    else if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (regular_formulas::is_seq(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::seq>(x));
    }
    else if (regular_formulas::is_alt(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::alt>(x));
    }
    else if (regular_formulas::is_trans(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::trans>(x));
    }
    else if (regular_formulas::is_trans_or_nil(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::trans_or_nil>(x));
    }
    else if (regular_formulas::is_untyped_regular_formula(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::untyped_regular_formula>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct sort_expression_traverser: public add_traverser_sort_expressions<action_formulas::sort_expression_traverser, Derived>
{
  typedef add_traverser_sort_expressions<action_formulas::sort_expression_traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
};
//--- end generated regular_formulas::add_traverser_sort_expressions code ---//

//--- start generated regular_formulas::add_traverser_data_expressions code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_data_expressions: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const regular_formulas::seq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const regular_formulas::alt& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const regular_formulas::trans& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const regular_formulas::trans_or_nil& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const regular_formulas::untyped_regular_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const regular_formulas::regular_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (action_formulas::is_action_formula(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::action_formula>(x));
    }
    else if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (regular_formulas::is_seq(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::seq>(x));
    }
    else if (regular_formulas::is_alt(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::alt>(x));
    }
    else if (regular_formulas::is_trans(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::trans>(x));
    }
    else if (regular_formulas::is_trans_or_nil(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::trans_or_nil>(x));
    }
    else if (regular_formulas::is_untyped_regular_formula(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::untyped_regular_formula>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct data_expression_traverser: public add_traverser_data_expressions<action_formulas::data_expression_traverser, Derived>
{
  typedef add_traverser_data_expressions<action_formulas::data_expression_traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
};
//--- end generated regular_formulas::add_traverser_data_expressions code ---//

//--- start generated regular_formulas::add_traverser_regular_formula_expressions code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_regular_formula_expressions: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const regular_formulas::seq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const regular_formulas::alt& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const regular_formulas::trans& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const regular_formulas::trans_or_nil& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const regular_formulas::untyped_regular_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const regular_formulas::regular_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (action_formulas::is_action_formula(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::action_formula>(x));
    }
    else if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (regular_formulas::is_seq(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::seq>(x));
    }
    else if (regular_formulas::is_alt(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::alt>(x));
    }
    else if (regular_formulas::is_trans(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::trans>(x));
    }
    else if (regular_formulas::is_trans_or_nil(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::trans_or_nil>(x));
    }
    else if (regular_formulas::is_untyped_regular_formula(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::untyped_regular_formula>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct regular_formula_traverser: public add_traverser_regular_formula_expressions<regular_formulas::regular_formula_traverser_base, Derived>
{
  typedef add_traverser_regular_formula_expressions<regular_formulas::regular_formula_traverser_base, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
};
//--- end generated regular_formulas::add_traverser_regular_formula_expressions code ---//

//--- start generated regular_formulas::add_traverser_variables code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_variables: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const regular_formulas::seq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const regular_formulas::alt& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const regular_formulas::trans& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const regular_formulas::trans_or_nil& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const regular_formulas::untyped_regular_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const regular_formulas::regular_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (action_formulas::is_action_formula(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::action_formula>(x));
    }
    else if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (regular_formulas::is_seq(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::seq>(x));
    }
    else if (regular_formulas::is_alt(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::alt>(x));
    }
    else if (regular_formulas::is_trans(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::trans>(x));
    }
    else if (regular_formulas::is_trans_or_nil(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::trans_or_nil>(x));
    }
    else if (regular_formulas::is_untyped_regular_formula(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::untyped_regular_formula>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct variable_traverser: public add_traverser_variables<action_formulas::variable_traverser, Derived>
{
  typedef add_traverser_variables<action_formulas::variable_traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
};
//--- end generated regular_formulas::add_traverser_variables code ---//

//--- start generated regular_formulas::add_traverser_identifier_strings code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_identifier_strings: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const regular_formulas::seq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const regular_formulas::alt& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const regular_formulas::trans& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const regular_formulas::trans_or_nil& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const regular_formulas::untyped_regular_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.name());
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const regular_formulas::regular_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (action_formulas::is_action_formula(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::action_formula>(x));
    }
    else if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (regular_formulas::is_seq(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::seq>(x));
    }
    else if (regular_formulas::is_alt(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::alt>(x));
    }
    else if (regular_formulas::is_trans(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::trans>(x));
    }
    else if (regular_formulas::is_trans_or_nil(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::trans_or_nil>(x));
    }
    else if (regular_formulas::is_untyped_regular_formula(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::untyped_regular_formula>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct identifier_string_traverser: public add_traverser_identifier_strings<action_formulas::identifier_string_traverser, Derived>
{
  typedef add_traverser_identifier_strings<action_formulas::identifier_string_traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
};
//--- end generated regular_formulas::add_traverser_identifier_strings code ---//

} // namespace regular_formulas

namespace state_formulas
{

/// \brief Traversal class for pbes_expressions. Used as a base class for pbes_expression_traverser.
template <typename Derived>
struct state_formula_traverser_base: public core::traverser<Derived>
{
  typedef core::traverser<Derived> super;
  using super::apply;
  using super::enter;
  using super::leave;

  void apply(const data::data_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }
};

//--- start generated state_formulas::add_traverser_sort_expressions code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_sort_expressions: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const state_formulas::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::must& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::may& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::yaled& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::yaled_timed& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.time_stamp());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::delay& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::delay_timed& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.time_stamp());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.arguments());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::nu& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::mu& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::state_formula_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.action_labels());
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::state_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (data::is_untyped_data_parameter(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_data_parameter>(x));
    }
    else if (state_formulas::is_true(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::true_>(x));
    }
    else if (state_formulas::is_false(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::false_>(x));
    }
    else if (state_formulas::is_not(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::not_>(x));
    }
    else if (state_formulas::is_and(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::and_>(x));
    }
    else if (state_formulas::is_or(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::or_>(x));
    }
    else if (state_formulas::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::imp>(x));
    }
    else if (state_formulas::is_forall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::forall>(x));
    }
    else if (state_formulas::is_exists(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::exists>(x));
    }
    else if (state_formulas::is_must(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::must>(x));
    }
    else if (state_formulas::is_may(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::may>(x));
    }
    else if (state_formulas::is_yaled(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::yaled>(x));
    }
    else if (state_formulas::is_yaled_timed(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::yaled_timed>(x));
    }
    else if (state_formulas::is_delay(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::delay>(x));
    }
    else if (state_formulas::is_delay_timed(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::delay_timed>(x));
    }
    else if (state_formulas::is_variable(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::variable>(x));
    }
    else if (state_formulas::is_nu(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::nu>(x));
    }
    else if (state_formulas::is_mu(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::mu>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct sort_expression_traverser: public add_traverser_sort_expressions<regular_formulas::sort_expression_traverser, Derived>
{
  typedef add_traverser_sort_expressions<regular_formulas::sort_expression_traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
};
//--- end generated state_formulas::add_traverser_sort_expressions code ---//

//--- start generated state_formulas::add_traverser_data_expressions code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_data_expressions: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const state_formulas::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::must& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::may& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::yaled& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::yaled_timed& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.time_stamp());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::delay& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::delay_timed& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.time_stamp());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.arguments());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::nu& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::mu& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::state_formula_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::state_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (data::is_untyped_data_parameter(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_data_parameter>(x));
    }
    else if (state_formulas::is_true(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::true_>(x));
    }
    else if (state_formulas::is_false(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::false_>(x));
    }
    else if (state_formulas::is_not(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::not_>(x));
    }
    else if (state_formulas::is_and(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::and_>(x));
    }
    else if (state_formulas::is_or(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::or_>(x));
    }
    else if (state_formulas::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::imp>(x));
    }
    else if (state_formulas::is_forall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::forall>(x));
    }
    else if (state_formulas::is_exists(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::exists>(x));
    }
    else if (state_formulas::is_must(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::must>(x));
    }
    else if (state_formulas::is_may(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::may>(x));
    }
    else if (state_formulas::is_yaled(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::yaled>(x));
    }
    else if (state_formulas::is_yaled_timed(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::yaled_timed>(x));
    }
    else if (state_formulas::is_delay(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::delay>(x));
    }
    else if (state_formulas::is_delay_timed(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::delay_timed>(x));
    }
    else if (state_formulas::is_variable(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::variable>(x));
    }
    else if (state_formulas::is_nu(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::nu>(x));
    }
    else if (state_formulas::is_mu(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::mu>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct data_expression_traverser: public add_traverser_data_expressions<regular_formulas::data_expression_traverser, Derived>
{
  typedef add_traverser_data_expressions<regular_formulas::data_expression_traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
};
//--- end generated state_formulas::add_traverser_data_expressions code ---//

//--- start generated state_formulas::add_traverser_state_formula_expressions code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_state_formula_expressions: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const state_formulas::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::must& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::may& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::yaled& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::yaled_timed& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::delay& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::delay_timed& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::nu& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::mu& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::state_formula_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::state_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (data::is_untyped_data_parameter(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_data_parameter>(x));
    }
    else if (state_formulas::is_true(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::true_>(x));
    }
    else if (state_formulas::is_false(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::false_>(x));
    }
    else if (state_formulas::is_not(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::not_>(x));
    }
    else if (state_formulas::is_and(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::and_>(x));
    }
    else if (state_formulas::is_or(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::or_>(x));
    }
    else if (state_formulas::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::imp>(x));
    }
    else if (state_formulas::is_forall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::forall>(x));
    }
    else if (state_formulas::is_exists(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::exists>(x));
    }
    else if (state_formulas::is_must(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::must>(x));
    }
    else if (state_formulas::is_may(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::may>(x));
    }
    else if (state_formulas::is_yaled(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::yaled>(x));
    }
    else if (state_formulas::is_yaled_timed(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::yaled_timed>(x));
    }
    else if (state_formulas::is_delay(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::delay>(x));
    }
    else if (state_formulas::is_delay_timed(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::delay_timed>(x));
    }
    else if (state_formulas::is_variable(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::variable>(x));
    }
    else if (state_formulas::is_nu(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::nu>(x));
    }
    else if (state_formulas::is_mu(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::mu>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct state_formula_traverser: public add_traverser_state_formula_expressions<state_formulas::state_formula_traverser_base, Derived>
{
  typedef add_traverser_state_formula_expressions<state_formulas::state_formula_traverser_base, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
};
//--- end generated state_formulas::add_traverser_state_formula_expressions code ---//

//--- start generated state_formulas::add_traverser_variables code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_variables: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const state_formulas::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::must& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::may& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::yaled& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::yaled_timed& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.time_stamp());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::delay& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::delay_timed& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.time_stamp());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.arguments());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::nu& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::mu& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::state_formula_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::state_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (data::is_untyped_data_parameter(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_data_parameter>(x));
    }
    else if (state_formulas::is_true(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::true_>(x));
    }
    else if (state_formulas::is_false(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::false_>(x));
    }
    else if (state_formulas::is_not(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::not_>(x));
    }
    else if (state_formulas::is_and(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::and_>(x));
    }
    else if (state_formulas::is_or(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::or_>(x));
    }
    else if (state_formulas::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::imp>(x));
    }
    else if (state_formulas::is_forall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::forall>(x));
    }
    else if (state_formulas::is_exists(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::exists>(x));
    }
    else if (state_formulas::is_must(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::must>(x));
    }
    else if (state_formulas::is_may(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::may>(x));
    }
    else if (state_formulas::is_yaled(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::yaled>(x));
    }
    else if (state_formulas::is_yaled_timed(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::yaled_timed>(x));
    }
    else if (state_formulas::is_delay(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::delay>(x));
    }
    else if (state_formulas::is_delay_timed(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::delay_timed>(x));
    }
    else if (state_formulas::is_variable(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::variable>(x));
    }
    else if (state_formulas::is_nu(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::nu>(x));
    }
    else if (state_formulas::is_mu(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::mu>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct variable_traverser: public add_traverser_variables<regular_formulas::variable_traverser, Derived>
{
  typedef add_traverser_variables<regular_formulas::variable_traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
};
//--- end generated state_formulas::add_traverser_variables code ---//

//--- start generated state_formulas::add_traverser_state_variables code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_state_variables: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const state_formulas::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::must& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::may& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::yaled& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::yaled_timed& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::delay& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::delay_timed& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::nu& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::mu& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::state_formula_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::state_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (data::is_untyped_data_parameter(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_data_parameter>(x));
    }
    else if (state_formulas::is_true(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::true_>(x));
    }
    else if (state_formulas::is_false(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::false_>(x));
    }
    else if (state_formulas::is_not(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::not_>(x));
    }
    else if (state_formulas::is_and(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::and_>(x));
    }
    else if (state_formulas::is_or(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::or_>(x));
    }
    else if (state_formulas::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::imp>(x));
    }
    else if (state_formulas::is_forall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::forall>(x));
    }
    else if (state_formulas::is_exists(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::exists>(x));
    }
    else if (state_formulas::is_must(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::must>(x));
    }
    else if (state_formulas::is_may(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::may>(x));
    }
    else if (state_formulas::is_yaled(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::yaled>(x));
    }
    else if (state_formulas::is_yaled_timed(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::yaled_timed>(x));
    }
    else if (state_formulas::is_delay(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::delay>(x));
    }
    else if (state_formulas::is_delay_timed(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::delay_timed>(x));
    }
    else if (state_formulas::is_variable(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::variable>(x));
    }
    else if (state_formulas::is_nu(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::nu>(x));
    }
    else if (state_formulas::is_mu(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::mu>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct state_variable_traverser: public add_traverser_state_variables<state_formulas::state_formula_traverser_base, Derived>
{
  typedef add_traverser_state_variables<state_formulas::state_formula_traverser_base, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
};
//--- end generated state_formulas::add_traverser_state_variables code ---//

//--- start generated state_formulas::add_traverser_identifier_strings code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_identifier_strings: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const state_formulas::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::must& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::may& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::yaled& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::yaled_timed& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.time_stamp());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::delay& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::delay_timed& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.time_stamp());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.name());
    static_cast<Derived&>(*this).apply(x.arguments());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::nu& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.name());
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::mu& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.name());
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::state_formula_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.action_labels());
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::state_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (data::is_untyped_data_parameter(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_data_parameter>(x));
    }
    else if (state_formulas::is_true(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::true_>(x));
    }
    else if (state_formulas::is_false(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::false_>(x));
    }
    else if (state_formulas::is_not(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::not_>(x));
    }
    else if (state_formulas::is_and(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::and_>(x));
    }
    else if (state_formulas::is_or(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::or_>(x));
    }
    else if (state_formulas::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::imp>(x));
    }
    else if (state_formulas::is_forall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::forall>(x));
    }
    else if (state_formulas::is_exists(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::exists>(x));
    }
    else if (state_formulas::is_must(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::must>(x));
    }
    else if (state_formulas::is_may(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::may>(x));
    }
    else if (state_formulas::is_yaled(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::yaled>(x));
    }
    else if (state_formulas::is_yaled_timed(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::yaled_timed>(x));
    }
    else if (state_formulas::is_delay(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::delay>(x));
    }
    else if (state_formulas::is_delay_timed(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::delay_timed>(x));
    }
    else if (state_formulas::is_variable(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::variable>(x));
    }
    else if (state_formulas::is_nu(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::nu>(x));
    }
    else if (state_formulas::is_mu(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::mu>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct identifier_string_traverser: public add_traverser_identifier_strings<regular_formulas::identifier_string_traverser, Derived>
{
  typedef add_traverser_identifier_strings<regular_formulas::identifier_string_traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
};
//--- end generated state_formulas::add_traverser_identifier_strings code ---//

//--- start generated state_formulas::add_traverser_regular_formula_expressions code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_regular_formula_expressions: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const state_formulas::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::must& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::may& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::yaled& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::yaled_timed& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::delay& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::delay_timed& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::nu& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::mu& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::state_formula_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const state_formulas::state_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (data::is_untyped_data_parameter(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_data_parameter>(x));
    }
    else if (state_formulas::is_true(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::true_>(x));
    }
    else if (state_formulas::is_false(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::false_>(x));
    }
    else if (state_formulas::is_not(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::not_>(x));
    }
    else if (state_formulas::is_and(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::and_>(x));
    }
    else if (state_formulas::is_or(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::or_>(x));
    }
    else if (state_formulas::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::imp>(x));
    }
    else if (state_formulas::is_forall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::forall>(x));
    }
    else if (state_formulas::is_exists(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::exists>(x));
    }
    else if (state_formulas::is_must(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::must>(x));
    }
    else if (state_formulas::is_may(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::may>(x));
    }
    else if (state_formulas::is_yaled(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::yaled>(x));
    }
    else if (state_formulas::is_yaled_timed(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::yaled_timed>(x));
    }
    else if (state_formulas::is_delay(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::delay>(x));
    }
    else if (state_formulas::is_delay_timed(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::delay_timed>(x));
    }
    else if (state_formulas::is_variable(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::variable>(x));
    }
    else if (state_formulas::is_nu(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::nu>(x));
    }
    else if (state_formulas::is_mu(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::mu>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct regular_formula_traverser: public add_traverser_regular_formula_expressions<regular_formulas::regular_formula_traverser, Derived>
{
  typedef add_traverser_regular_formula_expressions<regular_formulas::regular_formula_traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
};
//--- end generated state_formulas::add_traverser_regular_formula_expressions code ---//

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_TRAVERSER_H
