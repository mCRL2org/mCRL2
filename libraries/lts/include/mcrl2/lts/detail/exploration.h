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
  typedef atermpp::term_balanced_tree<data::data_expression> storage_state_t;

  private:
    lts_generation_options m_options;
    next_state_generator *m_generator;
    next_state_generator::summand_subset_t *m_main_subset;

    bool m_use_confluence_reduction;
    next_state_generator::summand_subset_t m_nonprioritized_subset;
    next_state_generator::summand_subset_t m_prioritized_subset;

    atermpp::indexed_set<storage_state_t> m_state_numbers;
    bit_hash_table m_bit_hash_table;

    lts_lts_t m_output_lts;
    atermpp::indexed_set<atermpp::aterm> m_action_label_numbers; // aterm should be replace by proper type.
    std::ofstream m_aut_file;

    bool m_maintain_traces;
    bool m_value_prioritize;

    next_state_generator::summand_subset_t m_tau_summands;

    std::vector<bool> m_detected_action_summands;

    std::map<storage_state_t, storage_state_t> m_backpointers;
    size_t m_traces_saved;

    size_t m_num_states;
    size_t m_num_transitions;
    size_t m_initial_state_number;
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
    data::data_expression_vector generator_state(const storage_state_t& storage_state);
    storage_state_t storage_state(const data::data_expression_vector& generator_state);
    storage_state_t get_prioritised_representative(const storage_state_t& state);
    void value_prioritize(std::vector<next_state_generator::transition_t>& transitions);
    bool save_trace(const storage_state_t& state, const std::string& filename);
    bool save_trace(const storage_state_t& state, const next_state_generator::transition_t& transition, const std::string& filename);
    void construct_trace(const storage_state_t& state1, mcrl2::trace::Trace& trace);
    bool search_divergence(const storage_state_t& state, std::set<storage_state_t>& current_path, std::set<storage_state_t>& visited);
    void check_divergence(const storage_state_t& state);
    void save_actions(const storage_state_t& state, const next_state_generator::transition_t& transition);
    void save_deadlock(const storage_state_t& state);
    void save_error(const storage_state_t& state);
    bool add_transition(const storage_state_t& source_state, next_state_generator::transition_t& transition);
    void get_transitions(const storage_state_t& state,
                         std::vector<lps2lts_algorithm::next_state_generator::transition_t>& transitions,
                         next_state_generator::enumerator_queue_t& enumeration_queue
    );
    void generate_lts_breadth_todo_max_is_npos();
    void generate_lts_breadth_todo_max_larger_than_0(const storage_state_t& initial_state);
    void generate_lts_breadth_bithashing(const storage_state_t& initial_state);
    void generate_lts_depth(const storage_state_t& initial_state);
    void generate_lts_random(const storage_state_t& initial_state);
};

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LTS_DETAIL_EXPLORATION_H
