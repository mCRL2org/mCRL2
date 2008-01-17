// Author(s): Muck van Weerdenburg
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lps2lts.cpp

#define NAME "lps2lts"

#include <string>
#include <getopt.h>
#include <aterm2.h>
#include <cassert>
#include <boost/lexical_cast.hpp>
#include "mcrl2/core/struct.h"
#include "mcrl2/lts/liblts.h"
#include "lps2lts.h"
#include "exploration.h"
#include "lts.h"

#include "mcrl2/core/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"
#include "mcrl2/utilities/version_info.h"

#include "squadt_interactor.ipp"

using namespace std;
using namespace ::mcrl2::utilities;
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

static void print_help_suggestion(FILE *f, const char *Name)
{
  fprintf(f,"Try '%s --help' for more information.\n",Name);
}

static void print_help(FILE *f, const char *Name)
{
  fprintf(f,
    "Usage: %s [OPTION]... INFILE [OUTFILE]\n"
    "Generate state space of the LPS in INFILE and save the result to OUTFILE. If\n"
    "OUTFILE is not supplied, the state space is not stored.\n"
    "\n"
    "The format of OUTFILE is determined by its extension (unless it is specified\n"
    "by an option). If the extension is unknown, the svc format will be used.\n"
    "\n"
    "Options:\n"
    "  -h, --help               display this help message\n"
    "      --version            display version information\n"
    "  -q, --quiet              do not display any unrequested information\n"
    "  -v, --verbose            display extra information about the state space\n"
    "  -d, --debug              display detailed intermediate messages\n"
    "                           generation\n"
    "  -f, --freevar            do not replace free variables in the LPS with dummy\n"
    "                           values\n"
    "  -y, --dummy              replace free variables in the LPS with dummy values\n"
    "                           (default)\n"
    "  -u, --unused-data        do not remove unused parts of the data specification\n"
    "  -c, --vector             store state in a vector (fastest, default)\n"
    "  -r, --tree               store state in a tree (for memory efficiency)\n"
    "  -b[NUM], --bit-hash[=NUM]\n"
    "                           use bit hashing to store states and store at most NUM\n"
    "                           states; note that this option may cause states to be\n"
    "                           mistaken for others (default value for NUM is\n"
    "                           approximately 2*10^8)\n"
    "  -lNUM, --max=NUM         explore at most NUM states\n"
    "      --todo-max=NUM       keep at most NUM states in todo lists; this option is\n"
    "                           only relevant for breadth-first search with\n"
    "                           bithashing, where NUM is the maximum number of states\n"
    "                           per level, and for depth first, where NUM is the\n"
    "                           maximum depth\n"
    "  -D, --deadlock           detect deadlocks (i.e. for every deadlock a message\n"
    "                           is printed)\n"
    "  -aNAME*, --action=NAME*  detect actions from NAME* (i.e. print a message for\n"
    "                           every occurrence)\n"
    "  -t[NUM], --trace[=NUM]   write at most NUM traces to states detected with the\n"
    "                           --deadlock or --action options\n"
    "                           (NUM is 10 by default)\n"
    "      --error-trace        if an error occurs during exploration, save a trace\n"
    "                           to the state that could not be explored\n"
    "  -C[NAME], --confluence[=NAME]\n"
    "                           apply on-the-fly confluence reduction with NAME the\n"
    "                           confluent tau action (when no NAME is supplied,\n"
    "                           'ctau' is used)\n"
/*    "  -p, --priority=NAME   give priority to action NAME (i.e. if it is\n"
    "                        possible to execute an action NAME in some state,\n"
    "                        than make it the only executable action from that\n"
    "                        state)\n" */
    "  -sNAME, --strategy=NAME  use strategy NAME to explore the state space with;\n"
    "                           the following strategies are available:\n"
    "                             b, breadth   breadth-first search (default)\n"
    "                             d, depth     depth-first search\n"
    "                             r, random    random simulation\n"
    "  -RNAME, --rewriter=NAME  use rewriter NAME (default 'jitty');\n"
    "                           available rewriters are inner, jitty, innerc and\n"
    "                           jittyc\n"
    "  -oFORMAT, --out=FORMAT   use FORMAT as the output format\n"
    "      --formats            list accepted output formats\n"
    "      --aut                alias for --out=aut\n"
    "      --svc                alias for --out=mcrl2\n"
    "      --no-info            do not add state information to OUTFILE\n"
    "      --init-tsize=NUM     set the initial size of the internally used hash\n"
    "                           tables (default is 10000)\n",
    Name);
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


  // Start 
#ifdef ENABLE_SQUADT_CONNECTIVITY
  if (mcrl2::utilities::squadt::interactor< squadt_interactor >::free_activation(argc, argv)) {
    return 0;
  }
#endif

  lts_generation_options lgopts;

  #define sopts "hqvdfyucrb::l:Da:t::C::R:s:o:"
  struct option lopts[] = {
    { "help",            no_argument,       NULL, 'h' },
    { "version",         no_argument,       NULL, 0   },
    { "quiet",           no_argument,       NULL, 'q' },
    { "verbose",         no_argument,       NULL, 'v' },
    { "debug",           no_argument,       NULL, 'd' },
    { "freevar",         no_argument,       NULL, 'f' },
    { "dummy",           no_argument,       NULL, 'y' },
    { "unused-data",     no_argument,       NULL, 'u' },
    { "vector",          no_argument,       NULL, 'c' },
    { "tree",            no_argument,       NULL, 'r' },
    { "bit-hash",        optional_argument, NULL, 'b' },
    { "max",             required_argument, NULL, 'l' },
    { "deadlock",        no_argument,       NULL, 'D' },
    { "deadlock-detect", no_argument,       NULL, 'D' },
    { "action",          required_argument, NULL, 'a' },
    { "action-detect",   required_argument, NULL, 'a' },
    { "trace",           optional_argument, NULL, 't' },
    { "confluence",      optional_argument, NULL, 'C' },
    { "strategy",        required_argument, NULL, 's' },
    { "rewriter",        required_argument, NULL, 'R' },
    { "aut",             no_argument,       NULL, 1   },
    { "svc",             no_argument,       NULL, 2   },
    { "out",             required_argument, NULL, 'o' },
    { "formats",         no_argument,       NULL, 7   },
    { "no-info",         no_argument,       NULL, 3   },
    { "init-tsize",      required_argument, NULL, 4   },
    { "todo-max",        required_argument, NULL, 5   },
    { "error-trace",     no_argument,       NULL, 6   },
// aterm lib options
    { "at-help",          no_argument,       NULL, 10  },
    { "at-verbose",       no_argument,       NULL, 11  },
    { "at-silent",        no_argument,       NULL, 12  },
    { "at-low-memory",    no_argument,       NULL, 13  },
    { "at-termtable",     required_argument, NULL, 14  },
    { "at-hashinfo",      no_argument,       NULL, 15  },
    { "at-check",         no_argument,       NULL, 16  },
    { "at-afuntable",     required_argument, NULL, 17  },
    { "at-print-gc-time", no_argument,       NULL, 18  },
    { "at-print-gc-info", no_argument,       NULL, 19  },
    { 0, 0, 0, 0 }
  };

  int opt;
  while ( (opt = getopt_long(argc,argv,sopts,lopts,NULL)) != -1 )
  {
    switch ( opt )
    {
      case 'h':
        print_help(stderr, argv[0]);
        return 0;
      case 0:
        print_version_information(NAME);
        return 0;
      case 'q':
        lgopts.quiet = true;
        break;
      case 'v':
        lgopts.verbose = true;
        break;
      case 'd':
        lgopts.debug = true;
        break;
      case 'f':
        lgopts.usedummies = false;
        break;
      case 'y':
        lgopts.usedummies = true;
        break;
      case 'u':
        lgopts.removeunused = false;
        break;
      case 'c':
        lgopts.stateformat = GS_STATE_VECTOR;
        break;
      case 'r':
        lgopts.stateformat = GS_STATE_TREE;
        break;
      case 'b':
        lgopts.bithashing = true;
        if ( optarg != NULL )
        {
          if ( (optarg[0] >= '0') && (optarg[0] <= '9') )
          {
            lgopts.bithashsize = boost::lexical_cast < unsigned long long > (optarg);
          } else {
            gsErrorMsg("invalid argument to -b/--bit-hash\n",optarg);
            return 1;
          }
        }
        break;
      case 'l':
        if ( (optarg[0] >= '0') && (optarg[0] <= '9') )
        {
          lgopts.max_states = boost::lexical_cast < unsigned long long > (optarg);
        } else {
          gsErrorMsg("invalid argument to -l/--max\n",optarg);
          return 1;
        }
        break;
      case 'D':
        lgopts.detect_deadlock = true;
        break;
      case 'a':
        lgopts.detect_action = true;
        lgopts.trace_actions = parse_action_list(optarg,&lgopts.num_trace_actions);
        break;
      case 't':
        lgopts.trace = true;
        if ( optarg != NULL )
        {
          if ( (optarg[0] >= '0') && (optarg[0] <= '9') )
          {
            lgopts.max_traces = strtoul(optarg,NULL,0);
          } else {
            gsErrorMsg("invalid argument to -t/--trace\n",optarg);
            return 1;
          }
        }
        break;
      case 'C':
        if ( optarg != NULL )
        {
          lgopts.priority_action = optarg;
        } else {
          lgopts.priority_action = "ctau";
        }
        break;
      case 's':
        lgopts.expl_strat = str_to_expl_strat(optarg);
        if ( lgopts.expl_strat == es_none )
        {
          gsErrorMsg("invalid exploration strategy '%s'\n",optarg);
          return 1;
        }
        break;
      case 'R':
        lgopts.strat = RewriteStrategyFromString(optarg);
        if ( lgopts.strat == GS_REWR_INVALID )
        {
          gsErrorMsg("invalid rewrite strategy '%s'\n",optarg);
          return 1;
        }
        break;
      case 1:
        lgopts.outformat = lts_aut;
        break;
      case 2:
        lgopts.outformat = lts_mcrl2;
        break;
      case 'o':
        lgopts.outformat = lts::parse_format(optarg);
        if ( lgopts.outformat == lts_none )
        {
          fprintf(stderr,"warning: format '%s' is not recognised; option ignored\n",optarg);
        }
        break;
      case 7:
        print_formats(stderr);
        return 0;
      case 3:
        lgopts.outinfo = false;
        break;
      case 4:
        if ( (optarg[0] >= '0') && (optarg[0] <= '9') )
        {
          lgopts.initial_table_size = strtoul(optarg,NULL,0);
        } else {
          gsErrorMsg("invalid argument to --init-tsize\n",optarg);
          return 1;
        }
        break;
      case 5:
        if ( (optarg[0] >= '0') && (optarg[0] <= '9') )
        {
          lgopts.todo_max = strtoul(optarg,NULL,0);
        } else {
          gsErrorMsg("invalid argument to --todo-max\n",optarg);
          return 1;
        }
        break;
      case 6:
        lgopts.trace = true;
        lgopts.save_error_trace = true;
        break;
      default:
        break;
    }
  }
  if ( argc-optind >= 1 )
  {
    lgopts.specification = argv[optind];
  }
  if ( argc-optind > 1 )
  {
    lgopts.lts = argv[optind+1];
    if ( lgopts.outformat == lts_none )
    {
      lgopts.outformat = lts::guess_format(lgopts.lts);
      if ( lgopts.outformat == lts_none )
      {
        gsWarningMsg("no output format set or detected; using default (mcrl2)\n");
        lgopts.outformat = lts_mcrl2;
      }
    }
  }
  if ( lgopts.quiet && lgopts.verbose )
  {
    gsErrorMsg("options -q/--quiet and -v/--verbose cannot be used together\n");
    return 1;
  }
  if ( lgopts.quiet && lgopts.debug )
  {
    gsErrorMsg("options -q/--quiet and -d/--debug cannot be used together\n");
    return 1;
  }
  if ( lgopts.bithashing && lgopts.trace )
  {
    gsErrorMsg("options -b/--bit-hash and -t/--trace cannot be used together\n");
    return 1;
  }
  if ( lgopts.quiet )
    gsSetQuietMsg();
  if ( lgopts.verbose )
    gsSetVerboseMsg();
  if ( lgopts.debug )
    gsSetDebugMsg();
  
  if ( lgopts.specification == "" )
  {
    gsErrorMsg("no LPS file supplied\n");
    print_help_suggestion(stderr,argv[0]);
    return 1;
  }
  
  if ( !initialise_lts_generation(&lgopts) )
  {
    return 1;
  }
  
  generate_lts();

  finalise_lts_generation();
}
