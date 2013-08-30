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
#include "mcrl2/process/traverser.h"

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
  using super::print_action_declarations;
  using super::print_assignments;
  using super::print_condition;
  using super::print_list;
  using super::print_variables;
  using super::print_expression;
  using super::print_binary_operation;

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

  void print_if_then_condition(const data::data_expression& condition, const std::string& arrow = "  ->  ", int precedence = max_precedence)
  {
    print_expression(condition, precedence);
    derived().print(arrow);
  }

  void operator()(const process::process_specification& x)
  {
    derived().enter(x);
    derived()(x.data());
    print_action_declarations(x.action_labels(), "act  ",";\n\n", ";\n     ");
    print_variables(x.global_variables(), true, true, true, "glob ", ";\n\n", ";\n     ");

    // N.B. We have to normalize the sorts of the equations. Otherwise predicates like
    // is_list(x) may return the wrong result.
    std::vector<process_equation> normalized_equations = x.equations();
    process::normalize_sorts(normalized_equations, x.data());
    print_list(normalized_equations, "proc ", "\n\n", "\n     ");

    print_initial_state(x.init());
    derived().leave(x);
  }

  void operator()(const process::process_identifier& x)
  {
    derived().enter(x);
    derived()(x.name());
    derived().leave(x);
  }

  void operator()(const process::process_equation& x)
  {
    derived().enter(x);
    derived()(x.identifier().name());
    print_variables(x.formal_parameters(), true, true, false);
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
    derived().print("(");
    print_assignments(x.assignments(), true, "", "");
    derived().print(")");
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
    print_variables(x.bound_variables(), true, true, false, "", "");
    derived().print(". ");
    print_expression(x.operand(), precedence(x));
    derived().leave(x);
  }

  void operator()(const process::block& x)
  {
    derived().enter(x);
    derived().print("block(");
    print_list(x.block_set(), "{", "}, ", ", ", true);
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
    if (!data::is_nil(x.name()))
    {
      derived().print(" -> ");
      derived()(x.name());
    }
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
    print_list(x.allow_set(), "{", "}, ", ", ", true);
    derived()(x.operand());
    derived().print(")");
    derived().leave(x);
  }

  void operator()(const process::sync& x)
  {
    derived().enter(x);
    print_binary_operation(x, " | ");
    derived().leave(x);
  }

  void operator()(const process::at& x)
  {
    derived().enter(x);
    derived()(x.operand());
    derived().print(" @ ");
    print_expression(x.time_stamp(), max_precedence);
    derived().leave(x);
  }

  void operator()(const process::seq& x)
  {
    derived().enter(x);
    print_binary_operation(x, " . ");
    derived().leave(x);
  }

  // TODO: find out why precedences are hard coded here
  void operator()(const process::if_then& x)
  {
    derived().enter(x);
    print_if_then_condition(x.condition(), " -> ", max_precedence);
    print_expression(x.then_case());
    derived().leave(x);
  }

  // TODO: find out why precedences are hard coded here
  void operator()(const process::if_then_else& x)
  {
    derived().enter(x);
    print_if_then_condition(x.condition(), " -> ", max_precedence);
    // N.B. the then case is printed with a higher precedence, since we want the expression a -> b -> c <> d <> e
    // to be printed as a -> (b -> c <> d) <> e
    print_expression(x.then_case(), precedence(x) + 1);
    derived().print(" <> ");
    print_expression(x.else_case());
    derived().leave(x);
  }

  void operator()(const process::bounded_init& x)
  {
    derived().enter(x);
    print_binary_operation(x, " << ");
    derived().leave(x);
  }

  void operator()(const process::merge& x)
  {
    derived().enter(x);
    print_binary_operation(x, " || ");
    derived().leave(x);
  }

  void operator()(const process::left_merge& x)
  {
    derived().enter(x);
    print_binary_operation(x, " ||_ ");
    derived().leave(x);
  }

  void operator()(const process::choice& x)
  {
    derived().enter(x);
    print_binary_operation(x, " + ");
    derived().leave(x);
  }

  void operator()(const process::untyped_process_assignment& x)
  {
    derived().enter(x);
    derived()(x.name());
    print_assignments(x.assignments(), false);
    derived().leave(x);
  }

  void operator()(const process::untyped_parameter_identifier& x)
  {
    derived().enter(x);
    derived()(x.name());
    print_list(x.arguments(), "(", ")", ", ");
    derived().leave(x);
  }
};

} // namespace detail

/// \brief Prints the object x to a stream.
struct stream_printer
{
  template <typename T>
  void operator()(const T& x, std::ostream& out)
  {
    core::detail::apply_printer<process::detail::printer> printer(out);
    printer(x);
  }
};

/// \brief Returns a string representation of the object x.
template <typename T>
std::string pp(const T& x)
{
  std::ostringstream out;
  stream_printer()(x, out);
  return out.str();
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_PRINT_H
