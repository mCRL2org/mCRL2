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

#include "mcrl2/lps/traverser.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/process/process_specification.h"

namespace mcrl2
{

namespace process
{

/// \brief Traversal class for actions. Used as a base class for process_expression_traverser.
template <typename Derived>
struct process_expression_traverser_base: public core::traverser<Derived>
{
  typedef core::traverser<Derived> super;
  using super::operator();
  using super::enter;
  using super::leave;

  void operator()(const lps::action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }
};

//--- start generated add_traverser_sort_expressions code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_sort_expressions: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

  void operator()(const process::process_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.action_labels());
    static_cast<Derived&>(*this)(x.init());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::process_identifier& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.sorts());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::process_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.identifier());
    static_cast<Derived&>(*this)(x.formal_parameters());
    static_cast<Derived&>(*this)(x.expression());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::process_instance& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.identifier());
    static_cast<Derived&>(*this)(x.actual_parameters());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::process_instance_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.identifier());
    static_cast<Derived&>(*this)(x.assignments());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::delta& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::tau& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::sum& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.bound_variables());
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::block& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::hide& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::rename& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::comm& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::allow& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::sync& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::at& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this)(x.time_stamp());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::seq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::if_then& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.condition());
    static_cast<Derived&>(*this)(x.then_case());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::if_then_else& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.condition());
    static_cast<Derived&>(*this)(x.then_case());
    static_cast<Derived&>(*this)(x.else_case());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::bounded_init& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::left_merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::choice& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::process_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result;
    if (process::is_process_instance(x))
    {
      static_cast<Derived&>(*this)(process::process_instance(atermpp::aterm_appl(x)));
    }
    else if (process::is_process_instance_assignment(x))
    {
      static_cast<Derived&>(*this)(process::process_instance_assignment(atermpp::aterm_appl(x)));
    }
    else if (process::is_delta(x))
    {
      static_cast<Derived&>(*this)(process::delta(atermpp::aterm_appl(x)));
    }
    else if (process::is_tau(x))
    {
      static_cast<Derived&>(*this)(process::tau(atermpp::aterm_appl(x)));
    }
    else if (process::is_sum(x))
    {
      static_cast<Derived&>(*this)(process::sum(atermpp::aterm_appl(x)));
    }
    else if (process::is_block(x))
    {
      static_cast<Derived&>(*this)(process::block(atermpp::aterm_appl(x)));
    }
    else if (process::is_hide(x))
    {
      static_cast<Derived&>(*this)(process::hide(atermpp::aterm_appl(x)));
    }
    else if (process::is_rename(x))
    {
      static_cast<Derived&>(*this)(process::rename(atermpp::aterm_appl(x)));
    }
    else if (process::is_comm(x))
    {
      static_cast<Derived&>(*this)(process::comm(atermpp::aterm_appl(x)));
    }
    else if (process::is_allow(x))
    {
      static_cast<Derived&>(*this)(process::allow(atermpp::aterm_appl(x)));
    }
    else if (process::is_sync(x))
    {
      static_cast<Derived&>(*this)(process::sync(atermpp::aterm_appl(x)));
    }
    else if (process::is_at(x))
    {
      static_cast<Derived&>(*this)(process::at(atermpp::aterm_appl(x)));
    }
    else if (process::is_seq(x))
    {
      static_cast<Derived&>(*this)(process::seq(atermpp::aterm_appl(x)));
    }
    else if (process::is_if_then(x))
    {
      static_cast<Derived&>(*this)(process::if_then(atermpp::aterm_appl(x)));
    }
    else if (process::is_if_then_else(x))
    {
      static_cast<Derived&>(*this)(process::if_then_else(atermpp::aterm_appl(x)));
    }
    else if (process::is_bounded_init(x))
    {
      static_cast<Derived&>(*this)(process::bounded_init(atermpp::aterm_appl(x)));
    }
    else if (process::is_merge(x))
    {
      static_cast<Derived&>(*this)(process::merge(atermpp::aterm_appl(x)));
    }
    else if (process::is_left_merge(x))
    {
      static_cast<Derived&>(*this)(process::left_merge(atermpp::aterm_appl(x)));
    }
    else if (process::is_choice(x))
    {
      static_cast<Derived&>(*this)(process::choice(atermpp::aterm_appl(x)));
    }
    else if (lps::is_action(x))
    {
      static_cast<Derived&>(*this)(lps::action(atermpp::aterm_appl(x)));
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
  using super::operator();
};
//--- end generated add_traverser_sort_expressions code ---//

//--- start generated add_traverser_data_expressions code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_data_expressions: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

  void operator()(const process::process_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.init());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::process_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.expression());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::process_instance& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.actual_parameters());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::process_instance_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.assignments());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::delta& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::tau& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::sum& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::block& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::hide& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::rename& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::comm& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::allow& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::sync& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::at& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this)(x.time_stamp());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::seq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::if_then& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.condition());
    static_cast<Derived&>(*this)(x.then_case());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::if_then_else& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.condition());
    static_cast<Derived&>(*this)(x.then_case());
    static_cast<Derived&>(*this)(x.else_case());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::bounded_init& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::left_merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::choice& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::process_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result;
    if (process::is_process_instance(x))
    {
      static_cast<Derived&>(*this)(process::process_instance(atermpp::aterm_appl(x)));
    }
    else if (process::is_process_instance_assignment(x))
    {
      static_cast<Derived&>(*this)(process::process_instance_assignment(atermpp::aterm_appl(x)));
    }
    else if (process::is_delta(x))
    {
      static_cast<Derived&>(*this)(process::delta(atermpp::aterm_appl(x)));
    }
    else if (process::is_tau(x))
    {
      static_cast<Derived&>(*this)(process::tau(atermpp::aterm_appl(x)));
    }
    else if (process::is_sum(x))
    {
      static_cast<Derived&>(*this)(process::sum(atermpp::aterm_appl(x)));
    }
    else if (process::is_block(x))
    {
      static_cast<Derived&>(*this)(process::block(atermpp::aterm_appl(x)));
    }
    else if (process::is_hide(x))
    {
      static_cast<Derived&>(*this)(process::hide(atermpp::aterm_appl(x)));
    }
    else if (process::is_rename(x))
    {
      static_cast<Derived&>(*this)(process::rename(atermpp::aterm_appl(x)));
    }
    else if (process::is_comm(x))
    {
      static_cast<Derived&>(*this)(process::comm(atermpp::aterm_appl(x)));
    }
    else if (process::is_allow(x))
    {
      static_cast<Derived&>(*this)(process::allow(atermpp::aterm_appl(x)));
    }
    else if (process::is_sync(x))
    {
      static_cast<Derived&>(*this)(process::sync(atermpp::aterm_appl(x)));
    }
    else if (process::is_at(x))
    {
      static_cast<Derived&>(*this)(process::at(atermpp::aterm_appl(x)));
    }
    else if (process::is_seq(x))
    {
      static_cast<Derived&>(*this)(process::seq(atermpp::aterm_appl(x)));
    }
    else if (process::is_if_then(x))
    {
      static_cast<Derived&>(*this)(process::if_then(atermpp::aterm_appl(x)));
    }
    else if (process::is_if_then_else(x))
    {
      static_cast<Derived&>(*this)(process::if_then_else(atermpp::aterm_appl(x)));
    }
    else if (process::is_bounded_init(x))
    {
      static_cast<Derived&>(*this)(process::bounded_init(atermpp::aterm_appl(x)));
    }
    else if (process::is_merge(x))
    {
      static_cast<Derived&>(*this)(process::merge(atermpp::aterm_appl(x)));
    }
    else if (process::is_left_merge(x))
    {
      static_cast<Derived&>(*this)(process::left_merge(atermpp::aterm_appl(x)));
    }
    else if (process::is_choice(x))
    {
      static_cast<Derived&>(*this)(process::choice(atermpp::aterm_appl(x)));
    }
    else if (lps::is_action(x))
    {
      static_cast<Derived&>(*this)(lps::action(atermpp::aterm_appl(x)));
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
  using super::operator();
};
//--- end generated add_traverser_data_expressions code ---//

//--- start generated add_traverser_process_expressions code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_process_expressions: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

  void operator()(const process::process_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.init());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::process_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.expression());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::process_instance& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::process_instance_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::delta& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::tau& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::sum& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::block& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::hide& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::rename& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::comm& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::allow& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::sync& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::at& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::seq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::if_then& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.then_case());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::if_then_else& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.then_case());
    static_cast<Derived&>(*this)(x.else_case());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::bounded_init& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::left_merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::choice& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::process_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result;
    if (process::is_process_instance(x))
    {
      static_cast<Derived&>(*this)(process::process_instance(atermpp::aterm_appl(x)));
    }
    else if (process::is_process_instance_assignment(x))
    {
      static_cast<Derived&>(*this)(process::process_instance_assignment(atermpp::aterm_appl(x)));
    }
    else if (process::is_delta(x))
    {
      static_cast<Derived&>(*this)(process::delta(atermpp::aterm_appl(x)));
    }
    else if (process::is_tau(x))
    {
      static_cast<Derived&>(*this)(process::tau(atermpp::aterm_appl(x)));
    }
    else if (process::is_sum(x))
    {
      static_cast<Derived&>(*this)(process::sum(atermpp::aterm_appl(x)));
    }
    else if (process::is_block(x))
    {
      static_cast<Derived&>(*this)(process::block(atermpp::aterm_appl(x)));
    }
    else if (process::is_hide(x))
    {
      static_cast<Derived&>(*this)(process::hide(atermpp::aterm_appl(x)));
    }
    else if (process::is_rename(x))
    {
      static_cast<Derived&>(*this)(process::rename(atermpp::aterm_appl(x)));
    }
    else if (process::is_comm(x))
    {
      static_cast<Derived&>(*this)(process::comm(atermpp::aterm_appl(x)));
    }
    else if (process::is_allow(x))
    {
      static_cast<Derived&>(*this)(process::allow(atermpp::aterm_appl(x)));
    }
    else if (process::is_sync(x))
    {
      static_cast<Derived&>(*this)(process::sync(atermpp::aterm_appl(x)));
    }
    else if (process::is_at(x))
    {
      static_cast<Derived&>(*this)(process::at(atermpp::aterm_appl(x)));
    }
    else if (process::is_seq(x))
    {
      static_cast<Derived&>(*this)(process::seq(atermpp::aterm_appl(x)));
    }
    else if (process::is_if_then(x))
    {
      static_cast<Derived&>(*this)(process::if_then(atermpp::aterm_appl(x)));
    }
    else if (process::is_if_then_else(x))
    {
      static_cast<Derived&>(*this)(process::if_then_else(atermpp::aterm_appl(x)));
    }
    else if (process::is_bounded_init(x))
    {
      static_cast<Derived&>(*this)(process::bounded_init(atermpp::aterm_appl(x)));
    }
    else if (process::is_merge(x))
    {
      static_cast<Derived&>(*this)(process::merge(atermpp::aterm_appl(x)));
    }
    else if (process::is_left_merge(x))
    {
      static_cast<Derived&>(*this)(process::left_merge(atermpp::aterm_appl(x)));
    }
    else if (process::is_choice(x))
    {
      static_cast<Derived&>(*this)(process::choice(atermpp::aterm_appl(x)));
    }
    else if (lps::is_action(x))
    {
      static_cast<Derived&>(*this)(lps::action(atermpp::aterm_appl(x)));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct process_expression_traverser: public add_traverser_process_expressions<process::process_expression_traverser_base, Derived>
{
  typedef add_traverser_process_expressions<process::process_expression_traverser_base, Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
};
//--- end generated add_traverser_process_expressions code ---//

//--- start generated add_traverser_variables code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_variables: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

  void operator()(const process::process_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.init());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::process_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.formal_parameters());
    static_cast<Derived&>(*this)(x.expression());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::process_instance& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.actual_parameters());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::process_instance_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.assignments());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::delta& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::tau& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::sum& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.bound_variables());
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::block& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::hide& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::rename& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::comm& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::allow& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::sync& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::at& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this)(x.time_stamp());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::seq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::if_then& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.condition());
    static_cast<Derived&>(*this)(x.then_case());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::if_then_else& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.condition());
    static_cast<Derived&>(*this)(x.then_case());
    static_cast<Derived&>(*this)(x.else_case());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::bounded_init& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::left_merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::choice& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::process_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result;
    if (process::is_process_instance(x))
    {
      static_cast<Derived&>(*this)(process::process_instance(atermpp::aterm_appl(x)));
    }
    else if (process::is_process_instance_assignment(x))
    {
      static_cast<Derived&>(*this)(process::process_instance_assignment(atermpp::aterm_appl(x)));
    }
    else if (process::is_delta(x))
    {
      static_cast<Derived&>(*this)(process::delta(atermpp::aterm_appl(x)));
    }
    else if (process::is_tau(x))
    {
      static_cast<Derived&>(*this)(process::tau(atermpp::aterm_appl(x)));
    }
    else if (process::is_sum(x))
    {
      static_cast<Derived&>(*this)(process::sum(atermpp::aterm_appl(x)));
    }
    else if (process::is_block(x))
    {
      static_cast<Derived&>(*this)(process::block(atermpp::aterm_appl(x)));
    }
    else if (process::is_hide(x))
    {
      static_cast<Derived&>(*this)(process::hide(atermpp::aterm_appl(x)));
    }
    else if (process::is_rename(x))
    {
      static_cast<Derived&>(*this)(process::rename(atermpp::aterm_appl(x)));
    }
    else if (process::is_comm(x))
    {
      static_cast<Derived&>(*this)(process::comm(atermpp::aterm_appl(x)));
    }
    else if (process::is_allow(x))
    {
      static_cast<Derived&>(*this)(process::allow(atermpp::aterm_appl(x)));
    }
    else if (process::is_sync(x))
    {
      static_cast<Derived&>(*this)(process::sync(atermpp::aterm_appl(x)));
    }
    else if (process::is_at(x))
    {
      static_cast<Derived&>(*this)(process::at(atermpp::aterm_appl(x)));
    }
    else if (process::is_seq(x))
    {
      static_cast<Derived&>(*this)(process::seq(atermpp::aterm_appl(x)));
    }
    else if (process::is_if_then(x))
    {
      static_cast<Derived&>(*this)(process::if_then(atermpp::aterm_appl(x)));
    }
    else if (process::is_if_then_else(x))
    {
      static_cast<Derived&>(*this)(process::if_then_else(atermpp::aterm_appl(x)));
    }
    else if (process::is_bounded_init(x))
    {
      static_cast<Derived&>(*this)(process::bounded_init(atermpp::aterm_appl(x)));
    }
    else if (process::is_merge(x))
    {
      static_cast<Derived&>(*this)(process::merge(atermpp::aterm_appl(x)));
    }
    else if (process::is_left_merge(x))
    {
      static_cast<Derived&>(*this)(process::left_merge(atermpp::aterm_appl(x)));
    }
    else if (process::is_choice(x))
    {
      static_cast<Derived&>(*this)(process::choice(atermpp::aterm_appl(x)));
    }
    else if (lps::is_action(x))
    {
      static_cast<Derived&>(*this)(lps::action(atermpp::aterm_appl(x)));
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
  using super::operator();
};
//--- end generated add_traverser_variables code ---//

//--- start generated add_traverser_identifier_strings code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_identifier_strings: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

  void operator()(const process::process_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.action_labels());
    static_cast<Derived&>(*this)(x.init());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::process_identifier& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.name());
    static_cast<Derived&>(*this)(x.sorts());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::process_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.identifier());
    static_cast<Derived&>(*this)(x.formal_parameters());
    static_cast<Derived&>(*this)(x.expression());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::rename_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.source());
    static_cast<Derived&>(*this)(x.target());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::communication_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.action_name());
    static_cast<Derived&>(*this)(x.name());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::action_name_multiset& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.names());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::process_instance& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.identifier());
    static_cast<Derived&>(*this)(x.actual_parameters());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::process_instance_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.identifier());
    static_cast<Derived&>(*this)(x.assignments());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::delta& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::tau& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::sum& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.bound_variables());
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::block& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.block_set());
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::hide& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.hide_set());
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::rename& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.rename_set());
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::comm& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.comm_set());
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::allow& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.allow_set());
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::sync& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::at& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this)(x.time_stamp());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::seq& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::if_then& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.condition());
    static_cast<Derived&>(*this)(x.then_case());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::if_then_else& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.condition());
    static_cast<Derived&>(*this)(x.then_case());
    static_cast<Derived&>(*this)(x.else_case());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::bounded_init& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::left_merge& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::choice& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const process::process_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    process::process_expression result;
    if (process::is_process_instance(x))
    {
      static_cast<Derived&>(*this)(process::process_instance(atermpp::aterm_appl(x)));
    }
    else if (process::is_process_instance_assignment(x))
    {
      static_cast<Derived&>(*this)(process::process_instance_assignment(atermpp::aterm_appl(x)));
    }
    else if (process::is_delta(x))
    {
      static_cast<Derived&>(*this)(process::delta(atermpp::aterm_appl(x)));
    }
    else if (process::is_tau(x))
    {
      static_cast<Derived&>(*this)(process::tau(atermpp::aterm_appl(x)));
    }
    else if (process::is_sum(x))
    {
      static_cast<Derived&>(*this)(process::sum(atermpp::aterm_appl(x)));
    }
    else if (process::is_block(x))
    {
      static_cast<Derived&>(*this)(process::block(atermpp::aterm_appl(x)));
    }
    else if (process::is_hide(x))
    {
      static_cast<Derived&>(*this)(process::hide(atermpp::aterm_appl(x)));
    }
    else if (process::is_rename(x))
    {
      static_cast<Derived&>(*this)(process::rename(atermpp::aterm_appl(x)));
    }
    else if (process::is_comm(x))
    {
      static_cast<Derived&>(*this)(process::comm(atermpp::aterm_appl(x)));
    }
    else if (process::is_allow(x))
    {
      static_cast<Derived&>(*this)(process::allow(atermpp::aterm_appl(x)));
    }
    else if (process::is_sync(x))
    {
      static_cast<Derived&>(*this)(process::sync(atermpp::aterm_appl(x)));
    }
    else if (process::is_at(x))
    {
      static_cast<Derived&>(*this)(process::at(atermpp::aterm_appl(x)));
    }
    else if (process::is_seq(x))
    {
      static_cast<Derived&>(*this)(process::seq(atermpp::aterm_appl(x)));
    }
    else if (process::is_if_then(x))
    {
      static_cast<Derived&>(*this)(process::if_then(atermpp::aterm_appl(x)));
    }
    else if (process::is_if_then_else(x))
    {
      static_cast<Derived&>(*this)(process::if_then_else(atermpp::aterm_appl(x)));
    }
    else if (process::is_bounded_init(x))
    {
      static_cast<Derived&>(*this)(process::bounded_init(atermpp::aterm_appl(x)));
    }
    else if (process::is_merge(x))
    {
      static_cast<Derived&>(*this)(process::merge(atermpp::aterm_appl(x)));
    }
    else if (process::is_left_merge(x))
    {
      static_cast<Derived&>(*this)(process::left_merge(atermpp::aterm_appl(x)));
    }
    else if (process::is_choice(x))
    {
      static_cast<Derived&>(*this)(process::choice(atermpp::aterm_appl(x)));
    }
    else if (lps::is_action(x))
    {
      static_cast<Derived&>(*this)(lps::action(atermpp::aterm_appl(x)));
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
  using super::operator();
};
//--- end generated add_traverser_identifier_strings code ---//

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_TRAVERSER_H
