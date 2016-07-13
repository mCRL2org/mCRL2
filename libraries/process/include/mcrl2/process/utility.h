// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/utility.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_UTILITY_H
#define MCRL2_PROCESS_UTILITY_H

#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/data/substitutions/assignment_sequence_substitution.h"
#include "mcrl2/process/find.h"
#include "mcrl2/process/multi_action_name.h"
#include "mcrl2/process/print.h"
#include "mcrl2/process/replace.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/sequence.h"
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2 {

namespace process {

/// \brief Returns true if x is a pCRL expression. N.B. This test depends on the assumption that
/// in mCRL2 a top level pCRL expression may never contain a non-pCRL expression.
inline
bool is_pcrl(const process_expression& x)
{
  return !is_merge(x) && !is_left_merge(x) && !is_sync(x) && !is_hide(x) && !is_rename(x) && !is_block(x) && !is_allow(x) && !is_comm(x);
}

inline
bool contains_tau(const multi_action_name_set& A)
{
  multi_action_name tau;
  return A.find(tau) != A.end();
}

inline
process_expression make_sync(const process_expression& x, const process_expression& y)
{
  if (is_delta(x) || is_delta(y))
  {
    return delta();
  }
  return sync(x, y);
}

inline
process_expression make_merge(const process_expression& x, const process_expression& y)
{
  if (is_delta(x) && is_delta(y))
  {
    return delta();
  }
  return merge(x, y);
}

inline
process_expression make_left_merge(const process_expression& x, const process_expression& y)
{
  if (is_delta(y))
  {
    return delta();
  }
  return left_merge(x, y);
}

inline
process_expression make_allow(const multi_action_name_set& A, const process_expression& x)
{
  if (A.empty())
  {
    return delta();
  }

  // convert A to an action_name_multiset_list B
  std::vector<action_name_multiset> v;
  for (const multi_action_name& alpha: A)
  {
    if (!alpha.empty()) // exclude tau
    {
      v.push_back(action_name_multiset(core::identifier_string_list(alpha.begin(), alpha.end())));
    }
  }
  action_name_multiset_list B(v.begin(), v.end());

  return B.empty() ? x : allow(B, x);
}

inline
process_expression make_comm(const communication_expression_list& C, const process_expression& x)
{
  if (C.empty())
  {
    return x;
  }
  else
  {
    return comm(C, x);
  }
}

inline
process_expression make_hide(const core::identifier_string_list& I, const process_expression& x)
{
  if (I.empty())
  {
    return x;
  }
  else
  {
    return hide(I, x);
  }
}

inline
process_expression make_block(const core::identifier_string_list& B, const process_expression& x)
{
  if (B.empty())
  {
    return x;
  }
  else
  {
    return block(B, x);
  }
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_UTILITY_H
