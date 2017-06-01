// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mainwindow.h"

#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/utilities/exception.h"

#include <QSettings>
#include <utility>

#include "dimensionsdialog.h"
#include "glwidget.h"
#include "information.h"
#include "springlayout.h"

#define MAX_NODE_COUNT 400

MainWindow::MainWindow(QWidget* parent) :
  QMainWindow(parent),
  m_fileDialog("", this)
{

  m_ui.setupUi(this);
  m_ui.dockOutput->setVisible(false);

  // Add graph area
  m_glwidget = new GLWidget(m_graph, m_ui.frame);
  m_glwidget->setDepth(0, 0);
  m_ui.widgetLayout->addWidget(m_glwidget);

  // Create springlayout algorithm + UI
  m_layout = new Graph::SpringLayout(m_graph, *m_glwidget);
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

  // Connect signals & slots
  connect(m_glwidget, SIGNAL(widgetResized(const QVector3D&)), this, SLOT(onWidgetResized(const QVector3D&)));
  connect(m_ui.actExit, SIGNAL(triggered()), this, SLOT(onExit()));
  connect(m_ui.actLayoutControl, SIGNAL(toggled(bool)), springlayoutui, SLOT(setVisible(bool)));
  connect(m_ui.actVisualization, SIGNAL(toggled(bool)), glwidgetui, SLOT(setVisible(bool)));
  connect(m_ui.actInformation, SIGNAL(toggled(bool)), informationui, SLOT(setVisible(bool)));
  connect(m_ui.actOutput, SIGNAL(toggled(bool)), m_ui.dockOutput, SLOT(setVisible(bool)));
  connect(m_ui.act3D, SIGNAL(toggled(bool)), this, SLOT(on3DChanged(bool)));
  connect(m_ui.actExplorationMode, SIGNAL(toggled(bool)), this, SLOT(onExplore(bool)));
  connect(m_ui.actLayout, SIGNAL(toggled(bool)), springlayoutui, SLOT(setActive(bool)));
  connect(m_ui.actReset, SIGNAL(triggered()), m_glwidget, SLOT(resetViewpoint()));
  connect(m_ui.actOpenFile, SIGNAL(triggered()), this, SLOT(onOpenFile()));
  connect(m_ui.actExportImage, SIGNAL(triggered()), this, SLOT(onExportImage()));
  connect(m_ui.actImport_XML, SIGNAL(triggered()), this, SLOT(onImportXML()));
  connect(m_ui.actExport_XML, SIGNAL(triggered()), this, SLOT(onExportXML()));
  connect(m_ui.dockWidgetOutput, SIGNAL(logMessage(QString, QString, QDateTime, QString, QString)), this, SLOT(onLogOutput(QString, QString, QDateTime, QString, QString)));

  QSettings settings("mCRL2", "LTSGraph");
  restoreGeometry(settings.value("geometry").toByteArray());
  restoreState(settings.value("windowState").toByteArray());
  springlayoutui->setSettings(settings.value("settings").toByteArray());
  m_ui.actExplorationMode->setChecked(settings.value("explore", 0).toInt() != 0);

  m_ui.actLayoutControl->setChecked(!springlayoutui->isHidden());
  m_ui.actVisualization->setChecked(!glwidgetui->isHidden());
  m_ui.actInformation->setChecked(!informationui->isHidden());
  m_ui.actOutput->setChecked(!m_ui.dockOutput->isHidden());
}

void MainWindow::closeEvent(QCloseEvent* event)
{
  QSettings settings("mCRL2", "LTSGraph");
  settings.setValue("geometry", saveGeometry());
  settings.setValue("windowState", saveState());
  settings.setValue("settings", m_layout->ui()->settings());
  settings.setValue("explore", m_graph.hasSelection() ? 1 : 0);
  QMainWindow::closeEvent(event);
}

void MainWindow::showEvent(QShowEvent* /*event*/)
{
  if (!m_delayedOpen.isEmpty())
  {
    openFile(m_delayedOpen);
    m_delayedOpen = QString();
  }
}

MainWindow::~MainWindow()
{
  delete m_layout;
  delete m_information;
  delete m_glwidget;
}

void MainWindow::onWidgetResized(const QVector3D& newsize)
{
  QVector3D limit{INFINITY, INFINITY, INFINITY};
  if (newsize.z() == 0.0)
  {
    limit.setZ(0.0);
  }
  m_graph.clip(-limit, limit);
  m_layout->setClipRegion(-limit, limit, newsize.length() / 4);
  m_glwidget->update();
}

void MainWindow::on3DChanged(bool enabled)
{
  if (enabled) {
    m_glwidget->setDepth(1000, 25);
  }
  else {
    m_glwidget->setDepth(0, 80);
  }
}

void MainWindow::onExplore(bool enabled)
{
  if (enabled)
  {
    m_graph.makeSelection();
    m_graph.toggleActive(m_graph.initialState());
  }
  else {
    m_graph.discardSelection();
  }
  m_glwidget->update();
}

void MainWindow::onLogOutput(const QString& /*level*/, const QString& /*hint*/, const QDateTime& /*timestamp*/, const QString& /*message*/, const QString& formattedMessage)
{
  m_ui.statusBar->showMessage(formattedMessage, 5000);
}

void MainWindow::delayedOpenFile(QString fileName)
{
  m_delayedOpen = std::move(fileName);
}

void MainWindow::openFile(const QString& fileName)
{
  if (!fileName.isNull())
  {
    try
    {
      bool hadSelection = m_graph.hasSelection();
      m_ui.actLayout->setChecked(false);
      m_glwidget->pause();
      m_glwidget->resetViewpoint(0);
      const double scaling = 0.7;
      QVector3D limit = m_glwidget->size3() / 2.0 * scaling;
      m_graph.load(fileName, -limit, limit,
          m_glwidget->nodeSize());

      if (m_graph.nodeCount() > MAX_NODE_COUNT && !hadSelection)
      {
        if (QMessageBox::question(this, "Exploration mode",
                                  tr("The selected LTS has a large number of states; "
                                     "this may cause LTS Graph to perform poorly or become unresponsive. "
                                     "Do you want to enable exploration mode?"),
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
        {
          m_ui.actExplorationMode->setChecked(true);
        }
      }
      else
      {
        onExplore(hadSelection);
      }

      m_glwidget->rebuild();
      m_glwidget->resume();
      m_information->update();
      setWindowTitle(QString("LTSGraph - ") + fileName);
      m_graph.setStable(false);
    }
    catch (mcrl2::runtime_error e)
    {
      QMessageBox::critical(this, "Error opening file", e.what());
      mCRL2log(mcrl2::log::error) << "Error opening file: " << e.what() << std::endl;
      setWindowTitle(QString("LTSGraph"));
    }
  }
}

void MainWindow::onOpenFile()
{

  QString fileName(m_fileDialog.getOpenFileName(tr("Open file"),
                   tr("Labelled transition systems (*.lts *.aut *.fsm *.dot)")));

  openFile(fileName);
}

void MainWindow::onExportImage()
{
  QString bitmap = tr("Bitmap images (*.png *.jpg *.jpeg *.gif *.bmp *.pbm *.pgm *.ppm *.xbm *.xpm)");
  QString vector = tr("Vector format (*.pdf *.ps *.eps *.svg *.pgf)");
  QString tikz = tr("LaTeX TikZ Image (*.tex)");

  QString filter = bitmap + ";;" + vector + ";;" + tikz;
  QString selectedFilter = bitmap;
  QString fileName(m_fileDialog.getSaveFileName(tr("Save file"),
                   filter,
                   &selectedFilter));

  if (!fileName.isNull())
  {
    if (selectedFilter == bitmap)
    {
      m_glwidget->saveBitmap(fileName);
    }
    else if (selectedFilter == vector)
    {
      m_glwidget->saveVector(fileName);
    }
    else
    {
      m_glwidget->saveTikz(fileName, m_glwidget->width() / m_glwidget->height());
    }
  }

}

void MainWindow::onImportXML()
{
  QString fileName(m_fileDialog.getOpenFileName(tr("Open file"),
                   tr("XML Graph (*.xml)")));

  if (!fileName.isNull())
  {
    bool hadSelection = m_graph.hasSelection();
    m_layout->ui()->setActive(false);
    m_glwidget->resetViewpoint(0);
    m_graph.loadXML(fileName);
    onExplore(hadSelection);
    m_glwidget->rebuild();
    m_information->update();
    m_graph.setStable(false);
  }

}

void MainWindow::onExportXML()
{
  QString fileName(m_fileDialog.getSaveFileName(tr("Save file"),
                   tr("XML Graph (*.xml)")));

  if (!fileName.isNull())
  {
    m_graph.saveXML(fileName);
  }
}

void MainWindow::onExit()
{
  if (m_layout && m_layout->ui())
  {
    m_layout->ui()->setActive(false);
  }
  QApplication::instance()->quit();
}
