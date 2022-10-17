// Author(s): Tom van Dijk
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//


#include "bdd_util.h"

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
  if (cache_get3(bdd_from_ldd_id, dd, bits_dd, firstvar, &result)) return result;

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
  cache_put3(bdd_from_ldd_id, dd, bits_dd, firstvar, result);

  return result;
}


/**
 * Extend a transition relation to a larger domain (using s=s')
 */
TASK_IMPL_3(BDD, extend_relation, BDD, relation, BDD, variables, int, state_length)
{
    /* first determine which state BDD variables are in rel */
    int has[state_length];
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

}