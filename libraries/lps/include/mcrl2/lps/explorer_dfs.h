// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/explorer_dfs.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_EXPLORER_DFS_H
#define MCRL2_LPS_EXPLORER_DFS_H

#ifndef MCRL2_LPS_EXPLORER_H
#include "mcrl2/lps/explorer.h"
#endif

namespace mcrl2::lps
{
  template <bool Stochastic, bool Timed, typename Specification>
  template <
      typename SummandSequence,
      typename DiscoverState,
      typename ExamineTransition,
      typename TreeEdge,
      typename BackEdge,
      typename ForwardOrCrossEdge,
      typename FinishState
    >
    void explorer<Stochastic, Timed, Specification>::generate_state_space_dfs_recursive(
      const state& s0,
      std::unordered_set<state> gray,
      std::unordered_set<state>& discovered,
      const SummandSequence& regular_summands,
      const SummandSequence& confluent_summands,
      DiscoverState discover_state,
      ExamineTransition examine_transition,
      TreeEdge tree_edge,
      BackEdge back_edge,
      ForwardOrCrossEdge forward_or_cross_edge,
      FinishState finish_state
    )
    {
      using utilities::detail::contains;

      // invariants:
      // - s not in discovered => color(s) = white
      // - s in discovered && s in gray => color(s) = gray
      // - s in discovered && s not in gray => color(s) = black

      gray.insert(s0);
      discovered.insert(s0);
      discover_state(0, s0);

      if (m_options.number_of_threads>1)
      {
        throw mcrl2::runtime_error("Dfs exploration is not thread safe.");
      }

      for (const transition& tr: out_edges(s0, regular_summands, confluent_summands, m_global_sigma, m_global_rewr, m_global_enumerator, m_global_id_generator))
      {
        if (m_must_abort)
        {
          break;
        }

        const auto&[a, s1] = tr;
        const std::size_t number_of_threads = 1;
        examine_transition(0, number_of_threads, s0, a, s1); // TODO MAKE THREAD SAFE

        if (discovered.find(s1) == discovered.end())
        {
          tree_edge(s0, a, s1);
          if constexpr (Timed)
          {
            const data::data_expression& t = s0[m_n];
            const data::data_expression& t1 = a.has_time() ? a.time() : t;
            state s1_at_t1;
            make_timed_state(s1_at_t1, s1, t1);
            discovered.insert(s1_at_t1);
          }
          else
          {
            discovered.insert(s1);
          }
          generate_state_space_dfs_recursive(s1, gray, discovered, regular_summands, confluent_summands, discover_state, examine_transition, tree_edge, back_edge, forward_or_cross_edge, finish_state);
        }
        else if (contains(gray, s1))
        {
          back_edge(s0, a, s1);
        }
        else
        {
          forward_or_cross_edge(s0, a, s1);
        }
      }
      gray.erase(s0);

      finish_state(0, s0); // TODO MAKE THREAD SAFE
    }

    template <bool Stochastic, bool Timed, typename Specification>
    template <
      typename DiscoverState,
      typename ExamineTransition,
      typename TreeEdge,
      typename BackEdge,
      typename ForwardOrCrossEdge,
      typename FinishState
    >
    void explorer<Stochastic, Timed, Specification>::generate_state_space_dfs_recursive(
      bool recursive,
      DiscoverState discover_state,
      ExamineTransition examine_transition,
      TreeEdge tree_edge,
      BackEdge back_edge,
      ForwardOrCrossEdge forward_or_cross_edge,
      FinishState finish_state
    )
    {
      m_recursive = recursive;
      std::unordered_set<state> gray;
      std::unordered_set<state> discovered;

      state s0;
      compute_state(s0, m_initial_state, m_global_sigma, m_global_rewr);
      if (!m_confluent_summands.empty())
      {
        s0 = find_representative(s0, m_confluent_summands, m_global_sigma, m_global_rewr, m_global_enumerator, m_global_id_generator);
      }
      if constexpr (Timed)
      {
        s0 = make_timed_state(s0, data::sort_real::real_zero());
      }
      generate_state_space_dfs_recursive(s0, gray, discovered, m_regular_summands, m_confluent_summands, discover_state, examine_transition, tree_edge, back_edge, forward_or_cross_edge, finish_state);
      m_recursive = false;
    }

    template <bool Stochastic, bool Timed, typename Specification>
    template <
      typename SummandSequence,
      typename DiscoverState,
      typename ExamineTransition,
      typename TreeEdge,
      typename BackEdge,
      typename ForwardOrCrossEdge,
      typename FinishState
    >
    void explorer<Stochastic, Timed, Specification>::generate_state_space_dfs_iterative(
      const state& s0,
      std::unordered_set<state>& discovered,
      const SummandSequence& regular_summands,
      const SummandSequence& confluent_summands,
      DiscoverState discover_state,
      ExamineTransition examine_transition,
      TreeEdge tree_edge,
      BackEdge back_edge,
      ForwardOrCrossEdge forward_or_cross_edge,
      FinishState finish_state
    )
    {
      using utilities::detail::contains;

      // invariants:
      // - s not in discovered => color(s) = white
      // - s in discovered && s in todo => color(s) = gray
      // - s in discovered && s not in todo => color(s) = black

      std::vector<std::pair<state, std::list<transition>>> todo;

      if (m_options.number_of_threads>0)
      {
        throw mcrl2::runtime_error("DFS exploration cannot be performed with multiple threads.");
      }
      todo.emplace_back(s0, out_edges(s0, regular_summands, confluent_summands, m_global_sigma, m_global_rewr, m_global_enumerator, m_global_id_generator));
      discovered.insert(s0);
      discover_state(s0);

      while (!todo.empty() && !m_must_abort)
      {
        const state* s = &todo.back().first;
        std::list<transition>* E = &todo.back().second;
        while (!E->empty())
        {
          transition e = E->front();
          const auto& a = e.action;
          const auto& s1 = e.state;
          E->pop_front();
          examine_transition(0, 1, *s, a, s1); // TODO: MAKE THREAD SAFE.

          if (discovered.find(s1) == discovered.end())
          {
            tree_edge(*s, a, s1);
            if constexpr (Timed)
            {
              const data::data_expression& t = (*s)[m_n];
              const data::data_expression& t1 = a.has_time() ? a.time() : t;
              state s1_at_t1;
              make_timed_state(s1_at_t1, s1, t1);
              discovered.insert(s1_at_t1);
              discover_state(s1_at_t1);
            }
            else
            {
              discovered.insert(s1);
              discover_state(s1);
            }
            todo.emplace_back(s1, out_edges(s1, regular_summands, confluent_summands, m_global_sigma, m_global_rewr, m_global_enumerator, m_global_id_generator));
            s = &todo.back().first;
            E = &todo.back().second;
          }
          else
          {
            if (std::find_if(todo.begin(), todo.end(), [&](const std::pair<state, std::list<transition>>& p) { return s1 == p.first; }) != todo.end())
            {
              back_edge(*s, a, s1);
            }
            else
            {
              forward_or_cross_edge(*s, a, s1);
            }
          }
        }
        todo.pop_back();
        finish_state(0, *s);  // TODO: Make thread safe
      }
      m_must_abort = false;
    }

    template <bool Stochastic, bool Timed, typename Specification>
    template <
      typename DiscoverState,
      typename ExamineTransition,
      typename TreeEdge,
      typename BackEdge,
      typename ForwardOrCrossEdge,
      typename FinishState
    >
    void explorer<Stochastic, Timed, Specification>::generate_state_space_dfs_iterative(
      bool recursive,
      DiscoverState discover_state,
      ExamineTransition examine_transition,
      TreeEdge tree_edge,
      BackEdge back_edge,
      ForwardOrCrossEdge forward_or_cross_edge,
      FinishState finish_state
    )
    {
      m_recursive = recursive;
      std::unordered_set<state> discovered;

      state s0;
      compute_state(s0,m_initial_state,m_global_sigma, m_global_rewr);
      if (!m_confluent_summands.empty())
      {
        s0 = find_representative(s0, m_confluent_summands, m_global_sigma, m_global_rewr, m_global_enumerator, m_global_id_generator);
      }
      if constexpr (Timed)
      {
        s0 = make_timed_state(s0, data::sort_real::real_zero());
      }
      generate_state_space_dfs_iterative(s0, discovered, m_regular_summands, m_confluent_summands, discover_state, examine_transition, tree_edge, back_edge, forward_or_cross_edge, finish_state);
      m_recursive = false;
    }

} // namespace mcrl2::lps

#endif // MCRL2_LPS_EXPLORER_DFS_H
