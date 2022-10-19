// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "symbolic_lts_bdd.h"

#include "bdd_util.h"

#include "mcrl2/symbolic/print.h"
#include "mcrl2/symbolic/utility.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/unordered_set.h"

#include <vector>

using namespace mcrl2::lps;
using namespace sylvan::ldds;
using namespace sylvan::bdds;
using mcrl2::log::log_level_t;

/// Compute the height of the LDD.
std::uint32_t compute_height(ldd set)
{
    std::uint32_t height = 0;

    ldd current = set;
    while (current != empty_set())
    {
        ++height;
        current = current.down();
    }

    return height;
}

void compute_highest_rec(ldd set, mcrl2::utilities::unordered_set<ldd>& cache, std::vector<uint32_t>& array, std::size_t index)
{   
    if (set == empty_set() || set == empty_list()) return;

    if (cache.count(set) != 0) {
        return;
    }
    cache.emplace(set);

    compute_highest_rec(set.right(), cache, array, index);
    compute_highest_rec(set.down(), cache, array, index+1);

    if (!set.is_copy()) {
        array[index] = std::max(array[index], set.value());
    }
}

/// Compute the highest value for each variable level.
std::vector<std::uint32_t> compute_highest(ldd set)
{
    std::vector<std::uint32_t> array(compute_height(set));
    mcrl2::utilities::unordered_set<ldd> cache;
    compute_highest_rec(set, cache, array, 0);
    return array;
}

std::uint32_t compute_highest_action_rec(ldd set, ldd meta, mcrl2::utilities::unordered_set<ldd>& cache)
{
    if (set == empty_set() || set == empty_list()) return 0;

    if (cache.count(set) != 0) {
        return 0; // Can return zero since this value will be returned somewhere else, and we are interested in the max.
    }
    cache.emplace(set);

    std::uint32_t highest = 0;
    highest = std::max(highest, compute_highest_action_rec(set.right(), meta, cache));

    if (meta.value() == 5)
    {
        return std::max(highest, set.value());
    }
    else
    {
        return std::max(highest, compute_highest_action_rec(set.down(), meta.down(), cache));
    }
}

/// Compute the highest value for the action label (meta = 5)
std::uint32_t compute_highest_action(ldd set, ldd meta)
{
    mcrl2::utilities::unordered_set<ldd> cache;
    return compute_highest_action_rec(set, meta, cache);
}

symbolic_lts_bdd::symbolic_lts_bdd(const symbolic_lts& lts)
{  
  mCRL2log(log_level_t::debug) << "Height:" << compute_height(lts.states) <<  std::endl;

  // Compute highest value at each level (from reachable states)
  std::vector<uint32_t> highest = compute_highest(lts.states);
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
  for (const auto& group: lts.summand_groups) {
      highest_action = std::max(highest_action, compute_highest_action(group.L, group.Ir));
  }
  
  mCRL2log(log_level_t::debug) << "Number of action label indices: " << highest_action << std::endl;

  // Compute number of bits for each level
  std::vector<uint32_t> bits(highest.size());
  {
    std::size_t i = 0;
    for (uint32_t& value: highest)
    {
        bits[i] = std::log(value) + 1;
        ++i;
    }
  }
  
  // Compute the variable names for each level.
  std::vector<uint32_t> variables;

  /// bdd_from_ldd assumes that even bits are the current state variables, this
  /// is necessary for interleaving with the next state variables since variable
  /// indices must be increasing.
  /// Furthermore, it is assumed that the first variable is zero, indicated by the zero passed below.
  int i = 0;
  for (uint32_t val: bits)
  {
    for (uint32_t k = 0; k < val; ++k)
    {
      variables.push_back(i*2);
      ++i;
    }
  }

  // All of the state variables.
  state_variables = sylvan::bdds::cube(variables);

  mCRL2log(log_level_t::debug) << symbolic::print_vectors(state_variables, state_variables) << std::endl;
  mCRL2log(log_level_t::debug) << "Convert states from LDD to BDD..." << std::endl;
  states = bdd_from_ldd(lts.states, bits, 0); 

  mCRL2log(log_level_t::debug) << "LDD size " << symbolic::print_size(lts.states, true) << std::endl;
  mCRL2log(log_level_t::debug) << "BDD size " << symbolic::print_size(states, state_variables, true) << std::endl;

  // Convert the transition relation from LDD to BDD.
  for (const auto& group: lts.summand_groups)
  {
    sylvan::bdds::bdd variables = meta_to_bdd(group.Ir, bits, 0);
    sylvan::bdds::bdd relation = bdd_from_ldd(group.L, bits, 0);
    
    transitions.emplace_back(relation, variables);
  }

  m_state_variables_length = variables.size();
}