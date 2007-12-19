// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/parelm.h
/// \brief Add your file description here.

#ifndef MCRL2_LPS_PARELM_H
#define MCRL2_LPS_PARELM_H

#include <algorithm>
#include <map>
#include <set>
#include <vector>
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/detail/data_assignment_functional.h"
#include "mcrl2/lps/mcrl22lps.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/detail/sorted_sequence_algorithm.h"
#include "mcrl2/lps/detail/remove_parameters.h"

namespace lps {

/// returns the data variables that appear in the condition, action or time of
/// the summands in the sequence [first, last[
template <typename Iterator>
std::set<data_variable> transition_variables(Iterator first, Iterator last)
{
  std::set<data_variable> result;
  for (Iterator i = first; i != last; ++i)
  {
    if (i->is_delta())
    {
      continue;
    }
    atermpp::find_all_if(i->condition(), is_data_variable, std::inserter(result, result.end()));   
    atermpp::find_all_if(i->actions(), is_data_variable, std::inserter(result, result.end()));   
    if (i->has_time())
    {
      atermpp::find_all_if(i->time(), is_data_variable, std::inserter(result, result.end()));
    }
  }
  return result;
}

/// Returns a set of insignificant process parameters that may be eliminated from p.
inline
std::set<data_variable> compute_insignificant_parameters(const linear_process& p)
{
  std::set<data_variable> process_parameters(p.process_parameters().begin(), p.process_parameters().end());

  // significant variables may not be removed by parelm
  std::set<data_variable> significant_variables = transition_variables(p.summands().begin(), p.summands().end());

  // recursively extend the set of significant variables
  std::set<data_variable> todo = significant_variables;
  while (!todo.empty())
  {
    data_variable x = *todo.begin();
    todo.erase(todo.begin());

    for (summand_list::iterator i = p.summands().begin(); i != p.summands().end(); ++i)
    {
      if (!i->is_delta())
      {
        data_assignment_list::iterator j = std::find_if(i->assignments().begin(), i->assignments().end(), detail::has_left_hand_side(x));
        if (j != i->assignments().end())
        {
          std::set<data_variable> new_variables = detail::set_difference(find_variables(j->rhs()), significant_variables);
          todo.insert(new_variables.begin(), new_variables.end());
          significant_variables.insert(new_variables.begin(), new_variables.end());
        }
      }
    }
  }
  return detail::set_difference(process_parameters, significant_variables);
}

/// Removes zero or more insignificant parameters from the specification spec.
inline
specification parelm(const specification& spec)
{
  std::set<data_variable> to_be_removed = compute_insignificant_parameters(spec.process());
    
  // logging statements
  gsVerboseMsg("Parelm removed %d process parameters: ", to_be_removed.size());
  for (std::set<data_variable>::iterator i = to_be_removed.begin(); i != to_be_removed.end(); ++i)
  {
    gsVerboseMsg("%s:%s ", pp(*i).c_str(), pp(i->sort()).c_str());
  }
  gsVerboseMsg("\n");
    
  specification result = detail::remove_parameters(spec, to_be_removed);
  assert(result.is_well_typed());
  return result;
}

} // namespace lps

#endif // MCRL2_LPS_PARELM_H
