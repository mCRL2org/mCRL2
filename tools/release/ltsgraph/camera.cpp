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

void CameraView::viewport(std::size_t width, std::size_t height)
{
  glViewport(0, 0, width, height);
  pixelsize = 1000.0 / (width < height ? height : width);
  world.setX(width * pixelsize);
  world.setY(height * pixelsize);
}

void CameraView::billboard_spherical(const QVector3D& pos)
{
  QVector3D rt, up, lk;
  GLfloat mm[16];

  glGetFloatv(GL_MODELVIEW_MATRIX, mm);
  lk.setX(mm[0] * pos.x() + mm[4] * pos.y() + mm[8] * pos.z() + mm[12]);
  lk.setY(mm[1] * pos.x() + mm[5] * pos.y() + mm[9] * pos.z() + mm[13]);
  lk.setZ(mm[2] * pos.x() + mm[6] * pos.y() + mm[10] * pos.z() + mm[14]);

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
  mcrl2::gui::applyRotation(rotation, /*reverse=*/true);
}

void CameraView::applyTranslation()
{
  float viewdepth = world.length() + 2 * pixelsize * 10;
  glTranslatef(0, 0, -5000.0005 - 0.5 * viewdepth);
  glTranslatef(translation.x(), translation.y(), translation.z());
}

void CameraView::applyFrustum()
{
  float viewdepth = world.length() + 2 * pixelsize * 10;
  float f = 2 * zoom * (10000.0 + (viewdepth - world.z())) / 10000.0;
  float nearPlane = 1000;
  float farPlane = viewdepth + 10000;
  glFrustum(-world.x() / f, world.x() / f, -world.y() / f, world.y() / f, nearPlane, farPlane);
}

void CameraView::applyPickMatrix(GLdouble x, GLdouble y, GLdouble fuzz)
{
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  // Viewport is always (0, 0, width, height)
  gluPickMatrix(x, viewport[3] - y, fuzz * pixelsize, fuzz * pixelsize, viewport);
}

void CameraAnimation::start_animation(std::size_t steps)
{
  m_source = *this;
  m_animation_steps = steps;
  m_animation = 0;
  if (steps == 0) {
    operator=(m_target);
  }
}

void CameraAnimation::operator=(const CameraView& other)
{
  rotation = other.rotation;
  translation = other.translation;
  zoom = other.zoom;
  world = other.world;
  pixelsize = other.pixelsize;
}

void CameraAnimation::interpolate_cam(float pos)
{
  if (pos > 0.999)
  {
    rotation = m_target.rotation;
    translation = m_target.translation;
    zoom = m_target.zoom;
  }
  else
  {
    // if this is unsatisfactory, use https://en.wikipedia.org/wiki/Slerp
    rotation = m_target.rotation * pos + m_source.rotation * (1.0 - pos);
    translation = m_target.translation * pos + m_source.translation * (1.0 - pos);
    zoom = m_target.zoom * pos + m_source.zoom * (1.0 - pos);
  }
}

void CameraAnimation::interpolate_world(float pos)
{
  m_resizing = true;
  if (pos > 0.999)
  {
    world = m_target.world;
  }
  else
  {
    world.setX(m_target.world.x() * pos + m_source.world.x() * (1.0 - pos));
    world.setY(m_target.world.y() * pos + m_source.world.y() * (1.0 - pos));
    if (m_target.world.z() > m_source.world.z()) {
      world.setZ(m_target.world.z() * sin(M_PI_2 * pos) + m_source.world.z() * (1.0 - sin(M_PI_2 * pos)));
    }
    else {
      world.setZ(m_target.world.z() * (1.0 - cos(M_PI_2 * pos)) + m_source.world.z() * cos(M_PI_2 * pos));
    }
  }
}

void CameraAnimation::animate()
{
  if ((m_target.rotation != rotation || m_target.translation != translation || m_target.zoom != zoom) &&
      (m_target.world != world))
  {
    std::size_t halfway = m_animation_steps / 2;
    if (m_animation < halfway) {
      interpolate_cam((float)(++m_animation) / halfway);
    }
    if (m_animation == halfway)
    {
      m_animation_steps -= halfway;
      m_animation = 0;
    }
  }
  else if (m_target.world != world)
  {
    interpolate_world((float)(++m_animation) / m_animation_steps);
  }
  else {
    interpolate_cam((float)(++m_animation) / m_animation_steps);
  }
}

void CameraAnimation::viewport(std::size_t width, std::size_t height)
{
  CameraView::viewport(width, height);
  m_target.world.setX(world.x());
  m_target.world.setY(world.y());
  m_target.pixelsize = pixelsize;
}

bool CameraAnimation::resizing()
{
  bool temp = m_resizing;
  m_resizing = false;
  return temp;
}

void CameraAnimation::setZoom(float factor, std::size_t animation)
{
  m_target.zoom = factor;
  start_animation(animation);
}

void CameraAnimation::setRotation(const QQuaternion& rotation, std::size_t animation)
{
  m_target.rotation = rotation;
  start_animation(animation);
}

void CameraAnimation::setTranslation(const QVector3D& translation, std::size_t animation)
{
  m_target.translation = translation;
  start_animation(animation);
}

void CameraAnimation::setSize(const QVector3D& size, std::size_t animation)
{
  m_target.world = size;
  start_animation(animation);
}
