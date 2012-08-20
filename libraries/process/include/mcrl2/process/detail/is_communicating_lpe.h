// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/detail/is_communicating_lpe.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_DETAIL_IS_COMMUNICATING_LPE_H
#define MCRL2_PROCESS_DETAIL_IS_COMMUNICATING_LPE_H

#include "mcrl2/process/traverser.h"

namespace mcrl2 {

namespace process {

namespace detail {

/// \brief Checks if a process expression is 'communicating LPE'.
struct communicating_lpe_traverser: public process_expression_traverser<communicating_lpe_traverser>
{
  typedef process_expression_traverser<communicating_lpe_traverser> super;
  using super::enter;
  using super::leave;
  using super::operator();

  bool result;

  communicating_lpe_traverser()
    : result(true)
  {}

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  void enter(const process_instance& x)
  {
    result = false;
  }

  void enter(const process_instance_assignment& x)
  {
    result = false;
  }

  void enter(const sum& x)
  {
    result = false;
  }

  void enter(const block& x)
  {
  }

  void enter(const hide& x)
  {
  }

  void enter(const rename& x)
  {
  }

  void enter(const comm& x)
  {
  }

  void enter(const allow& x)
  {
  }

  void enter(const sync& x)
  {
  }

  void enter(const at& x)
  {
    result = false;
  }

  void enter(const seq& x)
  {
    result = false;
  }

  void enter(const if_then& x)
  {
    result = false;
  }

  void enter(const if_then_else& x)
  {
    result = false;
  }

  void enter(const bounded_init& x)
  {
    result = false;
  }

  void enter(const merge& x)
  {
  }

  void enter(const left_merge& x)
  {
    result = false;
  }
};

inline
bool is_communicating_lpe(const process_expression& x)
{
  communicating_lpe_traverser f;
  f(x);
  return f.result;
}

} // namespace detail

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_DETAIL_IS_COMMUNICATING_LPE_H
