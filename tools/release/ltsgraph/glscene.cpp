// Author(s): Rimco Boudewijns and Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "glscene.h"

#include "export.h"
#include "mcrl2/utilities/logger.h"

#include <QFont>
#include <QFontMetrics>
#include <QImage>
#include <QPainter>

#include <cassert>

/// Execute the given QT OpenGL function that returns a boolean; logs error and aborts when it failed.
#define MCRL2_QGL_VERIFY(x) \
  if (!x) { mCRL2log(mcrl2::log::error) << #x " failed.\n"; std::abort(); }

/// \brief Checks for OpenGL errors, prints it and aborts.
#define MCRL2_OGL_CHECK() \
  { GLenum result = glGetError(); if (result != GL_NO_ERROR) { mCRL2log(mcrl2::log::error) << "OpenGL error: " << gluErrorString(result) << "\n"; std::abort(); } }

/// Executes x and checks for errors afterwards.
#define MCRL2_OGL_VERIFY(x) \
  x; { GLenum result = glGetError(); if (result != GL_NO_ERROR) { mCRL2log(mcrl2::log::error) << "OpenGL error: " #x " failed with " << gluErrorString(result) << "\n"; std::abort(); } }

constexpr float PI = 3.14159265358979323846f;
constexpr float PI_2 = PI * 0.5f;

/// \brief Number of orthogonal slices from which a circle representing a node is constructed.
constexpr int RES_NODE_SLICE = 64;

/// \brief Number of vertical planes from which a circle representing a node is constructed.
constexpr int RES_NODE_STACK = 1;

/// \brief Amount of segments in arrowhead cone
constexpr int RES_ARROWHEAD = 30;

/// \brief Amount of segments for edge arc
constexpr int RES_ARC       = 20;

/// This should match the layout of m_vertexbuffer.
constexpr int VERTICES_NODE_BORDER = RES_NODE_SLICE + 1;
constexpr int VERTICES_NODE_SPHERE = RES_NODE_SLICE * RES_NODE_STACK * 2;
constexpr int VERTICES_HINT = 4;
constexpr int VERTICES_HANDLE = 4;
constexpr int VERTICES_ARROWHEAD = RES_ARROWHEAD + 1;
constexpr int VERTICES_ARROWHEAD_BASE = RES_ARROWHEAD + 1;
constexpr int VERTICES_ARC = RES_ARC;

constexpr int OFFSET_NODE_BORDER = 0;
constexpr int OFFSET_NODE_SPHERE = OFFSET_NODE_BORDER + VERTICES_NODE_BORDER;
constexpr int OFFSET_HINT       = OFFSET_NODE_SPHERE + VERTICES_NODE_SPHERE;
constexpr int OFFSET_HANDLE     = OFFSET_HINT + VERTICES_HINT;
constexpr int OFFSET_ARROWHEAD  = OFFSET_HANDLE + VERTICES_HANDLE;
constexpr int OFFSET_ARROWHEAD_BASE = OFFSET_ARROWHEAD + VERTICES_ARROWHEAD;
constexpr int OFFSET_ARC        = OFFSET_ARROWHEAD_BASE + VERTICES_ARROWHEAD_BASE;

namespace
{

/// \brief A simple vertex shader just transforms the 3D vertices using a worldViewProjection matrix.
const char* g_vertexShader =
  "#version 330\n"

  "uniform mat4 g_worldViewProjMatrix;\n"

  "layout(location = 0) in vec3 vertex;\n"

  "void main(void)\n"
  "{\n"
  "   gl_Position = g_worldViewProjMatrix * vec4(vertex, 1.0f);\n"
  "}";

/// \brief A simple fragment shader that uses g_color as a fill color for the polygons.
const char* g_fragmentShader =
  "#version 330\n "

  "uniform vec3 g_color = vec3(1.0f, 1.0f, 1.0f);\n"

  "out vec4 fragColor;\n"

  "void main(void)\n"
  "{\n"
  "   fragColor = vec4(g_color, 1.0);\n"
  "}";
} // unnamed namespace

bool GlobalShader::link()
{
  // Here we compile the vertex and fragment shaders and combine the results.
  if (!addShaderFromSourceCode(QOpenGLShader::Vertex, g_vertexShader))
  {
    mCRL2log(mcrl2::log::error) << log().toStdString();
    std::abort();
  }

  if (!addShaderFromSourceCode(QOpenGLShader::Fragment, g_fragmentShader))
  {
    mCRL2log(mcrl2::log::error) << log().toStdString();
    std::abort();
  }

  if (!QOpenGLShaderProgram::link())
  {
    mCRL2log(mcrl2::log::error) << "Could not link shader program:" << log().toStdString();
    std::abort();
  }

  m_worldViewProjMatrix_location = uniformLocation("g_worldViewProjMatrix");
  if (m_worldViewProjMatrix_location == -1)
  {
    mCRL2log(mcrl2::log::warning) << "The global shader has no uniform named g_worldViewProjMatrix.\n";
  }

  m_color_location = uniformLocation("g_color");
  if (m_color_location == -1)
  {
    mCRL2log(mcrl2::log::warning) << "The global shader has no uniform named g_color.\n";
  }

  return true;
}

/// \brief A vertex shader that produces a cubic Bezier curve.
const char* g_arcVertexShader =
  "#version 330\n"

  "uniform mat4 g_viewProjMatrix;\n"

  "uniform vec3 g_controlPoint[4];"

  "layout(location = 0) in vec3 vertex;\n"

  "// Calculates the position on a cubic Bezier curve with 0 <= t <= 1.\n"
  "vec3 cubicBezier(float t)\n"
  "{"
  "   return pow(1 - t, 3) * g_controlPoint[0]"
  "        + 3 * pow(1 - t, 2) * t * g_controlPoint[1]"
  "        + 3 * (1 - t) * pow(t, 2) * g_controlPoint[2]"
  "        + pow(t, 3) * g_controlPoint[3];"
  "}\n"

  "void main(void)\n"
  "{\n"
  "   gl_Position = g_viewProjMatrix * vec4(cubicBezier(vertex.x), 1.0f);\n"
  "}";

bool ArcShader::link()
{
  // Here we compile the vertex and fragment shaders and combine the results.
  if (!addShaderFromSourceCode(QOpenGLShader::Vertex, g_arcVertexShader))
  {
    mCRL2log(mcrl2::log::error) << log().toStdString();
    std::abort();
  }

  if (!addShaderFromSourceCode(QOpenGLShader::Fragment, g_fragmentShader))
  {
    mCRL2log(mcrl2::log::error) << log().toStdString();
    std::abort();
  }

  if (!QOpenGLShaderProgram::link())
  {
    mCRL2log(mcrl2::log::error) << "Could not link shader program:" << log().toStdString();
    std::abort();
  }

  m_viewProjMatrix_location = uniformLocation("g_viewProjMatrix");
  if (m_viewProjMatrix_location == -1)
  {
    mCRL2log(mcrl2::log::warning) << "The arc shader has no uniform named g_viewProjMatrix.\n";
  }

  m_controlPoints_location = uniformLocation("g_controlPoint");
  if (m_controlPoints_location == -1)
  {
    mCRL2log(mcrl2::log::warning) << "The arc shader has no uniform named g_controlPoint.\n";
  }

  m_color_location = uniformLocation("g_color");
  if (m_color_location == -1)
  {
    mCRL2log(mcrl2::log::warning) << "The arc shader has no uniform named g_color.\n";
  }

  return true;
}

GLScene::GLScene(QOpenGLWidget& glwidget, Graph::Graph& g)
  :  m_glwidget(glwidget),
     m_graph(g)
{
  setFontSize(m_fontsize);
}

void GLScene::initialize()
{
  // Makes sure that we can call gl* functions after this.
  initializeOpenGLFunctions();

  // Initialize the global shader.
  m_global_shader.link();

  // Initialize the arc shader.
  m_arc_shader.link();

  // Generate vertices for node border (A slightly larger circle with polygons GL_TRIANGLE_FAN)
  std::vector<QVector3D> nodeborder(VERTICES_NODE_BORDER);
  {
    // The center of the circle, followed by the vertices on the edge.
    nodeborder[0] = QVector3D(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < RES_NODE_SLICE; ++i)
    {
      float t = -i * 2.0f * PI / (RES_NODE_SLICE - 1);
      nodeborder[i+1] = QVector3D(1.2f * std::sin(t), 1.2f * std::cos(t), 0.0f);
    }
  }

  // Generate vertices for node (a quad strip drawing a half sphere)
  std::vector<QVector3D> node(VERTICES_NODE_SPHERE);
  {
    float slice = 0;
    float sliced = 2.0f * PI / VERTICES_NODE_BORDER;
    float stack = 0;
    float stackd = PI_2 / RES_NODE_STACK;

    std::size_t n = 0;
    for (int j = 0; j < RES_NODE_STACK; ++j, stack += stackd)
    {
      for (int i = 0; i < RES_NODE_SLICE - 1; ++i, slice += sliced)
      {
        node[n++] = QVector3D(std::sin((float)(stack + stackd)) * std::sin(slice),
            std::sin((float)(stack + stackd)) * std::cos(slice),
            std::cos((float)(stack + stackd)));
        node[n++] = QVector3D(std::sin(stack) * std::sin(slice),
            std::sin(stack) * std::cos(slice),
            std::cos(stack));
      }

      node[n++] = QVector3D(std::sin((float)(stack + stackd)) * std::sin(0.0f),
          std::sin((float)(stack + stackd)) * std::cos(0.0f),
          std::cos((float)(stack + stackd)));
      node[n++] = QVector3D(std::sin(stack) * std::sin(0.0f),
          std::sin(stack) * std::cos(0.0f),
          std::cos(stack));
    }
  }

  // Generate plus (and minus) hint for exploration mode
  std::vector<QVector3D> hint(4);
  hint[0] = QVector3D(-0.3f, 0.0f, 0.0f);
  hint[1] = QVector3D(0.3f,  0.0f, 0.0f);
  hint[2] = QVector3D(0.0f, -0.3f, 0.0f);
  hint[3] = QVector3D(0.0f,  0.3f, 0.0f);

  // Generate vertices for handle (border + fill, both squares)
  std::vector<QVector3D> handle(4);
  handle[0] = QVector3D(-0.5f, -0.5f, 0.0f);
  handle[1] = QVector3D(0.5f , -0.5f, 0.0f);
  handle[2] = QVector3D(0.5f , 0.5f, 0.0f);
  handle[3] = QVector3D(-0.5f, 0.5f, 0.0f);

  // Generate vertices for arrowhead (a triangle fan drawing a cone)
  std::vector<QVector3D> arrowhead(VERTICES_ARROWHEAD);
  {
    arrowhead[0] = QVector3D(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < RES_ARROWHEAD; ++i)
    {
      float t = -i * 2.0f * PI / (RES_ARROWHEAD - 1);
      arrowhead[i+1] = QVector3D(-1.0f,
          0.3f * std::sin(t),
          0.3f * std::cos(t));
    }
  }

  std::vector<QVector3D> arrowhead_base(VERTICES_ARROWHEAD_BASE);
  {
    arrowhead_base[0] = QVector3D(-1.0f, 0.0f, 0.0f);

    for (int i = 0; i < RES_ARROWHEAD; ++i)
    {
      float t = i * 2.0f * PI / (RES_ARROWHEAD - 1);
      arrowhead_base[i+1] = QVector3D(-1.0f,
          0.3f * std::sin(t),
          0.3f * std::cos(t));
    }
  }

  // Generate vertices for the arc, these will be moved to the correct position by the vertex shader using the x coordinate as t.
  std::vector<QVector3D> arc(VERTICES_ARC);
  {
    for (int i = 0; i < VERTICES_ARC; ++i)
    {
      arc[i] = QVector3D(static_cast<float>(i) / (VERTICES_ARC - 1), 0.0f, 0.0f);
    }
  }

  // We are going to store all vertices in the same buffer and keep track of the offsets.
  std::vector<QVector3D> vertices;
  vertices.insert(vertices.end(), nodeborder.begin(), nodeborder.end());
  vertices.insert(vertices.end(), node.begin(), node.end());
  vertices.insert(vertices.end(), hint.begin(), hint.end());
  vertices.insert(vertices.end(), handle.begin(), handle.end());
  vertices.insert(vertices.end(), arrowhead.begin(), arrowhead.end());
  vertices.insert(vertices.end(), arrowhead_base.begin(), arrowhead_base.end());
  vertices.insert(vertices.end(), arc.begin(), arc.end());

  MCRL2_QGL_VERIFY(m_vertexbuffer.create());
  m_vertexbuffer.bind();
  m_vertexbuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_vertexbuffer.allocate(vertices.data(), vertices.size() * sizeof(QVector3D));

  // The vertex array object stores the layout of the vertex data that we use (vec3 float).
  {
    MCRL2_QGL_VERIFY(m_vertexarray.create());
    MCRL2_OGL_VERIFY(QOpenGLVertexArrayObject::Binder bind_vao(&m_vertexarray));
    MCRL2_OGL_VERIFY(glEnableVertexAttribArray(0));

    m_vertexbuffer.bind();
    MCRL2_OGL_VERIFY(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr));
  }
}

void GLScene::update()
{
  m_camera.update();
}

void GLScene::render(QPainter& painter)
{
  // Qt: Direct OpenGL commands can still be issued. However, you must make sure these are enclosed by a call to the painter's beginNativePainting() and endNativePainting().
  painter.beginNativePainting();

  // Cull polygons that are facing away from the camera.
  glEnable(GL_CULL_FACE);

  // Enable depth testing, so that we don't have to care too much about rendering in the right order.
  glEnable(GL_DEPTH_TEST);

  QColor clear(Qt::white);
  glClearColor(clear.red(), clear.green(), clear.blue(), 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_graph.lock(GRAPH_LOCK_TRACE); // enter critical section

  bool sel = m_graph.hasSelection();
  std::size_t nodeCount = sel ? m_graph.selectionNodeCount() : m_graph.nodeCount();
  std::size_t edgeCount = sel ? m_graph.selectionEdgeCount() : m_graph.edgeCount();

  // All other objects share the same shader and vertex layout.
  m_global_shader.bind();
  m_vertexarray.bind();

  QMatrix4x4 viewProjMatrix = m_camera.projectionMatrix() *  m_camera.viewMatrix();

  for (std::size_t i = 0; i < nodeCount; ++i)
  {
    renderNode(sel ? m_graph.selectionNode(i) : i, viewProjMatrix);
  }

  // All arrowheads and arcs are black.
  QVector3D arrowhead_color(0.0f, 0.0f, 0.0f);
  m_global_shader.setColor(arrowhead_color);

  for (std::size_t i = 0; i < edgeCount; ++i)
  {
    renderEdge(sel ? m_graph.selectionEdge(i) : i, viewProjMatrix);
  }

  for (std::size_t i = 0; i < edgeCount; ++i)
  {
    renderHandle(sel ? m_graph.selectionEdge(i) : i, viewProjMatrix);
  }

  painter.endNativePainting();

  // Use the painter to render the remaining text.
  glDisable(GL_DEPTH_TEST);

  painter.setFont(m_font);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  for (std::size_t i = 0; i < nodeCount; ++i)
  {
    if (m_drawstatenumbers)
    {
      renderStateNumber(painter, sel ? m_graph.selectionNode(i) : i);
    }

    if (m_drawstatelabels)
    {
      renderStateLabel(painter, sel ? m_graph.selectionNode(i) : i);
    }
  }

  if (m_drawtransitionlabels)
  {
    for (std::size_t i = 0; i < edgeCount; ++i)
    {
      renderTransitionLabel(painter, sel ? m_graph.selectionEdge(i) : i);
    }
  }

  m_graph.unlock(GRAPH_LOCK_TRACE); // exit critical section

  // Check that no OpenGL error has occured.
  MCRL2_OGL_CHECK();
}

GLScene::Selection GLScene::select(int x, int y)
{
  Selection s{so_none, 0};
  selectObject(s, x, y, so_node)
    || selectObject(s, x, y, so_handle)
    || selectObject(s, x, y, so_label)
    || selectObject(s, x, y, so_slabel)
    ;
  return s;
}

static bool isClose(int x, int y, const QVector3D& pos, float threshold, float& bestZ)
{
  float distance = std::sqrt(std::pow(pos.x() - x, 2) + std::pow(pos.y() - y, 2));
  if (distance < threshold && pos.z() < bestZ)
  {
    bestZ = pos.z();
    return true;
  }
  return false;
}

static bool isOnText(int x, int y, const QString& text, const QVector3D& eye,
    const QFontMetrics& metrics)
{
  if (text.isEmpty())
  {
    return false;
  }
  QRect bounds = metrics.boundingRect(text);
  int w = bounds.width();
  int h = bounds.height();
  // TODO why 4 / 3
  QRect textbox = {(int)eye.x() - w / 2, (int)eye.y() - 4 * h / 3, w, h};
  return textbox.contains(x, y);
}

bool GLScene::selectObject(GLScene::Selection& s,
                           int x,
                           int y,
                           SelectableObject type)
{
  float bestZ = INFINITY;
  bool sel = m_graph.hasSelection();
  std::size_t nodeCount = sel ? m_graph.selectionNodeCount() : m_graph.nodeCount();
  std::size_t edgeCount = sel ? m_graph.selectionEdgeCount() : m_graph.edgeCount();

  QFontMetrics metrics(m_font);
  switch (type)
  {
  case so_node:
  {
    float radius = nodeSizeOnScreen() / 2;
    for (std::size_t i = 0; i < nodeCount; i++)
    {
      std::size_t index = sel ? m_graph.selectionNode(i) : i;
      if (isClose(x, y, m_camera.worldToWindow(m_graph.node(index).pos()), radius, bestZ))
      {
        s.selectionType = type;
        s.index = index;
      }
    }
    break;
  }
  case so_handle:
  {
    float radius = handleSizeOnScreen() * 2;
    for (std::size_t i = 0; i < edgeCount; i++)
    {
      std::size_t index = sel ? m_graph.selectionEdge(i) : i;
      if (isClose(x, y, m_camera.worldToWindow(m_graph.handle(index).pos()), radius, bestZ))
      {
        s.selectionType = type;
        s.index = index;
      }
    }
    break;
  }
  case so_label:
  {
    for (std::size_t i = 0; i < edgeCount; i++)
    {
      std::size_t index = sel ? m_graph.selectionEdge(i) : i;
      const Graph::LabelNode& label = m_graph.transitionLabel(index);
      const QVector3D& eye = m_camera.worldToWindow(label.pos());
      const QString& labelstring = m_graph.transitionLabelstring(label.labelindex());
      if (isOnText(x, y, labelstring, eye, metrics))
      {
        s.selectionType = type;
        s.index = index;
        break;
      }
    }
    break;
  }
  case so_slabel:
  {
    for (std::size_t i = 0; i < nodeCount; i++)
    {
      std::size_t index = sel ? m_graph.selectionNode(i) : i;
      const Graph::LabelNode& label = m_graph.stateLabel(index);
      const QVector3D& eye = m_camera.worldToWindow(label.pos());
      const QString& labelstring = m_graph.stateLabelstring(label.labelindex());
      if (isOnText(x, y - nodeSizeOnScreen(), labelstring, eye, metrics))
      {
        s.selectionType = type;
        s.index = index;
        break;
      }
    }
    break;
  }
  case so_edge:
  case so_none:
    Q_UNREACHABLE();
  }

  return s.selectionType != so_none;
}

/// \brief Renders text, centered around the point at x and y
inline
QRect drawCenteredText(QPainter& painter, float x, float y, const QString& text, const QColor& color = Qt::black)
{
  QFontMetrics metrics(painter.font());
  QRect bounds = metrics.boundingRect(text);
  qreal w = bounds.width();
  qreal h = bounds.height();
  painter.setPen(color);
  painter.drawText(x - w / 2, y - h / 2, text);
  return bounds;
}

//
// GLScene private methods
//

void GLScene::renderEdge(std::size_t i, const QMatrix4x4& viewProjMatrix)
{
  Graph::Edge edge = m_graph.edge(i);

  // We define four control points of a spline.
  std::array<QVector3D, 4> control;
  QVector3D& from = control[0];
  QVector3D& to = control[3];

  // Calculate control points from handle
  QVector3D via = m_graph.handle(i).pos();
  from = m_graph.node(edge.from()).pos();
  to = m_graph.node(edge.to()).pos();

  control[1] = via * 1.33333f - (from + to) / 6.0f;

  if (edge.from() == edge.to())
  {
    // For self-loops, ctrl[1] and ctrl[2] need to lie apart, we'll spread
    // them in x-y direction.
    if (!m_drawselfloops)
    {
      return;
    }
    QVector3D diff = control[1] - control[0];
    diff = QVector3D::crossProduct(diff, QVector3D(0, 0, 1));
    diff = diff * ((via - from).length() / (diff.length() * 2.0));
    control[1] = control[1] + diff;
    control[2] = control[2] - diff;
  }
  else
  {
    // Else we use the same position for both points (effectively a quadratic curve).
    control[2] = control[1];
  }

  // Use the arc shader to draw the arcs.
  m_arc_shader.bind();

  QVector3D arcColor(m_graph.handle(i).selected(), 0.0f, 0.0f);
  m_arc_shader.setControlPoints(control);
  m_arc_shader.setColor(arcColor);
  m_arc_shader.setViewProjMatrix(viewProjMatrix);

  glDrawArrays(GL_LINE_STRIP, OFFSET_ARC, VERTICES_ARC);

  // Reset the shader.
  m_global_shader.bind();

  // Rotate to match the orientation of the arc
  QVector3D vec = to - control[2];

  // If to == ctrl[2], then something odd is going on. We'll just
  // make the executive decision not to draw the arrowhead then, as it
  // will just clutter the image.
  if (vec.length() > 0)
  {
    vec.normalize();

    QMatrix4x4 worldMatrix;

    // Go to arrowhead position
    worldMatrix.translate(to.x(), to.y(), to.z());

    // Rotate the arrowhead to orient it to the end of the arc.
    QVector3D axis = QVector3D::crossProduct(QVector3D(1, 0, 0), vec);
    float degrees = (180.0f / PI) * acos(vec.x());
    worldMatrix.rotate(degrees, axis);

    // Move the arrowhead outside of the node.
    worldMatrix.translate(-0.5f * nodeSizeOnScreen(), 0.0f, 0.0f);

    // Scale it according to its size.
    worldMatrix.scale(arrowheadSizeOnScreen());

    QMatrix4x4 worldViewProjMatrix = viewProjMatrix * worldMatrix;

    // Draw the arrow head
    m_global_shader.setWorldViewProjMatrix(worldViewProjMatrix);
    glDrawArrays(GL_TRIANGLE_FAN, OFFSET_ARROWHEAD, VERTICES_ARROWHEAD);

    // Draw a circle to enclose the arrowhead.
    glDrawArrays(GL_TRIANGLE_FAN, OFFSET_ARROWHEAD_BASE, VERTICES_ARROWHEAD_BASE);
  }
}

void GLScene::renderHandle(GLuint i, const QMatrix4x4& viewProjMatrix)
{
  Graph::Node& handle = m_graph.handle(i);
  if (handle.selected() > 0.1 || handle.locked())
  {
    QVector3D line(2 * handle.selected() - 1.0f, 0.0f, 0.0f);
    QVector3D fill(1.0f, 1.0f, 1.0f);

    if (handle.locked())
    {
      fill = QVector3D(0.7f, 0.7f, 0.7f);
    }

    //m_camera.billboard_cylindrical(handle.pos());

    // Move the handle to the correct position and with the correct scale.
    QMatrix4x4 worldMatrix;
    worldMatrix.translate(handle.pos());
    worldMatrix.scale(handleSizeOnScreen());

    // Update the shader parameters.
    QMatrix4x4 worldViewProjMatrix = viewProjMatrix * worldMatrix;
    m_global_shader.setWorldViewProjMatrix(worldViewProjMatrix);

    // First draw the inner quad.
    m_global_shader.setColor(fill);
    glDrawArrays(GL_TRIANGLE_STRIP, OFFSET_HANDLE, VERTICES_HANDLE);

    // Draw the outer lines.
    m_global_shader.setColor(line);
    glDrawArrays(GL_LINE_LOOP, OFFSET_HANDLE, VERTICES_HANDLE);
  }
}

void GLScene::renderNode(GLuint i, const QMatrix4x4& viewProjMatrix)
{
  Graph::NodeNode& node = m_graph.node(i);
  QVector3D fill;
  //Color4f hint;

  // Node stroke color: red when selected, black otherwise
  QVector3D line(0.6f * node.selected(), 0.0f, 0.0f);

  bool mark = (m_graph.initialState() == i) && m_drawinitialmarking;
  if (mark) // Initial node fill color: green or dark green (locked)
  {
    if (node.locked())
    {
      fill = QVector3D(0.1f, 0.7f, 0.1f);
    }
    else
    {
      fill = QVector3D(0.1f, 1.0f, 0.1f);
    }
  }
  else // Normal node fill color: node color or darkened node color (locked)
  {
    if (node.locked())
    {
      fill = QVector3D(0.7f * node.color()[0], 0.7f * node.color()[1], 0.7f * node.color()[2]);
    }
    else
    {
      GLfloat* color = node.color();
      assert(color != nullptr);
      fill = QVector3D(color[0], color[1], color[2]);
    }
  }

  //if (node.is_probabilistic())
  //{
  // Color3f blue = Color3f(0.1f, 0.1f, 0.7f);
  //  glDrawArrays(GL_TRIANGLE_STRIP, RES_NODE_SLICE - 1, RES_NODE_SLICE * RES_NODE_STACK * 2 / 4);
  //}

  //m_camera.billboard_spherical(node.pos());
  QMatrix4x4 worldMatrix;
  worldMatrix.translate(node.pos());
  worldMatrix.scale(0.5f * nodeSizeOnScreen());

  QMatrix4x4 worldViewProjMatrix = viewProjMatrix * worldMatrix;

  m_global_shader.setWorldViewProjMatrix(worldViewProjMatrix);

  m_global_shader.setColor(fill);
  glDrawArrays(GL_TRIANGLE_STRIP, OFFSET_NODE_SPHERE, VERTICES_NODE_SPHERE);

  m_global_shader.setColor(line);
  glDrawArrays(GL_TRIANGLE_FAN, OFFSET_NODE_BORDER, VERTICES_NODE_BORDER);

 /* if (m_graph.hasSelection() && !m_graph.isBridge(i) && m_graph.initialState() != i)
  {
    float s = (fill.r < 0.5 && fill.g < 0.5 && fill.b < 0.5) ? 0.2f : -0.2f;
    hint = Color4f(fill.r + s, fill.g + s, fill.b + s, true);

    drawWhetherNodeCanBeCollapsedOrExpanded(m_vertexdata, hint, node.active());
  }*/
}

void GLScene::renderTransitionLabel(QPainter& painter, GLuint i)
{
  Graph::Edge edge = m_graph.edge(i);
  if (edge.from() == edge.to() && !m_drawselfloops)
  {
    return;
  }

  Graph::LabelNode& label = m_graph.transitionLabel(i);

  if (!m_graph.transitionLabelstring(label.labelindex()).isEmpty())
  {
    QColor color(std::max(label.color(0), label.selected()), std::min(label.color(1), 1.0f - label.selected()), std::min(label.color(2), 1.0f - label.selected()));

    QVector3D windowCoordinates = m_camera.worldToWindow(label.pos());
    const QString& labelstring = m_graph.transitionLabelstring(label.labelindex());
    drawCenteredText(painter, windowCoordinates.x(), windowCoordinates.y(), labelstring, color);
  }
}

void GLScene::renderStateLabel(QPainter& painter, GLuint i)
{
  Graph::LabelNode& label = m_graph.stateLabel(i);
  if (!m_graph.stateLabelstring(label.labelindex()).isEmpty())
  {
    QVector3D window = m_camera.worldToWindow(label.pos());

    QColor color(std::max(label.color(0), label.selected()), std::min(label.color(1), 1.0f - label.selected()), std::min(label.color(2), 1.0f - label.selected()));
    drawCenteredText(painter, window.x(), window.y() + nodeSizeOnScreen(), m_graph.stateLabelstring(label.labelindex()), color);
  }
}

void GLScene::renderStateNumber(QPainter& painter, GLuint i)
{
  Graph::NodeNode& node = m_graph.node(i);
  QVector3D eye = m_camera.worldToWindow(node.pos());
  drawCenteredText(painter, eye.x(), eye.y(), QString::number(i));
}
