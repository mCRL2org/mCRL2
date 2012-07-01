// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./frame.h

#ifndef FRAME_H
#define FRAME_H

#include <QtCore>
#include <QtGui>

#include <cstddef>
#include <string>
#include <vector>
#include <wx/wx.h>
#include <wx/dataobj.h>
#include <wx/dnd.h>
#include <wx/listctrl.h>
#include <wx/toolbar.h>
#include <wx/splitter.h>
#include "attribute.h"
#include "colleague.h"
#include "dof.h"
#include "droptarget.h"
#include "glcanvas.h"
#include "graph.h"
#include "popupframe.h"
#include "settingsframe.h"
#include "utils.h"

class Frame : public wxFrame, public Colleague
{
  public:
    // -- constructors and destructors ------------------------------
    Frame(
      Mediator* m,
      wxString title);
    virtual ~Frame();

    // -- set functions ---------------------------------------------
    void setTitleText(const std::string& msg);
    void setStatusText(const std::string& msg);
    void setOutputText(const std::string& msg);
    void appOutputText(const std::string& msg);
    void enableEditMode(const bool& enable);

    void setFileOptionsActive();
    void setEditModeSelect();
    void setEditModeNote();
    void setEditModeDOF();

    void setDOFColorSelected();
    void setDOFOpacitySelected();

    // -- display functions -----------------------------------------
    void handleNote(const size_t& shapeId, const std::string& msg);
    void handleTextSize(const size_t& shapeId, const size_t& textSize);
    void displNumNodes(const size_t& val);
    void displNumEdges(const size_t& val);
    void displAttrInfo(
      const std::vector< size_t > &indices,
      const std::vector< std::string > &names,
      const std::vector< std::string > &types,
      const std::vector< size_t > &cards,
      const std::vector< std::string > &range);
    void displAttrInfo(
      const size_t& selectIdx,
      const std::vector< size_t > &indices,
      const std::vector< std::string > &names,
      const std::vector< std::string > &types,
      const std::vector< size_t > &cards,
      const std::vector< std::string > &range);
    void displDomainInfo(
      const std::vector< size_t > &indices,
      const std::vector< std::string > &values,
      const std::vector< size_t > &number,
      const std::vector< double > &perc);
    void clearDomainInfo();

    void displShapeMenu(
      const bool& cut,
      const bool& copy,
      const bool& paste,
      const bool& clear,
      const bool& bringToFront,
      const bool& sendToBack,
      const bool& bringForward,
      const bool& sendBackward,
      const bool& editDOF,
      const int&  checkedItemId);
    void displClusterMenu();
    void displDgrmMenu(
      const bool& sendSglToSiml,
      const bool& sendSglToTrace,
      const bool& sendSetToTrace,
      const bool& sendSglToExnr,
      const bool& sentSetToExnr);

    void displDOFInfo(
      const std::vector< size_t > &degsOfFrdmIndcs,
      const std::vector< std::string > &degsOfFrdm,
      const std::vector< std::string > &attrNames,
      const size_t& selIdx);
    void clearDOFInfo();

    void displClustMenu();
    void displAttrInfoClust(
      const std::vector< size_t > &indices,
      const std::vector< std::string > &names);

    void displAttrInfoPart(
      std::string attrName,
      size_t minParts,
      size_t maxParts,
      size_t curParts);

    void displSimClearDlg();
    void displExnrClearDlg();
    void displExnrFrameMenu(const bool& clear);

    void selectAttribute(const size_t& idx);
    void selectDomainVal(const size_t& idx);

    void handleDragDrop(
      const int& srcId,
      const int& tgtId,
      const int& tgtX,
      const int& tgtY,
      const std::vector< int > &data);

    void closePopupFrames();
    void handleCloseFrame(PopupFrame* f);

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
    void operator<<(const std::string& msg);

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

    void initPanelBotRgt();  // 4
    void initCanvasTwo();       // 5
    void initPanelRgtBotRgt();  // 4
    void initCanvasThree();     // 5

//    void initAboutFrameOld();   // 1

    void initFrameSettings();

    void initFrameDOF();
    void initFrameNote();
    void initFrameTextSize();
    void initPanelDOF();
    void initPanelNote();
    void initListCtrlDOF();
    void initTextOptionsDOF();
    void initCanvasColDOF();
    void initCanvasOpaDOF();
    void initNoteButtons();

    void initFramePlot();
    void initPanelPlot();
    void initCanvasPlot();

    void initFrameClust();
    void initPanelClust();
    void initListCtrlClust();
    void initButtonsClust();

    // -- event handlers --------------------------------------------
    void onMenuBar(wxCommandEvent& e);
    void onListCtrlSelect(wxListEvent& e);
    void onListCtrlBeginDrag(wxListEvent& e);
    void onListCtrlRgtClick(wxListEvent& e);
    void onPopupMenu(wxCommandEvent& e);
    void onClusterMenu(wxCommandEvent& e);
    void onTool(wxCommandEvent& e);
    void onButton(wxCommandEvent& e);
    void onRadioBox(wxCommandEvent& e);
    void onSplitterDoubleClick(wxSplitterEvent& e);

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

    // -- note -----------------------------------------------------
    size_t            currentShapeId;
    std::string       currentShapeNote;
    size_t          currentTextSize;

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
    double        sashRatioLft;

    wxBoxSizer*       sizerTopRgt;
    wxScrolledWindow* panelTopRgt;
    GLCanvas*         canvasOne;
    wxToolBar*        toolBarEdit;


    wxBoxSizer*       sizerBotRgt;
    wxScrolledWindow* panelBotRgt;
    GLCanvas*         canvasTwo;

    wxBoxSizer*       sizerRgtBotRgt;
    wxScrolledWindow* panelRgtBotRgt;
    wxTextCtrl*       textCtrl;

    // -- settings frame --------------------------------------------
    SettingsFrame*    frameSettings;

    // -- DOF frame -------------------------------------------------
    PopupFrame*       frameDOF;
    PopupFrame*       frameNote;
    PopupFrame*       frameTextSize;
    wxBoxSizer*       sizerFrameDOF;
    wxBoxSizer*       sizerFrameNote;
    wxScrolledWindow* panelDOF;
    wxScrolledWindow* panelNote;
    wxBoxSizer*       sizerDOF;
    wxBoxSizer*       sizerNote;
    wxBoxSizer*       sizerTextSize;
    wxTextCtrl*       noteText;
    wxComboBox*       textSizeBox;
    wxListCtrl*       listCtrlDOF;
    wxRadioBox*       radioBoxTextDOF;
    GLCanvas*         canvasColDOF;
    GLCanvas*         canvasOpaDOF;
    bool        dofMenu;

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
    wxFrame*  frameAbout;

    // -- menus -----------------------------------------------------
    wxMenu*  addAttributeMenu;
    wxMenu*  showVariableMenu;
    wxMenu*      clustMenu;

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
      ID_TOOL_NOTE,
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
      ID_PANEL_BOT_RGT,
      ID_CANVAS_RGT,

      ID_FRAME_SETTINGS,
      ID_FRAME_DOF,
      ID_FRAME_NOTE,
      ID_FRAME_TEXT_SIZE,
      ID_PANEL_DOF,
      ID_ADD_BUTTON_NOTE,
      ID_CLEAR_BUTTON_NOTE,
      ID_OK_BUTTON_TEXT_SIZE,
      ID_LIST_CTRL_DOF,
      ID_RADIO_BOX_TEXT_DOF,
      ID_CANVAS_COL_DOF,
      ID_CANVAS_OP_DOF,
      ID_MENU_ITEM_DOF_UNLINK,
      ID_MENU_ITEM_DOF_ATTRIBUTE_LIST,
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
      ID_MENU_ITEM_CLUST_DISTR_PLOT,
      ID_MENU_ITEM_CLUST_CORRL_PLOT,
      ID_MENU_ITEM_CLUST_COMBN_PLOT,
      ID_MENU_ITEM_CLUST_SUBCLUST,
      ID_MENU_ITEM_CLUST_UNCLUST,
      ID_FRAME_CLUST,
      ID_PANEL_CLUST,
      ID_LIST_CTRL_CLUST,
      ID_BUTTON_OK_CLUST,
      ID_MENU_ITEM_CLUSTER,
      ID_MENU_ITEM_SHOW_VARIABLES,
      ID_MENU_ITEM_SHOW_NOTE,
      ID_MENU_ITEM_TEXT_SIZE,
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

      ID_BUTTON_ABOUT
    };

    // -- declare event table ---------------------------------------
    DECLARE_EVENT_TABLE()
};

#endif

// -- end -----------------------------------------------------------
