// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "glwidget.h"
#include "glext.h"

GLWidget::GLWidget(QWidget *parent) :
  QGLWidget(parent)
{
  xRot = 0;
  yRot = 0;
  zRot = 0;

  qtGreen = QColor::fromCmykF(0.40, 0.0, 1.0, 0.0);
  qtPurple = QColor::fromCmykF(0.39, 0.39, 0.0, 0.0);
}

GLWidget::~GLWidget()
{

}

void GLWidget::initializeGL()
{

}

void GLWidget::resizeGL(int width, int height)
{

}

void GLWidget::paintGL()
{

}
