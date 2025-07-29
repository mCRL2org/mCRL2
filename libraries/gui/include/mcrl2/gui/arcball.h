// Author(s): Johannes Altmanninger
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_UTILITIES_ARCBALL_H
#define MCRL2_UTILITIES_ARCBALL_H

#include <QQuaternion>
#include <QVector3D>
#include <cmath>

namespace mcrl2::gui
{

QQuaternion arcballRotation(const QPoint& p1, const QPoint& p2);

void applyRotation(const QQuaternion& rotation, bool reverse = false);

} // namespace mcrl2::gui

#endif // MCRL2_UTILITIES_ARCBALL_H
