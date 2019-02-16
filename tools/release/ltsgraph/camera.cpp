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

#include <cmath>

CameraView::CameraView()
  : m_rotation(QQuaternion(1, 0, 0, 0)),
    m_position(QVector3D(0, 0, 0))
{}

void CameraView::viewport(std::size_t width, std::size_t height)
{
  m_viewport = QRect(0, 0, width, height);
}

void CameraView::billboard_spherical(const QVector3D& pos)
{
  QVector3D rt, up, lk;

  // I don't know what this operation is.
  /*QMatrix4x4& mm(m_viewMatrix);
  lk.setX(mm[0] * pos.x() + mm[4] * pos.y() + mm[8] * pos.z() + mm[12]);
  lk.setY(mm[1] * pos.x() + mm[5] * pos.y() + mm[9] * pos.z() + mm[13]);
  lk.setZ(mm[2] * pos.x() + mm[6] * pos.y() + mm[10] * pos.z() + mm[14]);
  */

  lk /= lk.length();
  rt = QVector3D::crossProduct(lk, QVector3D(0, 1, 0));
  up = QVector3D::crossProduct(rt, lk);
  GLfloat matrix[16] = {rt.x(), rt.y(), rt.z(), 0,
    up.x(), up.y(), up.z(), 0,
    -lk.x(),  -lk.y(),  -lk.z(),  0,
    0,        0,      0,  1
  };
  billboard_cylindrical(pos);
  glMultMatrixf(matrix);
}

void CameraView::billboard_cylindrical(const QVector3D& pos)
{
  glTranslatef(pos.x(), pos.y(), pos.z());
  mcrl2::gui::applyRotation(m_rotation, /*reverse=*/true);
}

void CameraView::update()
{
  m_viewMatrix = QMatrix4x4();
  m_viewMatrix.lookAt(QVector3D(0.0f, 0.0f, -1000.0f * m_zoomfactor) + m_position, QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f));

  m_projectionMatrix = QMatrix4x4();
  m_projectionMatrix.frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1000.0f);
}

QVector3D CameraView::worldToWindow(QVector3D world) const
{
  QVector3D eye = world.project(m_viewMatrix, m_projectionMatrix, m_viewport);

  // Flip the y-coordinate as Qt coordinate system is different from OpenGl.
  return QVector3D(eye.x(),
                 (m_viewport.height() - eye.y()),
                 eye.z());
}

QVector3D CameraView::windowToWorld(QVector3D eye) const
{
  // Flip the y-coordinate as Qt coordinate system is different from OpenGl.
  eye.setY(m_viewport.height() - eye.y());

  return eye.unproject(m_viewMatrix, m_projectionMatrix, m_viewport);
}


bool CameraView::operator!=(const CameraView& other)
{
  return (m_rotation != other.m_rotation || m_position != other.m_position || m_position != other.m_position);
}

// Implementation of CameraAnimation

void CameraAnimation::reset()
{
  m_rotation = QQuaternion(1, 0, 0, 0);
  m_position = QVector3D(0, 0, 0);
  m_zoomfactor = 5.0f;
}

void CameraAnimation::update()
{
  /*if (*this != m_source)
  {
    std::size_t halfway = m_animation_steps / 2;
    if (m_animation < halfway)
    {
      interpolate_cam((float)(++m_animation) / halfway);
    }

    if (m_animation == halfway)
    {
      m_animation_steps -= halfway;
      m_animation = 0;
    }
  }*/
  
  /*if (m_target.world != world)
  {
    interpolate_world((float)(++m_animation) / m_animation_steps);

    if (pos > 0.999)
    {
    world = m_target.world;
    }
    else
    {
    world.setX(m_target.world.x() * pos + m_source.world.x() * (1.0 - pos));
    world.setY(m_target.world.y() * pos + m_source.world.y() * (1.0 - pos));

    if (m_target.world.z() > m_source.world.z())
    {
    world.setZ(m_target.world.z() * sin(M_PI_2 * pos) + m_source.world.z() * (1.0 - sin(M_PI_2 * pos)));
    }
    else
    {
    world.setZ(m_target.world.z() * (1.0 - cos(M_PI_2 * pos)) + m_source.world.z() * cos(M_PI_2 * pos));
    }
    }
  }
  else
  {
  if (delta > 0.999)
  {
    *this = m_target;
  }
  else
  {
  }
  }*/

  if (!animationFinished())
  {
    float progress = (float)(++m_animation) / m_animation_steps;

    m_rotation.slerp(m_source.m_rotation, m_target.m_rotation, progress);
    m_position = m_target.m_position * progress + m_source.m_position * (1.0 - progress);
    m_zoomfactor = m_target.m_zoomfactor * progress + m_source.m_zoomfactor * (1.0 - progress);

    // Update the camera matrices.
    CameraView::update();
  }
}

void CameraAnimation::setZoom(float factor, std::size_t animation)
{
  m_target.m_zoomfactor += factor;
  start_animation(animation);
}

void CameraAnimation::setRotation(const QQuaternion& rotation, std::size_t steps)
{
  m_target.m_rotation += rotation;
  start_animation(steps);
}

void CameraAnimation::setTranslation(const QVector3D& translation, std::size_t steps)
{
  m_target.m_position += translation;
  start_animation(steps);
}

void CameraAnimation::setSize(const QVector3D& size, std::size_t steps)
{
  start_animation(steps);
}

void CameraAnimation::operator=(const CameraView& other)
{
  m_rotation = other.m_rotation;
  m_position = other.m_position;
  m_zoomfactor = other.m_zoomfactor;
}

void CameraAnimation::start_animation(std::size_t steps)
{
  m_source = *this;
  m_animation_steps = std::min((int)steps, 1);
  m_animation = 0;

  if (steps == 0)
  {

  }
}
