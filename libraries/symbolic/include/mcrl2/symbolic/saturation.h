// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/symbolic/saturation.h
/// \brief Recursive saturation with on-the-fly learning of the transition relations, sequential or with the
///        recursion in parallel (T. van Dijk, J. Meijer, J. van de Pol: Multi-core on-the-fly saturation, TACAS 2019).

#ifndef MCRL2_SYMBOLIC_SATURATION_H
#define MCRL2_SYMBOLIC_SATURATION_H

#ifdef MCRL2_ENABLE_SYLVAN

#include "mcrl2/symbolic/summand_group.h"

#include <sylvan_ldd.hpp>

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <limits>
#include <vector>

namespace mcrl2::symbolic {

/// \brief One transition relation (a summand group) as saturation sees it.
struct saturation_relation
{
  std::size_t topvar = 0;          // the first state variable that the group reads or writes
  sylvan::ldds::ldd topmeta;       // the relprod meta of the group, from topvar on
  sylvan::ldds::ldd topread;       // the projection on the read variables of the group, from topvar on
  std::size_t group = 0;           // the index of the group
  const sylvan::ldds::ldd* L = nullptr; // the learned relation of the group; it grows while saturation runs
  const sylvan::ldds::ldd* Ldomain = nullptr; // the projected states for which L has been learned
};

/// \brief Computes topvar, topmeta and topread of a summand group.
/// \returns false if the group reads and writes no variable (then it cannot change a state).
inline bool make_saturation_relation(const summand_group& group, std::size_t index, saturation_relation& result)
{
  using namespace sylvan::ldds;
  if (group.read.empty() && group.write.empty())
  {
    return false;
  }
  std::size_t topvar = std::numeric_limits<std::size_t>::max();
  if (!group.read.empty())
  {
    topvar = std::min(topvar, group.read.front());
  }
  if (!group.write.empty())
  {
    topvar = std::min(topvar, group.write.front());
  }

  // Ir has one entry 0 for every variable before the first read or written one; drop those.
  std::vector<std::uint32_t> meta(group.Ir.begin(), group.Ir.end());
  assert(meta.size() > topvar);
  for (std::size_t i = 0; i < topvar; i++)
  {
    assert(meta[i] == 0);
  }
  std::vector<std::uint32_t> topmeta(meta.begin() + topvar, meta.end());

  // 1 (keep) for the read variables, 0 (quantify) for the others, -2 (quantify the rest) after the last read one.
  std::vector<std::uint32_t> topread;
  if (group.read.empty())
  {
    topread.push_back(static_cast<std::uint32_t>(-2));
  }
  else
  {
    for (std::size_t i = topvar; i <= group.read.back(); i++)
    {
      topread.push_back(std::find(group.read.begin(), group.read.end(), i) != group.read.end() ? 1 : 0);
    }
    topread.push_back(static_cast<std::uint32_t>(-2));
  }

  result.topvar = topvar;
  result.topmeta = cube(topmeta);
  result.topread = cube(topread);
  result.group = index;
  result.L = &group.L;
  result.Ldomain = &group.Ldomain;
  return true;
}

/// \brief The input of a saturation run.
struct saturation_context
{
  std::vector<saturation_relation> rels;  // sorted on topvar
  std::uint64_t opid = 0;                 // Sylvan operation cache id of this saturation
  std::uint64_t id = 0;                   // changes for every call, because the relations grow in between
  bool parallel = false;                  // recurse in parallel (then expand must be thread-safe)

  /// \brief Learns the transitions of group for the projected states X (over the read variables of the group).
  /// Called on any Lace worker; X is protected by the caller.
  void (*expand)(void* expand_context, std::size_t group, sylvan::MDD X) = nullptr;
  void* expand_context = nullptr;
};

namespace detail {

// The Lace task macros define functions that are not inline, so give them
// internal linkage to keep this header usable in more than one translation unit.
namespace {

using namespace ::sylvan;

/// \brief Saturates set (an LDD whose first level is state variable depth) with the relations
/// ctx->rels[first, first + count), which all have topvar >= depth.
TASK_5(MDD, symbolic_go_sat, const saturation_context*, ctx, MDD, set, std::size_t, first, std::size_t, depth, std::size_t, count)
{
  /* Terminal cases */
  if (set == lddmc_false)
  {
    return lddmc_false;
  }
  if (count == 0)
  {
    return set;
  }
  assert(set != lddmc_true);

  /* Consult the cache */
  MDD result;
  MDD _set = set;
  if (cache_get3(ctx->opid, _set, first, ctx->id, &result))
  {
    return result;
  }
  lddmc_refs_pushptr(&_set);

  const std::size_t var = ctx->rels[first].topvar;
  assert(depth <= var);
  if (depth == var)
  {
    /* The relations that start at this level */
    std::size_t n = 1;
    while (n < count && ctx->rels[first + n].topvar == var)
    {
      n++;
    }
    /* Until a fixpoint: saturate deeper, then learn and apply each relation of this level once (chaining) */
    MDD prev = lddmc_false;
    lddmc_refs_pushptr(&set);
    lddmc_refs_pushptr(&prev);
    while (prev != set)
    {
      prev = set;
      set = CALL(symbolic_go_sat, ctx, set, first + n, depth, count - n);
      for (std::size_t i = 0; i < n; i++)
      {
        const saturation_relation& r = ctx->rels[first + i];
        if (ctx->expand != nullptr)
        {
          // Only call expand for the projections that the group has not learned yet; most calls find none. This check
          // uses raw MDDs (on the refs stack of this worker): ldd objects would each protect and unprotect themselves
          // in Sylvan's global protection table, whose shared control word all workers contend on.
          // project, then minus (not project_minus): the cache key of project does not contain the learned domain,
          // which changes all the time, so its results are reused across calls.
          MDD X = lddmc_refs_push(CALL(lddmc_project, set, r.topread.get()));
          MDD domain = lddmc_refs_push(r.Ldomain->load());
          MDD Y = lddmc_refs_push(CALL(lddmc_minus, X, domain));
          if (Y != lddmc_false)
          {
            ctx->expand(ctx->expand_context, r.group, Y);
          }
          lddmc_refs_pop(3);
        }
        // Read the relation after learning; protect it, since another worker may replace it meanwhile.
        MDD rel = r.L->load();
        lddmc_refs_push(rel);
        set = CALL(lddmc_relprod_union, set, rel, r.topmeta.get(), set);
        lddmc_refs_pop(1);
      }
    }
    lddmc_refs_popptr(2);
    result = set;
  }
  else if (ctx->parallel)
  {
    lddmc_refs_spawn(SPAWN(symbolic_go_sat, ctx, lddmc_getright(set), first, depth, count));
    MDD down = lddmc_refs_push(CALL(symbolic_go_sat, ctx, lddmc_getdown(set), first, depth + 1, count));
    MDD right = lddmc_refs_sync(SYNC(symbolic_go_sat));
    lddmc_refs_pop(1);
    result = lddmc_makenode(lddmc_getvalue(set), down, right);
  }
  else
  {
    MDD right = lddmc_refs_push(CALL(symbolic_go_sat, ctx, lddmc_getright(set), first, depth, count));
    MDD down = lddmc_refs_push(CALL(symbolic_go_sat, ctx, lddmc_getdown(set), first, depth + 1, count));
    lddmc_refs_pop(2);
    result = lddmc_makenode(lddmc_getvalue(set), down, right);
  }

  /* Store in cache */
  cache_put3(ctx->opid, _set, first, ctx->id, result);
  lddmc_refs_popptr(1);
  return result;
}

} // namespace

} // namespace detail

/// \brief Saturates X with the relations of ctx, learning them on the fly through ctx.expand.
/// One call may miss successors through transitions that were learned after a cached sub-result was computed, so
/// this repeats until X does not change, each time with a fresh cache id. Must be called from a Lace worker.
/// \returns The set of states reachable from X, and (in calls) the number of saturation passes.
inline sylvan::ldds::ldd saturate(saturation_context& ctx, const sylvan::ldds::ldd& X, std::size_t& calls)
{
  using sylvan::ldds::ldd;
  std::stable_sort(ctx.rels.begin(), ctx.rels.end(), [](const saturation_relation& a, const saturation_relation& b) { return a.topvar < b.topvar; });
  if (ctx.opid == 0)
  {
    ctx.opid = sylvan::cache_next_opid();
  }
  ldd result = X;
  calls = 0;
  for (;;)
  {
    ldd previous = result;
    ctx.id++;
    calls++;
    result = ldd(RUN(detail::symbolic_go_sat, &ctx, result.get(), 0, 0, ctx.rels.size()));
    if (result == previous)
    {
      return result;
    }
  }
}

} // namespace mcrl2::symbolic

#endif // MCRL2_ENABLE_SYLVAN

#endif // MCRL2_SYMBOLIC_SATURATION_H
