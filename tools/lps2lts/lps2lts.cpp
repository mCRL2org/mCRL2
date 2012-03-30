// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lps2lts_lts.cpp

#include "boost.hpp" // precompiled headers

// NAME is defined in lps2lts.h
#define AUTHOR "Muck van Weerdenburg"

#include <string>
#include <cassert>
#include <signal.h>
#include "mcrl2/aterm/aterm2.h"

#include "boost/lexical_cast.hpp"

#include "mcrl2/atermpp/aterm_init.h"

#include "mcrl2/utilities/logger.h"

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"

#include "mcrl2/lps/action_label.h"

#include "mcrl2/lts/lts_io.h"
#include "mcrl2/lts/detail/exploration_old.h"

#define __STRINGIFY(x) #x
#define STRINGIFY(x) __STRINGIFY(x)

using namespace std;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::lts;
using namespace mcrl2::lps;
using namespace mcrl2::log;

static atermpp::set < identifier_string > parse_action_list(const std::string& s)
{
  atermpp::set < identifier_string > result;

  for (std::string::size_type p = 0, q(s.find_first_of(",")); true; p = q + 1, q = s.find_first_of(",", q + 1))
  {
    const std::string a=s.substr(p, q - p);
    // Check that a is a proper string, with syntax: [a-zA-Z\_][a-zA-Z0-9\_']
    for (unsigned int i=0; i<a.size(); ++i)
    {
      const char c=a[i];
      if (!(('a'<=c && c<='z')||
            ('A'<=c && c<='Z')||
            (c=='_')||
            (i>0 && (('0'<=c && c<='9') || c=='\''))))
      {
        throw mcrl2::runtime_error("The string " + a + " is not a proper action label.");
      }
    }
    result.insert(identifier_string(a));

    if (q == std::string::npos)
    {
      break;
    }
  }
  return result;
}

static void check_whether_actions_on_commandline_exist(
             const atermpp::set < identifier_string > &actions, 
             const action_label_list action_labels)
{
  for(atermpp::set < identifier_string >::const_iterator i=actions.begin();
               i!=actions.end(); ++i)
  {
    mCRL2log(verbose) << "checking for occurrences of action '" << pp(*i) << "'.\n";

    bool found=(pp(*i)=="tau"); // If i equals tau, it does not need to be declared.
    for(action_label_list::const_iterator j=action_labels.begin(); 
              !found && j!=action_labels.end(); ++j)
    {
      found=(*i == j->name());  // The action in the set occurs in the action label.
    }
    if (!found)
    { 
      throw mcrl2::runtime_error("'" + pp(*i) + "' is not declared as an action in this LPS.");
    }
  }

}

typedef  rewriter_tool< input_output_tool > lps2lts_base;
class lps2lts_tool : public lps2lts_base
{
  private:
    lps2lts_algorithm lps2lts;
    lts_generation_options options;
    std::string m_filename;

  public:
    ~lps2lts_tool() { options.m_rewriter.reset(); }
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
                   +mcrl2::lts::detail::supported_lts_formats_text()
                  )
    {
    }

    void abort()
    {
      lps2lts.abort();
    }

    bool run()
    {
      options.specification.load(m_filename);
      options.trace_prefix = m_filename.substr(0, options.trace_prefix.find_last_of('.'));

      check_whether_actions_on_commandline_exist(options.trace_actions, options.specification.action_labels()); 
      if (!lps2lts.initialise_lts_generation(&options))
      {
        return false;
      }

      try
      {
        lps2lts.generate_lts();
      }
      catch (mcrl2::runtime_error& e)
      {
        mCRL2log(error) << e.what() << std::endl;
        lps2lts.finalise_lts_generation();
        return false;
      }

      lps2lts.finalise_lts_generation();

      return true;
    }

  protected:
    void add_options(interface_description& desc)
    {
      lps2lts_base::add_options(desc);

      desc.
      add_option("dummy", make_mandatory_argument("BOOL"),
                 "replace free variables in the LPS with dummy values based on the value of BOOL: 'yes' (default) or 'no'", 'y').
      add_option("unused-data",
                 "do not remove unused parts of the data specification", 'u').
      add_option("state-format", make_mandatory_argument("NAME"),
                 "store state internally in format NAME:\n"
                 "  'tree' for a tree (memory efficient, default), or\n"
                 "  'vector' for a vector (slightly faster, often far less memory efficient)"
                 , 'f').
      add_option("bit-hash", make_optional_argument("NUM", STRINGIFY(DEFAULT_BITHASHSIZE)),
                 "use bit hashing to store states and store at most NUM states; note that this "
                 "option may cause states to be mistaken for others (default value for NUM is "
                 "approximately 2*10^8)",'b').
      add_option("max", make_mandatory_argument("NUM"),
                 "explore at most NUM states", 'l').
      add_option("todo-max", make_mandatory_argument("NUM"),
                 "keep at most NUM states in todo lists; this option is only relevant for "
                 "breadth-first search with bithashing, where NUM is the maximum number of "
                 "states per level, and for depth first, where NUM is the maximum depth").
      add_option("deadlock",
                 "detect deadlocks (i.e. for every deadlock a message is printed)", 'D').
      add_option("divergence",
                 "detect divergences (i.e. for every state with a divergence (=tau loop) a message is printed).", 'F').
      add_option("action", make_mandatory_argument("NAMES"),
                 "detect actions from NAMES, a comma-separated list of action names; a message "
                 "is printed for every occurrence of one of these action names", 'a').
      add_option("trace", make_optional_argument("NUM", boost::lexical_cast<string>(DEFAULT_MAX_TRACES)),
                 "write at most NUM traces to states detected with the --deadlock, --divergence or --action "
                 "options (default is unlimited)", 't').
      add_option("error-trace",
                 "if an error occurs during exploration, save a trace to the state that could "
                 "not be explored").
      add_option("confluence", make_optional_argument("NAME", "ctau"),
                 "apply prioritization of transitions with the action label NAME."
                 "(when no NAME is supplied (i.e., '-c') priority is given to the action 'ctau'. To give priority to "
                 "to tau use the flag -ctau. Note that if the linear process is not tau-confluent, the generated "
                 "state space is necessarily branching bisimilar to the state space of the lps. The generation "
                 "algorithm that is used does not require the linear process to be tau convergent.", 'c').
      add_option("strategy", make_mandatory_argument("NAME"),
                 "explore the state space using strategy NAME:\n"
                 "  'b', 'breadth'   breadth-first search (default)\n"
                 "  'd', 'depth'     depth-first search\n"
                 "  'p', 'prioritized'  prioritize single actions on its first argument being of sort Nat where only those actions with the lowest value for this parameter are selected. E.g. if there are actions a(3) and b(4), a(3) remains and b(4) is skipped. Actions without a first parameter of sort Nat and multactions with more than one action are always chosen (option is experimental).\n"
                 "  'q', 'rprioritized'  prioritize actions on its first argument being of sort Nat (see option --prioritized), and randomly select one of these to obtain a prioritized random simulation (option is experimental).\n"
                 "  'r', 'random'    random simulation. Out of all next states one is chosen at random independently of whether this state has already been observed. Consequently, random simultation only terminates when a deadlocked state is encountered.", 's').
      add_option("out", make_mandatory_argument("FORMAT"),
                 "save the output in the specified FORMAT", 'o').
      add_option("no-info", "do not add state information to OUTFILE").
      add_option("suppress","in verbose mode, do not print progress messages indicating the number of visited states and transitions").
      add_option("init-tsize", make_mandatory_argument("NUM"),
                 "set the initial size of the internally used hash tables (default is 10000)");
    }

    void parse_options(const command_line_parser& parser)
    {
      lps2lts_base::parse_options(parser);
      options.removeunused    = parser.options.count("unused-data") == 0;
      options.detect_deadlock = parser.options.count("deadlock") != 0;
      options.detect_divergence = parser.options.count("divergence") != 0;
      options.outinfo         = parser.options.count("no-info") == 0;
      options.suppress_progress_messages = parser.options.count("suppress") !=0;
      options.strat           = parser.option_argument_as< mcrl2::data::rewriter::strategy >("rewriter");

      if (parser.options.count("dummy"))
      {
        if (parser.options.count("dummy") > 1)
        {
          parser.error("multiple use of option -y/--dummy; only one occurrence is allowed");
        }
        std::string dummy_str(parser.option_argument("dummy"));
        if (dummy_str == "yes")
        {
          options.usedummies = true;
        }
        else if (dummy_str == "no")
        {
          options.usedummies = false;
        }
        else
        {
          parser.error("option -y/--dummy has illegal argument '" + dummy_str + "'");
        }
      }

      if (parser.options.count("state-format"))
      {
        if (parser.options.count("state-format") > 1)
        {
          parser.error("multiple use of option -f/--state-format; only one occurrence is allowed");
        }
        std::string state_format_str(parser.option_argument("state-format"));
        if (state_format_str == "vector")
        {
          options.stateformat = GS_STATE_VECTOR;
        }
        else if (state_format_str == "tree")
        {
          options.stateformat = GS_STATE_TREE;
        }
        else
        {
          parser.error("option -f/--state-format has illegal argument '" + state_format_str + "'");
        }
      }

      if (parser.options.count("bit-hash"))
      {
        options.bithashing  = true;
        options.bithashsize = parser.option_argument_as< unsigned long > ("bit-hash");
      }
      if (parser.options.count("max"))
      {
        options.max_states = parser.option_argument_as< unsigned long > ("max");
      }
      if (parser.options.count("action"))
      {
        options.detect_action = true;
        options.trace_actions = parse_action_list(parser.option_argument("action").c_str());
      }
      if (parser.options.count("trace"))
      {
        options.trace      = true;
        options.max_traces = parser.option_argument_as< unsigned long > ("trace");
      }
      if (parser.options.count("confluence"))
      {
        options.priority_action = parser.option_argument("confluence");
      }
      if (parser.options.count("strategy"))
      {
        options.expl_strat = str_to_expl_strat(parser.option_argument("strategy").c_str());

        if (options.expl_strat == es_none)
        {
          parser.error("invalid exploration strategy '" + parser.option_argument("strategy") + "'");
        }
      }
      if (parser.options.count("out"))
      {
        options.outformat = mcrl2::lts::detail::parse_format(parser.option_argument("out"));

        if (options.outformat == lts_none)
        {
          parser.error("format '" + parser.option_argument("out") + "' is not recognised");
        }
      }
      if (parser.options.count("init-tsize"))
      {
        options.initial_table_size = parser.option_argument_as< unsigned long >("init-tsize");
      }
      if (parser.options.count("todo-max"))
      {
        options.todo_max = parser.option_argument_as< unsigned long >("todo-max");
      }
      if (parser.options.count("error-trace"))
      {
        options.save_error_trace = true;
      }

      /* if ( options.bithashing && options.trace ) {
        parser.error("options -b/--bit-hash and -t/--trace cannot be used together");
      } */

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
        options.lts = parser.arguments[1];
      }

      if (!options.lts.empty())
      {
        if (options.outformat == lts_none)
        {
          options.outformat = mcrl2::lts::detail::guess_format(options.lts);

          if (options.outformat == lts_none)
          {
            mCRL2log(warning) << "no output format set or detected; using default (mcrl2)" << std::endl;
            options.outformat = lts_lts;
          }
        }
      }
    }

};

class lps2lts_gui_tool: public mcrl2_gui_tool<lps2lts_tool>
{
  public:    
    lps2lts_gui_tool()
    {
      std::vector<std::string> values;
      m_gui_options["action"] = create_textctrl_widget();
      m_gui_options["bit-hash"] = create_textctrl_widget();
      m_gui_options["confluence"] = create_textctrl_widget();
      m_gui_options["deadlock"] = create_checkbox_widget();
      m_gui_options["error-trace"] = create_checkbox_widget();


      values.clear();
      values.push_back("vector");
      values.push_back("tree");

      m_gui_options["state-format"] = create_radiobox_widget(values);
      m_gui_options["divergence"] = create_checkbox_widget();
      m_gui_options["init-tsize"] = create_textctrl_widget();
      m_gui_options["max"] = create_textctrl_widget();
      m_gui_options["no-info"] = create_checkbox_widget();

      add_rewriter_widget();

      values.clear();
      values.push_back("breadth");
      values.push_back("depth");
      values.push_back("prioritized");
      values.push_back("rprioritized");
      values.push_back("random");
      m_gui_options["strategy"] = create_radiobox_widget(values);
      m_gui_options["suppress"] = create_checkbox_widget();
      m_gui_options["trace"] = create_textctrl_widget();
      m_gui_options["todo-max"] = create_textctrl_widget();

      m_gui_options["unused-data"] = create_checkbox_widget();
      m_gui_options["dummy"] = create_textctrl_widget();

      // TODO:
//      -oFORMAT, --out=FORMAT   save the output in the specified FORMAT


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
  MCRL2_ATERMPP_INIT(argc, argv)
  tool_instance = new lps2lts_gui_tool();

  signal(SIGABRT,premature_termination_handler);
  signal(SIGINT,premature_termination_handler);
  signal(SIGTERM,premature_termination_handler); // At ^C print a message.

  try
  { 
    result = tool_instance->execute(argc, argv);
  } 
  catch (...)
  {
    delete tool_instance;
    throw;
  }
  delete tool_instance;
  return result;
}
