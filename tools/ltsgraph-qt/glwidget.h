#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QtOpenGL>
#include "graph.h"
#include "glscene.h"
#include "mcrl2/lts/lts.h"

class GLWidgetImpl;

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    enum DragMode
    {
        dm_none,
        dm_dragnode,
        dm_rotate,
        dm_translate,
        dm_zoom
    };
private:
    Graph::Graph& m_graph;
    GLScene::Selection m_hover;
    DragMode m_dragmode;
    Graph::Node* m_dragnode;
    QPoint m_dragstart;
    GLScene* m_scene;
    std::list<GLScene::Selection> m_selections;

    void drawEdge(size_t i);
    void drawNode(size_t i);
    void drawLabel(size_t i);
    void drawHandle(size_t i);
    void drawScene();
    void resetMatrices(bool selectionmode);
    void updateSelection();
    void billboardAt(const Graph::Coord3D& pos);
public:
    explicit GLWidget(Graph::Graph& graph, QWidget *parent = 0);
    virtual ~GLWidget();

    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int width, int height);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void wheelEvent(QWheelEvent *e);

    void setDepth(float depth, size_t animation = 1);
    void rebuild();
    Graph::Coord3D size3();
signals:
    void widgetResized(const Graph::Coord3D& newsize);
public slots:
    void resetViewpoint(size_t animation = 1);
};

#endif // GLWIDGET_H
