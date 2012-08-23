// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./examiner.h

#ifndef EXAMINER_H
#define EXAMINER_H

#include <QtCore>
#include <QtGui>

#include <cstddef>
#include <cstdlib>
#include <cmath>
#include <vector>
#include "bundle.h"
#include "diagram.h"
#include "edge.h"
#include "graph.h"
#include "settings.h"
#include "utils.h"
#include "visualizer.h"
#include "visutils.h"

class Examiner : public Visualizer
{
  Q_OBJECT

  public:
    // -- constructors and destructor -------------------------------
    Examiner(
      QWidget *parent,
      Settings* s,
      Graph* g);
    virtual ~Examiner();

    QColor selectionColor() { return VisUtils::coolRed; }
    size_t selectedClusterIndex();

    void setDiagram(Diagram* dgrm) { diagram = dgrm; update(); }
    void setFrame(
      Cluster* frme,
      const std::vector< Attribute* > &attrs,
      QColor col);
    void clrFrame();

    void addFrameHist(
      Cluster* frme,
      const std::vector< Attribute* > &attrs);
    void addFrameHist(
      QList<Cluster*> frames,
      const std::vector< Attribute* > &attrs);

    // -- visualization functions  ----------------------------------
    void visualize(const bool& inSelectMode);

    // -- event handlers --------------------------------------------
    void handleSizeEvent();

    void handleMouseEvent(QMouseEvent* e);
    void handleKeyEvent(QKeyEvent* e);

    QSize sizeHint() { return QSize(200,200); }

  protected slots:
    void clearData();
    void clrFrameHistCur();

  signals:
    void routingCluster(Cluster *cluster, QList<Cluster *> clusterSet, QList<Attribute *> attributes);
    void selectionChanged();

  protected:
    // -- utility functions -----------------------------------------
    /*
        void initAttributes( const std::vector< Attribute* > &attrs );
        void initFrames();
        void initBundles();
    */
    void calcSettingsGeomBased();
    void calcSettingsDataBased();
    void calcPosFrame();
    void calcPosFramesHist();

    void clearAttributes();
    void clearDiagram();
    void clearFrames();

    // -- hit detection ---------------------------------------------
    void handleHits(const std::vector< int > &ids);
    void handleIconRwnd();
    void handleIconLft();
    /*
    void handleIconPlay();
    */
    void handleIconRgt();
    void processHits(
      GLint hits,
      GLuint buffer[]);

    // -- utility drawing functions ---------------------------------
    void clear();

    void drawFrame(const bool& inSelectMode);
    void drawFramesHist(const bool& inSelectMode);
    void drawControls(const bool& inSelectMode);

    enum
    {
      ID_ICON_CLR,
      ID_ICON_MORE,
      ID_ICON_RWND,
      ID_ICON_LFT,
      ID_ICON_RGT,
      ID_ICON_PLAY,
      ID_FRAME,
      ID_FRAME_HIST
    };

    // -- data members ----------------------------------------------
    Settings* settings;

    Diagram* diagram;                // association
    std::vector< Attribute* > attributes; // association

    Cluster* frame;                  // composition
    QColor colFrm;

    std::vector< Cluster* > framesHist;            // composition
    std::vector< std::vector< Attribute* > > attrsHist; // association

    Position2D posFrame;
    std::vector< Position2D > posFramesHist;

    size_t focusFrameIdx;

    double scaleFrame;
    double scaleFramesHist;

    double offset;
    size_t vsblHistIdxLft;
    size_t vsblHistIdxRgt;
};

#endif
