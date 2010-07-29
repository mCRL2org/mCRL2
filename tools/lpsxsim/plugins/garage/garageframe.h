// Author(s): Aad Mathijssen and Hannes Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file garageframe.h

// ---------------------------------
//
// This class is used to creat the
//    main application GUI.
//
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


#ifndef APPFRAME_H
#define APPFRAME_H

#include <wx/wx.h>
#include <string>
#include <vector>
#include <iostream>
#include "garagecanvas.h"
#include "simbase.h"
#include "aterm2.h"
#include "mcrl2/core/print.h"
#include "mcrl2/lps/nextstate.h"
#include "mcrl2/core/aterm_ext.h"

#ifndef GARAGEFRAMEDLL
class GarageFrame : public wxFrame, public SimulatorViewInterface
#else
class GarageFrame : public wxFrame, public SimulatorViewDLLInterface
#endif
{
public:
  GarageFrame( wxString const& title,
               int xPos,
               int yPos,
               int width,
               int height            );
  GarageFrame( wxWindow* parent,
               wxString const& title,
               int xPos,
               int yPos,
               int width,
               int height            );
  ~GarageFrame();

  void Initialise(ATermList Pars);
  void StateChanged(ATermAppl Transition, ATerm State, ATermList NextStates);
  void Reset(ATerm State);
  void Undo(unsigned int Count);
  void Redo(unsigned int Count);
  void TraceChanged(ATermList Trace, unsigned int From);
  void TracePosChanged(ATermAppl Transition, ATerm State, unsigned int Index);
  void Registered(SimulatorInterface *Simulator);
  void Unregistered();

private:
  GarageCanvas* canvas;
  SimulatorInterface* simulator;
  void UpdateState(ATerm State);
  void InitialiseFrame();
  ATermAppl MakeFloorPos(int row, int col, bool has_part, int part);
  ATermAppl MakeShuttlePos(int row, int part);
  ATermAppl MakeOpId(std::string name, ATermAppl SortExpr);
  ATermAppl MakeDataVarId(std::string name, ATermAppl SortExpr);
  ATermAppl MakeSortId(std::string name);
  int stateIndex;

  int liftHeight;
  int liftOccupied;
  int floorState[3][20];
  int shuttleState[3][2];

  void OnCloseWindow( wxCloseEvent &event );
  void OnMoveWindow( wxMoveEvent &event );
  DECLARE_EVENT_TABLE()
};


#endif // APPFRAME_H
