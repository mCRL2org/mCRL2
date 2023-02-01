// Author(s): Tom van Dijk
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_SYMBOLIC_BDD_UTIL_H
#define MCRL2_SYMBOLIC_BDD_UTIL_H

#include <sylvan_int.h>
#include <sylvan_ldd.hpp>
#include <sylvan_bdd.hpp>
#include <sylvan_int.h>

#include <functional>
#include <cmath>
#include <vector>

namespace sylvan::ldds
{

static uint64_t cache_bdd_from_ldd_id;
static uint64_t cache_bdd_from_ldd_rel_id;
static uint64_t cache_ldd_from_bdd_id;

inline void init_bdd_util() 
{  
  cache_bdd_from_ldd_id = cache_next_opid();
  cache_bdd_from_ldd_rel_id = cache_next_opid();
  cache_ldd_from_bdd_id = cache_next_opid();
}

const uint64_t action_first_var = 1000000;

/// Compute the BDD equivalent of the LDD of a set of states. The resulting BDD variables are firstvar + 2*bit.
TASK_DECL_3(MTBDD, lddmc_bdd_from_ldd, MDD, MDD, uint32_t)
#define lddmc_bdd_from_ldd(dd, bits, firstvar) CALL(lddmc_bdd_from_ldd, dd, bits, firstvar)

/// \brief Compute the BDD equivalent of an LDD transition relation 
/// \details Assumes that the last bits encode the action_label, and only meta.last() can be five.
TASK_DECL_4(MTBDD, lddmc_bdd_from_ldd_rel, MDD, MDD, uint32_t, MDD)
#define lddmc_bdd_from_ldd_rel(dd, bits, firstvar, meta) CALL(lddmc_bdd_from_ldd_rel, dd, bits, firstvar, meta)

inline 
bdds::bdd bdd_from_ldd(const ldd& set, const std::vector<std::uint32_t>& bits, std::uint32_t firstvar)
{
  LACE_ME;
  return bdds::bdd(lddmc_bdd_from_ldd(set.get(), cube(bits).get(), firstvar));
}

inline 
bdds::bdd bdd_from_ldd_rel(const ldd& set, const std::vector<std::uint32_t>& bits, std::uint32_t firstvar, const ldd& meta)
{
  LACE_ME;
  return bdds::bdd(lddmc_bdd_from_ldd_rel(set.get(), cube(bits).get(), firstvar, meta.get()));
}

/// Compute the BDD equivalent of the meta variable (to a variables cube). The variables become firstvar + 2*i, where i is the index in meta.
bdds::bdd meta_to_bdd(ldd meta, const std::vector<std::uint32_t>& bits, uint32_t firstvar);

/// Compute the BDD equivalent of the LDD of a set of states.
TASK_DECL_4(MDD, lddmc_ldd_from_bdd, MTBDD, MDD, uint32_t, uint32_t)
#define lddmc_ldd_from_bdd(dd, bits, bit, value) CALL(lddmc_ldd_from_bdd, dd, bits, bit, value)

inline 
ldd ldd_from_bdd(const bdds::bdd& set, const std::vector<std::uint32_t>& bits)
{
  LACE_ME;
  return ldd(lddmc_ldd_from_bdd(set.get(), cube(bits).get(), 0, 0));
}

/// Compute the height of the LDD.
std::uint32_t compute_height(ldd set);

/// Compute the highest value for each variable level.
std::vector<std::uint32_t> compute_highest(ldd set);

/// Computes the number of bits required to store the highest value at every level.
std::vector<std::uint32_t> compute_bits(const std::vector<std::uint32_t>& highest);

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

/// \brief Returns the number of bits required to encode using base 2 encoding.
inline
std::size_t base_two_bits(std::size_t max_value)
{
  return std::max(std::ceil(std::log2(max_value)), 1.0);
}

#endif // MCRL2_SYMBOLIC_BDD_UTIL_H