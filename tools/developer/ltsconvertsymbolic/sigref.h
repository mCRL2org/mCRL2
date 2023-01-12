// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_LPS_SIGREF_H
#define MCRL2_LPS_SIGREF_H

#include "mcrl2/lps/symbolic_lts_bdd.h"
#include "mcrl2/utilities/exception.h"

#include <sylvan.h>
#include <sylvan_bdd.hpp>

namespace sylvan::bdds
{
    
// The cache ids for the encode and decode block operations.
static uint64_t cache_encode_block_id;
static uint64_t cache_decode_block_id;
static uint64_t cache_swap_prime_id;
static uint64_t cache_refine_id;

const std::size_t block_variable_first_var = 2000000; // Must be higher than action_first_var + maximum bits required to encode the action.

/// Encode a block number (max 64bit) as a BDD singleton set where the bits represent the block number in base 2.
TASK_DECL_3(BDD, sylvan_encode_block, BDDSET, std::uint64_t, std::uint64_t);
#define sylvan_encode_block(vars, block_length, block_number) CALL(sylvan_encode_block, vars, block_length, block_number)

/// The inverse of sylvan_encode_block.
TASK_DECL_1(std::uint64_t, sylvan_decode_block, BDD);
#define sylvan_decode_block(block) CALL(sylvan_decode_block, block)

/// \brief Substitute each s by s' and vice versa.
/// \details Assumes that s and s' are all variables less than 99999 (in sylvan variables have natural numbers as names)
TASK_DECL_1(BDD, sylvan_swap_prime, BDD);
#define sylvan_swap_prime(set) CALL(sylvan_swap_prime, set)

inline
bdd encode_block(bdd variables, std::uint64_t block_size, std::uint64_t block_number)
{
    LACE_ME;
    return bdd(sylvan_encode_block(variables.get(), block_size, block_number));
}

inline
std::uint64_t decode_block(bdd block)
{
    LACE_ME;
    return sylvan_decode_block(block.get());
}

inline
bdd swap_prime(bdd set)
{
    LACE_ME;
    return bdd(sylvan_swap_prime(set.get()));
}

} // namespace sylvan::bdds

/// \brief Implements the partition refinement algorithm described in "Multi-core symbolic bisimulation minimisation" by Tom van Dijk and
///        Jaco van de Pol. Returns a minimised symbolic LTS.
class sigref_algorithm
{
public:
    mcrl2::lps::symbolic_lts_bdd run(const mcrl2::lps::symbolic_lts_bdd& lts);

private:
    using bdd = sylvan::bdds::bdd;

    std::size_t m_next_block = 0;
    int         m_block_length = 0; // number of block variables
    bdd         m_block_variables;

    mcrl2::utilities::unordered_map<std::uint64_t, bdd> m_block_signature;

    /// \brief Returns the next free block number.
    size_t
    get_next_block()
    {
        if (m_next_block + 1 % 10000 == 0)
        {
            std::cerr << "Created " << m_next_block + 1 << " blocks" << std::endl;
        }

        return m_next_block++;
    }

    void prepare_blocks(int block_length)
    {
        // TODO: in the original code there is the assumption that this could be at most 25, which makes it already unsuitable
        // for examples such as WMS. Figure out if dropping this leads to problems.
        //if (block_length > 25)
        //{
        //    throw mcrl2::runtime_error("At most 2^25 blocks allowed");
        //}

        m_block_length = block_length;
        std::vector<uint32_t> variables(block_length);
        for (int i=0; i<block_length; i++) 
        {
            variables[i] = sylvan::bdds::block_variable_first_var+2*i;
        }

        m_block_variables = sylvan::bdds::cube(variables);
    }
    
    size_t
    count_blocks()
    {
        return m_next_block;
    }

    bdd refine(bdd signature, bdd variables, bdd partition);
};

#endif // MCRL2_LPS_SIGREF_H