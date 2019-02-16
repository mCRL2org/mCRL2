// Author(s): Rimco Boudewijns and Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_LTSGRAPH_CAMERA_H
#define MCRL2_LTSGRAPH_CAMERA_H

#include <QColor>
#include <QOpenGLWidget>
#include <QPainter>
#include <QQuaternion>

struct CameraView
{
  QQuaternion rotation;   ///< Rotation of the camera
  QVector3D translation;  ///< Translation of the camera
  QVector3D world;        ///< The size of the box in which the graph lives
  float zoom;             ///< Zoom specifies by how much the view angle is narrowed. Larger numbers mean narrower angles.
  float pixelsize = 1;

  CameraView()
    : rotation(QQuaternion(1, 0, 0, 0)), translation(QVector3D(0, 0, 0)),
    world(QVector3D(1000.0, 1000.0, 1000.0))
  { }

  void viewport(std::size_t width, std::size_t height);
  void billboard_spherical(const QVector3D& pos);
  void billboard_cylindrical(const QVector3D& pos);
  void applyTranslation();
  void applyFrustum();
  void applyPickMatrix(GLdouble x, GLdouble y, GLdouble fuzz);
  /// \brief Converts the given position in world coordinates to a vector in eye space.
  QVector3D worldToEye(const QVector3D& position) const;
};

struct CameraAnimation : public CameraView
{
  CameraView m_source, m_target;
  std::size_t m_animation{0};
  std::size_t m_animation_steps{0};
  bool m_resizing{false};

  void start_animation(std::size_t steps);
  void operator=(const CameraView& other);
  void interpolate_cam(float pos);
  void interpolate_world(float pos);
  void animate();
  void viewport(std::size_t width, std::size_t height);
  bool resizing();
  void setZoom(float factor, std::size_t animation);
  void setRotation(const QQuaternion& rotation, std::size_t animation);
  void setTranslation(const QVector3D& translation, std::size_t animation);
  void setSize(const QVector3D& size, std::size_t animation);
  bool animationFinished() {
    return m_animation_steps == 0 || m_animation >= m_animation_steps;
  }
};

#endif // MCRL2_LTSGRAPH_CAMERA_H
