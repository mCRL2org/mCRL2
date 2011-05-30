// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/print.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_PRINT_H
#define MCRL2_LPS_PRINT_H

#include "mcrl2/core/print.h"
#include "mcrl2/data/print.h"
#include "mcrl2/lps/traverser.h"

namespace mcrl2
{

namespace lps
{

namespace detail
{

template <typename Derived>
struct printer: public lps::add_traverser_sort_expressions<data::detail::printer, Derived>
{
  typedef lps::add_traverser_sort_expressions<data::detail::printer, Derived> super;

  using super::enter;
  using super::leave;
  using super::operator();
  using core::detail::printer<Derived>::print_sorts;

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  void operator()(const lps::action_label& x)
  {
    derived().enter(x);
    derived()(x.sorts());
    derived().leave(x);
  }

  void operator()(const lps::action& x)
  {
    derived().enter(x);
    derived()(x.label());
    derived()(x.arguments());
    derived().leave(x);
  }

  void operator()(const lps::deadlock& x)
  {
    derived().enter(x);
    if (x.has_time())
    {
      derived()(x.time());
    }
    derived().leave(x);
  }

  void operator()(const lps::multi_action& x)
  {
    derived().enter(x);
    derived()(x.actions());
    if (x.has_time())
    {
      derived()(x.time());
    }
    derived().leave(x);
  }

  void operator()(const lps::deadlock_summand& x)
  {
    derived().enter(x);
    derived()(x.summation_variables());
    derived()(x.condition());
    derived()(x.deadlock());
    derived().leave(x);
  }

  void operator()(const lps::action_summand& x)
  {
    derived().enter(x);
    derived()(x.summation_variables());
    derived()(x.condition());
    derived()(x.multi_action());
    derived()(x.assignments());
    derived().leave(x);
  }

  void operator()(const lps::process_initializer& x)
  {
    derived().enter(x);
    derived()(x.assignments());
    derived().leave(x);
  }

  void operator()(const lps::linear_process& x)
  {
    derived().enter(x);
    derived()(x.process_parameters());
    derived()(x.deadlock_summands());
    derived()(x.action_summands());
    derived().leave(x);
  }

  void operator()(const lps::specification& x)
  {
    derived().enter(x);
    derived()(x.data());
    derived()(x.action_labels());
    derived()(x.process());
    derived()(x.initial_process());
    derived().leave(x);
  }
};

} // namespace detail

/// \brief Prints the object t to a stream.
template <typename T>
void print(const T& t, std::ostream& out)
{
  core::detail::apply_printer<lps::detail::printer> printer(out);
  printer(t);
}

/// \brief Returns a string representation of the object t.
template <typename T>
std::string print(const T& t)
{
  std::ostringstream out;
  lps::print(t, out);
  return out.str();
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_PRINT_H
