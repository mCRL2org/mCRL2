#define NAME "lpe2torx"
#define VERSION "0.1"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <getopt.h>
#include <aterm2.h>
#include <assert.h>
#include <iostream>
#include <sstream>
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "libprint.h"
#include "libnextstate.h"
#include "libenum.h"
#include "librewrite.h"
#include "libdataelm.h"

using namespace std;

#define is_tau(x) ATisEmpty((ATermList) ATgetArgument(x,0))
          
void print_torx_action(ostream &os, ATermAppl mact)
{
  if ( is_tau(mact) )
  {
    os << "tau";
  } else {
    ATermAppl act = (ATermAppl) ATgetFirst((ATermList) ATgetArgument(mact,0));
    PrintPart_CXX(cout,ATgetArgument(act,0), ppDefault);
    ATermList dl = (ATermList) ATgetArgument(act,1);
    for (; !ATisEmpty(dl); dl=ATgetNext(dl))
    {
      cout << "!";
      PrintPart_CXX(cout,ATgetFirst(dl), ppDefault);
    }
  }
}
  
typedef struct {
  int action;
  int state;
} index_pair;

class torx_data
{
  private:
    ATermIndexedSet stateactions;
    ATermTable state_indices;
    AFun fun_trip;

    ATerm triple(ATerm one, ATerm two, ATerm three)
    {
      return (ATerm) ATmakeAppl3(fun_trip,one,two,three);
    }

    ATerm third(ATerm trip)
    {
      return ATgetArgument((ATermAppl) trip,2);
    }

  public:
    torx_data(unsigned int initial_size)
    {
      stateactions = ATindexedSetCreate(10000,50);
      state_indices = ATtableCreate(10000,50);
      fun_trip = ATmakeAFun("@trip@",2,ATfalse);
      ATprotectAFun(fun_trip);
    }

    ~torx_data()
    {
      ATunprotectAFun(fun_trip);
      ATtableDestroy(state_indices);
      ATindexedSetDestroy(stateactions);
    }

    index_pair add_action_state(ATerm from, ATerm action, ATerm to)
    {
      ATbool is_new;
      index_pair p;

      p.action = ATindexedSetPut(stateactions,triple(from,action,to),&is_new);

      ATerm i;
      if ( (i = ATtableGet(state_indices,to)) == NULL )
      {
        ATtablePut(state_indices,to,(ATerm) ATmakeInt(p.action));
        p.state = p.action;
      } else {
        p.state = ATgetInt((ATermInt) i);
      }

      return p;
    }

    ATerm get_state(int index)
    {
      return third(ATindexedSetGetElem(stateactions,index));
    }
};

static void print_help_suggestion(FILE *f, char *Name)
{
  fprintf(f,"Try '%s --help' for more information.\n",Name);
}

static void print_help(FILE *f, char *Name)
{
  fprintf(f,
    "Usage: %s [OPTION]... [INFILE]\n"
    "Provide explorer interface for TorX using the LPE in INFILE. If INFILE is not\n"
    "supplied, stdin is used.\n"
    "\n"
    "Mandatory arguments to long options are mandatory for short options too.\n"
    "  -h, --help            display this help message\n"
    "      --version         display version information\n"
    "  -q, --quiet           do not print any unrequested information\n"
    "  -v, --verbose         display extra information about the state space\n"
    "                        generation\n"
    "  -f, --freevar         do not replace free variables in the LPE with dummy\n"
    "                        values\n"
    "  -y, --dummy           replace free variables in the LPE with dummy values\n"
    "                        (default)\n"
    "  -u, --unused-data     do not remove unused parts of the data specification\n"
    "  -c, --vector          store state in a vector (fastest, default)\n"
    "  -r, --tree            store state in a tree (for memory efficiency)\n"
    "  -m, --monitor         print detailed status of generation\n"
    "  -R, --rewriter=NAME   use rewriter NAME (default 'inner3')\n",
    Name);
}

static void print_version(FILE *f)
{
  fprintf(f,NAME " " VERSION " (revision %i)\n", REVISION);
}

int main(int argc, char **argv)
{
  FILE *SpecStream;
  ATerm stackbot;
  ATermAppl Spec;
  #define sopts "hqvfyucrmR:"
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
    { "rewriter",        required_argument, NULL, 'R' },
    { 0, 0, 0, 0 }
  };

  ATinit(argc,argv,&stackbot);

  bool quiet = false;
  bool verbose = false;
  RewriteStrategy strat = GS_REWR_INNER3;
  bool usedummies = true;
  bool removeunused = true;
  int stateformat = GS_STATE_VECTOR;
  int opt;
  char *SpecFileName;
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
        quiet = true;
        break;
      case 'v':
        verbose = true;
        break;
      case 'f':
        usedummies = false;
        break;
      case 'y':
        usedummies = true;
        break;
      case 'u':
        removeunused = false;
        break;
      case 'c':
        stateformat = GS_STATE_VECTOR;
        break;
      case 'r':
        stateformat = GS_STATE_TREE;
        break;
      case 'R':
        strat = RewriteStrategyFromString(optarg);
        if ( strat == GS_REWR_INVALID )
        {
          gsErrorMsg("invalid rewrite strategy '%s'\n",optarg);
          return 1;
        }
        break;
      default:
        break;
    }
  }
  if ( quiet && verbose )
  {
    gsErrorMsg("options -q/--quiet and -v/--verbose cannot be used together\n");
    return 1;
  }
  if ( quiet )
    gsSetQuietMsg();
  if ( verbose )
    gsSetVerboseMsg();
  
  if ( argc-optind > 1 )
  {
    print_help_suggestion(stderr,argv[0]);
    return 1;
  }

  if ( argc-optind == 1 )
  {
    SpecFileName = argv[optind];
    if ( (SpecStream = fopen(SpecFileName, "rb")) == NULL )
    {
      gsErrorMsg("could not open input file '%s' for reading: ", SpecFileName);
      perror(NULL);
      return 1;
    }
  } else {
    SpecStream = stdin;
  }
  if ( SpecStream == stdin )
    gsVerboseMsg("reading LPE from stdin\n");
  else
    gsVerboseMsg("reading LPE from '%s'\n", SpecFileName);
  Spec = (ATermAppl) ATreadFromFile(SpecStream);
  if ( Spec == NULL )
  {
    if ( SpecStream == stdin )
      gsErrorMsg("could not read LPE from stdin\n");
    else
      gsErrorMsg("could not read LPE from '%s'\n", SpecFileName);
    return 1;
  }
  assert(Spec != NULL);
  gsEnableConstructorFunctions();
  if (!gsIsSpecV1(Spec)) {
    if ( SpecStream == stdin )
      gsErrorMsg("stdin does not contain an LPE\n");
    else
      gsErrorMsg("'%s' does not contain an LPE\n", SpecFileName);
    return 1;
  }
  assert(gsIsSpecV1(Spec));

  if ( removeunused )
  {
    gsVerboseMsg("removing unused parts of the data specification.\n");
    Spec = removeUnusedData(Spec);
  }

  gsVerboseMsg("initialising...\n");
  torx_data td(10000);

  NextState *nstate = createNextState(Spec,!usedummies,stateformat,createEnumerator(Spec,createRewriter(ATAgetArgument(Spec,3),strat),true),true);

  ATerm initial_state = nstate->getInitialState();

  ATerm dummy_action = (ATerm) ATmakeAppl0(ATmakeAFun("@dummy_action@",0,ATfalse));
  td.add_action_state(initial_state,dummy_action,initial_state);

  gsVerboseMsg("generating state space...\n");

  NextStateGenerator *nsgen = NULL;
  bool err = false;
  bool notdone = true;
  while ( notdone && !cin.eof() )
  {
    string s;

    cin >> s;

    switch ( s[0] )
    {
      case 'r': // Reset
        // R event TAB solved TAB preds TAB freevars TAB identical
        cout << "R 0\t1\t\t\t" << endl;
        break;
      case 'e': // Expand
        {
        int index;
        ATerm state;
        
        cin >> index;
        state = td.get_state( index );
    
        cout << "EB" << endl;
        nsgen = nstate->getNextStates(state,nsgen);
        ATermAppl Transition;
        ATerm NewState;
        while ( nsgen->next(&Transition,&NewState) )
        {
          index_pair p;
    
          p = td.add_action_state(state,(ATerm) Transition,NewState);

          // Ee event TAB visible TAB solved TAB label TAB preds TAB freevars TAB identical
          cout << "Ee " << p.action << "\t" << (is_tau(Transition)?0:1) << "\t1\t";
          print_torx_action(cout,Transition);
          cout << "\t\t\t";
          if ( p.action != p.state )
          {
            cout << p.state;
          }
          cout << endl;
        }
        cout << "EE" << endl;

        if ( nsgen->errorOccurred() )
        {
          err = true;
          notdone = false;
        }
        break;
        }
      case 'q': // Quit
        cout << "Q" << endl;
        notdone = false;
        break;
      default:
        break;
    }
  }
  delete nsgen;
  delete nstate;
}
