// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/print.h
/// \brief add your file description here.

#ifndef MCRL2_BES_PRINT_H
#define MCRL2_BES_PRINT_H

#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/traverser.h"
#include "mcrl2/core/print.h"

namespace mcrl2 {

namespace bes {

/// \brief Returns the precedence of boolean expressions
constexpr inline int precedence(const imp&)    { return 2; }
constexpr inline int precedence(const or_&)    { return 3; }
constexpr inline int precedence(const and_&)   { return 4; }
constexpr inline int precedence(const not_&)   { return 5; }
inline int precedence(const boolean_expression& x)
{
  if (is_imp(x))      { return precedence(atermpp::down_cast<imp>(x)); }
  else if (is_or(x))  { return precedence(atermpp::down_cast<or_>(x)); }
  else if (is_and(x)) { return precedence(atermpp::down_cast<and_>(x)); }
  else if (is_not(x)) { return precedence(atermpp::down_cast<not_>(x)); }
  return core::detail::max_precedence;
}

// only defined for binary operators
inline bool is_left_associative(const imp&)  { return false; }
inline bool is_left_associative(const or_&)  { return true; }
inline bool is_left_associative(const and_&) { return true; }
inline bool is_left_associative(const boolean_expression& x)
{
  if (is_imp(x))      { return is_left_associative(atermpp::down_cast<imp>(x)); }
  else if (is_or(x))  { return is_left_associative(atermpp::down_cast<or_>(x)); }
  else if (is_and(x)) { return is_left_associative(atermpp::down_cast<and_>(x)); }
  return false;
}

inline bool is_right_associative(const imp&)  { return true; }
inline bool is_right_associative(const or_&)  { return true; }
inline bool is_right_associative(const and_&) { return true; }
inline bool is_right_associative(const boolean_expression& x)
{
  if (is_imp(x))      { return is_right_associative(atermpp::down_cast<imp>(x)); }
  else if (is_or(x))  { return is_right_associative(atermpp::down_cast<or_>(x)); }
  else if (is_and(x)) { return is_right_associative(atermpp::down_cast<and_>(x)); }
  return false;
}

namespace detail {

template <typename Derived>
struct printer: public bes::add_traverser_boolean_expressions<core::detail::printer, Derived>
{
  typedef bes::add_traverser_boolean_expressions<core::detail::printer, Derived> super;

  using super::enter;
  using super::leave;
  using super::apply;
  using super::derived;
  using super::print_expression;
  using super::print_unary_left_operation;
  using super::print_binary_operation;
  using super::print_list;

  void apply(const bes::boolean_equation& x)
  {
    derived().enter(x);
    derived().print(x.symbol().is_mu() ? "mu " : "nu ");
    derived().apply(x.variable());
    derived().print(" = ");
    derived().apply(x.formula());
    derived().leave(x);
  }

  void apply(const bes::boolean_equation_system& x)
  {
    print_list(x.equations(), "pbes\n    ", ";\n\n", ";\n    ");
    derived().print("init ");
    print_expression(x.initial_state(), false);
    derived().print(";\n");
    derived().leave(x);
  }

  void apply(const bes::true_& x)
  {
    derived().enter(x);
    derived().print("true");
    derived().leave(x);
  }

  void apply(const bes::false_& x)
  {
    derived().enter(x);
    derived().print("false");
    derived().leave(x);
  }

  void apply(const bes::not_& x)
  {
    derived().enter(x);
    print_unary_left_operation(x, "!");
    derived().leave(x);
  }

  void apply(const bes::and_& x)
  {
    derived().enter(x);
    print_binary_operation(x, " && ");
    derived().leave(x);
  }

  void apply(const bes::or_& x)
  {
    derived().enter(x);
    print_binary_operation(x, " || ");
    derived().leave(x);
  }

  void apply(const bes::imp& x)
  {
    derived().enter(x);
    print_binary_operation(x, " => ");
    derived().leave(x);
  }

  void apply(const bes::boolean_variable& x)
  {
    derived().enter(x);
    derived().apply(x.name());
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
    core::detail::apply_printer<bes::detail::printer> printer(out);
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

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_PRINT_H
