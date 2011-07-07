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
#include "mcrl2/core/print.h"

namespace mcrl2 {

namespace bes {

namespace detail
{

template <typename Derived>
struct printer: public data::add_traverser_sort_expressions<core::detail::printer, Derived>
{
  typedef data::add_traverser_sort_expressions<core::detail::printer, Derived> super;

  using super::enter;
  using super::leave;
  using super::operator();
  using core::detail::printer<Derived>::print_list;

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  void operator()(const bes::boolean_equation& x)
  {
    derived().enter(x);
    derived()(x.formula());
    derived().leave(x);
  }

  template <typename Container>
  void operator()(const bes::boolean_equation_system<Container>& x)
  {
    derived().enter(x);
    derived()(x.equations());
    derived()(x.initial_state());
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
    derived()(x.operand());
    derived().leave(x);
  }

  void operator()(const bes::and_& x)
  {
    derived().enter(x);
    derived()(x.left());
    derived()(x.right());
    derived().leave(x);
  }

  void operator()(const bes::or_& x)
  {
    derived().enter(x);
    derived()(x.left());
    derived()(x.right());
    derived().leave(x);
  }

  void operator()(const bes::imp& x)
  {
    derived().enter(x);
    derived()(x.left());
    derived()(x.right());
    derived().leave(x);
  }

  void operator()(const bes::boolean_variable& x)
  {
    derived().enter(x);
    // skip
    derived().leave(x);
  }

} // namespace detail

/// \brief Prints the object t to a stream.
template <typename T>
void print(const T& t, std::ostream& out)
{
  core::detail::apply_printer<bes::detail::printer> printer(out);
  printer(t);
}

/// \brief Returns a string representation of the object t.
template <typename T>
std::string print(const T& t)
{
  std::ostringstream out;
  bes::print(t, out);
  return out.str();
}

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_PRINT_H
