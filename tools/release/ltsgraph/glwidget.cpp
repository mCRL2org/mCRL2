// Author(s): Rimco Boudewijns and Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "glwidget.h"

#include "export.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/gui/arcball.h"
#include "ui_glwidget.h"

#include <map>

/// \brief Minimum distance for a drag to be registered (pixels)
constexpr float DRAG_MIN_DIST = 20.0f;

struct MoveRecord
{
  virtual ~MoveRecord() = default;

  Graph::Node* node;
  virtual void move(const QVector3D& pos)
  {
    node->pos_mutable() = pos;
  }
  virtual void grab(Graph::Graph& graph, std::size_t index) = 0;
  virtual void release(bool toggleLocked)
  {
    if (toggleLocked) {
      node->set_locked(!node->locked());
    }
    node->set_anchored(node->locked());
  }
  const QVector3D& pos()
  {
    return node->pos();
  }
};

struct LabelMoveRecord : public MoveRecord {
  void grab(Graph::Graph& graph, std::size_t index) override
  {
    node = &graph.transitionLabel(index);
    node->set_anchored(true);
  }
};

struct StateLabelMoveRecord : public MoveRecord {
  void grab(Graph::Graph& graph, std::size_t index) override
  {
    node = &graph.stateLabel(index);
    node->set_anchored(true);
  }
};

struct HandleMoveRecord : public MoveRecord
{
  bool movingLabel;
  LabelMoveRecord label;
  void grab(Graph::Graph& graph, std::size_t index) override
  {
    node = &graph.handle(index);
    node->set_anchored(true);
    movingLabel = !graph.transitionLabel(index).anchored();
    if (movingLabel) {
      label.grab(graph, index);
    }
  }
  void release(bool toggleLocked) override {
    MoveRecord::release(toggleLocked);
    if (movingLabel) {
      label.release(false);
    }
  }
  void move(const QVector3D& pos) override
  {
    MoveRecord::move(pos);
    if (movingLabel) {
      label.move(pos);
    }
  }
};

struct NodeMoveRecord : public MoveRecord
{
  StateLabelMoveRecord label;
  std::vector<HandleMoveRecord> edges;
  std::vector<Graph::Node*> endpoints;
  void grab(Graph::Graph& graph, std::size_t index) override
  {
    node = &graph.node(index);
    node->set_anchored(true);
    std::size_t nlabels = 0;
    for (std::size_t i = 0; i < graph.edgeCount(); ++i)
    {
      Graph::Edge e = graph.edge(i);
      if (e.from() != index)
      {
        std::size_t temp = e.from();
        e.from() = e.to();
        e.to() = temp;
      }
      if (e.from() == index && !graph.handle(i).anchored())
      {
        edges.resize(nlabels + 1);
        endpoints.resize(nlabels + 1);
        endpoints[nlabels] = &graph.node(e.to());
        edges[nlabels++].grab(graph, i);
      }
    }
    label.grab(graph, index);
  }
  void release(bool toggleLocked) override
  {
    MoveRecord::release(toggleLocked);
    for (std::size_t i = 0; i < edges.size(); ++i)
    {
      edges[i].release(false); // Do not toggle the edges around this node
    }
    label.release(toggleLocked);
  }
  void move(const QVector3D& pos) override
  {
    MoveRecord::move(pos);
    for (std::size_t i = 0; i < edges.size(); ++i)
    {
      edges[i].move((pos + endpoints[i]->pos()) / 2.0);
    }
    label.move(pos);
  }
};


GLWidget::GLWidget(Graph::Graph& graph, QWidget* parent)
  : QOpenGLWidget(parent),
    m_graph(graph),
    m_scene(*this, m_graph)
{
  setMouseTracking(true);

  m_scene.setDevicePixelRatio(devicePixelRatio());
}

GLWidget::~GLWidget()
{
  delete m_ui;
}

void GLWidget::pause()
{
  m_paused = true;
  m_selections.clear();
  m_dragmode = dm_none;
  m_dragnode = nullptr;
}

void GLWidget::resume()
{
  m_paused = false;
  update();
}

inline Graph::Node* select_object(const GLScene::Selection& s, Graph::Graph& g)
{
  switch (s.selectionType)
  {
    case GLScene::SelectableObject::label:
      return &g.transitionLabel(s.index);
    case GLScene::SelectableObject::slabel:
      return &g.stateLabel(s.index);
    case GLScene::SelectableObject::handle:
      return &g.handle(s.index);
    case GLScene::SelectableObject::node:
      return &g.node(s.index);
    default:
      return nullptr;
  }
}

void GLWidget::updateSelection()
{
  m_scene.setDevicePixelRatio(devicePixelRatio());

  // Determine the mouse position relative for the GLWidget.
  QPoint pos = mapFromGlobal(QCursor::pos());

  GLScene::Selection prev = m_hover;
  m_hover = m_scene.select(pos.x(), pos.y());

  // Indicates that the rendering should be updated because the selection has changed.
  bool needupdate = prev.selectionType != m_hover.selectionType || prev.index != m_hover.index;

  // Reduce the selection percentage for all (no longer) selected items by 5 percent and remove them from the selection when it goes below 5 percent.
  bool contains_hover = false;
  for (std::list<GLScene::Selection>::iterator it = m_selections.begin(); it != m_selections.end();)
  {
    if (*it != m_hover)
    {
      Graph::Node* selnode = select_object(*it, m_graph);
      if (selnode->selected() > 0.05f)
      {
        selnode->selected() -= 0.05f;
        ++it;
        needupdate |= true;
      }
      else
      {
        selnode->selected() = 0.0f;
        it = m_selections.erase(it);
      }
    }
    else
    {
      contains_hover = true;
      ++it;
    }
  }

  // Insert the hover selection, as it did not exist before.
  if (m_hover.has_selection() && !contains_hover)
  {
    // Set the selection amount to 100 percent for the object under the cursor.
    Graph::Node* hovernode = select_object(m_hover, m_graph);
    hovernode->selected() = 1.0f;

    if (!contains_hover)
    {
      m_selections.push_back(m_hover);
    }
  }

  /// If the selected item was a label or edge we also mark the edge handle to be selected for 50 percent.
  if (m_hover.selectionType == GLScene::SelectableObject::label || m_hover.selectionType == GLScene::SelectableObject::edge)
  {
    GLScene::Selection s = m_hover;
    s.selectionType = GLScene::SelectableObject::handle;
    Graph::Node* selnode = select_object(s, m_graph);

    if (selnode->selected() <= 0)
    {
      m_selections.push_back(s);
    }

    selnode->selected() = 0.5f;
  }

  if (needupdate)
  {
    update();
  }
}

void GLWidget::initializeGL()
{
  // Check whether context creation succeeded and print the OpenGL major.minor version.
  if (isValid())
  {
    QPair<int, int> version = format().version();
    mCRL2log(mcrl2::log::verbose) << "Created an OpenGL " << version.first << "." << version.second << " Core context.\n";
  }
  else
  {
    mCRL2log(mcrl2::log::error) << "The context was not created succesfully.\n";
    std::abort();
  }

  // Enable real-time logging of OpenGL errors when the GL_KHR_debug extension is available.
  m_logger = new QOpenGLDebugLogger(this);
  if (m_logger->initialize())
  {
    connect(m_logger, &QOpenGLDebugLogger::messageLogged, this, &GLWidget::logMessage);
    m_logger->startLogging();
  }
  else
  {
    mCRL2log(mcrl2::log::debug) << "The Qt5 OpenGL debug logger can not be initialized.\n";
  }

  m_scene.initialize();
}

void GLWidget::resizeGL(int width, int height)
{
  m_scene.camera().viewport(width, height);
}

void GLWidget::paintGL()
{
  QPainter painter(this);

  if (!m_paused)
  {
    m_scene.setDevicePixelRatio(devicePixelRatio());
    m_scene.update();
    m_scene.render(painter);

    // Updates the selection percentage (and checks for new selections under the cursor).
    updateSelection();
  }
}

void GLWidget::mousePressEvent(QMouseEvent* e)
{
  updateSelection();
  if (m_painting)
  {
    if (m_hover.selectionType == GLScene::SelectableObject::node)
    {
      Graph::NodeNode& node = m_graph.node(m_hover.index);
      node.color() = m_paintcolor;
    }
    if (m_hover.selectionType == GLScene::SelectableObject::label)
    {
      Graph::LabelNode& node = m_graph.transitionLabel(m_hover.index);
      node.color() = m_paintcolor;
    }
    if (m_hover.selectionType == GLScene::SelectableObject::slabel)
    {
      Graph::LabelNode& node = m_graph.stateLabel(m_hover.index);
      node.color() = m_paintcolor;
    }
    m_dragmode = dm_paint;
  }
  else
  {
    m_draglength = QVector2D(0.0, 0.0);
    m_dragstart = e->pos();
    if (e->modifiers() == Qt::ControlModifier)
    {
      if (e->button() == Qt::LeftButton) {
        m_dragmode = dm_translate;
      }
    }
    else if (e->modifiers() == Qt::ShiftModifier)
    {
      if (e->button() == Qt::LeftButton && m_is_threedimensional) {
        m_dragmode = dm_rotate;
      }
    }
    else if (e->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier))
    {
      if (e->button() == Qt::LeftButton) {
        m_dragmode = dm_translate;
      }
    }
    else
    {
      if (m_hover.selectionType == GLScene::SelectableObject::none)
      {
        if (e->button() == Qt::RightButton && m_is_threedimensional) {
          m_dragmode = dm_rotate;
        }
        else if (e->button() == Qt::MidButton || ((e->buttons() & (Qt::LeftButton | Qt::RightButton)) == (Qt::LeftButton | Qt::RightButton))) {
          m_dragmode = dm_zoom;
        }
      }
      else
      {
        m_dragmode = dm_dragnode;
        switch (m_hover.selectionType)
        {
          case GLScene::SelectableObject::node:
            m_dragnode = new NodeMoveRecord;
            break;
          case GLScene::SelectableObject::handle:
            m_dragnode = new HandleMoveRecord;
            break;
          case GLScene::SelectableObject::label:
            m_dragnode = new LabelMoveRecord;
            break;
          case GLScene::SelectableObject::slabel:
            m_dragnode = new StateLabelMoveRecord;
            break;
          default:
            m_dragnode = nullptr;
            m_dragmode = dm_none;
            break;
        }
        if (m_dragnode != nullptr) {
          m_dragnode->grab(m_graph, m_hover.index);
        }
      }
    }
  }
  update();
}

void GLWidget::mouseReleaseEvent(QMouseEvent* e)
{
  updateSelection();

  if (m_dragmode == dm_dragnode)
  {
    NodeMoveRecord* noderec = dynamic_cast<NodeMoveRecord*>(m_dragnode);
    if (m_hover.selectionType == GLScene::SelectableObject::node && e->button() == Qt::LeftButton
        && (noderec != nullptr) && m_draglength.length() < DRAG_MIN_DIST)
    {
      // A node has been clicked (not dragged):
      if (m_graph.isClosable(m_hover.index)) {
        m_graph.toggleOpen(m_hover.index);
      }
    }
    m_dragnode->release(e->button() == Qt::RightButton);
    delete m_dragnode;
    m_dragnode = nullptr;
    update();
  }

  m_dragmode = dm_none;
}

void GLWidget::wheelEvent(QWheelEvent* e)
{
  ArcballCameraView& camera = m_scene.camera();
  camera.zoom(camera.zoom() * pow(1.0005f, -e->delta()));
  update();
}

void GLWidget::mouseMoveEvent(QMouseEvent* e)
{
  updateSelection();
  ArcballCameraView& camera = m_scene.camera();

  if (m_dragmode != dm_none)
  {
    QPoint vec = e->pos() - m_dragstart;
    m_draglength += (QVector2D) vec;

    switch (m_dragmode)
    {
      case dm_paint:
        if (m_hover.selectionType == GLScene::SelectableObject::node)
        {
          Graph::NodeNode& node = m_graph.node(m_hover.index);
          node.color() = m_paintcolor;
        }
        if (m_hover.selectionType == GLScene::SelectableObject::label)
        {
          Graph::LabelNode& node = m_graph.transitionLabel(m_hover.index);
          node.color() = m_paintcolor;
        }
        if (m_hover.selectionType == GLScene::SelectableObject::slabel)
        {
          Graph::LabelNode& node = m_graph.stateLabel(m_hover.index);
          node.color() = m_paintcolor;
        }
        break;
      case dm_rotate:
      case dm_rotate_2d:
        {
          QQuaternion rotation = mcrl2::gui::arcballRotation(m_dragstart, e->pos());
          camera.rotation(rotation * camera.rotation());
          break;
        }
      case dm_translate:
        {
          int dx = e->pos().x() - m_dragstart.x();
          int dy = e->pos().y() - m_dragstart.y();
          QVector3D vec3(dx, -dy, 0);
          camera.center(camera.center() + camera.rotation().conjugated() * vec3);
          break;
        }
      case dm_dragnode:
        {
          QVector3D position(e->pos().x(), e->pos().y(), camera.worldToWindow(m_dragnode->pos()).z());
          m_dragnode->move(camera.windowToWorld(position));
          break;
        }
      case dm_zoom:
        camera.zoom(camera.zoom() * pow(1.0005f, vec.y()));
        break;
      default:
        break;
    }

    m_dragstart = e->pos();
    update();
  }
}

void GLWidget::rebuild()
{
  makeCurrent();
  update();
}

void GLWidget::set3D(bool enabled)
{
  if (!enabled)
  {
    m_scene.camera().resetRotation();
  }
  m_is_threedimensional = enabled;
  update();
}

void GLWidget::resetViewpoint(std::size_t)
{
  m_scene.camera().reset();
  update();
}

void GLWidget::setPaint(const QColor& color)
{
  m_paintcolor = QVector3D(color.redF(), color.greenF(), color.blueF());
}

const QVector3D& GLWidget::getPaint() const
{
  return m_paintcolor;
}

void GLWidget::startPaint()
{
  m_painting = true;
}

void GLWidget::endPaint()
{
  m_painting = false;
}

void GLWidget::saveTikz(const QString& filename, float aspectRatio)
{
  export_graph_as_tikz_input(m_graph, filename, aspectRatio);
}

void GLWidget::saveBitmap(const QString& filename)
{
  grabFramebuffer().save(filename);
}

void GLWidget::logMessage(const QOpenGLDebugMessage& debugMessage)
{
  mCRL2log(mcrl2::log::debug) << "OpenGL: " << debugMessage.message().toStdString() << "\n";
}

/// Source code for the GLWidgetUI

GLWidgetUi* GLWidget::ui(QWidget* parent)
{
  if (m_ui == nullptr) {
    m_ui = new GLWidgetUi(*this, parent);
  }
  return m_ui;
}

GLWidgetUi::GLWidgetUi(GLWidget& widget, QWidget* parent)
  : QDockWidget(parent), m_widget(widget)
{
  QColor initialcolor(255, 0, 0);
  m_ui.setupUi(this);
  m_colordialog = new QColorDialog(initialcolor, this);
  selectColor(initialcolor);

  connect(m_colordialog, SIGNAL(colorSelected(QColor)), this, SLOT(selectColor(QColor)));
  connect(m_ui.btnPaint, SIGNAL(toggled(bool)), this, SLOT(setPaintMode(bool)));
  connect(m_ui.btnSelectColor, SIGNAL(clicked()), m_colordialog, SLOT(exec()));
  connect(m_ui.cbTransitionLabels, SIGNAL(toggled(bool)), &m_widget, SLOT(toggleTransitionLabels(bool)));
  connect(m_ui.cbStateLabels, SIGNAL(toggled(bool)), &m_widget, SLOT(toggleStateLabels(bool)));
  connect(m_ui.cbStateNumbers, SIGNAL(toggled(bool)), &m_widget, SLOT(toggleStateNumbers(bool)));
  connect(m_ui.cbSelfLoops, SIGNAL(toggled(bool)), &m_widget, SLOT(toggleSelfLoops(bool)));
  connect(m_ui.cbInitial, SIGNAL(toggled(bool)), &m_widget, SLOT(toggleInitialMarking(bool)));
  connect(m_ui.cbFog, SIGNAL(toggled(bool)), &m_widget, SLOT(toggleFog(bool)));
  connect(m_ui.spinRadius, SIGNAL(valueChanged(int)), &m_widget, SLOT(setNodeSize(int)));
  connect(m_ui.spinFontSize, SIGNAL(valueChanged(int)), &m_widget, SLOT(setFontSize(int)));
  connect(m_ui.spinFog, SIGNAL(valueChanged(int)), &m_widget, SLOT(setFogDensity(int)));
}

GLWidgetUi::~GLWidgetUi()
{
  delete m_colordialog;
}

void GLWidgetUi::selectColor(const QColor& color)
{
  QPixmap icon(16, 16);
  QPainter painter(&icon);
  painter.fillRect(icon.rect(), color);
  m_ui.btnSelectColor->setIcon(QIcon(icon));
  m_widget.setPaint(color);
}

void GLWidgetUi::setPaintMode(bool paint)
{
  if (paint)
  {
    m_widget.startPaint();
  }
  else
  {
    m_widget.endPaint();
  }
}

void GLWidgetUi::setSettings(QByteArray state)
{
  if (state.isEmpty())
  {
    return;
  }
  QDataStream in(&state, QIODevice::ReadOnly);

  quint32 nodeSize, fogDistance, fontSize;
  bool paint, transitionLabels, stateLabels, stateNumbers, selfLoops, initial, fog;
  QColor color;
  in >> nodeSize >> fogDistance >> fontSize
     >> paint >> transitionLabels >> stateLabels >> stateNumbers >> selfLoops >> initial >> fog
     >> color
    ;

  if (in.status() == QDataStream::Ok)
  {
    m_ui.spinRadius->setValue(nodeSize);
    m_ui.spinFog->setValue(fogDistance);
    m_ui.spinFontSize->setValue(fontSize);
    m_ui.btnPaint->setChecked(paint);
    m_ui.cbTransitionLabels->setChecked(transitionLabels);
    m_ui.cbStateLabels->setChecked(stateLabels);
    m_ui.cbStateNumbers->setChecked(stateNumbers);
    m_ui.cbSelfLoops->setChecked(selfLoops);
    m_ui.cbInitial->setChecked(initial);
    m_ui.cbFog->setChecked(fog);
    m_colordialog->setCurrentColor(color);
    selectColor(color);
  }
}

QByteArray GLWidgetUi::settings()
{
  QByteArray result;
  QDataStream out(&result, QIODevice::WriteOnly);

  out << quint32(m_ui.spinRadius->value())
      << quint32(m_ui.spinFog->value())
      << quint32(m_ui.spinFontSize->value())
      << bool(m_ui.btnPaint->isChecked())
      << bool(m_ui.cbTransitionLabels->isChecked())
      << bool(m_ui.cbStateLabels->isChecked())
      << bool(m_ui.cbStateNumbers->isChecked())
      << bool(m_ui.cbSelfLoops->isChecked())
      << bool(m_ui.cbInitial->isChecked())
      << bool(m_ui.cbFog->isChecked())
      << QVector3D(m_widget.getPaint())
      ;

  return result;
}
