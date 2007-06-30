//  Copyright 2007 A.j. (Hannes) pretorius. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./frame.h

// --- frame.h ------------------------------------------------------
// (c) 2007  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------

#ifndef FRAME_H
#define FRAME_H

#include <cstddef>
#include <string>
using namespace std;
#include <wx/wx.h>
#include <wx/dataobj.h>
#include <wx/dnd.h>
#include <wx/listctrl.h>
#include <wx/toolbar.h>
#include <wx/splitter.h>
#include "attribute.h"
#include "bitmappanel.h"
#include "colleague.h"
#include "dof.h"
#include "droptarget.h"
#include "glcanvas.h"
#include "partitionframe.h"
#include "popupframe.h"
#include "settingsframe.h"
#include "utils.h"

class Frame : public Colleague, public wxFrame
{
public:
    // -- constructors and destructors ------------------------------
    Frame( 
        Mediator* m,
        wxString title );
    virtual ~Frame();

    // -- set functions ---------------------------------------------
    void setTitleText( const string &msg );
    void setStatusText( const string &msg );
    void setOutputText( const string &msg );
    void appOutputText( const string &msg );
    
    void setFileOptionsActive();
    void setEditModeSelect();
    void setEditModeDOF();

    // -- display functions -----------------------------------------
    void displNumNodes( const int &val );
    void displNumEdges( const int &val );
    void displAttrInfo( 
        const vector< int > &indices,
        const vector< string > &names,
        const vector< string > &types,
        const vector< int > &cards,
        const vector< string > &range );
    void displAttrInfo(
        const int &selectIdx,
        const vector< int > &indices,
        const vector< string > &names,
        const vector< string > &types,
        const vector< int > &cards,
        const vector< string > &range );
    void displDomainInfo( 
        const vector< int > &indices,
        const vector< string > &values,
        const vector< int > &number,
        const vector< double > &perc );
    void clearDomainInfo();
    
    void displShapeMenu(
        const bool &cut,
        const bool &copy,
        const bool &paste,
        const bool &clear,
        const bool &bringToFront, 
        const bool &sendToBack,
        const bool &bringForward, 
        const bool &sendBackward,
        const bool &editDOF );
    void displDgrmMenu(
        const bool &sendSglToSiml,
        const bool &sendSglToTrace,
        const bool &sendSetToTrace,
        const bool &sendSglToExnr,
        const bool &sentSetToExnr );
    
    void displDOFInfo(
        const vector< int > &degsOfFrdmIndcs,
        const vector< string > &degsOfFrdm,
        const vector< string > &attrNames,
        const int &selIdx );
    void clearDOFInfo();

    void displClustMenu();
    void displAttrInfoClust(
        const vector< int > &indices,
        const vector< string > &names );

    void displAttrInfoPart( 
        string attrName,
        int minParts,
        int maxParts,
        int curParts );

    void displSimClearDlg();
    void displExnrClearDlg();
    void displExnrFrameMenu( const bool &clear );

    void selectAttribute( const int &idx );
    void selectDomainVal( const int &idx );

    void handleDragDrop(
        const int &srcId,
        const int &tgtId,
        const int &tgtX,
        const int &tgtY,
        const vector< int > &data );

    void closePopupFrames();
    void handleCloseFrame( PopupFrame* f );

    // -- get functions ---------------------------------------------
    GLCanvas* getCanvasArcD();
    GLCanvas* getCanvasSiml();
    GLCanvas* getCanvasTrace();
    GLCanvas* getCanvasExnr();
    GLCanvas* getCanvasEdit();
    GLCanvas* getCanvasDistr();
    GLCanvas* getCanvasCorrl();
    GLCanvas* getCanvasCombn();
    GLCanvas* getCanvasColDOF();
    GLCanvas* getCanvasOpaDOF();
    
    // -- clear functions -------------------------------------------
    void clearOuput();

    // -- overloaded operators --------------------------------------
    void operator<<( const string &msg );

private:
    // -- GUI initialization ----------------------------------------
    void initFrame();           // 0
    void initIcon();            // 1
    void initMenuBar();         // 1
    void initSplitterFrame();   // 1
    
    void initSplitterLft();     // 2
    
    void initSplitterTopLft();  // 3
    
    void initPanelTopTopLft();  // 4
    void initLabelsGraphInfo(); // 5
    void initListCtrlAttr();    // 5
    void initButtonsAttr();     // 5

    void initPanelBotTopLft();  // 4
    void initListCtrlDomain();  // 5

    void initPanelBotLft();     // 3
    void initTextCtrl();        // 4
    
    void initSplitterRgt();     // 2
    void initSplitterTopRgt();  // 3
    
    void initPanelTopRgt();     // 3
    void initCanvasOne();       // 4
    void initToolbarEdit();     // 4
    
    void initSplitterBotRgt();  // 3

    void initPanelLftBotRgt();  // 4
    void initCanvasTwo();       // 5
    void initPanelRgtBotRgt();  // 4
    void initCanvasThree();     // 5

    void initAboutFrame();      // 1
    
    void initFrameSettings();
    void initFramePartition(
        wxString attrName,
        int minParts,
        int maxParts,
        int curParts );
    
    void initFrameDOF();
    void initPanelDOF();
    void initListCtrlDOF();
    void initTextOptionsDOF();
    void initCanvasColDOF();
    void initCanvasOpaDOF();
    
    void initFramePlot();
    void initPanelPlot();
    void initCanvasPlot();

    void initFrameClust();
    void initPanelClust();
    void initListCtrlClust();
    void initButtonsClust();
    
    // -- event handlers --------------------------------------------
    void onMenuBar( wxCommandEvent &e );
    void onListCtrlSelect( wxListEvent &e );
    void onListCtrlBeginDrag( wxListEvent &e );
    void onListCtrlRgtClick( wxListEvent &e );
    void onPopupMenu( wxCommandEvent &e );
    void onTool( wxCommandEvent &e );
    void onButton( wxCommandEvent &e );
    void onRadioBox( wxCommandEvent &e );
    
    // -- menu bar --------------------------------------------------
    wxMenuBar* menuBar;
    wxMenu*    fileMenu;
    wxMenu*    modeMenu;
    wxMenu*    viewMenu;
    wxMenu*    attributeMenu;
    wxMenu*    domainMenu;
    wxMenu*    settingsMenu;
    wxMenu*    helpMenu;

    // -- frame -----------------------------------------------------
    wxBoxSizer*       sizerFrame;
    wxSplitterWindow* splitterFrame;

    // -- left panel ------------------------------------------------
    wxSplitterWindow* splitterLft;

    wxSplitterWindow* splitterTopLft;
    
    wxBoxSizer*       sizerTopTopLft;
    wxScrolledWindow* panelTopTopLft;
    wxStaticText*     lblNumNodes;
    wxStaticText*     lblNumEdges;
    wxListCtrl*       listCtrlAttr;
    wxButton*         buttonClustAttr;
    wxButton*         buttonTraceAttr;
    
    wxBoxSizer*       sizerBotTopLft;
    wxScrolledWindow* panelBotTopLft;
    wxListCtrl*       listCtrlDomain;

    wxBoxSizer*       sizerBotLft;
    wxScrolledWindow* panelBotLft;
    GLCanvas*         canvasThree;
    
    // -- right panel -----------------------------------------------
    wxSplitterWindow* splitterRgt;
    double            sashRatioRgt;
    
    wxBoxSizer*       sizerTopRgt;
    wxScrolledWindow* panelTopRgt;
    GLCanvas*         canvasOne;
    wxToolBar*        toolBarEdit;

    wxSplitterWindow* splitterBotRgt;
    
    wxBoxSizer*       sizerLftBotRgt;
    wxScrolledWindow* panelLftBotRgt;
    GLCanvas*         canvasTwo;
    
    wxBoxSizer*       sizerRgtBotRgt;
    wxScrolledWindow* panelRgtBotRgt;
    wxTextCtrl*       textCtrl;
    
    // -- settings frame --------------------------------------------
    SettingsFrame*    frameSettings;

    // -- partition frame -------------------------------------------
    PartitionFrame*   framePartition;
    
    // -- DOF frame -------------------------------------------------
    PopupFrame*       frameDOF;
    wxBoxSizer*       sizerFrameDOF;
    wxScrolledWindow* panelDOF;
    wxBoxSizer*       sizerDOF;
    wxListCtrl*       listCtrlDOF;
    wxRadioBox*       radioBoxTextDOF;
    GLCanvas*         canvasColDOF;
    GLCanvas*         canvasOpaDOF;
    
    // -- Plot frame ------------------------------------------------
    PopupFrame*       framePlot;
    wxBoxSizer*       sizerFramePlot;
    wxScrolledWindow* panelPlot;
    wxBoxSizer*       sizerPlot;
    GLCanvas*         canvasPlot;

    // -- Cluster frame ---------------------------------------------
    PopupFrame*       frameClust;
    wxBoxSizer*       sizerFrameClust;
    wxScrolledWindow* panelClust;
    wxBoxSizer*       sizerClust;
    wxListCtrl*       listCtrlClust;
    wxButton*         buttonOKClust;

    // -- about frame -----------------------------------------------
    wxFrame*          frameAbout;
    
    // -- ID's ------------------------------------------------------
    enum
    {
        ID_MENU_ITEM_LOAD_CONFIG = wxID_HIGHEST,
        ID_MENU_ITEM_SAVE_CONFIG,
        ID_MENU_ITEM_LOAD_DIAGRAM,
        ID_MENU_ITEM_SAVE_DIAGRAM,
        ID_MENU_ITEM_MODE_ANALYSIS,
        ID_MENU_ITEM_MODE_EDIT,
        ID_MENU_ITEM_VIEW_SIM,
        ID_MENU_ITEM_VIEW_TRACE,
        ID_MENU_ITEM_SETTINGS_GENERAL,
        ID_MENU_ITEM_SETTINGS_CLUST_TREE,
        ID_MENU_ITEM_SETTINGS_BAR_TREE,
        ID_MENU_ITEM_SETTINGS_ARC_DIAGRAM,
        ID_MENU_ITEM_SETTINGS_SIMULATOR,
        ID_MENU_ITEM_SETTINGS_TRACE,
        ID_MENU_ITEM_SETTINGS_EDITOR,
        ID_SPLITTER_FRAME,
        ID_SPLITTER_LFT,
        ID_SPLITTER_TOP_LFT,
        ID_PANEL_TOP_TOP_LFT,
        ID_LBL_NUM_NODES,
        ID_LBL_NUM_EDGES,
        ID_LIST_CTRL_ATTR,
        ID_BUTTON_CLUST_ATTR,
        ID_BUTTON_TRACE_ATTR,
        ID_PANEL_BOT_TOP_LFT,
        ID_LIST_CTRL_DOMAIN,
        ID_PANEL_BOT_LFT,
        ID_TEXTCTRL,
        ID_MENU_ITEM_DOM_GROUP,
        ID_MENU_ITEM_DOM_UNGROUP,
        ID_MENU_ITEM_DOM_RENAME,
        ID_SPLITTER_RGT,
        ID_PANEL_TOP_RGT,
        ID_CANVAS_MAIN,
        ID_TOOL_BAR_EDIT,
        ID_TOOL_SELECT,
        ID_TOOL_DOF,
        ID_TOOL_RECT,
        ID_TOOL_ELLIPSE,             
        ID_TOOL_LINE,
        ID_TOOL_ARROW,
        ID_TOOL_DARROW,
        ID_TOOL_FILL_COL,
        ID_TOOL_LINE_COL,
        ID_TOOL_SHOW_GRID,
        ID_TOOL_SNAP_GRID,

        ID_SPLITTER_BOT_RGT,
        ID_PANEL_LFT_BOT_RGT,
        ID_CANVAS_LFT,
        ID_PANEL_RGT_BOT_RGT,
        ID_CANVAS_RGT,
        
        ID_FRAME_SETTINGS,
        ID_FRAME_PARTITION,
        ID_FRAME_DOF,
        ID_PANEL_DOF,
        ID_LIST_CTRL_DOF,
        ID_RADIO_BOX_TEXT_DOF,
        ID_CANVAS_COL_DOF,
        ID_CANVAS_OP_DOF,
        ID_MENU_ITEM_DOF_UNLINK,
        ID_FRAME_PLOT,
        ID_PANEL_PLOT,
        ID_CANVAS_PLOT,
        ID_MENU_ITEM_ATTR_DISTR_PLOT,
        ID_MENU_ITEM_ATTR_CORRL_PLOT,
        ID_MENU_ITEM_ATTR_COMBN_PLOT,
        ID_MENU_ITEM_ATTR_DUPL,
        ID_MENU_ITEM_ATTR_RENAME,
        ID_MENU_ITEM_ATTR_DELETE,
        ID_MENU_ITEM_ATTR_CLUST,
        ID_MENU_ITEM_ATTR_TRACE,
        ID_MENU_ITEM_ATTR_PARTITION,
        ID_MENU_ITEM_ATTR_DEPARTITION,
        ID_MENU_ITEM_CLUST_DISTR_PLOT,
        ID_MENU_ITEM_CLUST_CORRL_PLOT,
        ID_MENU_ITEM_CLUST_COMBN_PLOT,
        ID_MENU_ITEM_CLUST_SUBCLUST,
        ID_MENU_ITEM_CLUST_UNCLUST,
        ID_FRAME_CLUST,
        ID_PANEL_CLUST,
        ID_LIST_CTRL_CLUST,
        ID_BUTTON_OK_CLUST,
        ID_MENU_ITEM_SHAPE_CUT,
        ID_MENU_ITEM_SHAPE_COPY,
        ID_MENU_ITEM_SHAPE_PASTE,
        ID_MENU_ITEM_SHAPE_DELETE,
        ID_MENU_ITEM_SHAPE_BRING_TO_FRONT,
        ID_MENU_ITEM_SHAPE_SEND_TO_BACK,
        ID_MENU_ITEM_SHAPE_BRING_FORWARD,
        ID_MENU_ITEM_SHAPE_SEND_BACKWARD,
        ID_MENU_ITEM_SHAPE_EDIT_DOF,
        ID_MENU_ITEM_DGRM_SGL_TO_SIML,
        ID_MENU_ITEM_DGRM_SGL_TO_TRACE,
        ID_MENU_ITEM_DGRM_SET_TO_TRACE,
        ID_MENU_ITEM_DGRM_SGL_TO_EXNR,
        ID_MENU_ITEM_DGRM_SET_TO_EXNR,
        ID_MENU_ITEM_EXNR_CLEAR,

        ID_BUTTON_ABOUT,
    };

    // -- declare event table ---------------------------------------
    DECLARE_EVENT_TABLE()
};

#endif

// -- end -----------------------------------------------------------
