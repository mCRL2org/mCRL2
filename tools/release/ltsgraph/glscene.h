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

#include "mcrl2/gui/glu.h"

#include <QGLBuffer>
#include <QGLShaderProgram>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLVertexArrayObject>
#include <QPainter>

#include <vector>

/// \brief Contains all information required to render a given Graph.
class GLScene : private QOpenGLFunctions_3_3_Core
{
public:
  static const int handleSize = 8;
  static const int arrowheadSize = 12;

  /**
   * @brief An enumeration that identifies the types of objects that
            can be selected. The order determines priority during selection.
   */
  enum SelectableObject
  {
    so_none,     ///< Nothing was selected.
    so_edge,     ///< An edge was selected.
    so_label,    ///< An edge label was selected.
    so_slabel,   ///< An state label was selected.
    so_handle,   ///< An edge handle was selected.
    so_node      ///< A node was selected.
  };

  /**
   * @brief A structure that identifies a selectable object from m_graph.
   */
  struct Selection
  {
    SelectableObject selectionType; ///< The type of object.
    std::size_t index;              ///< The index of the object in m_graph.
  };

  /**
   * @brief Constructor.
   * @param glwidget The widget where this scene is drawn
   * @param g The graph that is to be visualised by this object.
   */
  GLScene(QOpenGLWidget& glwidget, Graph::Graph& g);

  /// \brief Initializes all state and data required for rendering.
  void initialize();

  /**
   * @brief Render the scene.
   */
  void render(QPainter& painter);

  /**
   * @brief Resize the OpenGL viewport.
   * @param width The new width for the viewport.
   * @param height The new height for the viewport.
   */
  void resize(std::size_t width, std::size_t height);
  
  /**
   * @brief Set the zoom factor to @e factor, interpolating to the new value
   *        for a number of frames if desired.
   * @param factor The new zoom factor.
   * @param animation The amount of frames to animate. 0 means apply
   *        immediately, 1 applies the change in the next frame. Higher
   *        values cause a gradual change.
   */
  void setZoom(float factor, std::size_t animation = 1);

  /**
   * @brief Set the rotation quaternion to @e rotation, interpolating to the
   *        new value for a number of frames if desired.
   * @param rotation The new rotation.
   * @param animation The amount of frames to animate. 0 means apply
   *        immediately, 1 applies the change in the next frame. Higher
   *        values cause a gradual change.
   */
  void setRotation(const QQuaternion& rotation, std::size_t animation = 1);

  /**
   * @brief Set the translation to @e amount, interpolating to the new value
   *        for a number of frames if desired.
   * @param translation The new translation.
   * @param animation The amount of frames to animate. 0 means apply
   *        immediately, 1 applies the change in the next frame. Higher
   *        values cause a gradual change.
   */
  void setTranslation(const QVector3D& translation, std::size_t animation = 1);

  /**
   * @brief Set the world size to @e size, interpolating to the new value
   *        for a number of frames if desired.
   * @param size The new world size.
   * @param animation The amount of frames to animate. 0 means apply
   *        immediately, 1 applies the change in the next frame. Higher
   *        values cause a gradual change.
   */
  void setSize(const QVector3D& size, std::size_t animation = 1);

  /**
   * @brief Returns the current world size.
   * @return The current world size in world coordinates.
   */
  QVector3D size();
  
  /**
   * @brief Retrieve the object at viewport coordinate (x, y).
   * @returns A record that represents the selected object.
   */
  Selection select(int x, int y);

  /**
   * @brief Select an object of type @type on (pixel) viewport coordinates x, y.
   * @returns true if an object was selected
   */
  bool selectObject(Selection& s, int x, int y, SelectableObject type);

  // Getters and setters

  bool drawStateLabels() const { return m_drawstatelabels; }
  bool drawTransitionLabels() const { return m_drawtransitionlabels; }

  std::size_t nodeSize() const { return m_size_node; }

  std::size_t fontSize() const { return m_fontsize; }

  float nodeSizeOnScreen() const { return m_size_node * m_device_pixel_ratio; }

  float handleSizeOnScreen() const { return handleSize * m_device_pixel_ratio; }

  float arrowheadSizeOnScreen() const { return arrowheadSize * m_device_pixel_ratio; }

  float fogDistance() const { return m_fogdistance; }

  void setDrawTransitionLabels(bool drawLabels) { m_drawtransitionlabels = drawLabels; }
  void setDrawStateLabels(bool drawLabels) { m_drawstatelabels = drawLabels; }
  void setDrawStateNumbers(bool drawLabels) { m_drawstatenumbers = drawLabels; }
  void setDrawSelfLoops(bool drawLoops) { m_drawselfloops = drawLoops; }
  void setDrawInitialMarking(bool drawMark) { m_drawinitialmarking = drawMark; }

  void setDrawFog(bool drawFog) { m_drawfog = drawFog; }
  void setNodeSize(std::size_t size) { m_size_node = size; }
  void setFontSize(std::size_t size) { m_font.setPixelSize(m_fontsize = size); }
  void setFogDistance(float dist) { m_fogdistance = dist; }
  void setDevicePixelRatio(float device_pixel_ratio) { m_device_pixel_ratio = device_pixel_ratio; }
  
  bool is_threedimensional() { return false; }

  CameraAnimation& camera() { return m_camera;  }

private:
  /**
   * @brief Renders a single edge.
   * @param i The index of the edge to render.
   */
  void renderEdge(std::size_t i);

  /**
   * @brief Renders a single edge handle.
   * @param i The index of the edge of the handle to render.
   */
  void renderHandle(GLuint i);

  /**
   * @brief Renders a single node.
   * @param i The index of the node to render.
   */
  void renderNode(GLuint i);

  /**
   * @brief Renders a single edge label.
   * @param i The index of the edge of the label to render.
   */
  void renderTransitionLabel(QPainter& painter, GLuint i);

  /**
   * @brief Renders a single state label.
   * @param i The index of the state of the label to render.
   */
  void renderStateLabel(QPainter& painter, GLuint i);

  /**
   * @brief Renders a single state number.
   * @param i The index of the state number to render.
   */
  void renderStateNumber(QPainter& painter, GLuint i);
  
  QOpenGLWidget& m_glwidget; /// The widget where this scene is drawn
  Graph::Graph& m_graph;     /// The graph that is being visualised.

  /// All opengl related items
  CameraAnimation m_camera;  /// Implementation details of the OpenGL camera handling.
  float m_device_pixel_ratio;
  QFont m_font;

  QGLShaderProgram m_shader;                /// The shader to draw everything.

  /// Store various settings.
  QVector3D m_worldsize;
  bool m_drawtransitionlabels = true;  /// Transition labels are only drawn if this field is true.
  bool m_drawstatelabels      = false; /// State labels are only drawn if this field is true.
  bool m_drawstatenumbers     = false; /// State numbers are only drawn if this field is true.
  bool m_drawselfloops        = true;  /// Self loops are only drawn if this field is true.
  bool m_drawinitialmarking   = true;  /// The initial state is marked if this field is true.
  std::size_t m_size_node     = 20;    /// Variable node size (radius).
  std::size_t m_fontsize      = 16;    /// Variable font size

  bool m_drawfog              = true;    /// Fog is rendered only if this field is true.
  float m_fogdistance         = 5500.0f; /// The distance at which the fog starts

  QOpenGLVertexArrayObject m_node_vao;  /// The vertex layout for nodes.
  QGLBuffer m_node_vbo;
  QGLBuffer m_arrowhead_vbo;
  QGLBuffer m_hint_vbo;
  QGLBuffer m_handle_vbo;
};

#endif // MCRL2_LTSGRAPH_GLSCENE_H
