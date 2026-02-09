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

#ifndef MCRL2_LPS_EXPLORER_BFS_H
#define MCRL2_LPS_EXPLORER_BFS_H

#ifndef MCRL2_LPS_EXPLORER_H
#include "mcrl2/lps/explorer.h"
#endif

namespace mcrl2::lps
{
    template <bool Stochastic, bool Timed, typename Specification>
    template <
      typename StateType,
      typename SummandSequence,
      typename DiscoverState,
      typename ExamineTransition,
      typename StartState,
      typename FinishState,
      typename DiscoverInitialState
    >
    void explorer<Stochastic, Timed, Specification>::generate_state_space_thread(
      std::unique_ptr<todo_set>& todo,
      const std::size_t thread_index,
      std::atomic<std::size_t>& number_of_active_processes,
      std::atomic<std::size_t>& number_of_idle_processes,
      const SummandSequence& regular_summands,
      const SummandSequence& confluent_summands,
      indexed_set_for_states_type& discovered,
      DiscoverState discover_state,
      ExamineTransition examine_transition,
      StartState start_state,
      FinishState finish_state,
      data::rewriter thread_rewr,
      data::mutable_indexed_substitution<> thread_sigma  // This is intentionally a copy. 
    )
    {
      thread_rewr.thread_initialise();
      mCRL2log(log::debug) << "Start thread " << thread_index << ".\n";
      data::enumerator_identifier_generator thread_id_generator("t_");;
      data::data_specification thread_data_specification = m_global_lpsspec.data(); /// XXXX Nodig??
      data::enumerator_algorithm<> thread_enumerator(thread_rewr, thread_data_specification, thread_rewr, thread_id_generator, false);
      state current_state;
      data::data_expression condition;   // The condition is used often, and it is effective not to declare it whenever it is used.
      state_type state_;                 // The same holds for state.
      std::vector<state> dummy;
      std::unique_ptr<todo_set> thread_todo=make_todo_set(dummy.begin(),dummy.end()); // The new states for each process are temporarily stored in this vector for each thread. 
      atermpp::aterm key;

      if (mcrl2::utilities::detail::GlobalThreadSafe && m_options.number_of_threads > 1)
      {
        m_exclusive_state_access.lock();
      }
      while (number_of_active_processes>0 || !todo->empty())
      {
        assert(m_must_abort || thread_todo->empty());
          
        if (!todo->empty())
        {
          todo->choose_element(current_state);
          thread_todo->insert(current_state);
          if (mcrl2::utilities::detail::GlobalThreadSafe && m_options.number_of_threads > 1)
          {
            m_exclusive_state_access.unlock();
          }

          while (!thread_todo->empty() && !m_must_abort.load(std::memory_order_relaxed))
          { 
            thread_todo->choose_element(current_state);
            std::size_t s_index = discovered.index(current_state,thread_index);
            start_state(thread_index, current_state, s_index);
            data::add_assignments(thread_sigma, m_process_parameters, current_state);
            for (const explorer_summand& summand: regular_summands)
            {   
              generate_transitions(
                summand,
                confluent_summands,
                thread_sigma,
                thread_rewr,
                condition,
                state_,
                key,
                thread_enumerator,
                thread_id_generator,
                [&](const lps::multi_action& a, const state_type& s1)
                {   
                  if constexpr (Timed)
                  { 
                    const data::data_expression& t = current_state[m_n];
                    if (a.has_time() && less_equal(a.time(), t, thread_sigma, thread_rewr))
                    {
                      return;
                    }
                  } 
                  if constexpr (Stochastic)
                  { 
                    std::list<std::size_t> s1_index;
                    const auto& S1 = s1.states;
                    // TODO: join duplicate targets
                    for (const state& s1_: S1)
                    { 
                      std::size_t k = discovered.index(s1_,thread_index);
                      if (k >= discovered.size())
                      { 
                        thread_todo->insert(s1_);
                        k = discovered.insert(s1_, thread_index).first;
                        discover_state(thread_index, s1_, k);
                      }
                      s1_index.push_back(k);
                    }

                    examine_transition(thread_index, m_options.number_of_threads, current_state, s_index, a, s1, s1_index, summand.index);
                  } 
                  else 
                  { 
                    std::size_t s1_index; 
                    if constexpr (Timed)
                    { 
                      s1_index = discovered.index(s1,thread_index);
                      if (s1_index >= discovered.size())
                      {   
                        const data::data_expression& t = current_state[m_n];
                        const data::data_expression& t1 = a.has_time() ? a.time() : t;
                        make_timed_state(state_, s1, t1);
                        s1_index = discovered.insert(state_, thread_index).first;
                        discover_state(thread_index, state_, s1_index);
                        thread_todo->insert(state_);
                      } 
                    }
                    else
                    { 
                      std::pair<std::size_t,bool> p = discovered.insert(s1, thread_index);
                      s1_index=p.first;
                      if (p.second)  // Index is newly added. 
                      {
                        discover_state(thread_index, s1, s1_index);
                        thread_todo->insert(s1); 
                      }
                    }

                    examine_transition(thread_index, m_options.number_of_threads, current_state, s_index, a, s1, s1_index, summand.index);
                  }
                }
              );
            }

            if (number_of_idle_processes>0 && thread_todo->size()>1)
            {
              if (mcrl2::utilities::detail::GlobalThreadSafe && m_options.number_of_threads > 1)
              {
                m_exclusive_state_access.lock();
              }

              if (todo->size() < m_options.number_of_threads) 
              {
                // move 25% of the states of this thread to the global todo buffer.
                for(std::size_t i=0; i<std::min(thread_todo->size()-1,1+(thread_todo->size()/4)); ++i)  
                {
                  thread_todo->choose_element(current_state);
                  todo->insert(current_state);
                }
              }

              if (mcrl2::utilities::detail::GlobalThreadSafe && m_options.number_of_threads > 1)
              {
                m_exclusive_state_access.unlock();
              }
            }

            finish_state(thread_index, m_options.number_of_threads, current_state, s_index, thread_todo->size());
            thread_todo->finish_state();
          }
        }
        else
        {
          if (mcrl2::utilities::detail::GlobalThreadSafe && m_options.number_of_threads > 1)
          {
            m_exclusive_state_access.unlock();
          }
        }
        // Check whether all processes are ready. If so the number_of_active_processes becomes 0. 
        // Otherwise, this thread becomes active again, and tries to see whether the todo buffer is
        // not empty, to take up more work. 
        number_of_active_processes--;
        number_of_idle_processes++;
        if (mcrl2::utilities::detail::GlobalThreadSafe && m_options.number_of_threads > 1)
        {
          m_exclusive_state_access.lock();
        }

        assert(thread_todo->empty() || m_must_abort);
        if (todo->empty())
        {
          if (mcrl2::utilities::detail::GlobalThreadSafe && m_options.number_of_threads > 1)
          {
            m_exclusive_state_access.unlock();
          }
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
          if (mcrl2::utilities::detail::GlobalThreadSafe && m_options.number_of_threads > 1)
          {
            m_exclusive_state_access.lock();
          }
        }
        if (number_of_active_processes>0 || !todo->empty())
        {
          number_of_active_processes++;
        }
        number_of_idle_processes--;
      } 
      mCRL2log(log::debug) << "Stop thread " << thread_index << ".\n";
      if (mcrl2::utilities::detail::GlobalThreadSafe && m_options.number_of_threads > 1)
      {
        m_exclusive_state_access.unlock();
      }

    }  // end generate_state_space_thread.

    template <bool Stochastic, bool Timed, typename Specification>
    template <
      typename StateType,
      typename SummandSequence,
      typename DiscoverState,
      typename ExamineTransition,
      typename StartState,
      typename FinishState,
      typename DiscoverInitialState
    >
    void explorer<Stochastic, Timed, Specification>::generate_state_space(
      bool recursive,
      const StateType& s0,
      const SummandSequence& regular_summands,
      const SummandSequence& confluent_summands,
      indexed_set_for_states_type& discovered,
      DiscoverState discover_state,
      ExamineTransition examine_transition,
      StartState start_state,
      FinishState finish_state,
      [[maybe_unused]]
      DiscoverInitialState discover_initial_state
    )
    {
      const std::size_t number_of_threads=m_options.number_of_threads;
      assert(number_of_threads>0);
      const std::size_t initialisation_thread_index= (number_of_threads==1?0:1);
      m_recursive = recursive;
      std::unique_ptr<todo_set> todo;
      discovered.clear(initialisation_thread_index);

      if constexpr (Stochastic)
      {
        state_type s0_ = make_state(s0);
        const auto& S = s0_.states;
        todo = make_todo_set(S.begin(), S.end());
        discovered.clear(initialisation_thread_index);
        std::list<std::size_t> s0_index;
        for (const state& s: S)
        {
          // TODO: join duplicate targets
          std::size_t s_index = discovered.index(s, initialisation_thread_index);
          if (s_index >= discovered.size())
          {
            s_index = discovered.insert(s, initialisation_thread_index).first;
            discover_state(initialisation_thread_index, s, s_index);
          }
          s0_index.push_back(s_index);
        }
        discover_initial_state(s0_, s0_index);
      }
      else
      {
        todo = make_todo_set(s0);
        std::size_t s0_index = discovered.insert(s0, initialisation_thread_index).first;
        discover_state(initialisation_thread_index, s0, s0_index);
      }

      std::atomic<std::size_t> number_of_active_processes=number_of_threads;
      std::atomic<std::size_t> number_of_idle_processes=0;

      if (number_of_threads>1)
      {
        std::vector<std::thread> threads;
        threads.reserve(number_of_threads);
        for(std::size_t i=1; i<=number_of_threads; ++i)  // Threads are numbered from 1 to number_of_threads. Thread number 0 is reserved as 
                                                         // indicator for a sequential implementation. 
        {
          threads.emplace_back([&, i](){ 
                                    generate_state_space_thread< StateType, SummandSequence,
                                                         DiscoverState, ExamineTransition,
                                                         StartState, FinishState,
                                                         DiscoverInitialState >
                                       (todo, 
                                        i, number_of_active_processes, number_of_idle_processes,
                                        regular_summands,confluent_summands,discovered, discover_state,
                                        examine_transition, start_state, finish_state, 
                                        m_global_rewr.clone(), m_global_sigma); } );  // It is essential that the rewriter is cloned as
                                                                                      // one rewriter cannot be used in parallel.
        }

        for(std::size_t i=1; i<=number_of_threads; ++i)
        {
          threads[i-1].join();
        }
      }
      else
      {
        // Single threaded variant. Do not start a separate thread. 
        assert(number_of_threads==1);
        const std::size_t single_thread_index=0;
        generate_state_space_thread< StateType, SummandSequence,
                                                DiscoverState, ExamineTransition,
                                                StartState, FinishState,
                                                DiscoverInitialState >
                                  (todo,single_thread_index,number_of_active_processes, number_of_idle_processes,
                                   regular_summands,confluent_summands,discovered, discover_state,
                                   examine_transition, start_state, finish_state, 
                                   m_global_rewr, m_global_sigma);  
      }

      m_must_abort = false;
    }

    template <bool Stochastic, bool Timed, typename Specification>
    template <
      typename DiscoverState,
      typename ExamineTransition,
      typename StartState,
      typename FinishState,
      typename DiscoverInitialState
    >
    void explorer<Stochastic, Timed, Specification>::generate_state_space(
      bool recursive,
      DiscoverState discover_state,
      ExamineTransition examine_transition,
      StartState start_state,
      FinishState finish_state,
      DiscoverInitialState discover_initial_state
    )
    {
      state_type s0;
      if constexpr (Stochastic)
      {
        compute_initial_stochastic_state(s0);
      }
      else
      {
        compute_state(s0,m_initial_state,m_global_sigma, m_global_rewr);
        if (!m_confluent_summands.empty())
        {
          s0 = find_representative(s0, m_confluent_summands, m_global_sigma, m_global_rewr, m_global_enumerator, m_global_id_generator);
        }
        if constexpr (Timed)
        {
          make_timed_state(s0, s0, data::sort_real::real_zero());
        }
      }
      generate_state_space(recursive, s0, m_regular_summands, m_confluent_summands, m_discovered, discover_state, 
                           examine_transition, start_state, finish_state, discover_initial_state);
    }

} // namespace mcrl2::lps

#endif // MCRL2_LPS_EXPLORER_BFS_H
