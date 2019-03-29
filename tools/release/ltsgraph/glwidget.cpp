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

#include <QtOpenGL>

#include <map>

#define DRAG_MIN_DIST 20.0f ///< Minimum distance for a drag to be registered (pixels)

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
    m_graph(graph)
{
  // Create an OpenGL 3.3 surface without depth, alpha and stencil buffers and with vsync enabled.
  QSurfaceFormat surfaceFormat = QSurfaceFormat::defaultFormat();
  surfaceFormat.setVersion(3, 3);
  surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
  surfaceFormat.setDepthBufferSize(1);
  surfaceFormat.setAlphaBufferSize(1);
  surfaceFormat.setStencilBufferSize(1);
  surfaceFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
  surfaceFormat.setSwapInterval(1);

  // We use the GL_KHR_debug extension to provide realtime logging of OpenGL errors.
  surfaceFormat.setOption(QSurfaceFormat::DebugContext);

  setFormat(surfaceFormat);
  setMouseTracking(true);

  m_scene = new GLScene(*this, m_graph);
  m_scene->setDevicePixelRatio(devicePixelRatio());
}

GLWidget::~GLWidget()
{
  delete m_scene;
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
    case GLScene::so_label:
      return &g.transitionLabel(s.index);
    case GLScene::so_slabel:
      return &g.stateLabel(s.index);
    case GLScene::so_handle:
      return &g.handle(s.index);
    case GLScene::so_node:
      return &g.node(s.index);
    default:
      return nullptr;
  }
}

void GLWidget::updateSelection()
{
  m_scene->setDevicePixelRatio(devicePixelRatio());
  Graph::Node* selnode;
  for (std::list<GLScene::Selection>::iterator it = m_selections.begin(); it != m_selections.end();)
  {
    selnode = select_object(*it, m_graph);
    if (selnode->selected() > 0.05f)
    {
      selnode->selected() -= 0.05f;
      ++it;
    }
    else
    {
      selnode->selected() = 0.0f;
      it = m_selections.erase(it);
    }
  }

  QPoint pos = mapFromGlobal(QCursor::pos());

  GLScene::Selection prev = m_hover;
  m_hover = m_scene->select(pos.x(), pos.y());
  bool needupdate = prev.selectionType != m_hover.selectionType || prev.index != m_hover.index;
  selnode = select_object(m_hover, m_graph);
  if (selnode != nullptr)
  {
    if (selnode->selected() <= 0) {
      m_selections.push_back(m_hover);
    }
    selnode->selected() = 1.0f;

  }
  if (m_hover.selectionType == GLScene::so_label || m_hover.selectionType == GLScene::so_edge)
  {
    GLScene::Selection s = m_hover;
    s.selectionType = GLScene::so_handle;
    selnode = select_object(s, m_graph);
    if (selnode->selected() <= 0) {
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
  if (!isValid())
  {
    mCRL2log(mcrl2::log::error) << "The context was not created succesfully.\n";
    std::abort();
  }

  m_logger = new QOpenGLDebugLogger(this);
  if (!m_logger->initialize())
  {
    mCRL2log(mcrl2::log::warning) << "The Qt5 OpenGL debug logger can not be initialized.\n";
  }
  else
  {
    connect(m_logger, &QOpenGLDebugLogger::messageLogged, this, &GLWidget::logMessage);
    m_logger->startLogging();
  }

  m_scene->initialize();
}

void GLWidget::resizeGL(int width, int height)
{
  m_scene->resize(width, height);
  emit widgetResized(m_scene->size());
}

void GLWidget::paintGL()
{
  if (m_paused)
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
  else
  {
    m_scene->setDevicePixelRatio(devicePixelRatio());
    m_scene->render();
    if (!m_scene->animationFinished())
    {
      update();
    }
  }
  if (m_scene->resizing()) {
    emit widgetResized(m_scene->size());
  }
}

void GLWidget::mousePressEvent(QMouseEvent* e)
{
  makeCurrent();
  updateSelection();
  if (m_painting)
  {
    if (m_hover.selectionType == GLScene::so_node)
    {
      Graph::NodeNode& node = m_graph.node(m_hover.index);
      node.color(0) = m_paintcolor.redF();
      node.color(1) = m_paintcolor.greenF();
      node.color(2) = m_paintcolor.blueF();
    }
    if (m_hover.selectionType == GLScene::so_label)
    {
      Graph::LabelNode& node = m_graph.transitionLabel(m_hover.index);
      node.color(0) = m_paintcolor.redF();
      node.color(1) = m_paintcolor.greenF();
      node.color(2) = m_paintcolor.blueF();
    }
    if (m_hover.selectionType == GLScene::so_slabel)
    {
      Graph::LabelNode& node = m_graph.stateLabel(m_hover.index);
      node.color(0) = m_paintcolor.redF();
      node.color(1) = m_paintcolor.greenF();
      node.color(2) = m_paintcolor.blueF();
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
      if (e->button() == Qt::LeftButton && m_scene->size().z() > 1) {
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
      if (m_hover.selectionType == GLScene::so_none)
      {
        if (e->button() == Qt::RightButton && m_scene->size().z() > 1) {
          m_dragmode = dm_rotate;
        }
//        else if (e->button() == Qt::RightButton)
//          m_dragmode = dm_rotate_2d;
        else if (e->button() == Qt::MidButton || ((e->buttons() & (Qt::LeftButton | Qt::RightButton)) == (Qt::LeftButton | Qt::RightButton))) {
          m_dragmode = dm_zoom;
        }
      }
      else
      {
        m_dragmode = dm_dragnode;
        switch (m_hover.selectionType)
        {
          case GLScene::so_node:
            m_dragnode = new NodeMoveRecord;
            break;
          case GLScene::so_handle:
            m_dragnode = new HandleMoveRecord;
            break;
          case GLScene::so_label:
            m_dragnode = new LabelMoveRecord;
            break;
          case GLScene::so_slabel:
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
  makeCurrent();
  updateSelection();
  if (m_dragmode == dm_dragnode)
  {
    NodeMoveRecord* noderec = dynamic_cast<NodeMoveRecord*>(m_dragnode);
    if (m_hover.selectionType == GLScene::so_node && e->button() == Qt::LeftButton
        && (noderec != nullptr) && m_draglength.length() < DRAG_MIN_DIST)
    {
      // A node has been clicked (not dragged):
      if (m_graph.isToggleable(m_hover.index)) {
        m_graph.toggleActive(m_hover.index);
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
  m_scene->zoom(pow(1.0005f, e->delta()));
  update();
}

void GLWidget::mouseMoveEvent(QMouseEvent* e)
{
  makeCurrent();
  updateSelection();
  if (m_dragmode != dm_none)
  {
    QPoint vec = e->pos() - m_dragstart;
    m_draglength += (QVector2D) vec;

    switch (m_dragmode)
    {
      case dm_paint:
        if (m_hover.selectionType == GLScene::so_node)
        {
          Graph::NodeNode& node = m_graph.node(m_hover.index);
          node.color(0) = m_paintcolor.redF();
          node.color(1) = m_paintcolor.greenF();
          node.color(2) = m_paintcolor.blueF();
        }
        if (m_hover.selectionType == GLScene::so_label)
        {
          Graph::LabelNode& node = m_graph.transitionLabel(m_hover.index);
          node.color(0) = m_paintcolor.redF();
          node.color(1) = m_paintcolor.greenF();
          node.color(2) = m_paintcolor.blueF();
        }
        if (m_hover.selectionType == GLScene::so_slabel)
        {
          Graph::LabelNode& node = m_graph.stateLabel(m_hover.index);
          node.color(0) = m_paintcolor.redF();
          node.color(1) = m_paintcolor.greenF();
          node.color(2) = m_paintcolor.blueF();
        }
        break;
      case dm_rotate:
      case dm_rotate_2d:
        {
          QQuaternion rotation = mcrl2::gui::arcballRotation(m_dragstart, e->pos());
          m_scene->rotate(rotation);
          break;
        }
      case dm_translate:
        {
          int dx = e->pos().x() - m_dragstart.x();
          int dy = e->pos().y() - m_dragstart.y();
          QVector3D vec3(dx, -dy, 0);
          m_scene->translate(vec3 / m_scene->magnificationFactor());
          break;
        }
      case dm_dragnode:
        {
          m_dragnode->move(m_scene->eyeToWorld(e->pos().x(), e->pos().y(), m_scene->worldToEye(m_dragnode->pos()).z()));
          break;
        }
      case dm_zoom:
        m_scene->zoom(pow(1.0005f, vec.y()));
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

void GLWidget::setDepth(float depth, std::size_t animation)
{
  makeCurrent();
  QVector3D size = m_scene->size();
  size.setZ(depth);
  m_scene->setRotation(QQuaternion(1, 0, 0, 0), animation);
  m_scene->setTranslation(QVector3D(0, 0, 0), animation);
  m_scene->setSize(size, animation);
  update();
}

QVector3D GLWidget::size3()
{
  return m_scene->size();
}

void GLWidget::resetViewpoint(std::size_t animation)
{
  makeCurrent();
  m_scene->setRotation(QQuaternion(1, 0, 0, 0), animation);
  m_scene->setTranslation(QVector3D(0, 0, 0), animation);
  m_scene->setZoom(5.0, animation);
  update();
}

void GLWidget::setPaint(const QColor& color)
{
  m_paintcolor = color;
}

const QColor& GLWidget::getPaint() const
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
  makeCurrent();
  grabFramebuffer().save(filename);
}

void GLWidget::logMessage(const QOpenGLDebugMessage& debugMessage)
{
  mCRL2log(mcrl2::log::debug) << "OpenGL message: " << debugMessage.message().toStdString() << "\n.";
}

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
  connect(m_ui.btnPaint, SIGNAL(toggled(bool)), this, SLOT(togglePaintMode(bool)));
  connect(m_ui.btnSelectColor, SIGNAL(clicked()), m_colordialog, SLOT(exec()));
  connect(m_ui.cbTransitionLabels, SIGNAL(toggled(bool)), &m_widget, SLOT(toggleTransitionLabels(bool)));
  connect(m_ui.cbStateLabels, SIGNAL(toggled(bool)), &m_widget, SLOT(toggleStateLabels(bool)));
  connect(m_ui.cbStateNumbers, SIGNAL(toggled(bool)), &m_widget, SLOT(toggleStateNumbers(bool)));
  connect(m_ui.cbSelfLoops, SIGNAL(toggled(bool)), &m_widget, SLOT(toggleSelfLoops(bool)));
  connect(m_ui.cbInitial, SIGNAL(toggled(bool)), &m_widget, SLOT(toggleInitialMarking(bool)));
  connect(m_ui.cbFog, SIGNAL(toggled(bool)), &m_widget, SLOT(toggleFog(bool)));
  connect(m_ui.spinRadius, SIGNAL(valueChanged(int)), &m_widget, SLOT(setNodeSize(int)));
  connect(m_ui.spinFontSize, SIGNAL(valueChanged(int)), &m_widget, SLOT(setFontSize(int)));
  connect(m_ui.spinFog, SIGNAL(valueChanged(int)), &m_widget, SLOT(setFogDistance(int)));
}

GLWidgetUi::~GLWidgetUi()
{
  delete m_colordialog;
}

void GLWidgetUi::selectColor(const QColor& color)
{
  QPixmap icon(16, 16);
  QPainter painter;
  painter.begin(&icon);
  painter.fillRect(icon.rect(), color);
  painter.end();
  m_ui.btnSelectColor->setIcon(QIcon(icon));
  m_widget.setPaint(color);
}

void GLWidgetUi::togglePaintMode(bool paint)
{
  if (paint) {
    m_widget.startPaint();
  }
  else {
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
    // always show the transition labels
    // m_ui.cbTransitionLabels->setChecked(transitionLabels);
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
      << QColor(m_widget.getPaint())
      ;

  return result;
}
