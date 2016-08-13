// Author(s): Rimco Boudewijns and Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <QtOpenGL>

#include "glwidget.h"
#include "springlayout.h"
#include "ui_glwidget.h"
#include "mcrl2/utilities/logger.h"

#include <map>

struct MoveRecord
{
    virtual ~MoveRecord() {}

    Graph::Node* node;
    virtual void move(const Graph::Coord3D& pos)
    {
      node->pos() = pos;
    }
    virtual void grab(Graph::Graph& graph, size_t index) = 0;
    virtual void release(bool toggleLocked)
    {
      if (toggleLocked)
        node->set_locked(!node->locked());
      node->set_anchored(node->locked());
    }
    const Graph::Coord3D& pos()
    {
      return node->pos();
    }
};

struct LabelMoveRecord : public MoveRecord {
    void grab(Graph::Graph &graph, size_t index)
    {
      node = &graph.transitionLabel(index);
      node->set_anchored(true);
    }
};

struct StateLabelMoveRecord : public MoveRecord {
    void grab(Graph::Graph &graph, size_t index)
    {
      node = &graph.stateLabel(index);
      node->set_anchored(true);
    }
};

struct HandleMoveRecord : public MoveRecord
{
    bool movingLabel;
    LabelMoveRecord label;
    void grab(Graph::Graph& graph, size_t index)
    {
      node = &graph.handle(index);
      node->set_anchored(true);
      movingLabel = !graph.transitionLabel(index).anchored();
      if (movingLabel)
        label.grab(graph, index);
    }
    void release(bool toggleLocked) {
      MoveRecord::release(toggleLocked);
      if (movingLabel)
        label.release(false);
    }
    void move(const Graph::Coord3D &pos)
    {
      MoveRecord::move(pos);
      if (movingLabel)
        label.move(pos);
    }
};

struct NodeMoveRecord : public MoveRecord
{
    StateLabelMoveRecord label;
    std::vector<HandleMoveRecord> edges;
    std::vector<Graph::Node*> endpoints;
    void grab(Graph::Graph& graph, size_t index)
    {
      node = &graph.node(index);
      node->set_anchored(true);
      size_t nlabels = 0;
      for (size_t i = 0; i < graph.edgeCount(); ++i)
      {
        Graph::Edge e = graph.edge(i);
        if (e.from() != index)
        {
          size_t temp = e.from();
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
    void release(bool toggleLocked)
    {
      MoveRecord::release(toggleLocked);
      for (size_t i = 0; i < edges.size(); ++i)
      {
        edges[i].release(false); // Do not toggle the edges around this node
      }
      label.release(toggleLocked);
    }
    void move(const Graph::Coord3D &pos)
    {
      MoveRecord::move(pos);
      for (size_t i = 0; i < edges.size(); ++i)
      {
        edges[i].move((pos + endpoints[i]->pos()) / 2.0);
      }
      label.move(pos);
    }
};


GLWidget::GLWidget(Graph::Graph& graph, QWidget *parent)
  : QGLWidget(parent), m_ui(NULL), m_graph(graph), m_painting(false), m_paused(true)
{
  m_scene = new GLScene(m_graph, devicePixelRatio());
  // Set up OpenGL to use alpha channel
  QGLFormat fmt = QGLFormat::defaultFormat();
  fmt.setVersion(1, 2);
  fmt.setDepth(true);
  fmt.setAlpha(true);
  setFormat(fmt);
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
  m_dragnode = NULL;
}

void GLWidget::resume()
{
  m_paused = false;
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
      return NULL;
  }
}

void GLWidget::updateSelection()
{
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

  m_hover = m_scene->select(pos.x(), pos.y());
  selnode = select_object(m_hover, m_graph);
  if (selnode)
  {
    if (selnode->selected() <= 0)
      m_selections.push_back(m_hover);
    selnode->selected() = 1.0f;

  }
  if (m_hover.selectionType == GLScene::so_label || m_hover.selectionType == GLScene::so_edge)
  {
    GLScene::Selection s = m_hover;
    s.selectionType = GLScene::so_handle;
    selnode = select_object(s, m_graph);
    if (selnode->selected() <= 0)
      m_selections.push_back(s);
    selnode->selected() = 0.5f;
  }
}

void GLWidget::initializeGL()
{
  m_scene->init(Qt::white);
  resizeGL(width(), height());
}

void GLWidget::resizeGL(int width, int height)
{
  m_scene->resize(width, height);
  emit widgetResized(m_scene->size());
}

void GLWidget::paintGL()
{
  // On OSX, there is a mysterious "invalid drawable" error, which seems to correspond
  // to not having a valid drawing surface in OpenGL. It looks like the paintGL routine
  // is called after OpenGL is requested to initialize, but before it has finished. As
  // there seems to be no workaround for this problem, the best we can do is to clear
  // the OpenGL error buffer before painting, so we don't trigger assert statements
  // later in the code due to this bug.
  glGetError();

  if (m_paused)
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
  else
  {
    updateSelection();
    m_scene->render();
  }
  if (m_scene->resizing())
    emit widgetResized(m_scene->size());
}

void GLWidget::mousePressEvent(QMouseEvent *e)
{
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
    m_dragstart = e->pos();
    if (e->modifiers() == Qt::ControlModifier)
    {
      if (e->button() == Qt::LeftButton)
        m_dragmode = dm_translate;
    }
    else if (e->modifiers() == Qt::ShiftModifier)
    {
      if (e->button() == Qt::LeftButton && m_scene->size().z > 1)
        m_dragmode = dm_rotate;
    }
    else if (e->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier))
    {
      if (e->button() == Qt::LeftButton)
        m_dragmode = dm_translate;
    }
    else
    {
      if (m_hover.selectionType == GLScene::so_none)
      {
        if (e->button() == Qt::RightButton && m_scene->size().z > 1)
          m_dragmode = dm_rotate;
//        else if (e->button() == Qt::RightButton)
//          m_dragmode = dm_rotate_2d;
        else if (e->button() == Qt::MidButton || ((e->buttons() & (Qt::LeftButton | Qt::RightButton)) == (Qt::LeftButton | Qt::RightButton)))
          m_dragmode = dm_zoom;
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
            m_dragnode = NULL;
            m_dragmode = dm_none;
            break;
        }
        if (m_dragnode)
          m_dragnode->grab(m_graph, m_hover.index);
      }
    }
  }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *e)
{
  if (m_dragmode == dm_dragnode)
  {
    m_dragnode->release(e->button() == Qt::RightButton);
    delete m_dragnode;
    m_dragnode = NULL;
  }
  m_dragmode = dm_none;
}

void GLWidget::wheelEvent(QWheelEvent *e)
{
  m_scene->zoom(pow(1.0005f, e->delta()));
}

void GLWidget::mouseMoveEvent(QMouseEvent *e)
{
  if (m_dragmode != dm_none)
  {
    QPoint vec = e->pos() - m_dragstart;

    Graph::Coord3D vec3 = m_scene->eyeToWorld(e->pos().x(), e->pos().y(), 1.0f) - m_scene->eyeToWorld(m_dragstart.x(), m_dragstart.y(), 1.0f);

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
        m_scene->rotate(Graph::Coord3D(360.0f * vec.y() / height(),
                                       360.0f * vec.x() / width(),
                                       0.0f));
        break;
      case dm_translate:
        m_scene->translate(vec3);
        break;
      case dm_dragnode:
        m_dragnode->move(m_scene->eyeToWorld(e->pos().x(), e->pos().y(), m_scene->worldToEye(m_dragnode->pos()).z));
        break;
      case dm_zoom:
        m_scene->zoom(pow(1.0005f, vec.y()));
        break;
      default:
        break;
    }

    m_dragstart = e->pos();
  }
}

void GLWidget::rebuild()
{
  m_scene->updateLabels();
  m_scene->updateShapes();
}

void GLWidget::setDepth(float depth, size_t animation)
{
  Graph::Coord3D size = m_scene->size();
  size.z = depth;
  m_scene->setRotation(Graph::Coord3D(0, 0, 0), animation);
  m_scene->setTranslation(Graph::Coord3D(0, 0, 0), animation);
  m_scene->setSize(size, animation);
}

Graph::Coord3D GLWidget::size3()
{
  return m_scene->size();
}

void GLWidget::resetViewpoint(size_t animation)
{
  m_scene->setRotation(Graph::Coord3D(0, 0, 0), animation);
  m_scene->setTranslation(Graph::Coord3D(0, 0, 0), animation);
  m_scene->setZoom(0.95f, animation);
}

void GLWidget::setPaint(const QColor &color)
{
  m_paintcolor = color;
}

void GLWidget::startPaint()
{
  m_painting = true;
}

void GLWidget::endPaint()
{
  m_painting = false;
}

void GLWidget::saveVector(const QString &filename)
{
  QString lcfn = filename.toLower();
  if (lcfn.endsWith(".pdf"))
  {
    m_scene->renderVectorGraphics(filename.toUtf8(), GL2PS_PDF);
  }
  else
  if (lcfn.endsWith(".ps"))
  {
    m_scene->renderVectorGraphics(filename.toUtf8(), GL2PS_PS);
  }
  else
  if (lcfn.endsWith(".eps"))
  {
    m_scene->renderVectorGraphics(filename.toUtf8(), GL2PS_EPS);
  }
  else
  if (lcfn.endsWith(".svg"))
  {
    m_scene->renderVectorGraphics(filename.toUtf8(), GL2PS_SVG);
  }
  else
  if (lcfn.endsWith(".tex"))
  {
    m_scene->renderVectorGraphics(filename.toUtf8(), GL2PS_PGF);
  }
  else
  {
	mCRL2log(mcrl2::log::error) << "Unable to determine file type from extension." << std::endl;
  }
}

void GLWidget::saveTikz(const QString& filename, float aspectRatio)
{
  m_scene->renderLatexGraphics(filename, aspectRatio);
}

void GLWidget::savePixmap(const QString &filename, const int w, const int h)
{
  m_scene->resize(w, h);
  renderPixmap(w, h).save(filename);
  m_scene->resize(width(), height());
}

GLWidgetUi* GLWidget::ui(QWidget *parent)
{
  if (!m_ui)
    m_ui = new GLWidgetUi(*this, parent);
  return m_ui;
}

GLWidgetUi::GLWidgetUi(GLWidget& widget, QWidget *parent)
  : QDockWidget(parent), m_widget(widget)
{
  QColor initialcolor(255, 0, 0);
  m_ui.setupUi(this);
  m_colordialog = new QColorDialog(initialcolor, this);
  selectColor(initialcolor);
  m_ui.spinRadius->setValue(m_widget.nodeSize());
  m_ui.spinFog->setValue(m_widget.fogDistance());

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
  if (paint)
    m_widget.startPaint();
  else
    m_widget.endPaint();
}
