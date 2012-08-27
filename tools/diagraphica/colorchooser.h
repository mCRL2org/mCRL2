// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./colorchooser.h

#ifndef COLORCHOOSER_H
#define COLORCHOOSER_H

#include <QtCore>
#include <QtGui>

#include <cstddef>
#include <cstdlib>
#include <cmath>
#include <vector>
#include "dof.h"
#include "graph.h"
#include "shape.h"
#include "utils.h"
#include "visualizer.h"

class ColorChooser : public Visualizer
{
  Q_OBJECT

  public:
    enum ColorType { HueColor, OpacityColor };
    ColorChooser(QWidget *parent, DOF *dof, QList<double> *yCoordinates, ColorType type);

    void visualize(const bool& inSelectMode);

    void handleMouseEnterEvent();
    void handleMouseLeaveEvent();
    void handleMouseEvent(QMouseEvent* e);

    QSize sizeHint() const { return QSize(300,100); }

  signals:
    void activated();
    void deactivated();

  protected:
    double xPosition(int index) { return m_dof->value(index) * 2.0 - 1.0; }
    double yPosition(int index) { return (*m_yCoordinates)[index]; }

    void drawColorSpectrum();
    void drawGrayScale();
    void drawPath(const bool& inSelectMode);
    void drawPoints(const bool& inSelectMode);

    void handleHits(const std::vector<int> &ids);
    void handleDrag();

    void processHits(GLint hits, GLuint buffer[]);

  protected:
    DOF *m_dof;
    QList<double> *m_yCoordinates;
    ColorType m_type;
    size_t m_dragIdx;
};

#endif

// -- end -----------------------------------------------------------
