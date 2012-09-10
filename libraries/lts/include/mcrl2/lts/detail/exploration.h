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
#include "mcrl2/lps/next_state_generator.h"
#include "mcrl2/lps/nextstate/nextstate_options.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/lts/detail/bithashtable.h"
#include "mcrl2/lts/detail/queue.h"
#include "mcrl2/lts/detail/lts_generation_options.h"
#include "mcrl2/atermpp/list.h"
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
  typedef next_state_generator::internal_state_t generator_state_t;
  typedef atermpp::aterm storage_state_t;

  private:
    lts_generation_options m_options;
    next_state_generator *m_generator;
    next_state_generator::substitution_t m_substitution;
    next_state_generator::summand_subset_t *m_main_subset;

    bool m_use_confluence_reduction;
    next_state_generator::summand_subset_t m_nonprioritized_subset;
    next_state_generator::summand_subset_t m_prioritized_subset;

    atermpp::indexed_set m_state_numbers;
    bit_hash_table m_bit_hash_table;

    lts_lts_t m_output_lts;
    atermpp::indexed_set m_action_label_numbers;
    std::ofstream m_aut_file;

    bool m_maintain_traces;
    bool m_value_prioritize;

    next_state_generator::summand_subset_t m_tau_summands;

    std::vector<bool> m_detected_action_summands;

    atermpp::map<storage_state_t, storage_state_t> m_backpointers;
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

    virtual ~lps2lts_algorithm()
    {
      delete m_generator;
    }

    virtual bool initialise_lts_generation(lts_generation_options* options);
    virtual bool generate_lts();
    virtual bool finalise_lts_generation();

    virtual void abort()
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
    generator_state_t generator_state(const storage_state_t storage_state);
    storage_state_t storage_state(const generator_state_t generator_state);
    generator_state_t get_prioritised_representative(generator_state_t state);
    void value_prioritize(atermpp::list<next_state_generator::transition_t> &transitions);
    bool save_trace(generator_state_t state, std::string filename);
    bool search_divergence(generator_state_t state, std::set<generator_state_t> &current_path, atermpp::set<generator_state_t> &visited);
    void check_divergence(generator_state_t state);
    void save_actions(generator_state_t state, const next_state_generator::transition_t &transition);
    void save_deadlock(generator_state_t state);
    void save_error(generator_state_t state);
    bool add_transition(generator_state_t state, const next_state_generator::transition_t &transition);
    atermpp::list<next_state_generator::transition_t> get_transitions(generator_state_t state);

    void generate_lts_breadth();
    void generate_lts_breadth_bithashing(generator_state_t initial_state);
    void generate_lts_depth(generator_state_t initial_state);
    void generate_lts_random(generator_state_t initial_state);
};

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LTS_DETAIL_EXPLORATION_H
