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

#include "mcrl2/data/print.h"
#include "mcrl2/process/traverser.h"

namespace mcrl2 {

namespace process {

namespace detail
{

template <typename Derived>
struct printer: public process::add_traverser_sort_expressions<data::detail::printer, Derived>
{
  typedef process::add_traverser_sort_expressions<data::detail::printer, Derived> super;

  using super::enter;
  using super::leave;
  using super::apply;
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
    derived().apply(init);
    derived().print(";\n");
  }

  void print_if_then_condition(const data::data_expression& condition, const std::string& arrow = "  ->  ", int precedence = max_precedence)
  {
    print_expression(condition, precedence);
    derived().print(arrow);
  }

  // Container contains elements of type T such that t.sort() is a sort_expression.
  template <typename Container>
  void print_action_declarations(const Container& container,
                                 const std::string& opener = "(",
                                 const std::string& closer = ")",
                                 const std::string& separator = ", "
                                )
  {
    // print nothing if the container is empty
    if (container.empty())
    {
      return;
    }

    auto first = container.begin();
    auto last = container.end();

    derived().print(opener);

    while (first != last)
    {
      if (first != container.begin())
      {
        derived().print(separator);
      }

      typename Container::const_iterator i = first;
      do
      {
        ++i;
      }
      while (i != last && first->sorts() == i->sorts());

      print_list(std::vector<process::action_label>(first, i), "", "", ",");
      if (!first->sorts().empty())
      {
        derived().print(": ");
        print_list(first->sorts(), "", "", " # ");
      }

      first = i;
    }
    derived().print(closer);
  }

  // Container contains elements of type T such that t.sort() is a sort_expression.
  template <typename Container>
  void print_action_declarations_maximally_shared(const Container& container,
                                                  const std::string& opener = "(",
                                                  const std::string& closer = ")",
                                                  const std::string& separator = ", "
                                                 )
  {
    typedef typename Container::value_type T;

    // print nothing if the container is empty
    if (container.empty())
    {
      return;
    }

    // sort_map[s] will contain all elements t of container with t.sorts() == s.
    std::map<data::sort_expression_list, std::vector<T> > sort_map;

    // sort_lists will contain all sort expression lists s that appear as a key in sort_map,
    // in the order they are encountered in container
    std::vector<data::sort_expression_list> sort_lists;

    for (auto i = container.begin(); i != container.end(); ++i)
    {
      if (sort_map.find(i->sorts()) == sort_map.end())
      {
        sort_lists.push_back(i->sorts());
      }
      sort_map[i->sorts()].push_back(*i);
    }

    // do the actual printing
    derived().print(opener);
    for (auto i = sort_lists.begin(); i != sort_lists.end(); ++i)
    {
      if (i != sort_lists.begin())
      {
        derived().print(separator);
      }
      const std::vector<T>& v = sort_map[*i];
      print_list(v, "", "", ",");
      if (!i->empty())
      {
        derived().print(": ");
        print_list(*i, "", "", " # ");
      }
    }
    derived().print(closer);
  }

  void apply(const process::action_label& x)
  {
    derived().enter(x);
    derived().apply(x.name());
    derived().leave(x);
  }

  void apply(const process::action& x)
  {
    derived().enter(x);
    derived().apply(x.label());
    print_list(x.arguments(), "(", ")", ", ");
    derived().leave(x);
  }

  void apply(const process::untyped_action& x)
  {
    derived().enter(x);
    derived().apply(x.name());
    print_list(x.arguments(), "(", ")", ", ");
    derived().leave(x);
  }

  void apply(const process::process_specification& x)
  {
    derived().enter(x);
    derived().apply(x.data());
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

  void apply(const process::process_identifier& x)
  {
    derived().enter(x);
    derived().apply(x.name());
    derived().leave(x);
  }

  void apply(const process::process_equation& x)
  {
    derived().enter(x);
    derived().apply(x.identifier().name());
    print_variables(x.formal_parameters(), true, true, false);
    derived().print(" = ");
    derived().apply(x.expression());
    derived().print(";");
    derived().leave(x);
  }

  void apply(const process::process_instance& x)
  {
    derived().enter(x);
    derived().apply(x.identifier().name());
    print_variables(x.actual_parameters(), false);
    derived().leave(x);
  }

  void apply(const process::process_instance_assignment& x)
  {
    derived().enter(x);
    derived().apply(x.identifier().name());
    derived().print("(");
    print_assignments(x.assignments(), true, "", "");
    derived().print(")");
    derived().leave(x);
  }

  void apply(const process::delta& x)
  {
    derived().enter(x);
    derived().print("delta");
    derived().leave(x);
  }

  void apply(const process::tau& x)
  {
    derived().enter(x);
    derived().print("tau");
    derived().leave(x);
  }

  void apply(const process::sum& x)
  {
    derived().enter(x);
    derived().print("sum ");
    print_variables(x.variables(), true, true, false, "", "");
    derived().print(". ");
    print_expression(x.operand(), left_precedence(x));
    derived().leave(x);
  }

  void apply(const process::stochastic_operator& x)
  {
    derived().enter(x);
    derived().print("dist ");
    print_variables(x.variables(), true, true, false, "", "");
    derived().print("[");
    derived().apply(x.distribution());
    derived().print("] . ");
    print_expression(x.operand(), left_precedence(x));
    derived().leave(x);
  }

  void apply(const process::block& x)
  {
    derived().enter(x);
    derived().print("block(");
    print_list(x.block_set(), "{", "}, ", ", ", true);
    derived().apply(x.operand());
    derived().print(")");
    derived().leave(x);
  }

  void apply(const process::hide& x)
  {
    derived().enter(x);
    derived().print("hide(");
    print_list(x.hide_set(), "{", "}, ", ", ");
    derived().apply(x.operand());
    derived().print(")");
    derived().leave(x);
  }

  void apply(const process::rename_expression& x)
  {
    derived().enter(x);
    derived().apply(x.source());
    derived().print(" -> ");
    derived().apply(x.target());
    derived().leave(x);
  }

  void apply(const process::rename& x)
  {
    derived().enter(x);
    derived().print("rename(");
    print_list(x.rename_set(), "{", "}, ", ", ");
    derived().apply(x.operand());
    derived().print(")");
    derived().leave(x);
  }

  void apply(const process::action_name_multiset& x)
  {
    derived().enter(x);
    print_list(x.names(), "", "", " | ");
    derived().leave(x);
  }

  void apply(const process::communication_expression& x)
  {
    derived().enter(x);
    derived().apply(x.action_name());
    if (!core::is_nil(x.name()))
    {
      derived().print(" -> ");
      derived().apply(x.name());
    }
    derived().leave(x);
  }

  void apply(const process::comm& x)
  {
    derived().enter(x);
    derived().print("comm(");
    print_list(x.comm_set(), "{", "}, ", ", ");
    derived().apply(x.operand());
    derived().print(")");
    derived().leave(x);
  }

  void apply(const process::allow& x)
  {
    derived().enter(x);
    derived().print("allow(");
    print_list(x.allow_set(), "{", "}, ", ", ", true);
    derived().apply(x.operand());
    derived().print(")");
    derived().leave(x);
  }

  void apply(const process::sync& x)
  {
    derived().enter(x);
    print_binary_operation(x, " | ");
    derived().leave(x);
  }

  void apply(const process::at& x)
  {
    derived().enter(x);
    derived().apply(x.operand());
    derived().print(" @ ");
    print_expression(x.time_stamp(), max_precedence);
    derived().leave(x);
  }

  void apply(const process::seq& x)
  {
    derived().enter(x);
    print_binary_operation(x, " . ");
    derived().leave(x);
  }

  // TODO: find out why precedences are hard coded here
  void apply(const process::if_then& x)
  {
    derived().enter(x);
    print_if_then_condition(x.condition(), " -> ", max_precedence);
    print_expression(x.then_case());
    derived().leave(x);
  }

  // TODO: find out why precedences are hard coded here
  void apply(const process::if_then_else& x)
  {
    derived().enter(x);
    print_if_then_condition(x.condition(), " -> ", max_precedence);
    // N.B. the then case is printed with a higher precedence, since we want the expression a -> b -> c <> d <> e
    // to be printed as a -> (b -> c <> d) <> e
    print_expression(x.then_case(), left_precedence(x) + 1);
    derived().print(" <> ");
    print_expression(x.else_case());
    derived().leave(x);
  }

  void apply(const process::bounded_init& x)
  {
    derived().enter(x);
    print_binary_operation(x, " << ");
    derived().leave(x);
  }

  void apply(const process::merge& x)
  {
    derived().enter(x);
    print_binary_operation(x, " || ");
    derived().leave(x);
  }

  void apply(const process::left_merge& x)
  {
    derived().enter(x);
    print_binary_operation(x, " ||_ ");
    derived().leave(x);
  }

  void apply(const process::choice& x)
  {
    derived().enter(x);
    print_binary_operation(x, " + ");
    derived().leave(x);
  }

  void apply(const process::untyped_process_assignment& x)
  {
    derived().enter(x);
    derived().apply(x.name());
    print_assignments(x.assignments(), false);
    derived().leave(x);
  }

  void apply(const process::untyped_parameter_identifier& x)
  {
    derived().enter(x);
    derived().apply(x.name());
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
    printer.apply(x);
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
