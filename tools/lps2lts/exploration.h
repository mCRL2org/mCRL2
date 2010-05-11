// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file exploration.h

#ifndef _EXPLORATION_H
#define _EXPLORATION_H

#include <string>
#include <limits>
#include <memory>

#include "boost/bind.hpp"
#include "boost/cstdint.hpp"

#include "aterm2.h"
#include "lts.h"
#include "mcrl2/lps/nextstate.h"
#include "mcrl2/lts/lts.h"

#include "workarounds.h"

#define DEFAULT_MAX_STATES ULLONG_MAX
#define DEFAULT_MAX_TRACES ULONG_MAX
#define DEFAULT_BITHASHSIZE 209715200ULL // ~25 MB
#define DEFAULT_INIT_TSIZE 10000UL

enum exploration_strategy { es_none, 
                            es_breadth, 
                            es_depth, 
                            es_random, 
                            es_value_prioritized, 
                            es_value_random_prioritized };

struct lts_generation_options {
  lts_generation_options();

  /* Method that takes an info string and an extension to produce a unique filename */
  boost::function< std::string (std::string const&, std::string const&) >
                                                   generate_filename_for_trace;
  /* Method for status display */
  boost::function< void (unsigned long, boost::uint64_t,
                         boost::uint64_t, boost::uint64_t,
                         boost::uint64_t) > display_status;

  /* Default function for generate_filename_for_trace */
  std::string generate_trace_file_name(std::string const& info, std::string const& extension);

  /* Default function for status display */
  void update_status_display(unsigned long, boost::uint64_t,
                             boost::uint64_t, boost::uint64_t const,
                             boost::uint64_t) {
  }

  mcrl2::data::rewriter::strategy strat;
  bool usedummies;
  bool removeunused;
  int stateformat;
  mcrl2::lts::lts_type outformat;
  bool outinfo;
  bool suppress_progress_messages;
  boost::uint64_t max_states;
  std::string priority_action;
  bool trace;
  int num_trace_actions;
  ATermAppl *trace_actions;
  unsigned long max_traces;
  bool detect_deadlock;
  bool detect_divergence;
  bool detect_action;
  bool save_error_trace;
  bool error_trace_saved;
  exploration_strategy expl_strat;
  bool bithashing;
  boost::uint64_t bithashsize;
  unsigned long todo_max;
  unsigned long initial_table_size;
  std::auto_ptr< mcrl2::data::rewriter > m_rewriter;
  std::auto_ptr< mcrl2::data::enumerator_factory< mcrl2::data::classic_enumerator<> > > m_enumerator_factory;
  mcrl2::lps::specification specification;
  std::string filename;
  std::string lts;
};

inline lts_generation_options::lts_generation_options() :
    strat(mcrl2::data::rewriter::jitty),
    usedummies(true),
    removeunused(true),
    stateformat(GS_STATE_VECTOR),
    outformat(mcrl2::lts::lts_none),
    outinfo(true),
    suppress_progress_messages(false),
    max_states(DEFAULT_MAX_STATES),
    trace(false),
    num_trace_actions(0),
    trace_actions(NULL),
    max_traces(DEFAULT_MAX_TRACES),
    detect_deadlock(false),
    detect_divergence(false),
    detect_action(false),
    save_error_trace(false),
    error_trace_saved(false),
    expl_strat(es_breadth),
    bithashing(false),
    bithashsize(DEFAULT_BITHASHSIZE),
    todo_max((std::numeric_limits< unsigned long >::max)()),
    initial_table_size(DEFAULT_INIT_TSIZE) {

  generate_filename_for_trace = boost::bind(
        &lts_generation_options::generate_trace_file_name, this, _1, _2);
  display_status              = boost::bind(
        &lts_generation_options::update_status_display, this, _1, _2, _3, _4, _5);
}

exploration_strategy str_to_expl_strat(const char *s);
const char *expl_strat_to_str(exploration_strategy es);

bool initialise_lts_generation(lts_generation_options *opts);
bool generate_lts();
bool finalise_lts_generation();
void finalise_lts_generation_when_interrupted(int);

#endif
