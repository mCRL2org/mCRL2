// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lps2lts_lts.cpp

// NAME is defined in lps2lts.h
#define AUTHOR "Muck van Weerdenburg"

#include <string>
#include <cassert>
#include <signal.h>

#include "boost/lexical_cast.hpp"

#include "mcrl2/utilities/logger.h"

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"

#include "mcrl2/lps/io.h"
#include "mcrl2/process/action_parse.h"

#include "mcrl2/lts/lts_io.h"
#include "mcrl2/lts/detail/exploration.h"

#define __STRINGIFY(x) #x
#define STRINGIFY(x) __STRINGIFY(x)

using namespace std;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::lts;
using namespace mcrl2::lps;
using namespace mcrl2::log;

static
std::list<std::string> split_actions(const std::string& s)
{
  size_t pcount = 0;
  std::string a;
  std::list<std::string> result;
  for (std::string::const_iterator i = s.begin(); i != s.end(); ++i)
  {
    if (*i == ',' && pcount == 0)
    {
      result.push_back(a);
      a.clear();
    }
    else
    {
      if (*i == '(') ++pcount;
      else if (*i == ')') --pcount;
      a.push_back(*i);
    }
  }
  if (!a.empty())
    result.push_back(a);
  return result;
}

typedef  rewriter_tool< input_output_tool > lps2lts_base;
class lps2lts_tool : public lps2lts_base
{
  protected:
    mcrl2::lts::lps2lts_algorithm m_lps2lts;
    lts_generation_options m_options;
    std::string m_filename;

  public:
    ~lps2lts_tool() { m_options.m_rewriter.reset(); }
    lps2lts_tool() :
      lps2lts_base("lps2lts",AUTHOR,
                   "generate an LTS from an LPS",
                   "Generate an LTS from the LPS in INFILE and save the result to OUTFILE. "
                   "If INFILE is not supplied, stdin is used. "
                   "If OUTFILE is not supplied, the LTS is not stored.\n"
                   "\n"
                   "If the 'jittyc' rewriter is used, then the MCRL2_COMPILEREWRITER environment "
                   "variable (default value: 'mcrl2compilerewriter') determines the script that "
                   "compiles the rewriter, and MCRL2_COMPILEDIR (default value: '.') determines "
                   "where temporary files are stored.\n"
                   "\n"
                   "Note that lps2lts can deliver multiple transitions with the same label between"
                   "any pair of states. If this is not desired, such transitions can be removed by"
                   "applying a strong bisimulation reducton using for instance the tool ltsconvert.\n"
                   "\n"
                   "The format of OUTFILE is determined by its extension (unless it is specified "
                   "by an option). The supported formats are:\n"
                   "\n"
                   +mcrl2::lts::detail::supported_lts_formats_text()+"\n"
                   "If the jittyc rewriter is used, then the MCRL2_COMPILEREWRITER environment "
                   "variable (default value: mcrl2compilerewriter) determines the script that "
                   "compiles the rewriter, and MCRL2_COMPILEDIR (default value: '.') "
                   "determines where temporary files are stored."
                   "\n"
                   "Note that lps2lts can deliver multiple transitions with the same "
                   "label between any pair of states. If this is not desired, such "
                   "transitions can be removed by applying a strong bisimulation reducton "
                   "using for instance the tool ltsconvert."
                  )
    {
    }

    void abort()
    {
      m_lps2lts.abort();
    }

    bool run()
    {
      load_lps(m_options.specification, m_filename);
      m_options.trace_prefix = m_filename.substr(0, m_options.trace_prefix.find_last_of('.'));

      // check_whether_actions_on_commandline_exist(m_options.trace_actions, m_options.specification.action_labels());
      try
      {
        m_options.validate_actions();
      }
      catch (mcrl2::runtime_error& e)
      {
        mCRL2log(error) << "Invalid (multi-)action given: " << e.what() << std::endl;
        return false;
      }

      if (!m_lps2lts.initialise_lts_generation(&m_options))
      {
        return false;
      }

      try
      {
        m_lps2lts.generate_lts();
      }
      catch (mcrl2::runtime_error& e)
      {
        mCRL2log(error) << e.what() << std::endl;
        m_lps2lts.finalise_lts_generation();
        return false;
      }

      m_lps2lts.finalise_lts_generation();

      return true;
    }

  protected:
    void add_options(interface_description& desc)
    {
      lps2lts_base::add_options(desc);

      desc.
      add_option("cached",
                 "use enumeration caching techniques to speed up state space generation.").
      add_option("prune",
                 "use summand pruning to speed up state space generation.").
      add_option("dummy", make_mandatory_argument("BOOL"),
                 "replace free variables in the LPS with dummy values based on the value of BOOL: 'yes' (default) or 'no'", 'y').
      add_option("unused-data",
                 "do not remove unused parts of the data specification", 'u').
      add_option("bit-hash", make_optional_argument("NUM", STRINGIFY(DEFAULT_BITHASHSIZE)),
                 "use bit hashing to store states and store at most NUM states. "
                 "This means that instead of keeping a full record of all states "
                 "that have been visited, a bit array is used that indicate whether "
                 "or not a hash of a state has been seen before. Although this means "
                 "that this option may cause states to be mistaken for others (because "
                 "they are mapped to the same hash), it can be useful to explore very "
                 "large LTSs that are otherwise not explorable. The default value for NUM is "
                 "approximately 2*10^8 (this corresponds to about 25MB of memory)",'b').
      add_option("max", make_mandatory_argument("NUM"),
                 "explore at most NUM states", 'l').
      add_option("todo-max", make_mandatory_argument("NUM"),
                 "keep at most NUM states in todo lists; this option is only relevant for "
                 "breadth-first search with bithashing, where NUM is the maximum number of "
                 "states per level, and for depth first, where NUM is the maximum depth").
      add_option("deadlock",
                 "detect deadlocks (i.e. for every deadlock a message is printed)", 'D').
      add_option("divergence",
                 "detect divergences (i.e. for every state with a divergence (=tau loop) a message is printed). "
                 "The algorithm to detect the divergences is linear for every state, "
                 "so state space exploration becomes quadratic with this option on, causing a state "
                 "space exploration to become slow when this option is enabled.", 'F').
      add_option("action", make_mandatory_argument("NAMES"),
                 "detect and report actions in the transitions system that have action "
                 "names from NAMES, a comma-separated list. This is for instance useful "
                 "to find (or prove the absence) of an action error. A message "
                 "is printed for every occurrence of one of these action names. "
                 "With the -t flag traces towards these actions are generated", 'a').
      add_option("multiaction", make_mandatory_argument("NAMES"),
                 "detect and report multiactions in the transitions system "
                 "from NAMES, a comma-separated list. Works like -a, except that multi-actions "
                 "are matched exactly, including data parameters.", 'm').
      add_option("trace", make_optional_argument("NUM", boost::lexical_cast<string>(DEFAULT_MAX_TRACES)),
                 "Write a shortest trace to each state that is reached with an action from NAMES "
                 "from option --action, is a deadlock detected with --deadlock, or is a "
                 "divergence detected with --divergence to a file. "
                 "No more than NUM traces will be written. If NUM is not supplied the number of "
                 "traces is unbounded."
                 "For each trace that is to be written a unique file with extension .trc (trace) "
                 "will be created containing a shortest trace from the initial state to the deadlock "
                 "state. The traces can be pretty printed and converted to other formats using tracepp.", 't').
      add_option("error-trace",
                 "if an error occurs during exploration, save a trace to the state that could "
                 "not be explored").
      add_option("confluence", make_optional_argument("NAME", "ctau"),
                 "apply prioritization of transitions with the action label NAME."
                 "(when no NAME is supplied (i.e., '-c') priority is given to the action 'ctau'. To give priority to "
                 "to tau use the flag -ctau. Note that if the linear process is not tau-confluent, the generated "
                 "state space is necessarily branching bisimilar to the state space of the lps. The generation "
                 "algorithm that is used does not require the linear process to be tau convergent.", 'c').
      add_option("strategy", make_enum_argument<exploration_strategy>("NAME")
                 .add_value_short(es_breadth, "b", true)
                 .add_value_short(es_depth, "d")
                 .add_value_short(es_value_prioritized, "p")
                 .add_value_short(es_value_random_prioritized, "q")
                 .add_value_short(es_random, "r")
                 , "explore the state space using strategy NAME:"
                 , 's').
      add_option("out", make_mandatory_argument("FORMAT"),
                 "save the output in the specified FORMAT", 'o').
      add_option("no-info", "do not add state information to OUTFILE"
                 "Without this option lps2lts adds state vector to the LTS. This "
                 "option causes this information to be discarded and states are only "
                 "indicated by a sequence number. Explicit state information is useful "
                 "for visualisation purposes, for instance, but can cause the OUTFILE "
                 "to grow considerably. Note that this option is implicit when writing "
                 "in the AUT format.").
      add_option("suppress","in verbose mode, do not print progress messages indicating the number of visited states and transitions. "
                 "For large state spaces the number of progress messages can be quite "
                 "horrendous. This feature helps to suppress those. Other verbose messages, "
                 "such as the total number of states explored, just remain visible.").
      add_option("init-tsize", make_mandatory_argument("NUM"),
                 "set the initial size of the internally used hash tables (default is 10000)");
    }

    void parse_options(const command_line_parser& parser)
    {
      lps2lts_base::parse_options(parser);
      m_options.removeunused    = parser.options.count("unused-data") == 0;
      m_options.detect_deadlock = parser.options.count("deadlock") != 0;
      m_options.detect_divergence = parser.options.count("divergence") != 0;
      m_options.outinfo         = parser.options.count("no-info") == 0;
      m_options.suppress_progress_messages = parser.options.count("suppress") !=0;
      m_options.strat           = parser.option_argument_as< mcrl2::data::rewriter::strategy >("rewriter");

      m_options.use_enumeration_caching = parser.options.count("cached");
      m_options.use_summand_pruning = parser.options.count("prune");

      if (parser.options.count("dummy"))
      {
        if (parser.options.count("dummy") > 1)
        {
          parser.error("multiple use of option -y/--dummy; only one occurrence is allowed");
        }
        std::string dummy_str(parser.option_argument("dummy"));
        if (dummy_str == "yes")
        {
          m_options.usedummies = true;
        }
        else if (dummy_str == "no")
        {
          m_options.usedummies = false;
        }
        else
        {
          parser.error("option -y/--dummy has illegal argument '" + dummy_str + "'");
        }
      }

      if (parser.options.count("bit-hash"))
      {
        m_options.bithashing  = true;
        m_options.bithashsize = parser.option_argument_as< unsigned long > ("bit-hash");
      }
      if (parser.options.count("max"))
      {
        m_options.max_states = parser.option_argument_as< unsigned long > ("max");
      }
      if (parser.options.count("action"))
      {
        m_options.detect_action = true;
        std::list<std::string> actions = split_actions(parser.option_argument("action"));
        for (std::list<std::string>::iterator it = actions.begin(); it != actions.end(); ++it)
          m_options.trace_actions.insert(mcrl2::core::identifier_string(it->c_str()));
      }
      if (parser.options.count("multiaction"))
      {
        std::list<std::string> actions = split_actions(parser.option_argument("multiaction"));
        m_options.trace_multiaction_strings.insert(actions.begin(), actions.end());
      }
      if (parser.options.count("trace"))
      {
        m_options.trace      = true;
        m_options.max_traces = parser.option_argument_as< unsigned long > ("trace");
      }
      if (parser.options.count("confluence"))
      {
        m_options.priority_action = parser.option_argument("confluence");
      }

      m_options.expl_strat = parser.option_argument_as<exploration_strategy>("strategy");

      if (parser.options.count("out"))
      {
        m_options.outformat = mcrl2::lts::detail::parse_format(parser.option_argument("out"));

        if (m_options.outformat == lts_none)
        {
          parser.error("format '" + parser.option_argument("out") + "' is not recognised");
        }
      }
      if (parser.options.count("init-tsize"))
      {
        m_options.initial_table_size = parser.option_argument_as< unsigned long >("init-tsize");
      }
      if (parser.options.count("todo-max"))
      {
        m_options.todo_max = parser.option_argument_as< unsigned long >("todo-max");
      }
      if (parser.options.count("error-trace"))
      {
        m_options.save_error_trace = true;
      }

      if (parser.options.count("suppress") && !mCRL2logEnabled(verbose))
      {
        parser.error("option --suppress requires --verbose (of -v)");
      }

      if (2 < parser.arguments.size())
      {
        parser.error("too many file arguments");
      }
      if (0 < parser.arguments.size())
      {
        m_filename = parser.arguments[0];
      }
      if (1 < parser.arguments.size())
      {
        m_options.lts = parser.arguments[1];
      }

      if (!m_options.lts.empty() && m_options.outformat == lts_none)
      {
        m_options.outformat = mcrl2::lts::detail::guess_format(m_options.lts);

        if (m_options.outformat == lts_none)
        {
          mCRL2log(warning) << "no output format set or detected; using default (mcrl2)" << std::endl;
          m_options.outformat = lts_lts;
        }
      }
    }

};

lps2lts_tool *tool_instance;

static
void premature_termination_handler(int)
{
  // Reset signal handlers.
  signal(SIGABRT,NULL);
  signal(SIGINT,NULL);
  signal(SIGTERM,NULL);
  tool_instance->abort();
}

int main(int argc, char** argv)
{
  int result;
  tool_instance = new lps2lts_tool();

  signal(SIGABRT,premature_termination_handler);
  signal(SIGINT,premature_termination_handler);
  signal(SIGTERM,premature_termination_handler); // At ^C print a message.

  try
  {
    result = tool_instance->execute(argc, argv);
#ifdef MCRL2_DISPLAY_REWRITE_STATISTICS
    data::detail::display_rewrite_statistics();
#endif
  }
  catch (...)
  {
    delete tool_instance;
    throw;
  }
  delete tool_instance;
  return result;
}
