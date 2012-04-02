#include "glwidget.h"
#include "springlayout.h"
#include "ui_glwidget.h"

#include <QtOpenGL>
#include <map>

GLWidget::GLWidget(Graph::Graph& graph, QWidget *parent)
    : QGLWidget(parent), m_ui(NULL), m_graph(graph)
{
    m_scene = new GLScene(m_graph);    
}

GLWidget::~GLWidget()
{
    delete m_scene;
    delete m_ui;
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
    if (m_painting)
    {
        if (m_hover.selectionType == GLScene::so_node)
        {
            Graph::NodeNode& node = m_graph.node(m_hover.index);
            node.color[0] = m_paintcolor.redF();
            node.color[1] = m_paintcolor.greenF();
            node.color[2] = m_paintcolor.blueF();
        }
        m_dragmode = dm_paint;
    }
    else
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
    if (m_dragmode != dm_none)
    {
        QPoint vec = e->pos() - m_dragstart;
        Graph::Coord3D vec3 = m_scene->eyeToWorld(vec.x() + m_dragstart.x(), vec.y() + m_dragstart.y(), 0);
        vec3 -= m_scene->eyeToWorld(m_dragstart.x(), m_dragstart.y(), 0);
        vec3.z = 0;

        switch (m_dragmode)
        {
        case dm_paint:
            if (m_hover.selectionType == GLScene::so_node)
            {
                Graph::NodeNode& node = m_graph.node(m_hover.index);
                node.color[0] = m_paintcolor.redF();
                node.color[1] = m_paintcolor.greenF();
                node.color[2] = m_paintcolor.blueF();
            }
            break;
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

void GLWidget::renderToFile(const QString &filename, const QString &filter)
{
    if (filter.startsWith("PDF"))
    {
        m_scene->renderVectorGraphics(filename.toUtf8(), GL2PS_PDF);
    }
    else
    if (filter.startsWith("Postscript"))
    {
        m_scene->renderVectorGraphics(filename.toUtf8(), GL2PS_PS);
    }
    else
    if (filter.startsWith("Encapsulated Postscript"))
    {
        m_scene->renderVectorGraphics(filename.toUtf8(), GL2PS_EPS);
    }
    else
    if (filter.startsWith("SVG"))
    {
        m_scene->renderVectorGraphics(filename.toUtf8(), GL2PS_SVG);
    }
    else
    if (filter.startsWith("LaTeX"))
    {
        m_scene->renderVectorGraphics(filename.toUtf8(), GL2PS_TEX);
    }
    else
    if (filter.startsWith("PGF"))
    {
        m_scene->renderVectorGraphics(filename.toUtf8(), GL2PS_PGF);
    }
    else
    {
        renderPixmap(1024, 768).save(filename);
        m_scene->resize(width(), height());
    }
}

void GLWidget::toggleLabels(bool show)
{
    m_scene->setDrawLabels(show);
}

GLWidgetUi* GLWidget::ui(QWidget *parent)
{
    if (!m_ui)
        m_ui = new GLWidgetUi(*this, parent);
    return m_ui;
}

GLWidgetUi::GLWidgetUi(GLWidget& widget, QWidget *parent)
    : QDockWidget(parent), m_widget(widget), m_ui(new Ui::GLWidget)
{
    QColor initialcolor(255, 0, 0);
    m_ui->setupUi(this);
    m_colordialog = new QColorDialog(initialcolor, this);
    selectColor(initialcolor);

    connect(m_colordialog, SIGNAL(colorSelected(QColor)), this, SLOT(selectColor(QColor)));
    connect(m_ui->btnPaint, SIGNAL(toggled(bool)), this, SLOT(togglePaintMode(bool)));
    connect(m_ui->btnSelectColor, SIGNAL(clicked()), m_colordialog, SLOT(exec()));
    connect(m_ui->cbStateLabels, SIGNAL(toggled(bool)), &m_widget, SLOT(toggleLabels(bool)));
}

GLWidgetUi::~GLWidgetUi()
{
    delete m_ui;
    delete m_colordialog;
}

void GLWidgetUi::selectColor(const QColor& color)
{
    QPixmap icon(16, 16);
    QPainter painter;
    painter.begin(&icon);
    painter.fillRect(icon.rect(), color);
    painter.end();
    m_ui->btnSelectColor->setIcon(QIcon(icon));
    m_widget.setPaint(color);
}

void GLWidgetUi::togglePaintMode(bool paint)
{
    if (paint)
        m_widget.startPaint();
    else
        m_widget.endPaint();
}
