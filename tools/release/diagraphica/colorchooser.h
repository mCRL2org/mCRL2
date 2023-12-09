// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./colorchooser.h

#ifndef COLORCHOOSER_H
#define COLORCHOOSER_H

#include "shape.h"

class ColorChooser : public Visualizer
{
  Q_OBJECT

  public:
    enum ColorType { HueColor, OpacityColor };
    ColorChooser(QWidget *parent, DOF *dof, QList<double> *yCoordinates, ColorType type);

    void visualize();
    void select();

    void handleMouseEnterEvent();
    void handleMouseLeaveEvent();
    void handleMouseEvent(QMouseEvent* e);

    QSize sizeHint() const { return QSize(300,100); }

  signals:
    void activated();
    void deactivated();

  private:
    double xPosition(int index) { return m_dof->value(index) * 2.0 - 1.0; }
    double yPosition(int index) { return (*m_yCoordinates)[index]; }

    template <Mode> void drawColorSpectrum();
    template <Mode> void drawGrayScale();
    template <Mode> void drawPath();
    template <Mode> void drawPoints();
    template <Mode> void draw();

    void handleHits(const std::vector<int> &ids);
    void handleDrag();

    void processHits(GLint hits, GLuint buffer[]);

    DOF *m_dof;
    QList<double> *m_yCoordinates;
    ColorType m_type;
    std::size_t m_dragIdx;
};

#endif

// -- end -----------------------------------------------------------
