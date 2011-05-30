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
  using core::detail::printer<Derived>::print_sorts;

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  void operator()(const process::process_specification& x)
  {
    derived().enter(x);
    derived()(x.action_labels());
    derived()(x.init());
    derived().leave(x);
  }

  void operator()(const process::process_identifier& x)
  {
    derived().enter(x);
    derived()(x.sorts());
    derived().leave(x);
  }

  void operator()(const process::process_equation& x)
  {
    derived().enter(x);
    derived()(x.identifier());
    derived()(x.formal_parameters());
    derived()(x.expression());
    derived().leave(x);
  }

  void operator()(const process::process_instance& x)
  {
    derived().enter(x);
    derived()(x.identifier());
    derived()(x.actual_parameters());
    derived().leave(x);
  }

  void operator()(const process::process_instance_assignment& x)
  {
    derived().enter(x);
    derived()(x.identifier());
    derived()(x.assignments());
    derived().leave(x);
  }

  void operator()(const process::delta& x)
  {
    derived().enter(x);
    // skip
    derived().leave(x);
  }

  void operator()(const process::tau& x)
  {
    derived().enter(x);
    // skip
    derived().leave(x);
  }

  void operator()(const process::sum& x)
  {
    derived().enter(x);
    derived()(x.bound_variables());
    derived()(x.operand());
    derived().leave(x);
  }

  void operator()(const process::block& x)
  {
    derived().enter(x);
    derived()(x.operand());
    derived().leave(x);
  }

  void operator()(const process::hide& x)
  {
    derived().enter(x);
    derived()(x.operand());
    derived().leave(x);
  }

  void operator()(const process::rename& x)
  {
    derived().enter(x);
    derived()(x.operand());
    derived().leave(x);
  }

  void operator()(const process::comm& x)
  {
    derived().enter(x);
    derived()(x.operand());
    derived().leave(x);
  }

  void operator()(const process::allow& x)
  {
    derived().enter(x);
    derived()(x.operand());
    derived().leave(x);
  }

  void operator()(const process::sync& x)
  {
    derived().enter(x);
    derived()(x.left());
    derived()(x.right());
    derived().leave(x);
  }

  void operator()(const process::at& x)
  {
    derived().enter(x);
    derived()(x.operand());
    derived()(x.time_stamp());
    derived().leave(x);
  }

  void operator()(const process::seq& x)
  {
    derived().enter(x);
    derived()(x.left());
    derived()(x.right());
    derived().leave(x);
  }

  void operator()(const process::if_then& x)
  {
    derived().enter(x);
    derived()(x.condition());
    derived()(x.then_case());
    derived().leave(x);
  }

  void operator()(const process::if_then_else& x)
  {
    derived().enter(x);
    derived()(x.condition());
    derived()(x.then_case());
    derived()(x.else_case());
    derived().leave(x);
  }

  void operator()(const process::bounded_init& x)
  {
    derived().enter(x);
    derived()(x.left());
    derived()(x.right());
    derived().leave(x);
  }

  void operator()(const process::merge& x)
  {
    derived().enter(x);
    derived()(x.left());
    derived()(x.right());
    derived().leave(x);
  }

  void operator()(const process::left_merge& x)
  {
    derived().enter(x);
    derived()(x.left());
    derived()(x.right());
    derived().leave(x);
  }

  void operator()(const process::choice& x)
  {
    derived().enter(x);
    derived()(x.left());
    derived()(x.right());
    derived().leave(x);
  }

};

} // namespace detail

/// \brief Prints the object t to a stream.
template <typename T>
void print(const T& t, std::ostream& out)
{
  core::detail::apply_printer<process::detail::printer> printer(out);
  printer(t);
}

/// \brief Returns a string representation of the object t.
template <typename T>
std::string print(const T& t)
{
  std::ostringstream out;
  process::print(t, out);
  return out.str();
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_PRINT_H
