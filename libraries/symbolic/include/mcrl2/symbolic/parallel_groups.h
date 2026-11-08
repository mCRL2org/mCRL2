// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/symbolic/parallel_groups.h
/// \brief Applies the transition relations of all summand groups to one set in parallel,
///        combining the results in a parallel tree of unions (as go_par in Sylvan's lddmc example).

#ifndef MCRL2_SYMBOLIC_PARALLEL_GROUPS_H
#define MCRL2_SYMBOLIC_PARALLEL_GROUPS_H

#ifdef MCRL2_ENABLE_SYLVAN

#include "mcrl2/symbolic/summand_group.h"

#include <sylvan_ldd.hpp>

#include <vector>

namespace mcrl2::symbolic {

namespace detail {

/// \brief The input of a parallel pass over the summand groups, as raw MDDs.
/// The caller keeps all of them alive (they are protected ldd objects), and
/// none of them changes during the pass.
struct parallel_groups_context
{
  bool backward = false;                    // false: relprod(X, L, Ir); true: relprev(X, L, Ir, U)
  sylvan::MDD X = sylvan::lddmc_false;      // the set to which the relations are applied
  sylvan::MDD U = sylvan::lddmc_false;      // backward: the predecessors are restricted to U
  sylvan::MDD minus = sylvan::lddmc_false;  // forward: if not false, every leaf removes these states from its successors
  std::vector<sylvan::MDD> L;
  std::vector<sylvan::MDD> Ir;
  std::vector<sylvan::MDD> Ip;              // projection: the meta data of project, per group
  std::vector<sylvan::MDD> avoid;           // projection: if not empty, the result excludes these (project_minus), per group
};

// The Lace task macros define functions that are not inline, so give them
// internal linkage to keep this header usable in more than one translation unit.
namespace {

using namespace sylvan;

/// \brief Returns the union over the groups i in [from, from + len) of relprod(X, L[i], Ir[i])
/// (forward) or relprev(X, L[i], Ir[i], U) (backward).
/// Forward only: if enabled is not null, it also sets *enabled to the states of X that have a
/// successor in one of these groups. *enabled must be protected by the caller.
TASK_4(MDD, parallel_groups_apply, const parallel_groups_context*, ctx, std::size_t, from, std::size_t, len, MDD*, enabled)
{
  if (len == 1)
  {
    if (ctx->backward)
    {
      return CALL(lddmc_relprev, ctx->X, ctx->L[from], ctx->Ir[from], ctx->U);
    }
    MDD succ = CALL(lddmc_relprod, ctx->X, ctx->L[from], ctx->Ir[from]);
    if (enabled != nullptr)
    {
      lddmc_refs_push(succ);
      *enabled = CALL(lddmc_relprev, succ, ctx->L[from], ctx->Ir[from], ctx->X);
      lddmc_refs_pop(1);
    }
    if (ctx->minus != lddmc_false)
    {
      lddmc_refs_push(succ);
      succ = CALL(lddmc_minus, succ, ctx->minus);
      lddmc_refs_pop(1);
    }
    return succ;
  }

  if (enabled != nullptr)
  {
    MDD enabled_left = lddmc_false;
    MDD enabled_right = lddmc_false;
    lddmc_refs_pushptr(&enabled_left);
    lddmc_refs_pushptr(&enabled_right);

    lddmc_refs_spawn(SPAWN(parallel_groups_apply, ctx, from, len / 2, &enabled_left));
    MDD right = CALL(parallel_groups_apply, ctx, from + len / 2, len - len / 2, &enabled_right);
    lddmc_refs_push(right);
    MDD left = lddmc_refs_sync(SYNC(parallel_groups_apply));
    lddmc_refs_push(left);

    // Both unions at once.
    lddmc_refs_spawn(SPAWN(lddmc_union, enabled_left, enabled_right));
    MDD result = CALL(lddmc_union, left, right);
    lddmc_refs_push(result);
    *enabled = lddmc_refs_sync(SYNC(lddmc_union));

    lddmc_refs_pop(3);
    lddmc_refs_popptr(2);
    return result;
  }

  lddmc_refs_spawn(SPAWN(parallel_groups_apply, ctx, from, len / 2, nullptr));
  MDD right = CALL(parallel_groups_apply, ctx, from + len / 2, len - len / 2, nullptr);
  lddmc_refs_push(right);
  MDD left = lddmc_refs_sync(SYNC(parallel_groups_apply));
  lddmc_refs_push(left);
  MDD result = CALL(lddmc_union, left, right);
  lddmc_refs_pop(2);
  return result;
}

/// \brief Sets out[i] to project(X, Ip[i]) minus avoid[i] (if avoid is given) for the groups i in [from, from + len).
/// The caller protects the entries of out.
VOID_TASK_4(parallel_groups_project, const parallel_groups_context*, ctx, std::size_t, from, std::size_t, len, MDD*, out)
{
  if (len == 1)
  {
    out[from] = ctx->avoid.empty() ? CALL(lddmc_project, ctx->X, ctx->Ip[from])
                                   : CALL(lddmc_project_minus, ctx->X, ctx->Ip[from], ctx->avoid[from]);
    return;
  }
  SPAWN(parallel_groups_project, ctx, from, len / 2, out);
  CALL(parallel_groups_project, ctx, from + len / 2, len - len / 2, out);
  SYNC(parallel_groups_project);
}

} // namespace

template <typename SummandGroup>
parallel_groups_context make_parallel_groups_context(const std::vector<SummandGroup>& groups)
{
  parallel_groups_context context;
  for (const auto& group: groups)
  {
    context.L.push_back(group.L.get());
    context.Ir.push_back(group.Ir.get());
  }
  return context;
}

} // namespace detail

/// \brief Returns the union over all groups of relprod(X, group.L, group.Ir), computed in parallel.
/// If enabled is not null, it is set to the states of X that have a successor (for deadlock detection).
/// Must be called from a Lace worker (for example inside the task that a tool runs with RUN).
/// If minus is not empty, the result excludes the states of minus (each group removes them from its successors before the unions).
template <typename SummandGroup>
sylvan::ldds::ldd relprod_groups_parallel(const sylvan::ldds::ldd& X, const std::vector<SummandGroup>& groups, sylvan::ldds::ldd* enabled = nullptr,
                                          const sylvan::ldds::ldd& minus = sylvan::ldds::empty_set())
{
  using sylvan::ldds::ldd;
  if (groups.empty())
  {
    if (enabled != nullptr)
    {
      *enabled = sylvan::ldds::empty_set();
    }
    return sylvan::ldds::empty_set();
  }

  detail::parallel_groups_context context = detail::make_parallel_groups_context(groups);
  context.X = X.get();
  context.minus = minus.get();

  sylvan::MDD enabled_mdd = sylvan::lddmc_false;
  sylvan::lddmc_refs_pushptr(&enabled_mdd);
  ldd result(RUN(detail::parallel_groups_apply, &context, 0, groups.size(), enabled != nullptr ? &enabled_mdd : nullptr));
  if (enabled != nullptr)
  {
    *enabled = ldd(enabled_mdd);
  }
  sylvan::lddmc_refs_popptr(1);
  return result;
}

/// \brief Returns the projections project(X, group.Ip) of all groups, computed in parallel. If cached is true,
/// each projection excludes the values that the group already learned: project_minus(X, group.Ip, group.Ldomain).
/// Must be called from a Lace worker.
template <typename SummandGroup>
std::vector<sylvan::ldds::ldd> project_groups_parallel(const sylvan::ldds::ldd& X, const std::vector<SummandGroup>& groups, bool cached)
{
  using sylvan::ldds::ldd;
  detail::parallel_groups_context context;
  context.X = X.get();
  for (const auto& group: groups)
  {
    context.Ip.push_back(group.Ip.get());
    if (cached)
    {
      context.avoid.push_back(group.Ldomain.get());
    }
  }

  std::vector<ldd> result(groups.size());
  if (groups.empty())
  {
    return result;
  }

  // The tasks write raw MDDs; protect them on this worker's refs stack until they are in ldd objects.
  std::vector<sylvan::MDD> out(groups.size(), sylvan::lddmc_false);
  for (const sylvan::MDD& x: out)
  {
    sylvan::lddmc_refs_pushptr(&x);
  }
  RUN(detail::parallel_groups_project, &context, 0, groups.size(), out.data());
  for (std::size_t i = 0; i < groups.size(); i++)
  {
    result[i] = ldd(out[i]);
  }
  sylvan::lddmc_refs_popptr(groups.size());
  return result;
}

/// \brief Returns { u in U | exists v in V: u -> v } for the union of the relations of all groups,
/// that is, the union over all groups of relprev(V, group.L, group.Ir, U), computed in parallel.
/// Must be called from a Lace worker.
template <typename SummandGroup>
sylvan::ldds::ldd relprev_groups_parallel(const sylvan::ldds::ldd& U, const sylvan::ldds::ldd& V, const std::vector<SummandGroup>& groups)
{
  using sylvan::ldds::ldd;
  if (groups.empty())
  {
    return sylvan::ldds::empty_set();
  }

  detail::parallel_groups_context context = detail::make_parallel_groups_context(groups);
  context.backward = true;
  context.X = V.get();
  context.U = U.get();
  return ldd(RUN(detail::parallel_groups_apply, &context, 0, groups.size(), nullptr));
}

} // namespace mcrl2::symbolic

#endif // MCRL2_ENABLE_SYLVAN

#endif // MCRL2_SYMBOLIC_PARALLEL_GROUPS_H
