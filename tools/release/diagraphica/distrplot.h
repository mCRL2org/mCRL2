// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./distrplot.h

#ifndef DISTRPLOT_H
#define DISTRPLOT_H

#include "diagram.h"

class DistrPlot : public Visualizer
{
  Q_OBJECT

  public:
    // -- constructors and destructor -------------------------------
    DistrPlot(QWidget* parent, Graph*, int attributeIndex);

    // -- set data functions ----------------------------------------

    void setDiagram(Diagram* dgrm);

    // -- visualization functions  ----------------------------------
    void visualize() override;
    void mark() override;

    // -- input event handlers --------------------------------------
    void mouseMoveEvent(QMouseEvent*) override;

    QSize sizeHint() const override { return QSize(400,400); }

  protected:
    // -- utility data functions ------------------------------------
    void calcMaxNumber();

    // -- utility drawing functions ---------------------------------
    template <Mode> void drawAxes();
    template <Mode> void drawLabels();
    template <Mode> void drawPlot();
    template <Mode> void drawDiagram();
    template <Mode> void draw();

    void showTooltip(std::size_t valueIndex, const QPointF& position);
    void hideTooltip();

    void calcPositions();
    void clearPositions();

    // -- hit detection ---------------------------------------------
    virtual void handleSelection(const Selection&) override;

    // -- data members ----------------------------------------------
    // data
    Attribute *attribute;
    std::vector< std::size_t >    number;
    std::size_t              maxNumber;
    // vis settings
    int    minHgtHintPx; // height cannot be smaller than this
    int    maxWthHintPx; // width cannot be greater than this
    double width;        // actual width calculated & used for every bar
    std::vector< Position2D > positions;
    // diagram
    Diagram*   diagram;        // association, user-defined diagram
    double     scaleDgrm;      // scale factor for diagram
    Position2D posDgrm;        // positions of diagram
    bool       showDgrm;       // show or hide diagram
    std::size_t        attrValIdxDgrm; // value idx of attribute associated with diagram
    std::string     msgDgrm;        // message to show with diagram
};

#endif

// -- end -----------------------------------------------------------
