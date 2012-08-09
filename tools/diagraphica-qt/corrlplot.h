// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./corrlplot.h

#ifndef CORRLPLOT_H
#define CORRLPLOT_H

#include <QtCore>
#include <QtGui>

#include <cstddef>
#include <cstdlib>
#include <cmath>
#include <vector>
#include "diagram.h"
#include "graph.h"
#include "utils.h"
#include "visualizer.h"

class CorrlPlot : public Visualizer
{
  Q_OBJECT

  public:
    // -- constructors and destructor -------------------------------
    CorrlPlot(
      QWidget *parent,
      Graph* g,
      int attributeIndex1,
      int attributeIndex2);

    // -- set data functions ----------------------------------------
    void setDiagram(Diagram* dgrm);

    // -- visualization functions  ----------------------------------
    void visualize(const bool& inSelectMode);
    void drawAxes(
      const bool& inSelectMode,
      const std::string& xLbl,
      const std::string& yLbl);
    void drawLabels(const bool& inSelectMode);
    void drawPlot(const bool& inSelectMode);
    void drawDiagram(const bool& inSelectMode);

    // -- input event handlers --------------------------------------
    void handleMouseEvent(QMouseEvent* e);

  protected:
    // -- utility data functions ------------------------------------
    void initLabels();
    void calcMaxNumber();

    // -- utility drawing functions ---------------------------------
    // ***
    //void clear();
    void setScalingTransf();
    void displTooltip(
      const int& xIdx,
      const int& yIdx);

    void calcPositions();
    void clearPositions();

    // -- hit detection ---------------------------------------------
    void processHits(
      GLint hits,
      GLuint buffer[]);

    // -- data members ----------------------------------------------

    // data
    size_t attrIdx1;
    size_t attrIdx2;
    std::string xLabel;
    std::string yLabel;
    std::vector< std::vector< size_t > > mapXToY; // for each x idx, 1 or more y indices
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
    size_t        attrValIdx1Dgrm; // value idx of 1st attribute associated with diagram
    size_t        attrValIdx2Dgrm; // value idx of 2nd attribute associated with diagram
    std::string     msgDgrm;        // message to show with diagram
};

#endif

// -- end -----------------------------------------------------------
