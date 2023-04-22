// Author(s): Jan Friso Groote. Based on bes/print.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/res/print.h
/// \brief add your file description here.

#ifndef MCRL2_RES_PRINT_H
#define MCRL2_RES_PRINT_H

#include "mcrl2/res/traverser.h"

namespace mcrl2 {

namespace res {

/// \brief Returns the precedence of res expressions
constexpr inline int precedence(const imp&)    { return 2; }
constexpr inline int precedence(const or_&)    { return 3; }
constexpr inline int precedence(const and_&)   { return 4; }
constexpr inline int precedence(const minus&)   { return 5; }
inline int precedence(const res_expression& x)
{
  if (is_imp(x))      { return precedence(atermpp::down_cast<imp>(x)); }
  else if (is_or(x))  { return precedence(atermpp::down_cast<or_>(x)); }
  else if (is_and(x)) { return precedence(atermpp::down_cast<and_>(x)); }
  else if (is_minus(x)) { return precedence(atermpp::down_cast<minus>(x)); }
  return core::detail::max_precedence;
}

// only defined for binary operators
inline bool is_left_associative(const imp&)  { return false; }
inline bool is_left_associative(const or_&)  { return true; }
inline bool is_left_associative(const and_&) { return true; }
inline bool is_left_associative(const res_expression& x)
{
  if (is_imp(x))      { return is_left_associative(atermpp::down_cast<imp>(x)); }
  else if (is_or(x))  { return is_left_associative(atermpp::down_cast<or_>(x)); }
  else if (is_and(x)) { return is_left_associative(atermpp::down_cast<and_>(x)); }
  return false;
}

inline bool is_right_associative(const imp&)  { return true; }
inline bool is_right_associative(const or_&)  { return true; }
inline bool is_right_associative(const and_&) { return true; }
inline bool is_right_associative(const res_expression& x)
{
  if (is_imp(x))      { return is_right_associative(atermpp::down_cast<imp>(x)); }
  else if (is_or(x))  { return is_right_associative(atermpp::down_cast<or_>(x)); }
  else if (is_and(x)) { return is_right_associative(atermpp::down_cast<and_>(x)); }
  return false;
}

namespace detail {

template <typename Derived>
struct printer: public res::add_traverser_res_expressions<core::detail::printer, Derived>
{
  typedef res::add_traverser_res_expressions<core::detail::printer, Derived> super;

  using super::enter;
  using super::leave;
  using super::apply;
  using super::derived;
  using super::print_expression;
  using super::print_unary_left_operation;
  using super::print_binary_operation;
  using super::print_list;

  void apply(const res::res_equation& x)
  {
    derived().enter(x);
    derived().print(x.symbol().is_mu() ? "mu " : "nu ");
    derived().apply(x.variable());
    derived().print(" = ");
    derived().apply(x.formula());
    derived().leave(x);
  }

  void apply(const res::res_equation_system& x)
  {
    print_list(x.equations(), "pres\n    ", ";\n\n", ";\n    ");
    derived().print("init ");
    print_expression(x.initial_state(), false);
    derived().print(";\n");
    derived().leave(x);
  }

  void apply(const res::true_& x)
  {
    derived().enter(x);
    derived().print("true");
    derived().leave(x);
  }

  void apply(const res::false_& x)
  {
    derived().enter(x);
    derived().print("false");
    derived().leave(x);
  }

  void apply(const res::minus& x)
  {
    derived().enter(x);
    print_unary_left_operation(x, " -");
    derived().leave(x);
  }

  void apply(const res::and_& x)
  {
    derived().enter(x);
    print_binary_operation(x, " && ");
    derived().leave(x);
  }

  void apply(const res::or_& x)
  {
    derived().enter(x);
    print_binary_operation(x, " || ");
    derived().leave(x);
  }

  void apply(const res::imp& x)
  {
    derived().enter(x);
    print_binary_operation(x, " => ");
    derived().leave(x);
  }

  void apply(const res::res_variable& x)
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
    core::detail::apply_printer<res::detail::printer> printer(out);
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

} // namespace res

} // namespace mcrl2

#endif // MCRL2_RES_PRINT_H
