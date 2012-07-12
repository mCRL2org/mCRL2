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

// This is the main application class and inherits from wxApp. It
// also serves as the mediator that controls the program.

#ifndef DIAGRAPH_H
#define DIAGRAPH_H

#include <QtCore>
#include <QtGui>

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
#include "settings.h"
#include "simulator.h"
#include "timeseries.h"

#include "mcrl2/utilities/wx_tool.h"
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"

class DiaGraph :  public mcrl2::utilities::wx::tool< DiaGraph,
  mcrl2::utilities::tools::input_tool > , public Mediator
{
    typedef mcrl2::utilities::wx::tool< DiaGraph,
            mcrl2::utilities::tools::input_tool > super;

  public:
    DiaGraph();

    // -- functions inherited from wxApp ----------------------------
    bool run();
    int OnExit();

    // -- load & save data ------------------------------------------
    void openFile(const std::string& path);
    void saveFile(const std::string& path);
    void handleLoadAttrConfig(const std::string& path);
    void handleSaveAttrConfig(const std::string& path);
    void handleLoadDiagram(const std::string& path);
    void handleSaveDiagram(const std::string& path);

    // -- general input & output ------------------------------------
    void initProgress(
      const std::string& title,
      const std::string& msg,
      const size_t& max);
    void updateProgress(const size_t& val);
    void closeProgress();

    void setOutputText(const std::string& msg);
    void setOutputText(const int& msg);
    void appOutputText(const std::string& msg);
    void appOutputText(const int& msg);
    void appOutputText(const size_t& msg);

    QColor getColor(QColor col);
    void handleCloseFrame(PopupFrame* f);

    // -- interaction with attributes & domains ---------------------
    void handleAttributeSel(const size_t& idx);
    void handleMoveAttr(
      const size_t& idxFr,
      const size_t& idxTo);
    void handleAttributeDuplicate(const std::vector< size_t > &indcs);
    /*
    void handleAttributeDelete( const std::vector< int > &indcs );
    */
    void handleAttributeDelete(const size_t& idx);
    void handleAttributeRename(
      const size_t& idx,
      const std::string& name);
    void handleAttributeCluster(const std::vector< size_t > &indcs);


    void handleMoveDomVal(
      const size_t& idxAttr,
      const size_t& idxFr,
      const size_t& idxTo);
    void handleDomainGroup(
      const size_t& attrIdx,
      const std::vector< int > domIndcs,
      const std::string& newValue);
    void handleDomainUngroup(const size_t& attrIdx);

    void getAttributeNames(
      const std::vector< size_t > &indcs,
      std::vector< wxString > &names);
    size_t getAttrSizeCurDomain(const size_t& idx);

    // -- attribute plots -------------------------------------------
    void handleAttributePlot(const size_t& idx);
    void handleAttributePlot(
      const size_t& idx1,
      const size_t& idx2);
    void handleAttributePlot(const std::vector< size_t > &indcs);
    void handlePlotFrameDestroy();

    void handleEditClust(Cluster* c);
    void handleClustFrameDisplay();
    void handleClustPlotFrameDisplay(const size_t& idx);
    void handleClustPlotFrameDisplay(
      const size_t& idx1,
      const size_t& idx2);
    void handleClustPlotFrameDisplay(const std::vector< size_t > &indcs);
    void setClustMode(const int& m);
    size_t getClustMode();

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
    void handleNote(const size_t& shapeId, const std::string& msg);
    void handleEditModeSelect();
    void handleEditModeNote();
    void handleEditModeDOF(Colleague* c);
    void handleEditModeRect();
    void handleEditModeEllipse();
    void handleEditModeLine();
    void handleEditModeArrow();
    void handleEditModeDArrow();
    void handleEditModeFillCol();
    void handleEditModeLineCol();
    void handleEditShowGrid(const bool& flag);
    void handleEditSnapGrid(const bool& flag);

    void handleEditShape(
      const bool& cut,
      const bool& copy,
      const bool& paste,
      const bool& clear,
      const bool& bringToFront,
      const bool& sendToBack,
      const bool& bringForward,
      const bool& sendBackward,
      const bool& editDOF,
      const int&  checkedItem);
    void handleShowVariable(const std::string& variable, const int& variableId);
    void handleShowNote(const std::string& variable, const size_t& shapeId);
    void handleAddText(std::string& variable, size_t& shapeId);
    void handleTextSize(size_t& textSize, size_t& shapeId);
    void handleSetTextSize(size_t& textSize, size_t& shapeId);
    void handleCutShape();
    void handleCopyShape();
    void handlePasteShape();
    void handleDeleteShape();
    void handleBringToFrontShape();
    void handleSendToBackShape();
    void handleBringForwardShape();
    void handleSendBackwardShape();
    void handleEditDOFShape();
    void handleSetDOF(const size_t& attrIdx);
    void handleCheckedVariable(const size_t& idDOF, const int& variableId);

    void handleEditDOF(
      const std::vector< size_t > &degsOfFrdmIds,
      const std::vector< std::string > &degsOfFrdm,
      const std::vector< size_t > &attrIndcs,
      const size_t& selIdx);
    void handleDOFSel(const size_t& DOFIdx);

    void setDOFColorSelected();
    void setDOFOpacitySelected();

    void handleSetDOFTextStatus(
      const size_t& DOFIdx,
      const int& status);
    size_t handleGetDOFTextStatus(const size_t& DOFIdx);

    void handleDOFColActivate();
    void handleDOFColDeactivate();
    void handleDOFColAdd(
      const double& hue,
      const double& y);
    void handleDOFColUpdate(
      const size_t& idx,
      const double& hue,
      const double& y);
    void handleDOFColClear(
      const size_t& idx);
    void handleDOFColSetValuesEdt(
      const std::vector< double > &hue,
      const std::vector< double > &y);

    void handleDOFOpaActivate();
    void handleDOFOpaDeactivate();
    void handleDOFOpaAdd(
      const double& opa,
      const double& y);
    void handleDOFOpaUpdate(
      const size_t& idx,
      const double& opa,
      const double& y);
    void handleDOFOpaClear(
      const size_t& idx);
    void handleDOFOpaSetValuesEdt(
      const std::vector< double > &opa,
      const std::vector< double > &y);

    void handleLinkDOFAttr(
      const size_t DOFIdx,
      const size_t attrIdx);
    void handleUnlinkDOFAttr(const size_t DOFIdx);
    void handleDOFFrameDestroy();
    void handleDOFDeselect();

    // -- simulator, time series & examiner -------------------------
    void initSimulator(
      Cluster* currFrame,
      const std::vector< Attribute* > &attrs);

    void initTimeSeries(const std::vector< size_t > attrIdcs);
    void markTimeSeries(
      Colleague* sender,
      Cluster* currFrame);
    void markTimeSeries(
      Colleague* sender,
      const std::vector< Cluster* > frames);

    void addToExaminer(
      Cluster* currFrame,
      const std::vector< Attribute* > &attrs);
    void addToExaminer(
      const std::vector< Cluster* > frames,
      const std::vector< Attribute* > &attrs);

    void handleShowClusterMenu();
    void handleSendDgrm(
      Colleague* sender,
      const bool& sendSglToSiml,
      const bool& sendSglToTrace,
      const bool& sendSetToTrace,
      const bool& sendSglToExnr,
      const bool& sendSetToExnr);
    void handleSendDgrmSglToSiml();
    void handleSendDgrmSglToTrace();
    void handleSendDgrmSetToTrace();
    void handleSendDgrmSglToExnr();
    void handleSendDgrmSetToExnr();

    void handleClearSim(Colleague* sender);
    void handleClearExnr(Colleague* sender);
    void handleClearExnrCur(Colleague* sender);

    //void handleAnimFrameBundl( Colleague* sender );
    void handleAnimFrameClust(Colleague* sender);

    void handleMarkFrameClust(Colleague* sender);
    void handleUnmarkFrameClusts(Colleague* sender);

    void handleShowFrame(
      Cluster* frame,
      const std::vector< Attribute* > &attrs,
      QColor col);
    void handleUnshowFrame();

    void getGridCoordinates(double& xLeft, double& xRight, double& yTop, double& yBottom);

    // -- visualization ---------------------------------------------
    void handlePaintEvent(GLCanvas* c);
    void handleSizeEvent(GLCanvas* c);

    void updateDependancies(GLCanvas* c);

    // -- input event handlers --------------------------------------
    void handleDragDrop(
      const int& srcWindowId,
      const int& tgtWindowId,
      const int& tgtX,
      const int& tgtY,
      const std::vector< int > &data);

    void handleMouseEvent(GLCanvas* c, QMouseEvent* e);
    void handleWheelEvent(GLCanvas* c, QWheelEvent* e);
    void handleMouseEnterEvent(GLCanvas* c);
    void handleMouseLeaveEvent(GLCanvas* c);
    void handleKeyEvent(GLCanvas* c, QKeyEvent* e);


    // -- overloaded operators --------------------------------------
    void operator<<(const std::string& msg);
    void operator<<(const int& msg);
    void operator<<(const size_t& msg);

  protected:

    Visualizer* currentVisualizer(GLCanvas* c);

    // -- protected functions inhereted from Mediator ---------------
    void initColleagues();
    void initVisualizers();
    void clearColleagues();

    void displAttributes();
    void displAttributes(const size_t& selAttrIdx);
    void displAttrDomain(const size_t& attrIdx);
    void clearAttrDomain();

  private:
    // -- data members ----------------------------------------------
    Graph*            graph;          // composition
    Frame*            frame;          // composition
    wxProgressDialog* progressDialog; // composition
    int               mode;
    int               view;
    bool              critSect;
    bool        clustered;

    Settings settings;

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


class DiaGraph_gui_tool: public mcrl2::utilities::mcrl2_gui_tool<DiaGraph>
{
  public:
    DiaGraph_gui_tool()
    {
      //m_gui_options["no-state"] = create_checkbox_widget();
    }
};

// declare wxApp
DECLARE_APP(DiaGraph_gui_tool)

#endif

// -- end -----------------------------------------------------------
