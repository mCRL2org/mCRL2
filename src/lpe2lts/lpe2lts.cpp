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

// Squadt protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <sip/tool.h>
#endif

#include "mcrl2_revision.h"

using namespace std;

#define OF_UNKNOWN  0
#define OF_AUT    1
#define OF_SVC    2

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
    "  -d, --deadlock        detect deadlocks (i.e. for every deadlock a message is\n"
    "                        printed)\n"
    "  -a, --action=NAME*    detect actions from NAME* (i.e. print a message for\n"
    "                        every occurrence)\n"
    "  -t, --trace[=NUM]     write at most NUM traces to states detected with the\n"
    "                        --deadlock or --action options\n"
    "                        (NUM is 10 by default)\n"
    "  -p, --priority=NAME   give priority to action NAME (i.e. if it is\n"
    "                        possible to execute an action NAME in some state,\n"
    "                        than make it the only executable action from that\n"
    "                        state)\n"
    "  -R, --rewriter=NAME   use rewriter NAME (default 'inner')\n"
    "      --aut             force OUTFILE to be in the aut format (implies\n"
    "                        --no-info, see below)\n"
    "      --svc             force OUTFILE to be in the svc format\n"
    "      --no-info         do not add state information to OUTFILE\n"
    "      --init-tsize=NUM  set the initial size of the internally used hash\n"
    "                        tables (default is 10000)\n",
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
static unsigned long max_states = ULONG_MAX;
static char *priority_action = NULL;
static bool trace = false;
static int num_trace_actions = 0;
static ATermAppl *trace_actions = NULL;
static unsigned long max_traces = 10;
static bool detect_deadlock = false;
static bool detect_action = false;

static NextState *nstate;
  
static ATermAppl term_nil;
static AFun afun_pair;
static ATermIndexedSet states;
static unsigned long num_states;
static unsigned long trans;
static unsigned long level;

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
          SVCsetInitialState(svc,SVCnewState(svc,(ATerm) ATmakeInt(0),&b));
        }
      }
      break;
    default:
      break;
  }
}

static void save_transition(unsigned long from, ATermAppl action, unsigned long to)
{
  switch ( outformat )
  {
    case OF_AUT:
      gsfprintf(aut,"(%lu,\"%P\",%lu)\n",from,action,to);
      fflush(aut);
      break;
    case OF_SVC:
      if ( outinfo )
      {
        SVCbool b;
        SVCputTransition(svc,
          SVCnewState(svc,(ATerm) nstate->makeStateVector(ATindexedSetGetElem(states,from)),&b),
          SVCnewLabel(svc,(ATerm) ATmakeAppl2(afun_pair,(ATerm) action,(ATerm) term_nil),&b),
          SVCnewState(svc,(ATerm) nstate->makeStateVector(ATindexedSetGetElem(states,to)),&b),
          svcparam);
      } else {
        SVCbool b;
        SVCputTransition(svc,
          SVCnewState(svc,(ATerm) ATmakeInt(from),&b),
          SVCnewLabel(svc,(ATerm) ATmakeAppl2(afun_pair,(ATerm) action,(ATerm) term_nil),&b),
          SVCnewState(svc,(ATerm) ATmakeInt(to),&b),
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
      fprintf(aut,"des (0,%lu,%lu)",trans,num_states);
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
  layout_manager->add(&*labels, margins(0,5,0,5));
  layout_manager->add(progbar, margins(0,5,0,5));

  status_display->set_top_manager(layout_manager);

  tc.send_display_layout(status_display);
}

static void update_status_display(unsigned int level, unsigned int explored, unsigned int seen, unsigned int transitions)
{
  fprintf(stderr,"sending new status...\n");
  char buf[21];
  sprintf(buf,"%u",level);
  lb_level->set_text(buf,&tc);
  sprintf(buf,"%u",explored);
  lb_explored->set_text(buf,&tc);
  sprintf(buf,"%u",seen);
  lb_seen->set_text(buf,&tc);
  sprintf(buf,"%u",transitions);
  lb_transitions->set_text(buf,&tc);
  progbar->set_maximum(seen,&tc);
  progbar->set_value(explored,&tc);
  fprintf(stderr,"done\n");
}
#endif


static bool generate_lts()
{
  ATerm state = get_repr(nstate->getInitialState());
  save_initial_state(state);

  ATbool new_state;
  unsigned long current_state = ATindexedSetPut(states,state,&new_state);
  num_states++;

#ifdef ENABLE_SQUADT_CONNECTIVITY
  if (tc.is_active()) {
    update_status_display(level,current_state,num_states,trans);
  }
#endif

  bool err = false;
  if ( max_states != 0 )
  {
    unsigned long nextlevelat = 1;
    unsigned long prevtrans = 0;
    unsigned long prevcurrent = 0;
    tracecnt = 0;
    gsVerboseMsg("generating state space...\n");

    NextStateGenerator *nsgen = NULL;
    while ( current_state < num_states )
    {
      state = ATindexedSetGetElem(states,current_state);
      bool deadlockstate = true;

      nsgen = nstate->getNextStates(state,nsgen);
      ATermAppl Transition;
      ATerm NewState;
      while ( nsgen->next(&Transition,&NewState) )
      {
        ATbool new_state;
        unsigned long i;

        NewState = get_repr(NewState);
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

        if ( i < num_states )
        {
          deadlockstate = false; // XXX is this ok here, or should it be outside the if?

          check_action_trace(state,Transition,NewState);

          save_transition(current_state,Transition,i);
          trans++;
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
        update_status_display(level,current_state,num_states,trans);
      }
#endif
      if ( gsVerbose && ((current_state%1000) == 0) )
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
#ifdef ENABLE_SQUADT_CONNECTIVITY
        if (tc.is_active()) {
          update_status_display(level,current_state,num_states,trans);
        }
#endif
        if ( gsVerbose )
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
    }
    delete nsgen;
  }
  
  return err;
}


#ifdef ENABLE_SQUADT_CONNECTIVITY
static const unsigned int lpd_file_for_input_no_lts = 0;
static const unsigned int lpd_file_for_input_lts = 1;
static const unsigned int lts_file_for_output = 2;

static const unsigned int option_max_states = 0;
static const unsigned int option_max_traces = 1;
static const unsigned int option_state_format_tree = 2;
static const unsigned int option_rewr_strat = 3;
static const unsigned int option_out_info = 4;
static const unsigned int option_confluence_reduction = 5;
static const unsigned int option_trace = 6;
static const unsigned int option_detect_deadlock = 7;
static const unsigned int option_removeunused = 8;
static const unsigned int option_usedummies = 9;

static bool validate_configuration(sip::configuration &c)
{
  return c.is_complete() && (
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

  checkbox* cb_aut = make_lts?(new checkbox("generate aut file instead of svc", false)):NULL;
/*  checkbox* cb_out_info = new checkbox("save state information in svc file", true);
  checkbox* cb_usedummies = new checkbox("substitute dummy values for free variables", true);
  checkbox* cb_removeunused = new checkbox("remove unused parts of data specification", true);
  checkbox* cb_deadlock = new checkbox("detect deadlocks", false);
  checkbox* cb_trace = new checkbox("save deadlock traces", false);
  checkbox* cb_confluence = new checkbox("apply on-the-fly confluence reduction", true);
  checkbox* cb_state_format_tree = new checkbox("use memory efficient state representation", false);
//  textbox* tb_max_states = new checkbox("4294967296", sip::datatype::standard_integer);
//  textbox* tb_max_traces = new checkbox("10", sip::datatype::standard_integer);
  radio_button* rb_rewr_strat_inner = new radio_button("innermost",NULL,true);
  radio_button* rb_rewr_strat_jitty = new radio_button("JITty",rb_rewr_strat_inner,false);
  radio_button* rb_rewr_strat_innerc = new radio_button("compiling innermost",rb_rewr_strat_inner,false);
  radio_button* rb_rewr_strat_jittyc = new radio_button("compiling JITty",rb_rewr_strat_inner,false);*/

  if ( make_lts )
    column->add(cb_aut, layout::left);
/*  column->add(cb_out_info, layout::left);
  column->add(cb_usedummies, layout::left);
  column->add(cb_removeunused, layout::left);
  column->add(cb_deadlock, layout::left);
  column->add(cb_trace, layout::left);
  column->add(cb_confluence, layout::left);
  column->add(cb_state_format_tree, layout::left);
  column->add(rb_rewr_strat_inner, layout::left);
  column->add(rb_rewr_strat_jitty, layout::left);
  column->add(rb_rewr_strat_innerc, layout::left);
  column->add(rb_rewr_strat_jittyc, layout::left);*/

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

  if ( make_lts )
  {
    std::string input_file_name = c.get_object(lpd_file_for_input_lts)->get_location();
    /* Add output file to the configuration */
    c.add_output(lts_file_for_output, (cb_aut->get_status()?"aut":"svc"), input_file_name + (cb_aut->get_status()?".aut":".svc"));
  }

  c.set_completed(true);

  /* Values for the options */
/*  c.add_option(option_out_info).append_argument(sip::datatype::standard_boolean, cb_out_info->get_status());
  c.add_option(option_usedummies).append_argument(sip::datatype::standard_boolean, cb_usedummies->get_status());
  c.add_option(option_removeunused).append_argument(sip::datatype::standard_boolean, cb_removeunused->get_status());
  c.add_option(option_detect_deadlock).append_argument(sip::datatype::standard_boolean, cb_deadlock->get_status());
  c.add_option(option_trace).append_argument(sip::datatype::standard_boolean, cb_trace->get_status());
  c.add_option(option_confluence_reduction).append_argument(sip::datatype::standard_boolean, cb_confluence->get_status());
  c.add_option(option_state_format_tree).append_argument(sip::datatype::standard_boolean, cb_state_format_tree->get_status());*/
//  c.add_option(option_max_states).append_argument(sip::datatype::standard_integer, tb_confluence->get_text());
//  c.add_option(option_max_traces).append_argument(sip::datatype::standard_integer, tb_confluence->get_text());
//  if ( rb_rewr_strat_inner->is_selected() ) c.add_options(option_rewr_strat).append_argument(sip::datatype::integer, GS_REWR_INNER);
//  if ( rb_rewr_strat_jitty->is_selected() ) c.add_options(option_rewr_strat).append_argument(sip::datatype::integer, GS_REWR_JITTY);
//  if ( rb_rewr_strat_innerc->is_selected() ) c.add_options(option_rewr_strat).append_argument(sip::datatype::integer, GS_REWR_INNERC);
//  if ( rb_rewr_strat_jittyc->is_selected() ) c.add_options(option_rewr_strat).append_argument(sip::datatype::integer, GS_REWR_JITTYC);

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
  unsigned long initial_table_size = 10000;
  #define sopts "hqvfyucrl:da:t::p:R:"
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
    { "action",          required_argument, NULL, 'a' },
    { "action-detect",   required_argument, NULL, 'a' },
    { "trace",           optional_argument, NULL, 't' },
    { "priority",        required_argument, NULL, 'p' },
    { "rewriter",        required_argument, NULL, 'R' },
    { "aut",             no_argument,       NULL, 1   },
    { "svc",             no_argument,       NULL, 2   },
    { "no-info",         no_argument,       NULL, 3   },
    { "init-tsize",      required_argument, NULL, 4   },
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
  gsSetVerboseMsg();

  /* Get tool capabilities in order to modify settings */
  sip::tool::capabilities& cp = tc.get_tool_capabilities();

  /* The tool has only one main input combination it takes an LPE and then behaves as a reporter */
  cp.add_input_combination(lpd_file_for_input_no_lts, "Reporting", "lpe");
  cp.add_input_combination(lpd_file_for_input_lts, "Transformation", "lpe");

  /* On purpose we do not catch exceptions */
  if (tc.activate(argc,argv)) {
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
        tc.set_configuration(configuration);
      } else {
        sip::report report;

        report.set_error("Invalid input combination!");

        tc.send_report(report);
      }
    }
    
    if (!validate_configuration(tc.get_configuration())) {
      /* Configuration is incomplete or incorrect; prompt the user */

      /* Draw a configuration layout in the tool display */
      set_basic_configuration_display(tc,make_lts);
     
      /* Static configuration cycle (phase 2: gather user input) */
      if (!validate_configuration(tc.get_configuration())) {
        /* Wait for configuration data to be send (either a previous configuration, or only an input combination) */
        tc.send_error_report("Fatal error: the configuration is invalid");
     
        exit(1);
      }
    }

    /* Send the controller the signal that we're ready to rumble (no further configuration necessary) */
    tc.send_accept_configuration();

    /* Wait for start message */
    gsVerboseMsg("waiting for start signal...\n");
    tc.await_message(sip::send_signal_start);
    gsVerboseMsg("starting execution...\n");

    sip::configuration c = tc.get_configuration();

    if ( c.object_exists(lts_file_for_output) )
    {
      lts_fn = c.get_object(lts_file_for_output)->get_location();
    }

//    max_states = boost::any_cast <int> (*(c.get_option(option_max_states)->get_value_iterator()));
//    max_traces = boost::any_cast <int> (*(c.get_option(option_max_traces)->get_value_iterator()));
//    strat = boost::any_cast <int> (*(c.get_option(option_rewr_strat)->get_value_iterator()));
/*    stateformat = (boost::any_cast <bool> (*(c.get_option(option_state_format_tree)->get_value_iterator())))?GS_STATE_TREE:GS_STATE_VECTOR;
    outinfo = boost::any_cast <bool> (*(c.get_option(option_out_info)->get_value_iterator()));
    confluence_reduction = boost::any_cast <bool> (*(c.get_option(option_confluence_reduction)->get_value_iterator()));
    trace = boost::any_cast <bool> (*(c.get_option(option_trace)->get_value_iterator()));
    detect_deadlock = boost::any_cast <bool> (*(c.get_option(option_detect_deadlock)->get_value_iterator()));
    removeunused = boost::any_cast <bool> (*(c.get_option(option_removeunused)->get_value_iterator()));
    usedummies = boost::any_cast <bool> (*(c.get_option(option_usedummies)->get_value_iterator()));*/
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
      case 'l':
        if ( (optarg[0] >= '0') && (optarg[0] <= '9') )
        {
          max_states = strtoul(optarg,NULL,0);
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
      case 4:
        if ( (optarg[0] >= '0') && (optarg[0] <= '9') )
        {
          initial_table_size = strtoul(optarg,NULL,0);
        } else {
          gsErrorMsg("invalid argument to --init-tsize\n",optarg);
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
  states = ATindexedSetCreate(initial_table_size,50);
  
  if ( trace )
  {
    backpointers = ATtableCreate(initial_table_size,50);
  } else {
    backpointers = NULL;
  }
  
  nstate = createNextState((ATermAppl) Spec,!usedummies,stateformat,createEnumerator((ATermAppl) Spec,createRewriter(ATAgetArgument((ATermAppl) Spec,3),strat),true),true);
  
  if ( priority_action != NULL )
  {
    gsVerboseMsg("prioritising action '%s'...\n",priority_action);
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
      sip::report report;

      report.set_comment("done with state space generation");

      tc.send_report(report);
    } else {
#endif
    fprintf(stderr,
      "done with state space generation (%lu level%s, %lu state%s and %lu transition%s).\n",
      level-1,
      (level==2)?"":"s",
      num_states,
      (num_states==1)?"":"s",
      trans,
      (trans==1)?"":"s"
    );
#ifdef ENABLE_SQUADT_CONNECTIVITY
    }
#endif
  }
}
