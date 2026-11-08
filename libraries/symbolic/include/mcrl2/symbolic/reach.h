// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/symbolic/reach.h
/// \brief The REACH operation (S. Brand, T. Bäck, A. Laarman: A Decision Diagram Operation for Reachability,
///        FM 2023) on LDDs, with on-the-fly learning of the transition relations of the summand groups.
///
/// \details REACH computes the states reachable from a set S with one recursive operation on S and a single
///          relation R, without a partition of R. On a state variable x it splits R into the transitions that keep
///          the value of x and those that change it. Until S no longer changes: for every value v of x (in
///          parallel), apply REACH below v with the transitions that keep v; then add the successors of the
///          transitions that change x (one relprod).
///
///          Here R is the union of the relations of all summand groups, each extended to all state variables
///          (copy nodes for the variables that the group does not touch), so that every level of R has a read
///          level and a write level. The relation of a group is still being learned, so a group joins R only on
///          the level of its first read or written variable (its top variable): there, after the states below
///          have been saturated, the group learns the transitions for the states of S (projected on its read
///          variables), and its current relation, extended from that level on, is added to R. Above its top
///          variable a group would only add copy nodes, so leaving it out there loses nothing.
///
///          Caching: reach(S, R, k) is the set of states reachable from S with R and with all transitions of the
///          groups whose top variable is at least k, learned or not. Learning can only add transitions of those
///          groups, so this set does not depend on when the call is made, and a cached result stays valid while
///          the relations grow. The loop on a level only ends after a pass in which learning added nothing for
///          the final set, so a single call gives all reachable states.

#ifndef MCRL2_SYMBOLIC_REACH_H
#define MCRL2_SYMBOLIC_REACH_H

#ifdef MCRL2_ENABLE_SYLVAN

#include "mcrl2/symbolic/saturation.h"

#include <sylvan_ldd.hpp>

#include <atomic>
#include <cstdint>
#include <vector>

namespace mcrl2::symbolic {

/// \brief The input of a REACH run.
struct reach_context
{
  std::size_t nvars = 0;                                  // the number of state variables
  std::vector<std::vector<saturation_relation>> groups;   // groups[k]: the groups whose top variable is k
  std::size_t last_level = 0;                             // the highest top variable of a group
  std::vector<sylvan::ldds::ldd> identity;                // identity[k]: the identity relation on the variables k..nvars-1
  std::uint64_t opid_reach = 0;
  std::uint64_t opid_extend = 0;
  std::uint64_t opid_keep = 0;
  std::uint64_t opid_change = 0;
  std::uint64_t opid_labels = 0;
  std::uint64_t opid_relprod = 0;
  std::uint64_t opid_downs = 0;
  std::uint64_t opid_learned = 0;
  std::uint64_t id = 0;                                   // tells the calls of reach apart in the operation cache

  /// \brief Learns the transitions of a group for the projected states X (over the read variables of the group).
  /// Called on any Lace worker, possibly by several at the same time; X is protected by the caller.
  void (*expand)(void* expand_context, std::size_t group, sylvan::MDD X) = nullptr;
  void* expand_context = nullptr;

  mutable std::atomic<std::size_t> top_iterations{0};     // the number of passes of the loop on the first level
};

namespace detail {

// The Lace task macros define functions that are not inline, so give them
// internal linkage to keep this header usable in more than one translation unit.
namespace {

using namespace ::sylvan;

/// \brief The union of the relations below a list of action labels.
TASK_2(MDD, reach_labels, const reach_context*, ctx, MDD, rel)
{
  if (rel == lddmc_false)
  {
    return lddmc_false;
  }
  MDD result;
  if (cache_get3(ctx->opid_labels, rel, 0, 0, &result))
  {
    return result;
  }
  MDD right = lddmc_refs_push(CALL(reach_labels, ctx, lddmc_getright(rel)));
  result = CALL(lddmc_union, lddmc_getdown(rel), right);
  lddmc_refs_pop(1);
  cache_put3(ctx->opid_labels, rel, 0, 0, result);
  return result;
}

TASK_DECL_4(MDD, reach_extend, const reach_context*, MDD, MDD, std::size_t);

/// \brief Applies reach_extend below every node of a list (mode 0), below every write of a list of reads (mode 1),
/// or for an only-read list, also adds the write of the same value (mode 2).
TASK_5(MDD, reach_extend_list, const reach_context*, ctx, MDD, list, MDD, meta, std::size_t, nvars, int, mode)
{
  if (list == lddmc_false)
  {
    return lddmc_false;
  }
  lddmc_refs_spawn(SPAWN(reach_extend_list, ctx, lddmc_getright(list), meta, nvars, mode));
  MDD down = mode == 1 ? CALL(reach_extend_list, ctx, lddmc_getdown(list), meta, nvars, 0)
                       : CALL(reach_extend, ctx, lddmc_getdown(list), meta, nvars);
  lddmc_refs_push(down);
  MDD right = lddmc_refs_sync(SYNC(reach_extend_list));
  lddmc_refs_push(right);
  MDD result;
  if (down == lddmc_false)
  {
    result = right;
  }
  else if (lddmc_iscopy(list))
  {
    if (mode == 2)
    {
      down = lddmc_make_copynode(down, lddmc_false);
    }
    result = lddmc_make_copynode(down, right);
  }
  else
  {
    const std::uint32_t value = lddmc_getvalue(list);
    if (mode == 2)
    {
      down = lddmc_makenode(value, down, lddmc_false);
    }
    result = lddmc_makenode(value, down, right);
  }
  lddmc_refs_pop(2);
  return result;
}

/// \brief Extends a relation rel with relprod meta meta to the next nvars state variables: every variable gets a
/// read level and a write level, with copy nodes (read any value, write the same value) for the variables that rel
/// does not touch. Action labels are quantified.
TASK_IMPL_4(MDD, reach_extend, const reach_context*, ctx, MDD, rel, MDD, meta, std::size_t, nvars)
{
  if (rel == lddmc_false)
  {
    return lddmc_false;
  }
  const std::uint32_t m_val = meta == lddmc_true ? static_cast<std::uint32_t>(-1) : lddmc_getvalue(meta);
  if (nvars == 0 && m_val != 5)
  {
    assert(m_val == static_cast<std::uint32_t>(-1));
    return lddmc_true;
  }
  if (m_val == static_cast<std::uint32_t>(-1))
  {
    assert(rel == lddmc_true);
    return ctx->identity[ctx->nvars - nvars].get(); // rel ends here: copy all remaining variables
  }

  sylvan_gc_test();

  MDD result;
  if (cache_get3(ctx->opid_extend, rel, meta, nvars, &result))
  {
    return result;
  }

  if (m_val == 0)
  {
    // not in rel: copy (read any value, write the same value)
    MDD down = CALL(reach_extend, ctx, rel, lddmc_getdown(meta), nvars - 1);
    if (down != lddmc_false)
    {
      down = lddmc_make_copynode(down, lddmc_false);
      down = lddmc_make_copynode(down, lddmc_false);
    }
    result = down;
  }
  else if (m_val == 1)
  {
    // read, then write
    result = CALL(reach_extend_list, ctx, rel, lddmc_getdown(lddmc_getdown(meta)), nvars - 1, 1);
  }
  else if (m_val == 3)
  {
    // only-read: read the value, then write the same value
    result = CALL(reach_extend_list, ctx, rel, lddmc_getdown(meta), nvars - 1, 2);
  }
  else if (m_val == 4)
  {
    // only-write: read any value, then write
    MDD down = CALL(reach_extend_list, ctx, rel, lddmc_getdown(meta), nvars - 1, 0);
    result = down == lddmc_false ? lddmc_false : lddmc_make_copynode(down, lddmc_false);
  }
  else
  {
    // action label: quantify
    assert(m_val == 5);
    MDD all = lddmc_refs_push(CALL(reach_labels, ctx, rel));
    result = CALL(reach_extend, ctx, all, lddmc_getdown(meta), nvars);
    lddmc_refs_pop(1);
  }

  cache_put3(ctx->opid_extend, rel, meta, nvars, result);
  return result;
}

/// \brief The transitions of rel (on a read level) that keep the value, as a list of values i, each with the
/// relation below the write level for i -> i. A copy node first holds the relation for i -> i for every i.
TASK_2(MDD, reach_keep, const reach_context*, ctx, MDD, rel)
{
  if (rel == lddmc_false)
  {
    return lddmc_false;
  }
  sylvan_gc_test();
  MDD result;
  if (cache_get3(ctx->opid_keep, rel, 0, 0, &result))
  {
    return result;
  }

  MDD writes = lddmc_getdown(rel);
  MDD copy = lddmc_false;
  if (lddmc_iscopy(writes))
  {
    copy = lddmc_getdown(writes);
    writes = lddmc_getright(writes);
  }

  MDD right = lddmc_refs_push(CALL(reach_keep, ctx, lddmc_getright(rel)));
  if (lddmc_iscopy(rel))
  {
    // read any i: write i (copy) keeps every i, write j keeps j
    result = CALL(lddmc_union, writes, right);
    if (copy != lddmc_false)
    {
      result = lddmc_make_copynode(copy, result);
    }
  }
  else
  {
    // read i: write i (copy or explicit) keeps i
    const std::uint32_t i = lddmc_getvalue(rel);
    MDD down = CALL(lddmc_union, copy, lddmc_follow(writes, i));
    result = lddmc_makenode(i, down, right);
  }
  lddmc_refs_pop(1);

  cache_put3(ctx->opid_keep, rel, 0, 0, result);
  return result;
}

/// \brief Removes a value from a list of (non-copy) nodes.
inline MDD reach_without(MDD list, std::uint32_t value)
{
  if (list == lddmc_false)
  {
    return lddmc_false;
  }
  const std::uint32_t v = lddmc_getvalue(list);
  if (v > value)
  {
    return list;
  }
  if (v == value)
  {
    return lddmc_getright(list);
  }
  return lddmc_makenode(v, lddmc_getdown(list), reach_without(lddmc_getright(list), value));
}

/// \brief The transitions of rel (on a read level) that change the value, in the same format as rel. After a read
/// of any value (copy node), the explicit writes also include writing the value that was read; that is harmless.
TASK_2(MDD, reach_change, const reach_context*, ctx, MDD, rel)
{
  if (rel == lddmc_false)
  {
    return lddmc_false;
  }
  sylvan_gc_test();
  MDD result;
  if (cache_get3(ctx->opid_change, rel, 0, 0, &result))
  {
    return result;
  }

  MDD writes = lddmc_getdown(rel);
  if (lddmc_iscopy(writes))
  {
    writes = lddmc_getright(writes);
  }

  MDD right = lddmc_refs_push(CALL(reach_change, ctx, lddmc_getright(rel)));
  if (lddmc_iscopy(rel))
  {
    result = writes == lddmc_false ? right : lddmc_make_copynode(writes, right);
  }
  else
  {
    const std::uint32_t i = lddmc_getvalue(rel);
    writes = reach_without(writes, i);
    result = lddmc_makenode(i, writes, right);
  }
  lddmc_refs_pop(1);

  cache_put3(ctx->opid_change, rel, 0, 0, result);
  return result;
}

/// \brief The union of the sets below the values of a list.
TASK_2(MDD, reach_downs, const reach_context*, ctx, MDD, set)
{
  if (set == lddmc_false)
  {
    return lddmc_false;
  }
  MDD result;
  if (cache_get3(ctx->opid_downs, set, 0, 0, &result))
  {
    return result;
  }
  MDD right = lddmc_refs_push(CALL(reach_downs, ctx, lddmc_getright(set)));
  result = CALL(lddmc_union, lddmc_getdown(set), right);
  lddmc_refs_pop(1);
  cache_put3(ctx->opid_downs, set, 0, 0, result);
  return result;
}

TASK_DECL_4(MDD, reach_relprod, const reach_context*, MDD, MDD, std::size_t);

/// \brief For every value j of the (explicit) list of writes on level k: j -> the successors of src (on level k+1)
/// with the relation below j.
TASK_4(MDD, reach_relprod_writes, const reach_context*, ctx, MDD, src, MDD, writes, std::size_t, k)
{
  if (writes == lddmc_false)
  {
    return lddmc_false;
  }
  assert(!lddmc_iscopy(writes));
  lddmc_refs_spawn(SPAWN(reach_relprod_writes, ctx, src, lddmc_getright(writes), k));
  MDD down = lddmc_refs_push(CALL(reach_relprod, ctx, src, lddmc_getdown(writes), k + 1));
  MDD right = lddmc_refs_sync(SYNC(reach_relprod_writes));
  lddmc_refs_pop(1);
  return lddmc_makenode(lddmc_getvalue(writes), down, right);
}

/// \brief The successors of set (a list on level k) with the explicit reads of a relation (a list on level k, without
/// a copy node), each with a list of explicit writes.
TASK_4(MDD, reach_relprod_reads, const reach_context*, ctx, MDD, set, MDD, reads, std::size_t, k)
{
  for (;;)
  {
    if (set == lddmc_false || reads == lddmc_false)
    {
      return lddmc_false;
    }
    const std::uint32_t sv = lddmc_getvalue(set);
    const std::uint32_t rv = lddmc_getvalue(reads);
    if (sv < rv)
    {
      set = lddmc_getright(set);
    }
    else if (rv < sv)
    {
      reads = lddmc_getright(reads);
    }
    else
    {
      break;
    }
  }
  lddmc_refs_spawn(SPAWN(reach_relprod_reads, ctx, lddmc_getright(set), lddmc_getright(reads), k));
  MDD here = lddmc_refs_push(CALL(reach_relprod_writes, ctx, lddmc_getdown(set), lddmc_getdown(reads), k));
  MDD rest = lddmc_refs_push(lddmc_refs_sync(SYNC(reach_relprod_reads)));
  MDD result = CALL(lddmc_union, here, rest);
  lddmc_refs_pop(2);
  return result;
}

/// \brief For every value v of set (a list on level k): v -> the successors below v with the relation copy (for every
/// v) and the relation for v in the list keep. The values of set are handled in parallel.
TASK_5(MDD, reach_relprod_keep, const reach_context*, ctx, MDD, set, MDD, copy, MDD, keep, std::size_t, k)
{
  if (copy == lddmc_false)
  {
    // only the values of keep: skip the other values of set
    for (;;)
    {
      if (set == lddmc_false || keep == lddmc_false)
      {
        return lddmc_false;
      }
      const std::uint32_t sv = lddmc_getvalue(set);
      const std::uint32_t kv = lddmc_getvalue(keep);
      if (sv < kv)
      {
        set = lddmc_getright(set);
      }
      else if (kv < sv)
      {
        keep = lddmc_getright(keep);
      }
      else
      {
        break;
      }
    }
  }
  if (set == lddmc_false)
  {
    return lddmc_false;
  }

  const std::uint32_t v = lddmc_getvalue(set);
  while (keep != lddmc_false && lddmc_getvalue(keep) < v)
  {
    keep = lddmc_getright(keep);
  }
  MDD rel = copy;
  if (keep != lddmc_false && lddmc_getvalue(keep) == v)
  {
    rel = CALL(lddmc_union, copy, lddmc_getdown(keep));
  }
  lddmc_refs_push(rel);

  lddmc_refs_spawn(SPAWN(reach_relprod_keep, ctx, lddmc_getright(set), copy, keep, k));
  MDD down = lddmc_refs_push(CALL(reach_relprod, ctx, lddmc_getdown(set), rel, k + 1));
  MDD right = lddmc_refs_sync(SYNC(reach_relprod_keep));
  lddmc_refs_pop(2);
  return lddmc_makenode(v, down, right);
}

/// \brief The successors of set (whose first level is state variable k) with the relation rel (a read and a write
/// level for each of the variables k..nvars-1), like lddmc_relprod. Unlike lddmc_relprod, it returns set at once for
/// the identity relation, and it keeps the values of set that a copy node (read any value, write the same value)
/// keeps without taking unions; it only takes unions for the transitions that change the value.
TASK_IMPL_4(MDD, reach_relprod, const reach_context*, ctx, MDD, set, MDD, rel, std::size_t, k)
{
  if (set == lddmc_false || rel == lddmc_false)
  {
    return lddmc_false;
  }
  if (rel == ctx->identity[k].get())
  {
    return set; // also for k == nvars, where rel and set are both lddmc_true
  }
  assert(k < ctx->nvars);

  sylvan_gc_test();

  MDD result;
  if (cache_get3(ctx->opid_relprod, set, rel, k, &result))
  {
    return result;
  }

  MDD keep = lddmc_refs_push(CALL(reach_keep, ctx, rel));
  MDD change = lddmc_refs_push(CALL(reach_change, ctx, rel));

  // the transitions that keep the value
  MDD copy = lddmc_false;
  MDD list = keep;
  if (lddmc_iscopy(list))
  {
    copy = lddmc_getdown(list);
    list = lddmc_getright(list);
  }
  lddmc_refs_spawn(SPAWN(reach_relprod_keep, ctx, set, copy, list, k));

  // the transitions that change the value: after reading any value (a copy node), and after explicit reads
  MDD changed = lddmc_false;
  lddmc_refs_pushptr(&changed);
  MDD reads = change;
  if (lddmc_iscopy(reads))
  {
    MDD all = lddmc_refs_push(CALL(reach_downs, ctx, set));
    changed = CALL(reach_relprod_writes, ctx, all, lddmc_getdown(reads), k);
    lddmc_refs_pop(1);
    reads = lddmc_getright(reads);
  }
  if (reads != lddmc_false)
  {
    MDD explicit_reads = lddmc_refs_push(CALL(reach_relprod_reads, ctx, set, reads, k));
    changed = CALL(lddmc_union, changed, explicit_reads);
    lddmc_refs_pop(1);
  }

  MDD kept = lddmc_refs_push(lddmc_refs_sync(SYNC(reach_relprod_keep)));
  result = CALL(lddmc_union, kept, changed);
  lddmc_refs_pop(3);
  lddmc_refs_popptr(1);

  cache_put3(ctx->opid_relprod, set, rel, k, result);
  return result;
}

/// \brief The part of set (a list on level k) whose values and sets below are not known to be learned by the groups
/// of level k (see reach_mark_learned).
inline MDD reach_unlearned(const reach_context* ctx, MDD set, std::size_t k)
{
  if (set == lddmc_false)
  {
    return lddmc_false;
  }
  MDD rest = reach_unlearned(ctx, lddmc_getright(set), k);
  MDD known;
  if (cache_get3(ctx->opid_learned, lddmc_getdown(set), lddmc_getvalue(set), k, &known))
  {
    return rest;
  }
  return lddmc_makenode(lddmc_getvalue(set), lddmc_getdown(set), rest);
}

/// \brief Remembers that the groups of level k have learned for all states of set (a list on level k), per value and
/// set below it: learning only adds, so this stays true, and the sets below a value often come back unchanged.
inline void reach_mark_learned(const reach_context* ctx, MDD set, std::size_t k)
{
  for (; set != lddmc_false; set = lddmc_getright(set))
  {
    cache_put3(ctx->opid_learned, lddmc_getdown(set), lddmc_getvalue(set), k, 1);
  }
}

/// \brief Learns the transitions of the groups ctx->groups[k][from, from + len) for the states of set (on level k), in
/// parallel: only the projections that a group has not learned yet go to expand (most checks find none). With more
/// than one worker, expand must be thread-safe (see effective_options).
VOID_TASK_5(reach_learn_groups, const reach_context*, ctx, MDD, set, std::size_t, k, std::size_t, from, std::size_t, len)
{
  if (len == 1)
  {
    const saturation_relation& r = ctx->groups[k][from];
    // project, then minus (not project_minus): the cache key of project does not contain the learned domain, which
    // changes all the time, so its results are reused across calls (measured on 4 sockets for saturation).
    MDD X = lddmc_refs_push(CALL(lddmc_project, set, r.topread.get()));
    MDD domain = lddmc_refs_push(r.Ldomain->load());
    MDD Y = lddmc_refs_push(CALL(lddmc_minus, X, domain));
    if (Y != lddmc_false)
    {
      ctx->expand(ctx->expand_context, r.group, Y);
    }
    lddmc_refs_pop(3);
    return;
  }
  const std::size_t half = len / 2;
  SPAWN(reach_learn_groups, ctx, set, k, from, half);
  CALL(reach_learn_groups, ctx, set, k, from + half, len - half);
  SYNC(reach_learn_groups);
}

/// \brief rel U the current relations of the groups whose top variable is k, extended from level k on.
TASK_3(MDD, reach_add_groups, const reach_context*, ctx, MDD, rel, std::size_t, k)
{
  MDD result = rel;
  lddmc_refs_pushptr(&result);
  for (const saturation_relation& r: ctx->groups[k])
  {
    // Read the relation once and protect it: another worker may replace it meanwhile.
    MDD L = lddmc_refs_push(r.L->load());
    MDD extended = CALL(reach_extend, ctx, L, r.topmeta.get(), ctx->nvars - k);
    lddmc_refs_push(extended);
    result = CALL(lddmc_union, result, extended);
    lddmc_refs_pop(2);
  }
  lddmc_refs_popptr(1);
  return result;
}

TASK_DECL_4(MDD, reach_go, const reach_context*, MDD, MDD, std::size_t);

/// \brief For every value i of set (a list on level k), applies reach below i with the relation copy (for every i)
/// and the relation for i in the list keep. The values of set are handled in parallel.
TASK_5(MDD, reach_keep_apply, const reach_context*, ctx, MDD, set, MDD, copy, MDD, keep, std::size_t, k)
{
  if (set == lddmc_false)
  {
    return lddmc_false;
  }
  if (copy == lddmc_false && keep == lddmc_false && ctx->last_level <= k)
  {
    return set; // nothing to apply below level k
  }

  const std::uint32_t i = lddmc_getvalue(set);
  while (keep != lddmc_false && lddmc_getvalue(keep) < i)
  {
    keep = lddmc_getright(keep);
  }
  MDD rel = copy;
  if (keep != lddmc_false && lddmc_getvalue(keep) == i)
  {
    rel = CALL(lddmc_union, copy, lddmc_getdown(keep));
  }
  lddmc_refs_push(rel);

  lddmc_refs_spawn(SPAWN(reach_keep_apply, ctx, lddmc_getright(set), copy, keep, k));
  MDD down = lddmc_refs_push(CALL(reach_go, ctx, lddmc_getdown(set), rel, k + 1));
  MDD right = lddmc_refs_sync(SYNC(reach_keep_apply));
  lddmc_refs_pop(2);
  return lddmc_makenode(i, down, right);
}

/// \brief The states reachable from set (whose first level is state variable k) with the relation rel (on the
/// variables k..nvars-1, a read and a write level for each) and with the groups whose top variable is at least k.
TASK_IMPL_4(MDD, reach_go, const reach_context*, ctx, MDD, set, MDD, rel, std::size_t, k)
{
  /* Terminal cases */
  if (set == lddmc_false)
  {
    return lddmc_false;
  }
  if (k == ctx->nvars)
  {
    return set;
  }
  if (rel == ctx->identity[k].get())
  {
    rel = lddmc_false; // the identity relation adds nothing
  }
  if (rel == lddmc_false && k > ctx->last_level)
  {
    return set;
  }

  sylvan_gc_test();

  /* Consult the cache */
  MDD result;
  const std::uint64_t key = (ctx->id << 32) | k;
  if (cache_get3(ctx->opid_reach, set, rel, key, &result))
  {
    return result;
  }

  const bool has_groups = !ctx->groups[k].empty();
  const bool learns = has_groups && ctx->expand != nullptr;
  MDD R = rel;
  MDD keep = lddmc_false;
  MDD change = lddmc_false;
  MDD learned = lddmc_false; // the last set for which the groups of this level have learned
  MDD fresh = lddmc_false;
  result = set;
  lddmc_refs_pushptr(&R);
  lddmc_refs_pushptr(&keep);
  lddmc_refs_pushptr(&change);
  lddmc_refs_pushptr(&learned);
  lddmc_refs_pushptr(&fresh);
  lddmc_refs_pushptr(&result);

  if (has_groups)
  {
    R = CALL(reach_add_groups, ctx, rel, k);
  }
  keep = CALL(reach_keep, ctx, R);
  change = CALL(reach_change, ctx, R);

  for (;;)
  {
    if (k == 0)
    {
      ctx->top_iterations.fetch_add(1, std::memory_order_relaxed);
    }

    /* Saturate below every value with the transitions that keep it */
    MDD copy = lddmc_false;
    MDD list = keep;
    if (lddmc_iscopy(list))
    {
      copy = lddmc_getdown(list);
      list = lddmc_getright(list);
    }
    result = CALL(reach_keep_apply, ctx, result, copy, list, k);

    /* Learn the transitions of the groups of this level for the states that they have not learned for yet */
    bool relation_changed = false;
    if (learns && result != learned)
    {
      // Only the values whose sets below are not known to be learned (see reach_mark_learned) need a check, and
      // only the projections that a group has not learned yet need expand (most checks find none).
      fresh = reach_unlearned(ctx, result, k);
      if (fresh != lddmc_false)
      {
        CALL(reach_learn_groups, ctx, fresh, k, 0, ctx->groups[k].size());
        reach_mark_learned(ctx, result, k);
      }
      learned = result;

      // Read the relations again, also when this worker learned nothing: another worker may have learned (and
      // marked) transitions for these states after R was made.
      MDD R1 = CALL(reach_add_groups, ctx, rel, k);
      if (R1 != R)
      {
        R = R1;
        relation_changed = true;
        keep = CALL(reach_keep, ctx, R);
        change = CALL(reach_change, ctx, R);
      }
    }

    /* Add the successors of the transitions that change the value */
    MDD before = result;
    if (change != lddmc_false)
    {
      // relprod, then union: the cached results of relprod for the parts of result that did not change since the
      // previous pass can be reused (those of a relprod_union could not, since its last argument always changes).
      MDD successors = lddmc_refs_push(CALL(reach_relprod, ctx, result, change, k));
      result = CALL(lddmc_union, result, successors);
      lddmc_refs_pop(1);
    }

    // Done when result is closed under the transitions that keep the value (keep_apply), learning added nothing for
    // it (so R is complete for it), and the transitions that change the value add nothing.
    if (result == before && !relation_changed)
    {
      break;
    }
  }
  lddmc_refs_popptr(6);

  /* Store in cache; the result is closed, so it is also its own result (callers often pass it again) */
  cache_put3(ctx->opid_reach, set, rel, key, result);
  if (result != set)
  {
    cache_put3(ctx->opid_reach, result, rel, key, result);
  }
  return result;
}

} // namespace

} // namespace detail

/// \brief Sets up ctx for the state variables 0..nvars-1 and the given groups (see make_saturation_relation).
inline void make_reach_context(reach_context& ctx, std::size_t nvars, const std::vector<saturation_relation>& rels)
{
  using namespace sylvan::ldds;
  ctx.nvars = nvars;
  ctx.groups.assign(nvars, {});
  ctx.last_level = 0;
  for (const saturation_relation& r: rels)
  {
    assert(r.topvar < nvars);
    ctx.groups[r.topvar].push_back(r);
    ctx.last_level = std::max(ctx.last_level, r.topvar);
  }

  ctx.identity.assign(nvars + 1, ldd());
  ctx.identity[nvars] = ldd(sylvan::lddmc_true);
  for (std::size_t k = nvars; k-- > 0;)
  {
    ctx.identity[k] = ldd(sylvan::lddmc_make_copynode(sylvan::lddmc_make_copynode(ctx.identity[k + 1].get(), sylvan::lddmc_false), sylvan::lddmc_false));
  }

  if (ctx.opid_reach == 0)
  {
    ctx.opid_reach = sylvan::cache_next_opid();
    ctx.opid_extend = sylvan::cache_next_opid();
    ctx.opid_keep = sylvan::cache_next_opid();
    ctx.opid_change = sylvan::cache_next_opid();
    ctx.opid_labels = sylvan::cache_next_opid();
    ctx.opid_relprod = sylvan::cache_next_opid();
    ctx.opid_downs = sylvan::cache_next_opid();
    ctx.opid_learned = sylvan::cache_next_opid();
  }
}

/// \brief The states reachable from X with the groups of ctx, which are learned on the fly through ctx.expand.
/// Must be called from a Lace worker (or with Lace started); the recursion runs in parallel.
inline sylvan::ldds::ldd reach(reach_context& ctx, const sylvan::ldds::ldd& X)
{
  ctx.id++;
  ctx.top_iterations = 0;
  if (ctx.nvars == 0)
  {
    return X;
  }
  return sylvan::ldds::ldd(RUN(detail::reach_go, &ctx, X.get(), sylvan::lddmc_false, 0));
}

} // namespace mcrl2::symbolic

#endif // MCRL2_ENABLE_SYLVAN

#endif // MCRL2_SYMBOLIC_REACH_H
