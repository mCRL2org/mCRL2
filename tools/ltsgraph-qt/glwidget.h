#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QtOpenGL>
#include "graph.h"
#include "glscene.h"
#include "mcrl2/lts/lts.h"

class GLWidgetImpl;

class GLWidgetUi;

class MoveRecord;

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
      dm_zoom,
      dm_paint
    };
  private:
    GLWidgetUi* m_ui;
    Graph::Graph& m_graph;
    GLScene::Selection m_hover;
    DragMode m_dragmode;
    MoveRecord* m_dragnode;
    QPoint m_dragstart;
    QColor m_paintcolor;
    bool m_painting;
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
    void renderToFile(const QString& filename, const QString& filter, const int w = 1024, const int h = 768);
    void setPaint(const QColor& color);
    void startPaint();
    void endPaint();
    Graph::Coord3D size3();
    GLWidgetUi* ui(QWidget* parent = 0);

    size_t nodeSize();
  signals:
    void widgetResized(const Graph::Coord3D& newsize);
  public slots:
    void resetViewpoint(size_t animation = 1);
    void toggleTransitionLabels(bool show);
    void toggleStateLabels(bool show);
    void toggleStateNumbers(bool show);
    void toggleInitialMarking(bool show);
    void setNodeSize(int size);
};

namespace Ui
{
  class GLWidget;
}

class GLWidgetUi : public QDockWidget
{
    Q_OBJECT
  private:
    GLWidget& m_widget;
    Ui::GLWidget* m_ui;
    QColorDialog* m_colordialog;
  public:
    GLWidgetUi(GLWidget& widget, QWidget *parent = 0);
    ~GLWidgetUi();
  public slots:
    void selectColor(const QColor& color);
    void togglePaintMode(bool paint);
};

#endif // GLWIDGET_H
