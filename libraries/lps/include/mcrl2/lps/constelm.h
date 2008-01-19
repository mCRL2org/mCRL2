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
#include <list>
#include <map>
#include <set>
#include <vector>
#include "mcrl2/data/data_variable_replace.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/detail/remove_parameters.h"

namespace mcrl2 {

namespace lps {

using namespace mcrl2::data;

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
      data_expression rc = r(data_variable_map_replace(i->condition(), replacements));
      if (rc == false_())
      {
        continue;
      }

      for (data_assignment_list::iterator j = i->assignments().begin(); j != i->assignments().end(); ++j)
      {
        std::map<data_variable, data_expression>::iterator k = replacements.find(j->lhs());
        if (k != replacements.end())
        {
          data_expression gj = data_variable_map_replace(j->rhs(), replacements);
          if (r(or_(not_(rc), not_equal_to(k->second, gj))) == true_())
          {
            replacements.erase(k);
            has_changed = true;
          }
        }
      }
      if (has_changed)
      {
        break;
      }
    }
  } while (has_changed == true);

  return replacements;
}

/// Returns zero or more constant process parameters of the process p with initial state init.
/// The result is returned as a map m that maps the constant parameters to their
/// constant value.
std::map<data_variable, data_expression> compute_constant_parameters_subst(const linear_process& p, data_expression_list init, rewriter& r)
{
  using namespace data_expr;
  
  typedef std::map<data_variable, std::list<rewriter::substitution>::iterator> index_map;

  // create a mapping from process parameters to initial values
  std::map<data_variable, data_expression> replacements;
  data_expression_list::iterator j = init.begin();
  for (data_variable_list::iterator i = p.process_parameters().begin(); i != p.process_parameters().end(); ++i, ++j)
  {
    replacements[*i] = *j;
  }

  // put the substitutions in a list, and make an index for it
  std::list<rewriter::substitution> substitutions;
  index_map index;
  for (std::map<data_variable, data_expression>::iterator i = replacements.begin(); i != replacements.end(); ++i)
  {
    substitutions.push_back(r.make_substitution(i->first, i->second));
    index[i->first] = --substitutions.end();
  }

  bool has_changed;
  do
  {
    has_changed = false;
    for (summand_list::iterator i = p.summands().begin(); i != p.summands().end(); ++i)
    {
      data_expression rc = r(i->condition(), substitutions.begin(), substitutions.end());

      if (rc == false_())
      {
        continue;
      }
      for (data_assignment_list::iterator j = i->assignments().begin(); j != i->assignments().end(); ++j)
      {
        index_map::iterator k = index.find(j->lhs());
        if (k != index.end())
        {
          const data_variable&   d  = j->lhs();  // process parameter
          const data_expression& g  = j->rhs();  // assigned value
          if (r(or_(not_(rc), not_equal_to(d, g)), substitutions.begin(), substitutions.end()) == true_())
          {
            replacements.erase(d);
            substitutions.erase(index[d]);
            index.erase(k);
            has_changed = true;
          }
        }
      }
      if (has_changed)
      {
        break;
      }
    }
  } while (has_changed == true);

  return replacements;
}

/// Removes zero or more constant parameters from the specification p.
template <typename Rewriter>
specification constelm(const specification& spec, Rewriter& r, bool verbose = false)
{
  std::map<data_variable, data_expression> replacements = compute_constant_parameters_subst(spec.process(), spec.initial_process().state(), r);
  std::set<data_variable> constant_parameters;
  for (std::map<data_variable, data_expression>::iterator i = replacements.begin(); i != replacements.end(); ++i)
  {
	  constant_parameters.insert(i->first);
  }

  if (verbose)
  {
    std::cout << "Removing the constant process parameters: ";
    for (std::set<data_variable>::iterator i = constant_parameters.begin(); i != constant_parameters.end(); ++i)
    {
      std::cout << pp(*i) << " ";
    }
    std::cout << std::endl;
  }

  specification result = repair_free_variables(spec);
  result = detail::remove_parameters(result, constant_parameters);

  // N.B. The replacements may only be applied to the process and the initial process!
  linear_process new_process   = data_variable_map_replace(result.process(), replacements);
  process_initializer new_init = data_variable_map_replace(result.initial_process(), replacements);
  result = set_lps(result, new_process);
  result = set_initial_process(result, new_init);

  assert(result.is_well_typed());
  return result;
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_CONSTELM_H
