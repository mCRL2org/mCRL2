// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lps2lts.cpp

#include "boost.hpp" // precompiled headers

// NAME is defined in lps2lts.h
#define AUTHOR "Muck van Weerdenburg"

#include <string>
#include <cassert>
#include <aterm2.h>
#include <boost/lexical_cast.hpp>
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/lts/lts.h"
#include "lps2lts.h"
#include "exploration.h"
#include "lts.h"

#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/squadt_tool.h"

#define __STRINGIFY(x) #x
#define STRINGIFY(x) __STRINGIFY(x)

using namespace std;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::lts;

ATermAppl *parse_action_list(const char *s, int *len)
{
  const char *p;

  *len = 0;
  p = s;
  while ( p != NULL )
  {
    *len = (*len)+1;
    p = strstr(p+1,",");
  }

  ATermAppl *r = (ATermAppl *) malloc((*len)*sizeof(ATermAppl));
  if ( r == NULL )
  {
    gsErrorMsg("not enough memory to store action list\n");
    exit(1);
  }
  for (int i=0; i<(*len); i++)
  {
    r[i] = NULL;
  }
  ATprotectArray((ATerm *) r,*len);

  char *t = strdup(s);
  p = strtok(t,",");
  int i=0;
  while ( p != NULL )
  {
    r[i] = mcrl2::core::detail::gsString2ATermAppl(p);
    i++;
    p = strtok(NULL,",");
  }
  free(t);

  return r;
}

#ifdef ENABLE_SQUADT_CONNECTIVITY
static bool initialise_types();
#endif

typedef squadt_tool< rewriter_tool< input_output_tool > > lps2lts_base;
class lps2lts_tool : public lps2lts_base
{
  private:
    lts_generation_options options;

  public:
    lps2lts_tool() :
      lps2lts_base(NAME,AUTHOR,
        "generate an LTS from an LPS",
        "Generate an LTS from the LPS in INFILE and save the result to OUTFILE. "
        "If INFILE is not supplied, stdin is used. "
        "If OUTFILE is not supplied, the LTS is not stored.\n"
        "\n"
        "The format of OUTFILE is determined by its extension (unless it is specified "
        "by an option). The supported formats are:\n"
        +lts::supported_lts_formats_text()
      )
    {
#ifdef ENABLE_SQUADT_CONNECTIVITY
      static bool initialised = initialise_types();

      static_cast< void > (initialised); // harmless, and prevents unused variable warnings
#endif
    }

    bool run()
    {
      if ( !initialise_lts_generation(&options) )
      {
        return false;
      }

      generate_lts();

      finalise_lts_generation();

      return true;
    }

  private:
    void add_options(interface_description &desc)
    {
      lps2lts_base::add_options(desc);

      desc.
        add_option("dummy", make_mandatory_argument("BOOL"),
          "replace free variables in the LPS with dummy values based on the value of BOOL: 'yes' (default) or 'no'", 'y').
        add_option("unused-data",
          "do not remove unused parts of the data specification", 'u').
        add_option("state-format", make_mandatory_argument("NAME"),
          "store state internally in format NAME:\n"
          "  'vector' for a vector (fastest, default), or\n"
          "  'tree' for a tree (for memory efficiency)"
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
        add_option("action", make_mandatory_argument("NAMES"),
          "detect actions from NAMES, a comma-separated list of action names; a message "
          "is printed for every occurrence of one of these action names", 'a').
        add_option("trace", make_optional_argument("NUM", boost::lexical_cast<string>(DEFAULT_MAX_TRACES)),
          "write at most NUM traces to states detected with the --deadlock or --action "
          "options (default is unlimited)", 't').
        add_option("error-trace",
          "if an error occurs during exploration, save a trace to the state that could "
          "not be explored").
        add_option("confluence", make_optional_argument("NAME", "ctau"),
          "apply on-the-fly confluence reduction with NAME the confluent tau action "
          "(when no NAME is supplied, 'ctau' is used)", 'c').
        add_option("strategy", make_mandatory_argument("NAME"),
          "explore the state space using strategy NAME:\n"
          "  'b', 'breadth'   breadth-first search (default)\n"
          "  'd', 'depth'     depth-first search\n"
          "  'r', 'random'    random simulation", 's').
        add_option("out", make_mandatory_argument("FORMAT"),
          "save the output in the specified FORMAT", 'o').
        add_option("no-info", "do not add state information to OUTFILE").
        add_option("init-tsize", make_mandatory_argument("NUM"),
          "set the initial size of the internally used hash tables (default is 10000)");
    }

    void parse_options(const command_line_parser &parser)
    {
      options.removeunused    = parser.options.count("unused-data") == 0;
      options.detect_deadlock = parser.options.count("deadlock");
      options.outinfo         = parser.options.count("no-info") == 0;
      options.strat           = parser.option_argument_as< mcrl2::data::rewriter::strategy >("rewriter");

      if (parser.options.count("dummy")) {
        if (parser.options.count("dummy") > 1) {
          parser.error("multiple use of option -y/--dummy; only one occurrence is allowed");
        }
        std::string dummy_str(parser.option_argument("dummy"));
        if (dummy_str == "yes") {
          options.usedummies = true;
        } else if (dummy_str == "no") {
          options.usedummies = false;
        } else {
          parser.error("option -y/--dummy has illegal argument '" + dummy_str + "'");
        }
      }

      if (parser.options.count("state-format")) {
        if (parser.options.count("state-format") > 1) {
          parser.error("multiple use of option -f/--state-format; only one occurrence is allowed");
        }
        std::string state_format_str(parser.option_argument("state-format"));
        if (state_format_str == "vector") {
          options.stateformat = GS_STATE_VECTOR;
        } else if (state_format_str == "tree") {
          options.stateformat = GS_STATE_TREE;
        } else {
          parser.error("option -f/--state-format has illegal argument '" + state_format_str + "'");
        }
      }

      if (parser.options.count("bit-hash")) {
        options.bithashing  = true;
        options.bithashsize = parser.option_argument_as< unsigned long long > ("bit-hash");
      }
      if (parser.options.count("max")) {
        options.max_states = parser.option_argument_as< unsigned long long > ("max");
      }
      if (parser.options.count("action")) {
        options.detect_action = true;
        options.trace_actions = parse_action_list(parser.option_argument("action").c_str(), &options.num_trace_actions);
      }
      if (parser.options.count("trace")) {
        options.trace      = true;
        options.max_traces = parser.option_argument_as< unsigned long > ("trace");
      }
      if (parser.options.count("confluence")) {
        options.priority_action = parser.option_argument("confluence");
      }
      if (parser.options.count("strategy")) {
        options.expl_strat = str_to_expl_strat(parser.option_argument("strategy").c_str());

        if ( options.expl_strat == es_none ) {
          parser.error("invalid exploration strategy '" + parser.option_argument("strategy") + "'");
        }
      }
      if (parser.options.count("out")) {
        options.outformat = lts::parse_format(parser.option_argument("out"));

        if (options.outformat == lts_none) {
          parser.error("format '" + parser.option_argument("out") + "' is not recognised");
        }
      }
      if (parser.options.count("init-tsize")) {
        options.initial_table_size = parser.option_argument_as< unsigned long >("init-tsize");
      }
      if (parser.options.count("todo-max")) {
        options.todo_max = parser.option_argument_as< unsigned long >("todo-max");
      }
      if (parser.options.count("error-trace")) {
        options.save_error_trace = true;
      }

      if ( options.bithashing && options.trace ) {
        parser.error("options -b/--bit-hash and -t/--trace cannot be used together");
      }

      if (2 < parser.arguments.size()) {
        parser.error("too many file arguments");
      }
      if (0 < parser.arguments.size()) {
        options.filename = parser.arguments[0];
      }
      if (1 < parser.arguments.size()) {
        options.lts = parser.arguments[1];
      }

      if (!options.lts.empty()) {
        if ( options.outformat == lts_none ) {
          options.outformat = lts::guess_format(options.lts);

          if ( options.outformat == lts_none ) {
            gsWarningMsg("no output format set or detected; using default (mcrl2)\n");
            options.outformat = lts_mcrl2;
          }
        }
      }
    }

#ifdef ENABLE_SQUADT_CONNECTIVITY
  friend class status_display;
  friend class storage_configuration;

  public:

    class status_display;
    class storage_configuration;

  private:

    static const char*  option_lts_type;
    static const char*  option_out_info;

    static const char*  option_usedummies;
    static const char*  option_state_format_tree;
    static const char*  option_removeunused;

    static const char*  option_rewrite_strategy;
    static const char*  option_exploration_strategy;

    static const char*  option_detect_deadlock;
    static const char*  option_detect_actions;
    static const char*  option_trace;
    static const char*  option_max_traces;
    static const char*  option_error_trace;

    static const char*  option_confluence_reduction;
    static const char*  option_confluent_tau;

    static const char*  option_max_states;

    static const char*  option_bithashing;
    static const char*  option_bithashsize;

    static const char*  option_init_tsize;

    static const char* lps_file_for_input;
    static const char* lts_file_for_output;
    static const char* trc_file_for_output;

  protected:

    void set_capabilities(tipi::tool::capabilities &capabilities) const;

    void user_interactive_configuration(tipi::configuration &configuration);

    bool check_configuration(tipi::configuration const &configuration) const;

    bool perform_task(tipi::configuration &configuration);
#endif
};

#define squadt_interactor lps2lts_tool

#include "squadt_interactor.ipp"

int main(int argc, char **argv)
{
  MCRL2_ATERM_INIT(argc, argv)

  lps2lts_tool tool;

  return tool.execute(argc,argv);
}
