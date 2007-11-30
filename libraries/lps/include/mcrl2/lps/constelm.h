// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/constelm.h
/// \brief Add your file description here.

#ifndef MCRL2_LPS_CONSTELM_H
#define MCRL2_LPS_CONSTELM_H

#include <algorithm>
#include <map>
#include <set>
#include <vector>
#include "mcrl2/data/data_variable_replace.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/detail/remove_parameters.h"

namespace lps {

/// Returns zero or more constant process parameters of the process p with initial state init.
/// The result is returned as a map m that maps the constant parameters to their
/// constant value.
template <typename Rewriter>
std::map<data_variable, data_expression> compute_constant_parameters(const linear_process& p, data_expression_list init, Rewriter& r)
{
  using namespace data_expr;
  
  std::map<data_variable, data_expression> replacements;
  data_variable_list::iterator i = p.process_parameters().begin();
  data_expression_list::iterator j = init.begin();
  for ( ; i != p.process_parameters().end(); ++i, ++j)
  {
    replacements[*i] = r(*j);
  }

  bool has_changed;
  do
  {
    has_changed = false;
    for (summand_list::iterator i = p.summands().begin(); i != p.summands().end(); ++i)
    {
      data_expression c = data_variable_map_replace(i->condition(), replacements);
      if (r(c) == false_())
      {
        continue;
      }

      for (data_assignment_list::iterator j = i->assignments().begin(); j != i->assignments().end(); ++j)
      {
        std::map<data_variable, data_expression>::iterator k = replacements.find(j->lhs());
        if (k != replacements.end())
        {
          data_expression gj = data_variable_map_replace(j->rhs(), replacements);
          if (k->second != r(gj))
          {
            replacements.erase(k);
            has_changed = true;
          }
        }
      }
      // if (has_changed) { break; }
    }
  } while (has_changed == true);

  return replacements;
}

/// Removes zero or more constant parameters from the specification p.
template <typename Rewriter>
specification constelm(const specification& spec, Rewriter& r)
{
  std::map<data_variable, data_expression> replacements = compute_constant_parameters(spec.process(), spec.initial_process().state(), r);

  std::set<data_variable> constant_parameters;
  for (std::map<data_variable, data_expression>::iterator i = replacements.begin(); i != replacements.end(); ++i)
  {
    constant_parameters.insert(i->first);
  }
  specification result = detail::remove_parameters(spec, constant_parameters);
  result = data_variable_map_replace(result, replacements);
  assert(result.is_well_typed());
  return result;
}

} // namespace lps

#endif // MCRL2_LPS_CONSTELM_H
