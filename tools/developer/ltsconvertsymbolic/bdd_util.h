// Author(s): Tom van Dijk
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_BDD_UTIL_H
#define MCRL2_BDD_UTIL_H

#include <sylvan_int.h>
#include <sylvan_ldd.hpp>
#include <sylvan_bdd.hpp>

#include <functional>
#include <vector>

namespace sylvan::ldds
{

/**
 * Compute the BDD equivalent of the LDD of a set of states.
 */
static uint64_t bdd_from_ldd_id;
TASK_DECL_3(MTBDD, lddmc_bdd_from_ldd, MDD, MDD, uint32_t)
#define lddmc_bdd_from_ldd(dd, bits, firstvar) CALL(lddmc_bdd_from_ldd, dd, bits, firstvar)

inline 
bdds::bdd bdd_from_ldd(const ldd& set, const std::vector<std::uint32_t>& bits, std::uint32_t firstvar)
{
  LACE_ME;
  return bdds::bdd(lddmc_bdd_from_ldd(set.get(), union_cube(false_(), bits.data(), bits.size()).get(), firstvar));
}

/// Compute the BDD equivalent of the meta variable (to a variables cube). The variables become firstvar + 2*i, where i is the index in meta.
bdds::bdd meta_to_bdd(ldd meta, const std::vector<std::uint32_t>& bits, uint32_t firstvar);

} // namespace sylvan::ldds

namespace sylvan::bdds
{
  
/// Extend a relation <rel> defined on variables <vars> to the full domain,
/// which has <state_length> state variables (and <state_length> prime variables)
TASK_DECL_3(BDD, extend_relation, BDD, BDD, int);
#define sylvan_extend_relation(rel, vars, state_length) CALL(extend_relation, rel, vars, state_length)

// Compute union of the given vector of BDDs
TASK_DECL_2(MTBDD, big_union, BDD*, size_t)
#define sylvan_big_union(sets, count) CALL(big_union, sets, count)

inline
bdds::bdd extend_relation(const bdds::bdd& set, const bdds::bdd& vars, int state_length)
{
  LACE_ME;
  return bdds::bdd(sylvan_extend_relation(set.get(), vars.get(), state_length));
}

inline
bdds::bdd big_union(const std::vector<bdds::bdd>& sets)
{
  std::vector<BDD> result;
  std::transform(sets.begin(), sets.end(), std::back_inserter(result), [](const bdds::bdd& bdd) { return bdd.get(); });
  
  LACE_ME
  return bdds::bdd(sylvan_big_union(result.data(), result.size()));
}


} // namespace sylvan::bdds

#endif // MCRL2_BDD_UTIL_H