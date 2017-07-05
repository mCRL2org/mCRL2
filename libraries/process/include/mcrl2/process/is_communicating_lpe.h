// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/is_communicating_lpe.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_IS_COMMUNICATING_LPE_H
#define MCRL2_PROCESS_IS_COMMUNICATING_LPE_H

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
  using super::apply;

  bool result;

  communicating_lpe_traverser()
    : result(true)
  {}

  void enter(const process::process_instance&)
  {
    result = false;
  }

  void enter(const process::process_instance_assignment&)
  {
    result = false;
  }

  void enter(const process::sum&)
  {
    result = false;
  }

  void enter(const process::block&)
  {
  }

  void enter(const process::hide&)
  {
  }

  void enter(const process::rename&)
  {
  }

  void enter(const process::comm&)
  {
  }

  void enter(const process::allow&)
  {
  }

  void enter(const process::sync&)
  {
  }

  void enter(const process::at&)
  {
    result = false;
  }

  void enter(const process::seq&)
  {
    result = false;
  }

  void enter(const process::if_then&)
  {
    result = false;
  }

  void enter(const process::if_then_else&)
  {
    result = false;
  }

  void enter(const process::bounded_init&)
  {
    result = false;
  }

  void enter(const process::merge&)
  {
  }

  void enter(const process::left_merge&)
  {
    result = false;
  }
};

} // namespace detail

/// \brief Returns true if x is in communicating LPE format
inline
bool is_communicating_lpe(const process::process_expression& x)
{
  detail::communicating_lpe_traverser f;
  f.apply(x);
  return f.result;
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_IS_COMMUNICATING_LPE_H
