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
#include "mcrl2/gui/workarounds.h"
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
  if (!x) { mCRL2log(mcrl2::log::debug) << #x " failed.\n"; std::abort(); }

/// Executes x and checks for glGetError
#define MCRL2_OGL_CHECK() \
  { GLenum result = glGetError(); if (result != GL_NO_ERROR) { mCRL2log(mcrl2::log::error) << "OpenGL error: " << gluErrorString(result) << "\n"; } }

void VertexData::initialize()
{
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
  m_node_vbo.setUsagePattern(QGLBuffer::StaticDraw);
  m_node_vbo.allocate(node.data(), node.size());

  // Generate plus (and minus) hint for exploration mode
  QVector3D m_hint[4]; // The plus and minus "hints?".
  QVector3D m_handle[4]; //
  m_hint[0] = QVector3D(-0.3, 0.0, 0.0f);
  m_hint[1] = QVector3D(0.3,  0.0, 0.0f);
  m_hint[2] = QVector3D(0.0, -0.3, 0.0f);
  m_hint[3] = QVector3D(0.0,  0.3, 0.0f);

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
  m_arrowhead_vbo.setUsagePattern(QGLBuffer::StaticDraw);
  m_arrowhead_vbo.allocate(arrowhead.data(), arrowhead.size());
}

//
// GLScene public methods
//

namespace
{
const char* g_vertexShader =
  "#version 330\n "

  "in vec4 vertex;"

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
  : m_glwidget(glwidget),
    m_graph(g)
{
  setFontSize(m_fontsize);
  setFogDistance(m_fogdistance);
}

void GLScene::initialize()
{
  // Here we compile the vertex and fragment shaders and combine the results.
  bool result = m_shader.addShaderFromSourceCode(QGLShader::Vertex, g_vertexShader);
  if (!result)
  {
    mCRL2log(mcrl2::log::error) << m_shader.log().toStdString();
    std::abort();
  }

  result = m_shader.addShaderFromSourceCode(QGLShader::Fragment, g_fragmentShader);
  if (!result)
  {
    mCRL2log(mcrl2::log::error) << m_shader.log().toStdString();
    std::abort();
  }

  result = m_shader.link();
  if (!result)
  {
    mCRL2log(mcrl2::log::error) << "Could not link shader program:" << m_shader.log().toStdString();
    std::abort();
  }

  m_vertexdata.initialize();

  // Enable anti-aliasing for lines and points. Anti-aliasing for polygons gives artifacts on
  // OSX when drawing a quadstrip.
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Enable depth testing, so that we don't have to care too much about
  // rendering in the right order.
  glEnable(GL_DEPTH_TEST);

  // We'll be using a lot of glDrawArrays, and all of them use the vertex
  // array. We enable that feature once and leave it untouched.
  glEnableClientState(GL_VERTEX_ARRAY);
}

void GLScene::render()
{
  m_camera.animate();

  QColor clear(Qt::white);
  glClearColor(clear.redF(), clear.greenF(), clear.blueF(), 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_graph.lock(GRAPH_LOCK_TRACE); // enter critical section

  bool sel = m_graph.hasSelection();
  std::size_t nodeCount = sel ? m_graph.selectionNodeCount() : m_graph.nodeCount();
  std::size_t edgeCount = sel ? m_graph.selectionEdgeCount() : m_graph.edgeCount();

  /*for (std::size_t i = 0; i < nodeCount; ++i)
  {
    renderNode(sel ? m_graph.selectionNode(i) : i);
  }

  for (std::size_t i = 0; i < edgeCount; ++i) {
    renderEdge(sel ? m_graph.selectionEdge(i) : i);
  }

  for (std::size_t i = 0; i < edgeCount; ++i)
  {
    renderHandle(sel ? m_graph.selectionEdge(i) : i);
  }

  // text drawing follows
  */

  glDepthMask(GL_FALSE);

  m_renderpainter.begin(&m_glwidget);
  m_renderpainter.setFont(m_font);
  m_renderpainter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  /*for (std::size_t i = 0; i < nodeCount; ++i)
  {
    if (m_drawstatenumbers)
    {
      renderStateNumber(sel ? m_graph.selectionNode(i) : i);
    }

    if (m_drawstatelabels)
    {
      renderStateLabel(sel ? m_graph.selectionNode(i) : i);
    }
  }

  for (std::size_t i = 0; i < edgeCount; ++i)
  {
    if (m_drawtransitionlabels)
    {
      renderTransitionLabel(sel ? m_graph.selectionEdge(i) : i);
    }
  }*/

  m_renderpainter.end();
  glDepthMask(GL_TRUE);

  m_graph.unlock(GRAPH_LOCK_TRACE); // exit critical section

  MCRL2_OGL_CHECK();
}

void GLScene::resize(std::size_t width, std::size_t height)
{
  m_camera.viewport(width, height);
}

QVector3D GLScene::eyeToWorld(int x, int y, GLfloat z) const
{
  GLint viewport[4];
  GLfloat projection[16];
  GLfloat modelview[16];
  x *= m_device_pixel_ratio;
  y *= m_device_pixel_ratio;
  glGetFloatv(GL_PROJECTION_MATRIX, projection);
  glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
  glGetIntegerv(GL_VIEWPORT, viewport);
  if (z < 0)
  {
    glReadPixels(x, viewport[3]-y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
  }
  QVector3D eye{float(x), float(viewport[3] - y), float(z)};

  QMatrix4x4 m(modelview);
  QMatrix4x4 p(projection);
  QRect v(viewport[0], viewport[1], viewport[2], viewport[3]);
  return mcrl2::gui::unproject(eye, m.transposed(), p.transposed(), v);
}

QVector3D GLScene::worldToEye(const QVector3D& world) const
{
  GLint viewport[4];
  GLfloat projection[16];
  GLfloat modelview[16];
  glGetFloatv(GL_PROJECTION_MATRIX, projection);
  glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
  glGetIntegerv(GL_VIEWPORT, viewport);

  QMatrix4x4 m(modelview);
  QMatrix4x4 p(projection);
  QRect v(viewport[0], viewport[1], viewport[2], viewport[3]);
  QVector3D eye = mcrl2::gui::project(world, m.transposed(), p.transposed(), v);

  return QVector3D(eye.x() / m_device_pixel_ratio,
                 (viewport[3] - eye.y()) / m_device_pixel_ratio,
                 eye.z());
}

QVector3D GLScene::size()
{
  return m_camera.world;
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

bool GLScene::selectObject(GLScene::Selection& s, int x, int y,
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
    float radius = nodeSizeOnScreen() * magnificationFactor() / 2;
    for (std::size_t i = 0; i < nodeCount; i++)
    {
      std::size_t index = sel ? m_graph.selectionNode(i) : i;
      if (isClose(x, y, worldToEye(m_graph.node(index).pos()), radius, bestZ))
      {
        s.selectionType = type;
        s.index = index;
      }
    }
    break;
  }
  case so_handle:
  {
    float radius = handleSizeOnScreen() * magnificationFactor() * 2;
    for (std::size_t i = 0; i < edgeCount; i++)
    {
      std::size_t index = sel ? m_graph.selectionEdge(i) : i;
      if (isClose(x, y, worldToEye(m_graph.handle(index).pos()), radius, bestZ))
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
      const QVector3D& eye = worldToEye(label.pos());
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
      const QVector3D& eye = worldToEye(label.pos());
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

void GLScene::zoom(float factor)
{
  setZoom(m_camera.zoom * factor, 0);
}

float GLScene::magnificationFactor() const
{
  QVector3D a = worldToEye({0, 0, 0});
  QVector3D b = worldToEye({1, 0, 0});
  return a.distanceToPoint(b);
}

void GLScene::rotate(const QQuaternion& delta)
{
  setRotation(delta * m_camera.rotation, 0);
}

void GLScene::translate(const QVector3D& amount)
{
  setTranslation(m_camera.translation + amount, 0);
}

bool GLScene::resizing()
{
  return m_camera.resizing();
}

void GLScene::setZoom(float factor, std::size_t animation)
{
  m_camera.setZoom(factor, animation);
}

void GLScene::setRotation(const QQuaternion& rotation, std::size_t animation)
{
  m_camera.setRotation(rotation, animation);
}

void GLScene::setTranslation(const QVector3D& translation, std::size_t animation)
{
  m_camera.setTranslation(translation, animation);
}

void GLScene::setSize(const QVector3D& size, std::size_t animation)
{
  m_camera.setSize(size, animation);
}

// Private functions

inline
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
    glVertexPointer(3, GL_FLOAT, 4*3, data.node());
    glDrawArrays(GL_TRIANGLE_STRIP, RES_NODE_SLICE - 1, RES_NODE_SLICE * RES_NODE_STACK * 2 / 4);
  }

  glVertexPointer(3, GL_FLOAT, 0, data.node());

  float alpha = translucent ? 0.5 : 1.0;
  glColor4fv(Color4f(fill, alpha));

  glDrawArrays(GL_TRIANGLE_STRIP, RES_NODE_SLICE - 1, RES_NODE_SLICE * RES_NODE_STACK * 2);

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
  drawArc(ctrl);

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
    drawArrowHead(m_vertexdata);
  }

  glPopMatrix();
}

void GLScene::renderNode(GLuint i)
{
  Graph::NodeNode& node = m_graph.node(i);
  Color3f fill;
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
  }

  glPushMatrix();

  m_camera.billboard_spherical(node.pos());
  float nodescale = 0.5f * nodeSizeOnScreen();
  glScalef(nodescale, nodescale, nodescale);
  drawNode(m_vertexdata, line, fill, m_graph.hasSelection() && !node.active(), node.is_probabilistic());

  if (m_graph.hasSelection() && !m_graph.isBridge(i) && m_graph.initialState() != i)
  {
    float s = (fill.r < 0.5 && fill.g < 0.5 && fill.b < 0.5) ? 0.2f : -0.2f;
    hint = Color4f(fill.r + s, fill.g + s, fill.b + s, true);

    glTranslatef(0, 0, m_size_node * m_camera.pixelsize);
    drawWhetherNodeCanBeCollapsedOrExpanded(m_vertexdata, hint, node.active());
  }

  glPopMatrix();
}

void GLScene::renderTransitionLabel(GLuint i)
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
    QVector3D eye = worldToEye(label.pos());
    const QString& labelstring = m_graph.transitionLabelstring(label.labelindex());
    drawCenteredText(eye.x(), eye.y(), labelstring, color);
  }
}

void GLScene::renderStateLabel(GLuint i)
{
  Graph::LabelNode& label = m_graph.stateLabel(i);
  if (!m_graph.stateLabelstring(label.labelindex()).isEmpty())
  {
    QVector3D eye = worldToEye(label.pos());

    QColor color(std::max(label.color(0), label.selected()), std::min(label.color(1), 1.0f - label.selected()), std::min(label.color(2), 1.0f - label.selected()));
    drawCenteredText(eye.x(), eye.y() + nodeSizeOnScreen(), m_graph.stateLabelstring(label.labelindex()), color);
  }
}

void GLScene::renderStateNumber(GLuint i)
{
  Graph::NodeNode& node = m_graph.node(i);
  QVector3D eye = worldToEye(node.pos());
  drawCenteredText(eye.x(), eye.y(), QString::number(i));
}

QRect GLScene::drawCenteredText(float x, float y, const QString& text, const QColor& color)
{
  QFontMetrics metrics(m_renderpainter.font());
  QRect bounds = metrics.boundingRect(text);
  qreal w = bounds.width();
  qreal h = bounds.height();
  m_renderpainter.setPen(color);
  m_renderpainter.drawText(x - w / 2, y - h / 2, text);
  return bounds;
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

    m_camera.billboard_cylindrical(handle.pos());
    float scale = handleSizeOnScreen();
    glScalef(scale, scale, scale);
    drawHandle(m_vertexdata, line, fill);

    glPopMatrix();
    glEnable(GL_LINE_SMOOTH);
  }
}
