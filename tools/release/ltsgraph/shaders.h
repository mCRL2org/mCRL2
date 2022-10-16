// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_LTSGRAPH_SHADERS_H
#define MCRL2_LTSGRAPH_SHADERS_H

#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions_3_3_Core>

#include <array>

/// \brief A simple shader that can be used to render three dimensional objects
/// with solid colors.
class GlobalShader : public QOpenGLShaderProgram
{
  public:
  /// \brief Sets the sources, links the program and extracts the required
  /// information.
  bool link() override;

  /// \brief Sets the world view projection matrix used to transform the object.
  void setWorldViewProjMatrix(const QMatrix4x4& matrix)
  {
    setUniformValue(m_worldViewProjMatrix_location, matrix);
  }

  /// \brief Sets the fill color of this object.
  void setColor(const QVector3D& color)
  {
    setUniformValue(m_color_location, QVector4D(color, 1.0f));
  }
  void setColor(const QVector4D& color)
  {
    setUniformValue(m_color_location, color);
  }

  private:
  int m_worldViewProjMatrix_location = -1;
  int m_color_location = -1;
};

class NodeShaderInstanced : public QOpenGLShaderProgram
{
  int u_eye_loc = -1;
  int u_VP_loc = -1;
  int u_scale_loc = -1;
  int u_V_loc = -1;
  public:
  bool link();
  void setEye(const QVector3D& eye){
    setUniformValue(u_eye_loc, eye);
  }
  void setVP(const QMatrix4x4& VP){
    setUniformValue(u_VP_loc, VP);
  }
  void setV(const QMatrix4x4& V){
    setUniformValue(u_V_loc, V);
  }
  void setScale(const float c){
    setUniformValue(u_scale_loc, c);
  }
};

class ArrowShaderInstanced : public QOpenGLShaderProgram
{
  int u_VP_loc = -1;
  int u_scale_loc = -1;
  public:
  bool link();
  void setVP(const QMatrix4x4& VP){
    setUniformValue(u_VP_loc, VP);
  }
  void setScale(const float c){
    setUniformValue(u_scale_loc, c);
  }
};

/// \brief A shader that generates a cubic bezier curve from given control
/// points using the vertex shader.
class ArcShader : public QOpenGLShaderProgram
{
  public:
  /// \brief Sets the sources, links the program and extracts the required
  /// information.
  bool link() override;

  /// \brief Sets the control points used by this shader.
  void setControlPoints(const std::array<QVector3D, 4>& points)
  {
    setUniformValueArray(m_controlPoints_location, points.data(), 4);
  }

  /// \brief Sets the view projection matrix used to transform the object.
  void setViewProjMatrix(const QMatrix4x4& matrix)
  {
    setUniformValue(m_viewProjMatrix_location, matrix);
  }

  /// \brief Sets the view matrix used to transform the object.
  void setViewMatrix(const QMatrix4x4& matrix)
  {
    setUniformValue(m_viewMatrix_location, matrix);
  }

  /// \brief Sets the color of the arc.
  void setColor(const QVector3D& color)
  {
    setUniformValue(m_color_location, color);
  }

  /// \brief Sets the fog density used.
  void setFogDensity(float density)
  {
    setUniformValue(m_fogdensity_location, density);
  }

  private:
  int m_viewProjMatrix_location = -1;
  int m_viewMatrix_location = -1;
  int m_color_location = -1;
  int m_fogdensity_location = -1;
  int m_controlPoints_location = -1;
};


class ArcShaderInstanced : public QOpenGLShaderProgram
{
  public:
  /// \brief Sets the sources, links the program and extracts the required
  /// information.
  bool link() override;


  /// \brief Sets the view projection matrix used to transform the object.
  void setViewProjMatrix(const QMatrix4x4& matrix)
  {
    setUniformValue(m_viewProjMatrix_location, matrix);
  }

  /// \brief Sets the view matrix used to transform the object.
  void setViewMatrix(const QMatrix4x4& matrix)
  {
    setUniformValue(m_viewMatrix_location, matrix);
  }

  /// \brief Sets the color of the arc.
  void setColor(const QVector3D& color)
  {
    setUniformValue(m_color_location, color);
  }

  /// \brief Sets the fog density used.
  void setFogDensity(float density)
  {
    setUniformValue(m_fogdensity_location, density);
  }

  private:
  int m_viewProjMatrix_location = -1;
  int m_viewMatrix_location = -1;
  int m_color_location = -1;
  int m_fogdensity_location = -1;
};

#endif // MCRL2_LTSGRAPH_SHADERS_H
