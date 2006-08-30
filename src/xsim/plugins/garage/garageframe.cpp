// ---------------------------------
// garageframe.cpp
// (c) 2005
// A.J. Pretorius
//
// Dept of Math and Computer Science
// Technische Universitiet Eindhoven
// a.j.pretorius@tue.nl
// ---------------------------------
//
// This class is used to creat the
//    main application GUI.
//
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#include "garageframe.h"
#include <sstream>

BEGIN_EVENT_TABLE(GarageFrame,wxFrame)
    EVT_CLOSE(GarageFrame::OnCloseWindow)
    EVT_MOVE(GarageFrame::OnMoveWindow)
END_EVENT_TABLE()

// -------------------------------
inline std::string intToString( int i )
// -------------------------------
{
	std::ostringstream oss;
	oss << i;
	return oss.str();
}

// --------------------------------------------
GarageFrame::GarageFrame( wxChar* title,
                          int xPos,
                          int yPos,
                          int width,
                          int height     )
           : wxFrame( (wxFrame*) NULL,
                      -1,
                      title,
                      wxPoint( xPos, yPos),
                      wxSize( width, height ) )
// --------------------------------------------
// The constructor initialises the main frame
//
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
{
  this->InitialiseFrame();
}



// --------------------------------------------
GarageFrame::GarageFrame( wxWindow* parent,
                          wxChar* title,
                          int xPos,
                          int yPos,
                          int width,
                          int height     )
          : wxFrame( parent,
                      -1,
                      title,
                      wxPoint( xPos, yPos),
                      wxSize( width, height ) )
// --------------------------------------------
// The constructor initialises the main frame
//
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
{
  this->InitialiseFrame();
}




// ------------------------
GarageFrame::~GarageFrame()
// ------------------------
{
  // Desctructor of base class destroys all wxWidget children
}


// ------------------------
void GarageFrame::InitialiseFrame()
// ------------------------
{
  // Initialize the class
  // Get size of this frame
  int frameWidth;
  int frameHeight;
  this->GetSize( &frameWidth,
                 &frameHeight );

  // Initialize canvas
  canvas = new GarageCanvas( this,
                         0,
                         0,
                         frameWidth,
                         frameHeight );

  stateIndex = -1;
  simulator = NULL;
  canvas->InitialiseCanvas();
  //enable constructor functions
  gsEnableConstructorFunctions();
}


// -------------------------------------------
void GarageFrame::Initialise( ATermList Pars )
// -------------------------------------------
{
  stateIndex = -1;
  int n = ATgetLength(Pars);
  ATermAppl gs_sl = MakeDataVarId("gs_sl", MakeSortId("GlobalState"));
  for (int i = 0; i < n; ++i) {
    if (ATisEqual(ATAelementAt(Pars, i), gs_sl)) {
      stateIndex = i;
      break;
    }
  }
}


// ---------------------------------------------------
void GarageFrame::StateChanged( ATermAppl ,
                                ATerm State,
                                ATermList )
// ---------------------------------------------------
{
  UpdateState(State);
}





// ---------------------------------------
void GarageFrame::Reset( ATerm )
// ---------------------------------------
{
}





// --------------------------------
void GarageFrame::Undo( int )
// --------------------------------
{
}





// --------------------------------
void GarageFrame::Redo( int )
// --------------------------------
{
}





// --------------------------------------------------------
void GarageFrame::TraceChanged( ATermList , int )
// --------------------------------------------------------
{
}




// ------------------------------------------------------
void GarageFrame::TracePosChanged( ATermAppl ,
                                   ATerm ,
                                   int             )
// ------------------------------------------------------
{
}

// ------------------------------------------------------
void GarageFrame::Registered(SimulatorInterface *Simulator)
// ------------------------------------------------------
{
  #ifdef GARAGEFRAMEDLL
  SimulatorViewDLLInterface::Registered(Simulator);
  #endif
  simulator = Simulator;
}

// ------------------------------------------------------
void GarageFrame::Unregistered()
// ------------------------------------------------------
{
  #ifdef GARAGEFRAMEDLL
  SimulatorViewDLLInterface::Unregistered();
  #endif
  simulator = NULL;
}


// --------------------------------------------
ATermAppl GarageFrame::MakeFloorPos( int row,
                                     int col,
                                     bool has_part,
                                     int part )
// --------------------------------------------
{
  assert(row >= 1 && row <= 3);
  assert(col >= 1 && row <= 10);
  assert(part >= 0 && part <= 1);
  ATermAppl sCol = MakeSortId("FloorCol");
  ATermAppl sPart = MakeSortId("FloorPosPart");
  ATermAppl fpSort;
  if (has_part) {
    fpSort = gsMakeSortArrow2(sCol, sPart, MakeSortId("FloorPos"));
  } else {
    fpSort = gsMakeSortArrow(sCol, MakeSortId("FloorPos"));
  }
  ATermAppl oPos = MakeOpId("pos_r" + intToString(row), fpSort);
  ATermAppl tCol = MakeOpId("c" + intToString(col), sCol);
  if (has_part) {
    ATermAppl tPart = MakeOpId((part == 0)?"pa":"pb", sPart);
    return gsMakeDataAppl2(oPos, tCol, tPart);
  } else {
    return gsMakeDataAppl(oPos, tCol);
  }
}

// --------------------------------------------
ATermAppl GarageFrame::MakeShuttlePos( int row,
                                       int part )
// --------------------------------------------
{
  assert(part >= 0 && part <= 1);
  assert(row >= 1 && row <= 3);
  return MakeOpId("r" + intToString(row) + ((part == 0)?"a":"b"),
    MakeSortId("ShuttlePos"));
}



// --------------------------------------------------
ATermAppl GarageFrame::MakeOpId( std::string name,
                                 ATermAppl SortExpr )
// --------------------------------------------------
{
  return gsMakeOpId(gsString2ATermAppl(name.c_str()), SortExpr);
}




// -------------------------------------------------------
ATermAppl GarageFrame::MakeDataVarId( std::string name,
                                      ATermAppl SortExpr )
// -------------------------------------------------------
{
  return gsMakeDataVarId(gsString2ATermAppl(name.c_str()), SortExpr);
}





// --------------------------------------------------
ATermAppl GarageFrame::MakeSortId( std::string name )
// --------------------------------------------------
{
  return gsMakeSortId(gsString2ATermAppl(name.c_str()));
}




// ---------------------------------------------
void GarageFrame::UpdateState( ATerm State )
// ---------------------------------------------
{
  if (stateIndex < 0 || simulator == NULL) {
    canvas->InitialiseCanvas();
  } else { //stateIndex >= 0 and simulator is initialised
    //get value of variable gs_hal
    NextState* nextState = simulator->GetNextState();
    ATermAppl gs_hal = nextState->getStateArgument(State, stateIndex);
    if (gsIsDataVarId(gs_hal)) {
      //gs_hal has a dummy value
      canvas->InitialiseCanvas();
    } else {
      //get the elements of gs_hal
      ATermList gs_hal_elts = gsGetDataExprArgs(gs_hal);

      Rewriter* rewriter = nextState->getRewriter();
  
      //update floor state
      ATermAppl sOccState = MakeSortId("OccState");
      ATermAppl tFree = MakeOpId("free", sOccState);
      ATermAppl tOccupied = MakeOpId("occupied", sOccState);
      ATermAppl fs = ATAelementAt(gs_hal_elts, 0);
      for (int i = 1; i <= 3; ++i) {
        for (int j = 1; j <= 10; ++j) {
          for (int k = 0; k <= 1; ++k) {
            ATermAppl fp = MakeFloorPos(i, j, i == 1, k);
            ATermAppl state = rewriter->rewrite(gsMakeDataAppl(fs, fp));
            if (ATisEqual(state, tFree)) {
              floorState[i-1][(j-1)*2+k] = 0;
            } else if (ATisEqual(state, tOccupied)) {
              floorState[i-1][(j-1)*2+k] = 1;
            } else {
              std::cerr 
                << "error: floor state of position "
                << PrintPart_CXX((ATerm) fp, ppDefault)
                << " cannot be shown because the term " 
                << PrintPart_CXX((ATerm) state, ppDefault)
                << " cannot be rewritten to normal form"
                << std::endl;
            } 
          }
        }
      }
  
      //update shuttle state
      ATermAppl tLowered =
        MakeOpId("lowered", MakeSortId("ShuttleOrientation"));
      ATermAppl tTilted = MakeOpId("tilted", MakeSortId("ShuttleOrientation"));
      ATermAppl tAvail = MakeOpId("avail", MakeSortId("AvailState"));
      ATermAppl tNAvail = MakeOpId("n_avail", MakeSortId("AvailState"));
      ATermAppl shs = ATAelementAt(gs_hal_elts, 1);
      for (int i = 1; i <= 3; ++i) {
        for (int j = 0; j <= 1; ++j) {
          ATermAppl sp = MakeShuttlePos(i, j);
          ATermAppl lstate = rewriter->rewrite(gsMakeDataAppl2(shs, sp, tLowered));
          if (ATisEqual(lstate, tNAvail)) {
            floorState[i-1][j*18]   = -1;
            floorState[i-1][j*18+1] = -1;
          } else if (!ATisEqual(lstate, tAvail)) {
            std::cerr
              << "error: state of lowered shuttle "
              << PrintPart_CXX((ATerm) sp, ppDefault)
              << " cannot be shown because the term "
              << PrintPart_CXX((ATerm) lstate, ppDefault)
              << " cannot be rewritten to normal form"
              << std::endl;
          }
          ATermAppl tstate = rewriter->rewrite(gsMakeDataAppl2(shs, sp, tTilted));
          if (ATisEqual(tstate, tAvail)) {
            shuttleState[i-1][j] = 1;
          } else if (ATisEqual(tstate, tNAvail)) {
            shuttleState[i-1][j] = 0;
          } else {
            std::cerr
              << "error: state of tilted shuttle "
              << PrintPart_CXX((ATerm) sp, ppDefault)
              << " cannot be shown because the term "
              << PrintPart_CXX((ATerm) tstate, ppDefault)
              << " cannot be rewritten to normal form"
              << std::endl;
          }
        }
      }
  
      //update lift state
      ATermAppl ls = ATAelementAt(gs_hal_elts, 2);
      ATermAppl lsSort = MakeSortId("LiftState");
      if (ATisEqual(ls, MakeOpId("lsf_street", lsSort))) {
        liftHeight = 0;
        liftOccupied = 0;
      } else if (ATisEqual(ls, MakeOpId("lso_street", lsSort))) {
        liftHeight = 0;
        liftOccupied = 1;
      } else if (ATisEqual(ls, MakeOpId("lsf_rotate", lsSort))) {
        liftHeight = -1;
        liftOccupied = 0;
      } else if (ATisEqual(ls, MakeOpId("lso_rotate", lsSort))) {
        liftHeight = -1;
        liftOccupied = 1;
      } else if (ATisEqual(ls, MakeOpId("ls_basement", lsSort))) {
        liftHeight = -2;
        liftOccupied = 0;
      } else {
        std::cerr
          << "error: lift state cannot be shown because the term "
          << PrintPart_CXX((ATerm) ls, ppDefault)
          << " is not in normal form"
          << std::endl;
      }
      if (liftHeight != -2) {
        floorState[0][11] = -1;
        floorState[0][12] = -1;
      }
  
      // Update canvas
      canvas->SetDataStructs(floorState, shuttleState, liftHeight, liftOccupied);
    }
  }
  //redraw
  canvas->Draw();
}

// ---------------------------------------------
void GarageFrame::OnCloseWindow( wxCloseEvent & )
// ---------------------------------------------
{
#ifdef GARAGEFRAMEDLL
  if (xsimdll != NULL) {
    xsimdll->Remove(this, true);
  }
#endif
  Destroy();
}

// ---------------------------------------------
void GarageFrame::OnMoveWindow( wxMoveEvent & )
// ---------------------------------------------
{
  //force a repaint of the canvas (hack)
  canvas->Update();
}

#ifdef GARAGEFRAMEDLL
static XSimViewsDLL *xsvdll;

extern "C" void SimulatorViewDLLAddView(SimulatorInterface *Simulator)
{
          GarageFrame *v;
          v = new GarageFrame(
            Simulator->MainWindow(), wxT("Garage State"),-1,-1,300,200);
          v->Show();
          v->SetXSimViewsDLL(xsvdll);
          xsvdll->Add(v,Simulator);
}

extern "C" __attribute__((constructor)) void SimulatorViewDLLInit()
{
          xsvdll = new XSimViewsDLL;
}

extern "C" __attribute__((destructor)) void SimulatorViewDLLCleanUp()
{
          delete xsvdll;
}
#endif
