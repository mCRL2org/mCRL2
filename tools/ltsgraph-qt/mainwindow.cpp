#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "springlayout.h"
#include "glwidget.h"
#include "mcrl2/lts/lts_lts.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);

    // Add graph area
    m_glwidget = new GLWidget(m_graph, m_ui->frame);
    m_ui->widgetLayout->addWidget(m_glwidget);

    // Create springlayout algorithm + UI
    m_layout = new Graph::SpringLayout(m_graph);
    Graph::SpringLayoutUi* springlayoutui = m_layout->ui(this);
    addDockWidget(Qt::RightDockWidgetArea, springlayoutui);

    // Create timer for rendering (at 25fps)
    m_timer = new QTimer(this);

    // Connect signals & slots
    connect(m_glwidget, SIGNAL(widgetResized(const Graph::Coord3D&)), this, SLOT(onWidgetResized(const Graph::Coord3D&)));
    connect(m_ui->actOpenFile, SIGNAL(triggered()), this, SLOT(onOpenFileClicked()));
    connect(springlayoutui, SIGNAL(runningChanged(bool)), m_ui->actGenerateRandomGraph, SLOT(setDisabled(bool)));
    connect(m_ui->actLayoutControl, SIGNAL(toggled(bool)), springlayoutui, SLOT(setVisible(bool)));
    connect(springlayoutui, SIGNAL(visibilityChanged(bool)), m_ui->actLayoutControl, SLOT(setChecked(bool)));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    connect(m_ui->act3D, SIGNAL(toggled(bool)), this, SLOT(on3DChanged(bool)));
    connect(m_ui->actLayout, SIGNAL(toggled(bool)), springlayoutui, SLOT(setActive(bool)));
    connect(m_ui->actReset, SIGNAL(triggered()), m_glwidget, SLOT(resetViewpoint()));

    m_glwidget->setDepth(0.0, 0);

    m_anim = 0;
    m_timer->start(40);
}

MainWindow::~MainWindow()
{
    delete m_timer;
    delete m_layout;
    delete m_ui;
    delete m_glwidget;
}

void MainWindow::onWidgetResized(const Graph::Coord3D& newsize)
{
    m_graph.clip(-newsize / 2.0, newsize / 2.0);
    m_layout->setClipRegion(-newsize / 2.0, newsize / 2.0);
}

void MainWindow::on3DChanged(bool enabled)
{
    if (enabled)
        m_glwidget->setDepth(1000, 25);
    else
        m_glwidget->setDepth(0, 80);
}

void MainWindow::onTimer()
{
    m_glwidget->updateGL();
}

void MainWindow::onOpenFileClicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Open LTS"),
                                                    tr(""),
                                                    tr("All labelled transition systems (*.aut *.fsm *.dot *.lts)"));
    if (!filename.isNull())
    {
        m_graph.load(filename, -m_glwidget->size3() / 2.0, m_glwidget->size3() / 2.0);
        m_glwidget->rebuild();
    }
}
