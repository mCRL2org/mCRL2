// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file exploration.cpp

#include "mcrl2/aterm/aterm2.h"
#include <time.h>
#include <sstream>
#include <set>
#include "mcrl2/utilities/logger.h"
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/core/detail/pp_deprecated.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/selection.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/lps/find.h"
// #include "mcrl2/lps/nextstate.h"
#include "mcrl2/lps/detail/instantiate_global_variables.h"
#include "mcrl2/lps/multi_action.h"
#include "mcrl2/trace/trace.h"
#include "mcrl2/lts/detail/exploration.h"
#include "mcrl2/lts/detail/lps2lts_lts.h"


using namespace std;
using namespace mcrl2::log;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::trace;

namespace mcrl2
{
namespace lts
{

exploration_strategy str_to_expl_strat(const string s)
{
  if (s=="b" || s=="breadth")
  {
    return es_breadth;
  }
  if (s=="d" || s=="depth")
  {
    return es_depth;
  }
  if (s=="r" || s=="random")
  {
    return es_random;
  }
  if (s=="p" || s=="prioritized")
  {
    return es_value_prioritized;
  }
  if (s=="q" || s=="rprioritized")
  {
    return es_value_random_prioritized;
  }
  return es_none;
}

const string expl_strat_to_str(exploration_strategy es)
{
  switch (es)
  {
    case es_breadth:
      return "breadth";
    case es_depth:
      return "depth";
    case es_random:
      return "random";
    case es_value_prioritized:
      return "prioritized";
    case es_value_random_prioritized:
      return "rprioritized";
    default:
      return "unknown";
  }
}

bool lps2lts_algorithm::initialise_lts_generation(lts_generation_options* opts)
{
  using namespace mcrl2;

  if (initialised)
  {
    throw mcrl2::runtime_error("lps2lts algorithm class may be instantiated only once.");
  }

  srand((unsigned)time(NULL)+getpid());

  lgopts = opts;

  lg_error = false;

  lps::detail::instantiate_global_variables(lgopts->specification);

  if (lgopts->bithashing)
  {
    bithash_table = bit_hash_table(lgopts->bithashsize);
  }
  else
  {
    states = atermpp::indexed_set(lgopts->initial_table_size,50);
  }

  assert(backpointers.empty());

  if (lgopts->removeunused)
  {
    mCRL2log(verbose) << "removing unused parts of the data specification." << std::endl;

    std::set<data::function_symbol> used_function_symbols=
                                  lps::find_function_symbols(lgopts->specification);

    // In the cases below the function > is used when generating the state space,
    // so it must be added separately, before removing the unused symbols. Otherwise,
    // the rewrite rules for > will not be removed.
    if ((lgopts->expl_strat == es_value_random_prioritized) ||
        (lgopts->expl_strat == es_value_prioritized))
    {
      used_function_symbols.insert(mcrl2::data::greater(mcrl2::data::sort_nat::nat()));
    }

    lgopts->m_rewriter.reset(
      new mcrl2::data::rewriter(lgopts->specification.data(),
                                mcrl2::data::used_data_equation_selector(
                                  lgopts->specification.data(),
                                  used_function_symbols,
                                  lgopts->specification.global_variables()
                                  ),
                                lgopts->strat
                               )
    );
#ifdef MCRL2_REWRITE_RULE_SELECTION_DEBUG
std::clog << "--- rewrite rule selection specification ---\n";
std::clog << lps::pp(lgopts->specification) << std::endl;
std::clog << "--- rewrite rule selection function symbols ---\n";
std::clog << core::detail::print_pp_set(lps::find_function_symbols(lgopts->specification), data::stream_printer()) << std::endl;
#endif
  }
  else
  {
    lgopts->m_rewriter.reset(new mcrl2::data::rewriter(lgopts->specification.data(), lgopts->strat));
  }

  nstate = createNextState(lgopts->specification,*(lgopts->m_rewriter),!lgopts->usedummies,lgopts->stateformat);

  if (lgopts->priority_action != "")
  {
    mCRL2log(verbose) << "applying confluence reduction with tau action '" << lgopts->priority_action << "'..." << std::endl;
    nstate->prioritise(lgopts->priority_action.c_str());
    initialise_representation(true);
  }
  else
  {
    initialise_representation(false);
  }

  if (lgopts->detect_deadlock)
  {
    mCRL2log(verbose) << "Detect deadlocks.\n" ;
  }

  if (lgopts->detect_divergence)
  {
    mCRL2log(verbose) << "Detect divergences with tau action is `" << lgopts->priority_action << "'.\n";
  }

  num_states = 0;
  trans = 0;
  level = 1;

  if (lgopts->lts != "")
  {
    lps2lts_lts_options lts_opts;
    lts_opts.outformat = lgopts->outformat;
    lts_opts.outinfo = lgopts->outinfo;
    lts_opts.nstate = nstate;
    lts_opts.spec.reset(new mcrl2::lps::specification(lgopts->specification));
    lts.open_lts(lgopts->lts.c_str(),lts_opts);
  }
  else
  {
    lgopts->outformat = mcrl2::lts::lts_none;
    mCRL2log(verbose) << "not saving state space." << std::endl;
  }

  initialised = true;
  return true;
}

bool lps2lts_algorithm::finalise_lts_generation()
{
  if (lg_error)
  {
    lts.remove_lts();
  }
  else
  {
    lts.close_lts(num_states,trans);
  }

  if (!lg_error)
  {
    if (lgopts->expl_strat == es_random)
    {
      mCRL2log(verbose) << "done with random walk of "
                        << trans << " transition" << ((trans==1)?"":"s")
                        << " (visited " << num_states
                        << " unique state" << ((num_states == 1)?"":"s") << ")" << std::endl;
    }
    else if (lgopts->expl_strat == es_value_prioritized)
    {
      mCRL2log(verbose) << "done with value prioritized walk of "
                        << trans << " transition" << ((trans==1)?"":"s")
                        << " (visited " << num_states
                        << " unique state" << ((num_states == 1)?"":"s") << ")" << std::endl;
    }
    else if (lgopts->expl_strat == es_value_random_prioritized)
    {
      mCRL2log(verbose) << "done with random value prioritized walk of "
                        << trans << " transition" << ((trans==1)?"":"s")
                        << " (visited " << num_states
                        << " unique state" << ((num_states == 1)?"":"s") << ")" << std::endl;
    }
    else if (lgopts->expl_strat == es_breadth)
    {
      mCRL2log(verbose) << "done with state space generation ("
                        << level-1 << " level" << ((level==2)?"":"s") << ", "
                        << num_states << " state" << ((num_states == 1)?"":"s")
                        << " and " << trans << " transition" << ((trans==1)?"":"s") << ")" << std::endl;
    }
    else if (lgopts->expl_strat == es_depth)
    {
      mCRL2log(verbose) << "done with state space generation ("
                        << num_states << " state" << ((num_states == 1)?"":"s")
                        << " and " << trans << " transition" << ((trans==1)?"":"s") << ")" << std::endl;
    }
  }

  states = atermpp::indexed_set(0,0);
  delete nstate;
  backpointers.clear();

  cleanup_representation();

  finalised = true;
  return true;
}

////////////////////////////////////////////////////////////////////////////////
//                              Trace functions                               //
////////////////////////////////////////////////////////////////////////////////

bool lps2lts_algorithm::occurs_in(atermpp::aterm_appl const& name, atermpp::term_list< atermpp::aterm_appl > const& ma)
{
  for (atermpp::term_list< atermpp::aterm_appl >::const_iterator i = ma.begin(); i != ma.end(); ++i)
  {
    if (name == atermpp::aterm_appl((*i)(0))(0))
    {
      return true;
    }
  }
  return false;
}

bool lps2lts_algorithm::savetrace(
  std::string const& info,
  const state_t state,
  NextState* nstate,
  const state_t extra_state,
  ATermAppl extra_transition)
{
  atermpp::aterm s = state;
  atermpp::map<atermpp::aterm,atermpp::aterm>::iterator ns;
  ATermList tr = ATmakeList0();
  NextStateGenerator* nsgen = NULL;

  if (extra_state != NULL)
  {
    tr = ATinsert(tr,(ATerm) ATmakeList2((ATerm) extra_transition,extra_state));
  }

  while ((ns = backpointers.find(s)) != backpointers.end())
  {
    ATermAppl trans;
    ATerm t;
    bool priority;
    nsgen = nstate->getNextStates(ns->second,nsgen);
    try
    {
      while (nsgen->next(&trans,&t,&priority))
      {
        if (!priority && ATisEqual(s,get_repr(t)))
        {
          break;
        }
      }
    }
    catch (mcrl2::runtime_error e)
    {
      delete nsgen;
      throw e;
    }
    tr = ATinsert(tr, (ATerm) ATmakeList2((ATerm) trans,s));
    s = ns->second;
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

  delete nsgen;

  try
  {
    trace.save(lgopts->generate_filename_for_trace(lgopts->trace_prefix, info, "trc"));
  }
  catch (...)
  {
    return false;
  }

  return true;
}

void lps2lts_algorithm::check_actiontrace(const state_t OldState, ATermAppl Transition, const state_t NewState)
{
  for (atermpp::set < mcrl2::core::identifier_string >::const_iterator j=lgopts->trace_actions.begin();
       j!=lgopts->trace_actions.end(); j++)
  {
    if (occurs_in(*j,atermpp::list_arg1(Transition)))
    {
      if (lgopts->trace && (tracecnt < lgopts->max_traces))
      {
        if (lgopts->trace_prefix.empty())
        {
        }
        std::ostringstream ss;
        //See line below. Bleh. Why cant I get j->string(), or something like that (JFG).
        ss << "act_" << tracecnt << "_" << ATgetName(ATgetAFun(ATermAppl(atermpp::aterm_appl(*j))));
        string sss(ss.str());
        bool saved_ok = savetrace(sss,OldState,nstate,NewState,Transition);

        if (lgopts->detect_action || mCRL2logEnabled(verbose))
        {
          if (saved_ok)
          {
            mCRL2log(info) << "detect: action '"
                           << lps::pp(mcrl2::lps::multi_action(Transition))
                           << "' found and saved to '"
                           << lgopts->trace_prefix << "_act_" << tracecnt << "_" << lps::pp(mcrl2::lps::multi_action(*j)) << ".trc'"
                           << "(state index: " << states.index(OldState) << ")." << std::endl;
          }
          else
          {
            mCRL2log(info) << "detect: action '"
                           << lps::pp(mcrl2::lps::multi_action(Transition))
                           << "' found, but could not be saved to '"
                           << lgopts->trace_prefix << "_act_" << tracecnt << "_" << lps::pp(mcrl2::lps::multi_action(*j)) << ".trc'"
                           << "(state index: " << states.index(OldState) << ")." << std::endl;
          }
        }
        tracecnt++;
      }
      else
      {
        mCRL2log(info) << "detect: action '"
                       << lps::pp(mcrl2::lps::multi_action(Transition))
                       << "' found (state index: " << states.index(OldState) << ")." << std::endl;
      }
    }
  }
}


void lps2lts_algorithm::save_error_trace(const state_t state)
{
  if (lgopts->save_error_trace)
  {
    bool saved_ok = savetrace("error",state,nstate);

    if (saved_ok)
    {
      mCRL2log(verbose) << "saved trace to error in '" << lgopts->trace_prefix << "_error.trc'.\n";
    }
    else
    {
      mCRL2log(verbose) << "trace to error could not be saved in '" << lgopts->trace_prefix << "_error.trc'.\n";
    }
  }
}


void lps2lts_algorithm::check_deadlocktrace(const state_t state)
{
  if (lgopts->detect_deadlock)
  {
    if (lgopts->trace && (tracecnt < lgopts->max_traces))
    {
      std::ostringstream ss;
      ss << "dlk_" << tracecnt;
      string sss(ss.str());
      bool saved_ok = savetrace(sss,state,nstate);

      if (lgopts->detect_deadlock || mCRL2logEnabled(verbose))
      {
        if (saved_ok)
        {
          cerr << "deadlock-detect: deadlock found and saved to '" << lgopts->trace_prefix << "_dlk_" << tracecnt << ".trc' (state index: " <<
               states.index(state) << ").\n";
        }
        else
        {
          cerr << "deadlock-detect: deadlock found, but could not be saved to '" << lgopts->trace_prefix << "_dlk_" << tracecnt <<
               ".trc' (state index: " << states.index(state) <<  ").\n";
        }
      }
      tracecnt++;
    }
    else
    {
      cerr << "deadlock-detect: deadlock found (state index: " << states.index(state) <<  ").\n";
    }
  }
}



////////////////////////////////////////////////////////////////////////////////
//                         Main exploration functions                         //
////////////////////////////////////////////////////////////////////////////////

// Confluence reduction based on S.C.C. Blom, Partial tau-confluence for
// Efficient State Space Generation, Technical Report SEN-R0123, CWI,
// Amsterdam, 2001

void lps2lts_algorithm::initialise_representation(bool confluence_reduction)
{
  apply_confluence_reduction = confluence_reduction;
  if (confluence_reduction)
  {
    repr_nsgen = NULL;
  }
  else if (lgopts->detect_divergence)
  {
    lgopts->priority_action = "tau";
  }
}

void lps2lts_algorithm::cleanup_representation()
{
  if (apply_confluence_reduction)
  {
    delete repr_nsgen;
  }
}

bool lps2lts_algorithm::search_divergence_recursively(
  const state_t current_state,
  set < state_t > &on_current_depth_first_path,
  atermpp::set< state_t > &repr_visited)
{
  on_current_depth_first_path.insert(current_state);
  vector < state_t > new_states;
  repr_nsgen = nstate->getNextStates(current_state,repr_nsgen);
  ATermAppl Transition;
  ATerm NewState;
  while (repr_nsgen->next(&Transition,&NewState))
  {
    if ((ATermList)ATgetArgument(Transition,0)==ATempty) // This is a tau transition.
    {
      if (repr_visited.insert(NewState).second) // We did not encounter NewState already.
      {
        new_states.push_back(NewState);
      }
      else
      {
        if (on_current_depth_first_path.find(NewState)!=on_current_depth_first_path.end())
        {
          // divergence found
          return true;
        }
      }
    }
  }

  for (vector <state_t>::const_iterator i=new_states.begin();
       i!=new_states.end(); ++i)
  {
    if (search_divergence_recursively(*i,on_current_depth_first_path,repr_visited))
    {
      return true;
    }
  }
  on_current_depth_first_path.erase(current_state);
  return false;
}

void lps2lts_algorithm::check_divergence(const state_t state)
{
  if (lgopts->detect_divergence)
  {
    atermpp::set < state_t >repr_visited;
    set < state_t > on_current_depth_first_path;
    repr_visited.insert(state);

    if (search_divergence_recursively(state,on_current_depth_first_path,repr_visited))
    {
      if (lgopts->trace && (tracecnt < lgopts->max_traces))
      {
        std::ostringstream ss;
        ss << "divergence_" << tracecnt;
        string sss(ss.str());
        bool saved_ok = savetrace(sss,state,nstate);

        if (lgopts->detect_divergence || mCRL2logEnabled(verbose))
        {
          if (saved_ok)
          {
            cerr << "divergence-detect: divergence found and saved to '" << lgopts->trace_prefix << "_dlk_" << tracecnt <<
                 ".trc' (state index: " << states.index(state) <<  ").\n";
          }
          else
          {
            cerr << "divergence-detect: divergence found, but could not be saved to '" << lgopts->trace_prefix << "_dlk_" << tracecnt <<
                 ".trc' (state index: " << states.index(state) <<  ").\n";
          }
        }
        tracecnt++;
      }
      else
      {
        cerr << "divergence-detect: divergence found (state index: " << states.index(state) <<  ").\n";
      }
    }
  }
}

lps2lts_algorithm::state_t lps2lts_algorithm::get_repr(const state_t state)
{
  if (!apply_confluence_reduction)
  {
    return state;
  }

  state_t v = state;
  atermpp::map<atermpp::aterm,size_t> repr_number;
  atermpp::map<atermpp::aterm,size_t> repr_low;
  atermpp::map<atermpp::aterm,atermpp::aterm_list> repr_next;
  atermpp::map<atermpp::aterm,atermpp::aterm> repr_back;
  size_t count;

  repr_number[v]=0;
  count = 0;
  bool notdone = true;
  while (notdone)
  {
    assert(repr_number.count(v)>0);
    if (repr_number[v] == 0)
    {
      count++;
      repr_number[v]=count;
      repr_low[v]=count;
      ATermList nextl = ATmakeList0();
      repr_nsgen = nstate->getNextStates(v,repr_nsgen);
      ATermAppl Transition;
      ATerm NewState;
      bool prioritised_action;
      while (repr_nsgen->next(&Transition,&NewState,&prioritised_action) && prioritised_action)
      {
        nextl = ATinsert(nextl,NewState);
        if (repr_number.count(NewState) == 0)   // This condition was missing in the report
        {
          repr_number[NewState]=0;
        }
      }
      if (!notdone)
      {
        break;
      }
      repr_next[v]=nextl;
    }
    ATermList nextl = repr_next[v];
    if (ATisEmpty(nextl))
    {
      assert(repr_number.count(v)>0);
      assert(repr_low.count(v)>0);
      if (repr_number[v]==repr_low[v])
      {
        break;
      }
      assert(repr_back.count(v)>0);
      ATerm backv = repr_back[v];
      const size_t a = repr_low[backv];
      const size_t b = repr_low[v];
      if (a < b)
      {
        repr_low[backv]=a;
      }
      else
      {
        repr_low[backv]=b;
      }
      v = backv;
    }
    else
    {
      ATerm u = ATgetFirst(nextl);
      repr_next[v]=ATgetNext(nextl);
      const size_t nu = repr_number[u];
      if (nu == 0)
      {
        repr_back[u]=v;
        v = u;
      }
      else
      {
        if (nu < repr_number[v])
        {
          const size_t lv = repr_low[v];
          if (nu < lv)
          {
            repr_low[v]=nu;
          }
        }
      }
    }
  }

  return v;
}

size_t lps2lts_algorithm::add_state(const state_t state, bool& is_new)
{
  if (lgopts->bithashing)
  {
    size_t i = bithash_table.add_state(state, is_new);
    return i;
  }
  else
  {
    std::pair<size_t, bool> result = states.put(state);
    is_new = result.second;
    return result.first;
  }
}

size_t lps2lts_algorithm::state_index(const state_t state)
{
  if (lgopts->bithashing)
  {
    return bithash_table.state_index(state);
  }
  else
  {
    return states.index(state);
  }
}

bool lps2lts_algorithm::add_transition(const state_t from, ATermAppl action, const state_t to)
{
  bool new_state;
  size_t i;

  i = add_state(to, new_state);

  if (new_state)
  {
    num_states++;
    if (lgopts->trace || lgopts->save_error_trace)
    {
      assert(backpointers.count(to)==0);
      backpointers[to]=from;
    }
  }
  else
  {
    num_found_same++;
  }

  check_actiontrace(from,action,to);

  lts.save_transition(state_index(from),from,action,i,to);
  trans++;

  return new_state;
}

bool lps2lts_algorithm::generate_lts()
{
  state_t state = get_repr(nstate->getInitialState());

  bool new_state;
  initial_state = add_state(state,new_state);
  lts.save_initial_state(initial_state,state);
  current_state = 0;
  ++num_states;

  if (lgopts->max_states != 0)
  {
    size_t endoflevelat = 1;
    size_t prevtrans = 0;
    size_t prevcurrent = 0;
    num_found_same = 0;
    tracecnt = 0;
    mCRL2log(verbose) << "generating state space with '" <<
                    expl_strat_to_str(lgopts->expl_strat) << "' strategy...\n";

    if (lgopts->expl_strat == es_random)
    {
      NextStateGenerator *nsgen = NULL;

      while (!must_abort && (current_state < lgopts->max_states) && (!lgopts->trace || (tracecnt < lgopts->max_traces)))
      {
        state_t NewState;
        size_t number_of_outgoing_transitions=0;

        check_divergence(state);

        try
        {
          nsgen = nstate->getNextStates(state,nsgen);
          bool priority;
          ATermAppl tempTransition;
          ATerm tempNewState;
          while (!must_abort && nsgen->next(&tempTransition,&tempNewState,&priority))
          {
            if (!priority)   // don't store confluent self loops
            {
              number_of_outgoing_transitions++;
              // Select this new state with probability 1/number_of_outgoing_transitions.
              // This guarantees that one out of all outgoing transitions is chosen
              // with equal probability.

              tempNewState=get_repr(tempNewState);
              add_transition(state,tempTransition,tempNewState);
              if (rand()%number_of_outgoing_transitions==0)
              {
                NewState = tempNewState;
              }
            }
          }
        }
        catch (mcrl2::runtime_error& e)
        {
          cerr << "Error while exploring state space: " << e.what() << "\n";
          lg_error = true;
          save_error_trace(state);
          exit(EXIT_FAILURE);
        }

        if (number_of_outgoing_transitions > 0)
        {
          state = NewState;
        }
        else
        {
          check_deadlocktrace(state);
          break;
        }

        current_state++;
        if (!lgopts->suppress_progress_messages && mCRL2logEnabled(verbose) && ((current_state%1000) == 0))
        {
          mCRL2log(verbose) << "monitor: currently explored "
                            << trans << " transition" << ((trans==1)?"":"s")
                            << " and encountered " << num_states << " unique state" << ((num_states==1)?"":"s") << std::endl;
        }
      }
      delete nsgen;
    }
    else if (lgopts->expl_strat == es_value_prioritized)
    {
      mcrl2::data::rewriter& rewriter=nstate->getRewriter();
      NextStateGenerator* nsgen = NULL;
      while (!must_abort && (current_state < lgopts->max_states) && (current_state < num_states) &&
                             (!lgopts->trace || (tracecnt < lgopts->max_traces)))
      {
        ATermList tmp_trans = ATmakeList0();
        ATermList tmp_states = ATmakeList0();
        ATermAppl Transition;
        ATerm NewState;
        state = states.get(current_state);
        check_divergence(state);
        try
        {
          nsgen = nstate->getNextStates(state,nsgen);
          bool priority;
          while (!must_abort && nsgen->next(&Transition,&NewState,&priority))
          {
            NewState = get_repr(NewState);
            if (!priority)   // don't store confluent self loops
            {
              tmp_trans = ATinsert(tmp_trans,(ATerm) Transition);
              tmp_states = ATinsert(tmp_states,NewState);
            }
          }

          // Filter the transitions by only taking the actions that either have no
          // positive number as first parameter, or that has the lowest positive number.
          // This can be non-deterministic, as there can be more actions with this low number
          // as first parameter.
          //
          // First find the lowest index.

          ATermAppl lowest_first_action_parameter=NULL;

          for (ATermList tmp_trans_walker=tmp_trans; !ATisEmpty(tmp_trans_walker);
               tmp_trans_walker=ATgetNext(tmp_trans_walker))
          {
            ATermList multi_action_list=(ATermList)ATgetArgument(ATgetFirst(tmp_trans_walker),0);
            if (ATgetLength(multi_action_list)==1)
            {
              ATermAppl first_action=(ATermAppl)ATgetFirst(multi_action_list);
              ATermList action_arguments=(ATermList)ATgetArgument(first_action,1);
              ATermList action_sorts=(ATermList)ATgetArgument(ATgetArgument(first_action,0),1);
              if (ATgetLength(action_arguments)>0)
              {
                ATermAppl first_argument=(ATermAppl)ATgetFirst(action_arguments);
                ATermAppl first_sort=(ATermAppl)ATgetFirst(action_sorts);
                if (mcrl2::data::sort_nat::is_nat(mcrl2::data::sort_expression(first_sort)))
                {
                  if (lowest_first_action_parameter==NULL)
                  {
                    lowest_first_action_parameter=first_argument;
                  }
                  else
                  {
                    using namespace mcrl2::data;
                    ATermAppl result=rewriter(greater(
                                                data_expression(lowest_first_action_parameter),
                                                data_expression(first_argument)));
                    if (sort_bool::is_true_function_symbol(data_expression(result)))
                    {
                      lowest_first_action_parameter=first_argument;
                    }
                    else if (!sort_bool::is_false_function_symbol(data_expression(result)))
                    {
                      throw mcrl2::runtime_error("Fail to rewrite term " + pp(data_expression(result)) +
                                                 " to true or false.");
                    }
                  }
                }
              }
            }
          }

          // Now carry out the actual filtering;
          ATermList new_tmp_trans = ATmakeList0();
          ATermList new_tmp_states = ATmakeList0();
          ATermList tmp_state_walker = tmp_states;
          for (ATermList tmp_trans_walker=tmp_trans; !ATisEmpty(tmp_trans_walker);
               tmp_trans_walker=ATgetNext(tmp_trans_walker))
          {
            ATermAppl multi_action=(ATermAppl)ATgetFirst(tmp_trans_walker);
            ATermAppl state=(ATermAppl)ATgetFirst(tmp_state_walker);
            tmp_state_walker=ATgetNext(tmp_state_walker);
            ATermList multi_action_list=(ATermList)ATgetArgument(ATgetFirst(tmp_trans_walker),0);
            if (ATgetLength(multi_action_list)==1)
            {
              ATermAppl first_action=(ATermAppl)ATgetFirst(multi_action_list);
              ATermList action_arguments=(ATermList)ATgetArgument(first_action,1);
              ATermList action_sorts=(ATermList)ATgetArgument(ATgetArgument(first_action,0),1);
              if (ATgetLength(action_arguments)>0)
              {
                ATermAppl first_argument=(ATermAppl)ATgetFirst(action_arguments);
                ATermAppl first_sort=(ATermAppl)ATgetFirst(action_sorts);
                if (mcrl2::data::sort_nat::is_nat(mcrl2::data::sort_expression(first_sort)))
                {
                  ATermAppl result=rewriter(mcrl2::data::equal_to(mcrl2::data::data_expression(lowest_first_action_parameter),mcrl2::data::data_expression(first_argument)));
                  if (mcrl2::data::sort_bool::is_true_function_symbol(mcrl2::data::data_expression(result)))
                  {
                    new_tmp_trans=ATinsert(new_tmp_trans,(ATerm)multi_action);
                    new_tmp_states=ATinsert(new_tmp_states,(ATerm)state);
                  }
                  else
                  {
                    assert(mcrl2::data::sort_bool::is_false_function_symbol(mcrl2::data::data_expression(result)));
                    // The transition is omitted!
                  }
                }
                else
                {
                  new_tmp_trans=ATinsert(new_tmp_trans,(ATerm)multi_action);
                  new_tmp_states=ATinsert(new_tmp_states,(ATerm)state);
                }
              }
              else
              {
                new_tmp_trans=ATinsert(new_tmp_trans,(ATerm)multi_action);
                new_tmp_states=ATinsert(new_tmp_states,(ATerm)state);
              }
            }
            else
            {
              new_tmp_trans=ATinsert(new_tmp_trans,(ATerm)multi_action);
              new_tmp_states=ATinsert(new_tmp_states,(ATerm)state);
            }
          }
          tmp_trans=ATreverse(new_tmp_trans);
          tmp_states=ATreverse(new_tmp_states);
        }
        catch (mcrl2::runtime_error& e)
        {
          cerr << "Error while exploring state space: " << e.what() << "\n";
          lg_error = true;
          save_error_trace(state);
          exit(EXIT_FAILURE);
        }

        size_t len = ATgetLength(tmp_trans);
        if (len > 0)
        {
          for (size_t i=0; i<len; i++)
          {
            if (num_states-current_state <= lgopts->todo_max)
            {
              add_transition(state,(ATermAppl) ATgetFirst(tmp_trans),ATgetFirst(tmp_states));
            }
            else if (rand()%2==0)  // with 50 % probability
            {
              current_state++;    // ignore the current state
              add_transition(state,(ATermAppl) ATgetFirst(tmp_trans),ATgetFirst(tmp_states));
            }
            else
            {
              // Ignore the new state.
            }

            tmp_trans = ATgetNext(tmp_trans);
            tmp_states = ATgetNext(tmp_states);
          }
        }
        else
        {
          check_deadlocktrace(state);
          break;
        }

        current_state++;
        if (!lgopts->suppress_progress_messages && mCRL2logEnabled(verbose) && ((current_state%1000) == 0))
        {
          mCRL2log(verbose) << "monitor: currently explored "
                                      << trans << " transition" << ((trans==1)?"":"s")
                                      << " and encountered " << num_states << " unique state" << ((num_states==1)?"":"s")
                                      << " [MAX " << lgopts->todo_max << "]." << std::endl;
        }
      }
      delete nsgen;
    }
    else if (lgopts->expl_strat == es_value_random_prioritized)
    {
      mcrl2::data::rewriter& rewriter=nstate->getRewriter();
      NextStateGenerator* nsgen = NULL;
      while (!must_abort && (current_state < lgopts->max_states) && (!lgopts->trace || (tracecnt < lgopts->max_traces)))
      {
        ATermList tmp_trans = ATmakeList0();
        ATermList tmp_states = ATmakeList0();
        ATermAppl Transition;
        ATerm NewState;

        check_divergence(state);

        try
        {
          // state = ATindexedSetGetElem(states,current_state);
          nsgen = nstate->getNextStates(state,nsgen);
          bool priority;
          while (!must_abort && nsgen->next(&Transition,&NewState,&priority))
          {
            NewState = get_repr(NewState);
            if (!priority)   // don't store confluent self loops
            {
              tmp_trans = ATinsert(tmp_trans,(ATerm) Transition);
              tmp_states = ATinsert(tmp_states,NewState);
            }
          }

          // Filter the transitions by only taking the actions that either have no
          // positive number as first parameter, or that has the lowest positive number.
          // This can be non-deterministic, as there can be more actions with this low number
          // as first parameter.
          //
          // First find the lowest index.

          ATermAppl lowest_first_action_parameter=NULL;

          for (ATermList tmp_trans_walker=tmp_trans; !ATisEmpty(tmp_trans_walker);
               tmp_trans_walker=ATgetNext(tmp_trans_walker))
          {
            ATermList multi_action_list=(ATermList)ATgetArgument(ATgetFirst(tmp_trans_walker),0);
            if (ATgetLength(multi_action_list)==1)
            {
              ATermAppl first_action=(ATermAppl)ATgetFirst(multi_action_list);
              ATermList action_arguments=(ATermList)ATgetArgument(first_action,1);
              ATermList action_sorts=(ATermList)ATgetArgument(ATgetArgument(first_action,0),1);
              if (ATgetLength(action_arguments)>0)
              {
                ATermAppl first_argument=(ATermAppl)ATgetFirst(action_arguments);
                ATermAppl first_sort=(ATermAppl)ATgetFirst(action_sorts);
                if (mcrl2::data::sort_nat::is_nat(mcrl2::data::sort_expression(first_sort)))
                {
                  if (lowest_first_action_parameter==NULL)
                  {
                    lowest_first_action_parameter=first_argument;
                  }
                  else
                  {
                    using namespace mcrl2::data;
                    ATermAppl result=rewriter(greater(
                                                data_expression(lowest_first_action_parameter),
                                                data_expression(first_argument)));
                    if (sort_bool::is_true_function_symbol(data_expression(result)))
                    {
                      lowest_first_action_parameter=first_argument;
                    }
                    else if (!sort_bool::is_false_function_symbol(data_expression(result)))
                    {
                      throw mcrl2::runtime_error("Fail to rewrite term " + pp(data_expression(result)) +
                                                 " to true or false.");

                    }
                  }
                }
              }
            }
          }

          // Now carry out the actual filtering;
          ATermList new_tmp_trans = ATmakeList0();
          ATermList new_tmp_states = ATmakeList0();
          ATermList tmp_state_walker = tmp_states;
          for (ATermList tmp_trans_walker=tmp_trans; !ATisEmpty(tmp_trans_walker);
               tmp_trans_walker=ATgetNext(tmp_trans_walker))
          {
            ATermAppl multi_action=(ATermAppl)ATgetFirst(tmp_trans_walker);
            ATermAppl state=(ATermAppl)ATgetFirst(tmp_state_walker);
            tmp_state_walker=ATgetNext(tmp_state_walker);
            ATermList multi_action_list=(ATermList)ATgetArgument(ATgetFirst(tmp_trans_walker),0);
            if (ATgetLength(multi_action_list)==1)
            {
              ATermAppl first_action=(ATermAppl)ATgetFirst(multi_action_list);
              ATermList action_arguments=(ATermList)ATgetArgument(first_action,1);
              ATermList action_sorts=(ATermList)ATgetArgument(ATgetArgument(first_action,0),1);
              if (ATgetLength(action_arguments)>0)
              {
                ATermAppl first_argument=(ATermAppl)ATgetFirst(action_arguments);
                ATermAppl first_sort=(ATermAppl)ATgetFirst(action_sorts);
                if (mcrl2::data::sort_nat::is_nat(mcrl2::data::sort_expression(first_sort)))
                {
                  ATermAppl result=rewriter(mcrl2::data::equal_to(mcrl2::data::data_expression(lowest_first_action_parameter),mcrl2::data::data_expression(first_argument)));
                  if (mcrl2::data::sort_bool::is_true_function_symbol(mcrl2::data::data_expression(result)))
                  {
                    new_tmp_trans=ATinsert(new_tmp_trans,(ATerm)multi_action);
                    new_tmp_states=ATinsert(new_tmp_states,(ATerm)state);
                  }
                  else
                  {
                    assert(mcrl2::data::sort_bool::is_false_function_symbol(mcrl2::data::data_expression(result)));
                  }
                }
                else
                {
                  new_tmp_trans=ATinsert(new_tmp_trans,(ATerm)multi_action);
                  new_tmp_states=ATinsert(new_tmp_states,(ATerm)state);
                }
              }
              else
              {
                new_tmp_trans=ATinsert(new_tmp_trans,(ATerm)multi_action);
                new_tmp_states=ATinsert(new_tmp_states,(ATerm)state);
              }
            }
            else
            {
              new_tmp_trans=ATinsert(new_tmp_trans,(ATerm)multi_action);
              new_tmp_states=ATinsert(new_tmp_states,(ATerm)state);
            }
          }

          // Randomly select one element from the list for experiments.
          if (ATgetLength(new_tmp_trans)>0)
          {
            size_t r = rand()%ATgetLength(new_tmp_trans);
            tmp_trans=ATgetSlice(new_tmp_trans,r,r+1);
            tmp_states=ATgetSlice(new_tmp_states,r,r+1);
          }
          else
          {
            tmp_trans=ATempty;
            tmp_states=ATempty;
          }
        }
        catch (mcrl2::runtime_error& e)
        {
          cerr << "Error while exploring state space: " << e.what() << "\n";
          lg_error = true;
          save_error_trace(state);
          exit(EXIT_FAILURE);
        }

        size_t len = ATgetLength(tmp_trans);
        if (len > 0)
        {
          state_t new_state;
          for (size_t i=0; i<len; i++)
          {
            add_transition(state,(ATermAppl) ATgetFirst(tmp_trans),ATgetFirst(tmp_states));
            new_state = ATgetFirst(tmp_states);

            tmp_trans = ATgetNext(tmp_trans);
            tmp_states = ATgetNext(tmp_states);
          }
          state = new_state;
        }
        else
        {
          check_deadlocktrace(state);
          break;
        }

        current_state++;
        if (!lgopts->suppress_progress_messages && mCRL2logEnabled(verbose) && ((current_state%1000) == 0))
        {
          mCRL2log(verbose) << "monitor: currently explored "
                                      << trans << " transition" << ((trans==1)?"":"s")
                                      << " and encountered " << num_states << " unique state" << ((num_states==1)?"":"s") << std::endl;
        }
      }
      delete nsgen;
    }
    else if (lgopts->expl_strat == es_breadth)
    {
      queue state_queue;

      NextStateGenerator* nsgen = NULL;
      size_t limit = lgopts->max_states;
      if (lgopts->bithashing)
      {
        lgopts->max_states = ULONG_MAX;
        state_queue.set_max_size(((limit-1)>lgopts->todo_max)?lgopts->todo_max:limit-1);
        state_queue.add_to_queue(state);
        state_queue.swap_queues();
      }
      // E is the set of explored states
      // S is the set of "seen" states
      //
      // normal:     S = [0..num_states), E = [0..current_state)
      // bithashing: S = { h | get_bithash(h) }, E = S \ "items left in queues"
      //
      // both:       |E| <= limit
      while (!must_abort && (current_state < endoflevelat) && (!lgopts->trace || (tracecnt < lgopts->max_traces)))
      {
        if (lgopts->bithashing)
        {
          state = state_queue.get_from_queue();
          assert(state != NULL);
        }
        else
        {
          state = states.get(current_state);
        }

        check_divergence(state);

        bool deadlockstate = true;

        try
        {
          nsgen = nstate->getNextStates(state,nsgen);
          ATermAppl Transition;
          ATerm NewState;
          bool priority;
          while (!must_abort && nsgen->next(&Transition,&NewState,&priority))
          {
            NewState = get_repr(NewState);
            if (!priority)   // don't store confluent self loops
            {
              deadlockstate = false;
              bool b = add_transition(state,Transition,NewState);
              if (lgopts->bithashing && b)
              {
                const state_t removed_state = state_queue.add_to_queue(NewState);
                if (removed_state != NULL)
                {
                  bithash_table.remove_state_from_bithash(removed_state);
                  num_states--;
                }
              }
            }
          }
        }
        catch (mcrl2::runtime_error& e)
        {
          cerr << "Error while exploring state space: " << e.what() << "\n";
          lg_error = true;
          save_error_trace(state);
          exit(EXIT_FAILURE);
        }

        if (deadlockstate)
        {
          check_deadlocktrace(state);
        }

        current_state++;
        if (!lgopts->suppress_progress_messages && mCRL2logEnabled(verbose) && ((current_state%1000) == 0))
        {
          mCRL2log(verbose) << "monitor: currently at level " << level << " with "
                                      << current_state << " state" << ((current_state==1)?"":"s") << " and "
                                      << trans << " transition" << ((trans==1)?"":"s")
                                      << " explored and " << num_states << " state" << ((num_states==1)?"":"s") << " seen." << std::endl;
        }
        if (current_state == endoflevelat)
        {
          if (lgopts->bithashing)
          {
            state_queue.swap_queues();
          }
          if (!lgopts->suppress_progress_messages)
          {
            mCRL2log(verbose) << "monitor: level " << level << " done."
                              << " (" << current_state - prevcurrent << " state"
                              << ((current_state-prevcurrent)==1?"":"s") << ", "
                              << trans-prevtrans << " transition"
                              << ((trans-prevtrans)==1?")\n":"s)\n");
          }
          level++;
          size_t nextcurrent = endoflevelat;
          endoflevelat = (limit>num_states)?num_states:limit;
          if (lgopts->bithashing)
          {
            if ((limit - num_states) < state_queue.max_size())
            {
              state_queue.set_max_size(limit - num_states);
            }
          }
          current_state = nextcurrent;
          prevcurrent = current_state;
          prevtrans = trans;
        }
      }
      delete nsgen;
    }
    else if (lgopts->expl_strat == es_depth)
    {
      size_t nsgens_size = (lgopts->todo_max<128)?lgopts->todo_max:128;
      NextStateGenerator** nsgens = (NextStateGenerator**) malloc(nsgens_size*sizeof(NextStateGenerator*));
      if (nsgens == NULL)
      {
        throw mcrl2::runtime_error("cannot create state stack");
      }
      nsgens[0] = nstate->getNextStates(state);
      for (size_t i=1; i<nsgens_size; i++)
      {
        nsgens[i] = NULL;
      }
      size_t nsgens_num = 1;

      bool top_trans_seen = false;
      // trans_seen(s) := we have seen a transition from state s
      // inv:  forall i : 0 <= i < nsgens_num-1 : trans_seen(nsgens[i]->get_state())
      //       nsgens_num > 0  ->  top_trans_seen == trans_seen(nsgens[nsgens_num-1])
      while (!must_abort && (nsgens_num > 0) && (! lgopts->trace || (tracecnt < lgopts->max_traces)))
      {
        NextStateGenerator* nsgen = nsgens[nsgens_num-1];
        state = nsgen->get_state();
        check_divergence(state);
        ATermAppl Transition;
        ATerm NewState;
        bool new_state = false;
        bool add_new_states = (current_state < lgopts->max_states);
        bool state_is_deadlock = !top_trans_seen /* && !nsgen->next(...) */ ;
        bool priority;
        try
        {
          if (nsgen->next(&Transition,&NewState,&priority))
          {
            NewState = get_repr(NewState);
            if (!priority)   // don't store confluent self loops
            {
              top_trans_seen = true;
              // inv
              state_is_deadlock = false;
              if (add_transition(state,Transition,NewState))
              {
                if (add_new_states)
                {
                  new_state = true;
                  if ((nsgens_num == nsgens_size) && (nsgens_size < lgopts->todo_max))
                  {
                    nsgens_size = nsgens_size*2;
                    if (nsgens_size > lgopts->todo_max)
                    {
                      nsgens_size = lgopts->todo_max;
                    }
                    nsgens = (NextStateGenerator**) realloc(nsgens,nsgens_size*sizeof(NextStateGenerator*));
                    if (nsgens == NULL)
                    {
                      throw mcrl2::runtime_error("cannot enlarge state stack");
                    }
                    for (size_t i=nsgens_num; i<nsgens_size; i++)
                    {
                      nsgens[i] = NULL;
                    }
                  }
                  if (nsgens_num < nsgens_size)
                  {
                    nsgens[nsgens_num] = nstate->getNextStates(NewState,nsgens[nsgens_num]);
                    nsgens_num++;
                    top_trans_seen = false;
                    // inv
                  }
                }
              }
            }
          }
          else
          {
            nsgens_num--;
            top_trans_seen = true;
            // inv
          }
          // inv

        }
        catch (mcrl2::runtime_error& e)
        {
          cerr << "Error while exploring state space: " << e.what() << "\n";
          lg_error = true;
          save_error_trace(state);
          exit(EXIT_FAILURE);
        }

        if (state_is_deadlock)
        {
          check_deadlocktrace(state);
        }

        if (new_state)
        {
          current_state++;
          if (!lgopts->suppress_progress_messages && mCRL2logEnabled(verbose) && ((current_state%1000) == 0))
          {
            mCRL2log(verbose) << "monitor: currently explored "
                              << current_state << " state" << ((current_state==1)?"":"s")
                              << " and " << trans << " transition" << ((trans==1)?"":"s")
                              << " (stacksize is " << nsgens_num << ")" << std::endl;
          }
        }
      }

      for (size_t i=0; i<nsgens_size; i++)
      {
        delete nsgens[i];
      }

      free(nsgens);
    }
    else
    {
      mCRL2log(error) << "unknown exploration strategy" << std::endl;
    }
  }

  completely_generated = true;

  return !lg_error;
}

} // namespace lts
} // namespace mcrl2
