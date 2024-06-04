// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./corrlplot.h

#ifndef CORRLPLOT_H
#define CORRLPLOT_H

#include "diagram.h"

class CorrlPlot : public Visualizer
{
  Q_OBJECT

  public:
    // -- constructors and destructor -------------------------------
    CorrlPlot(QWidget* parent, Graph*, int attributeIndex1, int attributeIndex2);

    // -- set data functions ----------------------------------------
    void setDiagram(Diagram* dgrm);

    // -- visualization functions  ----------------------------------
    void visualize() override;
    void mark() override;

    // -- input event handlers --------------------------------------
    void mouseMoveEvent(QMouseEvent*) override;

    QSize sizeHint() const override { return QSize(400,400); }

  private:
    // -- utility data functions ------------------------------------
    void initLabels();
    void calcMaxNumber();

    // -- utility drawing functions ---------------------------------
    template <Mode> void drawAxes();
    template <Mode> void drawLabels();
    template <Mode> void drawPlot();
    template <Mode> void drawDiagram();
    template <Mode> void draw();

    void showTooltip(std::size_t xIndex, std::size_t yIndex, const QPointF& position);
    void hideTooltip();

    void calcPositions();
    void clearPositions();

    // -- hit detection ---------------------------------------------
    virtual void handleSelection(const Selection&) override;

    // -- data members ----------------------------------------------

    // data
    Attribute *attribute1;
    Attribute *attribute2;
    std::string xLabel;
    std::string yLabel;
    std::vector< std::vector< std::size_t > > mapXToY; // for each x idx, 1 or more y indices
    std::vector< std::vector< int > > number;  // number associated with every entry in mapXToY
    std::vector< int > maxNumX;   // for every column, the largest number
    int sumMaxNumX;          // sum of entries in maxNumX
    std::vector< int > maxNumY;   // for every row, the largest number
    int sumMaxNumY;          // sum of entries in maxNumY
    int maxNumber;           // overall largest number

    // vis settings
    int minRadHintPx; // min radius cannot be smaller than this
    int maxRadHintPx; // max radius cannot be smaller than this
    std::vector< std::vector< double > > radii;
    std::vector< std::vector< Position2D > > positions;

    // diagram
    Diagram*   diagram;         // association, user-defined diagram
    double     scaleDgrm;       // scale factor for diagram
    Position2D posDgrm;         // positions of diagram
    bool       showDgrm;        // show or hide diagram
    std::size_t        attrValIdx1Dgrm; // value idx of 1st attribute associated with diagram
    std::size_t        attrValIdx2Dgrm; // value idx of 2nd attribute associated with diagram
    std::string     msgDgrm;        // message to show with diagram
};

#endif

// -- end -----------------------------------------------------------
