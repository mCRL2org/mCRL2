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
#include "mcrl2/pbes/normalize_sorts.h"
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
  using super::operator();
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
    bool print_parens = (precedence(x) < prec);
    bool is_data_expr = is_data(x);
    if (print_parens)
    {
      derived().print("(");
    }
    if (is_data_expr)
    {
      derived().print("val(");
    }
    derived()(x);
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
    print_pbes_expression(x.operand(), precedence(x));
  }

  // N.B. We need a special version due to the "val" operator that needs to be
  // put around data expressions.
  template <typename T>
  void print_pbes_binary_operation(const T& x, const std::string& op)
  {
    print_pbes_expression(x.left(), is_same_different_precedence(x, x.left()) ? precedence(x) + 1 : precedence(x));
    derived().print(op);
    print_pbes_expression(x.right(), is_same_different_precedence(x, x.right()) ? precedence(x) + 1 : precedence(x));
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
    print_pbes_expression(x.body());
    derived().leave(x);
  }

  void operator()(const pbes_system::propositional_variable& x)
  {
    derived().enter(x);
    derived()(x.name());
    print_variables(x.parameters());
    derived().leave(x);
  }

  void operator()(const pbes_system::pbes_equation& x)
  {
    derived().enter(x);
    derived().print(x.symbol().is_mu() ? "mu " : "nu ");
    derived()(x.variable());
    // TODO: change the weird convention of putting the rhs of an equation on a new line
    derived().print(" =\n       ");
    derived()(x.formula());
    derived().print(";");
    derived().leave(x);
  }

  template <typename Container>
  void operator()(const pbes_system::pbes<Container>& x)
  {
    derived().enter(x);
    derived()(x.data());
    print_variables(x.global_variables(), true, true, true, "glob ", ";\n\n", ";\n     ");

    // N.B. We have to normalize the sorts of the equations first.
    atermpp::vector<pbes_equation> normalized_equations = x.equations();
    pbes_system::normalize_sorts(normalized_equations, x.data());   
    print_list(normalized_equations, "pbes ", "\n\n", "\n     ");

    derived().print("init ");
    print_pbes_expression(x.initial_state());
    derived().print(";\n");
    derived().leave(x);
  }

  void operator()(const pbes_system::propositional_variable_instantiation& x)
  {
    derived().enter(x);
    derived()(x.name());
    print_variables(x.parameters(), false);
    derived().leave(x);
  }

  void operator()(const pbes_system::true_& x)
  {
    derived().enter(x);
    derived().print("true");
    derived().leave(x);
  }

  void operator()(const pbes_system::false_& x)
  {
    derived().enter(x);
    derived().print("false");
    derived().leave(x);
  }

  void operator()(const pbes_system::not_& x)
  {
    derived().enter(x);
    print_pbes_unary_operation(x, "!");
    derived().leave(x);
  }

  void operator()(const pbes_system::and_& x)
  {
    derived().enter(x);
    print_pbes_binary_operation(x, " && ");
    derived().leave(x);
  }

  void operator()(const pbes_system::or_& x)
  {
    derived().enter(x);
    print_pbes_binary_operation(x, " || ");
    derived().leave(x);
  }

  void operator()(const pbes_system::imp& x)
  {
    derived().enter(x);
    print_pbes_binary_operation(x, " => ");
    derived().leave(x);
  }

  void operator()(const pbes_system::forall& x)
  {
    derived().enter(x);
    print_pbes_abstraction(x, "forall");
    derived().leave(x);
  }

  void operator()(const pbes_system::exists& x)
  {
    derived().enter(x);
    print_pbes_abstraction(x, "exists");
    derived().leave(x);
  }
};

} // namespace detail

/// \brief Prints the object t to a stream.
template <typename T>
void print(const T& t, std::ostream& out)
{
  core::detail::apply_printer<pbes_system::detail::printer> printer(out);
  printer(t);
}

/// \brief Returns a string representation of the object t.
template <typename T>
std::string print(const T& t)
{
  std::ostringstream out;
  pbes_system::print(t, out);
  return out.str();
}

/// \brief Pretty prints a term.
/// \param[in] t A term
template <typename T>
std::string pp(const T& t)
{
  MCRL2_CHECK_PP(core::pp(t), pbes_system::print(t), t.to_string());
  return core::pp(t);
}

inline
std::string pp(const pbes_equation& eqn)
{
  MCRL2_CHECK_PP(core::pp(pbes_equation_to_aterm(eqn)), pbes_system::print(eqn), pbes_equation_to_aterm(eqn).to_string());
  return core::pp(pbes_equation_to_aterm(eqn));
}

/// \brief Pretty print function
/// \param spec A pbes specification
/// \return A pretty print representation of the specification
template <typename Container>
std::string pp(const pbes<Container>& spec, core::t_pp_format pp_format = core::ppDefault)
{
  MCRL2_CHECK_PP(core::pp(pbes_to_aterm(spec)), pbes_system::print(spec), pbes_to_aterm(spec).to_string());
  return core::pp(pbes_to_aterm(spec), pp_format);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PRINT_H
