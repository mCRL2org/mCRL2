// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
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

namespace detail {

template <typename Derived>
struct printer: public bes::add_traverser_boolean_expressions<core::detail::printer, Derived>
{
  typedef bes::add_traverser_boolean_expressions<core::detail::printer, Derived> super;

  using super::enter;
  using super::leave;
  using super::operator();
  using super::print_expression;
  using super::print_unary_operation;
  using super::print_binary_operation;
  using super::print_list;

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  void operator()(const bes::boolean_equation& x)
  {
    derived().enter(x);
    derived().print(x.symbol().is_mu() ? "mu " : "nu ");
    derived()(x.variable());
    derived().print(" = ");
    derived()(x.formula());
    derived().leave(x);
  }

  void operator()(const bes::boolean_equation_system& x)
  {
    print_list(x.equations(), "pbes\n    ", ";\n\n", ";\n    ");
    derived().print("init ");
    print_expression(x.initial_state());
    derived().print(";\n");
    derived().leave(x);
  }

  void operator()(const bes::true_& x)
  {
    derived().enter(x);
    derived().print("true");
    derived().leave(x);
  }

  void operator()(const bes::false_& x)
  {
    derived().enter(x);
    derived().print("false");
    derived().leave(x);
  }

  void operator()(const bes::not_& x)
  {
    derived().enter(x);
    print_unary_operation(x, "!");
    derived().leave(x);
  }

  void operator()(const bes::and_& x)
  {
    derived().enter(x);
    print_binary_operation(x, " && ");
    derived().leave(x);
  }

  void operator()(const bes::or_& x)
  {
    derived().enter(x);
    print_binary_operation(x, " || ");
    derived().leave(x);
  }

  void operator()(const bes::imp& x)
  {
    derived().enter(x);
    print_binary_operation(x, " => ");
    derived().leave(x);
  }

  void operator()(const bes::boolean_variable& x)
  {
    derived().enter(x);
    derived()(x.name());
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

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_PRINT_H
