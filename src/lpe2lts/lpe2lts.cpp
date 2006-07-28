#define NAME "lpe2lts"
#define VERSION "0.5"

#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <cstring>
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

#include <boost/lexical_cast.hpp>
#ifndef ULLONG_MAX
#define ULLONG_MAX      18446744073709551615ULL
#endif

// Squadt protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <sip/tool.h>
#endif

#include "mcrl2_revision.h"

using namespace std;

#define DEFAULT_MAX_STATES ULLONG_MAX
#define DEFAULT_MAX_TRACES 10UL
#define DEFAULT_BITHASHSIZE 209715200ULL // ~25 MB 
#define DEFAULT_INIT_TSIZE 10000UL 

#define OF_UNKNOWN  0
#define OF_AUT    1
#define OF_SVC    2

enum exploration_strategy { es_none, es_breadth, es_depth, es_random };

static exploration_strategy str_to_expl_strat(const char *s)
{
  if ( !strcmp(s,"b") || !strcmp(s,"breadth") )
  {
    return es_breadth;
  }
  if ( !strcmp(s,"d") || !strcmp(s,"depth") )
  {
    return es_depth;
  }
  if ( !strcmp(s,"r") || !strcmp(s,"random") )
  {
    return es_random;
  }
  return es_none;
}

static const char *expl_strat_to_str(exploration_strategy es)
{
  switch ( es )
  {
    case es_breadth:
      return "breadth";
    case es_depth:
      return "depth";
    case es_random:
      return "random";
    default:
      return "unknown";
  }
}

static ATermAppl *parse_action_list(const char *s, int *len)
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
    "  -C, --confluence[=NAME]  apply on-the-fly confluence reduction with NAME the\n"
    "                           confluent tau action\n"
/*    "  -p, --priority=NAME   give priority to action NAME (i.e. if it is\n"
    "                        possible to execute an action NAME in some state,\n"
    "                        than make it the only executable action from that\n"
    "                        state)\n"*/
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


////////////////////////////////////////////////////////////////////////////////
//                        Global variable definitions                         //
////////////////////////////////////////////////////////////////////////////////

static bool quiet = false;
static bool verbose = false;
static RewriteStrategy strat = GS_REWR_INNER;
static bool usedummies = true;
static bool removeunused = true;
static int stateformat = GS_STATE_VECTOR;
static int outformat = OF_UNKNOWN;
static bool outinfo = true;
static unsigned long long max_states = DEFAULT_MAX_STATES;
static char *priority_action = NULL;
static bool trace = false;
static int num_trace_actions = 0;
static ATermAppl *trace_actions = NULL;
static unsigned long max_traces = DEFAULT_MAX_TRACES;
static bool detect_deadlock = false;
static bool detect_action = false;
static exploration_strategy expl_strat = es_breadth;
static bool bithashing = false;
static unsigned long *bithashtable;
static unsigned long long bithashsize = DEFAULT_BITHASHSIZE;
static unsigned long todo_max = ULONG_MAX;

static NextState *nstate;
  
static ATermAppl term_nil;
static AFun afun_pair;
static ATermIndexedSet states;
static unsigned long long num_states;
static unsigned long long trans;
static unsigned long level;
static unsigned long long num_found_same;
static unsigned long long current_state;
static unsigned long long initial_state;

static ATermTable backpointers;

static unsigned long tracecnt;

static char *basefilename = NULL;

////////////////////////////////////////////////////////////////////////////////
//                               LTS functions                                //
////////////////////////////////////////////////////////////////////////////////

FILE *aut = NULL;
SVCfile svcf;
SVCfile *svc = &svcf;
SVCparameterIndex svcparam = 0;

static void open_lts(const char *filename)
{
  switch ( outformat )
  {
    case OF_AUT:
      gsVerboseMsg("writing state space in AUT format to '%s'.\n",filename);
      outinfo = false;
      if ( (aut = fopen(filename,"wb")) == NULL )
      {
        gsErrorMsg("cannot open '%s' for writing\n",filename);
        exit(1);
      }
      break;
    case OF_SVC:
      gsVerboseMsg("writing state space in SVC format to '%s'.\n",filename);
      {
        SVCbool b;
        char *t = strdup(filename);

        b = outinfo?SVCfalse:SVCtrue;
        SVCopen(svc,t,SVCwrite,&b); // XXX check result
        free(t);
        SVCsetCreator(svc,NAME);
        if (outinfo)
          SVCsetType(svc, "mCRL2+info");
        else
          SVCsetType(svc, "mCRL2");
        svcparam = SVCnewParameter(svc,(ATerm) ATmakeList0(),&b);
      }
      break;
    default:
      gsVerboseMsg("not saving state space.\n");
      break;
  }
}

static void save_initial_state(ATerm state)
{
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
          SVCsetInitialState(svc,SVCnewState(svc,(ATerm) ATmakeInt(initial_state),&b));
        }
      }
      break;
    default:
      break;
  }
}

static void save_transition(unsigned long long idx_from, ATerm from, ATermAppl action, unsigned long long idx_to, ATerm to)
{
  switch ( outformat )
  {
    case OF_AUT:
      if ( idx_from == initial_state )
        idx_from = 0;
      if ( idx_to == initial_state )
        idx_to = 0;
      gsfprintf(aut,"(%llu,\"%P\",%llu)\n",idx_from,action,idx_to);
      fflush(aut);
      break;
    case OF_SVC:
      if ( outinfo )
      {
        SVCbool b;
        SVCputTransition(svc,
          SVCnewState(svc,(ATerm) nstate->makeStateVector(from),&b),
          SVCnewLabel(svc,(ATerm) ATmakeAppl2(afun_pair,(ATerm) action,(ATerm) term_nil),&b),
          SVCnewState(svc,(ATerm) nstate->makeStateVector(to),&b),
          svcparam);
      } else {
        SVCbool b;
        SVCputTransition(svc,
          SVCnewState(svc,(ATerm) ATmakeInt(idx_from),&b),
          SVCnewLabel(svc,(ATerm) ATmakeAppl2(afun_pair,(ATerm) action,(ATerm) term_nil),&b),
          SVCnewState(svc,(ATerm) ATmakeInt(idx_to),&b),
          svcparam);
      }
      break;
    default:
      break;
  }
}

static void close_lts()
{
  switch ( outformat )
  {
    case OF_AUT:
      rewind(aut);
      fprintf(aut,"des (0,%llu,%llu)",trans,num_states);
      fclose(aut);
      break;
    case OF_SVC:
      SVCclose(svc);
      break;
    default:
      break;
  }
}


////////////////////////////////////////////////////////////////////////////////
//                              Trace functions                               //
////////////////////////////////////////////////////////////////////////////////

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

static bool save_trace(string &filename, ATerm state, ATermTable backpointers, NextState *nstate, ATerm extra_state = NULL, ATermAppl extra_transition = NULL)
{
  ATerm s = state;
  ATerm ns;
  ATermList tr = ATmakeList0();
  
  if ( extra_state != NULL )
  {
    tr = ATinsert(tr,(ATerm) ATmakeList2((ATerm) extra_transition,extra_state));
  }
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
  
  return trace.save(filename);
}

static void check_action_trace(ATerm OldState, ATermAppl Transition, ATerm NewState)
{
  // if ( detect_action )
  for (int j=0; j<num_trace_actions; j++)
  {
    if ( occurs_in(trace_actions[j],ATLgetArgument(Transition,0)) )
    {
      if ( trace && (tracecnt < max_traces) )
      {
        if ( basefilename == NULL )
        {
        }
        stringstream ss;
        ss << basefilename << "_act_" << tracecnt << "_" << ATgetName(ATgetAFun(trace_actions[j])) << ".trc";
        string sss(ss.str());
        bool saved_ok = save_trace(sss,OldState,backpointers,nstate,NewState,Transition);

        if ( detect_action || gsVerbose )
        {
          if ( saved_ok )
          {
            gsfprintf(stderr,"detect: action '%P' found and saved to '%s_act_%lu_%P.trc'.\n",trace_actions[j],basefilename,tracecnt,trace_actions[j]);
          } else {
            gsfprintf(stderr,"detect: action '%P' found, but could not be saved to '%s_act_%lu_%P.trc'.\n",trace_actions[j],basefilename,tracecnt,trace_actions[j]);
          }
          fflush(stderr);
        }
        tracecnt++;
      } else {
        gsfprintf(stderr,"detect: action '%P' found.\n",trace_actions[j]);
        fflush(stderr);
      }
    }
  }
}

static void check_deadlock_trace(ATerm state)
{
  if ( detect_deadlock )
  {
    if ( trace && (tracecnt < max_traces) )
    {
      stringstream ss;
      ss << basefilename << "_dlk_" << tracecnt << ".trc";
      string sss(ss.str());
      bool saved_ok = save_trace(sss,state,backpointers,nstate);

      if ( detect_deadlock || gsVerbose )
      {
        if ( saved_ok )
        {
          fprintf(stderr,"deadlock-detect: deadlock found and saved to '%s_dlk_%lu.trc'.\n",basefilename,tracecnt);
        } else {
          fprintf(stderr,"deadlock-detect: deadlock found, but could not be saved to '%s_dlk_%lu.trc'.\n",basefilename,tracecnt);
        }
        fflush(stderr);
      }
      tracecnt++;
    } else  {
      fprintf(stderr,"deadlock-detect: deadlock found.\n");
      fflush(stderr);
    }
  }
}


////////////////////////////////////////////////////////////////////////////////
//                         Main exploration functions                         //
////////////////////////////////////////////////////////////////////////////////

// Confluence reduction based on S.C.C. Blom, Partial tau-confluence for
// Efficient State Space Generation, Technical Report SEN-R0123, CWI,
// Amsterdam, 2001

static ATermTable representation = NULL;

static ATerm get_repr(ATerm state)
{
  if ( representation == NULL )
  {
    return state;
  }

  ATerm t = ATtableGet(representation,state);
  if ( t != NULL )
  {
    return t;
  }

  ATerm v = state;
  ATermIndexedSet visited = ATindexedSetCreate(1000,50);
  int num_visited = 0;
  ATermTable number = ATtableCreate(1000,50);
  ATermTable low = ATtableCreate(1000,50);
  ATermTable next = ATtableCreate(1000,50);
  ATermTable back = ATtableCreate(1000,50);
  int count;
  NextStateGenerator *nsgen = NULL;

  ATtablePut(number,v,(ATerm) ATmakeInt(0));
  count = 0;
  bool notdone = true;
  while ( notdone )
  {
    if ( ATgetInt((ATermInt) ATtableGet(number,v)) == 0 )
    {
      count++;
      ATtablePut(number,v,(ATerm) ATmakeInt(count));
      ATtablePut(low,v,(ATerm) ATmakeInt(count));
      ATermList nextl = ATmakeList0();
      nsgen = nstate->getNextStates(v,nsgen);
      ATermAppl Transition;
      ATerm NewState;
      bool prioritised_action;
      while ( nsgen->next(&Transition,&NewState,&prioritised_action) && prioritised_action )
      {
        ATbool b;
        ATindexedSetPut(visited,NewState,&b);
        if ( b == ATtrue )
        {
          num_visited++;
        }
        ATerm t = ATtableGet(representation,NewState);
        if ( t != NULL )
        {
          v = t;
          notdone = false;
          break;
        }
        nextl = ATinsert(nextl,NewState);
        ATtablePut(number,NewState,(ATerm) ATmakeInt(0));
      }
      if ( !notdone )
      {
        break;
      }
      ATtablePut(next,v,(ATerm) nextl);
    }
    ATermList nextl = (ATermList) ATtableGet(next,v);
    if ( ATisEmpty(nextl) )
    {
      if ( ATisEqual(ATtableGet(number,v),ATtableGet(low,v)) )
      {
        break;
      }
      ATerm backv = ATtableGet(back,v);
      int a = ATgetInt((ATermInt) ATtableGet(low,backv));
      int b = ATgetInt((ATermInt) ATtableGet(low,v));
      if ( a < b )
      {
        ATtablePut(low,backv,(ATerm) ATmakeInt(a));
      } else {
        ATtablePut(low,backv,(ATerm) ATmakeInt(b));
      }
      v = backv;
    } else {
      ATerm u = ATgetFirst(nextl);
      ATtablePut(next,v,(ATerm) ATgetNext(nextl));
      int nu = ATgetInt((ATermInt) ATtableGet(number,u));
      if ( nu == 0 )
      {
        ATtablePut(back,u,v);
        v = u;
      } else {
        if ( nu < ATgetInt((ATermInt) ATtableGet(number,v)) )
        {
          int lv = ATgetInt((ATermInt) ATtableGet(low,v));
          if ( nu < lv )
          {
            ATtablePut(low,v,(ATerm) ATmakeInt(nu));
          }
        }
      }
    }
  }
  for (int i=0; i<num_visited; i++)
  {
    ATtablePut(representation,ATindexedSetGetElem(visited,i),v);
  }

  ATtableDestroy(back);
  ATtableDestroy(next);
  ATtableDestroy(low);
  ATtableDestroy(number);
  ATindexedSetDestroy(visited);

  return v;
}

#ifdef ENABLE_SQUADT_CONNECTIVITY
sip::tool::communicator tc;

static sip::layout::tool_display::sptr status_display;
static sip::layout::manager::aptr layout_manager;
static sip::layout::manager::aptr labels;
static sip::layout::elements::label *lb_level;
static sip::layout::elements::label *lb_explored;
static sip::layout::elements::label *lb_seen;
static sip::layout::elements::label *lb_transitions;
static sip::layout::elements::progress_bar *progbar;
static void create_status_display(sip::tool::communicator &tc)
{
  using namespace sip;
  using namespace sip::layout;
  using namespace sip::layout::elements;

  status_display = sip::layout::tool_display::sptr(new layout::tool_display);

  /* Create and add the top layout manager */
  layout_manager = layout::vertical_box::create();
  labels = layout::horizontal_box::create();

  /* First column */
  layout::vertical_box* column1 = new layout::vertical_box();
  layout::vertical_box* column2 = new layout::vertical_box();

  label* lb_level_text = new label("Level:");
  lb_level = new label("0");
  label* lb_explored_text = new label("States explored:");
  lb_explored = new label("0");
  label* lb_seen_text = new label("States seen:");
  lb_seen = new label("0");
  label* lb_transitions_text = new label("Transitions:");
  lb_transitions = new label("0");

  column1->add(lb_level_text, layout::left);
  column2->add(lb_level, layout::right);
  column1->add(lb_explored_text, layout::left);
  column2->add(lb_explored, layout::right);
  column1->add(lb_seen_text, layout::left);
  column2->add(lb_seen, layout::right);
  column1->add(lb_transitions_text, layout::left);
  column2->add(lb_transitions, layout::right);

  progbar = new progress_bar(0,0,0);

  /* Attach columns*/
  labels->add(column1, margins(0,5,0,5));
  labels->add(column2, margins(0,5,0,5));
  layout_manager->add(labels.get(), margins(0,5,0,5));
  layout_manager->add(progbar, margins(0,5,0,5));

  status_display->set_top_manager(layout_manager);

  tc.send_display_layout(status_display);
}

static void update_status_display(unsigned long level, unsigned long long explored, unsigned long long seen, unsigned long long num_found_same, unsigned long long transitions)
{
  fprintf(stderr,"sending new status...\n");
  char buf[21];
  sprintf(buf,"%lu",level);
  lb_level->set_text(buf,&tc);
  sprintf(buf,"%llu",explored);
  lb_explored->set_text(buf,&tc);
  sprintf(buf,"%llu",seen);
  lb_seen->set_text(buf,&tc);
  sprintf(buf,"%llu",transitions);
  lb_transitions->set_text(buf,&tc);
  progbar->set_maximum(seen,&tc);
  progbar->set_value(explored,&tc);
  fprintf(stderr,"done\n");
}
#endif

// 96 bit mix function of Robert Jenkins
#define mix(a,b,c) \
  { a -= b; a -= c; a ^= (c>>13); \
    b -= c; b -= a; b ^= (a<<8);  \
    c -= a; c -= b; c ^= (b>>13); \
    a -= b; a -= c; a ^= (c>>12); \
    b -= c; b -= a; b ^= (a<<16); \
    c -= a; c -= b; c ^= (b>>5);  \
    a -= b; a -= c; a ^= (c>>3);  \
    b -= c; b -= a; b ^= (a<<10); \
    c -= a; c -= b; c ^= (b>>15); \
  }

static unsigned long sh_a,sh_b,sh_c;
static unsigned int sh_i;
static void calc_hash_init()
{
  sh_a = 0x9e3779b9;
  sh_b = 0x65e3083a;
  sh_c = 0xa45f7582;
  sh_i = 0;
}
static void calc_hash_add(unsigned long n)
{
  switch ( sh_i )
  {
    case 0:
      sh_a += n;
      sh_i = 1;
      break;
    case 1:
      sh_b += n;
      sh_i = 2;
      break;
    case 2:
      sh_c += n;
      sh_i = 0;
      mix(sh_a,sh_b,sh_c);
      break;
  }
}
static unsigned long long calc_hash_finish()
{
  while ( sh_i != 0 )
  {
    calc_hash_add(0x76a34e87);
  }
  return sh_a^sh_b^sh_c;
}
static void calc_hash_aterm(ATerm t)
{
  switch ( ATgetType(t) )
  {
    case AT_APPL:
      calc_hash_add(0x13ad3780);
      {
        unsigned int len = ATgetArity(ATgetAFun((ATermAppl) t));
        for (unsigned int i=0; i<len; i++)
        {
          calc_hash_aterm(ATgetArgument((ATermAppl) t, i));
        }
      }
      break;
    case AT_LIST:
      calc_hash_add(0x7eb9cdba);
      for (ATermList l=(ATermList) t; !ATisEmpty(l); l=ATgetNext(l))
      {
        calc_hash_aterm(ATgetFirst(l));
      }
      break;
    case AT_INT:
      calc_hash_add(ATgetInt((ATermInt) t));
      break;
    default:
      calc_hash_add(0xaa143f06);
      break;
  }
}
static unsigned long long calc_hash(ATerm state)
{
  calc_hash_init();

  calc_hash_aterm(state);
  
  return calc_hash_finish() % bithashsize;
}

static bool get_bithash(unsigned long long i)
{
  return (( bithashtable[i/(8*sizeof(unsigned long))] >> (i%(8*sizeof(unsigned long))) ) & 1) == 1;
}

static void set_bithash(unsigned long long i)
{
  bithashtable[i/(8*sizeof(unsigned long))] |=  1 << (i%(8*sizeof(unsigned long)));
}

static unsigned long long add_state(ATerm state, bool *is_new)
{
  if ( bithashing )
  {
    unsigned long long i = calc_hash(state);
    *is_new = !get_bithash(i);
    set_bithash(i);
    return i;
  } else {
    ATbool new_state;
    unsigned long i = ATindexedSetPut(states,state,&new_state);
    *is_new = (new_state == ATtrue);
    return i;
  }
}

static unsigned long long state_index(ATerm state)
{
  if ( bithashing )
  {
    assert(get_bithash(calc_hash(state)));
    return calc_hash(state);
  } else {
    return ATindexedSetGetIndex(states,state);
  }
}


static ATerm *queue_get = NULL;
static ATerm *queue_put = NULL;
static unsigned long queue_size = 0;
static unsigned long queue_size_max = UINT_MAX;
static unsigned long queue_get_pos = 0;
static unsigned long queue_get_count = 0;
static unsigned long queue_put_count = 0;
static bool queue_size_fixed = false;

static void add_to_queue(ATerm state)
{
  if ( queue_put_count == queue_size )
  {
    if ( queue_size_fixed )
    {
      return;
    }
    if ( queue_size == 0 )
    {
      queue_size = (queue_size_max<128)?queue_size_max:128;
    } else {
      if ( 2*queue_size > queue_size_max )
      {
        queue_size_fixed = true;
        if ( queue_size = queue_size_max )
        {
          return;
        } else {
          queue_size = queue_size_max;
        }
      } else {
        queue_size = queue_size * 2;
      }
      ATunprotectArray(queue_get);
      ATunprotectArray(queue_put);
    }
    ATerm *tmp;
    tmp = (ATerm *) realloc(queue_get, queue_size*sizeof(ATerm));
    if ( tmp == NULL )
    {
      gsWarningMsg("cannot store all unexplored states (more than %lu); dropping some states from now on\n",queue_put_count);
      queue_size = queue_put_count;
      ATprotectArray(queue_get,queue_size);
      ATprotectArray(queue_put,queue_size);
      queue_size_fixed = true;
      return;
    }
    queue_get = tmp;
    tmp = (ATerm *) realloc(queue_put, queue_size*sizeof(ATerm));
    if ( tmp == NULL )
    {
      gsWarningMsg("cannot store all unexplored states (more than %lu); dropping some states from now on\n",queue_put_count);
      tmp = (ATerm *) realloc(queue_get, queue_size*sizeof(ATerm));
      if ( tmp != NULL )
      {
        queue_get = tmp;
      }
      queue_size = queue_put_count;
      ATprotectArray(queue_get,queue_size);
      ATprotectArray(queue_put,queue_size);
      queue_size_fixed = true;
      return;
    }
    queue_put = tmp;
    for (unsigned long i=queue_put_count; i<queue_size; i++)
    {
      queue_get[i] = NULL;
      queue_put[i] = NULL;
    }
    ATprotectArray(queue_get,queue_size);
    ATprotectArray(queue_put,queue_size);
  }

  queue_put[queue_put_count++] = state;
}

static ATerm get_from_queue()
{
  if ( queue_get_pos == queue_get_count )
  {
    return NULL;
  } else {
    return queue_get[queue_get_pos++];
  }
}

static void swap_queues()
{
  ATerm *t = queue_get;
  queue_get = queue_put;
  queue_put = t;
  queue_get_pos = 0;
  queue_get_count = queue_put_count;
  queue_put_count = 0;
}


static bool add_transition(ATerm from, ATermAppl action, ATerm to)
{
  bool new_state;
  unsigned long long i;

  to = get_repr(to);
  i = add_state(to, &new_state);

  if ( new_state )
  {
    if ( num_states < max_states )
    {
            num_states++;
            if ( trace )
            {
                    ATtablePut(backpointers, to, (ATerm) ATmakeList2(from,(ATerm) action));
            }
    }
  } else {
    num_found_same++;
  }

  if ( bithashing || (i < num_states) )
  {
    check_action_trace(from,action,to);

    save_transition(state_index(from),from,action,i,to);
    trans++;
  }

  return new_state;
}

static bool generate_lts()
{
  ATerm state = get_repr(nstate->getInitialState());
  save_initial_state(state);

  bool new_state;
  initial_state = add_state(state,&new_state);
  current_state = 0;
  num_states++;

#ifdef ENABLE_SQUADT_CONNECTIVITY
  if (tc.is_active()) {
    update_status_display(level,current_state,num_states,0,trans);
  }
#endif

  bool err = false;
  if ( max_states != 0 )
  {
    unsigned long long nextlevelat = 1;
    unsigned long long prevtrans = 0;
    unsigned long long prevcurrent = 0;
    num_found_same = 0;
    tracecnt = 0;
    gsVerboseMsg("generating state space with '%s' strategy...\n",expl_strat_to_str(expl_strat));

    if ( expl_strat == es_random )
    {
      srand((unsigned)time(NULL));
      NextStateGenerator *nsgen = NULL;
      while ( current_state < max_states )
      {
        ATermList tmp_trans = ATmakeList0();
        ATermList tmp_states = ATmakeList0();
        ATermAppl Transition;
        ATerm NewState;

        nsgen = nstate->getNextStates(state,nsgen);
        while ( nsgen->next(&Transition,&NewState) )
        {
          tmp_trans = ATinsert(tmp_trans,(ATerm) Transition);
          tmp_states = ATinsert(tmp_states,NewState);
        }

        int len = ATgetLength(tmp_trans);
        if ( len > 0 )
        {
          int i = rand()%len;
          while ( i > 0 )
          {
            tmp_trans = ATgetNext(tmp_trans);
            tmp_states = ATgetNext(tmp_states);
            i--;
          }
          add_transition(state,(ATermAppl) ATgetFirst(tmp_trans),ATgetFirst(tmp_states));
          state = ATgetFirst(tmp_states);
        } else {
          check_deadlock_trace(state);
          break;
        }

        current_state++;
#ifdef ENABLE_SQUADT_CONNECTIVITY
        if ( tc.is_active() && ((current_state%200) == 0) ) {
          update_status_display(level,current_state,num_states,num_found_same,trans);
        }
#endif
        if ( gsVerbose && ((current_state%1000) == 0) )
        {
          fprintf(stderr,
            "monitor: currently explored %llu transition%s and encountered %llu unique state%s.\n",
            trans,
            (trans==1)?"":"s",
            num_states,
            (num_states==1)?"":"s"
          );
        }
      }
      delete nsgen;
    } else if ( expl_strat == es_breadth )
    {
      NextStateGenerator *nsgen = NULL;
      if ( bithashing )
      {
        queue_size_max = todo_max;
        add_to_queue(state);
        swap_queues();
      }
      while ( current_state < num_states )
      {
        if ( bithashing )
        {
          state = get_from_queue();
          assert(state != NULL);
        } else {
          state = ATindexedSetGetElem(states,current_state);
        }
        bool deadlockstate = true;
  
        nsgen = nstate->getNextStates(state,nsgen);
        ATermAppl Transition;
        ATerm NewState;
        while ( nsgen->next(&Transition,&NewState) )
        {
          deadlockstate = false;
          bool b = add_transition(state,Transition,NewState);
          if ( bithashing && b )
          {
            add_to_queue(NewState);
          }
        }
        
        if ( nsgen->errorOccurred() )
        {
          err = true;
          break;
        }
        if ( deadlockstate )
        {
          check_deadlock_trace(state);
        }
  
        current_state++;
#ifdef ENABLE_SQUADT_CONNECTIVITY
        if ( tc.is_active() && ((current_state%200) == 0) ) {
          update_status_display(level,current_state,num_states,num_found_same,trans);
        }
#endif
        if ( gsVerbose && ((current_state%1000) == 0) )
        {
          fprintf(stderr,
            "monitor: currently at level %lu with %llu state%s and %llu transition%s explored and %llu state%s seen.\n",
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
          if ( bithashing )
          {
            swap_queues();
          }
#ifdef ENABLE_SQUADT_CONNECTIVITY
          if (tc.is_active()) {
            update_status_display(level,current_state,num_states,num_found_same,trans);
          }
#endif
          if ( gsVerbose )
          {
            fprintf(stderr,
              "monitor: level %lu done. (%llu state%s, %llu transition%s)\n",
              level,current_state-prevcurrent,
              ((current_state-prevcurrent)==1)?"":"s",
              trans-prevtrans,
              ((trans-prevtrans)==1)?"":"s"
            );
            fflush(stderr);
          }
          level++;
          if ( bithashing && (current_state+todo_max < num_states) )
          {
            nextlevelat = current_state+todo_max;
          } else {
            nextlevelat = num_states;
          }
          prevcurrent = current_state;
          prevtrans = trans;
        }
      }
      delete nsgen;
    } else if ( expl_strat == es_depth )
    {
      unsigned long nsgens_size = (todo_max<128)?todo_max:128;
      NextStateGenerator **nsgens = (NextStateGenerator **) malloc(nsgens_size*sizeof(NextStateGenerator *));
      if ( nsgens == NULL )
      {
        gsErrorMsg("cannot create state stack\n");
        exit(1);
      }
      nsgens[0] = nstate->getNextStates(state);
      for (unsigned long i=1; i<nsgens_size; i++)
      {
        nsgens[i] = NULL;
      }
      unsigned long nsgens_num = 1;

      bool top_trans_seen = false;
      // trans_seen(s) := we have seen a transition from state s
      // inv:  forall i : 0 <= i < nsgens_num-1 : trans_seen(nsgens[i]->get_state())
      //       nsgens_num > 0  ->  top_trans_seen == trans_seen(nsgens[nsgens_num-1])
      while ( nsgens_num > 0 )
      {
        NextStateGenerator *nsgen = nsgens[nsgens_num-1];
        state = nsgen->get_state();
        ATermAppl Transition;
        ATerm NewState;
        bool new_state = false;
        bool trans_seen_new = false;
        if ( nsgen->next(&Transition,&NewState) )
        {
          top_trans_seen = true;
          if ( add_transition(state,Transition,NewState) )
          {
            new_state = true;
            if ( (nsgens_num == nsgens_size) && (nsgens_size < todo_max) )
            {
              nsgens_size = nsgens_size*2;
              if ( nsgens_size > todo_max )
              {
                nsgens_size = todo_max;
              }
              nsgens = (NextStateGenerator **) realloc(nsgens,nsgens_size*sizeof(NextStateGenerator *));
              if ( nsgens == NULL )
              {
                gsErrorMsg("cannot enlarge state stack\n");
                exit(1);
              }
              for (unsigned long i=nsgens_num; i<nsgens_size; i++)
              {
                nsgens[i] = NULL;
              }
            }
            if ( nsgens_num < nsgens_size )
            {
              nsgens[nsgens_num] = nstate->getNextStates(NewState,nsgens[nsgens_num]);
              nsgens_num++;
              trans_seen_new = false;
            }
          }
        } else {
          nsgens_num--;
        }
        
        if ( nsgen->errorOccurred() )
        {
          err = true;
          break;
        }
        if ( !top_trans_seen )
        {
          check_deadlock_trace(state);
        }
        top_trans_seen = trans_seen_new;

        if ( new_state )
        {
          current_state++;
#ifdef ENABLE_SQUADT_CONNECTIVITY
          if ( tc.is_active() && ((current_state%200) == 0) ) {
            update_status_display(level,current_state,num_states,num_found_same,trans);
          }
#endif
          if ( gsVerbose && ((current_state%1000) == 0) )
          {
            fprintf(stderr,
              "monitor: currently explored %llu state%s and %llu transition%s.\n",
              current_state,
              (current_state==1)?"":"s",
              trans,
              (trans==1)?"":"s"
            );
          }
        }
      }
#ifdef ENABLE_SQUADT_CONNECTIVITY
      if ( tc.is_active() ) {
        update_status_display(level,current_state,num_states,num_found_same,trans);
      }
#endif

      for (unsigned long i=0; i<nsgens_size; i++)
      {
        delete nsgens[i];
      }
    } else {
      gsErrorMsg("unknown exploration strategy\n");
    }
  }
  
  return err;
}


#ifdef ENABLE_SQUADT_CONNECTIVITY
static const unsigned int lpd_file_for_input_no_lts = 0;
static const unsigned int lpd_file_for_input_lts = 1;
static const unsigned int lts_file_for_output = 2;

enum lpe2lts_options {
  option_out_info,

  option_usedummies,
  option_state_format_tree,
  option_removeunused,

  option_rewr_strat,

  option_expl_strat,
  
  option_detect_deadlock,
  option_detect_actions,
  option_trace,
  option_max_traces,
  
  option_confluence_reduction,
  option_confluent_tau,
  
  option_max_states,
  
  option_bithashing,
  option_bithashsize,
  
  option_init_tsize
};

static bool validate_configuration(sip::configuration &c)
{
  return (
      c.object_exists(lpd_file_for_input_no_lts) ||
      (c.object_exists(lpd_file_for_input_lts) &&
       c.object_exists(lts_file_for_output))
      );
}

/* Communicate the basic configuration display, and wait until the ok button was pressed */
void set_basic_configuration_display(sip::tool::communicator& tc, bool make_lts) {
  using namespace sip;
  using namespace sip::layout;
  using namespace sip::layout::elements;

  //status_display = sip::layout::tool_display::sptr(new layout::tool_display);
  //layout::tool_display::sptr display(status_display);
  layout::tool_display::sptr display(new layout::tool_display);

  /* Create and add the top layout manager */
  layout::manager::aptr layout_manager = layout::horizontal_box::create();

  /* First column */
  layout::vertical_box* column = new layout::vertical_box();

  
  char buf[21];

  checkbox* cb_aut = make_lts?(new checkbox("generate aut file", false)):NULL;
  checkbox* cb_out_info = new checkbox("save state information", true);
  
  checkbox* cb_usedummies = new checkbox("fill in free variables", true);
  checkbox* cb_state_format_tree = new checkbox("memory efficient state repr.", false);
  checkbox* cb_removeunused = new checkbox("remove unused data", true);
  
  label* lb_rewr_strat = new label("Rewriter:");
  radio_button* rb_rewr_strat_inner = new radio_button("innermost");
  radio_button* rb_rewr_strat_jitty = new radio_button("JITty",rb_rewr_strat_inner,false);
  radio_button* rb_rewr_strat_innerc = new radio_button("compiling innermost",rb_rewr_strat_inner,false);
  radio_button* rb_rewr_strat_jittyc = new radio_button("compiling JITty",rb_rewr_strat_inner,false);
  
  label* lb_expl_strat = new label("Strategy:");
  radio_button* rb_expl_strat_breadth = new radio_button("breadth-first");
  radio_button* rb_expl_strat_depth = new radio_button("depth-first",rb_expl_strat_breadth,false);
  radio_button* rb_expl_strat_random = new radio_button("random",rb_expl_strat_breadth,false);
  
  checkbox* cb_deadlock = new checkbox("detect deadlocks", false);
  label* lb_actions = new label("detect actions:");
  text_field* tf_actions = new text_field("", sip::datatype::string::standard);
  checkbox* cb_trace = new checkbox("save action/deadlock traces", false);
  label* lb_max_traces = new label("at most:");
  sprintf(buf,"%lu",DEFAULT_MAX_TRACES);
  text_field* tf_max_traces = new text_field(buf, sip::datatype::integer::standard);
  
  checkbox* cb_confluence = new checkbox("confluence reduction", false);
  text_field* tf_conf_tau = new text_field("tau", sip::datatype::string::standard);
  
  
  label* lb_max_states = new label("maximum number of states:");
  sprintf(buf,"%llu",DEFAULT_MAX_STATES);
  text_field* tf_max_states = new text_field(buf, sip::datatype::integer::standard);

  checkbox* cb_bithashing = new checkbox("bit hashing", false);
  label* lb_bithashsize = new label("states:");
  sprintf(buf,"%llu",DEFAULT_BITHASHSIZE);
  text_field* tf_bithashsize = new text_field(buf, sip::datatype::integer::standard);

  label* lb_init_tsize = new label("initial table size:");
  sprintf(buf,"%lu",DEFAULT_INIT_TSIZE);
  text_field* tf_init_tsize = new text_field(buf, sip::datatype::integer::standard);

  if ( make_lts )
    column->add(cb_aut, layout::left);
  column->add(cb_out_info, layout::left);

  column->add(cb_usedummies, layout::left);
  column->add(cb_state_format_tree, layout::left);
  column->add(cb_removeunused, layout::left);

  column->add(lb_rewr_strat, layout::left);
  column->add(rb_rewr_strat_inner, layout::left);
  column->add(rb_rewr_strat_jitty, layout::left);
  column->add(rb_rewr_strat_innerc, layout::left);
  column->add(rb_rewr_strat_jittyc, layout::left);

  column->add(lb_expl_strat, layout::left);
  column->add(rb_expl_strat_breadth, layout::left);
  column->add(rb_expl_strat_depth, layout::left);
  column->add(rb_expl_strat_random, layout::left);

  column->add(cb_deadlock, layout::left);
  column->add(lb_actions, layout::left);
  column->add(tf_actions, layout::left);
  column->add(cb_trace, layout::left);
  column->add(lb_max_traces, layout::left);
  column->add(tf_max_traces, layout::left);

  column->add(cb_confluence, layout::left);
  column->add(tf_conf_tau, layout::left);

  column->add(lb_max_states, layout::left);
  column->add(tf_max_states, layout::left);

  column->add(cb_bithashing, layout::left);
  column->add(lb_bithashsize, layout::left);
  column->add(tf_bithashsize, layout::left);

  column->add(lb_init_tsize, layout::left);
  column->add(tf_init_tsize, layout::left);

  button* okay_button = new button("Ok");

  column->add(okay_button, layout::right);

  /* Attach columns*/
  layout_manager->add(column, margins(0,5,0,5));

  display->set_top_manager(layout_manager);

  tc.send_display_layout(display);

  /* Wait until the ok button was pressed */
  okay_button->await_change();

  /* Update the current configuration */
  sip::configuration& c = tc.get_configuration();

  /* Values for the options */
  if ( make_lts )
  {
    std::string input_file_name = c.get_object(lpd_file_for_input_lts)->get_location();
    /* Add output file to the configuration */
    c.add_output(lts_file_for_output, (cb_aut->get_status()?"aut":"svc"), c.get_output_name(cb_aut->get_status()?".aut":".svc"));
  }
  c.add_option(option_out_info).append_argument(sip::datatype::boolean::standard, cb_out_info->get_status());

  c.add_option(option_usedummies).append_argument(sip::datatype::boolean::standard, cb_usedummies->get_status());
  c.add_option(option_state_format_tree).append_argument(sip::datatype::boolean::standard, cb_state_format_tree->get_status());
  c.add_option(option_removeunused).append_argument(sip::datatype::boolean::standard, cb_removeunused->get_status());
  
  if ( rb_rewr_strat_inner->is_selected() ) c.add_option(option_rewr_strat).append_argument(sip::datatype::integer::standard, (long int) GS_REWR_INNER);
  if ( rb_rewr_strat_jitty->is_selected() ) c.add_option(option_rewr_strat).append_argument(sip::datatype::integer::standard, (long int) GS_REWR_JITTY);
  if ( rb_rewr_strat_innerc->is_selected() ) c.add_option(option_rewr_strat).append_argument(sip::datatype::integer::standard, (long int) GS_REWR_INNERC);
  if ( rb_rewr_strat_jittyc->is_selected() ) c.add_option(option_rewr_strat).append_argument(sip::datatype::integer::standard, (long int) GS_REWR_JITTYC);
  
  if ( rb_expl_strat_breadth->is_selected() ) c.add_option(option_expl_strat).append_argument(sip::datatype::integer::standard, (long int) es_breadth);
  if ( rb_expl_strat_depth->is_selected() ) c.add_option(option_expl_strat).append_argument(sip::datatype::integer::standard, (long int) es_depth);
  if ( rb_expl_strat_random->is_selected() ) c.add_option(option_expl_strat).append_argument(sip::datatype::integer::standard, (long int) es_random);
  if ( rb_expl_strat_breadth->is_selected() ) printf("breadth selected\n");
  if ( rb_expl_strat_depth->is_selected() ) printf("depth selected\n");
  if ( rb_expl_strat_random->is_selected() ) printf("random selected\n");

  c.add_option(option_detect_deadlock).append_argument(sip::datatype::boolean::standard, cb_deadlock->get_status());
  c.add_option(option_detect_actions).append_argument(sip::datatype::string::standard, tf_actions->get_text());
  c.add_option(option_trace).append_argument(sip::datatype::boolean::standard, cb_trace->get_status());
  c.add_option(option_max_traces).append_argument(sip::datatype::string::standard, tf_max_traces->get_text());
  
  c.add_option(option_confluence_reduction).append_argument(sip::datatype::boolean::standard, cb_confluence->get_status());
  c.add_option(option_confluent_tau).append_argument(sip::datatype::string::standard, tf_conf_tau->get_text());
  
  c.add_option(option_max_states).append_argument(sip::datatype::string::standard, tf_max_states->get_text());
  
  c.add_option(option_bithashing).append_argument(sip::datatype::boolean::standard, cb_bithashing->get_status());
  c.add_option(option_bithashsize).append_argument(sip::datatype::string::standard, tf_bithashsize->get_text());
  
  c.add_option(option_init_tsize).append_argument(sip::datatype::string::standard, tf_init_tsize->get_text());
  
  tc.clear_display();
}
#endif

////////////////////////////////////////////////////////////////////////////////
//                               main function                                //
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
  string spec_fn, lts_fn;
  FILE *SpecStream;
  ATerm Spec;
  unsigned long initial_table_size = DEFAULT_INIT_TSIZE;
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
  
    ATinit(a_argc,a_argv,&Spec);
  }
  // end handle aterm lib options 

#ifdef ENABLE_SQUADT_CONNECTIVITY
  /* Get tool capabilities in order to modify settings */
  sip::tool::capabilities& cp = tc.get_tool_capabilities();

  /* The tool has only one main input combination it takes an LPE and then behaves as a reporter */
  cp.add_input_combination(lpd_file_for_input_no_lts, "Reporting", "lpe");
  cp.add_input_combination(lpd_file_for_input_lts, "Transformation", "lpe");

  /* On purpose we do not catch exceptions */
  if (tc.activate(argc,argv)) {
    gsSetVerboseMsg();

    bool valid = false;
    bool make_lts = false;

    /* Static configuration cycle */
    while (!valid) {
      /* Wait for configuration data to be send (either a previous configuration, or only an input combination) */
      sip::configuration::sptr configuration = tc.await_configuration();

      /* Validate configuration specification, should contain a file name of an LPD that is to be read as input */
      valid  = configuration.get() != 0;
      bool valid_0 = configuration->object_exists(lpd_file_for_input_no_lts);
      bool valid_1 = configuration->object_exists(lpd_file_for_input_lts);
      valid &= valid_0 | valid_1;
      make_lts = valid_1;

      if (valid) {
        /* An object with the correct id exists, assume the URI is relative (i.e. a file name in the local file system) */
        spec_fn = configuration->get_object(valid_0?lpd_file_for_input_no_lts:lpd_file_for_input_lts)->get_location();
      } else {
        tc.send_status_report(sip::report::error, "Invalid input combination!");
      }
    }

    sip::configuration& cf = tc.get_configuration();
    
    if (cf.is_fresh()) {
      /* Configuration is incomplete or incorrect; prompt the user */

      /* Draw a configuration layout in the tool display */
      set_basic_configuration_display(tc,make_lts);
     
      /* Static configuration cycle (phase 2: gather user input) */
      if (!validate_configuration(tc.get_configuration())) {
        /* Wait for configuration data to be send (either a previous configuration, or only an input combination) */
        tc.send_status_report(sip::report::error, "The configuration is invalid");
     
        exit(1);
      }
    }

    /* Send the controller the signal that we're ready to rumble (no further configuration necessary) */
    tc.send_accept_configuration();

    /* Wait for start message */
    gsVerboseMsg("waiting for start signal...\n");
    tc.await_message(sip::message_signal_start);
    gsVerboseMsg("starting execution...\n");

    sip::configuration& c = tc.get_configuration();

    if ( c.object_exists(lts_file_for_output) )
    {
      lts_fn = c.get_object(lts_file_for_output)->get_location();
    }

    outinfo = boost::any_cast <bool> (*(c.get_option(option_out_info)->get_value_iterator()));

    usedummies = boost::any_cast <bool> (*(c.get_option(option_usedummies)->get_value_iterator()));
    stateformat = (boost::any_cast <bool> (*(c.get_option(option_state_format_tree)->get_value_iterator())))?GS_STATE_TREE:GS_STATE_VECTOR;
    removeunused = boost::any_cast <bool> (*(c.get_option(option_removeunused)->get_value_iterator()));
    
    max_traces = strtoul((boost::any_cast <string> (*(c.get_option(option_max_traces)->get_value_iterator()))).c_str(),NULL,0);

    strat = (RewriteStrategy) boost::any_cast <long int> (*(c.get_option(option_rewr_strat)->get_value_iterator()));
    
    expl_strat = (exploration_strategy) boost::any_cast <long int> (*(c.get_option(option_expl_strat)->get_value_iterator()));
    
    detect_deadlock = boost::any_cast <bool> (*(c.get_option(option_detect_deadlock)->get_value_iterator()));
    string s = boost::any_cast <string> (*(c.get_option(option_detect_actions)->get_value_iterator()));
    if ( s != "" )
    {
      detect_action = true;
      trace_actions = parse_action_list(s.c_str(),&num_trace_actions);
    }
    trace = boost::any_cast <bool> (*(c.get_option(option_trace)->get_value_iterator()));
    max_traces = strtoul((boost::any_cast <string> (*(c.get_option(option_max_traces)->get_value_iterator()))).c_str(),NULL,0);
    
    if ( boost::any_cast <bool> (*(c.get_option(option_confluence_reduction)->get_value_iterator())) )
    {
      priority_action = strdup((boost::any_cast <string> (*(c.get_option(option_confluent_tau)->get_value_iterator()))).c_str());
    }
    
    max_states = boost::lexical_cast < unsigned long long > ((
        boost::any_cast <string> (*(c.get_option(option_max_states)->get_value_iterator()))));
    
    bithashing = boost::any_cast <bool> (*(c.get_option(option_bithashing)->get_value_iterator()));
    bithashsize = boost::lexical_cast < unsigned long long > (
        (boost::any_cast <string> (*(c.get_option(option_bithashsize)->get_value_iterator()))));
    
    initial_table_size = strtoul((boost::any_cast <string> (*(c.get_option(option_init_tsize)->get_value_iterator()))).c_str(),NULL,0);
  }
  else {
#endif
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
      case 'b':
        bithashing = true;
        if ( optarg != NULL )
        {
          if ( (optarg[0] >= '0') && (optarg[0] <= '9') )
          {
            bithashsize = boost::lexical_cast < unsigned long long > (optarg);
          } else {
            gsErrorMsg("invalid argument to -b/--bit-hash\n",optarg);
            return 1;
          }
        }
        break;
      case 'l':
        if ( (optarg[0] >= '0') && (optarg[0] <= '9') )
        {
          max_states = boost::lexical_cast < unsigned long long > (optarg);
        } else {
          gsErrorMsg("invalid argument to -l/--max\n",optarg);
          return 1;
        }
        break;
      case 'd':
        detect_deadlock = true;
        break;
      case 'a':
        detect_action = true;
        trace_actions = parse_action_list(optarg,&num_trace_actions);
        break;
      case 't':
        trace = true;
        if ( optarg != NULL )
        {
          if ( (optarg[0] >= '0') && (optarg[0] <= '9') )
          {
            max_traces = strtoul(optarg,NULL,0);
          } else {
            gsErrorMsg("invalid argument to -t/--trace\n",optarg);
            return 1;
          }
        }
        break;
      case 'C':
        if ( optarg != NULL )
        {
          priority_action = strdup(optarg);
        } else {
          priority_action = strdup("tau");
        }
        break;
      case 's':
        expl_strat = str_to_expl_strat(optarg);
        if ( expl_strat == es_none )
        {
          gsErrorMsg("invalid exploration strategy '%s'\n",optarg);
          return 1;
        }
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
      case 4:
        if ( (optarg[0] >= '0') && (optarg[0] <= '9') )
        {
          initial_table_size = strtoul(optarg,NULL,0);
        } else {
          gsErrorMsg("invalid argument to --init-tsize\n",optarg);
          return 1;
        }
        break;
      case 5:
        if ( (optarg[0] >= '0') && (optarg[0] <= '9') )
        {
          todo_max = strtoul(optarg,NULL,0);
        } else {
          gsErrorMsg("invalid argument to --todo-max\n",optarg);
          return 1;
        }
        break;
      default:
        break;
    }
  }
  if ( argc-optind >= 1 )
  {
    spec_fn = argv[optind];
  }
  if ( argc-optind > 1 )
  {
    lts_fn = argv[optind+1];
  }
#ifdef ENABLE_SQUADT_CONNECTIVITY
  }
#endif
  if ( quiet && verbose )
  {
    gsErrorMsg("options -q/--quiet and -v/--verbose cannot be used together\n");
    return 1;
  }
  if ( bithashing && trace )
  {
    gsErrorMsg("options -b/--bit-hash and -t/--trace cannot be used together\n");
    return 1;
  }
  if ( quiet )
    gsSetQuietMsg();
  if ( verbose )
    gsSetVerboseMsg();
  
  if ( spec_fn == "" )
  {
    print_help_suggestion(stderr,argv[0]);
    return 1;
  }
  if ( (SpecStream = fopen(spec_fn.c_str(), "rb")) == NULL )
  {
    gsErrorMsg("could not open input file '%s' for reading: ", spec_fn.c_str());
    perror(NULL);
    return 1;
  }
  gsVerboseMsg("reading LPE from '%s'\n", spec_fn.c_str());
  Spec = ATreadFromFile(SpecStream);
  if ( Spec == NULL )
  {
    gsErrorMsg("could not read LPE from '%s'\n", spec_fn.c_str());
    return 1;
  }
  assert(Spec != NULL);
  gsEnableConstructorFunctions();
  if (!gsIsSpecV1((ATermAppl) Spec)) {
    gsErrorMsg("'%s' does not contain an LPE\n", spec_fn.c_str());
    return 1;
  }

  if ( removeunused )
  {
    gsVerboseMsg("removing unused parts of the data specification.\n");
    Spec = (ATerm) removeUnusedData((ATermAppl) Spec);
  }

  if ( lts_fn != "" )
  {
    if ( outformat == OF_UNKNOWN )
    {
      char *s = strrchr(lts_fn.c_str(),'.');
      if ( s == NULL )
      {
        gsWarningMsg("no extension given for output file; writing state space in SVC format\n",s);
        outformat = OF_SVC;
      } else {
        s++;
        if ( !strcmp(s,"aut") )
        {
          outformat = OF_AUT;
        } else {
          if ( strcmp(s,"svc") )
          {
            gsWarningMsg("extension '%s' of output file not recognised; writing state space in SVC format\n",s);
          }
          outformat = OF_SVC;
        }
      }
    }

    open_lts(lts_fn.c_str());
  } else {
    outformat = OF_UNKNOWN;
    gsVerboseMsg("not saving state space.\n");
  }

  gsVerboseMsg("initialising...\n");
  
  basefilename = strdup(spec_fn.c_str());
  char *s = strrchr(basefilename,'.');
  if ( s != NULL )
  {
    *s = '\0';
  }

  term_nil = gsMakeNil();
  ATprotectAppl(&term_nil);
  afun_pair = ATmakeAFun("pair",2,ATfalse);
  ATprotectAFun(afun_pair);
  if ( bithashing )
  {
    bithashtable = (unsigned long *) malloc(bithashsize/8); // sizeof(unsigned int) * bithashsize/(8*sizeof(unsigned int))
    if ( bithashtable == NULL )
    {
      gsErrorMsg("cannot create bit hash table\n");
      return 1;
    }
  } else {
    states = ATindexedSetCreate(initial_table_size,50);
  }
  
  if ( trace )
  {
    backpointers = ATtableCreate(initial_table_size,50);
  } else {
    backpointers = NULL;
  }
  
  nstate = createNextState((ATermAppl) Spec,!usedummies,stateformat,createEnumerator((ATermAppl) Spec,createRewriter(ATAgetArgument((ATermAppl) Spec,3),strat),true),true);
  
  if ( priority_action != NULL )
  {
    gsVerboseMsg("applying confluence reduction with tau action '%s'...\n",priority_action);
    nstate->prioritise(priority_action);
    representation = ATtableCreate(initial_table_size,50);
  }

  num_states = 0;
  trans = 0;
  level = 1;

#ifdef ENABLE_SQUADT_CONNECTIVITY
  if (tc.is_active()) {
    gsVerboseMsg("creating status display...\n");
    create_status_display(tc);
    gsVerboseMsg("done\n");
  }
#endif

  bool err = generate_lts();

  delete nstate;
  free(basefilename);

  close_lts();

  if ( !err && gsVerbose )
  {
#ifdef ENABLE_SQUADT_CONNECTIVITY
    if (tc.is_active()) {
      tc.send_status_report(sip::report::notice, "done with state space generation");

      tc.send_signal_done();

      tc.await_message(sip::message_request_termination);

    } else {
#endif
    if ( expl_strat == es_random )
    {
      fprintf(stderr,
        "done with random walk of %llu transition%s (visited %llu unique state%s).\n",
        trans,
        (trans==1)?"":"s",
        num_states,
        (num_states==1)?"":"s"
      );
    } else if ( expl_strat == es_breadth )
    {
      fprintf(stderr,
        "done with state space generation (%lu level%s, %llu state%s and %llu transition%s).\n",
        level-1,
        (level==2)?"":"s",
        num_states,
        (num_states==1)?"":"s",
        trans,
        (trans==1)?"":"s"
      );
    } else if ( expl_strat == es_depth )
    {
      fprintf(stderr,
        "done with state space generation (%llu state%s and %llu transition%s).\n",
        num_states,
        (num_states==1)?"":"s",
        trans,
        (trans==1)?"":"s"
      );
    }
#ifdef ENABLE_SQUADT_CONNECTIVITY
    }
#endif
  }
}
