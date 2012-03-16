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
#include <memory>
#include "mcrl2/aterm/aterm2.h"

#include "mcrl2/lps/nextstate/standard.h"
#include "mcrl2/data/rewriter.h"
#include "simbase.h"

class StandardSimulator : virtual public SimulatorInterface
{
  public:
    // constructors and destructors
    StandardSimulator();
    virtual ~StandardSimulator();

    virtual void LoadSpec(mcrl2::lps::specification const& spec);
    /* Load mCRL2 specification spec for simulation */
    virtual void LoadView(const std::string& filename);
    /* Load DLL filename as simulation view */
    virtual void SetTauPrioritisation(bool enable = true);
    /* Enable/disable (simplistic) tau-prioritisation */
    virtual void LoadTrace(const std::string& filename);
    /* Load trace from filename
     * This function throws a string on errors */
    virtual void SaveTrace(const std::string& filename);
    /* Save current trace to filename
     * This function throws a string on errors */
    virtual bool IsActive();
    /* Returns true iff a specification has been loaded */

    // XXX make private and use functions?
    bool use_dummies;
    mcrl2::data::rewriter::strategy rewr_strat;

    // SimulatorInterface methods
    virtual void Register(SimulatorViewInterface* View);
    virtual void Unregister(SimulatorViewInterface* View);
    virtual mcrl2::data::variable_list GetParameters();
    virtual void Reset();
    virtual void Reset(mcrl2::lps::state State);
    virtual bool Undo();
    virtual bool Redo();
    virtual mcrl2::lps::state GetState();
    virtual std::vector < mcrl2::lps::state > GetNextStates();
    virtual atermpp::vector < mcrl2::lps::multi_action > GetNextActions();
    virtual mcrl2::lps::NextState* GetNextState();
    virtual bool ChooseTransition(size_t index);
    virtual size_t GetTraceLength();
    virtual size_t GetTracePos();
    virtual bool SetTracePos(size_t pos);
    virtual mcrl2::trace::Trace GetTrace();
    virtual mcrl2::lps::state GetNextStateFromTrace();
    virtual mcrl2::lps::multi_action GetNextTransitionFromTrace();
    virtual void InitialiseViews();

  private:
    bool tau_prior;
    mcrl2::lps::specification m_spec;
    std::vector < mcrl2::lps::state > next_states;
    atermpp::vector < mcrl2::lps::multi_action > next_actions;
    mcrl2::trace::Trace trace;
    viewlist views;
    std::auto_ptr< mcrl2::data::rewriter >  m_rewriter;
    std::set < mcrl2::lps::state > seen_states;
    mcrl2::lps::NextState* nextstate;
    mcrl2::lps::NextStateGenerator* nextstategen;

    void UpdateTransitions();
    bool match_trace_recursively(const size_t pos,size_t &culprit_pos);
};

#endif
