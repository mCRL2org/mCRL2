// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#ifndef MCRL2_LTS_DETAIL_EXPLORATION_H
#define MCRL2_LTS_DETAIL_EXPLORATION_H

#include <string>
#include <limits>
#include <memory>

#include "boost/bind.hpp"
#include "boost/function.hpp"

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/indexed_set.h"
#include "mcrl2/lts/lts.h"
#include "mcrl2/lts/detail/lps2lts_lts.h"
#include "mcrl2/lts/detail/bithashtable.h"
#include "mcrl2/lts/detail/queue_old.h"
#include "mcrl2/lts/detail/lts_generation_options.h"

#include "workarounds.h"

namespace mcrl2
{
namespace lts
{
namespace old
{

class lps2lts_algorithm: public lps2lts_algorithm_base
{
    typedef atermpp::aterm state_t; // Type of a state.

  private:
    // lps2lts_algorithm may be initialised only once
    bool initialised;
    bool premature_termination_handler_called;
    bool finalised;
    bool completely_generated;

    lts_generation_options* lgopts;
    lps::NextState* nstate;
    atermpp::indexed_set states;
    lps2lts_lts lts;

    size_t num_states;
    size_t trans;
    size_t level;
    size_t num_found_same;
    size_t current_state;
    size_t initial_state;
    bool must_abort;

    std::map<atermpp::aterm,atermpp::aterm> backpointers;
    bit_hash_table bithash_table;

    size_t tracecnt;

    bool lg_error;

    bool apply_confluence_reduction;
    lps::NextStateGenerator* repr_nsgen;

  public:
    lps2lts_algorithm() :
      initialised(false),
      premature_termination_handler_called(false),
      finalised(false),
      completely_generated(false),
      nstate(NULL),
      states(0,0), //Table of size 0 initially
      num_states(0),
      trans(0),
      level(0),
      num_found_same(0),
      current_state(0),
      initial_state(0),
      must_abort(false),
      bithash_table(),
      tracecnt(0),
      lg_error(false),
      apply_confluence_reduction(false),
      repr_nsgen(NULL)
    {
    }

    virtual ~lps2lts_algorithm()
    {
      if (initialised && !finalised)
      {
        finalise_lts_generation();
      }
    }

    virtual bool initialise_lts_generation(lts_generation_options* opts);
    virtual bool generate_lts();
    virtual bool finalise_lts_generation();

    virtual void abort()
    {
      // Stops the exploration algorithm if it is running by making sure
      // not a single state can be generated anymore.
      if (!must_abort)
      {
        must_abort = true;
        mCRL2log(log::warning) << "state space generation was aborted prematurely" << std::endl;
      }
    }


  private:

    void initialise_representation(bool confluence_reduction);
    void cleanup_representation();
    bool search_divergence_recursively(
      const state_t current_state,
      std::set < state_t > &on_current_depth_first_path,
      std::set<state_t> &repr_visited);
    void check_divergence(const state_t state);
    state_t get_repr(const state_t state);

    // trace functions
    bool occurs_in(const core::identifier_string name, const lps::multi_action ma);
    bool savetrace(std::string const& info,
                   const state_t state,
                   lps::NextState* nstate,
                   const state_t extra_state = state_t(),
                   const lps::multi_action extra_transition = lps::multi_action());
    void check_actiontrace(const state_t OldState, const lps::multi_action Transition, const state_t NewState);
    void save_error_trace(const state_t state);
    void check_deadlocktrace(const state_t state);

    size_t add_state(const state_t state, bool& is_new);
    size_t state_index(const state_t state);

    // Main routine
    bool add_transition(const state_t from, lps::multi_action action, const state_t to);
};

}
}
}

#endif // MCRL2_LTS_DETAIL_EXPLORATION_H
