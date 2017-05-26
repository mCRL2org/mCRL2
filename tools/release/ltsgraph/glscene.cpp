// Author(s): Rimco Boudewijns and Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <cassert>
#include <cmath>
#include <cmath>
#include <cstdio>

#include <QFile>
#include <QFont>
#include <QFontMetrics>
#include <QImage>
#include <QPainter>
#include <QScreen>

#ifdef _WINDOWS
#include <windows.h>
#endif
#include <QtOpenGL>
#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include "mcrl2/gui/arcball.h"
#include "mcrl2/gui/workarounds.h"
#include "mcrl2/utilities/logger.h"

#include "glscene.h"

#define RES_ARROWHEAD  30  ///< Amount of segments in arrowhead cone
#define RES_ARC        20  ///< Amount of segments for edge arc
#define RES_NODE_SLICE 64  ///< Number of segments from which a circle representing a node is constructed.
#define RES_NODE_STACK  4

struct Color3f
{
  GLfloat r, g, b;
  Color3f() = default;
  Color3f(GLfloat r, GLfloat g, GLfloat b) : r(r), g(g), b(b) {}
  Color3f(GLfloat* c) : r(c[0]), g(c[1]), b(c[2]) {}
  operator const GLfloat* () const {
    return &r;
  }
};

struct Color4f
{
  GLfloat r, g, b, a;
  Color4f() = default;
  Color4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a) : r(r), g(g), b(b), a(a) {}
  Color4f(const Color3f& c, GLfloat a = 1.0) : r(c.r), g(c.g), b(c.b), a(a) {}
  Color4f(GLfloat* c) : r(c[0]), g(c[1]), b(c[2]), a(c[3]) {}
  operator const GLfloat* () const {
    return &r;
  }
};

void TextureData::clear()
{
  delete[] transitions;
  transitions = nullptr;
  delete[] states;
  states      = nullptr;
  delete[] numbers;
  numbers     = nullptr;
  for (QHash<QString,Texture*>::iterator it = labels.begin(); it != labels.end(); ++it) {
    delete it.value();
  }
  labels.clear();
}

void TextureData::createTexture(const QString& labelstring, Texture*& texture)
{
  // Reuse texture when possible
  if (labels.count(labelstring) != 0)
  {
    texture = labels[labelstring];
    return;
  }

  QFontMetrics metrics(font);
  QPainter p;
  QRect bounds = metrics.boundingRect(0, 0, 0, 0, Qt::AlignLeft, labelstring);
  // Save the original width and height for posterity
  size_t width = bounds.width() * device_pixel_ratio;
  size_t height = bounds.height() * device_pixel_ratio;
  texture = new Texture(width, height, pixelsize);
  labels[labelstring] = texture;

  QImage label(width, height, QImage::Format_ARGB32_Premultiplied);
  label.setDevicePixelRatio(device_pixel_ratio);
  label.fill(QColor(1, 1, 1, 0));
  p.begin(&label);
  p.setFont(font);
  p.setCompositionMode(QPainter::CompositionMode_Clear);
  p.setCompositionMode(QPainter::CompositionMode_SourceOver);
  p.setPen(QColor(255, 0, 0, 255));
  p.drawText(bounds, labelstring);
  p.end();

#ifndef NDEBUG
  size_t error=glGetError();
  assert(error == 0 || error == 1286); // TODO: The error 1286 indicates that something is problematic. This ought to be resolved.
#endif

  // OpenGL likes its textures to have dimensions that are powers of 2
  size_t w = 1, h = 1;
  while (w < width) {
    w <<= 1;
  }
  while (h < height) {
    h <<= 1;
  }
  // ... and also wants the alpha component to be the 4th component
  label = label.scaled(w, h).convertToFormat(QImage::Format_RGBA8888).mirrored();

  glBindTexture(GL_TEXTURE_2D, texture->name);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, label.width(), label.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, label.bits());

  assert(glGetError() == 0);
}

const Texture& TextureData::getTransitionLabel(size_t index)
{
  Texture*& texture = transitions[index];
  if (texture == nullptr) {
    createTexture(graph->transitionLabelstring(index), texture);
  }
  return *texture;
}

const Texture& TextureData::getStateLabel(size_t index)
{
  Texture*& texture = states[index];
  if (texture == nullptr) {
    createTexture(graph->stateLabelstring(index), texture);
  }
  return *texture;
}

const Texture& TextureData::getNumberLabel(size_t index)
{
  Texture*& texture = numbers[index];
  if (texture == nullptr) {
    createTexture(QString::number(index), texture);
  }
  return *texture;
}

void TextureData::generateTextureData(Graph::Graph& g)
{
  clear();

  g.lock(GRAPH_LOCK_TRACE); // enter critical section

  size_t transition_count = g.transitionLabelCount();
  size_t state_count = g.stateLabelCount();
  size_t number_count = g.nodeCount();

  g.unlock(GRAPH_LOCK_TRACE); // exit critical section

  graph = &g;
  transitions = new Texture*[transition_count]();
  states = new Texture*[state_count]();
  numbers = new Texture*[number_count]();
}

void TextureData::resize(float pixelsize)
{
  for (QHash<QString,Texture*>::iterator it = labels.begin(); it != labels.end(); ++it) {
    it.value()->resize(pixelsize);
  }
}

void VertexData::clear()
{
  delete[] node;
  node      = nullptr;
  delete[] hint;
  hint      = nullptr;
  delete[] handle;
  handle    = nullptr;
  delete[] arrowhead;
  arrowhead = nullptr;
}

void VertexData::generateVertexData(float handlesize, float nodesize, float arrowheadsize)
{
  // Delete old data
  clear();

  // Generate vertices for node border (a line loop drawing a circle)
  float slice = 0, sliced = (float)(2.0 * M_PI / (RES_NODE_SLICE - 1)),
        stack = 0, stackd = (float)(M_PI_2 / RES_NODE_STACK);
  node = new QVector3D[RES_NODE_SLICE - 1 + RES_NODE_SLICE * RES_NODE_STACK * 2];
  for (int i = 0; i < RES_NODE_SLICE - 1; ++i, slice += sliced) {
    node[i] = QVector3D(std::sin(slice), std::cos(slice), 0.1f);
  }
  // Generate vertices for node (a quad strip drawing a half sphere)
  slice = 0;
  size_t n = RES_NODE_SLICE - 1;
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
  for (size_t i = 0; i < n; ++i) {
    node[i] *= 0.5 * nodesize;
  }

  // Generate plus (and minus) hint for exploration mode
  hint = new QVector3D[4];
  hint[0] = QVector3D(-nodesize * 0.3, 0.0, 0.0);
  hint[1] = QVector3D(nodesize * 0.3, 0.0, 0.0);
  hint[2] = QVector3D(0.0, -nodesize * 0.3, 0.0);
  hint[3] = QVector3D(0.0, nodesize * 0.3, 0.0);

  // Generate vertices for handle (border + fill, both squares)
  handle = new QVector3D[4];
  handle[0] = QVector3D(-handlesize/2.0, -handlesize/2.0, 0.0);
  handle[1] = QVector3D(handlesize/2.0, -handlesize/2.0, 0.0);
  handle[2] = QVector3D(handlesize/2.0,  handlesize/2.0, 0.0);
  handle[3] = QVector3D(-handlesize/2.0,  handlesize/2.0, 0.0);

  // Generate vertices for arrowhead (a triangle fan drawing a cone)
  arrowhead = new QVector3D[RES_ARROWHEAD + 1];
  arrowhead[0] = QVector3D(-nodesize / 2.0, 0.0, 0.0);
  float diff = (float)(M_PI / 20.0), t = 0;
  for (int i = 1; i < RES_ARROWHEAD; ++i, t += diff) {
    arrowhead[i] = QVector3D(-nodesize / 2.0 - arrowheadsize,
        0.3 * arrowheadsize * std::sin(t),
        0.3 * arrowheadsize * std::cos(t));
  }
  arrowhead[RES_ARROWHEAD] = QVector3D(-nodesize / 2.0 - arrowheadsize,
      0.3 * arrowheadsize * std::sin(0.0f),
      0.3 * arrowheadsize * std::cos(0.0f));
}
void CameraView::viewport(size_t width, size_t height)
{
  glViewport(0, 0, width, height);
  pixelsize = 1000.0 / (width < height ? height : width);
  world.setX(width * pixelsize);
  world.setY(height * pixelsize);
}

void CameraView::billboard_spherical(const QVector3D& pos)
{
  QVector3D rt, up, lk;
  GLfloat mm[16];

  glGetFloatv(GL_MODELVIEW_MATRIX, mm);
  lk.setX(mm[0] * pos.x() + mm[4] * pos.y() + mm[8] * pos.z() + mm[12]);
  lk.setY(mm[1] * pos.x() + mm[5] * pos.y() + mm[9] * pos.z() + mm[13]);
  lk.setZ(mm[2] * pos.x() + mm[6] * pos.y() + mm[10] * pos.z() + mm[14]);

  lk /= lk.length();
  rt = QVector3D::crossProduct(lk, QVector3D(0, 1, 0));
  up = QVector3D::crossProduct(rt, lk);
  GLfloat matrix[16] = {rt.x(), rt.y(), rt.z(), 0,
    up.x(), up.y(), up.z(), 0,
    -lk.x(),  -lk.y(),  -lk.z(),  0,
    0,        0,      0,  1
  };
  billboard_cylindrical(pos);
  glMultMatrixf(matrix);
}

void CameraView::billboard_cylindrical(const QVector3D& pos)
{
  glTranslatef(pos.x(), pos.y(), pos.z());
  mcrl2::gui::applyRotation(rotation, /*reverse=*/true);
}

void CameraView::applyTranslation()
{
  float viewdepth = world.length() + 2 * pixelsize * 10;
  glTranslatef(0, 0, -5000.0005 - 0.5 * viewdepth);
  glTranslatef(translation.x(), translation.y(), translation.z());
}

void CameraView::applyFrustum()
{
  float viewdepth = world.length() + 2 * pixelsize * 10;
  float f = 2 * zoom * (10000.0 + (viewdepth - world.z())) / 10000.0;
  glFrustum(-world.x() / f, world.x() / f, -world.y() / f, world.y() / f, 5000, viewdepth + 5000.001);
}

void CameraView::applyPickMatrix(GLdouble x, GLdouble y, GLdouble fuzz)
{
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  // Viewport is always (0, 0, width, height)
  gluPickMatrix(x, viewport[3] - y, fuzz * pixelsize, fuzz * pixelsize, viewport);
}

void CameraAnimation::start_animation(size_t steps)
{
  m_source = *this;
  m_animation_steps = steps;
  m_animation = 0;
  if (steps == 0) {
    operator=(m_target);
  }
}

void CameraAnimation::operator=(const CameraView& other)
{
  rotation = other.rotation;
  translation = other.translation;
  zoom = other.zoom;
  world = other.world;
  pixelsize = other.pixelsize;
}

void CameraAnimation::interpolate_cam(float pos)
{
  if (pos > 0.999)
  {
    rotation = m_target.rotation;
    translation = m_target.translation;
    zoom = m_target.zoom;
  }
  else
  {
    // if this is unsatisfactory, use https://en.wikipedia.org/wiki/Slerp
    rotation = m_target.rotation * pos + m_source.rotation * (1.0 - pos);
    translation = m_target.translation * pos + m_source.translation * (1.0 - pos);
    zoom = m_target.zoom * pos + m_source.zoom * (1.0 - pos);
  }
}

void CameraAnimation::interpolate_world(float pos)
{
  m_resizing = true;
  if (pos > 0.999)
  {
    world = m_target.world;
  }
  else
  {
    world.setX(m_target.world.x() * pos + m_source.world.x() * (1.0 - pos));
    world.setY(m_target.world.y() * pos + m_source.world.y() * (1.0 - pos));
    if (m_target.world.z() > m_source.world.z()) {
      world.setZ(m_target.world.z() * sin(M_PI_2 * pos) + m_source.world.z() * (1.0 - sin(M_PI_2 * pos)));
    }
    else {
      world.setZ(m_target.world.z() * (1.0 - cos(M_PI_2 * pos)) + m_source.world.z() * cos(M_PI_2 * pos));
    }
  }
}

void CameraAnimation::animate()
{
  if ((m_target.rotation != rotation || m_target.translation != translation || m_target.zoom != zoom) &&
      (m_target.world != world))
  {
    size_t halfway = m_animation_steps / 2;
    if (m_animation < halfway) {
      interpolate_cam((float)(++m_animation) / halfway);
    }
    if (m_animation == halfway)
    {
      m_animation_steps -= halfway;
      m_animation = 0;
    }
  }
  else if (m_target.world != world)
  {
    interpolate_world((float)(++m_animation) / m_animation_steps);
  }
  else {
    interpolate_cam((float)(++m_animation) / m_animation_steps);
  }
}

void CameraAnimation::viewport(size_t width, size_t height)
{
  CameraView::viewport(width, height);
  m_target.world.setX(world.x());
  m_target.world.setY(world.y());
  m_target.pixelsize = pixelsize;
}

bool CameraAnimation::resizing()
{
  bool temp = m_resizing;
  m_resizing = false;
  return temp;
}

void CameraAnimation::setZoom(float factor, size_t animation)
{
  m_target.zoom = factor;
  start_animation(animation);
}

void CameraAnimation::setRotation(const QQuaternion& rotation, size_t animation)
{
  m_target.rotation = rotation;
  start_animation(animation);
}

void CameraAnimation::setTranslation(const QVector3D& translation, size_t animation)
{
  m_target.translation = translation;
  start_animation(animation);
}

void CameraAnimation::setSize(const QVector3D& size, size_t animation)
{
  m_target.world = size;
  start_animation(animation);
}

//
// Some auxiliary functions that extend OpenGL
//

inline
bool gl2ps()
{
  return (gl2psEnable(GL2PS_BLEND) == GL2PS_SUCCESS);
}

inline
void glStartName(GLuint objectType, GLuint index=0)
{
  glLoadName(objectType);
  glPushName(index);
}

inline
void glEndName()
{
  glPopName();
  glLoadName(GLScene::so_none);
}

//
// Functions that actually draw primitives by combining vertex data, textures and colors.
//

inline
void drawHandle(const VertexData& data, const Color3f& line, const Color3f& fill)
{
  glVertexPointer(3, GL_FLOAT, 0, data.handle);
  glColor3fv(fill);
  glDrawArrays(GL_QUADS, 0, 4);
  glColor3fv(line);
  glDrawArrays(GL_LINE_LOOP, 0, 4);
}

inline
void drawNode(const VertexData& data, const Color3f& line, const Color3f& fill, bool mark, bool translucent)
{
  glPushAttrib(GL_LINE_BIT);
  if (mark)
  {
    glLineWidth(5.0);
    gl2psLineWidth(1);
  }
  else
  {
    glLineWidth(2.0);
    gl2psLineWidth(0.25);
  }

  glVertexPointer(3, GL_FLOAT, 0, data.node);
  if (translucent)
  {
    Color4f fill2(fill, .15f);
    glColor4fv(fill2);
  }
  else
  {
    glColor3fv(fill);
  }
  glDrawArrays(GL_TRIANGLE_STRIP, RES_NODE_SLICE - 1, RES_NODE_SLICE * RES_NODE_STACK * 2);

  // disable the depth mask temporarily for drawing the border of a node
  // dragging an initial state in 2D mode over other nodes looks less weird this way
  // BUT not disabling the depth mask here causes some strange issue on Mac OS
  glDepthMask(GL_FALSE);

  if (translucent)
  {
    Color4f line2(line, .15f);
    glColor4fv(line2);
  }
  else
  {
    glColor3fv(line);
  }
  glDrawArrays(GL_LINE_LOOP, 0, RES_NODE_SLICE - 1);

  // see above
  glDepthMask(GL_TRUE);
  glPopAttrib();
  gl2psLineWidth(0.25);
}

inline
void drawHint(const VertexData& data, const Color4f& line, bool active)
{
  glPushAttrib(GL_LINE_BIT);
  glLineWidth(2.5);
  glVertexPointer(3, GL_FLOAT, 0, data.hint);
  glDepthMask(GL_FALSE);
  glColor4fv(line);
  glDrawArrays(GL_LINES, 0, active ? 2 : 4); // Plus or half a plus (minus)
  glDepthMask(GL_TRUE);
  glPopAttrib();
}

inline
void drawArrowHead(const VertexData& data)
{
  glVertexPointer(3, GL_FLOAT, 0, data.arrowhead);
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

inline
void drawTransitionLabel(TextureData& textures, size_t index)
{
  static const GLfloat texCoords[] = { 0.0, 0.0,
                                       1.0, 0.0,
                                       1.0, 1.0,
                                       0.0, 1.0
                                     };

  const Texture& texture = textures.getTransitionLabel(index);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture.name);

  glVertexPointer(3, GL_FLOAT, 0, texture.shape);
  glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
  glDrawArrays(GL_QUADS, 0, 4);

  glDisable(GL_TEXTURE_2D);
}

inline
void drawStateLabel(TextureData& textures, size_t index)
{
  static const GLfloat texCoords[] = { 0.0, 0.0,
                                       1.0, 0.0,
                                       1.0, 1.0,
                                       0.0, 1.0
                                     };

  const Texture& texture = textures.getStateLabel(index);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture.name);

  glVertexPointer(3, GL_FLOAT, 0, texture.shape);
  glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
  glDrawArrays(GL_QUADS, 0, 4);

  glDisable(GL_TEXTURE_2D);
}

inline
void drawNumber(TextureData& textures, size_t index)
{
  static const GLfloat texCoords[] = { 0.0, 0.0,
                                       1.0, 0.0,
                                       1.0, 1.0,
                                       0.0, 1.0
                                     };

  const Texture& texture = textures.getNumberLabel(index);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture.name);

  glVertexPointer(3, GL_FLOAT, 0, texture.shape);
  glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
  glDrawArrays(GL_QUADS, 0, 4);

  glDisable(GL_TEXTURE_2D);
}

//
// GLScene private methods
//

void GLScene::renderEdge(size_t i)
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

  glStartName(so_edge, i);
  glPushMatrix();

  glColor3f(m_graph.handle(i).selected(), 0.0, 0.0);

  // Draw the arc
  drawArc(ctrl);

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
  glEndName();
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

  glStartName(so_node, i);
  glPushMatrix();

  m_camera.billboard_spherical(node.pos());
  drawNode(m_vertexdata, line, fill, mark, m_graph.hasSelection() && !node.active());

  if (m_graph.hasSelection() && node.selected() != 0.0 && !m_graph.isBridge(i))
  {
    float s = (fill.r < 0.5 && fill.g < 0.5 && fill.b < 0.5) ? 0.2f : -0.2f;
    hint = Color4f(fill.r + s, fill.g + s, fill.b + s, node.selected());

    glTranslatef(0, 0, m_size_node * m_camera.pixelsize);
    drawHint(m_vertexdata, hint, node.active());
  }

  glPopMatrix();
  glEndName();
}

void GLScene::renderTransitionLabel(GLuint i)
{
  Graph::Edge edge = m_graph.edge(i);
  if (edge.from() == edge.to() && !m_drawselfloops) {
    return;
  }
  Graph::LabelNode& label = m_graph.transitionLabel(i);
  if (!m_graph.transitionLabelstring(label.labelindex()).isEmpty()) {
    glStartName(so_label, i);

    Color3f fill = Color3f((std::max)(label.color(0), label.selected()), (std::min)(label.color(1), 1.0f - label.selected()), (std::min)(label.color(2), 1.0f - label.selected()));
    glColor3fv(fill);
    if (gl2ps())
    {
      QVector3D pos = label.pos();
      const Texture& texture = m_texturedata.getTransitionLabel(label.labelindex());
      pos.setX(pos.x() - m_camera.pixelsize * texture.width / 2);
      pos.setY(pos.y() - m_camera.pixelsize * texture.height / 2);
      glRasterPos3f(pos.x(), pos.y(), pos.z());
      if (!m_graph.isTau(label.labelindex())) {
        gl2psText(m_graph.transitionLabelstring(label.labelindex()).toUtf8(), "", 10);
      }
      else {
        gl2psText("t", "Symbol", 10);
      }
    }
    else
    {
      glPushMatrix();

      m_camera.billboard_cylindrical(label.pos());
      drawTransitionLabel(m_texturedata, label.labelindex());

      glPopMatrix();
    }
    glEndName();
  }
}

void GLScene::renderStateLabel(GLuint i)
{
  Graph::LabelNode& label = m_graph.stateLabel(i);
  if (!m_graph.stateLabelstring(label.labelindex()).isEmpty()) {
    glStartName(so_slabel, i);
    Color3f fill = Color3f((std::max)(label.color(0), label.selected()), (std::min)(label.color(1), 1.0f - label.selected()), (std::min)(label.color(2), 1.0f - label.selected()));
    glColor3fv(fill);
    if (gl2ps())
    {
      QVector3D pos = label.pos();
      const Texture& texture = m_texturedata.getStateLabel(label.labelindex());
      pos.setX(pos.x() - m_camera.pixelsize * texture.width / 2);
      pos.setY(pos.y() - m_camera.pixelsize * texture.height / 2);
      glRasterPos3f(pos.x(), pos.y(), pos.z());
      gl2psText(m_graph.stateLabelstring(label.labelindex()).toUtf8(), "", 10);
    }
    else
    {
      glPushMatrix();

      m_camera.billboard_cylindrical(label.pos());
      glTranslatef(0, 0, m_size_node * m_camera.pixelsize * 1.01); // Position state label above state number
      drawStateLabel(m_texturedata, label.labelindex());

      glPopMatrix();
    }
    glEndName();
  }
}

void GLScene::renderStateNumber(GLuint i)
{
  Graph::NodeNode& node = m_graph.node(i);
  glStartName(so_node, i);
  if (gl2ps())
  {
    QVector3D pos = node.pos();
    const Texture& texture = m_texturedata.getNumberLabel(i);
    pos.setX(pos.x() - m_camera.pixelsize * texture.width / 2);
    pos.setY(pos.y() - m_camera.pixelsize * texture.height / 2);
    pos.setZ(pos.z() + m_size_node*m_camera.pixelsize);
    glRasterPos3f(pos.x(), pos.y(), pos.z());
    gl2psText(QString::number(i).toUtf8(), "", 10);
  }
  else
  {
    glPushMatrix();

    glColor3f(node.selected(), 0.0, 0.0);
    m_camera.billboard_spherical(node.pos());
    glTranslatef(0, 0, m_size_node * m_camera.pixelsize);
    drawNumber(m_texturedata, i);

    glPopMatrix();
  }
  glEndName();
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
    glStartName(so_handle, i);
    glPushMatrix();

    m_camera.billboard_cylindrical(handle.pos());
    drawHandle(m_vertexdata, line, fill);

    glPopMatrix();
    glEndName();
    glEnable(GL_LINE_SMOOTH);
  }
}

//
// GLScene public methods
//

GLScene::GLScene(Graph::Graph& g, float device_pixel_ratio)
  : m_graph(g), m_camera(), m_vertexdata(), m_texturedata(device_pixel_ratio, m_camera.pixelsize),
    m_drawtransitionlabels(true), m_drawstatelabels(false), m_drawstatenumbers(false), m_drawselfloops(true), m_drawinitialmarking(true),
    m_size_node(20), m_drawfog(true), m_fogdistance(5500.0)
{ }

void GLScene::init(const QColor& clear)
{
  // Set clear color to desired color
  glClearColor(clear.redF(), clear.greenF(), clear.blueF(), 1.0f);
  // Enable anti-aliasing for lines and points. Anti-aliasing for polygons gives artifacts on
  // OSX when drawing a quadstrip.
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  gl2psEnable(GL2PS_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  gl2psBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  GLfloat fog_color[4] = {float(clear.redF()), float(clear.greenF()), float(clear.blueF()), 1.0f};
  glFogf(GL_FOG_MODE, GL_LINEAR);
  glFogf(GL_FOG_DENSITY, 1);
  glFogfv(GL_FOG_COLOR, fog_color);
  const GLubyte* version = glGetString(GL_VERSION);
  if ((version != nullptr) && ((version[0] == '1' && version[2] >= '4') || version[0] > '1'))
  {
    glFogf(GL_FOG_COORD_SRC, GL_FRAGMENT_DEPTH);
  }
  updateFog();

  // Enable depth testing, so that we don't have to care too much about
  // rendering in the right order.
  glEnable(GL_DEPTH_TEST);
  // We'll be using a lot of glDrawArrays, and all of them use the vertex
  // array. We enable that feature once and leave it untouched.
  glEnableClientState(GL_VERTEX_ARRAY);
  // Load textures and shapes
  updateLabels();
  updateShapes();
}

void GLScene::updateFog()
{
  if (m_drawfog)
  {
    glFogf(GL_FOG_START, m_fogdistance);
    glFogf(GL_FOG_END, m_fogdistance+1500.0f);
    glEnable(GL_FOG);
  }
  else
  {
    glDisable(GL_FOG);
  }
}

void GLScene::render()
{
  // Initialise projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  m_camera.applyFrustum();
  m_camera.animate();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  m_camera.applyTranslation();
  mcrl2::gui::applyRotation(m_camera.rotation);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_graph.lock(GRAPH_LOCK_TRACE); // enter critical section

  bool sel = m_graph.hasSelection();
  size_t nodeCount = sel ? m_graph.selectionNodeCount() : m_graph.nodeCount();
  size_t edgeCount = sel ? m_graph.selectionEdgeCount() : m_graph.edgeCount();

  for (size_t i = 0; i < nodeCount; ++i) {
    renderNode(sel ? m_graph.selectionNode(i) : i);
  }
  for (size_t i = 0; i < edgeCount; ++i) {
    renderEdge(sel ? m_graph.selectionEdge(i) : i);
  }

  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glDepthMask(GL_FALSE);
  for (size_t i = 0; i < nodeCount; ++i)
  {
    if (m_drawstatenumbers) {
      renderStateNumber(sel ? m_graph.selectionNode(i) : i);
    }
    if (m_drawstatelabels) {
      renderStateLabel(sel ? m_graph.selectionNode(i) : i);
    }
  }
  for (size_t i = 0; i < edgeCount; ++i)
  {
    if (m_drawtransitionlabels) {
      renderTransitionLabel(sel ? m_graph.selectionEdge(i) : i);
    }
    renderHandle(sel ? m_graph.selectionEdge(i) : i);
  }
  glDepthMask(GL_TRUE);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  m_graph.unlock(GRAPH_LOCK_TRACE); // exit critical section
}

void GLScene::resize(size_t width, size_t height)
{
  m_camera.viewport(width, height);
  updateShapes();
}

void GLScene::updateLabels()
{
  m_texturedata.generateTextureData(m_graph);
}

void GLScene::updateShapes()
{
  m_vertexdata.generateVertexData(handleSizeOnScreen(), nodeSizeOnScreen(),
                                   arrowheadSizeOnScreen());
  m_texturedata.resize(m_camera.pixelsize);
}

QVector3D GLScene::eyeToWorld(int x, int y, GLfloat z)
{
  GLint viewport[4];
  GLfloat projection[16];
  GLfloat modelview[16];
  x *= m_texturedata.device_pixel_ratio;
  y *= m_texturedata.device_pixel_ratio;
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

QVector3D GLScene::worldToEye(const QVector3D& world)
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

  return QVector3D(eye.x() / m_texturedata.device_pixel_ratio,
                 (viewport[3] - eye.y()) / m_texturedata.device_pixel_ratio,
                 eye.z());
}

QVector3D GLScene::size()
{
  return m_camera.world;
}

GLScene::Selection GLScene::select(int x, int y)
{
  Selection s {so_none, 0};
  selectObject(s, x, y, so_node) || selectObject(s, x, y, so_handle);
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

bool GLScene::selectObject(GLScene::Selection& s, int x, int y,
                           SelectableObject type)
{
  float size, bestZ = INFINITY;
  switch (type)
  {
  case so_node:
  {
    size = nodeSizeOnScreen();
    for (size_t i = 0; i < m_graph.nodeCount(); i++)
    {
      if (isClose(x, y, worldToEye(m_graph.node(i).pos()), size, bestZ))
      {
        s.selectionType = type;
        s.index = i;
      }
    }
    break;
  }
  case so_handle:
  {
    size = handleSizeOnScreen();
    for (size_t i = 0; i < m_graph.edgeCount(); i++)
    {
      if (isClose(x, y, worldToEye(m_graph.handle(i).pos()), size, bestZ))
      {
        s.selectionType = type;
        s.index = i;
      }
    }
    break;
  }
  case so_slabel:
  case so_label:
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

void GLScene::setZoom(float factor, size_t animation)
{
  m_camera.setZoom(factor, animation);
  updateShapes();
}

void GLScene::setRotation(const QQuaternion& rotation, size_t animation)
{
  m_camera.setRotation(rotation, animation);
}

void GLScene::setTranslation(const QVector3D& translation, size_t animation)
{
  m_camera.setTranslation(translation, animation);
}

void GLScene::setSize(const QVector3D& size, size_t animation)
{
  m_camera.setSize(size, animation);
}

void GLScene::renderVectorGraphics(const char* filename, GLint format)
{
  FILE* outfile = fopen(filename, "wb+");
  GLint viewport[4];
  GLint buffersize = 1024*1024, state = GL2PS_OVERFLOW;

  while (state == GL2PS_OVERFLOW) {
    buffersize += 1024*1024;
    gl2psBeginPage(filename,
                   "mCRL2 toolset",
                   viewport,
                   format,
                   GL2PS_BSP_SORT,
                   GL2PS_SILENT |
                   GL2PS_USE_CURRENT_VIEWPORT |
                   GL2PS_OCCLUSION_CULL |
                   GL2PS_BEST_ROOT |
                   GL2PS_COMPRESS,
                   GL_RGBA,
                   0,
                   nullptr,
                   0, 0, 0,
                   buffersize,
                   outfile,
                   filename
                  );
    render();
    state = gl2psEndPage();
  }
  if (state != GL2PS_SUCCESS)
  {
    mCRL2log(mcrl2::log::error) << "Could not save file (gl2ps error)." << std::endl;
  }
  if (outfile != nullptr)
  {
    fclose(outfile);
  }
}

void GLScene::renderLatexGraphics(const QString& filename, float aspectRatio)
{
  QString tikz_code  = "\\documentclass[10pt, a4paper]{article}\n\n";
  tikz_code += "\\usepackage{tikz}\n";
  tikz_code += "\\usetikzlibrary{arrows}\n\n";

  tikz_code += "\\begin{document}\n";
  tikz_code += "\\begin{tikzpicture}\n";
  tikz_code += "  [scale=2]\n\n";
  tikz_code += "   \\tikzstyle{state}=[circle, draw]\n";
  tikz_code += "   \\tikzstyle{initstate}=[state,fill=green]\n";
  tikz_code += "   \\tikzstyle{transition}=[->,>=stealth']\n";

  m_graph.lock(GRAPH_LOCK_TRACE);

  bool sel = m_graph.hasSelection();
  size_t nodeCount = sel ? m_graph.selectionNodeCount() : m_graph.nodeCount();
  size_t edgeCount = sel ? m_graph.selectionEdgeCount() : m_graph.edgeCount();

  for (size_t i = 0; i < nodeCount; ++i)
  {
    tikz_code += tikzNode(sel ? m_graph.selectionNode(i) : i, aspectRatio);
  }

  for (size_t i = 0; i < edgeCount; ++i)
  {
    tikz_code += tikzEdge(sel ? m_graph.selectionEdge(i) : i, aspectRatio);
  }

  m_graph.unlock(GRAPH_LOCK_TRACE);

  tikz_code += "\n\\end{tikzpicture}\n";
  tikz_code += "\\end{document}\n";

  QFile file(filename);

  if (file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
  {
    file.write(tikz_code.toLatin1());
    file.close();
  }
}

QString GLScene::tikzNode(size_t i, float aspectRatio)
{
  Graph::NodeNode& node = m_graph.node(i);
  Color3f line(node.color());

  QString ret = "\\definecolor{currentcolor}{rgb}{%1,%2,%3}\n\\node at (%4pt, %5pt) [fill=currentcolor, %6state%8] (state%7) {%7};\n";

  ret = ret.arg(line.r, 0, 'f', 3).arg(line.g, 0, 'f', 3).arg(line.b, 0, 'f', 3);
  ret = ret.arg(node.pos().x() / 10.0f * aspectRatio, 6, 'f').arg(node.pos().y() / 10.0f, 6, 'f');
  ret = ret.arg(m_graph.initialState() == i ? "init" : "");
  ret = ret.arg(i);
  ret = ret.arg(node.active() ? "" : ", dashed");

  return ret;
}

static QString escapeLatex(const QString& str)
{
  QString escaped;
  QRegExp rx("[#$%_&{}^]");
  for (auto x : str) {
    if (rx.indexIn(x) != -1) {
      escaped.append('\\');
    }
    escaped.append(x);
  }
  return escaped;
}

QString GLScene::tikzEdge(size_t i, float aspectRatio)
{
  Graph::LabelNode& label = m_graph.transitionLabel(i);
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

  QString extraControls("");

  // For self-loops, ctrl[1] and ctrl[2] need to lie apart, we'll spread
  // them in x-y direction.
  if (edge.from() == edge.to())
  {
    QVector3D diff = ctrl[1] - ctrl[0];
    diff = QVector3D::crossProduct(diff, QVector3D(0, 0, 1));
    diff = diff * ((via - from).length() / (diff.length() * 2.0));
    ctrl[1] = ctrl[1] + diff;
    ctrl[2] = ctrl[2] - diff;

    extraControls = QString(" and (%1pt, %2pt)").arg(ctrl[2].x() / 10.0f * aspectRatio, 6, 'f').arg(ctrl[2].y() / 10.0f, 6, 'f');
  }

  QString ret = "\\draw [transition] (state%1) .. node[auto] {%3} controls (%4pt, %5pt)%6 .. (state%2);\n";
  ret = ret.arg(edge.from()).arg(edge.to());
  ret = ret.arg(escapeLatex(m_graph.transitionLabelstring(label.labelindex())));
  ret = ret.arg(ctrl[1].x() / 10.0f * aspectRatio, 6, 'f').arg(ctrl[1].y() / 10.0f, 6, 'f');
  ret = ret.arg(extraControls);

  return ret;
}
