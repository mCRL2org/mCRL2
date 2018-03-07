// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/builder.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_BUILDER_H
#define MCRL2_PROCESS_BUILDER_H

#include "mcrl2/core/builder.h"
#include "mcrl2/data/builder.h"
#include "mcrl2/process/process_specification.h"
#include "mcrl2/process/untyped_multi_action.h"

namespace mcrl2
{

namespace process
{

// Adds sort expression traversal to a builder
//--- start generated add_sort_expressions code ---//
template <template <class> class Builder, class Derived>
struct add_sort_expressions: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  process::action_label apply(const process::action_label& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::action_label result = process::action_label(x.name(), static_cast<Derived&>(*this).apply(x.sorts()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  void update(process::process_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    x.action_labels() = static_cast<Derived&>(*this).apply(x.action_labels());
    static_cast<Derived&>(*this).update(x.global_variables());
    static_cast<Derived&>(*this).update(x.equations());
    x.init() = static_cast<Derived&>(*this).apply(x.init());
    static_cast<Derived&>(*this).leave(x);
  }

  process::process_identifier apply(const process::process_identifier& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_identifier result = process::process_identifier(x.name(), static_cast<Derived&>(*this).apply(x.variables()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_equation apply(const process::process_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_equation result = process::process_equation(static_cast<Derived&>(*this).apply(x.identifier()), static_cast<Derived&>(*this).apply(x.formal_parameters()), static_cast<Derived&>(*this).apply(x.expression()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::untyped_multi_action apply(const process::untyped_multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::untyped_multi_action result = process::untyped_multi_action(static_cast<Derived&>(*this).apply(x.actions()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::action apply(const process::action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::action result = process::action(static_cast<Derived&>(*this).apply(x.label()), static_cast<Derived&>(*this).apply(x.arguments()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_instance apply(const process::process_instance& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_instance result = process::process_instance(static_cast<Derived&>(*this).apply(x.identifier()), static_cast<Derived&>(*this).apply(x.actual_parameters()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_instance_assignment apply(const process::process_instance_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_instance_assignment result = process::process_instance_assignment(static_cast<Derived&>(*this).apply(x.identifier()), static_cast<Derived&>(*this).apply(x.assignments()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::delta apply(const process::delta& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  process::tau apply(const process::tau& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  process::sum apply(const process::sum& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::sum result = process::sum(static_cast<Derived&>(*this).apply(x.variables()), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::block apply(const process::block& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::block result = process::block(x.block_set(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::hide apply(const process::hide& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::hide result = process::hide(x.hide_set(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::rename apply(const process::rename& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::rename result = process::rename(x.rename_set(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::comm apply(const process::comm& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::comm result = process::comm(x.comm_set(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::allow apply(const process::allow& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::allow result = process::allow(x.allow_set(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::sync apply(const process::sync& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::sync result = process::sync(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::at apply(const process::at& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::at result = process::at(static_cast<Derived&>(*this).apply(x.operand()), static_cast<Derived&>(*this).apply(x.time_stamp()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::seq apply(const process::seq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::seq result = process::seq(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::if_then apply(const process::if_then& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::if_then result = process::if_then(static_cast<Derived&>(*this).apply(x.condition()), static_cast<Derived&>(*this).apply(x.then_case()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::if_then_else apply(const process::if_then_else& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::if_then_else result = process::if_then_else(static_cast<Derived&>(*this).apply(x.condition()), static_cast<Derived&>(*this).apply(x.then_case()), static_cast<Derived&>(*this).apply(x.else_case()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::bounded_init apply(const process::bounded_init& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::bounded_init result = process::bounded_init(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::merge apply(const process::merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::merge result = process::merge(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::left_merge apply(const process::left_merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::left_merge result = process::left_merge(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::choice apply(const process::choice& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::choice result = process::choice(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::stochastic_operator apply(const process::stochastic_operator& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::stochastic_operator result = process::stochastic_operator(static_cast<Derived&>(*this).apply(x.variables()), static_cast<Derived&>(*this).apply(x.distribution()), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::untyped_process_assignment apply(const process::untyped_process_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::untyped_process_assignment result = process::untyped_process_assignment(x.name(), static_cast<Derived&>(*this).apply(x.assignments()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression apply(const process::process_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result;
    if (process::is_action(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::action>(x));
    }
    else if (process::is_process_instance(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::process_instance>(x));
    }
    else if (process::is_process_instance_assignment(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::process_instance_assignment>(x));
    }
    else if (process::is_delta(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::delta>(x));
    }
    else if (process::is_tau(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::tau>(x));
    }
    else if (process::is_sum(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::sum>(x));
    }
    else if (process::is_block(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::block>(x));
    }
    else if (process::is_hide(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::hide>(x));
    }
    else if (process::is_rename(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::rename>(x));
    }
    else if (process::is_comm(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::comm>(x));
    }
    else if (process::is_allow(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::allow>(x));
    }
    else if (process::is_sync(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::sync>(x));
    }
    else if (process::is_at(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::at>(x));
    }
    else if (process::is_seq(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::seq>(x));
    }
    else if (process::is_if_then(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::if_then>(x));
    }
    else if (process::is_if_then_else(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::if_then_else>(x));
    }
    else if (process::is_bounded_init(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::bounded_init>(x));
    }
    else if (process::is_merge(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::merge>(x));
    }
    else if (process::is_left_merge(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::left_merge>(x));
    }
    else if (process::is_choice(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::choice>(x));
    }
    else if (process::is_stochastic_operator(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::stochastic_operator>(x));
    }
    else if (process::is_untyped_process_assignment(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::untyped_process_assignment>(x));
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
struct sort_expression_builder: public add_sort_expressions<data::sort_expression_builder, Derived>
{
  typedef add_sort_expressions<data::sort_expression_builder, Derived> super;
};
//--- end generated add_sort_expressions code ---//

// Adds data expression traversal to a builder
//--- start generated add_data_expressions code ---//
template <template <class> class Builder, class Derived>
struct add_data_expressions: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  void update(process::process_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).update(x.equations());
    x.init() = static_cast<Derived&>(*this).apply(x.init());
    static_cast<Derived&>(*this).leave(x);
  }

  process::process_equation apply(const process::process_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_equation result = process::process_equation(x.identifier(), x.formal_parameters(), static_cast<Derived&>(*this).apply(x.expression()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::untyped_multi_action apply(const process::untyped_multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::untyped_multi_action result = process::untyped_multi_action(static_cast<Derived&>(*this).apply(x.actions()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::action apply(const process::action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::action result = process::action(x.label(), static_cast<Derived&>(*this).apply(x.arguments()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_instance apply(const process::process_instance& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_instance result = process::process_instance(x.identifier(), static_cast<Derived&>(*this).apply(x.actual_parameters()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_instance_assignment apply(const process::process_instance_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_instance_assignment result = process::process_instance_assignment(x.identifier(), static_cast<Derived&>(*this).apply(x.assignments()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::delta apply(const process::delta& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  process::tau apply(const process::tau& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  process::sum apply(const process::sum& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::sum result = process::sum(x.variables(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::block apply(const process::block& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::block result = process::block(x.block_set(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::hide apply(const process::hide& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::hide result = process::hide(x.hide_set(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::rename apply(const process::rename& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::rename result = process::rename(x.rename_set(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::comm apply(const process::comm& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::comm result = process::comm(x.comm_set(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::allow apply(const process::allow& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::allow result = process::allow(x.allow_set(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::sync apply(const process::sync& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::sync result = process::sync(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::at apply(const process::at& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::at result = process::at(static_cast<Derived&>(*this).apply(x.operand()), static_cast<Derived&>(*this).apply(x.time_stamp()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::seq apply(const process::seq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::seq result = process::seq(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::if_then apply(const process::if_then& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::if_then result = process::if_then(static_cast<Derived&>(*this).apply(x.condition()), static_cast<Derived&>(*this).apply(x.then_case()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::if_then_else apply(const process::if_then_else& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::if_then_else result = process::if_then_else(static_cast<Derived&>(*this).apply(x.condition()), static_cast<Derived&>(*this).apply(x.then_case()), static_cast<Derived&>(*this).apply(x.else_case()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::bounded_init apply(const process::bounded_init& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::bounded_init result = process::bounded_init(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::merge apply(const process::merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::merge result = process::merge(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::left_merge apply(const process::left_merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::left_merge result = process::left_merge(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::choice apply(const process::choice& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::choice result = process::choice(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::stochastic_operator apply(const process::stochastic_operator& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::stochastic_operator result = process::stochastic_operator(x.variables(), static_cast<Derived&>(*this).apply(x.distribution()), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::untyped_process_assignment apply(const process::untyped_process_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::untyped_process_assignment result = process::untyped_process_assignment(x.name(), static_cast<Derived&>(*this).apply(x.assignments()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression apply(const process::process_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result;
    if (process::is_action(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::action>(x));
    }
    else if (process::is_process_instance(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::process_instance>(x));
    }
    else if (process::is_process_instance_assignment(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::process_instance_assignment>(x));
    }
    else if (process::is_delta(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::delta>(x));
    }
    else if (process::is_tau(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::tau>(x));
    }
    else if (process::is_sum(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::sum>(x));
    }
    else if (process::is_block(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::block>(x));
    }
    else if (process::is_hide(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::hide>(x));
    }
    else if (process::is_rename(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::rename>(x));
    }
    else if (process::is_comm(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::comm>(x));
    }
    else if (process::is_allow(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::allow>(x));
    }
    else if (process::is_sync(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::sync>(x));
    }
    else if (process::is_at(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::at>(x));
    }
    else if (process::is_seq(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::seq>(x));
    }
    else if (process::is_if_then(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::if_then>(x));
    }
    else if (process::is_if_then_else(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::if_then_else>(x));
    }
    else if (process::is_bounded_init(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::bounded_init>(x));
    }
    else if (process::is_merge(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::merge>(x));
    }
    else if (process::is_left_merge(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::left_merge>(x));
    }
    else if (process::is_choice(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::choice>(x));
    }
    else if (process::is_stochastic_operator(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::stochastic_operator>(x));
    }
    else if (process::is_untyped_process_assignment(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::untyped_process_assignment>(x));
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
struct data_expression_builder: public add_data_expressions<data::data_expression_builder, Derived>
{
  typedef add_data_expressions<data::data_expression_builder, Derived> super;
};
//--- end generated add_data_expressions code ---//

//--- start generated add_variables code ---//
template <template <class> class Builder, class Derived>
struct add_variables: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  void update(process::process_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).update(x.global_variables());
    static_cast<Derived&>(*this).update(x.equations());
    x.init() = static_cast<Derived&>(*this).apply(x.init());
    static_cast<Derived&>(*this).leave(x);
  }

  process::process_identifier apply(const process::process_identifier& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_identifier result = process::process_identifier(x.name(), static_cast<Derived&>(*this).apply(x.variables()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_equation apply(const process::process_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_equation result = process::process_equation(static_cast<Derived&>(*this).apply(x.identifier()), static_cast<Derived&>(*this).apply(x.formal_parameters()), static_cast<Derived&>(*this).apply(x.expression()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::untyped_multi_action apply(const process::untyped_multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::untyped_multi_action result = process::untyped_multi_action(static_cast<Derived&>(*this).apply(x.actions()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::action apply(const process::action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::action result = process::action(x.label(), static_cast<Derived&>(*this).apply(x.arguments()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_instance apply(const process::process_instance& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_instance result = process::process_instance(static_cast<Derived&>(*this).apply(x.identifier()), static_cast<Derived&>(*this).apply(x.actual_parameters()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_instance_assignment apply(const process::process_instance_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_instance_assignment result = process::process_instance_assignment(static_cast<Derived&>(*this).apply(x.identifier()), static_cast<Derived&>(*this).apply(x.assignments()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::delta apply(const process::delta& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  process::tau apply(const process::tau& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  process::sum apply(const process::sum& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::sum result = process::sum(static_cast<Derived&>(*this).apply(x.variables()), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::block apply(const process::block& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::block result = process::block(x.block_set(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::hide apply(const process::hide& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::hide result = process::hide(x.hide_set(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::rename apply(const process::rename& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::rename result = process::rename(x.rename_set(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::comm apply(const process::comm& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::comm result = process::comm(x.comm_set(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::allow apply(const process::allow& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::allow result = process::allow(x.allow_set(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::sync apply(const process::sync& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::sync result = process::sync(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::at apply(const process::at& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::at result = process::at(static_cast<Derived&>(*this).apply(x.operand()), static_cast<Derived&>(*this).apply(x.time_stamp()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::seq apply(const process::seq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::seq result = process::seq(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::if_then apply(const process::if_then& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::if_then result = process::if_then(static_cast<Derived&>(*this).apply(x.condition()), static_cast<Derived&>(*this).apply(x.then_case()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::if_then_else apply(const process::if_then_else& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::if_then_else result = process::if_then_else(static_cast<Derived&>(*this).apply(x.condition()), static_cast<Derived&>(*this).apply(x.then_case()), static_cast<Derived&>(*this).apply(x.else_case()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::bounded_init apply(const process::bounded_init& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::bounded_init result = process::bounded_init(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::merge apply(const process::merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::merge result = process::merge(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::left_merge apply(const process::left_merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::left_merge result = process::left_merge(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::choice apply(const process::choice& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::choice result = process::choice(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::stochastic_operator apply(const process::stochastic_operator& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::stochastic_operator result = process::stochastic_operator(static_cast<Derived&>(*this).apply(x.variables()), static_cast<Derived&>(*this).apply(x.distribution()), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::untyped_process_assignment apply(const process::untyped_process_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::untyped_process_assignment result = process::untyped_process_assignment(x.name(), static_cast<Derived&>(*this).apply(x.assignments()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression apply(const process::process_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result;
    if (process::is_action(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::action>(x));
    }
    else if (process::is_process_instance(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::process_instance>(x));
    }
    else if (process::is_process_instance_assignment(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::process_instance_assignment>(x));
    }
    else if (process::is_delta(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::delta>(x));
    }
    else if (process::is_tau(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::tau>(x));
    }
    else if (process::is_sum(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::sum>(x));
    }
    else if (process::is_block(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::block>(x));
    }
    else if (process::is_hide(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::hide>(x));
    }
    else if (process::is_rename(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::rename>(x));
    }
    else if (process::is_comm(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::comm>(x));
    }
    else if (process::is_allow(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::allow>(x));
    }
    else if (process::is_sync(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::sync>(x));
    }
    else if (process::is_at(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::at>(x));
    }
    else if (process::is_seq(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::seq>(x));
    }
    else if (process::is_if_then(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::if_then>(x));
    }
    else if (process::is_if_then_else(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::if_then_else>(x));
    }
    else if (process::is_bounded_init(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::bounded_init>(x));
    }
    else if (process::is_merge(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::merge>(x));
    }
    else if (process::is_left_merge(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::left_merge>(x));
    }
    else if (process::is_choice(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::choice>(x));
    }
    else if (process::is_stochastic_operator(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::stochastic_operator>(x));
    }
    else if (process::is_untyped_process_assignment(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::untyped_process_assignment>(x));
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
struct variable_builder: public add_variables<data::data_expression_builder, Derived>
{
  typedef add_variables<data::data_expression_builder, Derived> super;
};
//--- end generated add_variables code ---//

// Adds process expression traversal to a builder
//--- start generated add_process_expressions code ---//
template <template <class> class Builder, class Derived>
struct add_process_expressions: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  void update(process::process_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).update(x.equations());
    x.init() = static_cast<Derived&>(*this).apply(x.init());
    static_cast<Derived&>(*this).leave(x);
  }

  process::process_equation apply(const process::process_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_equation result = process::process_equation(x.identifier(), x.formal_parameters(), static_cast<Derived&>(*this).apply(x.expression()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::action apply(const process::action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  process::process_instance apply(const process::process_instance& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  process::process_instance_assignment apply(const process::process_instance_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  process::delta apply(const process::delta& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  process::tau apply(const process::tau& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  process::sum apply(const process::sum& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::sum result = process::sum(x.variables(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::block apply(const process::block& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::block result = process::block(x.block_set(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::hide apply(const process::hide& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::hide result = process::hide(x.hide_set(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::rename apply(const process::rename& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::rename result = process::rename(x.rename_set(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::comm apply(const process::comm& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::comm result = process::comm(x.comm_set(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::allow apply(const process::allow& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::allow result = process::allow(x.allow_set(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::sync apply(const process::sync& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::sync result = process::sync(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::at apply(const process::at& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::at result = process::at(static_cast<Derived&>(*this).apply(x.operand()), x.time_stamp());
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::seq apply(const process::seq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::seq result = process::seq(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::if_then apply(const process::if_then& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::if_then result = process::if_then(x.condition(), static_cast<Derived&>(*this).apply(x.then_case()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::if_then_else apply(const process::if_then_else& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::if_then_else result = process::if_then_else(x.condition(), static_cast<Derived&>(*this).apply(x.then_case()), static_cast<Derived&>(*this).apply(x.else_case()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::bounded_init apply(const process::bounded_init& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::bounded_init result = process::bounded_init(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::merge apply(const process::merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::merge result = process::merge(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::left_merge apply(const process::left_merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::left_merge result = process::left_merge(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::choice apply(const process::choice& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::choice result = process::choice(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::stochastic_operator apply(const process::stochastic_operator& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::stochastic_operator result = process::stochastic_operator(x.variables(), x.distribution(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::untyped_process_assignment apply(const process::untyped_process_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  process::process_expression apply(const process::process_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result;
    if (process::is_action(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::action>(x));
    }
    else if (process::is_process_instance(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::process_instance>(x));
    }
    else if (process::is_process_instance_assignment(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::process_instance_assignment>(x));
    }
    else if (process::is_delta(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::delta>(x));
    }
    else if (process::is_tau(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::tau>(x));
    }
    else if (process::is_sum(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::sum>(x));
    }
    else if (process::is_block(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::block>(x));
    }
    else if (process::is_hide(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::hide>(x));
    }
    else if (process::is_rename(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::rename>(x));
    }
    else if (process::is_comm(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::comm>(x));
    }
    else if (process::is_allow(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::allow>(x));
    }
    else if (process::is_sync(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::sync>(x));
    }
    else if (process::is_at(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::at>(x));
    }
    else if (process::is_seq(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::seq>(x));
    }
    else if (process::is_if_then(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::if_then>(x));
    }
    else if (process::is_if_then_else(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::if_then_else>(x));
    }
    else if (process::is_bounded_init(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::bounded_init>(x));
    }
    else if (process::is_merge(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::merge>(x));
    }
    else if (process::is_left_merge(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::left_merge>(x));
    }
    else if (process::is_choice(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::choice>(x));
    }
    else if (process::is_stochastic_operator(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::stochastic_operator>(x));
    }
    else if (process::is_untyped_process_assignment(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::untyped_process_assignment>(x));
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
struct process_expression_builder: public add_process_expressions<core::builder, Derived>
{
  typedef add_process_expressions<core::builder, Derived> super;
};
//--- end generated add_process_expressions code ---//

//--- start generated add_process_identifiers code ---//
template <template <class> class Builder, class Derived>
struct add_process_identifiers: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  void update(process::process_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).update(x.equations());
    x.init() = static_cast<Derived&>(*this).apply(x.init());
    static_cast<Derived&>(*this).leave(x);
  }

  process::process_identifier apply(const process::process_identifier& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  process::process_equation apply(const process::process_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_equation result = process::process_equation(static_cast<Derived&>(*this).apply(x.identifier()), x.formal_parameters(), static_cast<Derived&>(*this).apply(x.expression()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::action apply(const process::action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  process::process_instance apply(const process::process_instance& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_instance result = process::process_instance(static_cast<Derived&>(*this).apply(x.identifier()), x.actual_parameters());
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_instance_assignment apply(const process::process_instance_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_instance_assignment result = process::process_instance_assignment(static_cast<Derived&>(*this).apply(x.identifier()), x.assignments());
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::delta apply(const process::delta& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  process::tau apply(const process::tau& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  process::sum apply(const process::sum& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::sum result = process::sum(x.variables(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::block apply(const process::block& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::block result = process::block(x.block_set(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::hide apply(const process::hide& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::hide result = process::hide(x.hide_set(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::rename apply(const process::rename& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::rename result = process::rename(x.rename_set(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::comm apply(const process::comm& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::comm result = process::comm(x.comm_set(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::allow apply(const process::allow& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::allow result = process::allow(x.allow_set(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::sync apply(const process::sync& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::sync result = process::sync(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::at apply(const process::at& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::at result = process::at(static_cast<Derived&>(*this).apply(x.operand()), x.time_stamp());
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::seq apply(const process::seq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::seq result = process::seq(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::if_then apply(const process::if_then& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::if_then result = process::if_then(x.condition(), static_cast<Derived&>(*this).apply(x.then_case()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::if_then_else apply(const process::if_then_else& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::if_then_else result = process::if_then_else(x.condition(), static_cast<Derived&>(*this).apply(x.then_case()), static_cast<Derived&>(*this).apply(x.else_case()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::bounded_init apply(const process::bounded_init& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::bounded_init result = process::bounded_init(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::merge apply(const process::merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::merge result = process::merge(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::left_merge apply(const process::left_merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::left_merge result = process::left_merge(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::choice apply(const process::choice& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::choice result = process::choice(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::stochastic_operator apply(const process::stochastic_operator& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::stochastic_operator result = process::stochastic_operator(x.variables(), x.distribution(), static_cast<Derived&>(*this).apply(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::untyped_process_assignment apply(const process::untyped_process_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  process::process_expression apply(const process::process_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result;
    if (process::is_action(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::action>(x));
    }
    else if (process::is_process_instance(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::process_instance>(x));
    }
    else if (process::is_process_instance_assignment(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::process_instance_assignment>(x));
    }
    else if (process::is_delta(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::delta>(x));
    }
    else if (process::is_tau(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::tau>(x));
    }
    else if (process::is_sum(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::sum>(x));
    }
    else if (process::is_block(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::block>(x));
    }
    else if (process::is_hide(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::hide>(x));
    }
    else if (process::is_rename(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::rename>(x));
    }
    else if (process::is_comm(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::comm>(x));
    }
    else if (process::is_allow(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::allow>(x));
    }
    else if (process::is_sync(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::sync>(x));
    }
    else if (process::is_at(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::at>(x));
    }
    else if (process::is_seq(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::seq>(x));
    }
    else if (process::is_if_then(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::if_then>(x));
    }
    else if (process::is_if_then_else(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::if_then_else>(x));
    }
    else if (process::is_bounded_init(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::bounded_init>(x));
    }
    else if (process::is_merge(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::merge>(x));
    }
    else if (process::is_left_merge(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::left_merge>(x));
    }
    else if (process::is_choice(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::choice>(x));
    }
    else if (process::is_stochastic_operator(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::stochastic_operator>(x));
    }
    else if (process::is_untyped_process_assignment(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<process::untyped_process_assignment>(x));
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
struct process_identifier_builder: public add_process_identifiers<core::builder, Derived>
{
  typedef add_process_identifiers<core::builder, Derived> super;
};
//--- end generated add_process_identifiers code ---//

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_BUILDER_H
