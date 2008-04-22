// Author(s): Muck van Weerdenburg
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lps2lts.cpp

#define NAME "lps2lts"
#define AUTHOR "Muck van Weerdenburg"

#include <string>
#include <cassert>
#include <aterm2.h>
#include "mcrl2/core/struct.h"
#include "mcrl2/lts/liblts.h"
#include "lps2lts.h"
#include "exploration.h"
#include "lts.h"

#include "mcrl2/core/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"
#include "mcrl2/utilities/command_line_interface.h" // after messaging.h and rewrite.h

#include "squadt_interactor.ipp"

#define STRINGIFY(x) #x

using namespace std;
using namespace ::mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
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
    r[i] = gsString2ATermAppl(p);
    i++;
    p = strtok(NULL,",");
  }
  free(t);

  return r;
}

static void print_formats(FILE *f)
{
  fprintf(f,
    "The following formats are accepted by " NAME ":\n"
    "\n"
    "  format  ext.  description                       remarks\n"
    "  -----------------------------------------------------------\n"
    "  aut     .aut  Aldebaran format (CADP)\n"
#ifdef MCRL2_BCG
    "  bcg     .bcg  Binary Coded Graph format (CADP)\n"
#endif
    "  dot     .dot  GraphViz format\n"
    "  fsm     .fsm  Finite State Machine format\n"
    "  mcrl    .svc  mCRL SVC format\n"
    "  mcrl2   .svc  mCRL2 SVC format                  default\n"
    "\n"
    );
}

lts_generation_options parse_command_line(int ac, char** av) {
  interface_description clinterface(av[0], NAME, AUTHOR, "[OPTION]... [INFILE [OUTFILE]]\n"
    "Generate state space of the LPS in INFILE and save the result to OUTFILE. If "
    "OUTFILE is not supplied, the state space is not stored.\n"
    "\n"
    "The format of OUTFILE is determined by its extension (unless it is specified "
    "by an option). If the extension is unknown, the svc format will be used.");

  clinterface.add_rewriting_options();

  clinterface.
    add_option("freevar",
      "do not replace free variables in the LPS with dummy values", 'f').
    add_option("dummy",
      "replace free variables in the LPS with dummy values (default)", 'y').
    add_option("unused-data",
      "do not remove unused parts of the data specification", 'u').
    add_option("vector",
      "store state in a vector (fastest, default)", 'c').
    add_option("tree",
      "store state in a tree (for memory efficiency)", 'T').
    add_option("bit-hash", make_optional_argument("NUM", STRINGIFY(DEFAULT_BITHASHSIZE)),
      "use bit hashing to store states and store at most NUM states; note that this "
      " option may cause states to be mistaken for others (default value for NUM is "
      "approximately 2*10^8)",'b').
    add_option("max", make_mandatory_argument("NUM"),
      "explore at most NUM states", 'l').
    add_option("todo-max", make_mandatory_argument("NUM"),
      "keep at most NUM states in todo lists; this option is only relevant for "
      "breadth-first search with bithashing, where NUM is the maximum number of "
      "states per level, and for depth first, where NUM is the maximum depth").
    add_option("deadlock", make_mandatory_argument("NUM"),
      "detect deadlocks (i.e. for every deadlock a message is printed)", 'D').
    add_option("action", make_mandatory_argument("NAMES"),
      "detect actions from NAMES, a comma-separated list of action names; a message "
      "is printed for every occurrence of one of these action names", 'a').
    add_option("trace", make_optional_argument("NUM", "10"),
      "write at most NUM traces to states detected with the --deadlock or --action "
      "options (NUM is 10 by default)", 't').
    add_option("error-trace",
      "if an error occurs during exploration, save a trace to the state that could "
      "not be explored").
    add_option("confluence", make_optional_argument("NAME", "ctau"),
      "apply on-the-fly confluence reduction with NAME the confluent tau action "
      "(when no NAME is supplied, 'ctau' is used)", 'C').
/*  add_option("priority", make_mandatory_argument("NAME"),
      "give priority to action NAME (i.e. if it is possible to execute an action "
      "NAME in some state, than make it the only executable action from that state)", 'p'). */
    add_option("strategy", make_mandatory_argument("NAME"),
      "use strategy NAME to explore the state space with;\n"
      "the following strategies are available:\n"
      "  'b', 'breadth'   breadth-first search (default)\n"
      "  'd', 'depth'     depth-first search\n"
      "  'r', 'random'    random simulation", 's').
    add_option("out", make_mandatory_argument("FORMAT"),
      "use FORMAT as the output format; for accepted formats, see --formats", 'o').
    add_option("formats", "list accepted output formats").
    add_option("no-info", "do not add state information to OUTFILE").
    add_option("init-tsize", make_mandatory_argument("NUM"),
      "set the initial size of the internally used hash tables (default is 10000)");

  command_line_parser parser(clinterface, ac, av);

  lts_generation_options options;

  options.usedummies      = parser.options.count("freevar") == 0;
  if ( !options.usedummies && parser.options.count("dummy") )
  {
    parser.error("conflicting options -f/--freevar and -y/--dummy cannot be used together");
  }

  options.removeunused    = parser.options.count("unused-data") == 0;
  options.detect_deadlock = parser.options.count("deadlock");
  options.outinfo         = parser.options.count("no-info") == 0;
  options.strat           = parser.option_argument_as< RewriteStrategy >("rewriter");

  if (parser.options.count("vector")) {
    options.stateformat = GS_STATE_VECTOR; 
  }
  if (parser.options.count("tree")) {
    options.stateformat = GS_STATE_TREE; 
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
    options.outformat = lts::parse_format(parser.option_argument("out").c_str());

    if (options.outformat == lts_none) {
      parser.error("format '" + parser.option_argument("out") + "' is not recognised; option ignored");
    }
  }
  if (parser.options.count("formats")) {
    print_formats(stderr);
    exit(EXIT_SUCCESS);
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

  if (0 < parser.arguments.size()) {
    options.specification = parser.arguments[0];
  }
  else {
    parser.error("no LPS file specified");
  }
  if (1 < parser.arguments.size()) {
    options.lts = parser.arguments[1];

    if ( options.outformat == lts_none ) {
      options.outformat = lts::guess_format(options.lts);

      if ( options.outformat == lts_none ) {
        gsWarningMsg("no output format set or detected; using default (mcrl2)\n");
        options.outformat = lts_mcrl2;
      }
    }
  }
  if (2 < parser.arguments.size()) {
    parser.error("too many file arguments");
  }

  return options;
}

int main(int argc, char **argv)
{
  MCRL2_ATERM_INIT(argc, argv)

/*  ATerm bot;
  // handle the aterm options
  {
    short a_argc=1;
    char *a_argv[11];
    a_argv[0]=argv[0];

    for(int i=1; (i<argc) && (a_argc<11); i++)
    {
      if(strncmp(argv[i],"--at-",5)!=0) continue;

      //fprintf(stderr, "at argument %s\n",argv[i]);
      a_argv[a_argc++]=argv[i]+1;

      if(strcmp(argv[i],"--at-afuntable")==0 || strcmp(argv[i],"--at-termtable")==0){
        i++;
        if(i==argc) break;
        else a_argv[a_argc++]=argv[i];
      }
    } 
  
    ATinit(a_argc,a_argv,&bot);
  } */
  // end handle aterm lib options 


  try {
  // Start 
#ifdef ENABLE_SQUADT_CONNECTIVITY
    if (mcrl2::utilities::squadt::interactor< squadt_interactor >::free_activation(argc, argv)) {
      return EXIT_SUCCESS;
    }
#endif

    lts_generation_options lgopts(parse_command_line(argc, argv));

    if ( !initialise_lts_generation(&lgopts) )
    {
      return 1;
    }
    
    generate_lts();
 
    finalise_lts_generation();

    return EXIT_SUCCESS;
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return EXIT_FAILURE;
}
