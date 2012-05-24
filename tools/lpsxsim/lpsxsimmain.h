// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file xsimmain.h

#ifndef __xsimmain_H__
#define __xsimmain_H__

#include <wx/wx.h>
#include <wx/splitter.h>
#include <tooltipListView.h>

#include "mcrl2/lps/simbase.h"
#include "simulatorgui.h"
#include "lpsxsimtrace.h"

//----------------------------------------------------------------------------
// XSimMain
//----------------------------------------------------------------------------

const int ID_SPLITTER = 10000;
const int ID_LISTCTRL1 = 10001;
const int ID_LISTCTRL2 = 10002;
const int ID_MENU = 10003;
const int ID_UNDO = 10004;
const int ID_REDO = 10005;
const int ID_RESET = 10006;
const int ID_FITCS = 10007;
const int ID_TRACE = 10008;
const int ID_LOADVIEW = 10009;
const int ID_LOADTRACE = 10010;
const int ID_SAVETRACE = 10011;
const int ID_TAU = 10012;
const int ID_SHOWDC = 10013;
const int ID_DELAY = 10014;
const int ID_PLAYI = 10015;
const int ID_PLAYC = 10016;
const int ID_PLAYRI = 10017;
const int ID_PLAYRC = 10018;
const int ID_STOP = 10019;
const int ID_TOOLTIP = 10020;

enum play_func_enum { FUNC_NONE, FUNC_PLAY, FUNC_RANDOM };

class XSimMain: public wxFrame, public SimulatorViewInterface
{
  public:
    // constructors and destructors
    XSimMain(wxWindow* parent, wxWindowID id, const wxString& title,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = wxDEFAULT_FRAME_STYLE);
    ~XSimMain();
    void SetViews(wxListCtrl* state, wxListCtrl* trans);
    void LoadFile(const wxString& filename);
    void LoadDLL(const wxString& filename);

    // XXX make private and use functions?
    StandardSimulatorGUI* simulator;

    // SimulatorViewInterface methods
    virtual void Registered(SimulatorInterface* Simulator);
    virtual void Unregistered();
    virtual void Initialise(const mcrl2::data::variable_list Pars);
    virtual void StateChanged(
                     mcrl2::lps::multi_action, 
                     const mcrl2::lps::state, 
                     std::vector<mcrl2::lps::multi_action>,
                     std::vector<mcrl2::lps::state>);
    virtual void StateChanged(
                     const mcrl2::lps::state, 
                     std::vector<mcrl2::lps::multi_action>,
                     std::vector<mcrl2::lps::state>);
    virtual void Reset(mcrl2::lps::state State);
    virtual void Undo(size_t Count);
    virtual void Redo(size_t Count);
    virtual void TraceChanged(mcrl2::trace::Trace tr, size_t From);
    virtual void TracePosChanged(size_t Index);

  private:
    // WDR: method declarations for XSimMain
    void CreateMenu();
    void CreateStatus();
    void CreateContent();
    void Stopper_Enter();
    void Stopper_Exit();
    void StopAutomation();
    void SetInteractiveness(bool interactive);

  private:
    // WDR: member variable declarations for XSimMain
    wxString base_title;
    wxTimer timer;
    mcrl2::data::variable_list state_varnames;
    wxMenuBar* menu;
    wxMenuItem* openitem;
    wxMenuItem* ldtrcitem;
    wxMenuItem* svtrcitem;
    wxMenu* editmenu;
    wxMenuItem* undo;
    wxMenuItem* redo;
    wxMenuItem* tau_prior;
    wxMenuItem* showdc;
    wxMenuItem* playiitem;
    wxMenuItem* playcitem;
    wxMenuItem* playriitem;
    wxMenuItem* playrcitem;
    wxMenuItem* stopitem;
    wxMenuItem* tooltip;
    wxBoxSizer* mainsizer;
    wxSplitterWindow* split;
    wxPanel* toppanel;
    wxBoxSizer* topsizer;
    wxBoxSizer* topboxsizer;
    tooltipListView* stateview;
    int stateview_header_height;
    int stateview_item_height;
    wxPanel* bottompanel;
    wxBoxSizer* bottomsizer;
    wxBoxSizer* bottomboxsizer;
    tooltipListView* transview;
    XSimTrace* tracewin;
    bool stopped;
    int stopper_cnt;
    bool interactive;
    play_func_enum timer_func;
    int timer_interval;
    mcrl2::lps::state current_state;
    mcrl2::lps::specification m_specification;

  private:
    // WDR: handler declarations for XSimMain
    void OnOpen(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnUndo(wxCommandEvent& event);
    void OnRedo(wxCommandEvent& event);
    void OnReset(wxCommandEvent& event);
    void OnLoadTrace(wxCommandEvent& event);
    void OnSaveTrace(wxCommandEvent& event);
    void OnFitCurrentState(wxCommandEvent& event);
    void OnTrace(wxCommandEvent& event);
    void OnLoadView(wxCommandEvent& event);
    void OnTauPrioritisation(wxCommandEvent& event);
    void OnShowDCChanged(wxCommandEvent& event);
    void OnSetDelay(wxCommandEvent& event);
    void OnResetAndPlay(wxCommandEvent& event);
    void OnPlay(wxCommandEvent& event);
    void OnResetAndPlayRandom(wxCommandEvent& event);
    void OnPlayRandom(wxCommandEvent& event);
    void OnTimer(wxTimerEvent& event);
    void OnStop(wxCommandEvent& event);
    void OnTraceClose(wxCloseEvent& event);
    void OnCloseWindow(wxCloseEvent& event);
    void stateOnListItemSelected(wxListEvent& event);
    void transOnListItemActivated(wxListEvent& event);
    void OnResize(wxSizeEvent& event);
    void OnMaximize(wxMaximizeEvent& event);
    void UpdateSizes();

  private:
    void SetCurrentState(mcrl2::lps::state s, bool showchange = false);
    void UpdateTransitions(
               const std::vector<mcrl2::lps::multi_action>& next_actions,
               const std::vector<mcrl2::lps::state>& next_states);

  private:
    DECLARE_EVENT_TABLE()
};

#endif
