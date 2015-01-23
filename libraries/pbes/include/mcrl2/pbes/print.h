// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/print.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_PRINT_H
#define MCRL2_PBES_PRINT_H

#include "mcrl2/data/print.h"
#include "mcrl2/pbes/traverser.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

template <typename Derived>
struct printer: public pbes_system::add_traverser_sort_expressions<data::detail::printer, Derived>
{
  typedef pbes_system::add_traverser_sort_expressions<data::detail::printer, Derived> super;

  using super::enter;
  using super::leave;
  using super::apply;
  using super::print_abstraction;
  using super::print_list;
  using super::print_binary_operation;
  using super::print_expression;
  using super::print_variables;

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  // N.B. We need a special version due to the "val" operator that needs to be
  // put around data expressions.
  template <typename T>
  void print_pbes_expression(const T& x, int prec = 5)
  {
    bool print_parens = (left_precedence(x) < prec);
    bool is_data_expr = is_data(x);
    if (print_parens)
    {
      derived().print("(");
    }
    if (is_data_expr)
    {
      derived().print("val(");
    }
    derived().apply(x);
    if (is_data_expr)
    {
      derived().print(")");
    }
    if (print_parens)
    {
      derived().print(")");
    }
  }

  // N.B. We need a special version due to the "val" operator that needs to be
  // put around data expressions.
  template <typename T>
  void print_pbes_unary_operation(const T& x, const std::string& op)
  {
    derived().print(op);
    print_pbes_expression(x.operand(), left_precedence(x));
  }

  // N.B. We need a special version due to the "val" operator that needs to be
  // put around data expressions.
  template <typename T>
  void print_pbes_binary_operation(const T& x, const std::string& op)
  {
    print_pbes_expression(x.left(), pbes_system::is_same_different_precedence(x, x.left()) ? left_precedence(x) + 1 : left_precedence(x));
    derived().print(op);
    print_pbes_expression(x.right(), pbes_system::is_same_different_precedence(x, x.right()) ? right_precedence(x) + 1 : right_precedence(x));
  }

  // N.B. We need a special version due to the "val" operator that needs to be
  // put around data expressions.
  template <typename Abstraction>
  void print_pbes_abstraction(const Abstraction& x, const std::string& op)
  {
    derived().enter(x);
    derived().print(op + " ");
    print_variables(x.variables(), true, true, false, "", "", ", ");
    derived().print(". ");
    print_pbes_expression(x.body(), left_precedence(x));
    derived().leave(x);
  }

  void apply(const pbes_system::propositional_variable& x)
  {
    derived().enter(x);
    derived().apply(x.name());
    print_variables(x.parameters());
    derived().leave(x);
  }

  void apply(const pbes_system::pbes_equation& x)
  {
    derived().enter(x);
    derived().print(x.symbol().is_mu() ? "mu " : "nu ");
    derived().apply(x.variable());
    // TODO: change the weird convention of putting the rhs of an equation on a new line
    derived().print(" =\n       ");
    bool print_val = data::is_data_expression(x.formula());
    if (print_val)
    {
      derived().print("val(");
    }
    derived().apply(x.formula());
    if (print_val)
    {
      derived().print(")");
    }
    derived().print(";");
    derived().leave(x);
  }

  void apply(const pbes_system::pbes& x)
  {
    derived().enter(x);
    derived().apply(x.data());
    print_variables(x.global_variables(), true, true, true, "glob ", ";\n\n", ";\n     ");

    // N.B. We have to normalize the sorts of the equations first.
    std::vector<pbes_equation> normalized_equations = x.equations();
    pbes_system::normalize_sorts(normalized_equations, x.data());
    print_list(normalized_equations, "pbes ", "\n\n", "\n     ");

    derived().print("init ");
    print_pbes_expression(x.initial_state());
    derived().print(";\n");
    derived().leave(x);
  }

  void apply(const pbes_system::propositional_variable_instantiation& x)
  {
    derived().enter(x);
    derived().apply(x.name());
    print_list(x.parameters(), "(", ")", ", ", false);
    derived().leave(x);
  }

  void apply(const pbes_system::not_& x)
  {
    derived().enter(x);
    print_pbes_unary_operation(x, "!");
    derived().leave(x);
  }

  void apply(const pbes_system::and_& x)
  {
    derived().enter(x);
    print_pbes_binary_operation(x, " && ");
    derived().leave(x);
  }

  void apply(const pbes_system::or_& x)
  {
    derived().enter(x);
    print_pbes_binary_operation(x, " || ");
    derived().leave(x);
  }

  void apply(const pbes_system::imp& x)
  {
    derived().enter(x);
    print_pbes_binary_operation(x, " => ");
    derived().leave(x);
  }

  void apply(const pbes_system::forall& x)
  {
    derived().enter(x);
    print_pbes_abstraction(x, "forall");
    derived().leave(x);
  }

  void apply(const pbes_system::exists& x)
  {
    derived().enter(x);
    print_pbes_abstraction(x, "exists");
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
    core::detail::apply_printer<pbes_system::detail::printer> printer(out);
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

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PRINT_H
