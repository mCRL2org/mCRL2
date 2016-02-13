// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_LTS_DETAIL_EXPLORATION_NEW_H
#define MCRL2_LTS_DETAIL_EXPLORATION_NEW_H

#include <string>
#include <limits>
#include <memory>

#include "mcrl2/atermpp/indexed_set.h"
#include "mcrl2/atermpp/aterm_balanced_tree.h"
#include "mcrl2/trace/trace.h"
#include "mcrl2/lps/next_state_generator.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/lts/detail/bithashtable.h"
#include "mcrl2/lts/detail/queue.h"
#include "mcrl2/lts/detail/lts_generation_options.h"
#include "mcrl2/lts/detail/exploration_strategy.h"

#include "mcrl2/utilities/workarounds.h"

namespace mcrl2
{

namespace lts
{

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

    atermpp::indexed_set<lps::state> m_state_numbers;
    bit_hash_table m_bit_hash_table;

    probabilistic_lts_lts_t m_output_lts;
    atermpp::indexed_set<atermpp::aterm> m_action_label_numbers; // aterm should be replace by proper type.
    std::ofstream m_aut_file;

    bool m_maintain_traces;
    bool m_value_prioritize;

    next_state_generator::summand_subset_t m_tau_summands;

    std::vector<bool> m_detected_action_summands;

    std::map<lps::state, lps::state> m_backpointers;
    size_t m_traces_saved;

    size_t m_num_states;
    size_t m_num_transitions;
    next_state_generator::transition_t::state_probability_list m_initial_states;
    size_t m_level;

    volatile bool m_must_abort;

  public:
    lps2lts_algorithm() :
      m_generator(0),
      m_must_abort(false)
    {
    }

    ~lps2lts_algorithm()
    {
      delete m_generator;
    }

    bool initialise_lts_generation(lts_generation_options* options);
    bool generate_lts();
    bool finalise_lts_generation();

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
    data::data_expression_vector generator_state(const lps::state& storage_state);
    lps::state storage_state(const data::data_expression_vector& generator_state);
    void set_prioritised_representatives(next_state_generator::transition_t::state_probability_list& states);
    lps::state get_prioritised_representative(const lps::state& state);
    void value_prioritize(std::vector<next_state_generator::transition_t>& transitions);
    bool save_trace(const lps::state& state, const std::string& filename);
    bool save_trace(const lps::state& state, const next_state_generator::transition_t& transition, const std::string& filename);
    void construct_trace(const lps::state& state1, mcrl2::trace::Trace& trace);
    bool search_divergence(const lps::state& state, std::set<lps::state>& current_path, std::set<lps::state>& visited);
    void check_divergence(const lps::state& state);
    void save_actions(const lps::state& state, const next_state_generator::transition_t& transition);
    void save_deadlock(const lps::state& state);
    void save_error(const lps::state& state);
    std::pair<size_t, bool> add_target_state(const lps::state& source_state, const lps::state& target_state);
    bool add_transition(const lps::state& source_state, next_state_generator::transition_t& transition);
    void get_transitions(const lps::state& state,
                         std::vector<lps2lts_algorithm::next_state_generator::transition_t>& transitions,
                         next_state_generator::enumerator_queue_t& enumeration_queue
    );
    void generate_lts_breadth_todo_max_is_npos();
    void generate_lts_breadth_todo_max_is_not_npos(const next_state_generator::transition_t::state_probability_list& initial_states);
    void generate_lts_breadth_bithashing(const next_state_generator::transition_t::state_probability_list& initial_states);
    void generate_lts_depth(const next_state_generator::transition_t::state_probability_list& initial_states);
    void generate_lts_random(const next_state_generator::transition_t::state_probability_list& initial_states);
    void print_target_distribution_in_aut_format(
               const lps::next_state_generator::transition_t::state_probability_list& state_probability_list,
               const size_t last_state_number,
               const lps::state& source_state);
    void print_target_distribution_in_aut_format(
                const lps::next_state_generator::transition_t::state_probability_list& state_probability_list,
                const lps::state& source_state);
    probabilistic_state<size_t, lps::probabilistic_data_expression> transform_initial_probabilistic_state_list
                 (const next_state_generator::transition_t::state_probability_list& m_initial_states);
    probabilistic_state<size_t, lps::probabilistic_data_expression> create_a_probabilistic_state_from_target_distribution(
               const size_t base_state_number,
               const next_state_generator::transition_t::state_probability_list& other_probabilities);


};

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LTS_DETAIL_EXPLORATION_H
