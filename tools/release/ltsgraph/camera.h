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

#include <QQuaternion>
#include <QMatrix4x4>
#include <QVector3D>

/// \brief The camera view keeps track of a camera rotating around a 3D position in space. It also keeps track of the
///        viewport that contains the width and height of the window. From this it computes the necessary projection
///        and view matrices to draw objects as if they are viewed from this camera. The assumption is that the camera
///        is always upright towards the z-axis. It also uses a frustum projection where objects further away appear
///        smaller, but the
class CameraView
{
public:
  void billboard_spherical(const QVector3D& pos);
  void billboard_cylindrical(const QVector3D& pos);

  /// \brief Update the view and projection matrix.
  void update();

  /// \brief Converts the given position from world coordinates to Qt window coordinates.
  QVector3D worldToWindow(QVector3D position) const;

  /// \brief Converts the given position from Qt window coordinates to world space coordinates.
  QVector3D windowToWorld(QVector3D eye) const;

  /// \brief Set the distance to the camera position.
  void zoom(float zoom) { m_zoom = zoom; }
  float zoom() const { return m_zoom; }

  /// \brief Set the rotation around the camera position.
  void rotation(const QQuaternion& rotation) { m_rotation = rotation; }
  const QQuaternion& rotation() const { return m_rotation; }

  /// \brief Set the position of the camera.
  void position(const QVector3D& position) { m_position = position; }
  const QVector3D& position() const { return m_position; }

  /// \brief Set the dimensions of the viewport.
  void viewport(std::size_t width, std::size_t height) { m_viewport = QRect(0, 0, width, height); }

  const QMatrix4x4& viewMatrix() const { return m_viewMatrix; }
  const QMatrix4x4& projectionMatrix() const { return m_projectionMatrix; }

  /// \brief Sets the position and orientation of the camera to its default values.
  void reset();

private:
  QVector3D m_position = QVector3D(0.0f, 0.0f, 0.0f);           /// Position of the camera.
  QQuaternion m_rotation = QQuaternion(1.0f, 0.0f, 0.0f, 0.0f); /// Rotation of the camera around the given position.
  float m_zoom = 500.0f;     /// The distance to the position.
  float m_viewdistance = 10000.0f; /// The maximum view distance of the camera.
  float m_vert_fieldofview = 70.0f;

  QMatrix4x4 m_projectionMatrix; /// \brief The project matrix of this camera.
  QMatrix4x4 m_viewMatrix;
  QRect m_viewport;
};

#endif // MCRL2_LTSGRAPH_CAMERA_H
