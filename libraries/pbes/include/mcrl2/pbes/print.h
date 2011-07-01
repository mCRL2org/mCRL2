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
#include "mcrl2/pbes/detail/precedence.h"

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
//  using core::detail::printer<Derived>::print_sorts;
//  using super::print_action_declarations;
//  using super::print_assignments;
//  using super::print_condition;
  using super::print_abstraction;
  using super::print_list;
  using super::print_binary_operation;
  using super::print_expression;
//  using super::print_time;
  using super::print_variables;

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
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
    derived()(x.variable());
    derived().print(" = ");
    derived()(x.formula());
    derived().print(";");
    derived().leave(x);
  }

  template <typename Container>
  void operator()(const pbes_system::pbes<Container>& x)
  {
    derived().enter(x);

    derived().enter(x);
    derived()(x.data());
    print_variables(x.global_variables(), true, true, "glob ", ";\n\n", ";\n     ");

    // N.B. We have to normalize the sorts of the equations first.
    atermpp::vector<pbes_equation> normalized_equations = x.equations();
    pbes_system::normalize_sorts(normalized_equations, x.data());   
    print_list(normalized_equations, "proc ", "\n\n", "\n     ");

    print_initial_state(x.init());
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
    derived()(x.operand());
    derived().leave(x);
  }

  void operator()(const pbes_system::and_& x)
  {
    derived().enter(x);
    print_binary_operation(x, " && ");
    derived().leave(x);
  }

  void operator()(const pbes_system::or_& x)
  {
    derived().enter(x);
    print_binary_operation(x, " || ");
    derived().leave(x);
  }

  void operator()(const pbes_system::imp& x)
  {
    derived().enter(x);
    print_binary_operation(x, " => ");
    derived().leave(x);
  }

  void operator()(const pbes_system::forall& x)
  {
    derived().enter(x);
    print_abstraction(x, "forall");
    derived().leave(x);
  }

  void operator()(const pbes_system::exists& x)
  {
    derived().enter(x);
    print_abstraction(x, "exists");
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

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PRINT_H
