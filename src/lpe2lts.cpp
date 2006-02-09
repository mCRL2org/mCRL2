#define NAME "lpe2lts"
#define VERSION "0.5"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <getopt.h>
#include <aterm2.h>
#include <assert.h>
#include <sstream>
#include "svc/svc.h" //XXX
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "libnextstate.h"
#include "libenum.h"
#include "librewrite.h"
#include "libtrace.h"
#include "libdataelm.h"

using namespace std;

#define OF_UNKNOWN  0
#define OF_AUT    1
#define OF_SVC    2

static ATermAppl *parse_action_list(char *s, int *len)
{
  char *p;

  *len = 0;
  p = s;
  while ( p != NULL )
  {
    *len = (*len)+1;
    p = strstr(p+1,",");
  }

  ATermAppl *r = (ATermAppl *) malloc((*len)*sizeof(ATermAppl));
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

static bool occurs_in(ATermAppl name, ATermList ma)
{
  for (; !ATisEmpty(ma); ma=ATgetNext(ma))
  {
    if ( ATisEqual(name,ATgetArgument(ATAgetArgument(ATAgetFirst(ma),0),0)) )
    {
      return true;
    }
  }
  return false;
}

static void save_trace(string &filename, ATerm state, ATermTable backpointers, NextState *nstate)
{
  ATerm s = state;
  ATerm ns;
  ATermList tr = ATmakeList0();
  
  while ( (ns = ATtableGet(backpointers, s)) != NULL )
  {
    tr = ATinsert(tr, (ATerm) ATmakeList2(ATgetFirst(ATgetNext((ATermList) ns)),s));
    s = ATgetFirst((ATermList) ns);
  }
  
  Trace trace;
  trace.setState(nstate->makeStateVector(s));
  for (; !ATisEmpty(tr); tr=ATgetNext(tr))
  {
    ATermList e = (ATermList) ATgetFirst(tr);
    trace.addAction((ATermAppl) ATgetFirst(e));
    e = ATgetNext(e);
    trace.setState(nstate->makeStateVector(ATgetFirst(e)));
  }

  trace.save(filename);
}

static void print_help_suggestion(FILE *f, char *Name)
{
  fprintf(f,"Try '%s --help' for more information.\n",Name);
}

static void print_help(FILE *f, char *Name)
{
  fprintf(f,
    "Usage: %s [OPTION]... INFILE [OUTFILE]\n"
    "Generate state space of the LPE in INFILE and save the result to OUTFILE (in the\n"
    "aut format). If OUTFILE is not supplied, the state space is not stored.\n"
    "\n"
    "The format of OUTFILE is determined by its extension (unless it is specified\n"
    "by an option). If the extension is unknown, the aut format will be used.\n"
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
    "  -l, --max=NUM         explore at most NUM states\n"
    "      --deadlock        synonym for --deadlock-detect\n"
    "  -d, --deadlock-detect detect deadlocks (i.e. for every deadlock a message is\n"
    "                        printed)\n"
    "  -e, --deadlock-trace  write trace to each deadlock state to a file\n"
    "  -D, --detect=NAME*    detect actions from NAME* (i.e. print a message for\n"
    "                        every occurence)\n"
    "  -t, --trace=NAME*     write trace to each state that is reached with an\n"
    "                        action from NAME* to a file\n"
    "  -p, --priority=NAME   give priority to action NAME (i.e. if it is\n"
    "                        possible to execute an action NAME in some state,\n"
    "                        than make it the only executable action from that\n"
    "                        state)\n"
    "  -m, --monitor         print detailed status of generation\n"
    "  -R, --rewriter=NAME   use rewriter NAME (default 'inner3')\n"
    "      --aut             force OUTFILE to be in the aut format (implies\n"
    "                        --no-info, see below)\n"
    "      --svc             force OUTFILE to be in the svc format\n"
    "      --no-info         do not add state information to OUTFILE\n",
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
  #define sopts "hqvfyucrl:deD:t:mp:R:"
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
    { "max",             required_argument, NULL, 'l' },
    { "deadlock",        no_argument,       NULL, 'd' },
    { "deadlock-detect", no_argument,       NULL, 'd' },
    { "deadlock-trace",  no_argument,       NULL, 'e' },
    { "detect",          required_argument, NULL, 'D' },
    { "trace",           required_argument, NULL, 't' },
    { "monitor",         no_argument,       NULL, 'm' },
    { "priority",        required_argument, NULL, 'p' },
    { "rewriter",        required_argument, NULL, 'R' },
    { "aut",             no_argument,       NULL, 1   },
    { "svc",             no_argument,       NULL, 2   },
    { "no-info",         no_argument,       NULL, 3   },
    { 0, 0, 0, 0 }
  };

  ATinit(argc,argv,&stackbot);

  bool quiet = false;
  bool verbose = false;
  RewriteStrategy strat = GS_REWR_INNER3;
  bool usedummies = true;
  bool removeunused = true;
  int stateformat = GS_STATE_VECTOR;
  int outformat = OF_UNKNOWN;
  bool outinfo = true;
  unsigned long max_states = ULONG_MAX;
  char *priority_action = NULL;
  bool trace = false;
  bool trace_deadlock = false;
  bool trace_action = false;
  int num_trace_actions = 0;
  ATermAppl *trace_actions = NULL;
  bool monitor = false;
  bool detect_deadlock = false;
  bool detect_action = false;
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
      case 'l':
        if ( (optarg[0] >= '0') && (optarg[0] <= '9') )
        {
          max_states = strtoul(optarg,NULL,0);
        }
        break;
      case 'd':
        detect_deadlock = true;
        break;
      case 'e':
        trace = true;
        trace_deadlock = true;
        break;
      case 'D':
        detect_action = true;
        trace_actions = parse_action_list(optarg,&num_trace_actions);
        break;
      case 't':
        trace = true;
        trace_action = true;
        trace_actions = parse_action_list(optarg,&num_trace_actions);
        break;
      case 'm':
        monitor = true;
        break;
      case 'p':
        priority_action = strdup(optarg);
        break;
      case 'R':
        strat = RewriteStrategyFromString(optarg);
        if ( strat == GS_REWR_INVALID )
        {
          gsErrorMsg("invalid rewrite strategy '%s'\n",optarg);
          return 1;
        }
        break;
      case 1:
        outformat = OF_AUT;
        break;
      case 2:
        outformat = OF_SVC;
        break;
      case 3:
        outinfo = false;
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
  
  if ( detect_action && trace_action )
  { // because of possibly incompatible action list arguments (XXX)
    gsWarningMsg("options -D/--detect and -t/--trace cannot be used together\n");
    return 1;
  }

  if ( argc-optind < 1 )
  {
    print_help_suggestion(stderr,argv[0]);
    return 1;
  }
        char *SpecFileName = argv[optind];
  if ( (SpecStream = fopen(SpecFileName, "rb")) == NULL )
  {
    gsErrorMsg("could not open input file '%s' for reading: ", SpecFileName);
    perror(NULL);
    return 1;
  }
  gsVerboseMsg("reading LPE from '%s'\n", SpecFileName);
  Spec = (ATermAppl) ATreadFromFile(SpecStream);
  if ( Spec == NULL )
  {
    gsErrorMsg("could not read LPE from '%s'\n", SpecFileName);
    return 1;
  }
  assert(Spec != NULL);
  gsEnableConstructorFunctions();
  if (!gsIsSpecV1(Spec)) {
    gsErrorMsg("'%s' does not contain an LPE\n", SpecFileName);
    return 1;
  }
  assert(gsIsSpecV1(Spec));

  if ( removeunused )
  {
    gsVerboseMsg("removing unused parts of the data specification.\n");
    Spec = removeUnusedData(Spec);
  }

  FILE *aut = NULL;
  SVCfile svcf;
  SVCfile *svc = &svcf;
  SVCparameterIndex svcparam = 0;
  if ( argc-optind > 1 )
  {
    if ( outformat == OF_UNKNOWN )
    {
      char *s;

      s = strrchr(argv[optind+1],'.');
      if ( s == NULL )
      {
        gsWarningMsg("no extension given for output file; writing state space in AUT format\n",s);
        outformat = OF_AUT;
      } else {
        s++;
        if ( !strcmp(s,"svc") )
        {
          outformat = OF_SVC;
        } else {
          if ( strcmp(s,"aut") )
          {
            gsWarningMsg("extension '%s' of output file not recognised; writing state space in AUT format\n",s);
          }
          outformat = OF_AUT;
        }
      }
    }

    switch ( outformat )
    {
      case OF_AUT:
        gsVerboseMsg("writing state space in AUT format to '%s'.\n",argv[optind+1]);
        outinfo = false;
        if ( (aut = fopen(argv[optind+1],"wb")) == NULL )
        {
          gsErrorMsg("cannot open '%s' for writing\n",argv[optind+1]);
          return 1;
        }
        break;
      case OF_SVC:
        gsVerboseMsg("writing state space in SVC format to '%s'.\n",argv[optind+1]);
        {
          SVCbool b;

          b = outinfo?SVCfalse:SVCtrue;
          SVCopen(svc,argv[optind+1],SVCwrite,&b); // XXX check result
          SVCsetCreator(svc,NAME);
          if (outinfo)
            SVCsetType(svc, "mCRL2+info");
          else
            SVCsetType(svc, "mCRL2");
          svcparam = SVCnewParameter(svc,(ATerm) ATmakeList0(),&b);
        }
      default:
        gsVerboseMsg("not saving state space.\n");
        break;
    }
  } else {
    outformat = OF_UNKNOWN;
    gsVerboseMsg("not saving state space.\n");
  }

  gsVerboseMsg("initialising...\n");
  ATermIndexedSet states = ATindexedSetCreate(10000,50);
  unsigned long num_states = 0;
  unsigned long trans = 0;
  unsigned long level = 1;
  ATermTable backpointers;
  if ( trace )
  {
    backpointers = ATtableCreate(10000,50);
  } else {
    backpointers = NULL;
  }

  NextState *nstate = createNextState(Spec,!usedummies,stateformat,createEnumerator(Spec,createRewriter(ATAgetArgument(Spec,3),strat),true),true);
  if ( priority_action != NULL )
  {
          gsVerboseMsg("prioritising action '%s'...\n",priority_action);
          nstate->prioritise(priority_action);
  }

  ATerm state = nstate->getInitialState();
  switch ( outformat )
  {
    case OF_AUT:
      fprintf(aut,"des (0,0,0)                   \n");
      break;
    case OF_SVC:
      {
        SVCbool b;
        if ( outinfo )
        {
          SVCsetInitialState(svc,SVCnewState(svc,(ATerm) nstate->makeStateVector(state),&b));
        } else {
          SVCsetInitialState(svc,SVCnewState(svc,(ATerm) ATmakeInt(0),&b));
        }
      }
      break;
    default:
      break;
  }

  ATbool new_state;
  unsigned long current_state = ATindexedSetPut(states,state,&new_state);
  num_states++;

  bool err = false;
  if ( max_states != 0 )
  {
  unsigned long nextlevelat = 1;
  unsigned long prevtrans = 0;
  unsigned long prevcurrent = 0;
  unsigned long tracecnt = 0;
  char *basefilename = NULL;
  gsVerboseMsg("generating state space...\n");

  NextStateGenerator *nsgen = NULL;
  while ( current_state < num_states )
  {
    state = ATindexedSetGetElem(states,current_state);
    bool deadlockstate = true;

    nsgen = nstate->getNextStates(state,nsgen);
    ATermAppl Transition;
    ATerm NewState;
    bool prioritised_action;
    while ( nsgen->next(&Transition,&NewState,&prioritised_action) )
    {
      ATbool new_state;
      unsigned long i;

      deadlockstate = false;

      i = ATindexedSetPut(states, NewState, &new_state);

      if ( new_state )
      {
        if ( num_states < max_states )
        {
                num_states++;
                if ( trace )
                {
                        ATtablePut(backpointers, NewState, (ATerm) ATmakeList2(state,(ATerm) Transition));
                }
        }
      }

      for (int i=0; i<num_trace_actions; i++)
      {
        if ( occurs_in(trace_actions[i],ATLgetArgument(Transition,0)) )
        {
          if ( trace_action )
          {
            if ( basefilename == NULL )
            {
              basefilename = strdup(SpecFileName);
              char *s = strrchr(basefilename,'.');
              if ( s != NULL )
              {
                *s = '\0';
              }
            }
            stringstream ss;
            ss << basefilename << "_act_" << tracecnt << "_" << ATgetName(ATgetAFun(trace_actions[i])) << ".trc";
            string sss(ss.str());
            save_trace(sss,state,backpointers,nstate);
    
            if ( detect_action || gsVerbose )
            {
              gsfprintf(stderr,"detect: action '%P' found and saved to '%s_act_%lu_%P.trc'.\n",trace_actions[i],basefilename,tracecnt,trace_actions[i]);
              fflush(stderr);
            }
            tracecnt++;
          } else //if ( detect_action )
          {
            gsfprintf(stderr,"detect: action '%P' found.\n",trace_actions[i]);
            fflush(stderr);
          }
        }
      }

      if ( i < num_states )
      {
        switch ( outformat )
        {
          case OF_AUT:
            gsfprintf(aut,"(%lu,\"%P\",%lu)\n",current_state,Transition,i);
            fflush(aut);
            break;
          case OF_SVC:
            if ( outinfo )
            {
              SVCbool b;
              SVCputTransition(svc,
                SVCnewState(svc,(ATerm) nstate->makeStateVector(ATindexedSetGetElem(states,current_state)),&b),
                SVCnewLabel(svc,(ATerm) Transition,&b),
                SVCnewState(svc,(ATerm) nstate->makeStateVector(ATindexedSetGetElem(states,i)),&b),
                svcparam);
            } else {
              SVCbool b;
              SVCputTransition(svc,
                SVCnewState(svc,(ATerm) ATmakeInt(current_state),&b),
                SVCnewLabel(svc,(ATerm) Transition,&b),
                SVCnewState(svc,(ATerm) ATmakeInt(i),&b),
                svcparam);
            }
            break;
          default:
            break;
        }
      }
      trans++;
      if ( prioritised_action )
      {
              break;
      }
    }
    
    if ( nsgen->errorOccurred() )
    {
      err = true;
      break;
    }
    if ( deadlockstate )
    {
      if ( trace_deadlock )
      {
        if ( basefilename == NULL )
        {
          basefilename = strdup(SpecFileName);
          char *s = strrchr(basefilename,'.');
          if ( s != NULL )
          {
            *s = '\0';
          }
        }
        stringstream ss;
        ss << basefilename << "_dlk_" << tracecnt << ".trc";
        string sss(ss.str());
        save_trace(sss,state,backpointers,nstate);

        if ( detect_deadlock || gsVerbose )
        {
          fprintf(stderr,"deadlock-detect: deadlock found and saved to '%s_dlk_%lu.trc'.\n",basefilename,tracecnt);
          fflush(stderr);
        }
        tracecnt++;
      } else if ( detect_deadlock ) {
        fprintf(stderr,"deadlock-detect: deadlock found.\n");
        fflush(stderr);
      }
    }

    current_state++;
    if ( (monitor || gsVerbose) && ((current_state%1000) == 0) )
    {
      fprintf(stderr,
        "monitor: currently at level %lu with %lu state%s and %lu transition%s explored and %lu state%s seen.\n",
        level,
        current_state,
        (current_state==1)?"":"s",
        trans,
        (trans==1)?"":"s",
        num_states,
        (num_states==1)?"":"s"
      );
    }
    if ( current_state == nextlevelat )
    {
      if ( monitor )
      {
        fprintf(stderr,
          "monitor: level %lu done. (%lu state%s, %lu transition%s)\n",
          level,current_state-prevcurrent,
          ((current_state-prevcurrent)==1)?"":"s",
          trans-prevtrans,
          ((trans-prevtrans)==1)?"":"s"
        );
        fflush(stderr);
      }
      level++;
      nextlevelat = num_states;
      prevcurrent = current_state;
      prevtrans = trans;
    }
/*    if ( current_state == nextlevelat )
    {
      if ( monitor )
      {
        fprintf(stderr,
          "monitor: Level %lu done. Currently %lu states visited and %lu states and %lu transitions explored.\n",
          level,
          num_states,
          current_state,
          trans
        );
        fflush(stderr);
      }
      level++;
      nextlevelat = num_states;
    }*/
  }
  delete nsgen;
  delete nstate;
  free(basefilename);
  }

  switch ( outformat )
  {
    case OF_AUT:
      rewind(aut);
      fprintf(aut,"des (0,%lu,%lu)",trans,num_states);
      fclose(aut);
      break;
    case OF_SVC:
      SVCclose(svc);
      break;
    default:
      break;
  }

  if ( !err && (monitor || gsVerbose))
  {
    fprintf(stderr,
      "done with state space generation (%lu level%s, %lu state%s and %lu transition%s).\n",
      level-1,
      (level==2)?"":"s",
      num_states,
      (num_states==1)?"":"s",
      trans,
      (trans==1)?"":"s"
    );
  }
}
