// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/print.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_PRINT_H
#define MCRL2_MODAL_FORMULA_PRINT_H

#include "mcrl2/core/print.h"
#include "mcrl2/modal_formula/traverser.h"
#include "mcrl2/lps/print.h"

namespace mcrl2 {

namespace action_formulas {

using core::detail::precedences::max_precedence;

namespace detail
{

template <typename Derived>
struct printer: public action_formulas::add_traverser_sort_expressions<lps::detail::printer, Derived>
{
  typedef action_formulas::add_traverser_sort_expressions<lps::detail::printer, Derived> super;

  using super::enter;
  using super::leave;
  using super::operator();
  using super::print_unary_operation;
  using super::print_binary_operation;
  using super::print_abstraction;
  using super::print_expression;
  using super::print_list;

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  void operator()(const action_formulas::true_& x)
  {
    derived().enter(x);
    derived().print("true");
    derived().leave(x);
  }

  void operator()(const action_formulas::false_& x)
  {
    derived().enter(x);
    derived().print("false");
    derived().leave(x);
  }

  void operator()(const action_formulas::not_& x)
  {
    derived().enter(x);
    print_unary_operation(x, "!");
    derived().leave(x);
  }

  void operator()(const action_formulas::and_& x)
  {
    derived().enter(x);
    print_binary_operation(x, " && ");
    derived().leave(x);
  }

  void operator()(const action_formulas::or_& x)
  {
    derived().enter(x);
    print_binary_operation(x, " || ");
    derived().leave(x);
  }

  void operator()(const action_formulas::imp& x)
  {
    derived().enter(x);
    print_binary_operation(x, " => ");
    derived().leave(x);
  }

  void operator()(const action_formulas::forall& x)
  {
    derived().enter(x);
    print_abstraction(x, "forall");
    derived().leave(x);
  }

  void operator()(const action_formulas::exists& x)
  {
    derived().enter(x);
    print_abstraction(x, "exists");
    derived().leave(x);
  }

  void operator()(const action_formulas::at& x)
  {
    derived().enter(x);
    derived()(x.operand());
    derived().print(" @ ");
    print_expression(x.time_stamp(), max_precedence);
    derived().leave(x);
  }

  void operator()(const action_formulas::multi_action& x)
  {
    derived().enter(x);
    if (x.actions().empty())
    {
      derived().print("tau");
    }
    else
    {
      print_list(x.actions(), "", "", "|");
    }
    derived().leave(x);
  }

  void operator()(const action_formulas::untyped_multi_action& x)
  {
    derived().enter(x);
    if (x.arguments().empty())
    {
      derived().print("tau");
    }
    else
    {
      print_list(x.arguments(), "", "", "|");
    }
    derived().leave(x);
  }
};

} // namespace detail

/// \brief Prints the object t to a stream.
template <typename T>
void pp(const T& t, std::ostream& out)
{
  core::detail::apply_printer<action_formulas::detail::printer> printer(out);
  printer(t);
}

/// \brief Returns a string representation of the object t.
template <typename T>
std::string pp(const T& t)
{
  std::ostringstream out;
  action_formulas::pp(t, out);
  return out.str();
}

} // namespace action_formulas

namespace regular_formulas {

using core::detail::precedences::max_precedence;

namespace detail
{

template <typename Derived>
struct printer: public regular_formulas::add_traverser_sort_expressions<action_formulas::detail::printer, Derived>
{
  typedef regular_formulas::add_traverser_sort_expressions<action_formulas::detail::printer, Derived> super;

  using super::enter;
  using super::leave;
  using super::operator();
  using super::print_unary_operation;
  using super::print_binary_operation;
  using super::print_expression;

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  void operator()(const regular_formulas::nil& x)
  {
    derived().enter(x);
    derived().leave(x);
  }

  void operator()(const regular_formulas::seq& x)
  {
    derived().enter(x);
    print_binary_operation(x, " . ");
    derived().leave(x);
  }

  void operator()(const regular_formulas::alt& x)
  {
    derived().enter(x);
    print_binary_operation(x, " + ");
    derived().leave(x);
  }

  void operator()(const regular_formulas::trans& x)
  {
    derived().enter(x);
    print_expression(x.operand(), precedence(x));
    derived().print("+");
    derived().leave(x);
  }

  void operator()(const regular_formulas::trans_or_nil& x)
  {
    derived().enter(x);
    print_expression(x.operand(), precedence(x));
    derived().print("*");
    derived().leave(x);
  }
};

} // namespace detail

/// \brief Prints the object t to a stream.
template <typename T>
void pp(const T& t, std::ostream& out)
{
  core::detail::apply_printer<regular_formulas::detail::printer> printer(out);
  printer(t);
}

/// \brief Returns a string representation of the object t.
template <typename T>
std::string pp(const T& t)
{
  std::ostringstream out;
  regular_formulas::pp(t, out);
  return out.str();
}

} // namespace regular_formulas

namespace state_formulas {

using core::detail::precedences::max_precedence;

namespace detail
{

template <typename Derived>
struct printer: public state_formulas::add_traverser_sort_expressions<regular_formulas::detail::printer, Derived>
{
  typedef state_formulas::add_traverser_sort_expressions<regular_formulas::detail::printer, Derived> super;

  using super::enter;
  using super::leave;
  using super::operator();
  using super::print_unary_operation;
  using super::print_binary_operation;
  using super::print_abstraction;
  using super::print_variables;
  using super::print_expression;

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  void operator()(const state_formulas::true_& x)
  {
    derived().enter(x);
    derived().print("true");
    derived().leave(x);
  }

  void operator()(const state_formulas::false_& x)
  {
    derived().enter(x);
    derived().print("false");
    derived().leave(x);
  }

  void operator()(const state_formulas::not_& x)
  {
    derived().enter(x);
    print_unary_operation(x, "!");
    derived().leave(x);
  }

  void operator()(const state_formulas::and_& x)
  {
    derived().enter(x);
    print_binary_operation(x, " && ");
    derived().leave(x);
  }

  void operator()(const state_formulas::or_& x)
  {
    derived().enter(x);
    print_binary_operation(x, " || ");
    derived().leave(x);
  }

  void operator()(const state_formulas::imp& x)
  {
    derived().enter(x);
    print_binary_operation(x, " => ");
    derived().leave(x);
  }

  void operator()(const state_formulas::forall& x)
  {
    derived().enter(x);
    print_abstraction(x, "forall");
    derived().leave(x);
  }

  void operator()(const state_formulas::exists& x)
  {
    derived().enter(x);
    print_abstraction(x, "exists");
    derived().leave(x);
  }

  void operator()(const state_formulas::must& x)
  {
    derived().enter(x);
    derived().print("[");
    derived()(x.formula());
    derived().print("]");
    derived()(x.operand());
    derived().leave(x);
  }

  void operator()(const state_formulas::may& x)
  {
    derived().enter(x);
    derived().print("<");
    derived()(x.formula());
    derived().print(">");
    derived()(x.operand());
    derived().leave(x);
  }

  void operator()(const state_formulas::yaled& x)
  {
    derived().enter(x);
    derived().print("yaled");
    derived().leave(x);
  }

  void operator()(const state_formulas::yaled_timed& x)
  {
    derived().enter(x);
    derived().print("yaled");
    derived().print(" @ ");
    derived()(x.time_stamp());
    derived().leave(x);
  }

  void operator()(const state_formulas::delay& x)
  {
    derived().enter(x);
    derived().print("delay");
    derived().leave(x);
  }

  void operator()(const state_formulas::delay_timed& x)
  {
    derived().enter(x);
    derived().print("delay");
    derived().print(" @ ");
    derived()(x.time_stamp());
    derived().leave(x);
  }

  void operator()(const state_formulas::variable& x)
  {
    derived().enter(x);
    derived()(x.name());
    print_variables(x.arguments(), false);
    derived().leave(x);
  }

  // TODO: merge this function with the version in data/print.h (?)
  void print_assignments(const data::assignment_list& assignments)
  {
    if (assignments.empty())
    {
      return;
    }
    derived().print("(");
    for (auto i = assignments.begin(); i != assignments.end(); ++i)
    {
      if (i != assignments.begin())
      {
        derived().print(", ");
      }
      derived()(i->lhs());
      derived().print(": ");
      derived()(i->lhs().sort());
      derived().print(" = ");
      derived()(i->rhs());
    }
    derived().print(")");
  }

  void operator()(const state_formulas::nu& x)
  {
    derived().enter(x);
    derived().print("nu ");
    derived()(x.name());
    print_assignments(x.assignments());
    derived().print(". ");
    derived()(x.operand());
    derived().leave(x);
  }

  void operator()(const state_formulas::mu& x)
  {
    derived().enter(x);
    derived().print("mu ");
    derived()(x.name());
    print_assignments(x.assignments());
    derived().print(". ");
    derived()(x.operand());
    derived().leave(x);
  }
};

} // namespace detail

/// \brief Prints the object t to a stream.
template <typename T>
void pp(const T& t, std::ostream& out)
{
  core::detail::apply_printer<state_formulas::detail::printer> printer(out);
  printer(t);
}

/// \brief Returns a string representation of the object t.
template <typename T>
std::string pp(const T& t)
{
  std::ostringstream out;
  state_formulas::pp(t, out);
  return out.str();
}

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_PRINT_H

