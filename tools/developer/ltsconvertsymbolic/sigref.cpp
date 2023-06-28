// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "sigref.h"

#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/symbolic/bdd_util.h"
#include "mcrl2/symbolic/print.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/stack_array.h"
#include "mcrl2/utilities/stopwatch.h"

#include <sylvan_int.h>

using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::symbolic;
using namespace mcrl2::data;
using namespace mcrl2::utilities;
using namespace mcrl2::lps;
using namespace sylvan::bdds;

namespace sylvan::bdds
{

TASK_IMPL_3(BDD, sylvan_encode_block, BDDSET, block_variables, std::uint64_t, block_length, uint64_t, block_number)
{
    assert(block_number < std::pow(2, block_length)); // We can only encode it using block_length bits.

    BDD result;
    if (cache_get3(cache_encode_block_id, block_variables, block_length, block_number, &result)) return result;

    MCRL2_DECLARE_STACK_ARRAY(bl, std::uint8_t, block_length);
    for (std::uint64_t i=0; i<block_length; i++) {
        bl[i] = block_number & 1 ? 1 : 0;
        block_number>>=1;
    }

    assert(block_number == 0); // block_length must be enough to encode the block_number.

    result = sylvan_cube(block_variables, bl.data());
    cache_put3(cache_encode_block_id, block_variables, block_length, block_number, result);
    return result;
} 

TASK_IMPL_1(uint64_t, sylvan_decode_block, BDD, block)
{
    std::uint64_t result = 0;
    if (cache_get3(cache_decode_block_id, block, 0, 0, &result)) return result;

    assert(block != sylvan_false);

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

    cache_put3(cache_decode_block_id, block, 0, 0, result);
    return result;
}

TASK_IMPL_1(MTBDD, sylvan_swap_prime, MTBDD, set)
{
    if (mtbdd_isleaf(set)) return set;

    // TODO: properly ignore action/block variables (blocks are >= 100000 and action labels >= 500000)
    if (mtbdd_getvar(set) >= 99999) return set;

    MTBDD result;
    if (cache_get3(cache_swap_prime_id, set, 0, 0, &result)) return result;

    sylvan_gc_test();

    mtbdd_refs_spawn(SPAWN(sylvan_swap_prime, mtbdd_getlow(set)));
    MTBDD high = mtbdd_refs_push(CALL(sylvan_swap_prime, mtbdd_gethigh(set)));
    MTBDD low = mtbdd_refs_sync(SYNC(sylvan_swap_prime));
    result = mtbdd_makenode(sylvan_var(set)^1, low, high); // x^1 is a hacky way to convert even x in to x+1 and odd x into x-1.
    mtbdd_refs_pop(1);

    cache_put3(cache_swap_prime_id, set, 0, 0, result);
    return result;
}


} // namespace sylvan::bdds

// Print a partition encoded by <s', b>
inline
std::string print_partition(const std::vector<data_expression_index>& data_index, const sylvan::bdds::bdd& L, const sylvan::bdds::bdd& variables, const std::vector<std::uint32_t>& bits, std::uint32_t bits_for_block)
{    
  std::ostringstream out;
  auto solutions = bdd_solutions(L, variables);
  std::vector<std::string> vectors;

  for (const auto& solution : solutions)
  {
    std::vector<std::uint32_t> result;

    // First convert the states.
    int offset = 0;
    for (std::uint32_t number_of_bits : bits)
    {
      result.emplace_back(bits_to_value(solution, number_of_bits, offset));
      offset += number_of_bits;
    }

    std::uint32_t block_number = bits_to_value_lsb(solution, bits_for_block, offset);

    vectors.emplace_back(mcrl2::core::detail::print_list(ldd2state(data_index, result)) + " -> " + std::to_string(block_number));
  }

  return print_container_multiline(vectors, [](const auto& container) { return container; });
}

// Print a signature encoded by <s, a, b>
inline
std::string print_signature(const std::vector<data_expression_index>& data_index, indexed_set<multi_action> action_index, const sylvan::bdds::bdd& L, const sylvan::bdds::bdd& variables, const std::vector<std::uint32_t>& bits, std::uint32_t bits_for_action, std::uint32_t bits_for_block)
{    
  std::ostringstream out;
  auto solutions = bdd_solutions(L, variables);
  std::vector<std::string> vectors;

  for (const auto& solution : solutions)
  {
    std::vector<std::uint32_t> result;

    // First convert the states.
    int offset = 0;
    for (std::uint32_t number_of_bits : bits)
    {
      result.emplace_back(bits_to_value(solution, number_of_bits, offset));
      offset += number_of_bits;
    }

    std::uint32_t action_number = bits_to_value(solution, bits_for_action, offset);
    std::uint32_t block_number = bits_to_value_lsb(solution, bits_for_block, offset + bits_for_action);

    vectors.emplace_back(mcrl2::core::detail::print_list(ldd2state(data_index, result)) + " -> (" + pp(action_index[action_number]) + ", " + std::to_string(block_number) + ")");
  }

  return print_container_multiline(vectors, [](const auto& container) { return container; });
}

/// \brief 
/// Assumes that the relation is extended to the full domain, and for now only a single relation.
bdd signature_strong(const std::vector<bdd>& relations, const bdd& partition, const bdd& prime_variables)
{
    return and_exists(relations[0], partition, prime_variables);
}

mcrl2::lps::symbolic_lts_bdd sigref_algorithm::run(const mcrl2::lps::symbolic_lts_bdd& lts)
{
  mCRL2log(verbose) << "Extending transition relation to full domain..." << std::endl;

  // extend transition relations to the full domain.
  std::vector<sylvan::bdds::bdd> new_transition_relations;
  for (const auto& group: lts.transition_groups()) {
      new_transition_relations.emplace_back(extend_relation(group.relation, group.variables, lts.state_variables_length()));
  }

  // Compute the target state variables (also used by extend_relation implicitly)
  bdd prime_variables = swap_prime(lts.state_variables());

  // merge all transition relations into a single monolithic transition relation.
  mCRL2log(verbose) << "Merging transition relations..." << std::endl;

  std::vector<bdd> merged_transition_relation(1);
  merged_transition_relation[0] = big_union(new_transition_relations);
  new_transition_relations = merged_transition_relation;

  mCRL2log(log_level_t::debug) << "transition relation size " << print_size(new_transition_relations[0], bdd_and(bdd_and(lts.state_variables(), prime_variables), lts.action_label_variables()), true, true) << std::endl;

  // mCRL2log(log_level_t::debug) << "transition relation = " << std::endl;     
  //mCRL2log(log_level_t::debug) << print_vectors(new_transition_relations[0], bdd_and(bdd_and(lts.state_variables, prime_variables), lts.action_label_variables)) << std::endl;

  // There are at most |states| blocks since every state can be in a unique block.
  double number_of_states = satcount(lts.states(), lts.state_variables());
  prepare_blocks(base_two_bits(number_of_states));
  mCRL2log(log_level_t::debug) << "using " << m_block_length << " bits to encode blocks for " << number_of_states << " states" << std::endl;     
  
  // Assign the initial partition, assigns block to prime variables.
  bdd block = encode_block(m_block_variables, m_block_length, get_next_block());
  bdd prime_states = swap_prime(lts.states());
  bdd partition = bdd_and(prime_states, block);
  
  // The actual signature refinement algorithm for strong bisimulation
  std::size_t old_num_of_blocks = count_blocks();
  std::size_t num_of_blocks = -1;
  std::size_t iteration = 0;

  // Computes nu Z. refine(signature_strong, Z), where Z is the partition. Uses number of blocks of the partition to determine stability.
  while(num_of_blocks != old_num_of_blocks)
  {    
    ++iteration;

    stopwatch loop_start;
    old_num_of_blocks = num_of_blocks;

    // compute signature
    mCRL2log(log_level_t::debug) << "partition = " << std::endl;     
    mCRL2log(log_level_t::debug) << print_partition(lts.data_index(), partition, bdd_and(prime_variables, m_block_variables), lts.state_variable_bits(), m_block_length) << std::endl;

    bdd signature = signature_strong(new_transition_relations, partition, prime_variables);
    
    mCRL2log(log_level_t::debug) << "signature = " << std::endl;  
    mCRL2log(log_level_t::debug) << print_signature(lts.data_index(), lts.action_index(), signature, 
        bdd_and(bdd_and(lts.state_variables(), lts.action_label_variables()), m_block_variables), lts.state_variable_bits(), lts.action_label_bits(), m_block_length) << std::endl;

    partition = refine(signature, prime_variables, partition);

    num_of_blocks = count_blocks();

    mCRL2log(log_level_t::verbose) << "found " << std::setw(12) << num_of_blocks << " potential equivalence classes after " 
                               << std::setw(3) << iteration << " iterations (time = " << std::setprecision(2)
                               << std::fixed << loop_start.seconds() << "s)" << std::endl;

    sylvan::cache_clear(); // Clear the cache between iterations.
    m_block_signature.clear(); // Clear the block table.
  }

  mCRL2log(log_level_t::verbose) << "There are " << num_of_blocks << " equivalence classes." << std::endl;  

  // Compute a new transition relation for the blocks, rename 'to' states to their block number
  std::vector<mcrl2::lps::transition_group> quotient_transition_relation;
  bdd relation = and_exists(new_transition_relations[0], partition, prime_variables);

  // Rename b to new b' variables relation[B -> s'].
  bdd_substitution subst;
  for (int i=0; i < m_block_length; i++) 
  {
    subst.put(sylvan::bdds::block_variable_first_var + 2*i, sylvan::bdds::block_variable_first_var + 2*i + 1);
  }

  relation = let(subst, relation);
  relation = and_exists(relation, partition, lts.state_variables());
  quotient_transition_relation.emplace_back(relation, m_block_variables);

  //mcrl2::lps::symbolic_lts_bdd quotient_lts(lts);
  bdd new_states = and_exists(lts.states(), partition, lts.state_variables());
  bdd new_initial_state = and_exists(lts.initial_state(), partition, lts.state_variables());

  // New process has a single parameter encoding the block number
  data::variable block_parameter(mcrl2::core::identifier_string("block"), data::sort_nat::nat());
  data::variable_list process_parameters;
  process_parameters.emplace_front(block_parameter);

  // This single parameter has block_length 
  std::vector<std::uint32_t> new_bits;
  new_bits.emplace_back(m_block_length);

  // Map indices to the actual blocks
  data_expression_index block_expressions(data::sort_nat::nat());
  for (std::size_t i = 0; i < count_blocks(); ++i)
  {
    block_expressions.insert(data::data_expression());
  }
  
  std::vector<symbolic::data_expression_index> data_index;
  data_index.emplace_back(block_expressions);

  return mcrl2::lps::symbolic_lts_bdd(
    lts.data_specification(),
    process_parameters,
    new_states,
    new_initial_state,
    m_block_variables,
    lts.action_label_variables(),
    new_bits,
    lts.action_label_bits(),
    data_index,
    lts.action_index(),
    quotient_transition_relation,
    m_block_length
  );
}

/// \brief The refine procedure defined in the paper, where s and s' are the current and next states, 'a' the action and 'b' the block.
/// \details Expecting signature \sigma to encode <s, a, b>
///          Expecting vars to be conjunction of variables in s' 
///          Expecting partition P to encode <s', b>
bdd sigref_algorithm::refine(bdd signature, bdd variables, bdd partition)
{
    // This is not in the algorithm described in the paper, why?
    if (partition.is_false())
    {
        return false_();
    }

    sylvan::BDD bdd_result;
    if (sylvan::cache_get3(cache_refine_id, signature.get(), variables.get(), partition.get(), &bdd_result)) return bdd(bdd_result);

    LACE_ME;
    sylvan_gc_test();

    bdd result;
    // std::cerr << "variables = " << variables.top() << ", signature = " << signature.top() << ", partition = " << partition.top() << std::endl;

    // 2. if result := cache[(\sigma, P, iter)]: return result
    // 3. v := topVar(\sigma , P)                               # interpret s′ in P as s
    // 4. if v equals s_i for some i                            # We know that variables are increasing, so if it's empty it's outside of s_i. 
    if (!variables.is_empty()) 
    {
        // # match state in \sigma and P
        // 5. do in parallel: 
        // 6.   low := refine(\sigma_(s_i = 0), P_(s'_i = 0) 
        // 7.   high := refine(\sigma_(s_i = 1), P_(s'_i = 1)
        // 8.   result := lookupBDDnode(s'_i, low, high)

        // In reality BDDs can 'skip' variables so we need to stay for cofactors where this has happened.
        bdd signature_low = (signature.top()+1 == variables.top()) ? signature.else_() : signature;
        bdd signature_high = (signature.top()+1 == variables.top()) ? signature.then() : signature;

        bdd partition_low = (partition.top() == variables.top()) ? partition.else_() : partition;
        bdd partition_high = (partition.top() == variables.top()) ? partition.then() : partition;

        bdd low = refine(signature_low, variables.then(), partition_low);
        bdd high = refine(signature_high, variables.then(), partition_high);
        result = bdd(variables.top(), low, high);
    }
    else
    {
        // 9. else:
        // # \sigma now encodes the state signature
        // # P now encodes the previous block (the block that was currently assigned)

        // 10. B := decodeBlock(P)
        std::uint64_t B = decode_block(partition);

        // # try to claim block B if still free
        // 11. if blocks[B].sig = \bottom: 
        // 12.   cas(blocks[B].sig, \bottom, \sigma)
        // 13. if blocks[B].sig = \sigma:
        // 14.  result := P;
        auto it = m_block_signature.find(B);
        if (it == m_block_signature.end())
        {
            //std::cerr << "inserted partition " << B << std::endl;
            m_block_signature[B] = signature;
            result = partition;
        }
        else
        {
            // 15. else: 
            // 16.   B := search_or_insert(\sigma, B) 
            // 17.   result := encodeBlock(B)
            if (it->second == signature)
            {
                //std::cerr << "reused partition " << B << std::endl;
                result = partition;
            }
            else
            {
                std::uint64_t B = get_next_block();
                //std::cerr << "new block " << B << std::endl;
                m_block_signature[B] = signature;
                result = encode_block(m_block_variables, m_block_length, B);
            }
        }
    }

    sylvan::cache_put3(cache_refine_id, signature.get(), variables.get(), partition.get(), result.get());
    return bdd(result);
}