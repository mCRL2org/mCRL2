// Author(s): Rimco Boudewijns and Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "camera.h"

#include "mcrl2/gui/arcball.h"
#include "mcrl2/gui/glu.h"

void ArcballCameraView::update()
{
  m_viewMatrix.setToIdentity();
  m_viewMatrix.translate(QVector3D(0.0f, 0.0f, -m_zoom));
  m_viewMatrix.rotate(m_rotation);
  m_viewMatrix.translate(m_center);

  QMatrix4x4 invertedViewMatrix = m_viewMatrix.inverted();
  m_worldPosition = QVector3D(invertedViewMatrix.column(3));

  m_projectionMatrix.setToIdentity();
  m_projectionMatrix.perspective(m_vert_fieldofview, static_cast<float>(m_viewport.width()) / static_cast<float>(m_viewport.height()), 1.0f, m_viewdistance);
}

QVector3D ArcballCameraView::worldToWindow(QVector3D world) const
{
  QVector3D eye = world.project(m_viewMatrix, m_projectionMatrix, m_viewport);

  // Flip the y-coordinate as Qt coordinate system is different from OpenGl.
  return QVector3D(eye.x(),
                 (m_viewport.height() - eye.y()),
                 eye.z());
}

QVector3D ArcballCameraView::windowToWorld(QVector3D eye) const
{
  // Flip the y-coordinate as Qt coordinate system is different from OpenGl.
  eye.setY(m_viewport.height() - eye.y());

  return eye.unproject(m_viewMatrix, m_projectionMatrix, m_viewport);
}

QVector3D ArcballCameraView::position() const
{
  return m_worldPosition;
}

void ArcballCameraView::reset()
{
  ArcballCameraView default_camera;
  m_center = default_camera.m_center;
  m_rotation = default_camera.m_rotation;
  m_zoom = default_camera.m_zoom;
}

void ArcballCameraView::resetRotation()
{
  ArcballCameraView default_camera;
  m_rotation = default_camera.m_rotation;
}
