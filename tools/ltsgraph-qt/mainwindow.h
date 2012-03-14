#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "graph.h"
#include "glwidget.h"
#include "springlayout.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void onOpenFileClicked();
    void onWidgetResized(const Graph::Coord3D& newsize);
    void on3DChanged(bool enabled);
    void onTimer();
private:
    Ui::MainWindow *m_ui;
    GLWidget* m_glwidget;
    Graph::Graph m_graph;
    Graph::SpringLayout* m_layout;
    QTimer* m_timer;

    float m_dXRot, m_dYRot, m_dDepth, m_targetDepth;
    int m_anim;
};

#endif // MAINWINDOW_H
