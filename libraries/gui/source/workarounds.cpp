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

QVector3D project(const QVector3D& obj, const QMatrix4x4& modelView,
                  const QMatrix4x4& projection, const QRect& viewport)
{
  QVector4D tmp(obj, 1.0);
  tmp = projection * modelView * tmp;
  if (qFuzzyIsNull(tmp.w()))
  {
    tmp.setW(1.0);
  }
  tmp /= tmp.w();
  tmp = tmp * 0.5 + QVector4D(0.5, 0.5, 0.5, 0.5);
  tmp.setX(tmp.x() * viewport.width() + viewport.x());
  tmp.setY(tmp.y() * viewport.height() + viewport.y());

  return tmp.toVector3D();
}

QVector3D unproject(const QVector3D& win, const QMatrix4x4& modelView,
                    const QMatrix4x4& projection, const QRect& viewport)
{
  QMatrix4x4 inverse = QMatrix4x4(projection * modelView).inverted();
  QVector4D tmp(win, 1.0);
  tmp.setX((tmp.x() - viewport.x()) / viewport.width());
  tmp.setY((tmp.y() - viewport.y()) / viewport.height());
  tmp = tmp * 2.0 - QVector4D(1.0, 1.0, 1.0, 1.0);

  QVector4D obj = inverse * tmp;
  if (qFuzzyIsNull(obj.w()))
  {
    obj.setW(1.0);
  }
  obj /= obj.w();

  return obj.toVector3D();
}

} // namespace gui

} // namespace mcrl2
