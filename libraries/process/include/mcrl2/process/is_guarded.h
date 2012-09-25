// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/is_guarded.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_IS_GUARDED_H
#define MCRL2_PROCESS_IS_GUARDED_H

#include "mcrl2/process/traverser.h"

namespace mcrl2 {

namespace process {

namespace detail {

template <typename Derived>
struct is_guarded_traverser: public process_expression_traverser<Derived>
{
  typedef process_expression_traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  bool result;

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  is_guarded_traverser()
    : result(true)
  {}

  // P(e1, ..., en)
  void enter(const process::process_instance& x)
  {
    result = false;
  }

  // P(d1 = e1, ..., dn = en)
  void enter(const process::process_instance_assignment& x)
  {
    result = false;
  }


  // p . q
  void operator()(const process::seq& x)
  {
    derived()(x.left()); // only p needs to be guarded
  }

  // p << q
  void operator()(const process::bounded_init& x)
  {
    derived()(x.left()); // only p needs to be guarded
  }
};

} // detail

inline
bool is_guarded(const process_expression& x)
{
  core::apply_traverser<detail::is_guarded_traverser> f;
  f(x);
  return f.result;
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_IS_GUARDED_H
