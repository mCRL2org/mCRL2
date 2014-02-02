// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lts/detail/lts_generation_options.h
/// \brief Options used during state space generation.

#ifndef MCRL2_LTS_DETAIL_LTS_GENERATION_OPTIONS_H
#define MCRL2_LTS_DETAIL_LTS_GENERATION_OPTIONS_H

#include "boost/bind.hpp"
#include "boost/function.hpp"

#include "mcrl2/data/rewrite_strategy.h"
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/lts/detail/exploration_strategy.h"
#include "mcrl2/lps/action_parse.h"

namespace mcrl2
{
namespace lts
{

#define DEFAULT_MAX_STATES ULONG_MAX
#define DEFAULT_MAX_TRACES ULONG_MAX
#define DEFAULT_BITHASHSIZE 209715200ULL // ~25 MB
#define DEFAULT_INIT_TSIZE 10000UL

struct lts_generation_options
{
  lts_generation_options() :
    usedummies(true),
    removeunused(true),
    strat(mcrl2::data::jitty),
    expl_strat(es_breadth),
    todo_max((std::numeric_limits< size_t >::max)()),
    max_states(DEFAULT_MAX_STATES),
    initial_table_size(DEFAULT_INIT_TSIZE),
    suppress_progress_messages(false),
    bithashing(false),
    bithashsize(DEFAULT_BITHASHSIZE),
    outformat(mcrl2::lts::lts_none),
    outinfo(true),
    trace(false),
    max_traces(DEFAULT_MAX_TRACES),
    save_error_trace(false),
    detect_deadlock(false),
    detect_divergence(false),
    detect_action(false),
    use_enumeration_caching(false),
    use_summand_pruning(false)
  {
    generate_filename_for_trace = boost::bind(&lts_generation_options::generate_trace_file_name, this, _1, _2, _3);
  }

  /* Method that takes an info string and an extension to produce a unique filename */
  boost::function< std::string(std::string const&, std::string const&, std::string const&) >
  generate_filename_for_trace;

  /* Default function for generate_filename_for_trace */
  std::string generate_trace_file_name(std::string const& basefilename, std::string const& info, std::string const& extension)
  {
    return basefilename + std::string("_") + info + std::string(".") + extension;
  }

  void validate_actions()
  {
    for (std::set < std::string >::const_iterator it = trace_multiaction_strings.begin(); it != trace_multiaction_strings.end(); ++it)
    {
      try
      {
        trace_multiactions.insert(mcrl2::lps::parse_multi_action(*it, specification.action_labels(), specification.data()));
      }
      catch (mcrl2::runtime_error& e)
      {
        throw mcrl2::runtime_error(*it + ": " + e.what());
      }
      mCRL2log(log::verbose) << "Checking for action \"" << *it << "\"\n";
    }
    if (detect_action)
    {
      for (std::set<mcrl2::core::identifier_string>::iterator ta = trace_actions.begin(); ta != trace_actions.end(); ++ta)
      {
        mcrl2::lps::action_label_list::iterator it = specification.action_labels().begin();
        bool found = (std::string(*ta) == "tau");
        while (!found && it != specification.action_labels().end())
        {
            found = (it++->name() == *ta);
        }
        if (!found)
          throw mcrl2::runtime_error(*ta);
        else
          mCRL2log(log::verbose) << "Checking for action " << *ta << "\n";
      }
    }
  }

  mcrl2::lps::specification specification;
  bool usedummies;
  bool removeunused;

  mcrl2::data::rewriter::strategy strat;
  exploration_strategy expl_strat;
  std::string priority_action;
  size_t todo_max;
  size_t max_states;
  size_t initial_table_size;
  bool suppress_progress_messages;

  bool bithashing;
  size_t bithashsize;

  mcrl2::lts::lts_type outformat;
  bool outinfo;
  std::string lts;

  bool trace;
  size_t max_traces;
  std::string trace_prefix;
  bool save_error_trace;
  bool detect_deadlock;
  bool detect_divergence;
  bool detect_action;
  std::set < mcrl2::core::identifier_string > trace_actions;
  std::set < std::string > trace_multiaction_strings;
  std::set < mcrl2::lps::multi_action > trace_multiactions;

  std::auto_ptr< mcrl2::data::rewriter > m_rewriter; /// REMOVE

  bool use_enumeration_caching;
  bool use_summand_pruning;
};

} // namespace lts
} // namespace mcrl2

#endif // MCRL2_LTS_DETAIL_LTS_GENERATION_OPTIONS_H
