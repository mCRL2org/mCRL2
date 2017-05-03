// Author(s): Johannes Altmanninger
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "mcrl2/gui/workarounds.h"

#include <QVector4D>

namespace mcrl2
{

namespace gui
{

// copied and adapted from Qt sources
// http://doc.qt.io/qt-5/qvector3d.html#project
QVector3D project(const QVector3D& self, const QMatrix4x4& modelView,
                  const QMatrix4x4& projection, const QRect& viewport)
{
  QVector4D tmp(self, 1.0f);
  tmp = projection * modelView * tmp;
  if (qFuzzyIsNull(tmp.w()))
  {
    tmp.setW(1.0f);
  }
  tmp /= tmp.w();

  tmp = tmp * 0.5f + QVector4D(0.5f, 0.5f, 0.5f, 0.5f);
  tmp.setX(tmp.x() * viewport.width() + viewport.x());
  tmp.setY(tmp.y() * viewport.height() + viewport.y());

  return tmp.toVector3D();
}

// copied and adapted from Qt sources
// http://doc.qt.io/qt-5/qvector3d.html#unproject
QVector3D unproject(const QVector3D& self, const QMatrix4x4& modelView,
                    const QMatrix4x4& projection, const QRect& viewport)
{
  QMatrix4x4 inverse = QMatrix4x4(projection * modelView).inverted();

  QVector4D tmp(self, 1.0f);
  tmp.setX((tmp.x() - float(viewport.x())) / float(viewport.width()));
  tmp.setY((tmp.y() - float(viewport.y())) / float(viewport.height()));
  tmp = tmp * 2.0f - QVector4D(1.0f, 1.0f, 1.0f, 1.0f);

  QVector4D obj = inverse * tmp;
  if (qFuzzyIsNull(obj.w()))
  {
    obj.setW(1.0f);
  }
  obj /= obj.w();
  return obj.toVector3D();
}

} // namespace gui

} // namespace mcrl2
