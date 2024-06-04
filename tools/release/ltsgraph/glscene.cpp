// Author(s): Rimco Boudewijns and Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "glscene.h"

#include "bezier.h"
#include "layoututility.h"

#include <QElapsedTimer>

#include <QFont>
#include <QFontMetrics>
#include <QImage>
#include <QOpenGLFramebufferObjectFormat>
#include <QStaticText>

#include <algorithm>
#include <cmath>
#include <list>

/// \brief Number of orthogonal slices from which a circle representing a node
/// is constructed.
constexpr int RES_NODE_SLICE = 128;

/// \brief Number of vertical planes from which a circle representing a node is
/// constructed.
constexpr int RES_NODE_STACK = 2;

/// \brief Amount of segments in arrowhead cone
constexpr int RES_ARROWHEAD = 16;

/// \brief Amount of segments for edge arc
constexpr int RES_ARC = 16;

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
constexpr int OFFSET_HINT = OFFSET_NODE_SPHERE + VERTICES_NODE_SPHERE;
constexpr int OFFSET_HANDLE_BODY = OFFSET_HINT + VERTICES_HINT;
constexpr int OFFSET_HANDLE_OUTLINE = OFFSET_HANDLE_BODY + VERTICES_HANDLE_BODY;
constexpr int OFFSET_ARROWHEAD = OFFSET_HANDLE_OUTLINE + VERTICES_HANDLE_OUTLINE;
constexpr int OFFSET_ARROWHEAD_BASE = OFFSET_ARROWHEAD + VERTICES_ARROWHEAD;
constexpr int OFFSET_ARC = OFFSET_ARROWHEAD_BASE + VERTICES_ARROWHEAD_BASE;

GLScene::GLScene(QOpenGLWidget& glwidget, Graph::Graph& g)
    : m_glwidget(glwidget),
      m_graph(g)
{
  setFontSize(m_fontsize);
  rebuild();
  m_drawNodeBorder = DrawInstances(OFFSET_NODE_BORDER, VERTICES_NODE_BORDER, 0, GL_TRIANGLE_FAN, "node border");
  m_drawHalfSphere = DrawInstances(OFFSET_NODE_BORDER, VERTICES_NODE_BORDER, 0, GL_TRIANGLE_FAN, "half-sphere");
  m_drawSphere = DrawInstances(OFFSET_NODE_SPHERE, VERTICES_NODE_SPHERE, 0, GL_TRIANGLE_STRIP, "sphere");
  m_drawMinusHint = DrawInstances(OFFSET_HINT, VERTICES_HINT / 2, 0, GL_TRIANGLES, "minus hint");
  m_drawPlusHint = DrawInstances(OFFSET_HINT, VERTICES_HINT, 0, GL_TRIANGLES, "plus hint");
  m_drawHandleBody = DrawInstances(OFFSET_HANDLE_BODY, VERTICES_HANDLE_BODY, 0, GL_TRIANGLE_STRIP, "handle body");
  m_drawHandleOutline
      = DrawInstances(OFFSET_HANDLE_OUTLINE, VERTICES_HANDLE_OUTLINE, 0, GL_LINE_LOOP, "handle outline");
  updateDrawInstructions();

  m_drawInstances = std::vector<DrawInstances*>({
      &m_drawNodeBorder,
      &m_drawHalfSphere,
      &m_drawSphere,
      &m_drawMinusHint,
      &m_drawPlusHint,
      &m_drawHandleBody,
      &m_drawHandleOutline,
  });
}

void GLScene::initialize()
{
  // Makes sure that we can call gl* functions after this.
  initializeOpenGLFunctions();

  // Generate vertices for node border (A slightly larger circle with polygons
  // GL_TRIANGLE_FAN)
  std::vector<QVector3D> nodeborder(VERTICES_NODE_BORDER);
  {
    // The center of the circle, followed by the vertices on the edge.
    nodeborder[0] = QVector3D(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < RES_NODE_SLICE; ++i)
    {
      float t = -i * 2.0f * PI / (RES_NODE_SLICE - 1);
      nodeborder[static_cast<std::size_t>(i) + 1] = QVector3D(std::sin(t), std::cos(t), 0.0f);
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
        node[n++] = QVector3D(std::sin(stack) * std::sin(slice), std::sin(stack) * std::cos(slice), std::cos(stack));
      }

      node[n++] = QVector3D(std::sin(stack + stackd) * std::sin(0.0f),
          std::sin(stack + stackd) * std::cos(0.0f),
          std::cos(stack + stackd));
      node[n++] = QVector3D(std::sin(stack) * std::sin(0.0f), std::sin(stack) * std::cos(0.0f), std::cos(stack));
    }
  }

  // Generate plus (and minus) hint for exploration mode, we generate 4
  // triangles as a triangle strip cannot handle the disconnect between the two
  // rectangles of the plus.
  std::vector<QVector3D> hint(12);
  hint[0] = QVector3D(-0.6f, 0.1f, 1.0f);
  hint[1] = QVector3D(-0.6f, -0.1f, 1.0f);
  hint[2] = QVector3D(0.6f, 0.1f, 1.0f);

  hint[3] = QVector3D(0.6f, 0.1f, 1.0f);
  hint[4] = QVector3D(-0.6f, -0.1f, 1.0f);
  hint[5] = QVector3D(0.6f, -0.1f, 1.0f);

  hint[6] = QVector3D(-0.1f, 0.6f, 1.0f);
  hint[7] = QVector3D(-0.1f, -0.6f, 1.0f);
  hint[8] = QVector3D(0.1f, 0.6f, 1.0f);

  hint[9] = QVector3D(-0.1f, -0.6f, 1.0f);
  hint[10] = QVector3D(0.1f, -0.6f, 1.0f);
  hint[11] = QVector3D(0.1f, 0.6f, 1.0f);

  // Generate vertices for handle (border + fill, both squares)
  std::vector<QVector3D> handle_body(4);
  handle_body[0] = QVector3D(1.0f, -1.0f, 0.3f);
  handle_body[1] = QVector3D(1.0f, 1.0f, 0.3f);
  handle_body[2] = QVector3D(-1.0f, -1.0f, 0.3f);
  handle_body[3] = QVector3D(-1.0f, 1.0f, 0.3f);
  std::vector<QVector3D> handle_outline(4);
  handle_outline[0] = QVector3D(-1.0f, -1.0f, 0.3f);
  handle_outline[1] = QVector3D(1.0f, -1.0f, 0.3f);
  handle_outline[2] = QVector3D(1.0f, 1.0f, 0.3f);
  handle_outline[3] = QVector3D(-1.0f, 1.0f, 0.3f);

  // Generate vertices for arrowhead (a triangle fan drawing a cone)
  std::vector<QVector3D> arrowhead(VERTICES_ARROWHEAD);
  {
    arrowhead[0] = QVector3D(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < RES_ARROWHEAD; ++i)
    {
      float t = -i * 2.0f * PI / (RES_ARROWHEAD - 1);
      arrowhead[static_cast<std::size_t>(i) + 1] = QVector3D(-1.0f, 0.3f * std::sin(t), 0.3f * std::cos(t));
    }
  }

  std::vector<QVector3D> arrowhead_base(VERTICES_ARROWHEAD_BASE);
  {
    arrowhead_base[0] = QVector3D(-1.0f, 0.0f, 0.0f);

    for (int i = 0; i < RES_ARROWHEAD; ++i)
    {
      float t = i * 2.0f * PI / (RES_ARROWHEAD - 1);
      arrowhead_base[static_cast<std::size_t>(i) + 1] = QVector3D(-1.0f, 0.3f * std::sin(t), 0.3f * std::cos(t));
    }
  }

  // Generate vertices for the arc, these will be moved to the correct position
  // by the vertex shader using the x coordinate as t.
  std::vector<QVector3D> arc(VERTICES_ARC);
  {
    for (int i = 0; i < VERTICES_ARC; ++i)
    {
      arc[static_cast<std::size_t>(i)] = QVector3D(static_cast<float>(i) / (VERTICES_ARC - 1), 0.0f, 0.0f);
    }
  }

  // We are going to store all vertices in the same buffer and keep track of the
  // offsets.
  std::vector<QVector3D> vertices;
  vertices.insert(vertices.end(), nodeborder.begin(), nodeborder.end());
  vertices.insert(vertices.end(), node.begin(), node.end());
  vertices.insert(vertices.end(), hint.begin(), hint.end());
  vertices.insert(vertices.end(), handle_body.begin(), handle_body.end());
  vertices.insert(vertices.end(), handle_outline.begin(), handle_outline.end());
  vertices.insert(vertices.end(), arrowhead.begin(), arrowhead.end());
  vertices.insert(vertices.end(), arrowhead_base.begin(), arrowhead_base.end());
  vertices.insert(vertices.end(), arc.begin(), arc.end());

  m_fbo_format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
  m_fbo_format.setSamples(16);

  m_fbo = new QOpenGLFramebufferObject(m_glwidget.size(), m_fbo_format);
  // Initialize the global shader.
  m_node_shader.link();
  // Initialize the arc shader.
  m_arc_shader.link();
  // Initialize the arrow shader.
  m_arrow_shader.link();

  m_node_shader.bind();

  int vertex_attrib_location = m_node_shader.attributeLocation("vertex");
  int offset_attrib_location = m_node_shader.attributeLocation("offset");
  int color_attrib_location = m_node_shader.attributeLocation("color");

  int recommended_max_buffer_size;
  glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &recommended_max_buffer_size);
  mCRL2log(mcrl2::log::verbose) << "The recommended maximum vertices per buffer on this machine is "
                                << recommended_max_buffer_size << std::endl;
  mCRL2log(mcrl2::log::verbose) << "Based on maximum recommended, setting batch rendering size to "
                                << recommended_max_buffer_size / 4 << std::endl;
  // To be safe we divide by 4 since a mat4 is technically 4 vec4s stacked
  // together
  m_batch_size = static_cast<std::size_t>(recommended_max_buffer_size / 16);

  m_vaoNode.create();
  m_vaoNode.bind();

  m_vertexbuffer.create();
  m_vertexbuffer.bind();
  m_vertexbuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_vertexbuffer.allocate(vertices.data(), static_cast<int>(vertices.size() * sizeof(QVector3D)));

  m_node_shader.setAttributeBuffer(vertex_attrib_location, GL_FLOAT, 0, 3);
  m_node_shader.enableAttributeArray(vertex_attrib_location);

  m_current_scene_size = std::max(m_batch_size, std::max(m_graph.nodeCount(), m_graph.edgeCount()));
  m_drawInstances.resize(m_batch_size);

  m_colorBuffer.create();
  m_colorBuffer.setUsagePattern(QOpenGLBuffer::StreamDraw);
  m_colorBuffer.bind();
  m_colorBuffer.allocate(static_cast<int>(m_batch_size * sizeof(QVector4D)));
  m_node_shader.setAttributeBuffer(color_attrib_location, GL_FLOAT, 0, 4);
  m_node_shader.enableAttributeArray(color_attrib_location);
  glVertexAttribDivisor(color_attrib_location, 1);
  m_colorBuffer.release();

  m_offsetBuffer.create();
  m_offsetBuffer.setUsagePattern(QOpenGLBuffer::StreamDraw);
  m_offsetBuffer.bind();
  m_offsetBuffer.allocate(static_cast<int>(m_batch_size * sizeof(QVector3D)));
  m_node_shader.setAttributeBuffer(offset_attrib_location, GL_FLOAT, 0, 3);
  m_node_shader.enableAttributeArray(offset_attrib_location);
  glVertexAttribDivisor(offset_attrib_location, 1);

  m_offsetBuffer.release();
  m_vaoNode.release();
  m_node_shader.release();

  m_arc_shader.bind();
  int arc_vertex_attrib_location = m_arc_shader.attributeLocation("vertex");
  int arc_ctrl_1_attrib_location = m_arc_shader.attributeLocation("ctrl1");
  int arc_ctrl_2_attrib_location = m_arc_shader.attributeLocation("ctrl2");
  int arc_ctrl_3_attrib_location = m_arc_shader.attributeLocation("ctrl3");
  int arc_ctrl_4_attrib_location = m_arc_shader.attributeLocation("ctrl4");
  int arc_color_attrib_location = m_arc_shader.attributeLocation("color");

  mCRL2log(mcrl2::log::debug) << "vertex: " << arc_vertex_attrib_location << " ctrl1 " << arc_ctrl_1_attrib_location
                              << " ctrl2 " << arc_ctrl_2_attrib_location << " ctrl3 " << arc_ctrl_3_attrib_location
                              << " ctrl4 " << arc_ctrl_4_attrib_location << " color " << arc_color_attrib_location
                              << std::endl;

  m_vaoArc.create();
  m_vaoArc.bind();
  m_vertexbuffer.bind();
  m_arc_shader.setAttributeBuffer(arc_vertex_attrib_location, GL_FLOAT, 0, 3);
  m_arc_shader.enableAttributeArray(arc_vertex_attrib_location);

  m_controlpointbuffer.create();
  m_controlpointbuffer.setUsagePattern(QOpenGLBuffer::StreamDraw);
  m_controlpointbuffer.bind();
  m_controlpointbuffer.allocate(static_cast<int>(m_batch_size * sizeof(QVector3D) * 4));
  // TODO: Duplicate code removal with loop
  m_arc_shader.setAttributeBuffer(arc_ctrl_1_attrib_location, GL_FLOAT, 0, 3, 4 * sizeof(QVector3D));
  m_arc_shader.enableAttributeArray(arc_ctrl_1_attrib_location);
  glVertexAttribDivisor(arc_ctrl_1_attrib_location, 1);

  m_arc_shader.setAttributeBuffer(arc_ctrl_2_attrib_location, GL_FLOAT, sizeof(QVector3D), 3, 4 * sizeof(QVector3D));
  m_arc_shader.enableAttributeArray(arc_ctrl_2_attrib_location);
  glVertexAttribDivisor(arc_ctrl_2_attrib_location, 1);

  m_arc_shader.setAttributeBuffer(arc_ctrl_3_attrib_location,
      GL_FLOAT,
      2 * sizeof(QVector3D),
      3,
      4 * sizeof(QVector3D));
  m_arc_shader.enableAttributeArray(arc_ctrl_3_attrib_location);
  glVertexAttribDivisor(arc_ctrl_3_attrib_location, 1);

  m_arc_shader.setAttributeBuffer(arc_ctrl_4_attrib_location,
      GL_FLOAT,
      3 * sizeof(QVector3D),
      3,
      4 * sizeof(QVector3D));
  m_arc_shader.enableAttributeArray(arc_ctrl_4_attrib_location);
  glVertexAttribDivisor(arc_ctrl_4_attrib_location, 1);

  m_colorBuffer.bind();
  m_arc_shader.setAttributeBuffer(arc_color_attrib_location, GL_FLOAT, 0, 3);
  m_arc_shader.enableAttributeArray(arc_color_attrib_location);
  glVertexAttribDivisor(arc_color_attrib_location, 1);

  m_vaoArc.release();
  m_arc_shader.release();

  m_arrow_shader.bind();

  m_vaoArrow.create();
  m_vaoArrow.bind();

  int arrow_vertex_attrib_location = m_arrow_shader.attributeLocation("vertex");
  int arrow_offset_attrib_location = m_arrow_shader.attributeLocation("offset");
  int arrow_color_attrib_location = m_arrow_shader.attributeLocation("color");
  int arrow_dir_attrib_location = m_arrow_shader.attributeLocation("direction");

  m_vertexbuffer.bind();
  m_arrow_shader.setAttributeBuffer(arrow_vertex_attrib_location, GL_FLOAT, 0, 3);
  m_arrow_shader.enableAttributeArray(arrow_vertex_attrib_location);

  m_offsetBuffer.bind();
  m_arrow_shader.setAttributeArray(arrow_offset_attrib_location, GL_FLOAT, 0, 3);
  m_arrow_shader.enableAttributeArray(arrow_offset_attrib_location);
  glVertexAttribDivisor(arrow_offset_attrib_location, 1);

  m_colorBuffer.bind();
  m_arrow_shader.setAttributeArray(arrow_color_attrib_location, GL_FLOAT, 0, 4);
  m_arrow_shader.enableAttributeArray(arrow_color_attrib_location);
  glVertexAttribDivisor(arrow_color_attrib_location, 1);

  m_directionBuffer.create();
  m_directionBuffer.setUsagePattern(QOpenGLBuffer::UsagePattern::StreamDraw);
  m_directionBuffer.bind();
  m_directionBuffer.allocate(static_cast<int>(m_current_scene_size * sizeof(QVector3D)));
  m_arrow_shader.setAttributeArray(arrow_dir_attrib_location, GL_FLOAT, 0, 3);
  m_arrow_shader.enableAttributeArray(arrow_dir_attrib_location);
  glVertexAttribDivisor(arrow_dir_attrib_location, 1);

  m_vaoArrow.release();
  m_arrow_shader.release();
}

void GLScene::resize(std::size_t width, std::size_t height)
{
  m_camera.viewport(width, height);
  if (m_fbo)
  {
    delete m_fbo;
  }
  m_fbo = new QOpenGLFramebufferObject(static_cast<int>(width), static_cast<int>(height), m_fbo_format);
  m_graph.hasNewFrame(true);
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

void GLScene::project2D()
{
  m_graph.lock(GRAPH_LOCK_TRACE);
  std::size_t nodeCount = m_graph.hasExploration() ? m_graph.explorationNodeCount() : m_graph.nodeCount();
  std::size_t edgeCount = m_graph.hasExploration() ? m_graph.explorationEdgeCount() : m_graph.edgeCount();
  std::vector<QVector3D> positions;

  for (std::size_t i = 0; i < nodeCount; i++)
  {
    std::size_t n = m_graph.hasExploration() ? m_graph.explorationNode(i) : i;
    positions.push_back(m_graph.node(n).pos());
  }
  for (std::size_t i = 0; i < edgeCount; i++)
  {
    std::size_t n = m_graph.hasExploration() ? m_graph.explorationEdge(i) : i;
    positions.push_back(m_graph.transitionLabel(n).pos());
  }

  auto cam_rotate = [&](const QVector3D& u)
  {
    return m_camera.rotation().rotatedVector(u);
  };
  auto cam_anti_rotate = [&](const QVector3D& u)
  {
    return m_camera.rotation().inverted().rotatedVector(u);
  };
  QVector3D eye = m_camera.position();
  QVector3D e_z = QVector3D(0, 0, 1);
  QVector3D _debug_e_z = cam_anti_rotate(cam_rotate(e_z));
  mCRL2log(mcrl2::log::debug) << "Rot^-1 Rot (e_z): " << _debug_e_z.x() << "," << _debug_e_z.y() << ","
                              << _debug_e_z.z() << std::endl;
  QVector3D forward = (-m_camera.center() - eye).normalized();

  mCRL2log(mcrl2::log::debug) << "Forward: " << forward.x() << ", " << forward.y() << ", " << forward.z() << std::endl;

  auto perp_distance = [&](const QVector3D& u)
  {
    return QVector3D::dotProduct(u, forward);
  };
  auto compare = [&](const QVector3D& u, const QVector3D& v)
  {
    return perp_distance(u - eye) < perp_distance(v - eye);
  };

  std::sort(positions.begin(), positions.end(), compare);

  float plane_dist = !positions.empty() ? perp_distance(positions[positions.size() / 2] - eye) : 0.0f;
  QVector3D relative_reference_point = forward * plane_dist;
  QVector3D reference_point = eye + relative_reference_point;

  mCRL2log(mcrl2::log::debug) << "Cam pos: " << eye.x() << ", " << eye.y() << ", " << eye.z() << std::endl;

  mCRL2log(mcrl2::log::debug) << "R: (" << reference_point.x() << "," << reference_point.y() << ","
                              << reference_point.z() << ")" << std::endl;

  mCRL2log(mcrl2::log::debug) << "dist: " << plane_dist << std::endl;

  auto project = [&](const QVector3D& u)
  {
    QVector3D dir = (u - eye).normalized();
    float denom = QVector3D::dotProduct(dir, forward);
    if (std::abs(denom) < 0.0001f)
    {
      return u;
    }
    else
    {
      float t = QVector3D::dotProduct(relative_reference_point, forward) / denom;
      QVector3D projection = eye + t * dir;
      /*mCRL2log(mcrl2::log::debug)
          << "u: (" << u.x() << "," << u.y() << "," << u.z() << ") -> ("
          << projection.x() << "," << projection.y() << "," << projection.z()
          << ") t: " << t << " dir: (" << dir.x() << "," << dir.y() << ","
          << dir.z() << ")" << std::endl;*/
      return projection;
    }
  };
  auto transform = [&](const QVector3D& x)
  {
    return cam_rotate(project(x) - reference_point);
  };
  for (std::size_t i = 0; i < nodeCount; i++)
  {
    std::size_t n = m_graph.hasExploration() ? m_graph.explorationNode(i) : i;
    m_graph.node(n).pos_mutable() = transform(m_graph.node(n).pos());
    m_graph.node(n).pos_mutable().setZ(0);

    m_graph.stateLabel(n).pos_mutable() = transform(m_graph.stateLabel(n).pos());
    m_graph.stateLabel(n).pos_mutable().setZ(0);
  }
  for (std::size_t i = 0; i < edgeCount; i++)
  {
    std::size_t n = m_graph.hasExploration() ? m_graph.explorationEdge(i) : i;
    m_graph.transitionLabel(n).pos_mutable() = transform(m_graph.transitionLabel(n).pos());
    m_graph.transitionLabel(n).pos_mutable().setZ(0);

    m_graph.handle(n).pos_mutable() = transform(m_graph.handle(n).pos());
    m_graph.handle(n).pos_mutable().setZ(0);
  }

  QVector3D center_of_mass = slicedAverage(m_graph);
  for (std::size_t i = 0; i < nodeCount; i++)
  {
    std::size_t n = m_graph.hasExploration() ? m_graph.explorationNode(i) : i;
    m_graph.node(n).pos_mutable() -= center_of_mass;

    m_graph.stateLabel(n).pos_mutable() -= center_of_mass;
  }
  for (std::size_t i = 0; i < edgeCount; i++)
  {
    std::size_t n = m_graph.hasExploration() ? m_graph.explorationEdge(i) : i;
    m_graph.transitionLabel(n).pos_mutable() -= center_of_mass;

    m_graph.handle(n).pos_mutable() -= center_of_mass;
  }

  m_graph.unlock(GRAPH_LOCK_TRACE);
  m_camera.reset();
  m_camera.translate(center_of_mass);
  m_camera.m_zoom *= std::abs(plane_dist) / m_camera.m_zoom;
}

void GLScene::render()
{
  m_fbo->bind();
  QElapsedTimer render_timer;
  render_timer.restart();

  // reset the draw instance vectors
  for (auto ptr : m_drawInstances)
  {
    if (ptr)
    {
      ptr->resize(0);
    }
  }
  m_drawArc.resize(0);
  m_drawArcColors.resize(0);
  m_drawArrowColors.resize(0);
  m_drawArrowOffsets.resize(0);
  m_drawArrowDirections.resize(0);

  m_graph.lock(GRAPH_LOCK_TRACE); // enter critical section

  bool exploration_active = m_graph.hasExploration();
  std::size_t nodeCount = exploration_active ? m_graph.explorationNodeCount() : m_graph.nodeCount();
  std::size_t edgeCount = exploration_active ? m_graph.explorationEdgeCount() : m_graph.edgeCount();

  QMatrix4x4 viewProjMatrix = m_camera.projectionMatrix() * m_camera.viewMatrix();
  for (std::size_t i = 0; i < nodeCount; ++i)
  {
    renderNode(exploration_active ? m_graph.explorationNode(i) : i, false);
  }

  for (std::size_t i = 0; i < edgeCount; ++i)
  {
    renderEdge(exploration_active ? m_graph.explorationEdge(i) : i);
    renderHandle(exploration_active ? m_graph.explorationEdge(i) : i);
  }

  if (m_graph.hasExploration())
  {
    for (std::size_t i = 0; i < nodeCount; ++i)
    {
      renderNode(exploration_active ? m_graph.explorationNode(i) : i, true);
    }
  }
  m_graph.unlock(GRAPH_LOCK_TRACE);

  QElapsedTimer openglTimer;
  openglTimer.restart();

  // Cull polygons that are facing away (back) from the camera, where their
  // front is defined as counter clockwise by default, see glFrontFace, meaning
  // that the vertices that make up a triangle should be oriented counter
  // clockwise to show the triangle.
  glEnable(GL_CULL_FACE);

  // Enable depth testing, so that we don't have to care too much about
  // rendering in the right order.
  glEnable(GL_DEPTH_TEST);

  // Enable multisample antialiasing.
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  QColor clear(Qt::white);
  glClearColor(clear.red(), clear.green(), clear.blue(), 1.0);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  m_arc_shader.bind();
  m_vaoArc.bind();

  m_arc_shader.setViewMatrix(m_camera.viewMatrix());
  m_arc_shader.setViewProjMatrix(viewProjMatrix);
  m_arc_shader.setFogDensity(m_drawfog * m_fogdensity);
  if (m_drawArc.size() > 0)
  {
    std::size_t amount_to_render = m_drawArc.size();
    std::size_t index = 0;
    while (amount_to_render > 0)
    {
      std::size_t n = std::min(m_batch_size, amount_to_render);
      m_controlpointbuffer.bind();
      m_controlpointbuffer.write(0, &m_drawArc[index], static_cast<int>(n * 12 * sizeof(float)));
      m_colorBuffer.bind();
      m_colorBuffer.write(0, &m_drawArcColors[index], static_cast<int>(n * 3 * sizeof(float)));
      glDrawArraysInstanced(GL_LINE_STRIP, OFFSET_ARC, VERTICES_ARC, static_cast<GLsizei>(n));
      glCheckError();
      index += n;
      amount_to_render -= n;
    }
  }
  // we need this to make sure everything is drawn on top of the lines
  glClear(GL_DEPTH_BUFFER_BIT);

  m_vaoArc.release();
  m_arc_shader.release();

  m_arrow_shader.bind();
  m_vaoArrow.bind();

  m_arrow_shader.setVP(viewProjMatrix);
  m_arrow_shader.setScale(arrowheadSizeScaled());
  if (m_drawArrowOffsets.size() > 0)
  {
    std::size_t amount_to_render = m_drawArc.size();
    std::size_t index = 0;
    while (amount_to_render > 0)
    {
      std::size_t n = std::min(m_batch_size, amount_to_render);

      m_colorBuffer.bind();
      m_colorBuffer.write(0, &m_drawArrowColors[index], static_cast<int>(n * 4 * sizeof(float)));

      m_offsetBuffer.bind();
      m_offsetBuffer.write(0, &m_drawArrowOffsets[index], static_cast<int>(n * 3 * sizeof(float)));

      m_directionBuffer.bind();
      m_directionBuffer.write(0, &m_drawArrowDirections[index], static_cast<int>(n * 3 * sizeof(float)));

      glDrawArraysInstanced(GL_TRIANGLE_FAN, OFFSET_ARROWHEAD, VERTICES_ARROWHEAD, static_cast<int>(n));
      glDrawArraysInstanced(GL_TRIANGLE_FAN, OFFSET_ARROWHEAD_BASE, VERTICES_ARROWHEAD_BASE, static_cast<int>(n));

      index += n;
      amount_to_render -= n;
    }
  }

  m_vaoArrow.release();
  m_arrow_shader.release();

  m_node_shader.bind();
  m_vaoNode.bind();

  m_node_shader.setEye(m_camera.position());
  m_node_shader.setVP(viewProjMatrix);
  m_node_shader.setV(m_camera.viewMatrix());

  // All data has been accumulated in the associated vectors
  // float* matValues = new float[m_current_buffer_size * 16];
  for (DrawInstances* di_ptr : m_drawInstances)
  {
    if (di_ptr && di_ptr->size() > 0)
    {
      m_node_shader.setScale(di_ptr->scale);
      std::size_t amount_to_render = di_ptr->size();
      std::size_t index = 0;
      while (amount_to_render > 0)
      {
        std::size_t n = std::min(m_batch_size, amount_to_render);
        // NB: sizeof(QMatrix4x4) is NOT the same as 16 * sizeof(float)
        m_offsetBuffer.bind();
        m_offsetBuffer.write(0, &di_ptr->offsets[index * 3], n * 3 * sizeof(float));

        m_colorBuffer.bind();
        m_colorBuffer.write(0, &di_ptr->colors[index * 4], static_cast<int>(n * sizeof(QVector4D)));

        glDrawArraysInstanced(di_ptr->draw_mode, di_ptr->offset, di_ptr->vertices, static_cast<int>(n));
        index += n;
        amount_to_render -= n;
        glCheckError();
      }
    }
    else
    {
      // mCRL2log(mcrl2::log::debug) << "Skipping \"" << di_ptr->identifier <<
      // "\" because it is empty." << std::endl;
    }
  }
  // delete[] matValues;
  m_vaoNode.release();
  m_node_shader.release();

  m_fbo->release();
  // Use the painter to render the remaining text.
  glDisable(GL_DEPTH_TEST);
  m_graph.hasNewFrame(false);
  // m_graph.unlock(GRAPH_LOCK_TRACE); // exit critical section
  // Make sure that glGetError() is not an error.
  glCheckError();

  // Very important. If this is not disabled the QPainter WILL draw
  // unpredictably.
  glDisable(GL_CULL_FACE);

  // mCRL2log(mcrl2::log::debug) << "Frame time: " << render_timer.elapsed() <<
  // " OpenGL: " << openglTimer.elapsed() << std::endl;
}

void GLScene::renderText(QPainter& painter)
{
  if (!(m_drawstatelabels || m_drawstatenumbers || m_drawtransitionlabels))
  {
    return;
  }
  bool exploration_active = m_graph.hasExploration();
  std::size_t nodeCount = exploration_active ? m_graph.explorationNodeCount() : m_graph.nodeCount();
  std::size_t edgeCount = exploration_active ? m_graph.explorationEdgeCount() : m_graph.edgeCount();
  if (nodeCount < 1 || edgeCount < 1)
  {
    return;
  }

  QFont font = m_font;
  painter.setFont(font);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  // Draw node labels
  {
    std::vector<double> distances(nodeCount);
    std::vector<std::size_t> indices(nodeCount);
    QVector3D closest_node;
    QVector3D cam_pos = m_camera.position();
    QVector3D center_of_screen
        = m_camera.windowToWorld(QVector3D(m_camera.viewport().width() * 0.5, m_camera.viewport().height() * 0.5, 0));
    double distance_to_closest_node = 1e15;

    for (std::size_t i = 0; i < nodeCount; i++)
    {
      std::size_t n = exploration_active ? m_graph.explorationNode(i) : i;

      double dist = (m_graph.node(n).pos() - cam_pos).lengthSquared();

      if (dist < distance_to_closest_node)
      {
        distance_to_closest_node = dist;
        closest_node = m_graph.node(n).pos();
      }
    }

    // project onto plane
    QVector3D focus_point = center_of_screen
                            + QVector3D::dotProduct(closest_node - center_of_screen, cam_pos - center_of_screen)
                                  / (cam_pos - center_of_screen).lengthSquared() * (cam_pos - center_of_screen);
    for (std::size_t i = 0; i < nodeCount; ++i)
    {
      std::size_t n = exploration_active ? m_graph.explorationNode(i) : i;

      distances[i] = (m_graph.node(n).pos() - focus_point).lengthSquared();
      indices[i] = i;
    }

    auto compare = [&](std::size_t i, std::size_t j)
    {
      return distances[i] < distances[j];
    };

    std::sort(indices.begin(), indices.end(), compare);

    double closest = 10000000000000;
    double furthest = 0;
    for (std::size_t i = 0; i < std::min(nodeCount,
                                std::max(static_cast<std::size_t>(m_textLimitStateLabels),
                                    static_cast<std::size_t>(m_textLimitStateNumbers)));
         ++i)
    {
      std::size_t n = exploration_active ? m_graph.explorationNode(indices[i]) : indices[i];
      double dist = (m_graph.node(n).pos() - cam_pos).lengthSquared();
      distances[indices[i]] = dist;
      dist = std::sqrt(dist);
      closest = std::min(dist, closest);
      furthest = std::max(dist, furthest);
    }

    auto getScale = [&](std::size_t i) -> double
    {
      const double furthest_scale = 0.8;
      const double closest_scale = 1.0;
      if (!m_is_threedimensional || furthest == closest)
      {
        return closest_scale;
      }
      else
      {
        return (closest_scale
                - (closest_scale - furthest_scale) * (std::sqrt(distances[i]) - closest) / (furthest - closest));
      }
    };
    for (std::size_t i = 0; i < std::min(nodeCount,
                                std::max(static_cast<std::size_t>(m_textLimitStateLabels),
                                    static_cast<std::size_t>(m_textLimitStateNumbers)));
         ++i)
    {
      font.setPixelSize(getScale(indices[i]) * m_fontsize);
      painter.setFont(font);
      if (m_drawstatenumbers && i < static_cast<std::size_t>(m_textLimitStateNumbers))
      {
        renderStateNumber(painter, exploration_active ? m_graph.explorationNode(indices[i]) : indices[i]);
      }

      if (m_drawstatelabels && i < static_cast<std::size_t>(m_textLimitStateLabels))
      {
        renderStateLabel(painter, exploration_active ? m_graph.explorationNode(indices[i]) : indices[i]);
      }
    }
  }

  // Draw transition labels
  {
    std::vector<double> distances(edgeCount);
    std::vector<std::size_t> indices(edgeCount);
    QVector3D closest_node;
    QVector3D cam_pos = m_camera.position();
    QVector3D center_of_screen
        = m_camera.windowToWorld(QVector3D(m_camera.viewport().width() * 0.5, m_camera.viewport().height() * 0.5, 0));
    double distance_to_closest_node = 1e15;
    for (std::size_t i = 0; i < edgeCount; i++)
    {
      std::size_t n = exploration_active ? m_graph.explorationEdge(i) : i;

      double dist = (m_graph.transitionLabel(n).pos() - cam_pos).lengthSquared();

      if (dist < distance_to_closest_node)
      {
        distance_to_closest_node = dist;
        closest_node = m_graph.transitionLabel(n).pos();
      }
    }

    // project onto plane
    QVector3D focus_point = center_of_screen
                            + QVector3D::dotProduct(closest_node - center_of_screen, cam_pos - center_of_screen)
                                  / (cam_pos - center_of_screen).lengthSquared() * (cam_pos - center_of_screen);

    for (std::size_t i = 0; i < edgeCount; ++i)
    {
      std::size_t n = exploration_active ? m_graph.explorationEdge(i) : i;

      distances[i] = (m_graph.transitionLabel(n).pos() - focus_point).lengthSquared();
      indices[i] = i;
    }

    auto compare = [&](std::size_t i, std::size_t j)
    {
      return distances[i] < distances[j];
    };

    std::sort(indices.begin(), indices.end(), compare);

    double closest = 10000000000000;
    double furthest = 0;
    for (std::size_t i = 0; i < std::min(edgeCount, static_cast<std::size_t>(m_textLimitTransLabels)); ++i)
    {
      std::size_t n = exploration_active ? m_graph.explorationEdge(indices[i]) : indices[i];
      double dist = (m_graph.transitionLabel(n).pos() - cam_pos).lengthSquared();
      distances[indices[i]] = dist;
      dist = std::sqrt(dist);
      closest = std::min(dist, closest);
      furthest = std::max(dist, furthest);
    }

    auto getScale = [&](std::size_t i) -> double
    {
      const double furthest_scale = 0.8;
      const double closest_scale = 1.0;
      if (!m_is_threedimensional || furthest == closest)
      {
        return closest_scale;
      }
      else
      {
        return (closest_scale
                - (closest_scale - furthest_scale) * (std::sqrt(distances[i]) - closest) / (furthest - closest));
      }
    };

    if (m_drawtransitionlabels)
    {
      for (std::size_t i = 0; i < std::min(edgeCount, static_cast<std::size_t>(m_textLimitTransLabels)); ++i)
      {
        font.setPixelSize(getScale(indices[i]) * m_fontsize);
        painter.setFont(font);
        renderTransitionLabel(painter, exploration_active ? m_graph.explorationEdge(indices[i]) : indices[i]);
      }
    }
  }
}

GLScene::Selection GLScene::select(int x, int y)
{
  Selection s{SelectableObject::none, 0};
  selectObject(s, x, y, SelectableObject::node)
      || selectObject(s, x, y, SelectableObject::handle)
      // Only select transition labels when they are rendered
      || (m_drawtransitionlabels && selectObject(s, x, y, SelectableObject::label))
      // Only select state labels when they are rendered
      || (m_drawstatelabels && selectObject(s, x, y, SelectableObject::slabel));
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

void GLScene::drawCenteredText3D(QPainter& painter,
    const QString& text,
    const QVector3D& position,
    const QVector3D& color)
{
  QVector3D window = m_camera.worldToWindow(position);
  float fog = 0.0f;
  if (!text.isEmpty() && window.z() <= 1.0f && isVisible(position, fog)) // There is text, that is not behind the camera
                                                                         // and it is visible.
  {
    QColor qcolor = vectorToColor(color);
    qcolor.setAlpha(static_cast<int>(255 * (1.0f - fog)));

    drawCenteredText(painter, window.x() / m_device_pixel_ratio, window.y() / m_device_pixel_ratio, text, qcolor);
  }
}

void GLScene::drawCenteredStaticText3D(QPainter& painter,
    const QStaticText& text,
    const QVector3D& position,
    const QVector3D& color)
{
  QVector3D window = m_camera.worldToWindow(position);
  float fog = 0.0f;
  if (window.z() <= 1.0f && isVisible(position, fog)) // There is text, that is not behind the camera
                                                      // and it is visible.
  {
    QColor qcolor = vectorToColor(color);
    qcolor.setAlpha(static_cast<int>(255 * (1.0f - fog)));

    drawCenteredStaticText(painter, window.x() / m_device_pixel_ratio, window.y() / m_device_pixel_ratio, text, qcolor);
  }
}

bool GLScene::isVisible(const QVector3D& position, float& fogamount)
{
  // Should match the vertex shader: fogAmount = (1.0f - exp(-1.0f *
  // pow(distance * g_density, 2)));
  float distance = (m_camera.position() - position).length();
  fogamount = m_drawfog * (1.0f - std::exp(-1.0f * std::pow(distance * m_fogdensity, 2.0f)));
  return (distance < m_camera.viewdistance() && fogamount < 0.99f);
}

void GLScene::renderEdge(std::size_t i)
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
  std::array<QVector3D, 4> control = calculateArc(from, via, to, edge.is_selfloop());

  // Calculate the position of the tip of the arrow, and the orientation of the
  // arc
  QVector3D start;
  QVector3D tip;
  QVector3D vec;
  {
    const Math::Circle to_node{to, 0.5f * nodeSizeScaled()};
    const Math::CubicBezier arc(control);
    Math::Scalar t = Math::make_intersection(to_node, arc).guessNearBack();
    tip = to_node.project(arc.at(t));
    const Math::Circle from_node{from, 0.5f * nodeSizeScaled()};
    t = Math::make_intersection(from_node, arc).guessNearFront();
    start = from_node.project(arc.at(t));
    const Math::Circle head{to, 0.5f * nodeSizeScaled() + arrowheadSizeScaled()};
    const Math::Scalar s = Math::make_intersection(head, arc).guessNearBack();
    const QVector3D top = arc.at(s);
    vec = tip - top;
  }
  control[0] = start;
  control[3] = tip;

  QVector3D arcColor(m_graph.handle(i).selected(), 0.0f, 0.0f);

  // Use the arc shader to draw the arcs.
  m_drawArc.emplace_back(control);
  m_drawArcColors.emplace_back(arcColor);

  // If to == ctrl[2], then something odd is going on.
  vec.normalize();

  float fog = 0.0f;
  if (isVisible(tip, fog))
  {
    // Apply the fog color.
    // m_global_shader.setColor();
    QVector4D color = QVector4D(applyFog(arcColor, fog), 1.0f);

    m_drawArrowColors.push_back(color);
    m_drawArrowOffsets.push_back(tip);
    m_drawArrowDirections.push_back(vec);
  }
}

void GLScene::renderHandle(std::size_t i)
{
  if (!m_drawselfloops && m_graph.edge(i).is_selfloop())
  {
    return;
  }

  const Graph::Node& handle = m_graph.handle(i);
  if (handle.selected() > 0.1f || handle.locked())
  {
    QVector4D line(2 * handle.selected() - 1.0f, 0.0f, 0.0f, 1.0f);
    QVector4D fill(1.0f, 1.0f, 1.0f, 1.0f);

    if (handle.locked())
    {
      fill = QVector4D(0.7f, 0.7f, 0.7f, 1.0f);
    }

    m_drawHandleBody.push_back(handle.pos(), fill);
    m_drawHandleOutline.push_back(handle.pos(), line);
  }
}

void GLScene::renderNode(std::size_t i, bool transparent)
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
  float alpha = (m_graph.hasExploration() && !node.active()) ? 1.0f : 1.0f; // Disabled for now until the transparent
                                                                            // window issue can be resolved.
  if (transparent || alpha > 0.99f)                                         // Check if these elements are visible and
                                                                            // opaque if transparency is disallowed.
  {
    // Node border color: red when selected, blue when probabilistic, and black otherwise. Apply fogging
    // afterwards.
    QVector3D line(0.6f * node.selected(), 0.0f, 1.0f*node.is_probabilistic());

    QVector4D color = QVector4D(applyFog(line, fog), alpha);
    m_drawNodeBorder.push_back(node.pos(), color);

    color = QVector4D(applyFog(fill, fog), alpha);
    // Draw the sphere to fill the internal area of a node. 
    m_drawSphere.push_back(node.pos(), color);

    if (m_graph.hasExploration() && !m_graph.isBridge(i) && m_graph.initialState() != i)
    {
      float s = (fill.x() < 0.5f && fill.y() < 0.5f && fill.z() < 0.5f) ? 0.2f : -0.2f;
      QVector4D hint = QVector4D(fill.x() + s, fill.y() + s, fill.z() + s, 1);

      // When the node is active, which means that its successors are shown in
      // exploration mode, only the "minus" is drawn by omitting the vertical
      // rectangle of the whole "plus" shape.
      if (node.active())
      {
        m_drawMinusHint.push_back(node.pos(), hint);
      }
      else
      {
        m_drawPlusHint.push_back(node.pos(), hint);
      }
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
  QVector3D fill(std::max(label.color().x(), label.selected()),
      std::min(label.color().y(), 1.0f - label.selected()),
      std::min(label.color().z(), 1.0f - label.selected()));
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

  QVector3D color(std::max(label.color().x(), label.selected()),
      std::min(label.color().y(), 1.0f - label.selected()),
      std::min(label.color().z(), 1.0f - label.selected()));
  drawCenteredStaticText3D(painter, m_state_labels[label.labelindex()], label.pos(), color);
}

void GLScene::renderStateNumber(QPainter& painter, std::size_t i)
{
  const Graph::NodeNode& node = m_graph.node(i);
  QVector3D color(0.0f, 0.0f, 0.0f);
  drawCenteredText3D(painter, QString::number(i), node.pos(), color);
}

bool GLScene::selectObject(GLScene::Selection& s, int x, int y, SelectableObject type)
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
  // Take the conjugated rotation of the camera to position the node in the
  // right direction
  QQuaternion centerRotation = m_camera.rotation().conjugated();

  // And compensate for the perspective of the camera on the object if its not
  // in the center of the screen
  QVector3D posToCamera = m_camera.position() - position;
  posToCamera.normalize();
  QVector3D camera = centerRotation.rotatedVector(QVector3D(0.0f, 0.0f, 1.0f));
  camera.normalize();
  // Compute the roration with the cross product and the dot product (aka
  // inproduct)
  QQuaternion perspectiveRotation = QQuaternion::fromAxisAndAngle(QVector3D::crossProduct(camera, posToCamera),
      radiansToDegrees(std::acos(QVector3D::dotProduct(camera, posToCamera))));

  // Return the combination of both rotations
  // NB: the order of this multiplication is important
  return perspectiveRotation * centerRotation;
}
