// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/lps/symbolic_lts_bdd.h"

#include "mcrl2/symbolic/print.h"
#include "mcrl2/symbolic/utility.h"
#include "mcrl2/symbolic/bdd_util.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/unordered_set.h"

#include <vector>

using namespace mcrl2::lps;
using namespace sylvan::ldds;
using namespace sylvan::bdds;
using mcrl2::log::log_level_t;

std::uint32_t compute_highest_action_rec(ldd set, ldd meta, mcrl2::utilities::unordered_set<ldd>& cache)
{
    if (set == empty_set() || set == empty_list()) return 0;

    if (cache.count(set) != 0) {
        return 0; // Can return zero since this value will be returned somewhere else, and we are interested in the max.
    }

    std::uint32_t highest = 0;
    highest = std::max(highest, compute_highest_action_rec(set.right(), meta, cache));

    if (meta.value() == 5)
    {
        highest = std::max(highest, set.value() + 1); // Count zero as additional value 
    }
    else if (meta.value() == 0)
    {
        // Not in the relation, so don't move down in set.
        highest = std::max(highest, compute_highest_action_rec(set, meta.down(), cache));
    }
    else
    {
        highest = std::max(highest, compute_highest_action_rec(set.down(), meta.down(), cache));
    }

    cache.emplace(set);
    return highest;
}

/// Compute the highest value for the action label (meta = 5)
std::uint32_t compute_highest_action(ldd set, ldd meta)
{
    mcrl2::utilities::unordered_set<ldd> cache;
    return compute_highest_action_rec(set, meta, cache);
}

std::vector<std::uint32_t> project_bits(const std::vector<std::uint32_t>& bits, const std::vector<std::size_t>& read, const std::vector<std::size_t>& write)
{
  std::vector<std::uint32_t> result;

  std::uint32_t max_value = 0;
  for (std::uint32_t value: read)
  {
    max_value = std::max(max_value, value);
  }
  for (std::uint32_t value: write)
  {
    max_value= std::max(max_value, value);
  }

  for (std::uint32_t i = 0; i <= max_value; ++i)
  {
    result.emplace_back(bits[i]);
  }

  return result;
}

symbolic_lts_bdd::symbolic_lts_bdd(const symbolic_lts& lts)
{  
  m_data_index = lts.data_index();
  m_action_index = lts.action_index();

  mCRL2log(log_level_t::debug) << "Height:" << sylvan::ldds::compute_height(lts.states()) <<  std::endl;

  // Compute highest value at each level (from reachable states)
  std::vector<uint32_t> highest = sylvan::ldds::compute_highest(lts.states());
  {
    mCRL2log(log_level_t::debug) << "State values per level:" << std::endl;
    std::size_t i = 0;
    for (const uint32_t value: highest)
    {
        mCRL2log(log_level_t::debug) << i << ": " << value << std::endl; 
        ++i;
    }
  }

  // Compute highest action label value (from transition relations)
  uint32_t highest_action = 0;
  for (const auto& group: lts.summand_groups()) 
  {
    highest_action = std::max(highest_action, compute_highest_action(group.L, group.Ir));
  }

  assert(highest_action == lts.action_index().size()); // Should match.
  
  mCRL2log(log_level_t::debug) << "Number of action label indices: " << highest_action << std::endl;
  mCRL2log(log_level_t::debug) << "Bits per level:" << std::endl;

  // Compute number of bits for each level
  m_bits = compute_bits(highest);
  for (int i = 0; i < m_bits.size(); ++i)
  {    
    mCRL2log(log_level_t::debug) << i << ": " << m_bits[i] << std::endl; 
  }

  // Includes the number of bits required to store the action label.
  m_bits_action_label = base_two_bits(highest_action);
  mCRL2log(log_level_t::debug) << "Bits for action label:" << m_bits_action_label << std::endl;
  
  // Compute the variable names for each level.
  std::vector<uint32_t> variables;

  /// bdd_from_ldd assumes that even bits are the current state variables, this
  /// is necessary for interleaving with the next state variables since variable
  /// must be increasing (and variables are natural numbers).
  /// Furthermore, it is assumed that the first variable is zero, indicated by the zero passed below.
  int i = 0;
  for (uint32_t val: m_bits)
  {
    for (uint32_t k = 0; k < val; ++k)
    {
      variables.push_back(i*2);
      ++i;
    }
  }
  
  // All of the state variables.
  m_state_variables = sylvan::bdds::cube(variables);

  mCRL2log(log_level_t::debug) << symbolic::print_vectors(m_state_variables, m_state_variables) << std::endl;
  mCRL2log(log_level_t::debug) << "Convert states from LDD to BDD..." << std::endl;
  m_states = bdd_from_ldd(lts.states(), m_bits, 0); 
  
  mCRL2log(log_level_t::debug) << symbolic::print_states(lts.data_index(), m_states, m_state_variables, m_bits) << std::endl;
  mCRL2log(log_level_t::debug) << "state space LDD size " << symbolic::print_size(lts.states(), true, true) << " to BDD size " << symbolic::print_size(m_states, m_state_variables, true, true) << std::endl;
  
  std::vector<uint32_t> action_variables;
  for (std::size_t i = 0; i < m_bits_action_label; ++i)
  {
    action_variables.emplace_back(action_first_var + i);
  }
  
  m_action_label_variables = sylvan::bdds::cube(action_variables);

  auto variables_vector = bdd_variables(m_action_label_variables, m_action_label_variables);
  assert(variables_vector.size() == 1); // Should be a singleton

  // Convert the transition relation from LDD to BDD.
  for (const auto& group: lts.summand_groups())
  {
    std::vector<std::uint32_t> bits_with_action_label = project_bits(m_bits, group.read, group.write);
    bits_with_action_label.emplace_back(m_bits_action_label);

    sylvan::bdds::bdd variables = meta_to_bdd(group.Ir, m_bits, 0);
    sylvan::bdds::bdd relation = bdd_from_ldd_rel(group.L, bits_with_action_label, 0, group.Ir);   

    //mCRL2log(log_level_t::debug) << symbolic::print_relation(lts.data_index, lts.action_index, group.L, group.read, group.write) << std::endl;
    //mCRL2log(log_level_t::debug) << symbolic::print_relation(lts.data_index, lts.action_index, relation, sylvan::bdds::bdd_and(variables, action_label_variables), bits, bits_action_label, group.read, group.write) << std::endl;    
    
    m_transitions.emplace_back(relation, variables);
    mCRL2log(log_level_t::debug) << "transition relation LLD size " << symbolic::print_size(group.L, true, true) 
      << " to BDD size " << symbolic::print_size(relation, sylvan::bdds::bdd_and(variables, m_action_label_variables), true, true) << std::endl;
  }

  m_state_variables_length = variables.size();
}

symbolic_lts_bdd::symbolic_lts_bdd(data::data_specification data_spec,
  data::variable_list process_parameters,
  bdd states,
  bdd initial_state,
  bdd state_variables,
  bdd action_label_variables,
  std::vector<std::uint32_t> bits,
  std::uint32_t bits_action_label,
  std::vector<symbolic::data_expression_index>,
  utilities::indexed_set<lps::multi_action>,
  std::vector<transition_group>,
  std::size_t m_state_variables_length) :
    m_data_spec(data_spec),
    m_process_parameters(process_parameters),
    m_states(states),
    m_initial_state(initial_state),
    m_state_variables(state_variables),
    m_action_label_variables(action_label_variables),
    m_bits(bits),
    m_bits_action_label(bits_action_label)
{
  //assert(m_process_parameters.size() == )
}