// Author(s): Rimco Boudewijns and Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "glscene.h"

#include "utility.h"
#include "bezier.h"

#include <QFont>
#include <QFontMetrics>
#include <QImage>
#include <QStaticText>

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

GLScene::GLScene(QOpenGLWidget& glwidget, const Graph::Graph& g)
  :  m_glwidget(glwidget),
     m_graph(g)
{
  setFontSize(m_fontsize);
  rebuild();
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
      nodeborder[static_cast<std::size_t>(i)+1] = QVector3D(std::sin(t), std::cos(t), 0.0f);
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
        node[n++] = QVector3D(std::sin(stack + stackd) * std::sin(slice),
            std::sin(stack + stackd) * std::cos(slice),
            std::cos(stack + stackd));
        node[n++] = QVector3D(std::sin(stack) * std::sin(slice),
            std::sin(stack) * std::cos(slice),
            std::cos(stack));
      }

      node[n++] = QVector3D(std::sin(stack + stackd) * std::sin(0.0f),
          std::sin(stack + stackd) * std::cos(0.0f),
          std::cos(stack + stackd));
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
      arrowhead[static_cast<std::size_t>(i)+1] = QVector3D(-1.0f,
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
      arrowhead_base[static_cast<std::size_t>(i)+1] = QVector3D(-1.0f,
          0.3f * std::sin(t),
          0.3f * std::cos(t));
    }
  }

  // Generate vertices for the arc, these will be moved to the correct position by the vertex shader using the x coordinate as t.
  std::vector<QVector3D> arc(VERTICES_ARC);
  {
    for (int i = 0; i < VERTICES_ARC; ++i)
    {
      arc[static_cast<std::size_t>(i)] = QVector3D(static_cast<float>(i) / (VERTICES_ARC - 1), 0.0f, 0.0f);
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

  m_vertexbuffer.create();
  m_vertexbuffer.bind();
  m_vertexbuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_vertexbuffer.allocate(vertices.data(), static_cast<int>(vertices.size() * sizeof(QVector3D)));

  // The vertex array object stores the layout of the vertex data that we use (vec3 float).
  m_vertexarray.create();
  {
    QOpenGLVertexArrayObject::Binder bind_vao(&m_vertexarray);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  }
}

void GLScene::update()
{
  m_camera.update();
}

void GLScene::rebuild()
{
  // Update the state labels.
  m_state_labels = std::vector<QStaticText>(m_graph.stateLabelCount());

  for (std::size_t i = 0; i < m_graph.stateLabelCount(); ++i)
  {
    m_state_labels[i] = QStaticText(m_graph.stateLabelstring(i));
    m_state_labels[i].setPerformanceHint(QStaticText::AggressiveCaching);
  }

  // Update the transition labels.
  m_transition_labels = std::vector<QStaticText>(m_graph.transitionLabelCount());

  for (std::size_t i = 0; i < m_graph.transitionLabelCount(); ++i)
  {
    m_transition_labels[i] = QStaticText(m_graph.transitionLabelstring(i));
    m_transition_labels[i].setPerformanceHint(QStaticText::AggressiveCaching);
  }
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

  // Change the alpha blending function to make an alpha of 1 opaque and 0 fully transparent.
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  // Enable multisample antialiasing.
  glEnable(GL_MULTISAMPLE);

  QColor clear(Qt::white);
  glClearColor(clear.red(), clear.green(), clear.blue(), 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_graph.lock(GRAPH_LOCK_TRACE); // enter critical section

  bool exploration_active = m_graph.hasExploration();
  std::size_t nodeCount = exploration_active ? m_graph.explorationNodeCount() : m_graph.nodeCount();
  std::size_t edgeCount = exploration_active ? m_graph.explorationEdgeCount() : m_graph.edgeCount();

  // All other objects share the same shader and vertex layout.
  m_global_shader.bind();
  m_vertexarray.bind();

  QMatrix4x4 viewProjMatrix = m_camera.projectionMatrix() *  m_camera.viewMatrix();

  // Render opaque objects
  glDepthMask(GL_TRUE);

  for (std::size_t i = 0; i < nodeCount; ++i)
  {
    renderNode(exploration_active ? m_graph.explorationNode(i) : i, viewProjMatrix, false);
  }

  for (std::size_t i = 0; i < edgeCount; ++i)
  {
    renderEdge(exploration_active ? m_graph.explorationEdge(i) : i, viewProjMatrix);
    renderHandle(exploration_active ? m_graph.explorationEdge(i) : i, viewProjMatrix);
  }

  // Render transparent objects
  glDepthMask(GL_FALSE);

  if (m_graph.hasExploration())
  {
    for (std::size_t i = 0; i < nodeCount; ++i)
    {
      renderNode(exploration_active ? m_graph.explorationNode(i) : i, viewProjMatrix, true);
    }
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
      renderStateNumber(painter, exploration_active ? m_graph.explorationNode(i) : i);
    }

    if (m_drawstatelabels)
    {
      renderStateLabel(painter, exploration_active ? m_graph.explorationNode(i) : i);
    }
  }

  if (m_drawtransitionlabels)
  {
    for (std::size_t i = 0; i < edgeCount; ++i)
    {
      renderTransitionLabel(painter, exploration_active ? m_graph.explorationEdge(i) : i);
    }
  }

  m_graph.unlock(GRAPH_LOCK_TRACE); // exit critical section

  // Make sure that glGetError() is not an error.
  glCheckError();
}

GLScene::Selection GLScene::select(int x, int y)
{
  Selection s{SelectableObject::none, 0};
  selectObject(s, x, y, SelectableObject::node)
    || selectObject(s, x, y, SelectableObject::handle)
    // Only select transition labels when they are rendered
    || (m_drawtransitionlabels && selectObject(s, x, y, SelectableObject::label))
    // Only select state labels when they are rendered
    || (m_drawstatelabels && selectObject(s, x, y, SelectableObject::slabel))
    ;
  return s;
}

float GLScene::sizeOnScreen(const QVector3D& pos, float length) const
{
  QVector3D rightPoint = sphericalBillboard(pos) * QVector3D(length, 0.0f, 0.0f);
  return (m_camera.worldToWindow(pos) - m_camera.worldToWindow(pos + rightPoint)).length();
}

//
// GLScene private methods
//

QVector3D GLScene::applyFog(const QVector3D& color, float fogAmount)
{
  return mix(clamp(fogAmount, 0.0f, 1.0f), color, m_clearColor);
}

void GLScene::drawCenteredText3D(QPainter& painter, const QString& text, const QVector3D& position, const QVector3D& color)
{
  QVector3D window = m_camera.worldToWindow(position);
  float fog = 0.0f;
  if (!text.isEmpty() && window.z() <= 1.0f && isVisible(position, fog)) // There is text, that is not behind the camera and it is visible.
  {
     QColor qcolor = vectorToColor(color);
     qcolor.setAlpha(static_cast<int>(255 * (1.0f - fog)));

     drawCenteredText(painter,
       window.x(),
       window.y(),
       text,
       qcolor);
  }
}

void GLScene::drawCenteredStaticText3D(QPainter& painter, const QStaticText& text, const QVector3D& position, const QVector3D& color)
{
  QVector3D window = m_camera.worldToWindow(position);
  float fog = 0.0f;
  if (window.z() <= 1.0f && isVisible(position, fog)) // There is text, that is not behind the camera and it is visible.
  {
     QColor qcolor = vectorToColor(color);
     qcolor.setAlpha(static_cast<int>(255 * (1.0f - fog)));

     drawCenteredStaticText(painter,
      window.x(),
      window.y(),
      text,
      qcolor);
  }
}

bool GLScene::isVisible(const QVector3D& position, float& fogamount)
{
  // Should match the vertex shader: fogAmount = (1.0f - exp(-1.0f * pow(distance * g_density, 2)));
  float distance = (m_camera.position() - position).length();
  fogamount = m_drawfog * (1.0f - std::exp(-1.0f * std::pow(distance * m_fogdensity, 2.0f)));
  return (distance < m_camera.viewdistance() && fogamount < 0.99f);
}

void GLScene::renderEdge(std::size_t i, const QMatrix4x4& viewProjMatrix)
{
  const Graph::Edge& edge = m_graph.edge(i);
  if (!m_drawselfloops && edge.is_selfloop())
  {
    return;
  }

  // Calculate control points from edge nodes and handle
  const QVector3D from = m_graph.node(edge.from()).pos();
  const QVector3D via = m_graph.handle(i).pos();
  const QVector3D to = m_graph.node(edge.to()).pos();
  const std::array<QVector3D, 4> control = calculateArc(from, via, to, edge.is_selfloop());

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

  // Calculate the position of the tip of the arrow, and the orientation of the arc
  QVector3D tip;
  QVector3D vec;
  {
    const Math::Circle node{to, 0.5f * nodeSizeScaled()};
    const Math::CubicBezier arc(control);
    const Math::Scalar t = Math::make_intersection(node, arc).guessNearBack();
    tip = node.project(arc.at(t));
    const Math::Circle head{to, 0.5f * nodeSizeScaled() + arrowheadSizeScaled()};
    const Math::Scalar s = Math::make_intersection(head, arc).guessNearBack();
    const QVector3D top = arc.at(s);
    vec = tip - top;
  }

  // If to == ctrl[2], then something odd is going on. We'll just
  // make the executive decision not to draw the arrowhead then, as it
  // will just clutter the image.
  if (vec.length() > 0)
  {
    vec.normalize();

    float fog = 0.0f;
    if (isVisible(tip, fog))
    {
      // Apply the fog color.
      m_global_shader.setColor(applyFog(arcColor, fog));

      QMatrix4x4 worldMatrix;

      // Go to arrowhead position
      worldMatrix.translate(tip.x(), tip.y(), tip.z());

      // Rotate the arrowhead to orient it to the end of the arc.
      QVector3D axis = QVector3D::crossProduct(QVector3D(1, 0, 0), vec);
      worldMatrix.rotate(radiansToDegrees(std::acos(vec.x())), axis);

      // Scale it according to its size.
      worldMatrix.scale(arrowheadSizeScaled());

      QMatrix4x4 worldViewProjMatrix = viewProjMatrix * worldMatrix;

      // Draw the arrow head
      m_global_shader.setWorldViewProjMatrix(worldViewProjMatrix);
      glDrawArrays(GL_TRIANGLE_FAN, OFFSET_ARROWHEAD, VERTICES_ARROWHEAD);

      // Draw a circle to enclose the arrowhead.
      glDrawArrays(GL_TRIANGLE_FAN, OFFSET_ARROWHEAD_BASE, VERTICES_ARROWHEAD_BASE);
    }
  }
}

void GLScene::renderHandle(std::size_t i, const QMatrix4x4& viewProjMatrix)
{
  if (!m_drawselfloops && m_graph.edge(i).is_selfloop())
  {
    return;
  }

  const Graph::Node& handle = m_graph.handle(i);
  if (handle.selected() > 0.1f || handle.locked())
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
    worldMatrix.scale(handleSizeScaled());

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

void GLScene::renderNode(std::size_t i, const QMatrix4x4& viewProjMatrix, bool transparent)
{
  const Graph::NodeNode& node = m_graph.node(i);
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
      fill = 0.7f * node.color();
    }
    else
    {
      fill = node.color();
    }
  }

  float fog = 0.0f;
  float alpha = (m_graph.hasExploration() && !node.active()) ? 1.0f : 1.0f; // Disabled for now until the transparent window issue can be resolved.
  if (isVisible(node.pos(), fog) && (transparent || alpha > 0.99f)) // Check if these elements are visible and opaque if transparency is disallowed.
  {
    QMatrix4x4 worldMatrix;
    worldMatrix.translate(node.pos());
    worldMatrix.rotate(sphericalBillboard(node.pos()));

    // Node stroke color: red when selected, black otherwise. Apply fogging afterwards.
    QVector3D line(0.6f * node.selected(), 0.0f, 0.0f);
    m_global_shader.setColor(QVector4D(applyFog(line, fog), alpha));

    // Scale the border such that they are of constant width.
    QMatrix4x4 borderMatrix(worldMatrix);
    float width = 1.0f;
    borderMatrix.scale(0.5f * (nodeSizeScaled() + width));
    m_global_shader.setWorldViewProjMatrix(viewProjMatrix * borderMatrix);
    glDrawArrays(GL_TRIANGLE_FAN, OFFSET_NODE_BORDER, VERTICES_NODE_BORDER);

    QMatrix4x4 nodeMatrix(worldMatrix);
    nodeMatrix.scale(0.5f * nodeSizeScaled());
    m_global_shader.setWorldViewProjMatrix(viewProjMatrix * nodeMatrix);

    // Apply fogging the node color and draw the node.
    m_global_shader.setColor(QVector4D(applyFog(fill, fog), alpha));
    if (node.is_probabilistic())
    {
      // Draw only the top section of the half sphere
      // This gives the appearance of a thicker border
      glDrawArrays(GL_TRIANGLE_STRIP, OFFSET_NODE_SPHERE, VERTICES_NODE_SPHERE / 2);
    }
    else
    {
      // Draw the half sphere
      glDrawArrays(GL_TRIANGLE_STRIP, OFFSET_NODE_SPHERE, VERTICES_NODE_SPHERE);
    }


    if (m_graph.hasExploration() && !m_graph.isBridge(i) && m_graph.initialState() != i)
    {
      float s = (fill.x() < 0.5f && fill.y() < 0.5f && fill.z() < 0.5f) ? 0.2f : -0.2f;
      QVector3D hint = QVector3D(fill.x() + s, fill.y() + s, fill.z() + s);

      m_global_shader.setColor(hint);

      // When the node is active, which means that its successors are shown in exploration mode, only the "minus" is drawn
      // by omitting the vertical rectangle of the whole "plus" shape.
      glDrawArrays(GL_TRIANGLES, OFFSET_HINT, node.active() ? VERTICES_HINT / 2 : VERTICES_HINT);
    }
  }
}

void GLScene::renderTransitionLabel(QPainter& painter, std::size_t i)
{
  if (!m_drawselfloops && m_graph.edge(i).is_selfloop())
  {
    return;
  }

  const Graph::LabelNode& label = m_graph.transitionLabel(i);
  QVector3D fill(std::max(label.color().x(), label.selected()), std::min(label.color().y(), 1.0f - label.selected()), std::min(label.color().z(), 1.0f - label.selected()));
  drawCenteredStaticText3D(painter, m_transition_labels[label.labelindex()], label.pos(), fill);
}

void GLScene::renderStateLabel(QPainter& painter, std::size_t i)
{
  const Graph::LabelNode& label = m_graph.stateLabel(i);
  if (label.labelindex() >= m_graph.stateLabelCount())
  {
    // the label does not actually exist, so we don't draw it
    return;
  }

  QVector3D color(std::max(label.color().x(), label.selected()), std::min(label.color().y(), 1.0f - label.selected()), std::min(label.color().z(), 1.0f - label.selected()));
  drawCenteredStaticText3D(painter, m_state_labels[label.labelindex()], label.pos(), color);
}

void GLScene::renderStateNumber(QPainter& painter, std::size_t i)
{
  const Graph::NodeNode& node = m_graph.node(i);
  QVector3D color(0.0f, 0.0f, 0.0f);
  drawCenteredText3D(painter, QString::number(i), node.pos(), color);
}

bool GLScene::selectObject(GLScene::Selection& s,
                           int x,
                           int y,
                           SelectableObject type)
{
  float bestZ = 1.0f;
  bool exploration_active = m_graph.hasExploration();
  std::size_t nodeCount = exploration_active ? m_graph.explorationNodeCount() : m_graph.nodeCount();
  std::size_t edgeCount = exploration_active ? m_graph.explorationEdgeCount() : m_graph.edgeCount();

  QFontMetrics metrics(m_font);
  switch (type)
  {
  case SelectableObject::node:
  {
    for (std::size_t i = 0; i < nodeCount; i++)
    {
      std::size_t index = exploration_active ? m_graph.explorationNode(i) : i;
      QVector3D screenPos = m_camera.worldToWindow(m_graph.node(index).pos());
      float radius = sizeOnScreen(m_graph.node(index).pos(), nodeSizeScaled()) / 2.0f;
      if (isCloseCircle(x, y, screenPos, radius, bestZ))
      {
        s.selectionType = type;
        s.index = index;
      }
    }
    break;
  }
  case SelectableObject::handle:
  {
    for (std::size_t i = 0; i < edgeCount; i++)
    {
      std::size_t index = exploration_active ? m_graph.explorationEdge(i) : i;
      if (!m_drawselfloops && m_graph.edge(index).is_selfloop())
      {
        continue;
      }
      const Graph::Node& handle = m_graph.handle(index);
      QVector3D screenPos = m_camera.worldToWindow(handle.pos());
      float radius = sizeOnScreen(handle.pos(), handleSizeScaled());
      if (isCloseSquare(x, y, screenPos, radius, bestZ))
      {
        s.selectionType = type;
        s.index = index;
      }
    }
    break;
  }
  case SelectableObject::label:
  {
    for (std::size_t i = 0; i < edgeCount; i++)
    {
      std::size_t index = exploration_active ? m_graph.explorationEdge(i) : i;
      if (!m_drawselfloops && m_graph.edge(index).is_selfloop())
      {
        continue;
      }
      const Graph::LabelNode& label = m_graph.transitionLabel(index);
      QVector3D window = m_camera.worldToWindow(label.pos());
      if (isOnText(x, y, m_transition_labels[label.labelindex()], window))
      {
        s.selectionType = type;
        s.index = index;
        break;
      }
    }
    break;
  }
  case SelectableObject::slabel:
  {
    for (std::size_t i = 0; i < nodeCount; i++)
    {
      std::size_t index = exploration_active ? m_graph.explorationNode(i) : i;
      const Graph::LabelNode& label = m_graph.stateLabel(index);
      if (label.labelindex() >= m_graph.stateLabelCount())
      {
        // the label does not exist, so we skip it
        continue;
      }

      QVector3D window = m_camera.worldToWindow(label.pos());
      if (isOnText(x, y, m_state_labels[label.labelindex()], window))
      {
        s.selectionType = type;
        s.index = index;
        break;
      }
    }
    break;
  }
  case SelectableObject::edge:
  case SelectableObject::none:
    Q_UNREACHABLE();
  }

  return s.selectionType != SelectableObject::none;
}

QQuaternion GLScene::sphericalBillboard(const QVector3D& position) const
{
  // Take the conjugated rotation of the camera to position the node in the right direction
  QQuaternion centerRotation = m_camera.rotation().conjugated();

  // And compensate for the perspective of the camera on the object if its not in the center of the screen
  QVector3D posToCamera = m_camera.position() - position;
  posToCamera.normalize();
  QVector3D camera = centerRotation.rotatedVector(QVector3D(0.0f, 0.0f, 1.0f));
  camera.normalize();
  // Compute the roration with the cross product and the dot product (aka inproduct)
  QQuaternion perspectiveRotation = QQuaternion::fromAxisAndAngle(QVector3D::crossProduct(camera, posToCamera),
     radiansToDegrees(std::acos(QVector3D::dotProduct(camera, posToCamera))));

  // Return the combination of both rotations
  // NB: the order of this multiplication is important
  return perspectiveRotation * centerRotation;
}
