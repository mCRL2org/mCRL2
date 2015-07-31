// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/builder.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_BUILDER_H
#define MCRL2_MODAL_FORMULA_BUILDER_H

#include "mcrl2/modal_formula/action_formula.h"
#include "mcrl2/modal_formula/regular_formula.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/data/builder.h"
#include "mcrl2/lps/builder.h"

namespace mcrl2
{

namespace action_formulas
{

/// \brief Base class for action_formula_builder.
template <typename Derived>
struct action_formula_builder_base: public core::builder<Derived>
{
  typedef core::builder<Derived> super;
  using super::apply;

  action_formula apply(const data::data_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }
};

//--- start generated action_formulas::add_sort_expressions code ---//
template <template <class> class Builder, class Derived>
struct add_sort_expressions: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  action_formulas::true_ apply(const action_formulas::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  action_formulas::false_ apply(const action_formulas::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  action_formulas::not_ apply(const action_formulas::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::not_ result = action_formulas::not_(static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::and_ apply(const action_formulas::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::and_ result = action_formulas::and_(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::or_ apply(const action_formulas::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::or_ result = action_formulas::or_(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::imp apply(const action_formulas::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::imp result = action_formulas::imp(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::forall apply(const action_formulas::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::forall result = action_formulas::forall(static_cast<Derived&>(*this).apply(x.variables()), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::exists apply(const action_formulas::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::exists result = action_formulas::exists(static_cast<Derived&>(*this).apply(x.variables()), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::at apply(const action_formulas::at& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::at result = action_formulas::at(static_cast<Derived&>(*this).apply(x.operand()), static_cast<Derived&>(*this).apply(x.time_stamp()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::multi_action apply(const action_formulas::multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::multi_action result = action_formulas::multi_action(static_cast<Derived&>(*this).apply(x.actions()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::action_formula apply(const action_formulas::action_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::action_formula result;
    if (data::is_data_expression(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (action_formulas::is_true(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::true_>(x));
    }
    else if (action_formulas::is_false(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::false_>(x));
    }
    else if (action_formulas::is_not(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::not_>(x));
    }
    else if (action_formulas::is_and(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::and_>(x));
    }
    else if (action_formulas::is_or(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::or_>(x));
    }
    else if (action_formulas::is_imp(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::imp>(x));
    }
    else if (action_formulas::is_forall(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::forall>(x));
    }
    else if (action_formulas::is_exists(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::exists>(x));
    }
    else if (action_formulas::is_at(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::at>(x));
    }
    else if (action_formulas::is_multi_action(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::multi_action>(x));
    }
    else if (process::is_untyped_multi_action(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::untyped_multi_action>(x));
    }
    else if (data::is_untyped_data_parameter(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_data_parameter>(x));
    }
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

};

/// \brief Builder class
template <typename Derived>
struct sort_expression_builder: public add_sort_expressions<lps::sort_expression_builder, Derived>
{
  typedef add_sort_expressions<lps::sort_expression_builder, Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;
};
//--- end generated action_formulas::add_sort_expressions code ---//

//--- start generated action_formulas::add_data_expressions code ---//
template <template <class> class Builder, class Derived>
struct add_data_expressions: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  action_formulas::true_ apply(const action_formulas::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  action_formulas::false_ apply(const action_formulas::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  action_formulas::not_ apply(const action_formulas::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::not_ result = action_formulas::not_(static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::and_ apply(const action_formulas::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::and_ result = action_formulas::and_(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::or_ apply(const action_formulas::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::or_ result = action_formulas::or_(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::imp apply(const action_formulas::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::imp result = action_formulas::imp(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::forall apply(const action_formulas::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::forall result = action_formulas::forall(x.variables(), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::exists apply(const action_formulas::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::exists result = action_formulas::exists(x.variables(), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::at apply(const action_formulas::at& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::at result = action_formulas::at(static_cast<Derived&>(*this).apply(x.operand()), static_cast<Derived&>(*this).apply(x.time_stamp()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::multi_action apply(const action_formulas::multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::multi_action result = action_formulas::multi_action(static_cast<Derived&>(*this).apply(x.actions()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::action_formula apply(const action_formulas::action_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::action_formula result;
    if (data::is_data_expression(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (action_formulas::is_true(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::true_>(x));
    }
    else if (action_formulas::is_false(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::false_>(x));
    }
    else if (action_formulas::is_not(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::not_>(x));
    }
    else if (action_formulas::is_and(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::and_>(x));
    }
    else if (action_formulas::is_or(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::or_>(x));
    }
    else if (action_formulas::is_imp(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::imp>(x));
    }
    else if (action_formulas::is_forall(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::forall>(x));
    }
    else if (action_formulas::is_exists(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::exists>(x));
    }
    else if (action_formulas::is_at(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::at>(x));
    }
    else if (action_formulas::is_multi_action(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::multi_action>(x));
    }
    else if (process::is_untyped_multi_action(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::untyped_multi_action>(x));
    }
    else if (data::is_untyped_data_parameter(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_data_parameter>(x));
    }
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

};

/// \brief Builder class
template <typename Derived>
struct data_expression_builder: public add_data_expressions<lps::data_expression_builder, Derived>
{
  typedef add_data_expressions<lps::data_expression_builder, Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;
};
//--- end generated action_formulas::add_data_expressions code ---//

//--- start generated action_formulas::add_variables code ---//
template <template <class> class Builder, class Derived>
struct add_variables: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  action_formulas::true_ apply(const action_formulas::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  action_formulas::false_ apply(const action_formulas::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  action_formulas::not_ apply(const action_formulas::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::not_ result = action_formulas::not_(static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::and_ apply(const action_formulas::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::and_ result = action_formulas::and_(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::or_ apply(const action_formulas::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::or_ result = action_formulas::or_(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::imp apply(const action_formulas::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::imp result = action_formulas::imp(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::forall apply(const action_formulas::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::forall result = action_formulas::forall(static_cast<Derived&>(*this).apply(x.variables()), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::exists apply(const action_formulas::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::exists result = action_formulas::exists(static_cast<Derived&>(*this).apply(x.variables()), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::at apply(const action_formulas::at& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::at result = action_formulas::at(static_cast<Derived&>(*this).apply(x.operand()), static_cast<Derived&>(*this).apply(x.time_stamp()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::multi_action apply(const action_formulas::multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::multi_action result = action_formulas::multi_action(static_cast<Derived&>(*this).apply(x.actions()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::action_formula apply(const action_formulas::action_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::action_formula result;
    if (data::is_data_expression(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (action_formulas::is_true(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::true_>(x));
    }
    else if (action_formulas::is_false(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::false_>(x));
    }
    else if (action_formulas::is_not(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::not_>(x));
    }
    else if (action_formulas::is_and(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::and_>(x));
    }
    else if (action_formulas::is_or(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::or_>(x));
    }
    else if (action_formulas::is_imp(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::imp>(x));
    }
    else if (action_formulas::is_forall(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::forall>(x));
    }
    else if (action_formulas::is_exists(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::exists>(x));
    }
    else if (action_formulas::is_at(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::at>(x));
    }
    else if (action_formulas::is_multi_action(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::multi_action>(x));
    }
    else if (process::is_untyped_multi_action(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::untyped_multi_action>(x));
    }
    else if (data::is_untyped_data_parameter(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_data_parameter>(x));
    }
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

};

/// \brief Builder class
template <typename Derived>
struct variable_builder: public add_variables<lps::data_expression_builder, Derived>
{
  typedef add_variables<lps::data_expression_builder, Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;
};
//--- end generated action_formulas::add_variables code ---//

//--- start generated action_formulas::add_action_formula_expressions code ---//
template <template <class> class Builder, class Derived>
struct add_action_formula_expressions: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  action_formulas::true_ apply(const action_formulas::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  action_formulas::false_ apply(const action_formulas::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  action_formulas::not_ apply(const action_formulas::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::not_ result = action_formulas::not_(static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::and_ apply(const action_formulas::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::and_ result = action_formulas::and_(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::or_ apply(const action_formulas::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::or_ result = action_formulas::or_(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::imp apply(const action_formulas::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::imp result = action_formulas::imp(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::forall apply(const action_formulas::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::forall result = action_formulas::forall(x.variables(), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::exists apply(const action_formulas::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::exists result = action_formulas::exists(x.variables(), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::at apply(const action_formulas::at& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::at result = action_formulas::at(static_cast<Derived&>(*this).apply(x.operand()), x.time_stamp());
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  action_formulas::multi_action apply(const action_formulas::multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  action_formulas::action_formula apply(const action_formulas::action_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    action_formulas::action_formula result;
    if (data::is_data_expression(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (action_formulas::is_true(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::true_>(x));
    }
    else if (action_formulas::is_false(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::false_>(x));
    }
    else if (action_formulas::is_not(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::not_>(x));
    }
    else if (action_formulas::is_and(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::and_>(x));
    }
    else if (action_formulas::is_or(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::or_>(x));
    }
    else if (action_formulas::is_imp(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::imp>(x));
    }
    else if (action_formulas::is_forall(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::forall>(x));
    }
    else if (action_formulas::is_exists(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::exists>(x));
    }
    else if (action_formulas::is_at(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::at>(x));
    }
    else if (action_formulas::is_multi_action(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::multi_action>(x));
    }
    else if (process::is_untyped_multi_action(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::untyped_multi_action>(x));
    }
    else if (data::is_untyped_data_parameter(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_data_parameter>(x));
    }
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

};

/// \brief Builder class
template <typename Derived>
struct action_formula_builder: public add_action_formula_expressions<action_formulas::action_formula_builder_base, Derived>
{
  typedef add_action_formula_expressions<action_formulas::action_formula_builder_base, Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;
};
//--- end generated action_formulas::add_action_formula_expressions code ---//

} // namespace action_formulas

namespace regular_formulas
{

/// \brief Builder class for regular_formula_builder. Used as a base class for pbes_expression_builder.
template <typename Derived>
struct regular_formula_builder_base: public core::builder<Derived>
{
  typedef core::builder<Derived> super;
  using super::apply;

  regular_formula apply(const data::data_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }
};

//--- start generated regular_formulas::add_sort_expressions code ---//
template <template <class> class Builder, class Derived>
struct add_sort_expressions: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  regular_formulas::nil apply(const regular_formulas::nil& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  regular_formulas::seq apply(const regular_formulas::seq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    regular_formulas::seq result = regular_formulas::seq(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  regular_formulas::alt apply(const regular_formulas::alt& x)
  {
    static_cast<Derived&>(*this).enter(x);
    regular_formulas::alt result = regular_formulas::alt(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  regular_formulas::trans apply(const regular_formulas::trans& x)
  {
    static_cast<Derived&>(*this).enter(x);
    regular_formulas::trans result = regular_formulas::trans(static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  regular_formulas::trans_or_nil apply(const regular_formulas::trans_or_nil& x)
  {
    static_cast<Derived&>(*this).enter(x);
    regular_formulas::trans_or_nil result = regular_formulas::trans_or_nil(static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  regular_formulas::regular_formula apply(const regular_formulas::regular_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    regular_formulas::regular_formula result;
    if (action_formulas::is_action_formula(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::action_formula>(x));
    }
    else if (data::is_data_expression(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (regular_formulas::is_nil(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::nil>(x));
    }
    else if (regular_formulas::is_seq(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::seq>(x));
    }
    else if (regular_formulas::is_alt(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::alt>(x));
    }
    else if (regular_formulas::is_trans(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::trans>(x));
    }
    else if (regular_formulas::is_trans_or_nil(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::trans_or_nil>(x));
    }
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

};

/// \brief Builder class
template <typename Derived>
struct sort_expression_builder: public add_sort_expressions<action_formulas::sort_expression_builder, Derived>
{
  typedef add_sort_expressions<action_formulas::sort_expression_builder, Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;
};
//--- end generated regular_formulas::add_sort_expressions code ---//

//--- start generated regular_formulas::add_data_expressions code ---//
template <template <class> class Builder, class Derived>
struct add_data_expressions: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  regular_formulas::nil apply(const regular_formulas::nil& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  regular_formulas::seq apply(const regular_formulas::seq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    regular_formulas::seq result = regular_formulas::seq(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  regular_formulas::alt apply(const regular_formulas::alt& x)
  {
    static_cast<Derived&>(*this).enter(x);
    regular_formulas::alt result = regular_formulas::alt(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  regular_formulas::trans apply(const regular_formulas::trans& x)
  {
    static_cast<Derived&>(*this).enter(x);
    regular_formulas::trans result = regular_formulas::trans(static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  regular_formulas::trans_or_nil apply(const regular_formulas::trans_or_nil& x)
  {
    static_cast<Derived&>(*this).enter(x);
    regular_formulas::trans_or_nil result = regular_formulas::trans_or_nil(static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  regular_formulas::regular_formula apply(const regular_formulas::regular_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    regular_formulas::regular_formula result;
    if (action_formulas::is_action_formula(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::action_formula>(x));
    }
    else if (data::is_data_expression(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (regular_formulas::is_nil(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::nil>(x));
    }
    else if (regular_formulas::is_seq(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::seq>(x));
    }
    else if (regular_formulas::is_alt(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::alt>(x));
    }
    else if (regular_formulas::is_trans(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::trans>(x));
    }
    else if (regular_formulas::is_trans_or_nil(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::trans_or_nil>(x));
    }
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

};

/// \brief Builder class
template <typename Derived>
struct data_expression_builder: public add_data_expressions<action_formulas::data_expression_builder, Derived>
{
  typedef add_data_expressions<action_formulas::data_expression_builder, Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;
};
//--- end generated regular_formulas::add_data_expressions code ---//

//--- start generated regular_formulas::add_variables code ---//
template <template <class> class Builder, class Derived>
struct add_variables: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  regular_formulas::nil apply(const regular_formulas::nil& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  regular_formulas::seq apply(const regular_formulas::seq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    regular_formulas::seq result = regular_formulas::seq(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  regular_formulas::alt apply(const regular_formulas::alt& x)
  {
    static_cast<Derived&>(*this).enter(x);
    regular_formulas::alt result = regular_formulas::alt(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  regular_formulas::trans apply(const regular_formulas::trans& x)
  {
    static_cast<Derived&>(*this).enter(x);
    regular_formulas::trans result = regular_formulas::trans(static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  regular_formulas::trans_or_nil apply(const regular_formulas::trans_or_nil& x)
  {
    static_cast<Derived&>(*this).enter(x);
    regular_formulas::trans_or_nil result = regular_formulas::trans_or_nil(static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  regular_formulas::regular_formula apply(const regular_formulas::regular_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    regular_formulas::regular_formula result;
    if (action_formulas::is_action_formula(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::action_formula>(x));
    }
    else if (data::is_data_expression(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (regular_formulas::is_nil(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::nil>(x));
    }
    else if (regular_formulas::is_seq(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::seq>(x));
    }
    else if (regular_formulas::is_alt(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::alt>(x));
    }
    else if (regular_formulas::is_trans(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::trans>(x));
    }
    else if (regular_formulas::is_trans_or_nil(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::trans_or_nil>(x));
    }
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

};

/// \brief Builder class
template <typename Derived>
struct variable_builder: public add_variables<action_formulas::data_expression_builder, Derived>
{
  typedef add_variables<action_formulas::data_expression_builder, Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;
};
//--- end generated regular_formulas::add_variables code ---//

//--- start generated regular_formulas::add_regular_formula_expressions code ---//
template <template <class> class Builder, class Derived>
struct add_regular_formula_expressions: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  regular_formulas::nil apply(const regular_formulas::nil& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  regular_formulas::seq apply(const regular_formulas::seq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    regular_formulas::seq result = regular_formulas::seq(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  regular_formulas::alt apply(const regular_formulas::alt& x)
  {
    static_cast<Derived&>(*this).enter(x);
    regular_formulas::alt result = regular_formulas::alt(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  regular_formulas::trans apply(const regular_formulas::trans& x)
  {
    static_cast<Derived&>(*this).enter(x);
    regular_formulas::trans result = regular_formulas::trans(static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  regular_formulas::trans_or_nil apply(const regular_formulas::trans_or_nil& x)
  {
    static_cast<Derived&>(*this).enter(x);
    regular_formulas::trans_or_nil result = regular_formulas::trans_or_nil(static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  regular_formulas::regular_formula apply(const regular_formulas::regular_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    regular_formulas::regular_formula result;
    if (action_formulas::is_action_formula(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<action_formulas::action_formula>(x));
    }
    else if (data::is_data_expression(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (regular_formulas::is_nil(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::nil>(x));
    }
    else if (regular_formulas::is_seq(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::seq>(x));
    }
    else if (regular_formulas::is_alt(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::alt>(x));
    }
    else if (regular_formulas::is_trans(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::trans>(x));
    }
    else if (regular_formulas::is_trans_or_nil(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<regular_formulas::trans_or_nil>(x));
    }
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

};

/// \brief Builder class
template <typename Derived>
struct regular_formula_builder: public add_regular_formula_expressions<regular_formulas::regular_formula_builder_base, Derived>
{
  typedef add_regular_formula_expressions<regular_formulas::regular_formula_builder_base, Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;
};
//--- end generated regular_formulas::add_regular_formula_expressions code ---//

} // namespace regular_formulas

namespace state_formulas
{

/// \brief Builder class for pbes_expressions. Used as a base class for pbes_expression_builder.
template <typename Derived>
struct state_formula_builder_base: public core::builder<Derived>
{
  typedef core::builder<Derived> super;
  using super::apply;

  state_formula apply(const data::data_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }
};

//--- start generated state_formulas::add_sort_expressions code ---//
template <template <class> class Builder, class Derived>
struct add_sort_expressions: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  state_formulas::true_ apply(const state_formulas::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  state_formulas::false_ apply(const state_formulas::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  state_formulas::not_ apply(const state_formulas::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::not_ result = state_formulas::not_(static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::and_ apply(const state_formulas::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::and_ result = state_formulas::and_(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::or_ apply(const state_formulas::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::or_ result = state_formulas::or_(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::imp apply(const state_formulas::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::imp result = state_formulas::imp(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::forall apply(const state_formulas::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::forall result = state_formulas::forall(static_cast<Derived&>(*this).apply(x.variables()), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::exists apply(const state_formulas::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::exists result = state_formulas::exists(static_cast<Derived&>(*this).apply(x.variables()), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::must apply(const state_formulas::must& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::must result = state_formulas::must(static_cast<Derived&>(*this).apply(x.formula()), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::may apply(const state_formulas::may& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::may result = state_formulas::may(static_cast<Derived&>(*this).apply(x.formula()), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::yaled apply(const state_formulas::yaled& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  state_formulas::yaled_timed apply(const state_formulas::yaled_timed& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::yaled_timed result = state_formulas::yaled_timed(static_cast<Derived&>(*this).apply(x.time_stamp()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::delay apply(const state_formulas::delay& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  state_formulas::delay_timed apply(const state_formulas::delay_timed& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::delay_timed result = state_formulas::delay_timed(static_cast<Derived&>(*this).apply(x.time_stamp()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::variable apply(const state_formulas::variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::variable result = state_formulas::variable(x.name(), static_cast<Derived&>(*this).apply(x.arguments()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::nu apply(const state_formulas::nu& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::nu result = state_formulas::nu(x.name(), static_cast<Derived&>(*this).apply(x.assignments()), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::mu apply(const state_formulas::mu& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::mu result = state_formulas::mu(x.name(), static_cast<Derived&>(*this).apply(x.assignments()), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::state_formula apply(const state_formulas::state_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::state_formula result;
    if (data::is_data_expression(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (data::is_untyped_data_parameter(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_data_parameter>(x));
    }
    else if (state_formulas::is_true(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::true_>(x));
    }
    else if (state_formulas::is_false(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::false_>(x));
    }
    else if (state_formulas::is_not(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::not_>(x));
    }
    else if (state_formulas::is_and(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::and_>(x));
    }
    else if (state_formulas::is_or(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::or_>(x));
    }
    else if (state_formulas::is_imp(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::imp>(x));
    }
    else if (state_formulas::is_forall(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::forall>(x));
    }
    else if (state_formulas::is_exists(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::exists>(x));
    }
    else if (state_formulas::is_must(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::must>(x));
    }
    else if (state_formulas::is_may(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::may>(x));
    }
    else if (state_formulas::is_yaled(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::yaled>(x));
    }
    else if (state_formulas::is_yaled_timed(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::yaled_timed>(x));
    }
    else if (state_formulas::is_delay(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::delay>(x));
    }
    else if (state_formulas::is_delay_timed(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::delay_timed>(x));
    }
    else if (state_formulas::is_variable(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::variable>(x));
    }
    else if (state_formulas::is_nu(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::nu>(x));
    }
    else if (state_formulas::is_mu(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::mu>(x));
    }
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

};

/// \brief Builder class
template <typename Derived>
struct sort_expression_builder: public add_sort_expressions<regular_formulas::sort_expression_builder, Derived>
{
  typedef add_sort_expressions<regular_formulas::sort_expression_builder, Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;
};
//--- end generated state_formulas::add_sort_expressions code ---//

//--- start generated state_formulas::add_data_expressions code ---//
template <template <class> class Builder, class Derived>
struct add_data_expressions: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  state_formulas::true_ apply(const state_formulas::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  state_formulas::false_ apply(const state_formulas::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  state_formulas::not_ apply(const state_formulas::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::not_ result = state_formulas::not_(static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::and_ apply(const state_formulas::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::and_ result = state_formulas::and_(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::or_ apply(const state_formulas::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::or_ result = state_formulas::or_(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::imp apply(const state_formulas::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::imp result = state_formulas::imp(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::forall apply(const state_formulas::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::forall result = state_formulas::forall(x.variables(), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::exists apply(const state_formulas::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::exists result = state_formulas::exists(x.variables(), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::must apply(const state_formulas::must& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::must result = state_formulas::must(static_cast<Derived&>(*this).apply(x.formula()), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::may apply(const state_formulas::may& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::may result = state_formulas::may(static_cast<Derived&>(*this).apply(x.formula()), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::yaled apply(const state_formulas::yaled& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  state_formulas::yaled_timed apply(const state_formulas::yaled_timed& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::yaled_timed result = state_formulas::yaled_timed(static_cast<Derived&>(*this).apply(x.time_stamp()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::delay apply(const state_formulas::delay& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  state_formulas::delay_timed apply(const state_formulas::delay_timed& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::delay_timed result = state_formulas::delay_timed(static_cast<Derived&>(*this).apply(x.time_stamp()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::variable apply(const state_formulas::variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::variable result = state_formulas::variable(x.name(), static_cast<Derived&>(*this).apply(x.arguments()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::nu apply(const state_formulas::nu& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::nu result = state_formulas::nu(x.name(), static_cast<Derived&>(*this).apply(x.assignments()), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::mu apply(const state_formulas::mu& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::mu result = state_formulas::mu(x.name(), static_cast<Derived&>(*this).apply(x.assignments()), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::state_formula apply(const state_formulas::state_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::state_formula result;
    if (data::is_data_expression(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (data::is_untyped_data_parameter(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_data_parameter>(x));
    }
    else if (state_formulas::is_true(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::true_>(x));
    }
    else if (state_formulas::is_false(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::false_>(x));
    }
    else if (state_formulas::is_not(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::not_>(x));
    }
    else if (state_formulas::is_and(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::and_>(x));
    }
    else if (state_formulas::is_or(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::or_>(x));
    }
    else if (state_formulas::is_imp(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::imp>(x));
    }
    else if (state_formulas::is_forall(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::forall>(x));
    }
    else if (state_formulas::is_exists(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::exists>(x));
    }
    else if (state_formulas::is_must(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::must>(x));
    }
    else if (state_formulas::is_may(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::may>(x));
    }
    else if (state_formulas::is_yaled(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::yaled>(x));
    }
    else if (state_formulas::is_yaled_timed(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::yaled_timed>(x));
    }
    else if (state_formulas::is_delay(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::delay>(x));
    }
    else if (state_formulas::is_delay_timed(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::delay_timed>(x));
    }
    else if (state_formulas::is_variable(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::variable>(x));
    }
    else if (state_formulas::is_nu(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::nu>(x));
    }
    else if (state_formulas::is_mu(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::mu>(x));
    }
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

};

/// \brief Builder class
template <typename Derived>
struct data_expression_builder: public add_data_expressions<regular_formulas::data_expression_builder, Derived>
{
  typedef add_data_expressions<regular_formulas::data_expression_builder, Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;
};
//--- end generated state_formulas::add_data_expressions code ---//

//--- start generated state_formulas::add_variables code ---//
template <template <class> class Builder, class Derived>
struct add_variables: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  state_formulas::true_ apply(const state_formulas::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  state_formulas::false_ apply(const state_formulas::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  state_formulas::not_ apply(const state_formulas::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::not_ result = state_formulas::not_(static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::and_ apply(const state_formulas::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::and_ result = state_formulas::and_(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::or_ apply(const state_formulas::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::or_ result = state_formulas::or_(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::imp apply(const state_formulas::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::imp result = state_formulas::imp(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::forall apply(const state_formulas::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::forall result = state_formulas::forall(static_cast<Derived&>(*this).apply(x.variables()), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::exists apply(const state_formulas::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::exists result = state_formulas::exists(static_cast<Derived&>(*this).apply(x.variables()), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::must apply(const state_formulas::must& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::must result = state_formulas::must(static_cast<Derived&>(*this).apply(x.formula()), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::may apply(const state_formulas::may& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::may result = state_formulas::may(static_cast<Derived&>(*this).apply(x.formula()), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::yaled apply(const state_formulas::yaled& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  state_formulas::yaled_timed apply(const state_formulas::yaled_timed& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::yaled_timed result = state_formulas::yaled_timed(static_cast<Derived&>(*this).apply(x.time_stamp()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::delay apply(const state_formulas::delay& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  state_formulas::delay_timed apply(const state_formulas::delay_timed& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::delay_timed result = state_formulas::delay_timed(static_cast<Derived&>(*this).apply(x.time_stamp()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::variable apply(const state_formulas::variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::variable result = state_formulas::variable(x.name(), static_cast<Derived&>(*this).apply(x.arguments()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::nu apply(const state_formulas::nu& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::nu result = state_formulas::nu(x.name(), static_cast<Derived&>(*this).apply(x.assignments()), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::mu apply(const state_formulas::mu& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::mu result = state_formulas::mu(x.name(), static_cast<Derived&>(*this).apply(x.assignments()), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::state_formula apply(const state_formulas::state_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::state_formula result;
    if (data::is_data_expression(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (data::is_untyped_data_parameter(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_data_parameter>(x));
    }
    else if (state_formulas::is_true(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::true_>(x));
    }
    else if (state_formulas::is_false(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::false_>(x));
    }
    else if (state_formulas::is_not(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::not_>(x));
    }
    else if (state_formulas::is_and(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::and_>(x));
    }
    else if (state_formulas::is_or(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::or_>(x));
    }
    else if (state_formulas::is_imp(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::imp>(x));
    }
    else if (state_formulas::is_forall(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::forall>(x));
    }
    else if (state_formulas::is_exists(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::exists>(x));
    }
    else if (state_formulas::is_must(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::must>(x));
    }
    else if (state_formulas::is_may(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::may>(x));
    }
    else if (state_formulas::is_yaled(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::yaled>(x));
    }
    else if (state_formulas::is_yaled_timed(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::yaled_timed>(x));
    }
    else if (state_formulas::is_delay(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::delay>(x));
    }
    else if (state_formulas::is_delay_timed(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::delay_timed>(x));
    }
    else if (state_formulas::is_variable(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::variable>(x));
    }
    else if (state_formulas::is_nu(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::nu>(x));
    }
    else if (state_formulas::is_mu(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::mu>(x));
    }
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

};

/// \brief Builder class
template <typename Derived>
struct variable_builder: public add_variables<regular_formulas::data_expression_builder, Derived>
{
  typedef add_variables<regular_formulas::data_expression_builder, Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;
};
//--- end generated state_formulas::add_variables code ---//

//--- start generated state_formulas::add_state_formula_expressions code ---//
template <template <class> class Builder, class Derived>
struct add_state_formula_expressions: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  state_formulas::true_ apply(const state_formulas::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  state_formulas::false_ apply(const state_formulas::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  state_formulas::not_ apply(const state_formulas::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::not_ result = state_formulas::not_(static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::and_ apply(const state_formulas::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::and_ result = state_formulas::and_(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::or_ apply(const state_formulas::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::or_ result = state_formulas::or_(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::imp apply(const state_formulas::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::imp result = state_formulas::imp(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::forall apply(const state_formulas::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::forall result = state_formulas::forall(x.variables(), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::exists apply(const state_formulas::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::exists result = state_formulas::exists(x.variables(), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::must apply(const state_formulas::must& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::must result = state_formulas::must(x.formula(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::may apply(const state_formulas::may& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::may result = state_formulas::may(x.formula(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::yaled apply(const state_formulas::yaled& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  state_formulas::yaled_timed apply(const state_formulas::yaled_timed& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  state_formulas::delay apply(const state_formulas::delay& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  state_formulas::delay_timed apply(const state_formulas::delay_timed& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  state_formulas::variable apply(const state_formulas::variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  state_formulas::nu apply(const state_formulas::nu& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::nu result = state_formulas::nu(x.name(), x.assignments(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::mu apply(const state_formulas::mu& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::mu result = state_formulas::mu(x.name(), x.assignments(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  state_formulas::state_formula apply(const state_formulas::state_formula& x)
  {
    static_cast<Derived&>(*this).enter(x);
    state_formulas::state_formula result;
    if (data::is_data_expression(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::data_expression>(x));
    }
    else if (data::is_untyped_data_parameter(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_data_parameter>(x));
    }
    else if (state_formulas::is_true(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::true_>(x));
    }
    else if (state_formulas::is_false(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::false_>(x));
    }
    else if (state_formulas::is_not(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::not_>(x));
    }
    else if (state_formulas::is_and(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::and_>(x));
    }
    else if (state_formulas::is_or(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::or_>(x));
    }
    else if (state_formulas::is_imp(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::imp>(x));
    }
    else if (state_formulas::is_forall(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::forall>(x));
    }
    else if (state_formulas::is_exists(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::exists>(x));
    }
    else if (state_formulas::is_must(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::must>(x));
    }
    else if (state_formulas::is_may(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::may>(x));
    }
    else if (state_formulas::is_yaled(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::yaled>(x));
    }
    else if (state_formulas::is_yaled_timed(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::yaled_timed>(x));
    }
    else if (state_formulas::is_delay(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::delay>(x));
    }
    else if (state_formulas::is_delay_timed(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::delay_timed>(x));
    }
    else if (state_formulas::is_variable(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::variable>(x));
    }
    else if (state_formulas::is_nu(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::nu>(x));
    }
    else if (state_formulas::is_mu(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<state_formulas::mu>(x));
    }
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

};

/// \brief Builder class
template <typename Derived>
struct state_formula_builder: public add_state_formula_expressions<state_formulas::state_formula_builder_base, Derived>
{
  typedef add_state_formula_expressions<state_formulas::state_formula_builder_base, Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;
};
//--- end generated state_formulas::add_state_formula_expressions code ---//

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_BUILDER_H
