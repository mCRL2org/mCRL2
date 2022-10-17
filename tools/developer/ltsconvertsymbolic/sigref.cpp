// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

TASK_IMPL_1(BDD, encode_block, uint64_t, b)
{
    BDD result;
    if (cache_get3(CACHE_ENCODE_BLOCK, 0, b, 0, &result)) return result;

    // for now, assume max 64 bits for a block....
    uint8_t bl[block_length];
    for (int i=0; i<block_length; i++) {
        bl[i] = b & 1 ? 1 : 0;
        b>>=1;
    }

    result = sylvan_cube(block_variables, bl);
    cache_put3(CACHE_ENCODE_BLOCK, 0, b, 0, result);
    return result;
}

TASK_IMPL_1(uint64_t, decode_block, BDD, block)
{
    uint64_t result = 0;
    if (cache_get3(CACHE_DECODE_BLOCK, block, 0, 0, &result)) return result;

    uint64_t mask = 1;
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