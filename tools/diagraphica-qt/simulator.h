// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./simulator.h

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QtCore>
#include <QtGui>

#include <cstddef>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <wx/event.h>
#include <wx/timer.h>
#include "bundle.h"
#include "diagram.h"
#include "edge.h"
#include "glcanvas.h"
#include "graph.h"
#include "utils.h"
#include "visualizer.h"
#include "visutils.h"

class Simulator : public QObject, public Visualizer
{
  Q_OBJECT
  public:
    // -- constructors and destructor -------------------------------
    Simulator(
      Mediator* m,
      Graph* g,
      GLCanvas* c,
      QObject* parent = 0);
    virtual ~Simulator();


    // -- get functions ---------------------------------------------
    static QColor getColorClr() { return colClr; }
    static QColor getColorTxt() { return colTxt; }
    static int getSizeTxt() { return szeTxt; }
    static QColor getColorBdl() { return colBdl; }

    static int getBlendType() { return blendType; }

    QColor getColorSel() { return VisUtils::coolGreen; }
    size_t getIdxClstSel();


    // -- set functions ---------------------------------------------
    static void setColorClr(QColor col) { colClr = col; }
    static void setColorTxt(QColor col) { colTxt = col; }
    static void setSizeTxt(const int& sze) { szeTxt = sze; }
    static void setColorBdl(QColor col) { colBdl = col; }

    static void setBlendType(const int& type) { blendType = type; }

    void setDiagram(Diagram* dgrm);
    void initFrameCurr(
      Cluster* frame,
      const std::vector< Attribute* > &attrs);
    void updateFrameCurr(
      Cluster* frame,
      const Position2D& pos);
    void clearData();

    void handleSendDgrmSglToExnr();

    // -- visualization functions  ----------------------------------
    void visualize(const bool& inSelectMode);

    // -- event handlers --------------------------------------------
    void handleMouseLftDownEvent(
      const int& x,
      const int& y);
    void handleMouseLftUpEvent(
      const int& x,
      const int& y);
    void handleMouseLftDClickEvent(
      const int& x,
      const int& y);
    void handleMouseRgtDownEvent(
      const int& x,
      const int& y);
    void handleMouseRgtUpEvent(
      const int& x,
      const int& y);
    void handleMouseMotionEvent(
      const int& x,
      const int& y);
    void handleMouseLeaveEvent();

    void handleKeyDownEvent(const int& keyCode);
    /*
    void handleMarkFrameClust(
        DiagramChooser* dc,
        const int &idx );
    */

  public slots:

    // -- utility event handlers ------------------------------------
    void onTimer();

  protected:
    // -- utility functions -----------------------------------------
    void initAttributes(const std::vector< Attribute* > &attrs);
    void initFramesPrevNext();
    void initBundles();
    void sortFramesPrevNext();

    void calcSettingsGeomBased();
    void calcSettingsDataBased();
    void calcIntervals();
    void calcPositions();
    void calcPosFrames();
    void calcPosBundles();

    void handleKeyUp();
    void handleKeyRgt();
    void handleKeyDwn();
    void handleKeyLft();
    void markFrameClusts();

    void clearAttributes();
    void clearDiagram();
    void clearFrames();
    void clearBundles();

    // -- hit detection ---------------------------------------------
    void handleHits(const std::vector< int > &ids);
    void processHits(
      GLint hits,
      GLuint buffer[]);

    // -- utility drawing functions ---------------------------------
    void clear();
    QColor calcColor(size_t iter, size_t numr);

    void drawFrameCurr(const bool& inSelectMode);
    void drawFramesPrev(const bool& inSelectMode);
    void drawFramesNext(const bool& inSelectMode);
    void drawBdlLblGridPrev(const bool& inSelectMode);
    void drawBdlLblGridNext(const bool& inSelectMode);
    void drawBundlesPrev(const bool& inSelectMode);
    void drawBundlesNext(const bool& inSelectMode);
    void drawControls(const bool& inSelectMode);
    void animate();

    // -- static variables ------------------------------------------

    static QColor colClr;
    static QColor colTxt;
    static int      szeTxt;
    static QColor colBdl;

    enum
    {
      ID_CANVAS,
      ID_ICON_CLEAR,
      ID_ICON_UP,
      ID_ICON_NEXT,
      ID_ICON_DOWN,
      ID_ICON_PREV,
      ID_FRAME_CURR,
      ID_FRAME_PREV,
      ID_FRAME_NEXT,
      ID_BUNDLE_PREV,
      ID_BUNDLE_NEXT,
      ID_BUNDLE_LBL,
      ID_TIMER,
      ANIM_NONE,
      ANIM_POS,
      ANIM_BLEND,
      ID_CLEAR,
      ID_DIAGRAM_MORE
    };

    // -- data members ----------------------------------------------
    Diagram* diagram;                // association
    std::vector< Attribute* > attributes; // association

    Cluster* frameCurr;            // composition
    std::vector< Cluster* > framesPrev; // composition
    std::vector< Cluster* > framesNext; // composition
    std::vector< Bundle* >  bundles;    // composition

    std::vector< Bundle* >  bundlesByLbl;
    std::vector< Bundle* >  bundlesPrevByLbl;
    std::vector< Bundle* >  bundlesNextByLbl;

    static int itvLblPixVert;
    double scaleDgrmHori;
    double scaleDgrmVert;

    int focusDepthIdx;
    int focusFrameIdx;
    int focusDepthIdxLast;
    int focusFrameIdxPrevLast;
    int focusFrameIdxNextLast;

    size_t fcsLblPrevIdx;
    size_t fcsLblNextIdx;

    Position2D posFrameCurr;
    std::vector< Position2D > posFramesPrev;
    std::vector< Position2D > posFramesNext;

    std::vector< Position2D > posBdlLblGridPrevTopLft;
    std::vector< Position2D > posBdlLblGridPrevBotRgt;
    std::vector< Position2D > posBdlLblGridNextTopLft;
    std::vector< Position2D > posBdlLblGridNextBotRgt;

    std::vector< std::vector< Position2D > > posBundlesPrevTopLft;
    std::vector< std::vector< Position2D > > posBundlesPrevBotRgt;
    std::vector< std::vector< Position2D > > posBundlesNextTopLft;
    std::vector< std::vector< Position2D > > posBundlesNextBotRgt;

    // animation
    static int itvTmrMS;
    static double pixPerMS;
    double timeTotalMS;
    double timeAlphaMS;
    int animPhase;

    QTimer timerAnim;

    Cluster* keyFrameFr;
    Cluster* keyFrameTo;
    Position2D posKeyFrameFr;
    Position2D posKeyFrameTo;
    Position2D posTweenFrame;

    static int blendType;
    double opacityKeyFrameFr;
    double opacityKeyFrameTo;

};

#endif

// -- end -----------------------------------------------------------
