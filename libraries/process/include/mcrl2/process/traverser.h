// Author(s): Jeroen van der Wulp, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/traverser.h
/// \brief add your file description here.

// To avoid circular inclusion problems
#ifndef MCRL2_PROCESS_SPECIFICATION_H
#include "mcrl2/process/process_specification.h"
#endif

#ifndef MCRL2_PROCESS_TRAVERSER_H
#define MCRL2_PROCESS_TRAVERSER_H

#include "mcrl2/data/traverser.h"
#include "mcrl2/process/process_specification.h"
#include "mcrl2/process/untyped_multi_action.h"

namespace mcrl2
{

namespace process
{

//--- start generated add_traverser_sort_expressions code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_sort_expressions: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const process::action_label& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.sorts());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.action_labels());
    static_cast<Derived&>(*this).apply(x.global_variables());
    static_cast<Derived&>(*this).apply(x.equations());
    static_cast<Derived&>(*this).apply(x.init());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_identifier& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.identifier());
    static_cast<Derived&>(*this).apply(x.formal_parameters());
    static_cast<Derived&>(*this).apply(x.expression());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::untyped_multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.actions());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.label());
    static_cast<Derived&>(*this).apply(x.arguments());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_instance& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.identifier());
    static_cast<Derived&>(*this).apply(x.actual_parameters());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_instance_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.identifier());
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::delta& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::tau& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::sum& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::block& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::hide& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::rename& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::comm& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::allow& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::sync& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::at& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).apply(x.time_stamp());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::seq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::if_then& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.condition());
    static_cast<Derived&>(*this).apply(x.then_case());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::if_then_else& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.condition());
    static_cast<Derived&>(*this).apply(x.then_case());
    static_cast<Derived&>(*this).apply(x.else_case());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::bounded_init& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::left_merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::choice& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::stochastic_operator& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.distribution());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::untyped_process_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (process::is_action(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::action>(x));
    }
    else if (process::is_process_instance(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::process_instance>(x));
    }
    else if (process::is_process_instance_assignment(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::process_instance_assignment>(x));
    }
    else if (process::is_delta(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::delta>(x));
    }
    else if (process::is_tau(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::tau>(x));
    }
    else if (process::is_sum(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::sum>(x));
    }
    else if (process::is_block(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::block>(x));
    }
    else if (process::is_hide(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::hide>(x));
    }
    else if (process::is_rename(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::rename>(x));
    }
    else if (process::is_comm(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::comm>(x));
    }
    else if (process::is_allow(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::allow>(x));
    }
    else if (process::is_sync(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::sync>(x));
    }
    else if (process::is_at(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::at>(x));
    }
    else if (process::is_seq(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::seq>(x));
    }
    else if (process::is_if_then(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::if_then>(x));
    }
    else if (process::is_if_then_else(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::if_then_else>(x));
    }
    else if (process::is_bounded_init(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::bounded_init>(x));
    }
    else if (process::is_merge(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::merge>(x));
    }
    else if (process::is_left_merge(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::left_merge>(x));
    }
    else if (process::is_choice(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::choice>(x));
    }
    else if (process::is_stochastic_operator(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::stochastic_operator>(x));
    }
    else if (process::is_untyped_process_assignment(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::untyped_process_assignment>(x));
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
struct sort_expression_traverser: public add_traverser_sort_expressions<data::sort_expression_traverser, Derived>
{
  typedef add_traverser_sort_expressions<data::sort_expression_traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
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

  void apply(const process::process_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.equations());
    static_cast<Derived&>(*this).apply(x.init());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.expression());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::untyped_multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.actions());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.arguments());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_instance& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.actual_parameters());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_instance_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::delta& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::tau& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::sum& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::block& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::hide& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::rename& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::comm& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::allow& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::sync& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::at& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).apply(x.time_stamp());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::seq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::if_then& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.condition());
    static_cast<Derived&>(*this).apply(x.then_case());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::if_then_else& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.condition());
    static_cast<Derived&>(*this).apply(x.then_case());
    static_cast<Derived&>(*this).apply(x.else_case());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::bounded_init& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::left_merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::choice& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::stochastic_operator& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.distribution());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::untyped_process_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (process::is_action(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::action>(x));
    }
    else if (process::is_process_instance(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::process_instance>(x));
    }
    else if (process::is_process_instance_assignment(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::process_instance_assignment>(x));
    }
    else if (process::is_delta(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::delta>(x));
    }
    else if (process::is_tau(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::tau>(x));
    }
    else if (process::is_sum(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::sum>(x));
    }
    else if (process::is_block(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::block>(x));
    }
    else if (process::is_hide(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::hide>(x));
    }
    else if (process::is_rename(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::rename>(x));
    }
    else if (process::is_comm(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::comm>(x));
    }
    else if (process::is_allow(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::allow>(x));
    }
    else if (process::is_sync(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::sync>(x));
    }
    else if (process::is_at(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::at>(x));
    }
    else if (process::is_seq(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::seq>(x));
    }
    else if (process::is_if_then(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::if_then>(x));
    }
    else if (process::is_if_then_else(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::if_then_else>(x));
    }
    else if (process::is_bounded_init(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::bounded_init>(x));
    }
    else if (process::is_merge(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::merge>(x));
    }
    else if (process::is_left_merge(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::left_merge>(x));
    }
    else if (process::is_choice(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::choice>(x));
    }
    else if (process::is_stochastic_operator(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::stochastic_operator>(x));
    }
    else if (process::is_untyped_process_assignment(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::untyped_process_assignment>(x));
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
struct data_expression_traverser: public add_traverser_data_expressions<data::data_expression_traverser, Derived>
{
  typedef add_traverser_data_expressions<data::data_expression_traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
};
//--- end generated add_traverser_data_expressions code ---//

//--- start generated add_traverser_process_expressions code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_process_expressions: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const process::process_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.equations());
    static_cast<Derived&>(*this).apply(x.init());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.expression());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_instance& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_instance_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::delta& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::tau& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::sum& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::block& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::hide& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::rename& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::comm& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::allow& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::sync& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::at& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::seq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::if_then& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.then_case());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::if_then_else& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.then_case());
    static_cast<Derived&>(*this).apply(x.else_case());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::bounded_init& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::left_merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::choice& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::stochastic_operator& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::untyped_process_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (process::is_action(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::action>(x));
    }
    else if (process::is_process_instance(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::process_instance>(x));
    }
    else if (process::is_process_instance_assignment(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::process_instance_assignment>(x));
    }
    else if (process::is_delta(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::delta>(x));
    }
    else if (process::is_tau(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::tau>(x));
    }
    else if (process::is_sum(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::sum>(x));
    }
    else if (process::is_block(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::block>(x));
    }
    else if (process::is_hide(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::hide>(x));
    }
    else if (process::is_rename(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::rename>(x));
    }
    else if (process::is_comm(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::comm>(x));
    }
    else if (process::is_allow(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::allow>(x));
    }
    else if (process::is_sync(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::sync>(x));
    }
    else if (process::is_at(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::at>(x));
    }
    else if (process::is_seq(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::seq>(x));
    }
    else if (process::is_if_then(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::if_then>(x));
    }
    else if (process::is_if_then_else(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::if_then_else>(x));
    }
    else if (process::is_bounded_init(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::bounded_init>(x));
    }
    else if (process::is_merge(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::merge>(x));
    }
    else if (process::is_left_merge(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::left_merge>(x));
    }
    else if (process::is_choice(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::choice>(x));
    }
    else if (process::is_stochastic_operator(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::stochastic_operator>(x));
    }
    else if (process::is_untyped_process_assignment(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::untyped_process_assignment>(x));
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
struct process_expression_traverser: public add_traverser_process_expressions<core::traverser, Derived>
{
  typedef add_traverser_process_expressions<core::traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
};
//--- end generated add_traverser_process_expressions code ---//

//--- start generated add_traverser_variables code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_variables: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const process::process_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.global_variables());
    static_cast<Derived&>(*this).apply(x.equations());
    static_cast<Derived&>(*this).apply(x.init());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_identifier& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.identifier());
    static_cast<Derived&>(*this).apply(x.formal_parameters());
    static_cast<Derived&>(*this).apply(x.expression());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::untyped_multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.actions());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.arguments());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_instance& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.identifier());
    static_cast<Derived&>(*this).apply(x.actual_parameters());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_instance_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.identifier());
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::delta& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::tau& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::sum& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::block& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::hide& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::rename& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::comm& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::allow& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::sync& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::at& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).apply(x.time_stamp());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::seq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::if_then& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.condition());
    static_cast<Derived&>(*this).apply(x.then_case());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::if_then_else& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.condition());
    static_cast<Derived&>(*this).apply(x.then_case());
    static_cast<Derived&>(*this).apply(x.else_case());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::bounded_init& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::left_merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::choice& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::stochastic_operator& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.distribution());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::untyped_process_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (process::is_action(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::action>(x));
    }
    else if (process::is_process_instance(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::process_instance>(x));
    }
    else if (process::is_process_instance_assignment(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::process_instance_assignment>(x));
    }
    else if (process::is_delta(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::delta>(x));
    }
    else if (process::is_tau(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::tau>(x));
    }
    else if (process::is_sum(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::sum>(x));
    }
    else if (process::is_block(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::block>(x));
    }
    else if (process::is_hide(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::hide>(x));
    }
    else if (process::is_rename(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::rename>(x));
    }
    else if (process::is_comm(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::comm>(x));
    }
    else if (process::is_allow(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::allow>(x));
    }
    else if (process::is_sync(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::sync>(x));
    }
    else if (process::is_at(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::at>(x));
    }
    else if (process::is_seq(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::seq>(x));
    }
    else if (process::is_if_then(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::if_then>(x));
    }
    else if (process::is_if_then_else(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::if_then_else>(x));
    }
    else if (process::is_bounded_init(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::bounded_init>(x));
    }
    else if (process::is_merge(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::merge>(x));
    }
    else if (process::is_left_merge(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::left_merge>(x));
    }
    else if (process::is_choice(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::choice>(x));
    }
    else if (process::is_stochastic_operator(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::stochastic_operator>(x));
    }
    else if (process::is_untyped_process_assignment(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::untyped_process_assignment>(x));
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
struct variable_traverser: public add_traverser_variables<data::variable_traverser, Derived>
{
  typedef add_traverser_variables<data::variable_traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
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

  void apply(const process::action_label& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.name());
    static_cast<Derived&>(*this).apply(x.sorts());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.action_labels());
    static_cast<Derived&>(*this).apply(x.global_variables());
    static_cast<Derived&>(*this).apply(x.equations());
    static_cast<Derived&>(*this).apply(x.init());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_identifier& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.name());
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.identifier());
    static_cast<Derived&>(*this).apply(x.formal_parameters());
    static_cast<Derived&>(*this).apply(x.expression());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::rename_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.source());
    static_cast<Derived&>(*this).apply(x.target());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::communication_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.action_name());
    static_cast<Derived&>(*this).apply(x.name());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::action_name_multiset& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.names());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::untyped_multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.actions());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.label());
    static_cast<Derived&>(*this).apply(x.arguments());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_instance& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.identifier());
    static_cast<Derived&>(*this).apply(x.actual_parameters());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_instance_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.identifier());
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::delta& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::tau& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::sum& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::block& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.block_set());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::hide& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.hide_set());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::rename& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.rename_set());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::comm& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.comm_set());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::allow& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.allow_set());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::sync& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::at& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).apply(x.time_stamp());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::seq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::if_then& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.condition());
    static_cast<Derived&>(*this).apply(x.then_case());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::if_then_else& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.condition());
    static_cast<Derived&>(*this).apply(x.then_case());
    static_cast<Derived&>(*this).apply(x.else_case());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::bounded_init& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::left_merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::choice& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::stochastic_operator& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.distribution());
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::untyped_process_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.name());
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (process::is_action(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::action>(x));
    }
    else if (process::is_process_instance(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::process_instance>(x));
    }
    else if (process::is_process_instance_assignment(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::process_instance_assignment>(x));
    }
    else if (process::is_delta(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::delta>(x));
    }
    else if (process::is_tau(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::tau>(x));
    }
    else if (process::is_sum(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::sum>(x));
    }
    else if (process::is_block(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::block>(x));
    }
    else if (process::is_hide(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::hide>(x));
    }
    else if (process::is_rename(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::rename>(x));
    }
    else if (process::is_comm(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::comm>(x));
    }
    else if (process::is_allow(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::allow>(x));
    }
    else if (process::is_sync(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::sync>(x));
    }
    else if (process::is_at(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::at>(x));
    }
    else if (process::is_seq(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::seq>(x));
    }
    else if (process::is_if_then(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::if_then>(x));
    }
    else if (process::is_if_then_else(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::if_then_else>(x));
    }
    else if (process::is_bounded_init(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::bounded_init>(x));
    }
    else if (process::is_merge(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::merge>(x));
    }
    else if (process::is_left_merge(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::left_merge>(x));
    }
    else if (process::is_choice(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::choice>(x));
    }
    else if (process::is_stochastic_operator(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::stochastic_operator>(x));
    }
    else if (process::is_untyped_process_assignment(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::untyped_process_assignment>(x));
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
struct identifier_string_traverser: public add_traverser_identifier_strings<data::identifier_string_traverser, Derived>
{
  typedef add_traverser_identifier_strings<data::identifier_string_traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
};
//--- end generated add_traverser_identifier_strings code ---//

//--- start generated add_traverser_action_labels code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_action_labels: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const process::action_label& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.action_labels());
    static_cast<Derived&>(*this).apply(x.equations());
    static_cast<Derived&>(*this).apply(x.init());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.expression());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.label());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_instance& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_instance_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::delta& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::tau& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::sum& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::block& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::hide& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::rename& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::comm& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::allow& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::sync& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::at& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::seq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::if_then& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.then_case());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::if_then_else& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.then_case());
    static_cast<Derived&>(*this).apply(x.else_case());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::bounded_init& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::left_merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::choice& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::stochastic_operator& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::untyped_process_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const process::process_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (process::is_action(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::action>(x));
    }
    else if (process::is_process_instance(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::process_instance>(x));
    }
    else if (process::is_process_instance_assignment(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::process_instance_assignment>(x));
    }
    else if (process::is_delta(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::delta>(x));
    }
    else if (process::is_tau(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::tau>(x));
    }
    else if (process::is_sum(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::sum>(x));
    }
    else if (process::is_block(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::block>(x));
    }
    else if (process::is_hide(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::hide>(x));
    }
    else if (process::is_rename(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::rename>(x));
    }
    else if (process::is_comm(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::comm>(x));
    }
    else if (process::is_allow(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::allow>(x));
    }
    else if (process::is_sync(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::sync>(x));
    }
    else if (process::is_at(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::at>(x));
    }
    else if (process::is_seq(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::seq>(x));
    }
    else if (process::is_if_then(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::if_then>(x));
    }
    else if (process::is_if_then_else(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::if_then_else>(x));
    }
    else if (process::is_bounded_init(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::bounded_init>(x));
    }
    else if (process::is_merge(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::merge>(x));
    }
    else if (process::is_left_merge(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::left_merge>(x));
    }
    else if (process::is_choice(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::choice>(x));
    }
    else if (process::is_stochastic_operator(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::stochastic_operator>(x));
    }
    else if (process::is_untyped_process_assignment(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<process::untyped_process_assignment>(x));
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
struct action_label_traverser: public add_traverser_action_labels<core::traverser, Derived>
{
  typedef add_traverser_action_labels<core::traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
};
//--- end generated add_traverser_action_labels code ---//

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_TRAVERSER_H
