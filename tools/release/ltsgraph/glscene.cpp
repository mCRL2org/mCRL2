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
#include <cmath>

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
constexpr int RES_NODE_SLICE = 32;

/// \brief Number of vertical planes from which a circle representing a node is constructed.
constexpr int RES_NODE_STACK = 2;

/// \brief Amount of segments in arrowhead cone
constexpr int RES_ARROWHEAD = 16;

/// \brief Amount of segments for edge arc
constexpr int RES_ARC       = 16;

/// This should match the layout of m_vertexbuffer.
constexpr int VERTICES_NODE_BORDER = RES_NODE_SLICE + 1;
constexpr int VERTICES_NODE_SPHERE = RES_NODE_SLICE * RES_NODE_STACK * 2;
constexpr int VERTICES_HINT = 12;
constexpr int VERTICES_HANDLE_BODY = 4;
constexpr int VERTICES_HANDLE_OUTLINE = 4;
constexpr int VERTICES_ARROWHEAD = RES_ARROWHEAD + 1;
constexpr int VERTICES_ARROWHEAD_BASE = RES_ARROWHEAD + 1;
constexpr int VERTICES_ARC = RES_ARC;

constexpr int OFFSET_NODE_BORDER = 0;
constexpr int OFFSET_NODE_SPHERE = OFFSET_NODE_BORDER + VERTICES_NODE_BORDER;
constexpr int OFFSET_HINT       = OFFSET_NODE_SPHERE + VERTICES_NODE_SPHERE;
constexpr int OFFSET_HANDLE_BODY= OFFSET_HINT + VERTICES_HINT;
constexpr int OFFSET_HANDLE_OUTLINE = OFFSET_HANDLE_BODY + VERTICES_HANDLE_BODY;
constexpr int OFFSET_ARROWHEAD  = OFFSET_HANDLE_OUTLINE + VERTICES_HANDLE_OUTLINE;
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

namespace
{

/// \brief A vertex shader that produces a cubic Bezier curve.
const char* g_arcVertexShader =
  "#version 330\n"

  "uniform mat4 g_viewProjMatrix;\n"
  "uniform mat4 g_viewMatrix;\n"

  "uniform vec3 g_controlPoint[4];"

  "uniform float g_density = 0.0001f;"

  "layout(location = 0) in vec3 vertex;\n"

  "out float fogAmount;\n"

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
  "   // Calculate the actual position of the vertex.\n"
  "   vec4 position = vec4(cubicBezier(vertex.x), 1.0f);\n"

  "   // Apply the fog calculation to the resulting position.\n"
  "   float distance = length(g_viewMatrix * position);\n"
  "   fogAmount = (1.0f - exp(-1.0f * pow(distance * g_density, 2)));\n"

  "   // Calculate the actual vertex position in clip space.\n"
  "   gl_Position = g_viewProjMatrix * position;\n"
  "}";

/// \brief A fragment shader that uses g_color as a fill color for the polygons and applies per vertex fogging.
const char* g_arcFragmentShader =
  "#version 330\n "

  "uniform vec3 g_color = vec3(1.0f, 1.0f, 1.0f);\n"

  "in float fogAmount;\n"

  "out vec4 fragColor;\n"

  "void main(void)\n"
  "{\n"
  "   fragColor = vec4(mix(g_color, vec3(1.0f, 1.0f, 1.0f), fogAmount), 1.0);\n"
  "}";
} // unnamed namespace

bool ArcShader::link()
{
  // Here we compile the vertex and fragment shaders and combine the results.
  if (!addShaderFromSourceCode(QOpenGLShader::Vertex, g_arcVertexShader))
  {
    mCRL2log(mcrl2::log::error) << log().toStdString();
    std::abort();
  }

  if (!addShaderFromSourceCode(QOpenGLShader::Fragment, g_arcFragmentShader))
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

  m_viewMatrix_location = uniformLocation("g_viewMatrix");
  if (m_viewMatrix_location == -1)
  {
    mCRL2log(mcrl2::log::warning) << "The arc shader has no uniform named g_viewMatrix.\n";
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

  m_fogdensity_location = uniformLocation("g_density");
  if (m_fogdensity_location == -1)
  {
    mCRL2log(mcrl2::log::warning) << "The arc shader has no uniform named g_density.\n";
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
      nodeborder[i+1] = QVector3D(std::sin(t), std::cos(t), 0.0f);
    }
  }

  // Generate vertices for node (a quad strip drawing a half sphere)
  std::vector<QVector3D> node(VERTICES_NODE_SPHERE);
  {
    float stack = 0;
    float stackd = PI_2 / RES_NODE_STACK;

    std::size_t n = 0;
    for (int j = 0; j < RES_NODE_STACK; ++j, stack += stackd)
    {
      for (int i = 0; i < RES_NODE_SLICE - 1; ++i)
      {
        float slice = i * 2.0f * PI / (RES_NODE_SLICE - 2);
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

  // Generate plus (and minus) hint for exploration mode, we generate 4 triangles as a
  // triangle strip cannot handle the disconnect between the two rectangles of the plus.
  std::vector<QVector3D> hint(12);
  hint[0] = QVector3D(-0.6f,  0.1f, 1.0f);
  hint[1] = QVector3D(-0.6f, -0.1f, 1.0f);
  hint[2] = QVector3D( 0.6f,  0.1f, 1.0f);

  hint[3] = QVector3D( 0.6f,  0.1f, 1.0f);
  hint[4] = QVector3D(-0.6f, -0.1f, 1.0f);
  hint[5] = QVector3D( 0.6f, -0.1f, 1.0f);

  hint[6] = QVector3D(-0.1f,  0.6f, 1.0f);
  hint[7] = QVector3D(-0.1f, -0.6f, 1.0f);
  hint[8] = QVector3D( 0.1f,  0.6f, 1.0f);

  hint[9]  = QVector3D(-0.1f, -0.6f, 1.0f);
  hint[10] = QVector3D( 0.1f, -0.6f, 1.0f);
  hint[11] = QVector3D( 0.1f,  0.6f, 1.0f);

  // Generate vertices for handle (border + fill, both squares)
  std::vector<QVector3D> handle_body(4);
  handle_body[0] = QVector3D(1.0f, -1.0f, 0.3f);
  handle_body[1] = QVector3D(1.0f , 1.0f, 0.3f);
  handle_body[2] = QVector3D(-1.0f , -1.0f, 0.3f);
  handle_body[3] = QVector3D(-1.0f, 1.0f, 0.3f);
  std::vector<QVector3D> handle_outline(4);
  handle_outline[0] = QVector3D(-1.0f, -1.0f, 0.3f);
  handle_outline[1] = QVector3D(1.0f , -1.0f, 0.3f);
  handle_outline[2] = QVector3D(1.0f , 1.0f, 0.3f);
  handle_outline[3] = QVector3D(-1.0f, 1.0f, 0.3f);

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
  vertices.insert(vertices.end(), handle_body.begin(), handle_body.end());
  vertices.insert(vertices.end(), handle_outline.begin(), handle_outline.end());
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

  // Cull polygons that are facing away (back) from the camera, where their front is defined as counter clockwise by default, see glFrontFace, meaning that the
  // vertices that make up a triangle should be oriented counter clockwise to show the triangle.
  glEnable(GL_CULL_FACE);

  // Enable depth testing, so that we don't have to care too much about rendering in the right order.
  glEnable(GL_DEPTH_TEST);

  // Enable multisample antialiasing.
  glEnable(GL_MULTISAMPLE);

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

  if (m_drawfog)
  {
    glEnable(GL_BLEND);
  }

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

//
// GLScene private methods
//

/// \returns The angle in degrees [0, 180] from a given angle in radians [0, PI].
inline static float radiansToDegrees(float radians)
{
  return 180.0f / PI * radians;
}

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

  m_arc_shader.setViewMatrix(m_camera.viewMatrix());
  m_arc_shader.setViewProjMatrix(viewProjMatrix);
  m_arc_shader.setFogDensity(m_drawfog * m_fogdensity);

  QVector3D arcColor(m_graph.handle(i).selected(), 0.0f, 0.0f);
  m_arc_shader.setControlPoints(control);
  m_arc_shader.setColor(arcColor);

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

    float fog = 0.0f;
    if (isVisible(to, fog))
    {
      // Apply the fog color.
      m_global_shader.setColor(applyFog(QVector3D(0, 0, 0), fog));

      QMatrix4x4 worldMatrix;

      // Go to arrowhead position
      worldMatrix.translate(to.x(), to.y(), to.z());

      // Rotate the arrowhead to orient it to the end of the arc.
      QVector3D axis = QVector3D::crossProduct(QVector3D(1, 0, 0), vec);
      worldMatrix.rotate(radiansToDegrees(acos(vec.x())), axis);

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

    // Move the handle to the correct position and with the correct scale.
    QMatrix4x4 worldMatrix;
    worldMatrix.translate(handle.pos());
    worldMatrix.rotate(sphericalBillboard(handle.pos()));
    worldMatrix.scale(handleSizeOnScreen());

    // Update the shader parameters.
    QMatrix4x4 worldViewProjMatrix = viewProjMatrix * worldMatrix;
    m_global_shader.setWorldViewProjMatrix(worldViewProjMatrix);

    // First draw the inner quad.
    m_global_shader.setColor(fill);
    glDrawArrays(GL_TRIANGLE_STRIP, OFFSET_HANDLE_BODY, VERTICES_HANDLE_BODY);

    // Draw the outer lines.
    m_global_shader.setColor(line);
    glDrawArrays(GL_LINE_LOOP, OFFSET_HANDLE_OUTLINE, VERTICES_HANDLE_OUTLINE);
  }
}

void GLScene::renderNode(GLuint i, const QMatrix4x4& viewProjMatrix)
{
  Graph::NodeNode& node = m_graph.node(i);
  QVector3D fill;

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

  float fog = 0.0f;
  if (isVisible(node.pos(), fog)) // Check if these elements are visible.
  {
    QMatrix4x4 worldMatrix;
    worldMatrix.translate(node.pos());
    worldMatrix.rotate(sphericalBillboard(node.pos()));

    QMatrix4x4 nodeMatrix(worldMatrix);
    nodeMatrix.scale(0.5f * nodeSizeOnScreen());
    m_global_shader.setWorldViewProjMatrix(viewProjMatrix * nodeMatrix);

    // Apply fogging the node color and draw the node.
    m_global_shader.setColor(applyFog(fill, fog));
    glDrawArrays(GL_TRIANGLE_STRIP, OFFSET_NODE_SPHERE, VERTICES_NODE_SPHERE);

    // Node stroke color: red when selected, black otherwise. Apply fogging afterwards.
    QVector3D line(0.6f * node.selected(), 0.0f, 0.0f);
    m_global_shader.setColor(applyFog(line, fog));

    // Scale the border such that they are of constant width.
    QMatrix4x4 borderMatrix(worldMatrix);
    float width = 1.0f;
    borderMatrix.scale(0.5f * (nodeSizeOnScreen() + width));
    m_global_shader.setWorldViewProjMatrix(viewProjMatrix * borderMatrix);
    glDrawArrays(GL_TRIANGLE_FAN, OFFSET_NODE_BORDER, VERTICES_NODE_BORDER);

    if (m_graph.hasSelection() && !m_graph.isBridge(i) && m_graph.initialState() != i)
    {
      float s = (fill.x() < 0.5 && fill.y() < 0.5 && fill.z() < 0.5) ? 0.2f : -0.2f;
      QVector3D hint = QVector3D(fill.x() + s, fill.y() + s, fill.z() + s);

      m_global_shader.setColor(hint);

      // When the node is active, which means that its successors are shown in exploration mode, only the "minus" is drawn
      // by omitting the vertical rectangle of the whole "plus" shape.
      glDrawArrays(GL_TRIANGLES, OFFSET_HINT, node.active() ? VERTICES_HINT / 2 : VERTICES_HINT);
    }

    if (node.is_probabilistic())
    {
      QVector3D blue(0.1f, 0.1f, 0.7f);

      m_global_shader.setColor(applyFog(blue, fog));
      glDrawArrays(GL_TRIANGLE_STRIP, RES_NODE_SLICE - 1, RES_NODE_SLICE * RES_NODE_STACK * 2 / 4);
    }
  }
}

/// \brief Converts a QVector3D of floats [0,1] to a QColor object with integers [0,255] for colors.
QColor vectorToColor(const QVector3D& vector)
{
  return QColor(vector.x() * 255, vector.y() * 255, vector.z() * 255);
}

void GLScene::renderTransitionLabel(QPainter& painter, GLuint i)
{
  Graph::Edge edge = m_graph.edge(i);
  if (edge.from() == edge.to() && !m_drawselfloops)
  {
    return;
  }

  Graph::LabelNode& label = m_graph.transitionLabel(i);
  QVector3D fill((std::max)(label.color(0), label.selected()), (std::min)(label.color(1), 1.0f - label.selected()), (std::min)(label.color(2), 1.0f - label.selected()));
  drawCenteredText3D(painter, m_graph.stateLabelstring(label.labelindex()), label.pos(), fill);

}

void GLScene::renderStateLabel(QPainter& painter, GLuint i)
{
  Graph::LabelNode& label = m_graph.stateLabel(i);
  QVector3D color(std::max(label.color(0), label.selected()), std::min(label.color(1), 1.0f - label.selected()), std::min(label.color(2), 1.0f - label.selected()));
  drawCenteredText3D(painter, m_graph.stateLabelstring(label.labelindex()), label.pos(), color);
}

void GLScene::renderStateNumber(QPainter& painter, GLuint i)
{
  Graph::NodeNode& node = m_graph.node(i);
  QVector3D color(0.0f, 0.0f, 0.0f);
  drawCenteredText3D(painter, QString::number(i), node.pos(), color);
}

bool GLScene::isVisible(const QVector3D& position, float& fogamount)
{
  // Should match the vertex shader: fogAmount = (1.0f - exp(-1.0f * pow(distance * g_density, 2)));
  float distance = (m_camera.position() - position).length();
  fogamount = m_drawfog * (1.0f - std::exp(-1.0f * std::pow(distance * m_fogdensity, 2.0f)));
  return (distance < m_camera.viewdistance() && fogamount < 0.99f);
}

/// \returns The given value clamped between a min and a max.
template<typename T>
inline static T clamp(T value, T min, T max)
{
  return std::min(std::max(value, min), max);
}

/// \returns A linear interpolation between a and b using the given value.
inline static QVector3D mix(float value, QVector3D a, QVector3D b)
{
  return (1 - value) * a + (value * b);
}

QVector3D GLScene::applyFog(const QVector3D& color, float fogAmount)
{
  return mix(clamp(fogAmount, 0.0f, 1.0f), color, m_clearColor);
}

QQuaternion GLScene::sphericalBillboard(const QVector3D& position)
{
  // Take the conjugated rotation of the camera to position the node in the right direction
  QQuaternion centerRotation = m_camera.rotation().conjugated();

  // And compensate for the perspective of the camera on the object if its not in the center of the screen
  QVector3D posToCamera = m_camera.position() - position;
  posToCamera.normalize();
  QVector3D camera = m_camera.position();
  camera.normalize();
  // Compute the roration with the cross product and the dot product (aka inproduct)
  QQuaternion perspectiveRotation = QQuaternion::fromAxisAndAngle(QVector3D::crossProduct(camera, posToCamera),
     radiansToDegrees(std::acos(QVector3D::dotProduct(camera, posToCamera))));

  // Return the combination of both rotations
  // NB: the order of this multiplication is important
  return perspectiveRotation * centerRotation;
}

/// Helper functions

/// \brief Renders text, centered around the point at x and y
inline static
void drawCenteredText(QPainter& painter, float x, float y, const QString& text, const QColor& color = Qt::black)
{
  QFontMetrics metrics(painter.font());
  QRect bounds = metrics.boundingRect(text);
  qreal w = bounds.width();
  qreal h = bounds.height();
  painter.setPen(color);
  painter.drawText(x - w / 2, y - h / 2, text);
}

void GLScene::drawCenteredText3D(QPainter& painter, const QString& text, const QVector3D& position, const QVector3D& color)
{
  QVector3D window = m_camera.worldToWindow(position);
  float fog = 0.0f;
  if (!text.isEmpty() && window.z() <= 1.0f && isVisible(position, fog)) // There is text, that is not behind the camera and it is visible.
  {
     QColor qcolor = vectorToColor(color);
     qcolor.setAlpha(255 * (1.0f - fog));

     drawCenteredText(painter,
       window.x(),
       window.y(),
       text,
       qcolor);
  }

}
