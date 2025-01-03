// Author(s): Jan Friso Groote, Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/linearise_utilities.h
/// \brief Utilities used in linearisation. Mainly for actions and multiactions.

#ifndef MCRL2_LPS_LINEARISE_UTILITY_H
#define MCRL2_LPS_LINEARISE_UTILITY_H

#include "mcrl2/process/process_expression.h"


namespace mcrl2
{

namespace lps
{

/// Determine if a1 < a2; the key requirement is that orderings of action labels and the actions in multiactions are
/// consistent.
bool action_label_compare(const process::action_label& a1, const process::action_label& a2)
{
  /* first compare the strings in the actions */
  if (std::string(a1.name())<std::string(a2.name()))
  {
    return true;
  }

  if (a1.name()==a2.name())
  {
    /* the strings are equal; the sorts are used to
       determine the ordering */
    return a1.sorts()<a2.sorts();
  }

  return false;
}

  /// Determine if a1 < a2; the key requirement is that orderings of action labels and the actions in multiactions are
/// consistent.
bool action_compare(const process::action& a1, const process::action& a2)
{
  return action_label_compare(a1.label(), a2.label());
}

} // namespace lps

} // namespace mcrl2



#endif // MCRL2_LPS_LINEARISE_RENAME_H
