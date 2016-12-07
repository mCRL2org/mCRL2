// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/merge_action_specifications.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_MERGE_ACTION_SPECIFICATIONS_H
#define MCRL2_PROCESS_MERGE_ACTION_SPECIFICATIONS_H

#include <set>
#include "mcrl2/process/action_label.h"

namespace mcrl2 {

namespace process {

/// \brief Merges two action specifications.
inline
action_label_list merge_action_specifications(const action_label_list& actspec1, const action_label_list& actspec2)
{
  std::set<action_label> result;
  result.insert(actspec1.begin(), actspec1.end());
  result.insert(actspec2.begin(), actspec2.end());
  return action_label_list(result.begin(), result.end());
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_MERGE_ACTION_SPECIFICATIONS_H
