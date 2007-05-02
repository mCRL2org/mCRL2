// --- diagraph.h ---------------------------------------------------
// (c) 2006  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------

// ------------------------------------------------------------------
// This is the main application class and inherits from wxApp. It
// also serves as the mediator that controls the program.
// ------------------------------------------------------------------

#ifndef DIAGRAPH_H
#define DIAGRAPH_H

#include <cstddef>
#include <wx/wx.h>
#include <wx/busyinfo.h>
#include <wx/colordlg.h>
#include <wx/progdlg.h>
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

class DiaGraph : public wxApp, public Mediator
{
public:
    // -- functions inherited from wxApp ----------------------------
    virtual bool OnInit();
	virtual int OnExit();
    
    // -- load & save data ------------------------------------------
    void openFile( const string &path );
    void saveFile( const string &path );
    void handleLoadAttrConfig( const string &path );
    void handleSaveAttrConfig( const string &path );
    void handleLoadDiagram( const string &path );
    void handleSaveDiagram( const string &path );
    
    // -- general input & output ------------------------------------
    void initProgress(
        const string &title,
        const string &msg,
        const int &max );
    void updateProgress( const int &val );
    void closeProgress();
    
    void setOutputText( const string &msg );
    void setOutputText( const int &msg );
    void appOutputText( const string &msg );
    void appOutputText( const int &msg );

    void getColor( ColorRGB &col );
    void handleCloseFrame( PopupFrame* f );
    
    // -- interaction with attributes & domains ---------------------
    void handleAttributeSel( const int &idx );
    void handleMoveAttr( 
        const int &idxFr,
        const int &idxTo );
    void handleAttributeDuplicate( const vector< int > &indcs );
    void handleAttributeDelete( const vector< int > &indcs );
    void handleAttributeRename( 
        const int &idx,
        const string &name );
    void handleAttributeCluster( const vector< int > &indcs );
    
    void handleMoveDomVal(
        const int &idxAttr,
        const int &idxFr,
        const int &idxTo );
    void handleDomainGroup(
        const int &attrIdx,
        const vector< int > domIndcs,
        const string &newValue );
    void handleDomainUngroup( const int &attrIdx );

    void getAttributeNames( 
        const vector< int > &indcs,
        vector< wxString > &names );

    // -- attribute plots -------------------------------------------
    void handleAttributePlot( const int &idx );
    void handleAttributePlot( 
        const int &idx1,
        const int &idx2 );
    void handleAttributePlot( const vector< int > &indcs );
    void handlePlotFrameDestroy();
    
    void handleEditClust( Cluster* c );
    void handleClustFrameDisplay();
    void handleClustPlotFrameDisplay( const int &idx );
    void handleClustPlotFrameDisplay( 
        const int &idx1,
        const int &idx2 );
    void handleClustPlotFrameDisplay( const vector< int > &indcs );
    void setClustMode( const int &m );
    int getClustMode();

    // -- global mode changes ---------------------------------------
    void handleSetModeAnalysis();
    void handleSetModeEdit();
    int getMode();

    // -- diagram editor --------------------------------------------
    void handleEditModeSelect();
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
        const bool &editDOF );
    void handleCutShape();
    void handleCopyShape();
    void handlePasteShape();
    void handleDeleteShape();
    void handleBringToFrontShape();
    void handleSendToBackShape();
    void handleBringForwardShape();
    void handleSendBackwardShape();
    void handleEditDOFShape();
    
    void handleEditDOF( 
        const vector< int > &degsOfFrdmIds,
        const vector< string > &degsOfFrdm,
        const vector< int > &attrIndcs,
        const int &selIdx );
    void handleDOFSel( const int &DOFIdx );
    
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
        const vector< double > &hue,
        const vector< double > &y );
    
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
        const vector< double > &opa,
        const vector< double > &y );
    
    void handleLinkDOFAttr(
        const int DOFIdx,
        const int attrIdx );
    void handleUnlinkDOFAttr( const int DOFIdx );
    void handleDOFFrameDestroy();
    void handleDOFDeselect();

    // -- simulator & examiner --------------------------------------
    void initSimulator(
        Cluster* currFrame,
        const vector< Attribute* > &attrs );
    void addToExaminer(
        Cluster* currFrame,
        const vector< Attribute* > &attrs );
    void addToExaminer(
        const vector< Cluster* > frames,
        const vector< Attribute* > &attrs );

    void handleSendDgrm(
        Colleague* sender,
        const bool &sendSglToSiml,
        const bool &sendSglToExnr,
        const bool &sendSetToExnr );
    void handleSendDgrmSglToSiml();
    void handleSendDgrmSglToExnr();
    void handleSendDgrmSetToExnr();

    void handleClearSim( Colleague* sender );
    void handleClearExnr( Colleague* sender );
    void handleClearExnrCur( Colleague* sender );

    void handleMarkFrameClust( Colleague* sender );
    void handleUnmarkFrameClusts( Colleague* sender );

    void handleShowFrame(
        Cluster* frame,
        const vector< Attribute* > &attrs,
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
    void getSettingsArcDiagram( 
        bool &showNodes,
        bool &showArcs,
        wxColour &colArcs,
        double &trspArcs );

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
        const vector< int > &data );
    
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
        const int &keyCode );
    void handleKeyUpEvent(
        GLCanvas* c,
        const int &keyCode );

    // -- overloaded operators --------------------------------------
    void operator<<( const string &msg );
    void operator<<( const int &msg );

protected:
    // -- protected functions inhereted from Mediator ---------------
    void initColleagues();
    void initVisualizers();
    void clearColleagues();
    
    void displAttributes();
    void displAttrDomain( const int &attrIdx );
    void clearAttrDomain();

private:
    // -- data members ----------------------------------------------
    Graph*            graph;          // composition
    Frame*            frame;          // composition
    wxProgressDialog* progressDialog; // composition
    int               mode;

    // -*- //
    bool critSect;
    
    // -- visualization ---------------------------------------------
    GLCanvas* canvasArcD;  // association
    ArcDiagram* arcDgrm;   // composition
    
    GLCanvas* canvasSiml;  // association
    Simulator* simulator;  // composition
    
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
};

// declare wxApp
DECLARE_APP( DiaGraph )

#endif

// -- end -----------------------------------------------------------
