// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./diagraph.h

// --- diagraph.h ---------------------------------------------------
// (c) 2007  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------

// ------------------------------------------------------------------
// This is the main application class and inherits from wxApp. It
// also serves as the mediator that controls the program.
// ------------------------------------------------------------------

#ifndef DIAGRAPH_H
#define DIAGRAPH_H

#include <cstddef>
#include <string>
#include <vector>
#include <set>
#include <wx/wx.h>
#include <wx/busyinfo.h>
#include <wx/cmdline.h>
#include <wx/colordlg.h>
#include <wx/progdlg.h>
#include "attribute.h"
#include "arcdiagram.h"
#include "colorchooser.h"
#include "combnplot.h"
#include "corrlplot.h"
#include "diagrameditor.h"
#include "distrplot.h"
#include "examiner.h"
#include "frame.h"
#include "graph.h"
#include "mediator.h"
#include "opacitychooser.h"
#include "parser.h"
#include "simulator.h"
#include "timeseries.h"

class DiaGraph : public mcrl2::utilities::wx::tool< DiaGraph >, public Mediator
{
  friend class mcrl2::utilities::wx::tool< DiaGraph >;

public:
        DiaGraph();

	// -- functions inherited from wxApp ----------------------------
	bool DoInit();
	int OnExit();
	
	// -- load & save data ------------------------------------------
	void openFile( const std::string &path );
	void saveFile( const std::string &path );
	void handleLoadAttrConfig( const std::string &path );
	void handleSaveAttrConfig( const std::string &path );
	void handleLoadDiagram( const std::string &path );
	void handleSaveDiagram( const std::string &path );
	
	// -- general input & output ------------------------------------
	void initProgress(
	const std::string &title,
	const std::string &msg,
	const int &max );
	void updateProgress( const int &val );
	void closeProgress();
	
	void setOutputText( const std::string &msg );
	void setOutputText( const int &msg );
	void appOutputText( const std::string &msg );
	void appOutputText( const int &msg );
	
	void getColor( ColorRGB &col );
	void handleCloseFrame( PopupFrame* f );
	
	// -- interaction with attributes & domains ---------------------
	void handleAttributeSel( const int &idx );
	void handleMoveAttr( 
	const int &idxFr,
	const int &idxTo );
	void handleAttributeDuplicate( const std::vector< int > &indcs );
	/*
	void handleAttributeDelete( const std::vector< int > &indcs );
	*/
	void handleAttributeDelete( const int &idx );
	void handleAttributeRename( 
	const int &idx,
	const std::string &name );
	void handleAttributeCluster( const std::vector< int > &indcs );
	
	// -*- //
	void handleAttrPartition( const int &attrIdx );
	void handleAttrPartition(
	const int &numParts,
	const int &method );
	void handleAttrDepartition( const int &attrIdx );
	void handleAttrPartitionCloseFrame();
	
	void getAttrValues(
	const int &attrIdx,
	std::vector< double > &vals );
	void getAttrValues(
	const int &attrIdx,
	std::set< double > &vals );
	// -*- //
	
	void handleMoveDomVal(
	const int &idxAttr,
	const int &idxFr,
	const int &idxTo );
	void handleDomainGroup(
	const int &attrIdx,
	const std::vector< int > domIndcs,
	const std::string &newValue );
	void handleDomainUngroup( const int &attrIdx );
	
	void getAttributeNames( 
	const std::vector< int > &indcs,
	std::vector< wxString > &names );
	int getAttributeType( const int &idx );
	int getAttrSizeCurDomain( const int &idx );
	
	// -- attribute plots -------------------------------------------
	void handleAttributePlot( const int &idx );
	void handleAttributePlot( 
	const int &idx1,
	const int &idx2 );
	void handleAttributePlot( const std::vector< int > &indcs );
	void handlePlotFrameDestroy();
	
	void handleEditClust( Cluster* c );
	void handleClustFrameDisplay();
	void handleClustPlotFrameDisplay( const int &idx );
	void handleClustPlotFrameDisplay( 
	const int &idx1,
	const int &idx2 );
	void handleClustPlotFrameDisplay( const std::vector< int > &indcs );
	void setClustMode( const int &m );
	int getClustMode();
	
	// -- global mode changes ---------------------------------------
	void handleSetModeAnalysis();
	void handleSetModeEdit();
	int getMode();
	void handleSetViewSim();
	void handleSetViewTrace();
	int getView();
	bool getClustered();
	
	// -- diagram editor --------------------------------------------
	
	void* getGraph();
	void handleNote( const int &shapeId, const std::string &msg );
	void handleEditModeSelect();
	void handleEditModeNote();
	void handleEditModeDOF( Colleague* c );
	void handleEditModeRect();
	void handleEditModeEllipse();
	void handleEditModeLine();
	void handleEditModeArrow();
	void handleEditModeDArrow();
	void handleEditModeFillCol();
	void handleEditModeLineCol();
	void handleEditShowGrid( const bool &flag );
	void handleEditSnapGrid( const bool &flag );
	
	void handleEditShape(
	const bool &cut,
	const bool &copy,
	const bool &paste,
	const bool &clear,
	const bool &bringToFront, 
	const bool &sendToBack,
	const bool &bringForward, 
	const bool &sendBackward,
	const bool &editDOF,
	const int  &checkedItem );
	void handleShowVariable( const std::string &variable, const int &variableId );
	void handleShowNote( const std::string &variable, const int &shapeId );
	void handleAddText( std::string &variable, int &shapeId );
	void handleTextSize( int &textSize, int &shapeId );
	void handleSetTextSize( int &textSize, int &shapeId );
	void handleCutShape();
	void handleCopyShape();
	void handlePasteShape();
	void handleDeleteShape();
	void handleBringToFrontShape();
	void handleSendToBackShape();
	void handleBringForwardShape();
	void handleSendBackwardShape();
	void handleEditDOFShape();
	void handleSetDOF( const int &attrIdx );
	void handleCheckedVariable( const int &idDOF, const int &variableId );
	
	void handleEditDOF( 
	const std::vector< int > &degsOfFrdmIds,
	const std::vector< std::string > &degsOfFrdm,
	const std::vector< int > &attrIndcs,
	const int &selIdx );
	void handleDOFSel( const int &DOFIdx );
	
	void setDOFColorSelected();
	void setDOFOpacitySelected();
	
	void handleSetDOFTextStatus( 
	const int &DOFIdx,
	const int &status );
	int handleGetDOFTextStatus( const int &DOFIdx );
	
	void handleDOFColActivate();
	void handleDOFColDeactivate();
	void handleDOFColAdd(
	const double &hue,
	const double &y );
	void handleDOFColUpdate(
	const int &idx,
	const double &hue,
	const double &y );
	void handleDOFColClear(
	const int &idx );
	void handleDOFColSetValuesEdt(
	const std::vector< double > &hue,
	const std::vector< double > &y );
	
	void handleDOFOpaActivate();
	void handleDOFOpaDeactivate();
	void handleDOFOpaAdd(
	const double &opa,
	const double &y );
	void handleDOFOpaUpdate(
	const int &idx,
	const double &opa,
	const double &y );
	void handleDOFOpaClear(
	const int &idx );
	void handleDOFOpaSetValuesEdt(
	const std::vector< double > &opa,
	const std::vector< double > &y );
	
	void handleLinkDOFAttr(
	const int DOFIdx,
	const int attrIdx );
	void handleUnlinkDOFAttr( const int DOFIdx );
	void handleDOFFrameDestroy();
	void handleDOFDeselect();
	
	// -- simulator, time series & examiner -------------------------
	void initSimulator(
	Cluster* currFrame,
	const std::vector< Attribute* > &attrs );
	
	void initTimeSeries( const std::vector< int > attrIdcs );
	void markTimeSeries( 
	Colleague* sender,
	Cluster* currFrame );
	void markTimeSeries( 
	Colleague* sender,
	const std::vector< Cluster* > frames );
	
	void addToExaminer(
	Cluster* currFrame,
	const std::vector< Attribute* > &attrs );
	void addToExaminer(
	const std::vector< Cluster* > frames,
	const std::vector< Attribute* > &attrs );
	
	void handleShowClusterMenu();
	void handleSendDgrm(
	Colleague* sender,
	const bool &sendSglToSiml,
	const bool &sendSglToTrace,
	const bool &sendSetToTrace,
	const bool &sendSglToExnr,
	const bool &sendSetToExnr );
	void handleSendDgrmSglToSiml();
	void handleSendDgrmSglToTrace();
	void handleSendDgrmSetToTrace();
	void handleSendDgrmSglToExnr();
	void handleSendDgrmSetToExnr();
	
	void handleClearSim( Colleague* sender );
	void handleClearExnr( Colleague* sender );
	void handleClearExnrCur( Colleague* sender );
	
	//void handleAnimFrameBundl( Colleague* sender );
	void handleAnimFrameClust( Colleague* sender );
	
	void handleMarkFrameClust( Colleague* sender );
	void handleUnmarkFrameClusts( Colleague* sender );
	
	void handleShowFrame(
	Cluster* frame,
	const std::vector< Attribute* > &attrs,
	ColorRGB &col );
	void handleUnshowFrame();
	
	// -- visualization settings ------------------------------------
	void setSettingsGeneral( 
	const wxColour &colClr,
	const wxColour &colTxt,
	const int &szeTxt,
	const double &spdAnim );
	void setSettingsClustTree( 
	const bool &show,
	const bool &annotate,
	const int &colMap );
	void setSettingsBarTree( 
	const bool &show,
	const double &magn );
	void setSettingsSimulator( const int &blendType );
	void setSettingsTrace( const bool &useShading );
	void setSettingsArcDiagram( 
	const bool &showNodes,
	const bool &showArcs,
	const wxColour &colArcs,
	const double &trspArcs );
	
	void getSettingsGeneral( 
	wxColour &colClr,
	wxColour &colTxt,
	int &szeTxt,
	double &spdAnim );
	void getSettingsClustTree(
	bool &show,
	bool &annotate,
	int &colMap );
	void getSettingsBarTree( 
	bool &show,
	double &magn );
	void getSettingsSimulator( int &blendType );
	void getSettingsTrace( bool &useShading );
	void getSettingsArcDiagram( 
	bool &showNodes,
	bool &showArcs,
	wxColour &colArcs,
	double &trspArcs );
	void getGridCoordinates( double &xLeft, double &xRight, double &yTop, double &yBottom);
	
	// -- visualization ---------------------------------------------
	void handlePaintEvent( GLCanvas* c );
	void handleSizeEvent( GLCanvas* c );
	
	void updateDependancies( GLCanvas* c );
	
	// -- input event handlers --------------------------------------
	void handleDragDrop(
	const int &srcWindowId,
	const int &tgtWindowId,
	const int &tgtX,
	const int &tgtY,
	const std::vector< int > &data );
	
	void handleMouseLftDownEvent( 
	GLCanvas* c, 
	const int &x, 
	const int &y );
	void handleMouseLftUpEvent(
		GLCanvas* c,
		const int &x,
		const int &y );
	void handleMouseLftDClickEvent(
	GLCanvas* c,
	const int &x,
	const int &y );
	void handleMouseRgtDownEvent( 
	GLCanvas* c, 
	const int &x, 
	const int &y );
	void handleMouseRgtUpEvent(
		GLCanvas* c,
		const int &x,
		const int &y );
	void handleMouseRgtDClickEvent(
	GLCanvas* c,
	const int &x,
	const int &y );
	void handleMouseMotionEvent(
		GLCanvas* c,
		const int &x,
		const int &y );
	void handleMouseWheelIncEvent(
	GLCanvas* c,
	const int &x,
	const int &y );
	void handleMouseWheelDecEvent(
	GLCanvas* c,
	const int &x,
	const int &y );
	void handleMouseEnterEvent( GLCanvas* c );
	void handleMouseLeaveEvent( GLCanvas* c );
	void handleKeyDownEvent(
	GLCanvas* c,
	const int &keyCode,
	const int &specialKey );
	void handleKeyUpEvent(
	GLCanvas* c,
	const int &keyCode,
	const int &specialKey );
	
	// -- overloaded operators --------------------------------------
	void operator<<( const std::string &msg );
	void operator<<( const int &msg );

protected:
    // -- protected functions inhereted from Mediator ---------------
    void initColleagues();
    void initVisualizers();
    void clearColleagues();
    
    void displAttributes();
    void displAttributes( const int &selAttrIdx );
    void displAttrDomain( const int &attrIdx );
    void clearAttrDomain();

    bool parse_command_line(int argc, wxChar** argv);

private:
    // -- data members ----------------------------------------------
    Graph*            graph;          // composition
    Frame*            frame;          // composition
    wxProgressDialog* progressDialog; // composition
    int               mode;
    int               view;
    bool              critSect;
    bool			  clustered;
    
    // -- visualization ---------------------------------------------
    GLCanvas* canvasArcD;  // association
    ArcDiagram* arcDgrm;   // composition
    
    GLCanvas* canvasSiml;  // association
    Simulator* simulator;  // composition

    GLCanvas* canvasTrace; // association
    TimeSeries* timeSeries;// composition
    
    GLCanvas* canvasExnr;  // association
    Examiner* examiner;    // composition

    GLCanvas* canvasEdit;  // association
    DiagramEditor* editor; // composition

    GLCanvas* canvasDistr; // association
    DistrPlot* distrPlot;  // composition
    
    GLCanvas* canvasCorrl; // association
    CorrlPlot* corrlPlot;  // composition
    
    GLCanvas* canvasCombn; // association
    CombnPlot* combnPlot;  // composition

    GLCanvas* canvasColChooser; // association
    ColorChooser* colChooser;   // composition

    GLCanvas* canvasOpaChooser; // association
    OpacityChooser* opaChooser; // composition

    // -- sender of diagram -----------------------------------------
    Colleague* dgrmSender;

    // -- attribute plots -------------------------------------------
    Cluster* tempClust;
    int      clustMode;

    // -- attribute to partition ------------------------------------
    Attribute* tempAttr;
};

// declare wxApp
DECLARE_APP( DiaGraph )

#endif

// -- end -----------------------------------------------------------
