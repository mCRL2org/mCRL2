// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL_PBES_PBES_SUMMAND_GROUP_H
#define MCRL_PBES_PBES_SUMMAND_GROUP_H

#ifdef MCRL2_ENABLE_SYLVAN

#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/pbes/srf_pbes.h"
#include "mcrl2/symbolic/summand_group.h"

#include <functional>
#include <set>
#include <vector>

namespace mcrl2::pbes_system
{

/// \brief Computes the parameters read and written for the given pbes equation.
inline
std::pair<std::set<data::variable>, std::set<data::variable>> read_write_parameters(
  const pbes_system::srf_equation& equation,
  const pbes_system::srf_summand& summand,
  const data::variable_list& process_parameters)
{
  using utilities::detail::set_union;
  using utilities::detail::set_intersection;
  using utilities::detail::as_set;

  std::set<data::variable> read_parameters = data::find_free_variables(summand.condition());
  std::set<data::variable> write_parameters;

  // We need special handling for the first parameter, since the clause 'propvar == X' is not in the condition yet
  read_parameters.insert(process_parameters.front());
  if (equation.variable().name() != summand.variable().name())
  {
    write_parameters.insert(process_parameters.front());
  }

  const data::data_expression_list& expressions = summand.variable().parameters();
  auto pi = ++process_parameters.begin(); // skip the first parameter
  auto ei = expressions.begin();
  for (; ei != expressions.end(); ++pi, ++ei)
  {
    if (*pi != *ei)
    {
      write_parameters.insert(*pi);
      data::find_free_variables(*ei, std::inserter(read_parameters, read_parameters.end()));
    }
  }

  auto process_parameter_set = as_set(process_parameters);
  return { set_intersection(read_parameters, process_parameter_set), set_intersection(write_parameters, process_parameter_set) };
}

/// \brief Computes a mapping from parameter to the index in the parameter list.
inline
std::map<data::variable, std::size_t> process_parameter_index(const data::variable_list& process_parameters)
{
  std::map<data::variable, std::size_t> result;
  std::size_t i = 0;
  for (const data::variable& v: process_parameters)
  {
    result[v] = i++;
  }
  return result;
}

/// \brief Computes a vector of bitsets where every entry of the vector corresponds to a pbes equation in the specification.
///        Given a list of parameters p_0, ..., p_n then for parameter p_i the bit 2*i is true iff p_i is read and bit 2*i+1 is true iff p_i is written based on syntactic occurrences.  
inline
std::vector<boost::dynamic_bitset<>> compute_read_write_patterns(const pbes_system::srf_pbes& pbesspec, const data::variable_list& process_parameters)
{
  std::vector<boost::dynamic_bitset<>> result;

  std::size_t n = process_parameters.size();
  std::map<data::variable, std::size_t> index = process_parameter_index(process_parameters);

  for (const detail::pre_srf_equation<false>& equation: pbesspec.equations())
  {
    for (const detail::pre_srf_summand<false>& summand: equation.summands())
    {
      auto [read_parameters, write_parameters] = read_write_parameters(equation, summand, process_parameters);
      auto read = symbolic::parameter_indices(read_parameters, index);
      auto write = symbolic::parameter_indices(write_parameters, index);
      boost::dynamic_bitset<> rw(2*n);
      for (std::size_t j: read)
      {
        rw[2*j] = true;
      }
      for (std::size_t j: write)
      {
        rw[2*j + 1] = true;
      }
      result.push_back(rw);
    }
  }

  return result;
}

/// \brief A convenience function to add the propositional variable index in front of the state vector.
inline
data::data_expression_list make_state(const pbes_system::propositional_variable_instantiation& x, const std::unordered_map<core::identifier_string, data::data_expression>& propvar_map)
{
  data::data_expression_list result = x.parameters();
  result.push_front(propvar_map.at(x.name()));
  return result;
}


struct pbes_summand_group: public symbolic::summand_group
{
  pbes_summand_group(
    const pbes_system::srf_pbes& pbesspec,
    const data::variable_list& process_parameters, // the reordered process parameters
    const std::unordered_map<core::identifier_string, data::data_expression>& propvar_map,
    const std::set<std::size_t>& summand_group_indices,
    const boost::dynamic_bitset<>& read_write_pattern,
    const std::vector<boost::dynamic_bitset<>>& read_write_patterns,
    const std::vector<std::size_t> variable_order // a permutation of [0 .. |process_parameters| - 1]
  )
    : symbolic::summand_group(process_parameters, read_write_pattern, false)
  {
    using symbolic::project;
    using utilities::detail::as_vector;
    using utilities::detail::as_set;
    using utilities::detail::set_union;
    using utilities::detail::contains;

    std::set<std::size_t> used;
    for (std::size_t j: read)
    {
      used.insert(2*j);
    }
    for (std::size_t j: write)
    {
      used.insert(2*j + 1);
    }

    const auto& equations = pbesspec.equations();
    std::size_t k = 0;
    for (const auto& equation : equations)
    {
      const core::identifier_string& X_i = equation.variable().name();
      const auto& equation_summands = equation.summands();
      for (std::size_t j = 0; j < equation_summands.size(); j++, k++)
      {
        if (contains(summand_group_indices, k))
        {
          std::vector<int> copy;
          for (std::size_t q: used)
          {
            bool b = read_write_patterns[k][q];
            copy.push_back(b ? 0 : 1);
          }
          const pbes_system::srf_summand& smd = equation_summands[j];
          auto next_state = make_state(smd.variable(), propvar_map);
          next_state = symbolic::permute_copy(next_state, variable_order);
          summands.emplace_back(data::and_(data::equal_to(process_parameters.front(), propvar_map.at(X_i)), smd.condition()), smd.parameters(), project(as_vector(next_state), write), copy);
        }
      }
    }
  }
};

} // namespace mcrl2::pbes_system

#endif // MCRL2_ENABLE_SYLVAN

#endif // MCRL_PBES_PBES_SUMMAND_GROUP_H