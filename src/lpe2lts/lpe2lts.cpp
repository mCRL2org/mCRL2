#include <string>
#include <getopt.h>
#include <aterm2.h>
#include <assert.h>
#include <boost/lexical_cast.hpp>
#include "libstruct.h"
#include "libprint_c.h"
#include "lpe2lts.h"
#include "squadt_lpe2lts.h"
#include "exploration.h"
#include "lts.h"

using namespace std;

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

static void print_help_suggestion(FILE *f, const char *Name)
{
  fprintf(f,"Try '%s --help' for more information.\n",Name);
}

static void print_help(FILE *f, const char *Name)
{
  fprintf(f,
    "Usage: %s [OPTION]... INFILE [OUTFILE]\n"
    "Generate state space of the LPE in INFILE and save the result to OUTFILE. If\n"
    "OUTFILE is not supplied, the state space is not stored.\n"
    "\n"
    "The format of OUTFILE is determined by its extension (unless it is specified\n"
    "by an option). If the extension is unknown, the aut format will be used.\n"
    "\n"
    "Mandatory arguments to long options are mandatory for short options too.\n"
    "  -h, --help               display this help message\n"
    "      --version            display version information\n"
    "  -q, --quiet              do not print any unrequested information\n"
    "  -v, --verbose            display extra information about the state space\n"
    "                           generation\n"
    "  -f, --freevar            do not replace free variables in the LPE with dummy\n"
    "                           values\n"
    "  -y, --dummy              replace free variables in the LPE with dummy values\n"
    "                           (default)\n"
    "  -u, --unused-data        do not remove unused parts of the data specification\n"
    "  -c, --vector             store state in a vector (fastest, default)\n"
    "  -r, --tree               store state in a tree (for memory efficiency)\n"
    "  -b, --bit-hash[=NUM]     use bit hashing to store states and store at most NUM\n"
    "                           states; note that this option may cause states to be\n"
    "                           mistaken for others\n"
    "  -l, --max=NUM            explore at most NUM states\n"
    "      --todo-max=NUM       keep at most NUM states in todo lists; this option is\n"
    "                           only relevant for breadth-first search with\n"
    "                           bithashing, where NUM is the maximum number of states\n"
    "                           per level, and for depth first, where NUM is the\n"
    "                           maximum depth\n"
    "  -d, --deadlock           detect deadlocks (i.e. for every deadlock a message\n"
    "                           is printed)\n"
    "  -a, --action=NAME*       detect actions from NAME* (i.e. print a message for\n"
    "                           every occurrence)\n"
    "  -t, --trace[=NUM]        write at most NUM traces to states detected with the\n"
    "                           --deadlock or --action options\n"
    "                           (NUM is 10 by default)\n"
    "      --error-trace        if an error occurs during exploration, save a trace\n"
    "                           to the state that could not be explored\n"
    "  -C, --confluence[=NAME]  apply on-the-fly confluence reduction with NAME the\n"
    "                           confluent tau action\n"
/*    "  -p, --priority=NAME   give priority to action NAME (i.e. if it is\n"
    "                        possible to execute an action NAME in some state,\n"
    "                        than make it the only executable action from that\n"
    "                        state)\n" */
    "  -s, --strategy=NAME      use strategy NAME to explore the state space with;\n"
    "                           the following strategies are available:\n"
    "                             b, breadth   breadth-first search (default)\n"
    "                             d, depth     depth-first search\n"
    "                             r, random    random search\n"
    "  -R, --rewriter=NAME      use rewriter NAME (default 'inner');\n"
    "                           available rewriters are inner, jitty, innerc and\n"
    "                           jittyc\n"
    "      --aut                force OUTFILE to be in the aut format (implies\n"
    "                           --no-info, see below)\n"
    "      --svc                force OUTFILE to be in the svc format\n"
    "      --no-info            do not add state information to OUTFILE\n"
    "      --init-tsize=NUM     set the initial size of the internally used hash\n"
    "                           tables (default is 10000)\n",
    Name);
}

static void print_version(FILE *f)
{
  fprintf(f,NAME " " VERSION " (revision %i)\n", REVISION);
}

int main(int argc, char **argv)
{
  ATerm bot;
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
  }
  // end handle aterm lib options 
  
  gsEnableConstructorFunctions();

  // Start 
  squadt_lpe2lts sl;
#ifdef ENABLE_SQUADT_CONNECTIVITY
  if ( sl.try_interaction(argc, argv) )
  {
    return 0;
  }
#endif

  lts_generation_options lgopts;
  initialise_lts_generation_options(lgopts);
  lgopts.squadt = &sl;

  #define sopts "hqvfyucrb::l:da:t::C::R:s:"
  struct option lopts[] = {
    { "help",            no_argument,       NULL, 'h' },
    { "version",         no_argument,       NULL, 0   },
    { "quiet",           no_argument,       NULL, 'q' },
    { "verbose",         no_argument,       NULL, 'v' },
    { "freevar",         no_argument,       NULL, 'f' },
    { "dummy",           no_argument,       NULL, 'y' },
    { "unused-data",     no_argument,       NULL, 'u' },
    { "vector",          no_argument,       NULL, 'c' },
    { "tree",            no_argument,       NULL, 'r' },
    { "bit-hash",        optional_argument, NULL, 'b' },
    { "max",             required_argument, NULL, 'l' },
    { "deadlock",        no_argument,       NULL, 'd' },
    { "deadlock-detect", no_argument,       NULL, 'd' },
    { "action",          required_argument, NULL, 'a' },
    { "action-detect",   required_argument, NULL, 'a' },
    { "trace",           optional_argument, NULL, 't' },
    { "confluence",      optional_argument, NULL, 'C' },
    { "strategy",        required_argument, NULL, 's' },
    { "rewriter",        required_argument, NULL, 'R' },
    { "aut",             no_argument,       NULL, 1   },
    { "svc",             no_argument,       NULL, 2   },
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
        print_version(stderr);
        return 0;
      case 'q':
        lgopts.quiet = true;
        break;
      case 'v':
        lgopts.verbose = true;
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
      case 'd':
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
          lgopts.priority_action = strdup(optarg);
        } else {
          lgopts.priority_action = strdup("tau");
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
        lgopts.outformat = OF_AUT;
        break;
      case 2:
        lgopts.outformat = OF_SVC;
        break;
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
  }
  if ( lgopts.quiet && lgopts.verbose )
  {
    gsErrorMsg("options -q/--quiet and -v/--verbose cannot be used together\n");
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
  
  if ( lgopts.specification == "" )
  {
    gsErrorMsg("no LPE file supplied\n");
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
