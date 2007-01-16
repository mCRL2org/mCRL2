#include <aterm2.h>
#include <assert.h>
#include <time.h>
#include <sstream>
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "lpe/data_specification.h"
#include "libdataelm.h"
#include "libnextstate.h"
#include "libenum.h"
#include "librewrite.h"
#include "libtrace.h"
#include "exploration.h"
#include "lts.h"

using namespace std;

exploration_strategy str_to_expl_strat(const char *s)
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

const char *expl_strat_to_str(exploration_strategy es)
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

void initialise_lts_generation_options(lts_generation_options &lgopts)
{
  lgopts.quiet = false;
  lgopts.verbose = false;
  lgopts.strat = GS_REWR_INNER;
  lgopts.usedummies = true;
  lgopts.removeunused = true;
  lgopts.stateformat = GS_STATE_VECTOR;
  lgopts.outformat = OF_UNKNOWN;
  lgopts.outinfo = true;
  lgopts.max_states = DEFAULT_MAX_STATES;
  lgopts.priority_action = "";
  lgopts.trace = false;
  lgopts.num_trace_actions = 0;
  lgopts.trace_actions = NULL;
  lgopts.max_traces = DEFAULT_MAX_TRACES;
  lgopts.detect_deadlock = false;
  lgopts.detect_action = false;
  lgopts.save_error_trace = false;
  lgopts.error_trace_saved = false;
  lgopts.expl_strat = es_breadth;
  lgopts.bithashing = false;
  lgopts.bithashsize = DEFAULT_BITHASHSIZE;
  lgopts.todo_max = ULONG_MAX;
  lgopts.initial_table_size = DEFAULT_INIT_TSIZE;
  lgopts.specification = "";
  lgopts.lts = "";
  lgopts.squadt = NULL;
}


static lts_generation_options *lgopts;

static NextState *nstate;

static ATermIndexedSet states;
static unsigned long long num_states;
static unsigned long long trans;
static unsigned long level;
static unsigned long long num_found_same;
static unsigned long long current_state;
static unsigned long long initial_state;
 
static ATermTable backpointers;
static unsigned long *bithashtable;
static ATermTable representation = NULL;
 
static unsigned long tracecnt;
 
static char *basefilename = NULL;

static bool lg_error = false;

bool initialise_lts_generation(lts_generation_options *opts)
{
  gsVerboseMsg("initialising...\n");

  lgopts = opts;
 
  FILE *SpecStream;
  if ( (SpecStream = fopen(lgopts->specification.c_str(), "rb")) == NULL )
  {
    gsErrorMsg("could not open input file '%s' for reading: ", lgopts->specification.c_str());
    perror(NULL);
    return false;
  }
  gsVerboseMsg("reading LPE from '%s'\n", lgopts->specification.c_str());
  ATerm Spec = ATreadFromFile(SpecStream);
  if ( Spec == NULL )
  {
    gsErrorMsg("could not read LPE from '%s'\n", lgopts->specification.c_str());
    return false;
  }
  assert(Spec != NULL);
  if (!gsIsSpecV1((ATermAppl) Spec)) {
    gsErrorMsg("'%s' does not contain an LPE\n", lgopts->specification.c_str());
    return false;
  }

  if ( lgopts->removeunused )
  {
    gsVerboseMsg("removing unused parts of the data specification.\n");
    Spec = (ATerm) removeUnusedData((ATermAppl) Spec);
  }

  basefilename = strdup(lgopts->specification.c_str());
  char *s = strrchr(basefilename,'.');
  if ( s != NULL )
  {
    *s = '\0';
  }

  if ( lgopts->bithashing )
  {
    unsigned long long bithashtablesize;
    if ( lgopts->bithashsize > ULLONG_MAX-4*sizeof(unsigned long) )
    {
      bithashtablesize = (1ULL << (sizeof(unsigned long long)*8-3)) / sizeof(unsigned long);
    } else {
      bithashtablesize = (lgopts->bithashsize+4*sizeof(unsigned long))/(8*sizeof(unsigned long));
    }
    bithashtable = (unsigned long *) calloc(bithashtablesize,sizeof(unsigned long)); // sizeof(unsigned int) * lgopts->bithashsize/(8*sizeof(unsigned int))
    if ( bithashtable == NULL )
    {
      gsErrorMsg("cannot create bit hash table\n");
      return false;
    }
  } else {
    states = ATindexedSetCreate(lgopts->initial_table_size,50);
  }
  
  if ( lgopts->trace )
  {
    backpointers = ATtableCreate(lgopts->initial_table_size,50);
  } else {
    backpointers = NULL;
  }
 
  nstate = createNextState((ATermAppl) Spec,!lgopts->usedummies,lgopts->stateformat,createEnumerator((ATermAppl) Spec,createRewriter(lpe::data_specification(ATAgetArgument((ATermAppl) Spec,0)),lgopts->strat),true),true);
 
  if ( lgopts->priority_action != "" )
  {
    gsVerboseMsg("applying confluence reduction with tau action '%s'...\n",lgopts->priority_action.c_str());
    nstate->prioritise(lgopts->priority_action.c_str());
    representation = ATtableCreate(lgopts->initial_table_size,50);
  }

  num_states = 0;
  trans = 0;
  level = 1;

  if ( lgopts->lts != "" )
  {
    if ( lgopts->outformat == OF_UNKNOWN )
    {
      const char *s = strrchr(lgopts->lts.c_str(),'.');
      if ( s == NULL )
      {
        gsWarningMsg("no extension given for output file; writing state space in SVC format\n",s);
        lgopts->outformat = OF_SVC;
      } else {
        s++;
        if ( !strcmp(s,"aut") )
        {
          lgopts->outformat = OF_AUT;
        } else {
          if ( strcmp(s,"svc") )
          {
            gsWarningMsg("extension '%s' of output file not recognised; writing state space in SVC format\n",s);
          }
          lgopts->outformat = OF_SVC;
        }
      }
    }

    lts_options lts_opts;
    lts_opts.outformat = lgopts->outformat;
    lts_opts.outinfo = lgopts->outinfo;
    lts_opts.nstate = nstate;
    open_lts(lgopts->lts.c_str(),lts_opts);
  } else {
    lgopts->outformat = OF_UNKNOWN;
    gsVerboseMsg("not saving state space.\n");
  }

  return true;
}
  
bool finalise_lts_generation()
{
  delete nstate;
  free(basefilename);

  close_lts(num_states,trans);

  if ( !lg_error && gsVerbose )
  {
    if ( lgopts->expl_strat == es_random )
    {
      gsVerboseMsg(
        "done with random walk of %llu transition%s (visited %llu unique state%s).\n",
        trans,
        (trans==1)?"":"s",
        num_states,
        (num_states==1)?"":"s"
      );
    } else if ( lgopts->expl_strat == es_breadth )
    {
      gsVerboseMsg(
        "done with state space generation (%lu level%s, %llu state%s and %llu transition%s).\n",
        level-1,
        (level==2)?"":"s",
        num_states,
        (num_states==1)?"":"s",
        trans,
        (trans==1)?"":"s"
      );
    } else if ( lgopts->expl_strat == es_depth )
    {
      gsVerboseMsg(
        "done with state space generation (%llu state%s and %llu transition%s).\n",
        num_states,
        (num_states==1)?"":"s",
        trans,
        (trans==1)?"":"s"
      );
    }
  }

  return true;
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

static bool savetrace(string const &info, ATerm state, ATermTable backpointers, NextState *nstate, ATerm extra_state = NULL, ATermAppl extra_transition = NULL)
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

  string filename;
  if ( lgopts->squadt->is_active() )
  {
    filename = lgopts->squadt->add_output_file(info,"trc");
  } else {
    filename = basefilename;
    filename += "_"+info+".trc";
  }
  return trace.save(filename);
}

static void check_actiontrace(ATerm OldState, ATermAppl Transition, ATerm NewState)
{
  // if ( lgopts->detect_action )
  for (int j=0; j<lgopts->num_trace_actions; j++)
  {
    if ( occurs_in(lgopts->trace_actions[j],ATLgetArgument(Transition,0)) )
    {
      if ( lgopts->trace && (tracecnt < lgopts->max_traces) )
      {
        if ( basefilename == NULL )
        {
        }
        std::ostringstream ss;
        ss << "act_" << tracecnt << "_" << ATgetName(ATgetAFun(lgopts->trace_actions[j]));
        string sss(ss.str());
        bool saved_ok = savetrace(sss,OldState,backpointers,nstate,NewState,Transition);

        if ( lgopts->detect_action || gsVerbose )
        {
          if ( saved_ok )
          {
            gsVerboseMsg("detect: action '%P' found and saved to '%s_act_%lu_%P.trc'.\n",Transition,basefilename,tracecnt,lgopts->trace_actions[j]);
          } else {
            gsVerboseMsg("detect: action '%P' found, but could not be saved to '%s_act_%lu_%P.trc'.\n",Transition,basefilename,tracecnt,lgopts->trace_actions[j]);
          }
          fflush(stderr);
        }
        tracecnt++;
      } else {
        gsVerboseMsg("detect: action '%P' found.\n",Transition);
        fflush(stderr);
      }
    }
  }
}

static void save_error_trace(ATerm state)
{
  if ( lgopts->save_error_trace )
  {
    bool saved_ok = savetrace("error",state,backpointers,nstate);

    if ( saved_ok )
    {
      lgopts->error_trace_saved = true;
      gsVerboseMsg("saved trace to error in '%s_trace.trc'.\n",basefilename);
    } else {
      gsVerboseMsg("trace to error could not be saved in '%s_trace.trc'.\n",basefilename);
    }
    fflush(stderr);
  }
}

static void check_deadlocktrace(ATerm state)
{
  if ( lgopts->detect_deadlock )
  {
    if ( lgopts->trace && (tracecnt < lgopts->max_traces) )
    {
      std::ostringstream ss;
      ss << "dlk_" << tracecnt;
      string sss(ss.str());
      bool saved_ok = savetrace(sss,state,backpointers,nstate);

      if ( lgopts->detect_deadlock || gsVerbose )
      {
        if ( saved_ok )
        {
          gsVerboseMsg("deadlock-detect: deadlock found and saved to '%s_dlk_%lu.trc'.\n",basefilename,tracecnt);
        } else {
          gsVerboseMsg("deadlock-detect: deadlock found, but could not be saved to '%s_dlk_%lu.trc'.\n",basefilename,tracecnt);
        }
        fflush(stderr);
      }
      tracecnt++;
    } else  {
      gsVerboseMsg("deadlock-detect: deadlock found.\n");
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
        if ( ATtableGet(number,NewState) == NULL ) // This condition was missing in the report
        {
          ATtablePut(number,NewState,(ATerm) ATmakeInt(0));
        }
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

  delete nsgen;
  ATtableDestroy(back);
  ATtableDestroy(next);
  ATtableDestroy(low);
  ATtableDestroy(number);
  ATindexedSetDestroy(visited);

  return v;
}

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
  return (((unsigned long long) (sh_a & 0xffff0000)) << 24) |
         (((unsigned long long) (sh_b & 0xffff0000)) << 16) |
         (((unsigned long long) (sh_c & 0xffff0000))     ) |
         (sh_a & 0x0000ffff)^(sh_b & 0x0000ffff)^(sh_c & 0x0000ffff);
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
  
  return calc_hash_finish() % lgopts->bithashsize;
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
  if ( lgopts->bithashing )
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
  if ( lgopts->bithashing )
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
        if ( queue_size == queue_size_max )
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

  i = add_state(to, &new_state);

  if ( new_state )
  {
    if ( num_states < lgopts->max_states )
    {
            num_states++;
            if ( lgopts->trace )
            {
                    ATtablePut(backpointers, to, (ATerm) ATmakeList2(from,(ATerm) action));
            }
    }
  } else {
    num_found_same++;
  }

  if ( lgopts->bithashing || (i < num_states) )
  {
    check_actiontrace(from,action,to);

    save_transition(state_index(from),from,action,i,to);
    trans++;
  }

  return new_state;
}

bool generate_lts()
{
  ATerm state = get_repr(nstate->getInitialState());
  save_initial_state(initial_state,state);

  bool new_state;
  initial_state = add_state(state,&new_state);
  current_state = 0;
  num_states++;

  lgopts->squadt->update_status_display(level,current_state,num_states,0,trans);

  if ( lgopts->max_states != 0 )
  {
    unsigned long long nextlevelat = 1;
    unsigned long long endoflevelat = 1;
    unsigned long long prevtrans = 0;
    unsigned long long prevcurrent = 0;
    unsigned long long statesskipped = 0;
    unsigned long long statestobeskipped = 0;
    num_found_same = 0;
    tracecnt = 0;
    gsVerboseMsg("generating state space with '%s' strategy...\n",expl_strat_to_str(lgopts->expl_strat));

    if ( lgopts->expl_strat == es_random )
    {
      srand((unsigned)time(NULL)+getpid());
      NextStateGenerator *nsgen = NULL;
      while ( current_state < lgopts->max_states )
      {
        ATermList tmp_trans = ATmakeList0();
        ATermList tmp_states = ATmakeList0();
        ATermAppl Transition;
        ATerm NewState;

        nsgen = nstate->getNextStates(state,nsgen);
        bool priority;
        while ( nsgen->next(&Transition,&NewState,&priority) )
        {
          NewState = get_repr(NewState);
          if ( !priority ) // don't store confluent self loops
          {
            tmp_trans = ATinsert(tmp_trans,(ATerm) Transition);
            tmp_states = ATinsert(tmp_states,NewState);
          }
        }

        if ( nsgen->errorOccurred() )
        {
          lg_error = true;
          save_error_trace(state);
          break;
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
          check_deadlocktrace(state);
          break;
        }

        current_state++;
        if ( (current_state%200) == 0 ) {
          lgopts->squadt->update_status_display(level,current_state,num_states,num_found_same,trans);
        }
        if ( gsVerbose && ((current_state%1000) == 0) )
        {
          gsVerboseMsg(
            "monitor: currently explored %llu transition%s and encountered %llu unique state%s.\n",
            trans,
            (trans==1)?"":"s",
            num_states,
            (num_states==1)?"":"s"
          );
        }
      }
      delete nsgen;
    } else if ( lgopts->expl_strat == es_breadth )
    {
      NextStateGenerator *nsgen = NULL;
      if ( lgopts->bithashing )
      {
        queue_size_max = lgopts->todo_max;
        add_to_queue(state);
        swap_queues();
      }
      while ( current_state < endoflevelat )
      {
        if ( lgopts->bithashing )
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
        bool priority;
        while ( nsgen->next(&Transition,&NewState,&priority) )
        {
          NewState = get_repr(NewState);
          if ( !priority ) // don't store confluent self loops
          {
            deadlockstate = false;
            bool b = add_transition(state,Transition,NewState);
            if ( lgopts->bithashing && b )
            {
              add_to_queue(NewState);
            }
          }
        }
        
        if ( nsgen->errorOccurred() )
        {
          lg_error = true;
          save_error_trace(state);
          break;
        }
        if ( deadlockstate )
        {
          check_deadlocktrace(state);
        }
  
        current_state++;
        if ( (current_state%200) == 0 ) {
          lgopts->squadt->update_status_display(level,current_state,num_states,num_found_same,trans);
        }
        if ( gsVerbose && ((current_state%1000) == 0) )
        {
          gsVerboseMsg(
            "monitor: currently at level %lu with %llu state%s and %llu transition%s explored and %llu state%s seen.\n",
            level,
            current_state-statesskipped,
            (current_state-statesskipped==1)?"":"s",
            trans,
            (trans==1)?"":"s",
            num_states,
            (num_states==1)?"":"s"
          );
        }
        if ( current_state == endoflevelat )
        {
          if ( lgopts->bithashing )
          {
            swap_queues();
          }
          current_state = current_state+statestobeskipped;
          lgopts->squadt->update_status_display(level,current_state,num_states,num_found_same,trans);
          if ( gsVerbose )
          {
            gsVerboseMsg(
              "monitor: level %lu done. (%llu state%s, %llu transition%s)\n",
              level,current_state-prevcurrent,
              ((current_state-prevcurrent)==1)?"":"s",
              trans-prevtrans,
              ((trans-prevtrans)==1)?"":"s"
            );
            fflush(stderr);
          }
          level++;
	  statesskipped = statesskipped+statestobeskipped;
          if ( lgopts->bithashing && (current_state+lgopts->todo_max < num_states) )
          {
            endoflevelat = current_state+lgopts->todo_max;
	    statestobeskipped = num_states-endoflevelat;
          } else {
            endoflevelat = num_states;
          }
	  current_state = nextlevelat;
          nextlevelat = num_states;
          prevcurrent = current_state;
          prevtrans = trans;
        }
      }
      delete nsgen;
    } else if ( lgopts->expl_strat == es_depth )
    {
      unsigned long nsgens_size = (lgopts->todo_max<128)?lgopts->todo_max:128;
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
      while ( (nsgens_num > 0) && (current_state < lgopts->max_states) )
      {
        NextStateGenerator *nsgen = nsgens[nsgens_num-1];
        state = nsgen->get_state();
        ATermAppl Transition;
        ATerm NewState;
        bool new_state = false;
        bool trans_seen_new = false;
        bool priority;
        if ( nsgen->next(&Transition,&NewState,&priority) )
        {
          NewState = get_repr(NewState);
          if ( !priority ) // don't store confluent self loops
          {
            top_trans_seen = true;
            if ( add_transition(state,Transition,NewState) )
            {
              new_state = true;
              if ( (nsgens_num == nsgens_size) && (nsgens_size < lgopts->todo_max) )
              {
                nsgens_size = nsgens_size*2;
                if ( nsgens_size > lgopts->todo_max )
                {
                  nsgens_size = lgopts->todo_max;
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
          }
        } else {
          nsgens_num--;
        }
        
        if ( nsgen->errorOccurred() )
        {
          lg_error = true;
          save_error_trace(state);
          break;
        }
        if ( !top_trans_seen )
        {
          check_deadlocktrace(state);
        }
        top_trans_seen = trans_seen_new;

        if ( new_state )
        {
          current_state++;
          if ( (current_state%200) == 0 ) {
            lgopts->squadt->update_status_display(level,current_state,num_states,num_found_same,trans);
          }
          if ( gsVerbose && ((current_state%1000) == 0) )
          {
            gsVerboseMsg(
              "monitor: currently explored %llu state%s and %llu transition%s.\n",
              current_state,
              (current_state==1)?"":"s",
              trans,
              (trans==1)?"":"s"
            );
          }
        }
      }
      lgopts->squadt->update_status_display(level,current_state,num_states,num_found_same,trans);

      for (unsigned long i=0; i<nsgens_size; i++)
      {
        delete nsgens[i];
      }
    } else {
      gsErrorMsg("unknown exploration strategy\n");
    }
  }
  
  return !lg_error;
}
