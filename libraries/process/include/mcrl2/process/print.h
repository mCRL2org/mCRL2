// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/print.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_PRINT_H
#define MCRL2_PROCESS_PRINT_H

#include "mcrl2/lps/print.h"
#include "mcrl2/process/normalize_sorts.h"
#include "mcrl2/process/traverser.h"
#include "mcrl2/process/detail/precedence.h"

namespace mcrl2 {

namespace process {

namespace detail
{

template <typename Derived>
struct printer: public process::add_traverser_sort_expressions<lps::detail::printer, Derived>
{
  typedef process::add_traverser_sort_expressions<lps::detail::printer, Derived> super;

  using super::enter;
  using super::leave;
  using super::operator();
  using core::detail::printer<Derived>::print_sorts;
  using super::print_action_declarations;
  using super::print_assignments;
  using super::print_condition;
  using super::print_data_expression;
  using super::print_list;
  using super::print_time;
  using super::print_variables;

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  void print_initial_state(const process_expression& init)
  {
    derived().print("init ");
    derived()(init);
    derived().print(";\n");
  }

  void print_process_expression(const process_expression& x, int precedence)
  {
    bool print_parens = process::detail::precedence(x) < precedence;
    if (print_parens)
    {
      derived().print("(");
    }
    derived()(x);
    if (print_parens)
    {
      derived().print(")");
    }
  }

  template <typename T>
  void print_binary_process_operation(const T& x, const std::string& op)
  {
    print_process_expression(x.left(), process::detail::precedence(x));
    derived().print(op);
    print_process_expression(x.right(), process::detail::precedence(x));
  }

  void operator()(const process::process_specification& x)
  {
    derived().enter(x);
    derived()(x.data());
    print_action_declarations(x.action_labels(), "act  ",";\n\n", ";\n     ");
    print_variables(x.global_variables(), true, true, "glob ", ";\n\n", ";\n     ");

    // N.B. We have to normalize the sorts of the equations. Otherwise predicates like
    // is_list(x) may return the wrong result.
    atermpp::vector<process_equation> normalized_equations = x.equations();
    process::normalize_sorts(normalized_equations, x.data());   
    print_list(normalized_equations, "proc ", "\n\n", "\n     ");

    print_initial_state(x.init());
    derived().leave(x);
  }

  void operator()(const process::process_identifier& x)
  {
    derived().enter(x);
    derived()(x.sorts());
    derived().leave(x);
  }

  void operator()(const process::process_equation& x)
  {
    derived().enter(x);
    derived()(x.identifier().name());
    print_variables(x.formal_parameters());
    derived().print(" = ");
    derived()(x.expression());
    derived().print(";");
    derived().leave(x);
  }

  void operator()(const process::process_instance& x)
  {
    derived().enter(x);
    derived()(x.identifier().name());
    print_variables(x.actual_parameters(), false);
    derived().leave(x);
  }

  void operator()(const process::process_instance_assignment& x)
  {
    derived().enter(x);
    derived()(x.identifier().name());
    print_assignments(x.assignments(), false);
    derived().leave(x);
  }

  void operator()(const process::delta& x)
  {
    derived().enter(x);
    derived().print("delta");
    derived().leave(x);
  }

  void operator()(const process::tau& x)
  {
    derived().enter(x);
    derived().print("tau");
    derived().leave(x);
  }

  void operator()(const process::sum& x)
  {
    derived().enter(x);
    derived().print("sum ");
    print_variables(x.bound_variables(), true, true, "", "");
    derived().print(". ");   
    print_process_expression(x.operand(), process::detail::precedence(x));
    derived().leave(x);
  }

  void operator()(const process::block& x)
  {
    derived().enter(x);
    derived().print("block(");
    print_list(x.block_set(), "{", "}, ", ", ");
    derived()(x.operand());
    derived().print(")");
    derived().leave(x);
  }

  void operator()(const process::hide& x)
  {
    derived().enter(x);
    derived().print("hide(");
    print_list(x.hide_set(), "{", "}, ", ", ");
    derived()(x.operand());
    derived().print(")");
    derived().leave(x);
  }

  void operator()(const process::rename_expression& x)
  {
    derived().enter(x);
    derived()(x.source());
    derived().print(" -> ");
    derived()(x.target());
    derived().leave(x);
  }

  void operator()(const process::rename& x)
  {
    derived().enter(x);
    derived().print("rename(");
    print_list(x.rename_set(), "{", "}, ", ", ");
    derived()(x.operand());
    derived().print(")");
    derived().leave(x);
  }

  void operator()(const process::action_name_multiset& x)
  {
    derived().enter(x);
    print_list(x.names(), "", "", " | ");
    derived().leave(x);
  }

  void operator()(const process::communication_expression& x)
  {
    derived().enter(x);
    derived()(x.action_name());
    derived().print(" -> ");
    derived()(x.name());
    derived().leave(x);
  }

  void operator()(const process::comm& x)
  {
    derived().enter(x);
    derived().print("comm(");
    print_list(x.comm_set(), "{", "}, ", ", ");
    derived()(x.operand());
    derived().print(")");
    derived().leave(x);
  }

  void operator()(const process::allow& x)
  {
    derived().enter(x);
    derived().print("allow(");
    print_list(x.allow_set(), "{", "}, ", ", ");
    derived()(x.operand());
    derived().print(")");
    derived().leave(x);
  }

  void operator()(const process::sync& x)
  {
    derived().enter(x);
    print_binary_process_operation(x, " | ");
    derived().leave(x);
  }

  void operator()(const process::at& x)
  {
    derived().enter(x);
    derived()(x.operand());
    derived().print(" @ ");
    print_time(x.time_stamp());
    derived().leave(x);
  }

  void operator()(const process::seq& x)
  {
    derived().enter(x);
    print_binary_process_operation(x, " . ");
    derived().leave(x);
  }

  // TODO: find out why precedences are hard coded here
  void operator()(const process::if_then& x)
  {
    derived().enter(x);
    print_condition(x.condition(), " -> ", data::detail::prefix_precedence());
    print_process_expression(x.then_case(), 5);
    derived().leave(x);
  }

  // TODO: find out why precedences are hard coded here
  void operator()(const process::if_then_else& x)
  {
    derived().enter(x);
    print_condition(x.condition(), " -> ", data::detail::prefix_precedence());
    print_process_expression(x.then_case(), 5);
    derived().print(" <> ");
    // N.B. the else case is printed with a lower precedence, since we want the expression a -> b -> c <> d <> e
    // to be printed as a -> (b -> c <> d) <> e
    print_process_expression(x.else_case(), 5);
    derived().leave(x);
  }

  void operator()(const process::bounded_init& x)
  {
    derived().enter(x);
    print_binary_process_operation(x, " << ");
    derived().leave(x);
  }

  void operator()(const process::merge& x)
  {
    derived().enter(x);
    print_binary_process_operation(x, " || ");
    derived().leave(x);
  }

  void operator()(const process::left_merge& x)
  {
    derived().enter(x);
    print_binary_process_operation(x, " ||_ ");
    derived().leave(x);
  }

  void operator()(const process::choice& x)
  {
    derived().enter(x);
    print_binary_process_operation(x, " + ");
    derived().leave(x);
  }

};

} // namespace detail

/// \brief Prints the object t to a stream.
template <typename T>
void print(const T& t, std::ostream& out)
{
  core::detail::apply_printer<process::detail::printer> printer(out);
  printer(t);
}

/// \brief Returns a string representation of the object t.
template <typename T>
std::string print(const T& t)
{
  std::ostringstream out;
  process::print(t, out);
  return out.str();
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_PRINT_H
