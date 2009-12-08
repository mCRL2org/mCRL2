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
    #pragma implementation "simulator.h"
#endif

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <sstream>
#include <cstdlib>
#include <aterm2.h>
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/lps/nextstate.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/trace.h"
#include "mcrl2/core/aterm_ext.h"
#include "simulator.h"

using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::trace;

StandardSimulator::StandardSimulator()
{
    use_dummies = false;
    rewr_strat = mcrl2::data::rewriter::jitty;

    state_vars = ATmakeList0();
    ATprotectList(&state_vars);
    initial_state = NULL;;
    ATprotect(&initial_state);
    current_state = NULL;
    ATprotect(&current_state);
    next_states = ATmakeList0();
    ATprotectList(&next_states);
    trace = ATmakeList0();
    ATprotectList(&trace);
    ecart = ATmakeList0();
    ATprotectList(&ecart);

    seen_states = ATindexedSetCreate(100,80);

    tau_prior = false;
    // error = false;

    nextstate = NULL;
    nextstategen = NULL;
}

StandardSimulator::~StandardSimulator()
{
	for (viewlist::iterator i = views.begin(); i != views.end(); i++)
	{
		(*i)->Unregistered();
	}

	if ( initial_state != NULL )
	{
		delete nextstategen;
		delete nextstate;
	}

	ATunprotectList(&state_vars);
	ATunprotect(&initial_state);
	ATunprotect(&current_state);
	ATunprotectList(&next_states);
	ATunprotectList(&trace);
	ATunprotectList(&ecart);
}

void StandardSimulator::LoadSpec(mcrl2::lps::specification const& spec)
{
    state_vars = spec.process().process_parameters();

    delete nextstategen;
    delete nextstate;
    m_rewriter.reset(new mcrl2::data::rewriter(spec.data(), rewr_strat));
    m_enumerator_factory.reset(new mcrl2::data::enumerator_factory< mcrl2::data::classic_enumerator< > >(spec.data(), *m_rewriter));
    nextstate = createNextState(spec, *m_enumerator_factory, !use_dummies,GS_STATE_VECTOR);
    nextstategen = NULL;
    initial_state = nextstate->getInitialState();

    current_state = NULL;
    InitialiseViews();
    Reset(initial_state);
}

void StandardSimulator::LoadView(const std::string &filename)
{
  gsErrorMsg("cannot open DLLs without wxWidgets\n");
}

void StandardSimulator::SetTauPrioritisation(bool enable)
{
  tau_prior = enable;
}

bool StandardSimulator::IsActive()
{
  return !ATisEmpty(trace);
}


void StandardSimulator::Register(SimulatorViewInterface *View)
{
	views.push_back(View);
	View->Registered(this);
	if ( !ATisEmpty(trace) )
	{
	        View->Initialise(state_vars);
                View->StateChanged(NULL, current_state, next_states);
		View->TraceChanged(GetTrace(),0);
		View->TracePosChanged(ATAgetFirst(ATLgetFirst(trace)),current_state,ATgetLength(trace)-1);
	}
}

void StandardSimulator::Unregister(SimulatorViewInterface *View)
{
	views.remove(View);
	View->Unregistered();
}

ATermList StandardSimulator::GetParameters()
{
	return state_vars;
}

void StandardSimulator::Reset()
{
        Reset(nextstate->getInitialState());
}

void StandardSimulator::Reset(ATerm State)
{
	initial_state = State;
	if ( initial_state != NULL )
	{
		traceReset(initial_state);
		SetCurrentState(initial_state);
		UpdateTransitions();

		for (viewlist::iterator i = views.begin(); i != views.end(); i++)
		{
			(*i)->Reset(initial_state);
			(*i)->StateChanged(NULL,initial_state,next_states);
		}
	}
}

bool StandardSimulator::Undo()
{
	if ( ATgetLength(trace) > 1 )
	{
		ATermList l = traceUndo();
		ATerm state = ATgetFirst(ATgetNext(l));

		SetCurrentState(state);
		UpdateTransitions();

		for (viewlist::iterator i = views.begin(); i != views.end(); i++)
		{
			(*i)->Undo(1);
			(*i)->StateChanged(NULL,state,next_states);
		}

		return true;
	} else {
		return false;
	}
}

bool StandardSimulator::Redo()
{
	if ( !ATisEmpty(ecart) )
	{
		ATermList trans = traceRedo();
		ATerm state = ATgetFirst(ATgetNext(trans));

		SetCurrentState(state);
		UpdateTransitions();

		for (viewlist::iterator i = views.begin(); i != views.end(); i++)
		{
			(*i)->Redo(1);
			(*i)->StateChanged(NULL,state,next_states);
		}

		return true;
	} else {
		return false;
	}
}

ATerm StandardSimulator::GetState()
{
	return current_state;
}

ATermList StandardSimulator::GetNextStates()
{
	return next_states;
}

NextState *StandardSimulator::GetNextState()
{
	return nextstate;
}

bool StandardSimulator::ChooseTransition(unsigned int index)
{
	if ( !ATisEmpty(next_states) && (index < ATgetLength(next_states)) )
	{
		ATermList l = ATLelementAt(next_states,index);
		ATermAppl trans = ATAgetFirst(l);
		ATerm state = ATgetFirst(ATgetNext(l));

		SetCurrentState(state);
		UpdateTransitions();

		traceSetNext(l);

		for (viewlist::iterator i = views.begin(); i != views.end(); i++)
		{
			(*i)->StateChanged(trans,state,next_states);
		}

		if ( tau_prior )
		{
			ATbool b;
			ATindexedSetPut(seen_states,current_state,&b);

			bool found = false;
			unsigned int i=0;
			for (ATermList l=next_states; !ATisEmpty(l); l=ATgetNext(l),i++)
			{
				ATermList trans = ATLgetFirst(l);
				if ( ATisEmpty(ATLgetArgument(ATAgetFirst(trans),0)) )
				{
					if ( ATindexedSetGetIndex(seen_states,ATgetFirst(ATgetNext(trans))) < 0 )
					{
						found = true;
						break;
					}
				}
			}
			if ( found )
			{
				return ChooseTransition(i);
			} // else
			ATindexedSetReset(seen_states);
		}

		return true;
	} else {
		return false;
	}
}

int StandardSimulator::GetTraceLength()
{
	return ATgetLength(trace)+ATgetLength(ecart);
}

int StandardSimulator::GetTracePos()
{
	return ATgetLength(trace)-1;
}

bool StandardSimulator::SetTracePos(unsigned int pos)
{
	if ( ATgetLength(trace) == 0 )
	{
		return false;
	}

	unsigned int l = ATgetLength(trace)-1;

	if ( pos <= l+ATgetLength(ecart) )
	{
		while ( l < pos )
		{
			trace = ATinsert(trace,ATgetFirst(ecart));
			ecart = ATgetNext(ecart);
			l++;
		}
		while ( l > pos )
		{
			ecart = ATinsert(ecart,ATgetFirst(trace));
			trace = ATgetNext(trace);
			l--;
		}

		ATermAppl trans = ATAgetFirst(ATLgetFirst(trace));
		ATerm state = ATgetFirst(ATgetNext(ATLgetFirst(trace)));

		SetCurrentState(state);
		UpdateTransitions();

		for (viewlist::iterator i = views.begin(); i != views.end(); i++)
		{
			(*i)->TracePosChanged(trans,state,pos);
			(*i)->StateChanged(NULL,state,next_states);
		}

		return true;
	} else {
		return false;
	}
}

ATermList StandardSimulator::GetTrace()
{
	ATermList l = ecart;
	ATermList m = trace;

	for (; !ATisEmpty(m); m=ATgetNext(m))
	{
		l = ATinsert(l,ATgetFirst(m));
	}

	return l;
}

ATerm StandardSimulator::GetNextStateFromTrace()
{
  if ( ATisEmpty(ecart) )
  {
    return NULL;
  }

  return ATgetFirst(ATgetNext(ATLgetFirst(ecart)));
}

ATermAppl StandardSimulator::GetNextTransitionFromTrace()
{
  if ( ATisEmpty(ecart) )
  {
    return NULL;
  }

  return ATAgetFirst(ATLgetFirst(ecart));
}

bool StandardSimulator::SetTrace(ATermList /* Trace */, unsigned int /* From */)
{
	// XXX
	return false;
}


void StandardSimulator::InitialiseViews()
{
	for (viewlist::iterator i = views.begin(); i != views.end(); i++)
	{
		(*i)->Initialise(state_vars);
	}
}


void StandardSimulator::traceReset(ATerm state)
{
	trace = ATmakeList1((ATerm) ATmakeList2((ATerm) gsMakeNil(), state));
	ecart = ATmakeList0();
}

void StandardSimulator::traceSetNext(ATermList transition)
{
	trace = ATinsert(trace,(ATerm) transition);
	ecart = ATmakeList0();
}

ATermList StandardSimulator::traceUndo()
{
	ecart = ATinsert(ecart,ATgetFirst(trace));
	trace = ATgetNext(trace);

	return ATLgetFirst(trace);
}

ATermList StandardSimulator::traceRedo()
{
	trace = ATinsert(trace,ATgetFirst(ecart));
	ecart = ATgetNext(ecart);

	return ATLgetFirst(trace);
}

void StandardSimulator::LoadTrace(const std::string &filename)
{
	Trace tr(filename);

	ATerm state = (ATerm) tr.currentState();
	ATermList newtrace = ATmakeList0();

	if ( (state != NULL) && ((state = nextstate->parseStateVector((ATermAppl) state)) == NULL) )
	{
          std::string s = "initial state of trace is not a valid state for this specification";
                throw s;
	}

	if ( state == NULL )
	{
	    Reset();
                state = current_state;
	} else {
	    Reset(state);
	}

	ATermAppl act;
        unsigned int idx = 0;
	while ( (act = tr.nextAction()) != NULL )
	{
            idx++;
	    nextstategen = nextstate->getNextStates(state,nextstategen);
	    if ( gsIsMultAct(act) )
	    {
		    ATermAppl Transition;
		    ATerm NewState;
		    bool found = false;
		    while ( nextstategen->next(&Transition,&NewState) )
		    {
			    if ( ATisEqual(Transition,act) )
			    {
				    if ( (tr.currentState() == NULL) || ((NewState = nextstate->parseStateVector(tr.currentState(),NewState)) != NULL) )
				    {
					    newtrace = ATinsert(newtrace,(ATerm) ATmakeList2((ATerm) Transition,NewState));
					    state = NewState;
					    found = true;
					    break;
				    }
			    }
		    }
		    if ( !found )
		    {
                      std::stringstream ss;
                      ss << "could not perform action " << idx << " (";
                      PrintPart_CXX(ss,(ATerm) act,ppDefault);
                      ss << ") from trace";
                      throw ss.str();
		    }
	    } else {
		    // Perhaps trace was in plain text format; try pp-ing actions
		    // XXX Only because libtrace cannot parse text (yet)
		    ATermAppl Transition;
		    ATerm NewState;
                    std::string s(ATgetName(ATgetAFun(act)));
		    bool found = false;
		    while ( nextstategen->next(&Transition,&NewState) )
		    {
                      std::string t = PrintPart_CXX((ATerm) Transition, ppDefault);
			    if ( s == t )
			    {
				    if ( (tr.currentState() == NULL) || ((NewState = nextstate->parseStateVector(tr.currentState(),NewState)) != NULL) )
				    {
					    newtrace = ATinsert(newtrace,(ATerm) ATmakeList2((ATerm) Transition,NewState));
					    state = NewState;
					    found = true;
					    break;
				    }
			    }
		    }
		    if ( !found )
		    {
                      std::stringstream ss;
                            ss << "could not perform action " << idx << " (" << ATwriteToString((ATerm) act) << ") from trace";
                            throw ss.str();
		    }
	    }
	}

	for (ATermList l=newtrace; !ATisEmpty(l); l=ATgetNext(l) )
	{
	        ecart = ATinsert(ecart,ATgetFirst(l));
	}
	newtrace = ATinsert(ecart,ATgetFirst(trace));
	for (viewlist::iterator i = views.begin(); i != views.end(); i++)
	{
	        (*i)->TraceChanged(newtrace,0);
	}
}

void StandardSimulator::SaveTrace(const std::string &filename)
{
	Trace tr;
	if ( !ATisEmpty(trace) )
	{
	        ATermList m = ATreverse(trace);
	        tr.setState(nextstate->makeStateVector(ATgetFirst(ATgetNext(ATLgetFirst(m)))));
	        for (ATermList l=ATconcat(ATgetNext(m),ecart); !ATisEmpty(l); l=ATgetNext(l))
	        {
	    	    tr.addAction(ATAgetFirst(ATLgetFirst(l)));
	    	    tr.setState(nextstate->makeStateVector(ATgetFirst(ATgetNext(ATLgetFirst(l)))));
	        }
	}

	tr.save(filename);
}

void StandardSimulator::SetCurrentState(ATerm state)
{
	current_state = state;
}

void StandardSimulator::UpdateTransitions()
{
	nextstategen = nextstate->getNextStates(current_state,nextstategen);
	next_states = ATmakeList0();
	ATermAppl transition;
	ATerm newstate;
	while ( nextstategen->next(&transition,&newstate) )
	{
		next_states = ATinsert(next_states,(ATerm) ATmakeList2((ATerm) transition,newstate));
	}
	// error = nextstategen->errorOccurred();
}

/* bool StandardSimulator::ErrorOccurred()
{
	return error;
} */
