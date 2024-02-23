// Author(s): Tom van Dijk
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/symbolic/bdd_util.h"

#include "mcrl2/symbolic/utility.h"
#include "mcrl2/utilities/stack_array.h"
#include "mcrl2/utilities/unordered_set.h"

#include <iostream>

namespace sylvan::ldds
{

TASK_IMPL_3(MTBDD, lddmc_bdd_from_ldd, MDD, dd, MDD, bits_dd, uint32_t, firstvar)
{
  /* simple for leaves */
  if (dd == lddmc_false) return mtbdd_false;
  if (dd == lddmc_true) return mtbdd_true;

  MTBDD result;
  /* get from cache */
  /* note: some assumptions about the encoding... */
  if (cache_get3(cache_bdd_from_ldd_id, dd, bits_dd, firstvar, &result)) return result;

  mddnode_t n = LDD_GETNODE(dd);
  mddnode_t nbits = LDD_GETNODE(bits_dd);
  int bits = (int)mddnode_getvalue(nbits);

  /* spawn right, same bits_dd and firstvar */
  mtbdd_refs_spawn(SPAWN(lddmc_bdd_from_ldd, mddnode_getright(n), bits_dd, firstvar));

  /* call down, with next bits_dd and firstvar */
  MTBDD down = CALL(lddmc_bdd_from_ldd, mddnode_getdown(n), mddnode_getdown(nbits), firstvar + 2*bits);

  /* encode current value */
  uint32_t val = mddnode_getvalue(n);
  for (int i=0; i<bits; i++) {
      /* encode with high bit first */
      int bit = bits-i-1;
      if (val & (1LL<<i)) down = mtbdd_makenode(firstvar + 2*bit, mtbdd_false, down);
      else down = mtbdd_makenode(firstvar + 2*bit, down, mtbdd_false);
  }

  /* sync right */
  mtbdd_refs_push(down);
  MTBDD right = mtbdd_refs_sync(SYNC(lddmc_bdd_from_ldd));

  /* take union of current and right */
  mtbdd_refs_push(right);
  result = sylvan_or(down, right);
  mtbdd_refs_pop(2);

  /* put in cache */
  cache_put3(cache_bdd_from_ldd_id, dd, bits_dd, firstvar, result);

  return result;
}

TASK_IMPL_4(MTBDD, lddmc_bdd_from_ldd_rel, MDD, dd, MDD, bits_dd, uint32_t, firstvar, MDD, meta)
{
    if (dd == lddmc_false) return mtbdd_false;
    if (dd == lddmc_true) return mtbdd_true;
    assert(meta != lddmc_false && meta != lddmc_true);

    /* meta:
     * -1 is end
     *  0 is skip
     *  1 is read
     *  2 is write
     *  3 is only-read
     *  4 is only-write
     */

    MTBDD result;
    /* note: assumptions */
    if (cache_get4(cache_bdd_from_ldd_rel_id, dd, bits_dd, firstvar, meta, &result)) return result;

    const mddnode_t n = LDD_GETNODE(dd);
    const mddnode_t nmeta = LDD_GETNODE(meta);
    const mddnode_t nbits = LDD_GETNODE(bits_dd);
    const int bits = (int)mddnode_getvalue(nbits);

    const uint32_t vmeta = mddnode_getvalue(nmeta);
    assert(vmeta != (uint32_t)-1);

    if (vmeta == 0) {
        /* skip level */
        result = lddmc_bdd_from_ldd_rel(dd, mddnode_getdown(nbits), firstvar + 2*bits, mddnode_getdown(nmeta));
    } else if (vmeta == 1) {
        /* read level */
        assert(!mddnode_getcopy(n));  // do not process read copy nodes for now
        assert(mddnode_getright(n) != mtbdd_true);

        /* spawn right */
        mtbdd_refs_spawn(SPAWN(lddmc_bdd_from_ldd_rel, mddnode_getright(n), bits_dd, firstvar, meta));

        /* compute down with same bits / firstvar */
        MTBDD down = lddmc_bdd_from_ldd_rel(mddnode_getdown(n), bits_dd, firstvar, mddnode_getdown(nmeta));
        mtbdd_refs_push(down);

        /* encode read value */
        uint32_t val = mddnode_getvalue(n);
        MTBDD part = mtbdd_true;
        for (int i=0; i<bits; i++) {
            /* encode with high bit first */
            int bit = bits-i-1;
            if (val & (1LL<<i)) part = mtbdd_makenode(firstvar + 2*bit, mtbdd_false, part);
            else part = mtbdd_makenode(firstvar + 2*bit, part, mtbdd_false);
        }

        /* intersect read value with down result */
        mtbdd_refs_push(part);
        down = sylvan_and(part, down);
        mtbdd_refs_pop(2);

        /* sync right */
        mtbdd_refs_push(down);
        MTBDD right = mtbdd_refs_sync(SYNC(lddmc_bdd_from_ldd_rel));

        /* take union of current and right */
        mtbdd_refs_push(right);
        result = sylvan_or(down, right);
        mtbdd_refs_pop(2);
    } else if (vmeta == 2 || vmeta == 4) {
        /* write or only-write level */

        /* spawn right */
        assert(mddnode_getright(n) != mtbdd_true);
        mtbdd_refs_spawn(SPAWN(lddmc_bdd_from_ldd_rel, mddnode_getright(n), bits_dd, firstvar, meta));

        /* get recursive result */
        MTBDD down = CALL(lddmc_bdd_from_ldd_rel, mddnode_getdown(n), mddnode_getdown(nbits), firstvar + 2*bits, mddnode_getdown(nmeta));

        if (mddnode_getcopy(n)) {
            /* encode a copy node */
            for (int i=0; i<bits; i++) {
                int bit = bits-i-1;
                MTBDD low = mtbdd_makenode(firstvar + 2*bit + 1, down, mtbdd_false);
                mtbdd_refs_push(low);
                MTBDD high = mtbdd_makenode(firstvar + 2*bit + 1, mtbdd_false, down);
                mtbdd_refs_pop(1);
                down = mtbdd_makenode(firstvar + 2*bit, low, high);
            }
        } else {
            /* encode written value */
            uint32_t val = mddnode_getvalue(n);
            for (int i=0; i<bits; i++) {
                /* encode with high bit first */
                int bit = bits-i-1;
                if (val & (1LL<<i)) down = mtbdd_makenode(firstvar + 2*bit + 1, mtbdd_false, down);
                else down = mtbdd_makenode(firstvar + 2*bit + 1, down, mtbdd_false);
            }
        }

        /* sync right */
        mtbdd_refs_push(down);
        MTBDD right = mtbdd_refs_sync(SYNC(lddmc_bdd_from_ldd_rel));

        /* take union of current and right */
        mtbdd_refs_push(right);
        result = sylvan_or(down, right);
        mtbdd_refs_pop(2);
    } else if (vmeta == 3) {
        /* only-read level */
        assert(!mddnode_getcopy(n));  // do not process read copy nodes

        /* spawn right */
        mtbdd_refs_spawn(SPAWN(lddmc_bdd_from_ldd_rel, mddnode_getright(n), bits_dd, firstvar, meta));

        /* get recursive result */
        MTBDD down = CALL(lddmc_bdd_from_ldd_rel, mddnode_getdown(n), mddnode_getdown(nbits), firstvar + 2*bits, mddnode_getdown(nmeta));

        /* encode read value */
        uint32_t val = mddnode_getvalue(n);
        for (int i=0; i<bits; i++) {
            /* encode with high bit first */
            int bit = bits-i-1;
            /* only-read, so write same value */
            if (val & (1LL<<i)) down = mtbdd_makenode(firstvar + 2*bit + 1, mtbdd_false, down);
            else down = mtbdd_makenode(firstvar + 2*bit + 1, down, mtbdd_false);
            if (val & (1LL<<i)) down = mtbdd_makenode(firstvar + 2*bit, mtbdd_false, down);
            else down = mtbdd_makenode(firstvar + 2*bit, down, mtbdd_false);
        }

        /* sync right */
        mtbdd_refs_push(down);
        MTBDD right = mtbdd_refs_sync(SYNC(lddmc_bdd_from_ldd_rel));

        /* take union of current and right */
        mtbdd_refs_push(right);
        result = sylvan_or(down, right);
        mtbdd_refs_pop(2);
    } else if (vmeta == 5) {
        assert(!mddnode_getcopy(n));  // not allowed!

        /* we assume this is the last value */
        result = mtbdd_true;

        /* encode action value */
        uint32_t val = mddnode_getvalue(n);
        for (int i=0; i<bits; i++) {
            /* encode with high bit first */
            int bit = bits-i-1;
            /* only-read, so write same value */
            if (val & (1LL<<i)) result = mtbdd_makenode(action_first_var + bit, mtbdd_false, result);
            else result = mtbdd_makenode(action_first_var + bit, result, mtbdd_false);
        }
    } else {
        assert(vmeta <= 5);
    }

    cache_put4(cache_bdd_from_ldd_rel_id, dd, bits_dd, firstvar, meta, result);

    return result;
}

MTBDD
meta_to_bdd_impl(MDD meta, MDD bits_dd, uint32_t firstvar)
{
    if (meta == lddmc_false || meta == lddmc_true) return mtbdd_true;

    /* meta:
     * -1 is end
     *  0 is skip (no variables)
     *  1 is read (variables added by write)
     *  2 is write
     *  3 is only-read
     *  4 is only-write
     */

    const mddnode_t nmeta = LDD_GETNODE(meta);
    const uint32_t vmeta = mddnode_getvalue(nmeta);
    if (vmeta == (uint32_t)-1) return mtbdd_true;
    
    if (vmeta == 1) {
        /* return recursive result, don't go down on bits */
        return meta_to_bdd_impl(mddnode_getdown(nmeta), bits_dd, firstvar);
    }

    const mddnode_t nbits = LDD_GETNODE(bits_dd);
    const int bits = (int)mddnode_getvalue(nbits);

    /* compute recursive result */
    MTBDD res = meta_to_bdd_impl(mddnode_getdown(nmeta), mddnode_getdown(nbits), firstvar + 2*bits);

    /* add our variables if meta is 2,3,4 */
    if (vmeta != 0 && vmeta != 5) {
        for (int i=0; i<bits; i++) {
            res = mtbdd_makenode(firstvar + 2*(bits-i-1) + 1, mtbdd_false, res);
            res = mtbdd_makenode(firstvar + 2*(bits-i-1), mtbdd_false, res);
        }
    }

    return res;
}

bdds::bdd meta_to_bdd(ldd meta, const std::vector<std::uint32_t>& bits, uint32_t firstvar)
{
  return bdds::bdd(meta_to_bdd_impl(meta.get(), union_cube(false_(), bits.data(), bits.size()).get(), firstvar));
}

TASK_IMPL_4(MDD, lddmc_ldd_from_bdd, MTBDD, dd, MDD, bits_dd, uint32_t, bit, uint32_t, value)
{
    if (dd == mtbdd_true) { return lddmc_makenode(value, lddmc_true, lddmc_false); }
    else if (dd == mtbdd_false) { return lddmc_false; }
    else if (bits_dd == lddmc_true) { return lddmc_true; }

    MDD result;
    if (cache_get4(cache_ldd_from_bdd_id, dd, bits_dd, bit, value, &result)) return result;

    const mddnode_t nbits_dd = LDD_GETNODE(bits_dd);
    std::uint32_t num_bits = mddnode_getvalue(nbits_dd); // The number of bits on this level.

    std::cerr << bit << " of " << num_bits << ", value is " << value << std::endl;

    if (num_bits == bit + 1)
    {
        // Go to the next level
        MDD down = lddmc_ldd_from_bdd(dd, mddnode_getdown(nbits_dd), 0, 0);
        result = lddmc_makenode(value, down, lddmc_false);
    }
    else
    {
        const mtbddnode_t ndd = MTBDD_GETNODE(dd);

        MDD first = lddmc_ldd_from_bdd(mtbddnode_gethigh(ndd), bits_dd, bit+1, value + (1 << (num_bits - bit - 1)));
        MDD second = lddmc_ldd_from_bdd(mtbddnode_getlow(ndd), bits_dd, bit+1, value); 
        
        // The second value is always less than the first.
        result = lddmc_makenode(lddmc_getvalue(second), lddmc_getdown(second), first);
    }

    cache_put4(cache_ldd_from_bdd_id, dd, bits_dd, bit, value, result);
    return result;
}

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
        array[index] = std::max(array[index], set.value() + 1); // Count zero as additional value 
    }
}

std::vector<std::uint32_t> compute_highest(ldd set)
{
    std::vector<std::uint32_t> array(compute_height(set) - 1); // The last level are only terminal nodes.
    mcrl2::utilities::unordered_set<ldd> cache;
    compute_highest_rec(set, cache, array, 0);
    return array;
}

std::vector<std::uint32_t> compute_bits(const std::vector<std::uint32_t>& highest)
{    
  // Compute number of bits for each level
  std::vector<uint32_t> bits (highest.size());
  {
    std::size_t i = 0;
    for (uint32_t value: highest)
    {
        bits[i] = base_two_bits(value);
        ++i;
    }
  }
  return bits;
}

} // namespace sylvan::ldds

namespace sylvan::bdds
{

/**
 * Extend a transition relation to a larger domain (using s=s')
 */
TASK_IMPL_3(BDD, extend_relation, BDD, relation, BDD, variables, int, state_length)
{
    /* first determine which state BDD variables are in rel */
    MCRL2_DECLARE_STACK_ARRAY(has, int, state_length);
    for (int i=0; i<state_length; i++) has[i] = 0;
    BDDSET s = variables;
    while (s != sylvan_true) {
        BDDVAR v = sylvan_var(s);
        if (v/2 >= (unsigned)state_length) break; // action labels
        has[v/2] = 1;
        s = sylvan_high(s);
    }
    /* create "s=s'" for all variables not in rel */
    BDD eq = sylvan_true;
    for (int i=state_length-1; i>=0; i--) {
        if (has[i]) continue;
        BDD low = sylvan_makenode(2*i+1, eq, sylvan_false);
        bdd_refs_push(low);
        BDD high = sylvan_makenode(2*i+1, sylvan_false, eq);
        bdd_refs_pop(1);
        eq = sylvan_makenode(2*i, low, high);
    }

    bdd_refs_push(eq);
    BDD result = sylvan_and(relation, eq);
    bdd_refs_pop(1);

    return result;
}

TASK_IMPL_2(MTBDD, big_union, MTBDD*, sets, size_t, count)
{
    if (count == 1) return *sets;
    mtbdd_refs_spawn(SPAWN(big_union, sets, count/2));
    MTBDD right = mtbdd_refs_push(CALL(big_union, sets+count/2, count-count/2));
    MTBDD left = mtbdd_refs_push(mtbdd_refs_sync(SYNC(big_union)));
    MTBDD result = mtbdd_plus(left, right);
    mtbdd_refs_pop(2);
    return result;
}

} // namespace sylvan::bdds