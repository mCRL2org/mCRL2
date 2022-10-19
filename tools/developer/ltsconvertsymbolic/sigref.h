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

#include "symbolic_lts_bdd.h"
#include "mcrl2/utilities/exception.h"

#include <sylvan.h>
#include <sylvan_bdd.hpp>

namespace sylvan::bdds
{
    
// The cache ids for the encode and decode block operations.
static uint64_t CACHE_ENCODE_BLOCK;
static uint64_t CACHE_DECODE_BLOCK;
static uint64_t CACHE_SWAPPRIME;

/// Encode a block number (max 64bit) as a BDD cube where the bits represent the number in base 2.
TASK_DECL_3(BDD, sylvan_encode_block, BDDSET, std::uint64_t, std::uint64_t);
#define sylvan_encode_block(vars, block_length, block_number) CALL(sylvan_encode_block, vars, block_length, block_number)

/// The inverse of sylvan_encode_block.
TASK_DECL_1(std::uint64_t, sylvan_decode_block, BDD);
#define sylvan_decode_block(block) CALL(sylvan_decode_block, block)

/// Substitute each s by s' and vice versa
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


class sigref_algorithm
{
public:
    void run(const mcrl2::lps::symbolic_lts_bdd& lts);

private:
    using bdd = sylvan::bdds::bdd;

    std::size_t m_next_block = 0;
    int         m_block_length = 0; // number of block variables
    bdd m_block_variables;

    /// \brief Returns the next free block number.
    size_t
    get_next_block()
    {
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
        uint32_t block_base = 2000000; // base for block variables (this is some magic value)
        std::vector<uint32_t> variables(block_length);
        for (int i=0; i<block_length; i++) 
        {
            variables[i] = block_base+2*i;
        }

        m_block_variables = sylvan::bdds::cube(variables);
    }
    
    size_t
    count_blocks()
    {
        return m_next_block - 1;
    }

    bdd refine(bdd signature, bdd variables, bdd partition);
};

#endif // MCRL2_LPS_SIGREF_H