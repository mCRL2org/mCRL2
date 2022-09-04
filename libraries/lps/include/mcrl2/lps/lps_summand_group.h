// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL_LPS_LPS_SUMMAND_GROUP_H
#define MCRL_LPS_LPS_SUMMAND_GROUP_H

#ifdef MCRL2_ENABLE_SYLVAN

#include "mcrl2/data/data_expression.h"
#include "mcrl2/lps/action_summand.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/symbolic/summand_group.h"
#include "mcrl2/symbolic/utility.h"

#include <functional>
#include <set>

namespace mcrl2::lps
{

/// \brief Computes a vector of bitsets where every entry of the vector corresponds to a summand in the specification.
///        Given a list of parameters p_0, ..., p_n then for parameter p_i the bit 2*i is true iff p_i is read and bit 2*i+1 is true iff p_i is written based on syntactic occurrences.
inline std::vector<boost::dynamic_bitset<>> compute_read_write_patterns(const lps::specification& lpsspec);

struct lps_summand_group: public symbolic::summand_group
{
  lps_summand_group(
    const lps::specification& lpsspec,
    const data::variable_list& process_parameters, // the reordered process parameters
    const std::set<std::size_t>& group_indices,
    const boost::dynamic_bitset<>& group_pattern,
    const std::vector<boost::dynamic_bitset<>>& read_write_patterns,
    const std::vector<std::size_t> variable_order // a permutation of [0 .. |process_parameters| - 1]
  )
    : summand_group(process_parameters, group_pattern, true)
  {
    using symbolic::project;
    using utilities::detail::as_vector;
    using utilities::detail::as_set;
    using utilities::detail::set_union;

    std::set<std::size_t> used;
    for (std::size_t j: read)
    {
      used.insert(2*j);
    }
    for (std::size_t j: write)
    {
      used.insert(2*j + 1);
    }

    const auto& lps_summands = lpsspec.process().action_summands();
    for (std::size_t i: group_indices)
    {
      std::vector<int> copy;
      for (std::size_t j: used)
      {
        bool b = read_write_patterns[i][j];
        copy.push_back(b ? 0 : 1);
      }
      copy.push_back(0); // Add one additional copy for the action label (needed by union_cube_copy).

      const auto& smd = lps_summands[i];
      actions.emplace_back(smd.multi_action());
      summands.emplace_back(smd.condition(), smd.summation_variables(), project(as_vector(symbolic::permute_copy(smd.next_state(lpsspec.process().process_parameters()), variable_order)), write), copy);
    }
  }

  lps_summand_group(const data::variable_list& process_parameters, std::vector<data::variable> _read_parameters, std::vector<data::variable> _write_parameters)
    : summand_group(process_parameters, _read_parameters, _write_parameters, true)
  {}
  
  std::vector<lps::multi_action> actions;
};

namespace 
{

/// \brief Computes the read and written process parameters for the given summand
inline
std::pair<std::set<data::variable>, std::set<data::variable>> read_write_parameters(const lps::action_summand& summand, const std::set<data::variable>& process_parameters)
{
  using utilities::detail::set_union;
  using utilities::detail::set_intersection;

  // TODO: multi-action free variables are only necessary when actions are rewritten.
  std::set<data::variable> read_parameters = set_union(data::find_free_variables(summand.condition()), lps::find_free_variables(summand.multi_action()));
  std::set<data::variable> write_parameters;

  for (const auto& assignment: summand.assignments())
  {
    if (assignment.lhs() != assignment.rhs())
    {
      write_parameters.insert(assignment.lhs());
      data::find_free_variables(assignment.rhs(), std::inserter(read_parameters, read_parameters.end()));
    }
  }

  return { set_intersection(read_parameters, process_parameters), set_intersection(write_parameters, process_parameters) };
}

/// \brief Assigns a unique index to every parameter of the process.
inline
std::map<data::variable, std::size_t> process_parameter_index(const lps::specification& lpsspec)
{
  std::map<data::variable, std::size_t> result;
  std::size_t i = 0;
  for (const data::variable& v: lpsspec.process().process_parameters())
  {
    result[v] = i++;
  }
  return result;
}

} // internal

std::vector<boost::dynamic_bitset<>> compute_read_write_patterns(const lps::specification& lpsspec)
{
  using utilities::detail::as_set;

  std::vector<boost::dynamic_bitset<>> result;

  auto process_parameters = as_set(lpsspec.process().process_parameters());
  std::size_t n = process_parameters.size();
  std::map<data::variable, std::size_t> index = process_parameter_index(lpsspec);

  for (const auto& summand: lpsspec.process().action_summands())
  {
    auto [read_parameters, write_parameters] = read_write_parameters(summand, process_parameters);
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

  return result;
}

} // namespace mcrl2::lps

#endif // MCRL2_ENABLE_SYLVAN

#endif // MCRL_LPS_LPS_SUMMAND_GROUP_H