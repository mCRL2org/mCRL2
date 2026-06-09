// Author(s): Johannes Altmanninger
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "mcrl2/gui/arcball.h"

#include "mcrl2/gui/glu.h"

#include <array>

#ifndef M_PI
  #define M_PI 3.14159265358979323846264338327950288
#endif


namespace mcrl2::gui
{

/// \brief Compute the projection of the point p on a sphere that covers the viewport
static QVector3D arcballVector(const QPoint& p)
{
  float x, y, z;
  const float radius = 1.5f;
  std::array<GLint, 4> viewport;
  glGetIntegerv(GL_VIEWPORT, viewport.data());

  // Compute x and y relative to the middle of the viewport
  x = static_cast<float>(p.x()) / static_cast<float>(viewport[2]) * 2.0f - 1.0f;
  y = static_cast<float>(p.y()) / static_cast<float>(viewport[3]) * 2.0f - 1.0f;
  y = -y;

  float squared = x * x + y * y;
  if (squared <= radius * radius)
  {
    z = std::sqrt(radius * radius - squared);
  }
  else
  {
    float len = std::sqrt(squared);
    if (std::isnormal(len))
    {
      x /= len;
      y /= len;
    }
    else
    {
      x = 0.0f;
      y = 0.0f;
    }
    z = 0.0f;
  }
  return QVector3D(x, y, z).normalized();
}

QQuaternion arcballRotation(const QPoint& p1, const QPoint& p2)
{
  // update rotation based on the difference in mouse coordinates
  QVector3D v1 = arcballVector(p1).normalized();
  QVector3D v2 = arcballVector(p2).normalized();
  QVector3D cross = QVector3D::crossProduct(v1, v2);
  float dot = QVector3D::dotProduct(v1, v2);
  return QQuaternion(dot, cross);
}

void applyRotation(const QQuaternion& rotation, bool reverse)
{
  float angle = static_cast<float>(180.0 / M_PI) * std::acos(std::clamp(rotation.scalar(), -1.0f, 1.0f));
  if (reverse)
  {
    angle = -angle;
  }
  // not sure why the angle has to be doubled
  glRotatef(2 * angle, rotation.x(), rotation.y(), rotation.z());
}

} // namespace mcrl2::gui

