// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file simulator.cpp

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "mcrl2/lps/simulator.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include <sstream>
#include <cstdlib>
#include "mcrl2/aterm/aterm2.h"
#include "mcrl2/aterm/aterm_ext.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/lps/simulator.h"

using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::trace;
using namespace mcrl2::data;
using namespace mcrl2::log;

StandardSimulator::StandardSimulator()
{
  use_dummies = false;
  rewr_strat = mcrl2::data::rewriter::jitty;
  tau_prior = false;
  nextstate = NULL;
  nextstategen = NULL;
}

StandardSimulator::~StandardSimulator()
{
  for (viewlist::iterator i = views.begin(); i != views.end(); i++)
  {
    (*i)->Unregistered();
  }

  if (nextstategen != NULL)
  {
    delete nextstategen;
    delete nextstate;
  }

}

void StandardSimulator::LoadSpec(const mcrl2::lps::specification &spec)
{
  m_spec=spec;

  delete nextstategen;
  delete nextstate;
  // Reset the trace and provide it with information to parse actions. 
  m_rewriter.reset(new mcrl2::data::rewriter(spec.data(), rewr_strat));
  nextstate = createNextState(spec, *m_rewriter, !use_dummies,GS_STATE_VECTOR);
  nextstategen = NULL;

  InitialiseViews();
  Reset();
}

void StandardSimulator::LoadView(const std::string& /*filename*/)
{
  mCRL2log(mcrl2::log::error) << "cannot open DLLs without wxWidgets" << std::endl;
}

void StandardSimulator::SetTauPrioritisation(bool enable)
{
  tau_prior = enable;
}

bool StandardSimulator::IsActive()
{
  return nextstate!=NULL;
}


void StandardSimulator::Register(SimulatorViewInterface* View)
{
  views.push_back(View);
  View->Registered(this);
  if (trace.number_of_actions()>0)
  {
    View->Initialise(m_spec.process().process_parameters());
    View->StateChanged(trace.currentState(), next_actions, next_states);
    View->TraceChanged(GetTrace(),0);
    View->TracePosChanged(trace.getPosition());
  }
}

void StandardSimulator::Unregister(SimulatorViewInterface* View)
{
  views.remove(View);
  View->Unregistered();
}

variable_list StandardSimulator::GetParameters()
{
  return m_spec.process().process_parameters();
}

void StandardSimulator::Reset()
{
  Reset(nextstate->make_new_state_vector(nextstate->getInitialState()));
}

void StandardSimulator::Reset(mcrl2::lps::state State)
{
  assert(IsActive());
  trace=Trace(m_spec.data(),m_spec.action_labels());

  trace.setState(State);

  UpdateTransitions();

  for (viewlist::iterator i = views.begin(); i != views.end(); i++)
  {
    (*i)->Reset(State);
    (*i)->StateChanged(State, next_actions, next_states);
  }
}

bool StandardSimulator::Undo()
{
  if (trace.getPosition() > 0)
  {
    trace.decreasePosition();
    UpdateTransitions();

    for (viewlist::iterator i = views.begin(); i != views.end(); i++)
    {
      (*i)->Undo(1);
      (*i)->StateChanged(trace.currentState(), next_actions, next_states);
    }

    return true;
  }
  else
  {
    return false;
  }
}

bool StandardSimulator::Redo()
{
  if (trace.getPosition()<trace.number_of_actions())
  {
    trace.increasePosition();
    UpdateTransitions();
    for (viewlist::iterator i = views.begin(); i != views.end(); i++)
    {
      (*i)->Redo(1);
      (*i)->StateChanged(trace.currentState(), next_actions, next_states);
    }

    return true;
  }
  else
  {
    return false;
  }
}

mcrl2::lps::state StandardSimulator::GetState()
{
  return trace.currentState();
} 

std::vector < mcrl2::lps::state > StandardSimulator::GetNextStates()
{
  return next_states;
}

atermpp::vector < mcrl2::lps::multi_action > StandardSimulator::GetNextActions()
{
  return next_actions;
}

NextState* StandardSimulator::GetNextState()
{
  return nextstate;
} 

bool StandardSimulator::ChooseTransition(const size_t index)
{
  if (index < next_states.size())
  {
    trace.truncate();

    assert(index<next_actions.size());
    const mcrl2::lps::multi_action ma=next_actions[index];
    trace.addAction(ma);
    const mcrl2::lps::state s=next_states[index];
    trace.setState(s);

    UpdateTransitions();

    for (viewlist::iterator i = views.begin(); i != views.end(); i++)
    {
      (*i)->StateChanged(ma, s, next_actions, next_states);
    }

    if (tau_prior)
    {
      seen_states.insert(s);

      bool found = false;
      size_t i=0;
      for (size_t i=0; i<next_states.size(); ++i) 
      {
        if (next_actions[i].actions().empty() )   // This is a tau action.
        {
          if (seen_states.count(next_states[i])==0)  // not present.
          {
            found = true;
            break;
          }
        }
      }
      if (found)
      {
        return ChooseTransition(i);
      }
      seen_states.clear();
    }

    return true;
  }
  else
  {
    return false;
  }
}

size_t StandardSimulator::GetTraceLength()
{
  return trace.number_of_actions();
}

size_t StandardSimulator::GetTracePos()
{
  return trace.getPosition();
}

bool StandardSimulator::SetTracePos(size_t pos)
{
  if (trace.number_of_actions() < pos)
  {
    return false;
  }
 
  trace.setPosition(pos);
  UpdateTransitions();

  for (viewlist::iterator i = views.begin(); i != views.end(); i++)
  {
    (*i)->Reset(trace.currentState());
    (*i)->StateChanged(trace.currentState(), next_actions, next_states);
  }

  return true;
}

Trace StandardSimulator::GetTrace()
{
  return trace;
}

mcrl2::lps::state StandardSimulator::GetNextStateFromTrace()
{
  return trace.nextState();
} 

mcrl2::lps::multi_action StandardSimulator::GetNextTransitionFromTrace()
{
  return trace.currentAction();
}

void StandardSimulator::InitialiseViews()
{
  for (viewlist::iterator i = views.begin(); i != views.end(); i++)
  {
    (*i)->Initialise(m_spec.process().process_parameters());
  }
}

void StandardSimulator::LoadTrace(const std::string& filename)
{
  trace.load(filename);
  // trace=Trace(filename,m_spec.data(),m_spec.action_labels()); 

  if (trace.current_state_exists())
  {
    if (trace.currentState()!=nextstate->make_new_state_vector(nextstate->getInitialState()))
    {
      throw mcrl2::runtime_error("The initial state of the trace is not equal to the initial state of this specification");
    }
  }
  else
  {
    assert(trace.getPosition()==0);
    trace.setState(nextstate->make_new_state_vector(nextstate->getInitialState()));
  }

  // Check whether the trace matches the specification, and reconstruct the states if they are not part of the trace.
  for (mcrl2::lps::multi_action act; trace.getPosition()<trace.number_of_actions(); )
  {
    act=trace.currentAction();
    nextstategen = nextstate->getNextStates(nextstate->parse_state_vector_new(trace.currentState()),nextstategen);
    mcrl2::lps::multi_action ma;
    ATerm NewState;
    bool found = false;
    while (nextstategen->next(ma,&NewState))
    {
      if (ma==act)
      {
        mcrl2::lps::state new_state=nextstate->make_new_state_vector(NewState);
        if (trace.getPosition()+1>=trace.number_of_states() || new_state==trace.nextState()) 
        {
          trace.increasePosition();
          trace.setState(new_state);
          found = true;
          break;
        }
      }
    }
    if (!found)
    {
      std::stringstream ss;
      ss << "could not perform action " << trace.getPosition() << " (";
      ss << pp(act) << ") from trace";
      throw mcrl2::runtime_error(ss.str());
    }
  }
  trace.setPosition(0);

  for (viewlist::iterator i = views.begin(); i != views.end(); i++)
  {
    (*i)->TraceChanged(trace,0);
  } 
}

void StandardSimulator::SaveTrace(const std::string& filename)
{
  trace.save(filename);
}

void StandardSimulator::UpdateTransitions()
{
  nextstategen = nextstate->getNextStates(nextstate->parse_state_vector_new(trace.currentState()),nextstategen);
  next_states.clear();
  next_actions.clear();
  mcrl2::lps::multi_action ma;
  ATerm NewState;
  while (nextstategen->next(ma,&NewState))
  {
    next_states.push_back(nextstate->make_new_state_vector(NewState));
    next_actions.push_back(ma);
  }
}

