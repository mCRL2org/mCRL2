#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "springlayout.h"
#include "information.h"
#include "glwidget.h"
#include "mcrl2/lts/lts_lts.h"
#include <QFileDialog>
#include "dimensionsdialog.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  m_ui(new Ui::MainWindow)
{

  m_ui->setupUi(this);
  m_ui->dockOutput->setVisible(false);

  // Add graph area
  m_glwidget = new GLWidget(m_graph, m_ui->frame);
  m_glwidget->setDepth(0.0, 0);
  m_ui->widgetLayout->addWidget(m_glwidget);

  // Create springlayout algorithm + UI
  m_layout = new Graph::SpringLayout(m_graph);
  Graph::SpringLayoutUi* springlayoutui = m_layout->ui(this);
  addDockWidget(Qt::RightDockWidgetArea, springlayoutui);
  springlayoutui->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

  // Create GLWidget UI
  GLWidgetUi* glwidgetui = m_glwidget->ui(this);
  addDockWidget(Qt::RightDockWidgetArea, glwidgetui);
  glwidgetui->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

  // Add information UI
  m_information = new Graph::Information(m_graph);
  Graph::InformationUi* informationui = m_information->ui(this);
  addDockWidget(Qt::RightDockWidgetArea, informationui);
  informationui->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

  // Create timer for rendering (at 25fps)
  m_timer = new QTimer(this);

  // Connect signals & slots
  connect(m_glwidget, SIGNAL(widgetResized(const Graph::Coord3D&)), this, SLOT(onWidgetResized(const Graph::Coord3D&)));
  connect(m_ui->actLayoutControl, SIGNAL(toggled(bool)), springlayoutui, SLOT(setVisible(bool)));
  connect(m_ui->actVisualization, SIGNAL(toggled(bool)), glwidgetui, SLOT(setVisible(bool)));
  connect(m_ui->actInformation, SIGNAL(toggled(bool)), informationui, SLOT(setVisible(bool)));
  connect(m_ui->actOutput, SIGNAL(toggled(bool)), m_ui->dockOutput, SLOT(setVisible(bool)));
  connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimer()));
  connect(m_ui->act3D, SIGNAL(toggled(bool)), this, SLOT(on3DChanged(bool)));
  connect(m_ui->actLayout, SIGNAL(toggled(bool)), springlayoutui, SLOT(setActive(bool)));
  connect(m_ui->actReset, SIGNAL(triggered()), m_glwidget, SLOT(resetViewpoint()));
  connect(m_ui->actOpenFile, SIGNAL(triggered()), this, SLOT(onOpenFile()));
  connect(m_ui->actExportImage, SIGNAL(triggered()), this, SLOT(onExportImage()));
  connect(m_ui->actImport_XML, SIGNAL(triggered()), this, SLOT(onImportXML()));
  connect(m_ui->actExport_XML, SIGNAL(triggered()), this, SLOT(onExportXML()));

  m_timer->start(40);
}

MainWindow::~MainWindow()
{
  delete m_timer;
  delete m_layout;
  delete m_information;
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

void MainWindow::openFile(QString fileName)
{
  if (!fileName.isNull())
  {
    m_layout->ui()->setActive(false);
    m_glwidget->resetViewpoint(0);
    m_graph.load(fileName, -m_glwidget->size3() / 2.0, m_glwidget->size3() / 2.0);
    m_glwidget->rebuild();
    m_information->update();
  }
}

void MainWindow::onOpenFile()
{

  QString fileName(QFileDialog::getOpenFileName(this, tr("Open file"), QString(),
                                                tr("Labelled transition systems (*.lts *.aut *.fsm *.dot)")));

  openFile(fileName);
}

void MainWindow::onExportImage()
{
  QString selectedFilter = tr("Bitmap images (*.png *.jpg *.jpeg *.gif *.bmp *.pbm *.pgm *.ppm *.xbm *.xpm)");
  QString fileName(QFileDialog::getSaveFileName(this, tr("Save file"), QString(),
                                                tr("Bitmap images (*.png *.jpg *.jpeg *.gif *.bmp *.pbm *.pgm *.ppm *.xbm *.xpm);;"
                                                   "PDF (*.pdf);;"
                                                   "Postscript (*.ps);;"
                                                   "Encapsulated Postscript (*.eps);;"
                                                   "SVG (*.svg);;"
                                                   "LaTeX (*.tex);;"
                                                   "PGF (*.pgf);;"
                                                   ),
                                                &selectedFilter));

  if (!fileName.isNull())
  {
    if (selectedFilter.startsWith("Bitmap images"))
    {
      DimensionsDialog dDialog(this);
      if (dDialog.exec())
      {
        m_glwidget->renderToFile(fileName, selectedFilter, dDialog.resultWidth(), dDialog.resultHeight());
      }
    }
    else
    {
      m_glwidget->renderToFile(fileName, selectedFilter);
    }
  }

}

void MainWindow::onImportXML()
{
  QString fileName(QFileDialog::getOpenFileName(this, tr("Open file"), QString(),
                                                tr("XML Graph (*.xml)")));

  if (!fileName.isNull())
  {
    m_layout->ui()->setActive(false);
    m_glwidget->resetViewpoint(0);
    m_graph.loadXML(fileName);
    m_glwidget->rebuild();
    m_information->update();
  }

}

void MainWindow::onExportXML()
{
  QString fileName(QFileDialog::getSaveFileName(this, tr("Save file"), QString(),
                                                tr("XML Graph (*.xml)")));

  if (!fileName.isNull())
  {
    m_graph.saveXML(fileName);
  }
}
