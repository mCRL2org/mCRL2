// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./mediator.h

// ------------------------------------------------------------------
// This is an ABSTRACT CLASS with a number of PURE VIRUAL FUNCTIONS.
// This serves as an INTERFACE that that should be implemented by
// subclasses. This allows for the implementation of a MEDIATOR
// design pattern where an instance of a subclass serves as the
// mediator or controller.
// ------------------------------------------------------------------

#ifndef MEDIATOR_H
#define MEDIATOR_H

#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <wx/wx.h>
#include "visutils.h"

class Attribute;
class Cluster;
class Colleague;
class DiagramChooser;
class GLCanvas;
class PopupFrame;

class Mediator
{
public:
	// -- destructor ------------------------------------------------
	virtual ~Mediator() {}

	// -- load & save data ------------------------------------------
	virtual void openFile( const std::string &path ) = 0;
	virtual void saveFile( const std::string &path ) = 0;
	virtual void handleLoadAttrConfig( const std::string &path ) = 0;
	virtual void handleSaveAttrConfig( const std::string &path ) = 0;
	virtual void handleLoadDiagram( const std::string &path ) = 0;
	virtual void handleSaveDiagram( const std::string &path ) = 0;

	// -- general input & output ------------------------------------
	virtual void initProgress(
		const std::string &title,
		const std::string &msg,
		const int &max ) = 0;
	virtual void updateProgress( const int &val ) = 0;
	virtual void closeProgress() = 0;

	virtual void setOutputText( const std::string &msg ) = 0;
	virtual void setOutputText( const int &val ) = 0;
	virtual void appOutputText( const std::string &msg ) = 0;
	virtual void appOutputText( const int &val ) = 0;

	virtual void getColor( ColorRGB &col ) = 0;
	virtual void handleCloseFrame( PopupFrame* f ) = 0;

	// -- interaction with attributes & domains ---------------------
	virtual void handleAttributeSel( const int &idx ) = 0;
	virtual void handleMoveAttr(
		const int &idxFr,
		const int &idxTo ) = 0;
	virtual void handleAttributeDuplicate( const std::vector< int > &indcs ) = 0;
	/*
	virtual void handleAttributeDelete( const std::vector< int > &indcs ) = 0;
	*/
	virtual void handleAttributeDelete( const int &idx ) = 0;
	virtual void handleAttributeRename(
		const int &idx,
		const std::string &name ) = 0;
	virtual void handleAttributeCluster( const std::vector< int > &indcs ) = 0;

	// -*- //
	virtual void handleAttrPartition( const int &attrIdx ) = 0;
	virtual void handleAttrPartition(
		const int &numParts,
		const int &method ) = 0;
	virtual void handleAttrDepartition( const int &attrIdx ) = 0;
	virtual void handleAttrPartitionCloseFrame() = 0;

	virtual void getAttrValues(
		const int &attrIdx,
		std::vector< double > &vals ) = 0;
	virtual void getAttrValues(
		const int &attrIdx,
		std::set< double > &vals ) = 0;

	// -*- //

	virtual void handleMoveDomVal(
		const int &idxAttr,
		const int &idxFr,
		const int &idxTo ) = 0;
	virtual void handleDomainGroup(
		const int &attrIdx,
		const std::vector< int > domIndcs,
		const std::string &newValue ) = 0;
	virtual void handleDomainUngroup( const int &attrIdx ) = 0;

	virtual void getAttributeNames(
		const std::vector< int > &indcs,
		std::vector< wxString > &names ) = 0;
	virtual int getAttributeType( const int &idx ) = 0;
	virtual int getAttrSizeCurDomain( const int &idx ) = 0;

	// -- attribute plots -------------------------------------------
	virtual void handleAttributePlot( const int &idx ) = 0;
	virtual void handleAttributePlot(
		const int &idx1,
		const int &idx2 ) = 0;
	virtual void handleAttributePlot( const std::vector< int > &indcs ) = 0;
	virtual void handlePlotFrameDestroy() = 0;

	virtual void handleEditClust( Cluster* c ) = 0;
	virtual void handleClustFrameDisplay() = 0;
	virtual void handleClustPlotFrameDisplay( const int &idx ) = 0;
	virtual void handleClustPlotFrameDisplay(
		const int &idx1,
		const int &idx2 ) = 0;
	virtual void handleClustPlotFrameDisplay( const std::vector< int > &indcs ) = 0;
	virtual void setClustMode( const int &m ) = 0;
	virtual int getClustMode() = 0;

	// -- global mode changes ---------------------------------------
	virtual void handleSetModeAnalysis() = 0;
	virtual void handleSetModeEdit() = 0;
	virtual int getMode() = 0;
	virtual void handleSetViewSim() = 0;
	virtual void handleSetViewTrace() = 0;
	virtual int getView() = 0;
	virtual bool getClustered() = 0;

	// -- diagram editor --------------------------------------------
	virtual void* getGraph() = 0;
	virtual void handleNote( const int &shapeId, const std::string &msg ) = 0;
	virtual void handleEditModeSelect() = 0;
	virtual void handleEditModeNote() = 0;
	virtual void handleEditModeDOF( Colleague* c ) = 0;
	virtual void handleEditModeRect() = 0;
	virtual void handleEditModeEllipse() = 0;
	virtual void handleEditModeLine() = 0;
	virtual void handleEditModeArrow() = 0;
	virtual void handleEditModeDArrow() = 0;
	virtual void handleEditModeFillCol() = 0;
	virtual void handleEditModeLineCol() = 0;
	virtual void handleEditShowGrid( const bool &flag ) = 0;
	virtual void handleEditSnapGrid( const bool &flag ) = 0;

	virtual void handleEditShape(
		const bool &cut,
		const bool &copy,
		const bool &paste,
		const bool &clear,
		const bool &bringToFront,
		const bool &sendToBack,
		const bool &bringForward,
		const bool &sendBackward,
		const bool &editDOF,
		const int  &checkedItem ) = 0;
	virtual void handleShowVariable( const std::string &variable, const int &variableId ) = 0;
	virtual void handleShowNote( const std::string &variable, const int &shapeId ) = 0;
	virtual void handleAddText( std::string &variable, int &shapeId ) = 0;
	virtual void handleTextSize( int &textSize, int &shapeId ) = 0;
	virtual void handleSetTextSize( int &textSize, int &shapeId ) = 0;
	virtual void handleCutShape() = 0;
	virtual void handleCopyShape() = 0;
	virtual void handlePasteShape() = 0;
	virtual void handleDeleteShape() = 0;
	virtual void handleBringToFrontShape() = 0;
	virtual void handleSendToBackShape() = 0;
	virtual void handleBringForwardShape() = 0;
	virtual void handleSendBackwardShape() = 0;
	virtual void handleEditDOFShape() = 0;
	virtual void handleSetDOF( const int &attrIdx ) = 0;
	virtual void handleCheckedVariable( const int &idDOF, const int &variableId ) = 0;

	virtual void handleEditDOF(
		const std::vector< int > &degsOfFrdmIds,
		const std::vector< std::string > &degsOfFrdm,
		const std::vector< int > &attrIndcs,
		const int &selIdx ) = 0;
	virtual void handleDOFSel( const int &DOFIdx ) = 0;

	virtual void setDOFColorSelected() = 0;
		virtual void setDOFOpacitySelected() = 0;

	virtual void handleSetDOFTextStatus(
		const int &DOFIdx,
		const int &status ) = 0;
	virtual int handleGetDOFTextStatus( const int &DOFIdx ) = 0;

	virtual void handleDOFColActivate() = 0;
	virtual void handleDOFColDeactivate() = 0;
	virtual void handleDOFColAdd(
		const double &hue,
		const double &y ) = 0;
	virtual void handleDOFColUpdate(
		const int &idx,
		const double &hue,
		const double &y ) = 0;
	virtual void handleDOFColClear(
		const int &idx ) = 0;
	virtual void handleDOFColSetValuesEdt(
		const std::vector< double > &hue,
		const std::vector< double > &y ) = 0;

	virtual void handleDOFOpaActivate() = 0;
	virtual void handleDOFOpaDeactivate() = 0;
	virtual void handleDOFOpaAdd(
		const double &hue,
		const double &y ) = 0;
	virtual void handleDOFOpaUpdate(
		const int &idx,
		const double &hue,
		const double &y ) = 0;
	virtual void handleDOFOpaClear(
		const int &idx ) = 0;
	virtual void handleDOFOpaSetValuesEdt(
		const std::vector< double > &hue,
		const std::vector< double > &y ) = 0;

	virtual void handleLinkDOFAttr(
		const int DOFIdx,
		const int attrIdx ) = 0;
	virtual void handleUnlinkDOFAttr( const int DOFIdx ) = 0;
	virtual void handleDOFFrameDestroy() = 0;
	virtual void handleDOFDeselect() = 0;

	// -- simulator, time series & examiner -------------------------
	virtual void initSimulator(
		Cluster* currFrame,
		const std::vector< Attribute* > &attrs ) = 0;

	virtual void initTimeSeries( const std::vector< int > attrIdcs ) = 0;
	virtual void markTimeSeries(
		Colleague* sender,
		Cluster* currFrame ) = 0;
	virtual void markTimeSeries(
		Colleague* sender,
		const std::vector< Cluster* > frames ) = 0;

	virtual void addToExaminer(
		Cluster* currFrame,
		const std::vector< Attribute* > &attrs ) = 0;
	virtual void addToExaminer(
		const std::vector< Cluster* > frames,
		const std::vector< Attribute* > &attrs ) = 0;

	virtual void handleShowClusterMenu() = 0;
	virtual void handleSendDgrm(
		Colleague* sender,
		const bool &sendSglToSiml,
		const bool &sendSglToTrace,
		const bool &sendSetToTrace,
		const bool &sendSglToExnr,
		const bool &sendSetToExnr ) = 0;
	virtual void handleSendDgrmSglToSiml() = 0;
	virtual void handleSendDgrmSglToTrace() = 0;
	virtual void handleSendDgrmSetToTrace() = 0;
	virtual void handleSendDgrmSglToExnr() = 0;
	virtual void handleSendDgrmSetToExnr() = 0;

	virtual void handleClearSim( Colleague* sender ) = 0;
	virtual void handleClearExnr( Colleague* sender ) = 0;
	virtual void handleClearExnrCur( Colleague* sender ) = 0;

	//    virtual void handleAnimFrameBundl( Colleague* sender ) = 0;
	virtual void handleAnimFrameClust( Colleague* sender ) = 0;

	virtual void handleMarkFrameClust( Colleague* sender ) = 0;
	virtual void handleUnmarkFrameClusts( Colleague* sender ) = 0;

	virtual void handleShowFrame(
		Cluster* frame,
		const std::vector< Attribute* > &attrs,
		ColorRGB &col ) = 0;
	virtual void handleUnshowFrame() = 0;

	// -- visualization std::settings ------------------------------------
	virtual void setSettingsGeneral(
		const wxColour &colClr,
		const wxColour &colTxt,
		const int &szeTxt,
		const double &itvAnim ) = 0;
	virtual void setSettingsClustTree(
		const bool &show,
		const bool &annotate,
		const int &colMap ) = 0;
	virtual void setSettingsBarTree(
		const bool &show,
		const double &magn ) = 0;
	virtual void setSettingsSimulator( const int &blendType ) = 0;
	virtual void setSettingsTrace( const bool &useShading ) = 0;
	virtual void setSettingsArcDiagram(
		const bool &showNodes,
		const bool &showArcs,
		const wxColour &colArcs,
		const double &trspArcs ) = 0;

	virtual void getSettingsGeneral(
		wxColour &colClr,
		wxColour &colTxt,
		int &szeTxt,
		double &itvAnim ) = 0;
	virtual void getSettingsClustTree(
		bool &show,
		bool &annotate,
		int &colMap ) = 0;
	virtual void getSettingsBarTree(
		bool &show,
		double &magn ) = 0;
	virtual void getSettingsSimulator( int &blendType ) = 0;
	virtual void getSettingsTrace( bool &useShading ) = 0;
	virtual void getSettingsArcDiagram(
		bool &showNodes,
		bool &showArcs,
		wxColour &colArcs,
		double &trspArcs ) = 0;
	virtual void getGridCoordinates( double &xLeft, double &xRight, double &yTop, double &yBottom) = 0;

	// -- visualization ---------------------------------------------
	virtual void handlePaintEvent( GLCanvas* c ) = 0;
	virtual void handleSizeEvent( GLCanvas* c ) = 0;

	virtual void updateDependancies( GLCanvas* c ) = 0;

	// -- input event handlers --------------------------------------
	virtual void handleDragDrop(
		const int &srcWindowId,
		const int &tgtWindowId,
		const int &tgtX,
		const int &tgtY,
		const std::vector< int > &data ) = 0;

	virtual void handleMouseLftDownEvent(
		GLCanvas* c,
		const int &x,
		const int &y ) = 0;
	virtual void handleMouseLftUpEvent(
			GLCanvas* c,
			const int &x,
			const int &y ) = 0;
	virtual void handleMouseLftDClickEvent(
		GLCanvas* c,
		const int &x,
		const int &y ) = 0;
		virtual void handleMouseRgtDownEvent(
		GLCanvas* c,
		const int &x,
		const int &y ) = 0;
	virtual void handleMouseRgtUpEvent(
			GLCanvas* c,
			const int &x,
			const int &y ) = 0;
	virtual void handleMouseRgtDClickEvent(
		GLCanvas* c,
		const int &x,
		const int &y ) = 0;
		virtual void handleMouseMotionEvent(
			GLCanvas* c,
			const int &x,
			const int &y ) = 0;
		virtual void handleMouseWheelIncEvent(
		GLCanvas* c,
		const int &x,
		const int &y ) = 0;
	virtual void handleMouseWheelDecEvent(
		GLCanvas* c,
		const int &x,
		const int &y ) = 0;
	virtual void handleMouseEnterEvent( GLCanvas* c ) = 0;
	virtual void handleMouseLeaveEvent( GLCanvas* c ) = 0;
	virtual void handleKeyDownEvent(
		GLCanvas* c,
		const int &keyCode,
		const int &specialKey ) = 0;
	virtual void handleKeyUpEvent(
		GLCanvas* c,
		const int &keyCode,
		const int &specialKey ) = 0;

	// -- overloaded operators --------------------------------------
	virtual void operator<<( const std::string &msg ) = 0;
	virtual void operator<<( const int& msg ) = 0;

	// -- public constants ------------------------------------------
	enum
	{
		MODE_ANALYSIS,
		MODE_EDIT,
		VIEW_SIM,
		VIEW_TRACE,
		CLUST_DISTR_PLOT,
		CLUST_CORRL_PLOT,
		CLUST_COMBN_PLOT
	};

protected:
    // -- init colleagues -------------------------------------------
    virtual void initColleagues() = 0;
};

#endif

// -- end -----------------------------------------------------------
