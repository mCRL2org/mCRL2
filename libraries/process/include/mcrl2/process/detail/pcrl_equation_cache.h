// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/detail/pcrl_equation_cache.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_DETAIL_PCRL_EQUATION_CACHE_H
#define MCRL2_PROCESS_DETAIL_PCRL_EQUATION_CACHE_H

#include <map>
#include "mcrl2/process/alphabet_efficient.h"
#include "mcrl2/process/utility.h"

namespace mcrl2 {

namespace process {

namespace detail {

// TODO: This mapping can probably be computed more efficiently using the SCC decomposition of the equations.
inline
std::map<process_identifier, multi_action_name_set> compute_pcrl_equation_cache(const std::vector<process_equation>& equations)
{
  std::map<process_identifier, multi_action_name_set> result;
  for (const process_equation& eqn: equations)
  {
    if (is_pcrl(eqn.expression()))
    {
      result[eqn.identifier()] = alphabet_efficient(eqn.expression(), equations);
    }
  }
  return result;
}

} // namespace detail

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_DETAIL_PCRL_EQUATION_CACHE_H
