// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "sigref.h"

#include "bdd_util.h"
#include "mcrl2/symbolic/print.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/stack_array.h"

#include <sylvan_int.h>

using namespace mcrl2::log;
using namespace mcrl2::symbolic;
using namespace sylvan::bdds;

namespace sylvan::bdds
{

TASK_IMPL_3(BDD, sylvan_encode_block, BDDSET, block_variables, std::uint64_t, block_length, uint64_t, block_number)
{
    BDD result;
    if (cache_get3(CACHE_ENCODE_BLOCK, block_variables, block_length, block_number, &result)) return result;

    // for now, assume max 64 bits for a block....
    MCRL2_DECLARE_STACK_ARRAY(bl, std::uint8_t, block_length);
    for (std::uint64_t i=0; i<block_length; i++) {
        bl[i] = block_number & 1 ? 1 : 0;
        block_number>>=1;
    }

    result = sylvan_cube(block_variables, bl.data());
    cache_put3(CACHE_ENCODE_BLOCK, block_variables, block_length, block_number, result);
    return result;
} 

TASK_IMPL_1(uint64_t, sylvan_decode_block, BDD, block)
{
    std::uint64_t result = 0;
    if (cache_get3(CACHE_DECODE_BLOCK, block, 0, 0, &result)) return result;

    std::uint64_t mask = 1;
    while (block != sylvan_true) {
        BDD b_low = sylvan_low(block);
        if (b_low == sylvan_false) {
            result |= mask;
            block = sylvan_high(block);
        } else {
            block = b_low;
        }
        mask <<= 1;
    }

    cache_put3(CACHE_DECODE_BLOCK, block, 0, 0, result);
    return result;
}

TASK_IMPL_1(MTBDD, sylvan_swap_prime, MTBDD, set)
{
    if (mtbdd_isleaf(set)) return set;

    // TODO: properly ignore action/block variables
    if (mtbdd_getvar(set) >= 99999) return set;

    MTBDD result;
    if (cache_get3(CACHE_SWAPPRIME, set, 0, 0, &result)) return result;

    sylvan_gc_test();

    mtbdd_refs_spawn(SPAWN(sylvan_swap_prime, mtbdd_getlow(set)));
    MTBDD high = mtbdd_refs_push(CALL(sylvan_swap_prime, mtbdd_gethigh(set)));
    MTBDD low = mtbdd_refs_sync(SYNC(sylvan_swap_prime));
    result = mtbdd_makenode(sylvan_var(set)^1, low, high);
    mtbdd_refs_pop(1);

    cache_put3(CACHE_SWAPPRIME, set, 0, 0, result);
    return result;
}


} // namespace sylvan::bdds

/// \brief 
/// Assumes that the relation is extended to the full domain, and for now only a single relation.
bdd signature_strong(const std::vector<bdd>& relations, const bdd& partition, const bdd& prime_variables)
{
    return and_exists(relations[0], partition, prime_variables);
}

void sigref_algorithm::run(const mcrl2::lps::symbolic_lts_bdd& lts)
{
  mCRL2log(verbose) << "Extending transition relation to full domain..." << std::endl;

  // extend transition relations to the full domain.
  std::vector<sylvan::bdds::bdd> new_transition_relations;
  for (const auto& group: lts.transitions) {
      new_transition_relations.emplace_back(extend_relation(group.relation, group.variables, lts.state_variables_length()));
  }

  // extend_relation uses odd variables to encode the target state.
  std::vector<std::uint32_t> prime_variables_vector(lts.state_variables_length());
  for (std::size_t i = 0; i < lts.state_variables_length(); ++i)
  {
    prime_variables_vector[i] = 2*i + 1;
  }

  bdd prime_variables = cube(prime_variables_vector);

  // merge all transition relations into a single monolithic transition relation.
  mCRL2log(verbose) << "Merging transition relations..." << std::endl;

  std::vector<bdd> merged_transition_relation(1);
  merged_transition_relation[0] = big_union(new_transition_relations);
  new_transition_relations = merged_transition_relation;

  mCRL2log(log_level_t::debug) << "transition relation size " << print_size(new_transition_relations[0], bdd_and(lts.state_variables, prime_variables), true) << std::endl;

  // There are at most 2^|state_vector| + 1 blocks since every state can be in a unique block.
  prepare_blocks(lts.state_variables_length()+1);

  // Assign the initial partition, assigns block to prime variables.
  bdd block = encode_block(m_block_variables, m_block_length, get_next_block());
  bdd prime_states = swap_prime(lts.states);
  bdd partition = bdd_and(prime_states, block);

  // The actual signature refinement algorithm for strong bisimulation
  std::size_t old_num_of_blocks = count_blocks();
  std::size_t num_of_blocks = -1;
  std::size_t iteration = 0;

  while(num_of_blocks != old_num_of_blocks)
  {    
    old_num_of_blocks = num_of_blocks;

    // compute signature
    bdd signature = signature_strong(new_transition_relations, partition, prime_variables);

    partition = refine(signature, lts.state_variables, partition);

    num_of_blocks = count_blocks();

    mCRL2log(log_level_t::verbose) << "iteration " << iteration << " blocks " << num_of_blocks << std::endl;
    ++iteration;
  }
  
}

bdd sigref_algorithm::refine(bdd signature, bdd variables, bdd partition)
{
    return bdd();
}