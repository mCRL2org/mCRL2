#include "glwidget.h"
#include "springlayout.h"

#include <QtOpenGL>
#include <map>

GLWidget::GLWidget(Graph::Graph& graph, QWidget *parent)
    : QGLWidget(parent), m_graph(graph)
{
    m_scene = new GLScene(m_graph);
}

GLWidget::~GLWidget()
{
    delete m_scene;
}

inline Graph::Node* select_object(const GLScene::Selection& s, Graph::Graph& g)
{
    switch (s.selectionType)
    {
    case GLScene::so_label:
        return &g.label(s.index);
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
        if (selnode->selected > 0.05)
        {
            selnode->selected -= 0.05;
            ++it;
        }
        else
        {
            selnode->selected = 0.0;
            it = m_selections.erase(it);
        }
    }

    QPoint pos = mapFromGlobal(QCursor::pos());

    m_hover = m_scene->select(pos.x(), pos.y());
    selnode = select_object(m_hover, m_graph);
    if (selnode)
    {
        if (!selnode->selected > 0)
            m_selections.push_back(m_hover);
        selnode->selected = 1.0;

    }
    if (m_hover.selectionType == GLScene::so_label || m_hover.selectionType == GLScene::so_edge)
    {
        GLScene::Selection s = m_hover;
        s.selectionType = GLScene::so_handle;
        selnode = select_object(s, m_graph);
        if (!selnode->selected > 0)
            m_selections.push_back(s);
        selnode->selected = 0.5;
    }
}

void GLWidget::initializeGL()
{
    m_scene->init(parentWidget()->palette().background().color());
    resizeGL(width(), height());
}

void GLWidget::resizeGL(int width, int height)
{
    m_scene->resize(width, height);
    emit widgetResized(m_scene->size());
}

void GLWidget::paintGL()
{
    updateSelection();
    m_scene->render();
    if (m_scene->resizing())
        emit widgetResized(m_scene->size());
}

void GLWidget::mousePressEvent(QMouseEvent *e)
{
    m_dragstart = e->pos();
    if (m_hover.selectionType == GLScene::so_none)
    {
        if (e->button() == Qt::RightButton && m_scene->size().z > 1)
            m_dragmode = dm_rotate;
        else if (e->button() == Qt::MiddleButton)
            m_dragmode = dm_zoom;
        else if (e->button() == Qt::LeftButton)
            m_dragmode = dm_translate;
    }
    else
    {
        m_dragmode = dm_dragnode;
        m_dragnode = select_object(m_hover, m_graph);
        m_dragnode->anchored = true;
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (m_dragmode == dm_dragnode)
    {
        if (e->button() == Qt::RightButton)
        {
            m_dragnode->locked = !m_dragnode->locked;
        }
        m_dragnode->anchored = m_dragnode->locked;
    }
    m_dragmode = dm_none;
}

void GLWidget::wheelEvent(QWheelEvent *e)
{
    m_scene->zoom(pow(1.0005, (float)e->delta()));
}

void GLWidget::mouseMoveEvent(QMouseEvent *e)
{
    QPoint vec = e->pos() - m_dragstart;
    Graph::Coord3D vec3 = m_scene->eyeToWorld(vec.x() + m_dragstart.x(), vec.y() + m_dragstart.y(), 0);
    vec3 -= m_scene->eyeToWorld(m_dragstart.x(), m_dragstart.y(), 0);
    vec3.z = 0;

    switch (m_dragmode)
    {
    case dm_rotate:
        m_scene->rotate(Graph::Coord3D(360.0 * vec.y() / height(),
                                       360.0 * vec.x() / width(),
                                       0.0));
        break;
    case dm_translate:
        m_scene->translate(vec3);
        break;
    case dm_dragnode:
        m_dragnode->pos = m_scene->eyeToWorld(e->pos().x(), e->pos().y(), m_scene->worldToEye(m_dragnode->pos).z);
        break;
    case dm_zoom:
        m_scene->zoom(pow(1.0005, (float)vec.y()));
        break;
    default:
        break;
    }

    m_dragstart = e->pos();
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
    m_scene->setZoom(1.0, animation);
}
