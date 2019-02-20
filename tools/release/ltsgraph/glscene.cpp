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
#include "mcrl2/gui/arcball.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/gui/glu.h"

#include <QFont>
#include <QFontMetrics>
#include <QImage>
#include <QPainter>
#include <QScreen>
#include <QtOpenGL>

#include <cassert>
#include <cmath>
#include <cstdio>

constexpr int RES_ARROWHEAD = 30; /// Amount of segments in arrowhead cone
constexpr int RES_ARC       = 20; /// Amount of segments for edge arc
constexpr int RES_NODE_SLICE = 64; /// Number of segments from which a circle representing a node is constructed.
constexpr int RES_NODE_STACK = 4;

/// Execute the given QT OpenGL function that returns a boolean; logs error and aborts when it failed.
#define MCRL2_QGL_VERIFY(x) \
  if (!x) { mCRL2log(mcrl2::log::error) << #x " failed.\n"; std::abort(); }

/// \brief Checks for OpenGL errors, prints it and aborts.
#define MCRL2_OGL_CHECK() \
  { GLenum result = glGetError(); if (result != GL_NO_ERROR) { mCRL2log(mcrl2::log::error) << "OpenGL error: " << gluErrorString(result) << "\n"; std::abort(); } }

/// Executes x and checks for errors afterwards.
#define MCRL2_OGL_VERIFY(x) \
  x; { GLenum result = glGetError(); if (result != GL_NO_ERROR) { mCRL2log(mcrl2::log::error) << "OpenGL error: " #x " failed with " << gluErrorString(result) << "\n"; std::abort(); } }

namespace
{
const char* g_vertexShader =
  "#version 330\n "

  "layout(location = 0) in vec4 vertex;"

  "void main( void )"
  "{"
  "   gl_Position = vertex;"
  "}";

const char* g_fragmentShader =
  "#version 330\n "

  "layout(location = 0, index = 0) out vec4 fragColor;"

  "void main( void )"
  "{"
  "   fragColor = vec4( 1.0, 0.0, 0.0, 1.0 );"
  "}";
} // unnamed namespace

GLScene::GLScene(QOpenGLWidget& glwidget, Graph::Graph& g)
  :  m_glwidget(glwidget),
     m_graph(g)
{
  setFontSize(m_fontsize);
  setFogDistance(m_fogdistance);
}

void GLScene::initialize()
{
  // Makes sure that we can call gl* functions after this.
  initializeOpenGLFunctions();

  // We are going to use vertex buffer and vertex array objects.
  glEnableClientState(GL_VERTEX_ARRAY);

  // Here we compile the vertex and fragment shaders and combine the results.
  if (!m_shader.addShaderFromSourceCode(QOpenGLShader::Vertex, g_vertexShader))
  {
    mCRL2log(mcrl2::log::error) << m_shader.log().toStdString();
    std::abort();
  }

  if (!m_shader.addShaderFromSourceCode(QOpenGLShader::Fragment, g_fragmentShader))
  {
    mCRL2log(mcrl2::log::error) << m_shader.log().toStdString();
    std::abort();
  }

  if (!m_shader.link())
  {
    mCRL2log(mcrl2::log::error) << "Could not link shader program:" << m_shader.log().toStdString();
    std::abort();
  }

  std::vector<QVector3D> node(RES_NODE_SLICE - 1 + RES_NODE_SLICE * RES_NODE_STACK * 2);

  // Generate vertices for node border (a line loop drawing a circle)
  float slice = 0;
  float sliced = (float)(2.0 * M_PI / (RES_NODE_SLICE - 1));
  float stack = 0;
  float stackd = (float)(M_PI_2 / RES_NODE_STACK);

  for (int i = 0; i < RES_NODE_SLICE - 1; ++i, slice += sliced)
  {
    node[i] = QVector3D(std::sin(slice), std::cos(slice), 0.1f);
  }

  // Generate vertices for node (a quad strip drawing a half sphere)
  slice = 0;
  std::size_t n = RES_NODE_SLICE - 1;
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

  MCRL2_QGL_VERIFY(m_node_vbo.create());
  m_node_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
  MCRL2_QGL_VERIFY(m_node_vbo.bind());
  m_node_vbo.allocate(node.data(), node.size() * sizeof(QVector3D));

  // The vertex array object stores the layout of the vertex data that we use (vec3 float).
  MCRL2_QGL_VERIFY(m_node_vao.create());
  m_node_vao.bind();
  MCRL2_OGL_VERIFY(glEnableVertexAttribArray(0));
  m_node_vbo.bind();
  MCRL2_OGL_VERIFY(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr));

  // Generate plus (and minus) hint for exploration mode
  QVector3D m_hint[4]; // The plus and minus "hints?".
  QVector3D m_handle[4]; //
  m_hint[0] = QVector3D(-0.3f, 0.0f, 0.0f);
  m_hint[1] = QVector3D(0.3f,  0.0f, 0.0f);
  m_hint[2] = QVector3D(0.0f, -0.3f, 0.0f);
  m_hint[3] = QVector3D(0.0f,  0.3f, 0.0f);

  // Generate vertices for handle (border + fill, both squares)
  m_handle[0] = QVector3D(-0.5f, -0.5f, 0.0f);
  m_handle[1] = QVector3D(0.5f , -0.5f, 0.0f);
  m_handle[2] = QVector3D(0.5f , 0.5f, 0.0f);
  m_handle[3] = QVector3D(-0.5f, 0.5f, 0.0f);

  // Generate vertices for arrowhead (a triangle fan drawing a cone)
  std::vector<QVector3D> arrowhead(RES_ARROWHEAD + 1);
  arrowhead[0] = QVector3D(0.0f, 0.0f, 0.0f);
  float diff = (float)(M_PI / 20.0f);
  float t = 0.0f;

  for (int i = 1; i < RES_ARROWHEAD; ++i, t += diff)
  {
    arrowhead[i] = QVector3D(-1.0f,
        0.3f * std::sin(t),
        0.3f * std::cos(t));
  }

  arrowhead[RES_ARROWHEAD] = QVector3D(-1.0f,
      0.3f * std::sin(0.0f),
      0.3f * std::cos(0.0f));

  MCRL2_QGL_VERIFY(m_arrowhead_vbo.create());
  m_arrowhead_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_arrowhead_vbo.allocate(arrowhead.data(), arrowhead.size() * sizeof(QVector3D));
}

void GLScene::render(QPainter& painter)
{
  // Update the state
  m_camera.update();

  // Doc: Direct OpenGL commands can still be issued. However, you must make sure these are enclosed by a call to the painter's beginNativePainting() and endNativePainting().
  painter.begin(&m_glwidget);
  painter.beginNativePainting();

  QColor clear(Qt::white);
  glClearColor(clear.red(), clear.green(), clear.blue(), 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_graph.lock(GRAPH_LOCK_TRACE); // enter critical section

  bool sel = m_graph.hasSelection();
  std::size_t nodeCount = sel ? m_graph.selectionNodeCount() : m_graph.nodeCount();
  std::size_t edgeCount = sel ? m_graph.selectionEdgeCount() : m_graph.edgeCount();

  // All nodes share the same shader.
  m_shader.bind();

  for (std::size_t i = 0; i < nodeCount; ++i)
  {
    renderNode(sel ? m_graph.selectionNode(i) : i);
  }

  /*for (std::size_t i = 0; i < edgeCount; ++i) {
    renderEdge(sel ? m_graph.selectionEdge(i) : i);
  }

  for (std::size_t i = 0; i < edgeCount; ++i)
  {
    renderHandle(sel ? m_graph.selectionEdge(i) : i);
  }

  // text drawing follows
  */

  painter.endNativePainting();

  // Use the painter to render the remaining text.
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

  painter.end();

  m_graph.unlock(GRAPH_LOCK_TRACE); // exit critical section

  MCRL2_OGL_CHECK();
}

QVector3D GLScene::size()
{
  return m_worldsize;
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
// Helper functions for drawing.
//

/*inline
void drawHandle(const VertexData& data, const Color3f& line, const Color3f& fill)
{
  glVertexPointer(3, GL_FLOAT, 0, data.handle());
  glColor3fv(fill);
  glDrawArrays(GL_QUADS, 0, 4);
  glColor3fv(line);
  glDrawArrays(GL_LINE_LOOP, 0, 4);
}

inline
void drawNode(const VertexData& data, const Color3f& line, const Color3f& fill, bool translucent, bool probabilistic)
{
  glPushAttrib(GL_LINE_BIT);
  glLineWidth(2.0);

  if (probabilistic)
  {
    Color3f blue = Color3f(0.1f, 0.1f, 0.7f);
    glColor3fv(blue);
    //glVertexPointer(3, GL_FLOAT, 4*3, data.node());
    glDrawArrays(GL_TRIANGLE_STRIP, RES_NODE_SLICE - 1, RES_NODE_SLICE * RES_NODE_STACK * 2 / 4);
  }

  //glVertexPointer(3, GL_FLOAT, 0, data.node());

  float alpha = translucent ? 0.5 : 1.0;
  glColor4fv(Color4f(fill, alpha));


  // disable the depth mask temporarily for drawing the border of a node
  // dragging an initial state in 2D mode over other nodes looks less weird this way
  // BUT not disabling the depth mask here causes some strange issue on Mac OS
  glDepthMask(GL_FALSE);

  glColor4fv(Color4f(line, alpha));
  glDrawArrays(GL_LINE_LOOP, 0, RES_NODE_SLICE - 1);

  // see above
  glDepthMask(GL_TRUE);
  glPopAttrib();
}


inline
void drawWhetherNodeCanBeCollapsedOrExpanded(const VertexData& data, const Color4f& line, bool active)
{
  glPushAttrib(GL_LINE_BIT);
  glLineWidth(2.5);
  glVertexPointer(3, GL_FLOAT, 0, data.hint());
  glDepthMask(GL_FALSE);
  glColor4fv(line);
  glDrawArrays(GL_LINES, 0, active ? 2 : 4); // Plus or half a plus (minus)
  glDepthMask(GL_TRUE);
  glPopAttrib();
}

inline
void drawArrowHead(const VertexData& data)
{
  glVertexPointer(3, GL_FLOAT, 0, data.arrowhead());
  glDrawArrays(GL_TRIANGLE_FAN, 0, RES_ARROWHEAD + 1);
}

inline
void drawArc(const QVector3D controlpoints[4])
{
  glDepthMask(GL_FALSE);

  float cp[3 * 4];
  for (int i = 0; i < 4; i++)
  {
    cp[3 * i + 0] = controlpoints[i].x();
    cp[3 * i + 1] = controlpoints[i].y();
    cp[3 * i + 2] = controlpoints[i].z();
  }
  glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, cp);
  glEnable(GL_MAP1_VERTEX_3);
  glMapGrid1f(RES_ARC, 0, 1);
  glEvalMesh1(GL_LINE, 0, RES_ARC);

  glDepthMask(GL_TRUE);
}*/

/**
 * @brief Renders text, centered around the point at x and y
 */
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

void GLScene::renderEdge(std::size_t i)
{
  Graph::Edge edge = m_graph.edge(i);
  QVector3D ctrl[4];
  QVector3D& from = ctrl[0];
  QVector3D& to = ctrl[3];
  QVector3D via = m_graph.handle(i).pos();
  from = m_graph.node(edge.from()).pos();
  to = m_graph.node(edge.to()).pos();

  // Calculate control points from handle
  ctrl[1] = via * 1.33333f - (from + to) / 6.0f;
  ctrl[2] = ctrl[1];

  // For self-loops, ctrl[1] and ctrl[2] need to lie apart, we'll spread
  // them in x-y direction.
  if (edge.from() == edge.to())
  {
    if (!m_drawselfloops) {
      return;
    }
    QVector3D diff = ctrl[1] - ctrl[0];
    diff = QVector3D::crossProduct(diff, QVector3D(0, 0, 1));
    diff = diff * ((via - from).length() / (diff.length() * 2.0));
    ctrl[1] = ctrl[1] + diff;
    ctrl[2] = ctrl[2] - diff;
  }

  glPushMatrix();

  glColor3f(m_graph.handle(i).selected(), 0.0, 0.0);

  // Draw the arc
  //drawArc(ctrl);

  // Move the arrowhead outside of the node.
  glTranslatef(-0.5f * nodeSizeOnScreen(), 0.0f, 0.0f);

  // Scale it according to its size.
  float scale = arrowheadSizeOnScreen();
  glScalef(scale, scale, scale);

  // Go to arrowhead position
  glTranslatef(to.x(), to.y(), to.z());

  // Rotate to match the orientation of the arc
  QVector3D vec = to - ctrl[2];
  // If ctrl[3] == ctrl[2], then something odd is going on. We'll just
  // make the executive decision not to draw the arrowhead then, as it
  // will just clutter the image.
  if (vec.length() > 0)
  {
    vec /= vec.length();

    QVector3D axis = QVector3D::crossProduct(QVector3D(1, 0, 0), vec);
    float angle = acos(vec.x());
    glRotatef(angle * 180.0 / M_PI, axis.x(), axis.y(), axis.z());

    // Draw the arrow head
    //drawArrowHead(m_vertexdata);
  }

  glPopMatrix();
}

void GLScene::renderNode(GLuint i)
{
  Graph::NodeNode& node = m_graph.node(i);
  /*Color3f fill;
  Color3f line;
  Color4f hint;

  // Node stroke color: red when selected, black otherwise
  line = Color3f(0.6f * node.selected(), 0.0f, 0.0f);

  bool mark = (m_graph.initialState() == i) && m_drawinitialmarking;
  if (mark) // Initial node fill color: green or dark green (locked)
  {
    if (node.locked()) {
      fill = Color3f(0.1f, 0.7f, 0.1f);
    }
    else {
      fill = Color3f(0.1f, 1.0f, 0.1f);
    }
  }
  else // Normal node fill color: node color or darkened node color (locked)
  {
    if (node.locked()) {
      fill = Color3f(0.7f * node.color()[0], 0.7f * node.color()[1], 0.7f * node.color()[2]);
    }
    else {
      fill = node.color();
    }
  }*/

  //m_camera.billboard_spherical(node.pos());
  QMatrix4x4 modelMatrix;
  modelMatrix.scale(0.5f * nodeSizeOnScreen());

  m_node_vao.bind();
  MCRL2_OGL_VERIFY(glDrawArrays(GL_TRIANGLE_STRIP, RES_NODE_SLICE - 1, RES_NODE_SLICE * RES_NODE_STACK * 2));

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
    QVector3D eye = m_camera.worldToWindow(label.pos());

    QColor color(std::max(label.color(0), label.selected()), std::min(label.color(1), 1.0f - label.selected()), std::min(label.color(2), 1.0f - label.selected()));
    drawCenteredText(painter, eye.x(), eye.y() + nodeSizeOnScreen(), m_graph.stateLabelstring(label.labelindex()), color);
  }
}

void GLScene::renderStateNumber(QPainter& painter, GLuint i)
{
  Graph::NodeNode& node = m_graph.node(i);
  QVector3D eye = m_camera.worldToWindow(node.pos());
  drawCenteredText(painter, eye.x(), eye.y(), QString::number(i));
}

void GLScene::renderHandle(GLuint i)
{
  Graph::Node& handle = m_graph.handle(i);
  if (handle.selected() > 0.1 || handle.locked())
  {
    Color3f line(2 * handle.selected() - 1.0f, 0.0f, 0.0f);
    Color3f fill(1.0f, 1.0f, 1.0f);
    if (handle.locked()) {
      fill = Color3f(0.7f, 0.7f, 0.7f);
    }

    glDisable(GL_LINE_SMOOTH);
    glPushMatrix();

    //m_camera.billboard_cylindrical(handle.pos());
    float scale = handleSizeOnScreen();
    glScalef(scale, scale, scale);
    //drawHandle(m_vertexdata, line, fill);

    glPopMatrix();
    glEnable(GL_LINE_SMOOTH);
  }
}
