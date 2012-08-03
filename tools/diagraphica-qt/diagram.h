// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./diagram.h

#ifndef DIAGRAM_H
#define DIAGRAM_H

#include <QtCore>
#include <QtGui>

#include <cstddef>
#include <vector>
#include "colleague.h"
#include "visutils.h"
#include "shape.h"

class Diagram : public Colleague
{
  public:
    // -- constructors and destructor -------------------------------
    Diagram(Mediator* m);
    virtual ~Diagram();

    // -- set functions ---------------------------------------------
    void addShape(Shape* s);
    void moveShapeToFront(const size_t& idx);
    void moveShapeToBack(const size_t& idx);
    void moveShapeForward(const size_t& idx);
    void moveShapeBackward(const size_t& idx);

    void setShowGrid(const bool& flag);
    void setSnapGrid(const bool& flag);

    // -- get functions ---------------------------------------------
    size_t getSizeShapes();
    Shape* getShape(const size_t& idx);

    bool getSnapGrid();
    double getGridInterval(double pixelSize);
    double getAngleInterval();
    void getGridCoordinates(double& xLeft, double& xRight, double& yTop, double& yBottom);

    // -- clear functions -------------------------------------------
    void deleteShape(const size_t& idx);

    // -- vis functions ---------------------------------------------
    void visualize(
      const bool& inSelectMode,
      double pixelSize);
    void visualize(
      const bool& inSelectMode,
      double pixelSize,
      const std::vector< Attribute* > attrs,
      const std::vector< double > attrValIdcs);
    void visualize(
      const bool& inSelectMode,
      double pixelSize,
      const std::vector< Attribute* > attrs,
      const std::vector< double > attrValIdcs,
      double opacity);


  protected:
    // -- private utility functions ---------------------------------
    void initGridSettings();

    void drawAxes(
      const bool& inSelectMode,
      double pixelSize);
    void drawBorder(
      const bool& inSelectMode,
      double pixelSize);
    void drawBorderFlush(
      const bool& inSelectMode);
    void drawBorderFlush(
      const bool& inSelectMode,
      const double& opacity);
    void drawGrid(
      const bool& inSelectMode,
      double pixelSize);
    void drawShapes(
      const bool& inSelectMode,
      double pixelSize);

    // -- data members ----------------------------------------------
    std::vector< Shape* > shapes; // composition

    bool   showGrid;
    bool   snapGrid;
    double gridXLeft;
    double gridXRight;
    double gridYTop;
    double gridYBottom;

    // constants
    int SIZE_BORDER;
    int GRID_NUM_INTERV_HINT;
    int ANGL_NUM_INTERV_HINT;
};

#endif

// -- end -----------------------------------------------------------
