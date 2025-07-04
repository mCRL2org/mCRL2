// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/is_multi_action.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_IS_MULTI_ACTION_H
#define MCRL2_PROCESS_IS_MULTI_ACTION_H

#include "mcrl2/process/multi_action_name.h"
#include "mcrl2/process/traverser.h"

namespace mcrl2::process
{

namespace detail {

struct sync_multi_action_name_traverser: public process_expression_traverser<sync_multi_action_name_traverser>
{
  using super = process_expression_traverser<sync_multi_action_name_traverser>;
  using super::enter;
  using super::leave;
  using super::apply;

  multi_action_name result;

  void apply(const process::action& x)
  {
    result.insert(x.label().name());
  }
};

} // namespace detail

/// \brief Returns true if x is a multi action
inline
bool is_multi_action(const process_expression& x)
{
  if (is_action(x))
  {
    return true;
  }
  if (is_sync(x))
  {
    const sync& y = atermpp::down_cast<sync>(x);
    return is_multi_action(y.left()) && is_multi_action(y.right());
  }
  return false;
}

/// Computes a multi action name corresponding to a sync (provided that the sync is a pCRL expression).
inline
multi_action_name sync_multi_action_name(const sync& x)
{
  detail::sync_multi_action_name_traverser f;
  f.apply(x);
  return f.result;
}

} // namespace mcrl2::process

#endif // MCRL2_PROCESS_IS_MULTI_ACTION_H
