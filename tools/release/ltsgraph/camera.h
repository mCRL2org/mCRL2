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
#include <QMatrix4x4>

/// \brief The camera view keeps track of the camera position around a point in 3D space. It also keeps track of the
///        viewport that contains the width and height of the view. From this it computes the necessary projection
///        and view matrices to draw objects as if they are viewed from this camera.
class CameraView
{
public:
  CameraView();

  /// \brief Update the dimensions of the viewport.
  void viewport(std::size_t width, std::size_t height);

  void billboard_spherical(const QVector3D& pos);
  void billboard_cylindrical(const QVector3D& pos);

  /// \brief Update the view and projection matrix.
  void update();

  /// \brief Converts the given position from world coordinates to a vector (in pixels) in viewport coordinates.
  QVector3D worldToViewport(const QVector3D& position) const;

  /// \brief Converts the given position from eye space to a vector in world coordinates.
  QVector3D eyeToWorld(const QVector3D& eye) const;

public:
  QQuaternion rotation;   /// Rotation of the camera
  QVector3D translation;  /// Translation of the camera
  float zoom;             /// Zoom specifies by how much the view angle is narrowed. Larger numbers mean narrower angles.

private:
  QMatrix4x4 m_projectionMatrix; /// \brief The project matrix of this camera.
  QMatrix4x4 m_viewMatrix;
  float m_viewdistance;
  QRect m_viewport;
};

/// \brief Applies an animation to the camera of which the result is this class itself.
class CameraAnimation : public CameraView
{
public:
  void start_animation(std::size_t steps);
  void operator=(const CameraView& other);
  void interpolate_cam(float pos);
  void interpolate_world(float pos);
  void animate();
  bool resizing();
  void setZoom(float factor, std::size_t animation);
  void setRotation(const QQuaternion& rotation, std::size_t animation);
  void setTranslation(const QVector3D& translation, std::size_t animation);
  void setSize(const QVector3D& size, std::size_t animation);
  bool animationFinished() {
    return m_animation_steps == 0 || m_animation >= m_animation_steps;
  }

private:
  CameraView m_source, m_target;
  std::size_t m_animation = 0;
  std::size_t m_animation_steps = 0;
  bool m_resizing = false;

};

#endif // MCRL2_LTSGRAPH_CAMERA_H
