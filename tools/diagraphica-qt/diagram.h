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
#include "visutils.h"
#include "shape.h"

class Diagram : public QObject
{
  Q_OBJECT

  public:
    // -- constructors and destructor -------------------------------
    Diagram(QObject *parent = 0);
    virtual ~Diagram();

    // -- set functions ---------------------------------------------
    void addShape(Shape* s)     { m_shapes.append(s); }
    void moveShapeToFront(int index);
    void moveShapeToBack(int index);
    void moveShapeForward(int index);
    void moveShapeBackward(int index);

    void setShowGrid(bool flag) { m_showGrid = flag; }
    void setSnapGrid(bool flag) { m_snapGrid = flag; }

    // -- get functions ---------------------------------------------
    int shapeCount()        { return m_shapes.size(); }
    Shape* shape(int index);

    bool snapGrid()                       { return m_snapGrid; }
    double gridInterval(double pixelSize) { return (2.0-(2.0*pixelSize*SIZE_BORDER))/(double)GRID_NUM_INTERV_HINT; }
    double angleInterval()                { return 360.0/(double)ANGL_NUM_INTERV_HINT; }
    QRectF gridCoordinates()               { return m_gridCoordinates; }

    // -- clear functions -------------------------------------------
    void removeShape(int index);

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
    QList<Shape*> m_shapes; // composition

    bool  m_showGrid;
    bool  m_snapGrid;
    QRectF m_gridCoordinates;

    // constants
    int SIZE_BORDER;
    int GRID_NUM_INTERV_HINT;
    int ANGL_NUM_INTERV_HINT;
};

#endif

// -- end -----------------------------------------------------------
