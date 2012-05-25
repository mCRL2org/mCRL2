// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file xsimtrace.h

#ifndef __xsimtrace_H__
#define __xsimtrace_H__

#include <wx/wx.h>
#include <wx/listctrl.h>
#include "mcrl2/aterm/aterm2.h"
#include "mcrl2/lps/simbase.h"

//----------------------------------------------------------------------------
// XSimTrace
//----------------------------------------------------------------------------

class XSimTrace: public wxFrame, public SimulatorViewInterface
{
  public:
    // constructors and destructors
    XSimTrace(wxWindow* parent);
    /*    XSimTrace( wxWindow *parent, wxWindowID id, const wxString &title,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE ) : XSimTrace(parent);*/

    // SimulatorViewInterface
    virtual void Registered(SimulatorInterface* Simulator);
    virtual void Unregistered();
    virtual void Initialise(const mcrl2::data::variable_list Pars);
    virtual void StateChanged(
                     mcrl2::lps::multi_action,
                     const mcrl2::lps::state,
                     atermpp::vector<mcrl2::lps::multi_action>,
                     std::vector<mcrl2::lps::state>);
     virtual void StateChanged(
                     const mcrl2::lps::state,
                     atermpp::vector<mcrl2::lps::multi_action>,
                     std::vector<mcrl2::lps::state>);
    virtual void Reset(mcrl2::lps::state State);
    virtual void Undo(size_t Count);
    virtual void Redo(size_t Count);
    virtual void TraceChanged(mcrl2::trace::Trace tr, size_t From);
    virtual void TracePosChanged(size_t Index);

  private:
    // WDR: method declarations for XSimMain
    void AddState(const mcrl2::lps::multi_action Transition, const mcrl2::lps::state &State, bool enabled);
    void _add_state(const mcrl2::lps::multi_action Transition, const mcrl2::lps::state &State, bool enabled);
    void _reset(const mcrl2::lps::state &State);
    void _update();

  private:
    // WDR: member variable declarations for XSimMain
    wxListView* traceview;
    SimulatorInterface* simulator;
    size_t current_pos;

  private:
    // WDR: handler declarations for XSimMain
    void OnCloseWindow(wxCloseEvent& event);
    void OnListItemActivated(wxListEvent& event);

  private:

  private:
    DECLARE_EVENT_TABLE()
};

#endif
