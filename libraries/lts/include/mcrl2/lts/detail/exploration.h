// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define USE_INDEXED_SET

#ifndef MCRL2_LTS_DETAIL_EXPLORATION_NEW_H
#define MCRL2_LTS_DETAIL_EXPLORATION_NEW_H

#include <string>
#include <limits>
#include <memory>

// #include "mcrl2/utilities/unordered_map.h"   // This version of an unordered set is more memory efficient
                                             // but it does not guarantee linear time traversal of its stored elements.
#include "unordered_map"   

#ifdef USE_INDEXED_SET
#include "mcrl2/atermpp/indexed_set.h"
#endif

#include "mcrl2/trace/trace.h"
#include "mcrl2/lps/next_state_generator.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/lts/detail/bithashtable.h"
#include "mcrl2/lts/detail/queue.h"
#include "mcrl2/lts/detail/lts_generation_options.h"
#include "mcrl2/lps/exploration_strategy.h"


namespace mcrl2
{
namespace lts
{
namespace detail
{

    template <class COUNTER_EXAMPLE_GENERATOR>
    class state_index_pair
    {
      protected:
        lps::state m_state;
        typename COUNTER_EXAMPLE_GENERATOR::index_type m_index;

      public:
        state_index_pair(const lps::state& state, typename COUNTER_EXAMPLE_GENERATOR::index_type index)
         : m_state(state),
           m_index(index)
        {}

        lps::state state() const
        {
          return m_state;
        }

        typename COUNTER_EXAMPLE_GENERATOR::index_type index() const
        {
          return m_index;
        }
    };

    // The class below is used to get a unique number for a multi-action which can contain time. 
    // The class is a little complex because a multi-action is a pair of actions and time, and
    // not by itself an aterm. 
    class multi_action_indexed_set 
    {
      protected:
#ifdef USE_INDEXED_SET
        atermpp::indexed_set<atermpp::aterm> storage;
#else
        // mcrl2::utilities::unordered_map<atermpp::aterm, std::size_t> storage;
        std::unordered_map<atermpp::aterm, std::size_t> storage;
#endif
      
      public:
        inline
        std::pair<std::size_t, bool> put(const lps::multi_action& ma) 
        {
          if (ma.time()==data::undefined_real())
          {
            // If the time is undefined, which means the multi-action can take place
            // at any time, we find a number based on the actions. 
#ifdef USE_INDEXED_SET
            return storage.put(ma.actions());
#else
            // std::pair<mcrl2::utilities::unordered_map<atermpp::aterm, std::size_t>::iterator, bool> result = storage.emplace(ma.actions(), storage.size()); 
            std::pair<std::unordered_map<atermpp::aterm, std::size_t>::iterator, bool> result = storage.emplace(ma.actions(), storage.size()); 
            return std::pair<std::size_t, bool>(result.first->second, result.second);
#endif
          }
          else 
          { 
            // When the time is non trivial we put the time as the first element of the 
            // list of actions. This is not very elegant but it works. 
            atermpp::aterm_list l=atermpp::down_cast<atermpp::aterm_list>(static_cast<const atermpp::aterm&>(ma.actions()));
            l.push_front(ma.time()); 
#ifdef USE_INDEXED_SET
            return storage.put(l);
#else
            // std::pair<mcrl2::utilities::unordered_map<atermpp::aterm, std::size_t>::iterator, bool> result = storage.emplace(l, storage.size()); 
            std::pair<std::unordered_map<atermpp::aterm, std::size_t>::iterator, bool> result = storage.emplace(l, storage.size()); 
            return std::pair<std::size_t, bool>(result.first->second, result.second);
#endif
          }
        }
    };

} // end namespace detail

class lps2lts_algorithm
{
  private:
    typedef lps::next_state_generator next_state_generator;

  private:
    lts_generation_options m_options;
    next_state_generator *m_generator;
    next_state_generator::summand_subset_t *m_main_subset;

    bool m_use_confluence_reduction;
    next_state_generator::summand_subset_t m_nonprioritized_subset;
    next_state_generator::summand_subset_t m_prioritized_subset;

#ifdef USE_INDEXED_SET
    atermpp::indexed_set<lps::state> m_state_numbers;
#else
    // mcrl2::utilities::unordered_map<lps::state, std::size_t>m_state_numbers;
    std::unordered_map<lps::state, std::size_t>m_state_numbers;
#endif

    bit_hash_table m_bit_hash_table;

    probabilistic_lts_lts_t m_output_lts;
    detail::multi_action_indexed_set m_action_label_numbers;
    std::ofstream m_aut_file;

    bool m_maintain_traces;
    bool m_value_prioritize;

    next_state_generator::summand_subset_t m_tau_summands;

    std::vector<bool> m_detected_action_summands;

    std::map<lps::state, lps::state> m_backpointers;
    std::size_t m_traces_saved;

    std::size_t m_num_states;
    std::size_t m_num_transitions;
    next_state_generator::transition_t::state_probability_list m_initial_states;
    std::size_t m_level;

    std::unordered_set<lps::state> non_divergent_states;  // This set is filled with states proven not to be divergent,
                                                          // when lps2lts_algorithm is requested to search for divergencies.

    volatile bool m_must_abort;

  public:
    lps2lts_algorithm() :
      m_generator(nullptr),
      m_must_abort(false)
    {
      m_action_label_numbers.put(action_label_lts::tau_action());  // The action tau has index 0 by default.
    }

    ~lps2lts_algorithm()
    {
      delete m_generator;
    }

    bool generate_lts(const lts_generation_options& options);

    void abort()
    {
      // Stops the exploration algorithm if it is running by making sure
      // not a single state can be generated anymore.
      if (!m_must_abort)
      {
        m_must_abort = true;
        mCRL2log(log::warning) << "state space generation was aborted prematurely" << std::endl;
      }
    }

  private:
    void initialise_lts_generation(const lts_generation_options& options);
    void finalise_lts_generation();
    data::data_expression_vector generator_state(const lps::state& storage_state);
    lps::state storage_state(const data::data_expression_vector& generator_state);
    void set_prioritised_representatives(next_state_generator::transition_t::state_probability_list& states);
    lps::state get_prioritised_representative(const lps::state& state1);
    void value_prioritize(std::vector<next_state_generator::transition_t>& transitions);
    bool save_trace(const lps::state& state1, const std::string& filename);
    bool save_trace(const lps::state& state1, const next_state_generator::transition_t& transition, const std::string& filename);
    void construct_trace(const lps::state& state1, mcrl2::trace::Trace& trace);

    bool is_nondeterministic(std::vector<lps2lts_algorithm::next_state_generator::transition_t>& transitions,
                             next_state_generator::transition_t& nondeterminist_transition);
    template <class COUNTER_EXAMPLE_GENERATOR>
    bool search_divergence(const detail::state_index_pair<COUNTER_EXAMPLE_GENERATOR>& state,
                           std::set<lps::state>& current_path, std::set<lps::state>& visited,
                           COUNTER_EXAMPLE_GENERATOR& divergence_loop);
    template <class COUNTER_EXAMPLE_GENERATOR>
    void check_divergence(const detail::state_index_pair<COUNTER_EXAMPLE_GENERATOR>& state,
                          COUNTER_EXAMPLE_GENERATOR divergence_loop);
    void save_actions(const lps::state& state, const next_state_generator::transition_t& transition);
    void save_deadlock(const lps::state& state);
    void save_nondeterministic_state(const lps::state& state, const next_state_generator::transition_t& nondeterminist_transition);
    void save_error(const lps::state& state);
    std::pair<std::size_t, bool> add_target_state(const lps::state& source_state, const lps::state& target_state);
    bool add_transition(const lps::state& source_state, const next_state_generator::transition_t& transition);
    void get_transitions(const lps::state& state,
                         std::vector<lps2lts_algorithm::next_state_generator::transition_t>& transitions,
                         next_state_generator::enumerator_queue_t& enumeration_queue
    );
    void generate_lts_breadth_todo_max_is_npos(const next_state_generator::transition_t::state_probability_list& initial_states);
    void generate_lts_breadth_todo_max_is_not_npos(const next_state_generator::transition_t::state_probability_list& initial_states);
    void generate_lts_breadth_bithashing(const next_state_generator::transition_t::state_probability_list& initial_states);
    void generate_lts_depth(const next_state_generator::transition_t::state_probability_list& initial_states);
    void generate_lts_random(const next_state_generator::transition_t::state_probability_list& initial_states);
    void print_target_distribution_in_aut_format(
               const lps::next_state_generator::transition_t::state_probability_list& state_probability_list,
               const std::size_t last_state_number,
               const lps::state& source_state);
    void print_target_distribution_in_aut_format(
                const lps::next_state_generator::transition_t::state_probability_list& state_probability_list,
                const lps::state& source_state);
    probabilistic_state<std::size_t, lps::probabilistic_data_expression> transform_initial_probabilistic_state_list
                 (const next_state_generator::transition_t::state_probability_list& initial_states);
    probabilistic_state<std::size_t, lps::probabilistic_data_expression> create_a_probabilistic_state_from_target_distribution(
               const std::size_t base_state_number,
               const next_state_generator::transition_t::state_probability_list& other_probabilities,
               const lps::state& source_state);


};

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LTS_DETAIL_EXPLORATION_H
