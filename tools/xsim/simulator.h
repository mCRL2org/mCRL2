// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file simulator.h

#ifndef __simulator_H__
#define __simulator_H__

#include <string>
#include <aterm2.h>

#include <mcrl2/lps/nextstate.h>
#include "mcrl2/data/rewrite.h"
#include "simbase.h"

class StandardSimulator: virtual public SimulatorInterface
{
public:
    // constructors and destructors
    StandardSimulator();
    virtual ~StandardSimulator();

    virtual void LoadSpec(ATermAppl spec);
    /* Load mCRL2 specification spec for simulation */
    virtual void LoadView(const std::string &filename);
    /* Load DLL filename as simulation view */
    virtual void SetTauPrioritisation(bool enable = true);
    /* Enable/disable (simplistic) tau-prioritisation */
    virtual void LoadTrace(const std::string &filename);
    /* Load trace from filename
     * This function throws a string on errors */
    virtual void SaveTrace(const std::string &filename);
    /* Save current trace to filename
     * This function throws a string on errors */
    virtual bool IsActive();
    /* Returns true iff a specification has been loaded */
    bool ErrorOccurred();
    /* Returns true iff an error occurred while computing transitions
     * from the current state */

    // XXX make private and use functions?
    bool use_dummies;
    RewriteStrategy rewr_strat;

    // SimulatorInterface methods
    virtual void Register(SimulatorViewInterface *View);
    virtual void Unregister(SimulatorViewInterface *View);
    virtual ATermList GetParameters();
    virtual void Reset();
    virtual void Reset(ATerm State);
    virtual bool Undo();
    virtual bool Redo();
    virtual ATerm GetState();
    virtual ATermList GetNextStates();
    virtual NextState *GetNextState();
    virtual bool ChooseTransition(unsigned int index);
    virtual int GetTraceLength();
    virtual int GetTracePos();
    virtual bool SetTracePos(unsigned int pos);
    virtual ATermList GetTrace();
    virtual ATerm GetNextStateFromTrace();
    virtual ATermAppl GetNextTransitionFromTrace();
    virtual bool SetTrace(ATermList Trace, unsigned int From);
    virtual void InitialiseViews();

private:
    void traceReset(ATerm state);
    void traceSetNext(ATermList transition);
    ATermList traceUndo();
    ATermList traceRedo();

private:
    bool tau_prior;
    bool error;
    ATermList state_vars;
    ATerm initial_state;
    ATerm current_state;
    ATermList next_states;
    ATermList trace;
    ATermList ecart;
    viewlist views;
    ATermIndexedSet seen_states;
    NextState *nextstate;
    NextStateGenerator *nextstategen;

private:
    void SetCurrentState(ATerm state);
    void UpdateTransitions();
};

#endif
