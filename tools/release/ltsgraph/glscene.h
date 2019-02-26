// Author(s): Rimco Boudewijns and Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_LTSGRAPH_GLSCENE_H
#define MCRL2_LTSGRAPH_GLSCENE_H

#include "graph.h"
#include "camera.h"

#include <QOpenGLBuffer>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QPainter>

#include <array>
#include <vector>

/// \brief A simple shader that can be used to render three dimensional objects.
class GlobalShader : public QOpenGLShaderProgram
{
public:
  /// \brief Sets the sources, links the program and extracts the required information.
  bool link() override;

  /// \brief Sets the world view projection matrix used to transform the object.
  void setWorldViewProjMatrix(const QMatrix4x4& matrix) { setUniformValue(m_worldViewProjMatrix_location, matrix); }

  /// \brief Sets the fill color of this object.
  void setColor(const QVector3D& color) { setUniformValue(m_color_location, color); }

private:
  int m_worldViewProjMatrix_location = -1;
  int m_color_location = -1;
};

/// \brief A shader that constructs an arc from given control points using the vertex shader.
class ArcShader : public QOpenGLShaderProgram
{
public:
  /// \brief Sets the sources, links the program and extracts the required information.
  bool link() override;

  /// \brief Sets the control points used by this shader.
  void setControlPoints(const std::array<QVector3D, 4>& points) { setUniformValueArray(m_controlPoints_location, points.data(), 4); }

  /// \brief Sets the view projection matrix used to transform the object.
  void setViewProjMatrix(const QMatrix4x4& matrix) { setUniformValue(m_viewProjMatrix_location, matrix); }

  /// \brief Sets the view matrix used to transform the object.
  void setViewMatrix(const QMatrix4x4& matrix) { setUniformValue(m_viewMatrix_location, matrix); }

  /// \brief Sets the color of the arc.
  void setColor(const QVector3D& color) { setUniformValue(m_color_location, color); }

  /// \brief Sets the fog density used.
  void setFogDensity(float density) { setUniformValue(m_fogdensity_location, density); }

private:
  int m_viewProjMatrix_location = -1;
  int m_viewMatrix_location = -1;
  int m_color_location = -1;
  int m_fogdensity_location = -1;
  int m_controlPoints_location = -1;
};

/// \brief The scene contains the graph that is shown and the camera from which the graph is viewed. It performs
///        all the necessary OpenGL calls to render this graph as if shown from the camera. It assumes
///        to be the only place where OpenGL calls are performed as it does not keep track or reset the internal OpenGL
///        state between calls to render.
/// \todo The select functionality should be removed from this class as it does not rely on OpenGL.
class GLScene : private QOpenGLFunctions_3_3_Core
{
public:
  static const int handleSize = 8;
  static const int arrowheadSize = 12;

  /// \brief An enumeration that identifies the types of objects that
  ///        can be selected. The order determines priority during selection.
  enum SelectableObject
  {
    so_none,     ///< Nothing was selected.
    so_edge,     ///< An edge was selected.
    so_label,    ///< An edge label was selected.
    so_slabel,   ///< An state label was selected.
    so_handle,   ///< An edge handle was selected.
    so_node      ///< A node was selected.
  };

  /// \brief A structure that identifies a selectable object from m_graph.
  struct Selection
  {
    SelectableObject selectionType; ///< The type of object.
    std::size_t index;              ///< The index of the object in m_graph.
  };

  /// \brief Constructor.
  /// \param glwidget The widget where this scene is drawn
  /// \param g The graph that is to be visualised by this object.
  GLScene(QOpenGLWidget& glwidget, Graph::Graph& g);

  /// \brief Initializes all state and data required for rendering.
  void initialize();

  /// \brief Updates the state of the scene.
  void update();

  /// \brief Render the scene.
  void render(QPainter& painter);

  /// \brief Resize the OpenGL viewport.
  void resize(std::size_t width, std::size_t height);
  
  /// \returns The current world size in world coordinates.
  QVector3D size() { return m_worldsize; }
  
  /// \brief Retrieve the object at viewport coordinate (x, y).
  /// \returns A record that represents the selected object.
  Selection select(int x, int y);

  /// \brief Select an object of type @type on (pixel) viewport coordinates x, y.
  /// \returns true if an object was selected.
  bool selectObject(Selection& s, int x, int y, SelectableObject type);

  /// Getters

  bool drawStateLabels() const { return m_drawstatelabels; }
  bool drawTransitionLabels() const { return m_drawtransitionlabels; }
  std::size_t nodeSize() const { return m_size_node; }
  std::size_t fontSize() const { return m_fontsize; }
  float nodeSizeOnScreen() const { return m_size_node * m_device_pixel_ratio; }
  float handleSizeOnScreen() const { return handleSize * m_device_pixel_ratio; }
  float arrowheadSizeOnScreen() const { return arrowheadSize * m_device_pixel_ratio; }

  bool is_threedimensional() { return m_worldsize.z() > 0.0f; }

  ArcballCameraView& camera() { return m_camera;  }

  /// Setters

  void setDrawTransitionLabels(bool drawLabels) { m_drawtransitionlabels = drawLabels; }
  void setDrawStateLabels(bool drawLabels) { m_drawstatelabels = drawLabels; }
  void setDrawStateNumbers(bool drawLabels) { m_drawstatenumbers = drawLabels; }
  void setDrawSelfLoops(bool drawLoops) { m_drawselfloops = drawLoops; }
  void setDrawInitialMarking(bool drawMark) { m_drawinitialmarking = drawMark; }
  void setDrawFog(bool enabled) { m_drawfog = enabled; }

  void setNodeSize(std::size_t size) { m_size_node = size; }
  void setFontSize(std::size_t size) { m_fontsize = size; m_font.setPixelSize(m_fontsize); }
  void setFogDistance(int value) { m_fogdensity = 1.0f / (value + 1); }
  void setDevicePixelRatio(float device_pixel_ratio) { m_device_pixel_ratio = device_pixel_ratio; }

  /// \brief Sets the depth of the cube in which the scene lives (the z-coordinate)
  void setDepth(float depth) { m_worldsize.setZ(depth); }

private:

  /// \brief Renders a single edge.
  /// \param i The index of the edge to render.
  void renderEdge(std::size_t i, const QMatrix4x4& viewProjMatrix);

  /// \brief Renders a single edge handle.
  /// \param i The index of the edge of the handle to render.
  void renderHandle(GLuint i, const QMatrix4x4& viewProjMatrix);

  /// \brief Renders a single node.
  /// \param i The index of the node to render.
  void renderNode(GLuint i, const QMatrix4x4& viewProjMatrix);

  /// \brief Renders a single edge label.
  /// \param i The index of the edge of the label to render.
  void renderTransitionLabel(QPainter& painter, GLuint i);

  /// \brief Renders a single state label.
  /// \param i The index of the state of the label to render.
  void renderStateLabel(QPainter& painter, GLuint i);

  /// \brief Renders a single state number.
  /// \param i The index of the state number to render.
  void renderStateNumber(QPainter& painter, GLuint i);

  /// \returns The amount of fog that a given world coordinate receives.
  float fogAmount(const QVector3D& position);

  /// \returns The color of an object received fogAmount amount of fog.
  QVector3D applyFog(const QVector3D& color, float fogAmount);

  QOpenGLWidget& m_glwidget; /// The widget where this scene is drawn
  Graph::Graph& m_graph;     /// The graph that is being visualised.

  ArcballCameraView m_camera;
  float m_device_pixel_ratio;
  QFont m_font;

  /// \brief The shader to draw uniformly filled three dimensional objects.
  GlobalShader m_global_shader;

  /// \brief The shader to draw arcs, uses control points to position the vertices.
  ArcShader m_arc_shader;

  /// \brief The dimensions of a cube in which the scene lives.
  QVector3D m_worldsize = QVector3D(1000.0f, 1000.0f, 0.0f);

  /// \brief Transition labels are only drawn if this field is true.
  bool m_drawtransitionlabels = true;
  /// \brief State labels are only drawn if this field is true.
  bool m_drawstatelabels      = false;
  /// \brief State numbers are only drawn if this field is true.
  bool m_drawstatenumbers     = false;
  /// \brief Self loops are only drawn if this field is true.
  bool m_drawselfloops        = true;
  /// \brief The initial state is marked if this field is true.
  bool m_drawinitialmarking   = true;
  /// \brief Enable drawing fog.
  bool m_drawfog = false;
  /// \brief The size of a node (radius).
  std::size_t m_size_node     = 20;
  /// \brief The size of the font.
  std::size_t m_fontsize      = 16;

  /// Fog is rendered only if this field is true.
  float m_fogdensity = 0.0005f;

  /// The vertex layout and vertex buffer object for all objects with the 3 float per vertex layout.
  QOpenGLVertexArrayObject m_vertexarray;
  QOpenGLBuffer m_vertexbuffer = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);

  QVector3D m_clearColor = QVector3D(1.0f, 1.0f, 1.0f);
};

#endif // MCRL2_LTSGRAPH_GLSCENE_H
