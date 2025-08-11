// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./diagram.h

#ifndef DIAGRAM_H
#define DIAGRAM_H

#include "shape.h"

class Diagram : public QObject
{
  Q_OBJECT

  public:
    // -- constructors and destructor -------------------------------
    Diagram(QObject *parent = 0);
    Diagram &operator=(const Diagram &other);

    // -- set functions ---------------------------------------------
    void addShape(Shape* s)     { m_shapes.append(s); }
    void moveShapeToFront(int index);
    void moveShapeToBack(int index);
    void moveShapeForward(int index);
    void moveShapeBackward(int index);

    void setShowGrid(bool flag) { m_showGrid = flag; }
    void setSnapGrid(bool flag) { m_snapGrid = flag; }

    // -- get functions ---------------------------------------------
    int shapeCount() const { return m_shapes.size(); }
    Shape* shape(int index) { return m_shapes[index]; }
    const Shape* shape(int index) const { return m_shapes[index]; }

    bool snapGrid()                       { return m_snapGrid; }
    double gridInterval(double pixelSize) { return (2.0-(2.0*pixelSize*SIZE_BORDER))/(double)GRID_NUM_INTERV_HINT; }
    double angleInterval()                { return 360.0/(double)ANGL_NUM_INTERV_HINT; }
    QRectF gridCoordinates()               { return m_gridCoordinates; }

    // -- clear functions -------------------------------------------
    void removeShape(int index);

    // -- vis functions ---------------------------------------------
    using Mode = Visualizer::Mode;

    template <Mode> void draw(double pixelSize);
    template<Mode>
    void draw(double pixelSize, std::vector<Attribute*> attrs, std::vector<double> attrValIdcs, double opacity = 1.0);

  private:
    // -- private utility functions ---------------------------------

    template <Mode> void drawAxes(double pixelSize);
    template <Mode> void drawBorder(double pixelSize);
    template <Mode> void drawBorderFlush();
    template <Mode> void drawBorderFlush(const double& opacity);
    template <Mode> void drawGrid(double pixelSize);
    template <Mode> void drawShapes(double pixelSize);

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
