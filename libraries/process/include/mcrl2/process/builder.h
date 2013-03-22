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
#include "mcrl2/lps/builder.h"
#include "mcrl2/process/process_specification.h"

namespace mcrl2
{

namespace process
{

/// \brief Traversal class for actions. Used as a base class for process_expression_traverser.
template <typename Derived>
struct process_expression_builder_base: public core::builder<Derived>
{
  typedef core::builder<Derived> super;
  using super::operator();
  using super::enter;
  using super::leave;

  const process_expression& operator()(const lps::action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return atermpp::aterm_cast<process_expression>(x);
  }
};

// Adds sort expression traversal to a builder
//--- start generated add_sort_expressions code ---//
template <template <class> class Builder, class Derived>
struct add_sort_expressions: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

  void operator()(process::process_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    x.action_labels() = static_cast<Derived&>(*this)(x.action_labels());
    static_cast<Derived&>(*this)(x.global_variables());
    static_cast<Derived&>(*this)(x.equations());
    x.init() = static_cast<Derived&>(*this)(x.init());
    static_cast<Derived&>(*this).leave(x);
  }

  process::process_identifier operator()(const process::process_identifier& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_identifier result = process::process_identifier(x.name(), static_cast<Derived&>(*this)(x.sorts()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_equation operator()(const process::process_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_equation result = process::process_equation(static_cast<Derived&>(*this)(x.identifier()), static_cast<Derived&>(*this)(x.formal_parameters()), static_cast<Derived&>(*this)(x.expression()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::process_instance& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::process_instance(static_cast<Derived&>(*this)(x.identifier()), static_cast<Derived&>(*this)(x.actual_parameters()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::process_instance_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::process_instance_assignment(static_cast<Derived&>(*this)(x.identifier()), static_cast<Derived&>(*this)(x.assignments()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::delta& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  process::process_expression operator()(const process::tau& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  process::process_expression operator()(const process::sum& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::sum(static_cast<Derived&>(*this)(x.bound_variables()), static_cast<Derived&>(*this)(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::block& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::block(x.block_set(), static_cast<Derived&>(*this)(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::hide& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::hide(x.hide_set(), static_cast<Derived&>(*this)(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::rename& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::rename(x.rename_set(), static_cast<Derived&>(*this)(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::comm& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::comm(x.comm_set(), static_cast<Derived&>(*this)(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::allow& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::allow(x.allow_set(), static_cast<Derived&>(*this)(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::sync& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::sync(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::at& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::at(static_cast<Derived&>(*this)(x.operand()), static_cast<Derived&>(*this)(x.time_stamp()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::seq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::seq(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::if_then& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::if_then(static_cast<Derived&>(*this)(x.condition()), static_cast<Derived&>(*this)(x.then_case()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::if_then_else& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::if_then_else(static_cast<Derived&>(*this)(x.condition()), static_cast<Derived&>(*this)(x.then_case()), static_cast<Derived&>(*this)(x.else_case()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::bounded_init& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::bounded_init(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::merge(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::left_merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::left_merge(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::choice& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::choice(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::parameter_identifier& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  process::process_expression operator()(const process::id_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::id_assignment(x.name(), static_cast<Derived&>(*this)(x.assignment()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::process_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result;
    if (process::is_process_instance(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::process_instance>(x));
    }
    else if (process::is_process_instance_assignment(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::process_instance_assignment>(x));
    }
    else if (process::is_delta(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::delta>(x));
    }
    else if (process::is_tau(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::tau>(x));
    }
    else if (process::is_sum(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::sum>(x));
    }
    else if (process::is_block(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::block>(x));
    }
    else if (process::is_hide(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::hide>(x));
    }
    else if (process::is_rename(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::rename>(x));
    }
    else if (process::is_comm(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::comm>(x));
    }
    else if (process::is_allow(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::allow>(x));
    }
    else if (process::is_sync(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::sync>(x));
    }
    else if (process::is_at(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::at>(x));
    }
    else if (process::is_seq(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::seq>(x));
    }
    else if (process::is_if_then(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::if_then>(x));
    }
    else if (process::is_if_then_else(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::if_then_else>(x));
    }
    else if (process::is_bounded_init(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::bounded_init>(x));
    }
    else if (process::is_merge(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::merge>(x));
    }
    else if (process::is_left_merge(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::left_merge>(x));
    }
    else if (process::is_choice(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::choice>(x));
    }
    else if (process::is_parameter_identifier(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::parameter_identifier>(x));
    }
    else if (process::is_id_assignment(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::id_assignment>(x));
    }
    else if (lps::is_action(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<lps::action>(x));
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
  using super::operator();
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
  using super::operator();

  void operator()(process::process_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.equations());
    x.init() = static_cast<Derived&>(*this)(x.init());
    static_cast<Derived&>(*this).leave(x);
  }

  process::process_equation operator()(const process::process_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_equation result = process::process_equation(x.identifier(), x.formal_parameters(), static_cast<Derived&>(*this)(x.expression()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::process_instance& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::process_instance(x.identifier(), static_cast<Derived&>(*this)(x.actual_parameters()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::process_instance_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::process_instance_assignment(x.identifier(), static_cast<Derived&>(*this)(x.assignments()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::delta& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  process::process_expression operator()(const process::tau& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  process::process_expression operator()(const process::sum& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::sum(x.bound_variables(), static_cast<Derived&>(*this)(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::block& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::block(x.block_set(), static_cast<Derived&>(*this)(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::hide& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::hide(x.hide_set(), static_cast<Derived&>(*this)(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::rename& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::rename(x.rename_set(), static_cast<Derived&>(*this)(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::comm& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::comm(x.comm_set(), static_cast<Derived&>(*this)(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::allow& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::allow(x.allow_set(), static_cast<Derived&>(*this)(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::sync& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::sync(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::at& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::at(static_cast<Derived&>(*this)(x.operand()), static_cast<Derived&>(*this)(x.time_stamp()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::seq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::seq(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::if_then& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::if_then(static_cast<Derived&>(*this)(x.condition()), static_cast<Derived&>(*this)(x.then_case()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::if_then_else& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::if_then_else(static_cast<Derived&>(*this)(x.condition()), static_cast<Derived&>(*this)(x.then_case()), static_cast<Derived&>(*this)(x.else_case()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::bounded_init& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::bounded_init(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::merge(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::left_merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::left_merge(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::choice& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::choice(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::parameter_identifier& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  process::process_expression operator()(const process::id_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::id_assignment(x.name(), static_cast<Derived&>(*this)(x.assignment()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::process_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result;
    if (process::is_process_instance(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::process_instance>(x));
    }
    else if (process::is_process_instance_assignment(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::process_instance_assignment>(x));
    }
    else if (process::is_delta(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::delta>(x));
    }
    else if (process::is_tau(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::tau>(x));
    }
    else if (process::is_sum(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::sum>(x));
    }
    else if (process::is_block(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::block>(x));
    }
    else if (process::is_hide(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::hide>(x));
    }
    else if (process::is_rename(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::rename>(x));
    }
    else if (process::is_comm(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::comm>(x));
    }
    else if (process::is_allow(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::allow>(x));
    }
    else if (process::is_sync(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::sync>(x));
    }
    else if (process::is_at(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::at>(x));
    }
    else if (process::is_seq(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::seq>(x));
    }
    else if (process::is_if_then(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::if_then>(x));
    }
    else if (process::is_if_then_else(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::if_then_else>(x));
    }
    else if (process::is_bounded_init(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::bounded_init>(x));
    }
    else if (process::is_merge(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::merge>(x));
    }
    else if (process::is_left_merge(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::left_merge>(x));
    }
    else if (process::is_choice(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::choice>(x));
    }
    else if (process::is_parameter_identifier(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::parameter_identifier>(x));
    }
    else if (process::is_id_assignment(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::id_assignment>(x));
    }
    else if (lps::is_action(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<lps::action>(x));
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
  using super::operator();
};
//--- end generated add_data_expressions code ---//

// Adds process expression traversal to a builder
//--- start generated add_process_expressions code ---//
template <template <class> class Builder, class Derived>
struct add_process_expressions: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

  void operator()(process::process_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.equations());
    x.init() = static_cast<Derived&>(*this)(x.init());
    static_cast<Derived&>(*this).leave(x);
  }

  process::process_equation operator()(const process::process_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_equation result = process::process_equation(x.identifier(), x.formal_parameters(), static_cast<Derived&>(*this)(x.expression()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::process_instance& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  process::process_expression operator()(const process::process_instance_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  process::process_expression operator()(const process::delta& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  process::process_expression operator()(const process::tau& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  process::process_expression operator()(const process::sum& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::sum(x.bound_variables(), static_cast<Derived&>(*this)(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::block& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::block(x.block_set(), static_cast<Derived&>(*this)(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::hide& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::hide(x.hide_set(), static_cast<Derived&>(*this)(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::rename& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::rename(x.rename_set(), static_cast<Derived&>(*this)(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::comm& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::comm(x.comm_set(), static_cast<Derived&>(*this)(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::allow& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::allow(x.allow_set(), static_cast<Derived&>(*this)(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::sync& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::sync(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::at& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::at(static_cast<Derived&>(*this)(x.operand()), x.time_stamp());
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::seq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::seq(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::if_then& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::if_then(x.condition(), static_cast<Derived&>(*this)(x.then_case()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::if_then_else& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::if_then_else(x.condition(), static_cast<Derived&>(*this)(x.then_case()), static_cast<Derived&>(*this)(x.else_case()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::bounded_init& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::bounded_init(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::merge(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::left_merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::left_merge(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::choice& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result = process::choice(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process::process_expression operator()(const process::parameter_identifier& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  process::process_expression operator()(const process::id_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  process::process_expression operator()(const process::process_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result;
    if (process::is_process_instance(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::process_instance>(x));
    }
    else if (process::is_process_instance_assignment(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::process_instance_assignment>(x));
    }
    else if (process::is_delta(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::delta>(x));
    }
    else if (process::is_tau(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::tau>(x));
    }
    else if (process::is_sum(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::sum>(x));
    }
    else if (process::is_block(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::block>(x));
    }
    else if (process::is_hide(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::hide>(x));
    }
    else if (process::is_rename(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::rename>(x));
    }
    else if (process::is_comm(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::comm>(x));
    }
    else if (process::is_allow(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::allow>(x));
    }
    else if (process::is_sync(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::sync>(x));
    }
    else if (process::is_at(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::at>(x));
    }
    else if (process::is_seq(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::seq>(x));
    }
    else if (process::is_if_then(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::if_then>(x));
    }
    else if (process::is_if_then_else(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::if_then_else>(x));
    }
    else if (process::is_bounded_init(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::bounded_init>(x));
    }
    else if (process::is_merge(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::merge>(x));
    }
    else if (process::is_left_merge(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::left_merge>(x));
    }
    else if (process::is_choice(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::choice>(x));
    }
    else if (process::is_parameter_identifier(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::parameter_identifier>(x));
    }
    else if (process::is_id_assignment(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<process::id_assignment>(x));
    }
    else if (lps::is_action(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<lps::action>(x));
    }
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

};

/// \brief Builder class
template <typename Derived>
struct process_expression_builder: public add_process_expressions<process::process_expression_builder_base, Derived>
{
  typedef add_process_expressions<process::process_expression_builder_base, Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
};
//--- end generated add_process_expressions code ---//

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_BUILDER_H
