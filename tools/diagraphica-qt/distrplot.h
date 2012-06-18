// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./distrplot.h

#ifndef DISTRPLOT_H
#define DISTRPLOT_H

#include <cstddef>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <string>
#include "diagram.h"
#include "graph.h"
#include "utils.h"
#include "visualizer.h"

class DistrPlot : public Visualizer
{
  public:
    // -- constructors and destructor -------------------------------
    DistrPlot(
      Mediator* m,
      Graph* g,
      GLCanvas* c);
    virtual ~DistrPlot();

    // -- set data functions ----------------------------------------
    void setValues(
      const size_t& idx,
      const std::vector< size_t > &num);
    void clearValues();

    void setDiagram(Diagram* dgrm);

    // -- visualization functions  ----------------------------------
    void visualize(const bool& inSelectMode);
    void drawAxes(const bool& inSelectMode);
    void drawLabels(const bool& inSelectMode);
    void drawPlot(const bool& inSelectMode);
    void drawDiagram(const bool& inSelectMode);

    // -- input event handlers --------------------------------------
    void handleMouseMotionEvent(
      const int& x,
      const int& y);
    /*
    void handleMouseEnterEvent();
    void handleMouseLeaveEvent();
    */

  protected:
    // -- utility data functions ------------------------------------
    void calcMaxNumber();

    // -- utility drawing functions ---------------------------------
    // ***
    //void clear();
    void setScalingTransf();
    void displTooltip(const size_t& posIdx);

    void calcPositions();
    void clearPositions();

    // -- hit detection ---------------------------------------------
    void processHits(
      GLint hits,
      GLuint buffer[]);

    // -- data members ----------------------------------------------
    // data
    size_t              attrIdx;
    std::vector< size_t >    number;
    size_t              maxNumber;
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
    size_t        attrValIdxDgrm; // value idx of attribute associated with diagram
    std::string     msgDgrm;        // message to show with diagram
};

#endif

// -- end -----------------------------------------------------------
