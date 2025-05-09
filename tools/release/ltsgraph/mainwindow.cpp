// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mainwindow.h"

#include "mcrl2/lts/lts_lts.h"

#include <QMessageBox>
#include <QSettings>

#include "dimensionsdialog.h"
#include "springlayout.h"

/// \brief The number of vertices before the user is prompted to enable exploration mode.
constexpr std::size_t MAX_NODE_COUNT    = 400;


MainWindow::MainWindow(QWidget* parent) :
  QMainWindow(parent),
  m_fileDialog("", this)
{
  m_ui.setupUi(this);
  m_ui.dockOutput->setVisible(false);

  // Add graph area

  m_glwidget = new GLWidget(m_graph, m_ui.frame);
  m_ui.widgetLayout->addWidget(m_glwidget);

  // Create springlayout algorithm + UI
  m_layout = new Graph::SpringLayout(m_graph, *m_glwidget);
  m_advancedwidget = new Graph::CustomQWidget(m_ui.actionAdvancedSpringlayout, nullptr); // nullptr so it is treated as a separate window
  m_advancedwidget->setWindowFlags(Qt::WindowStaysOnTopHint);
  Graph::SpringLayoutUi* springlayoutui = m_layout->ui(m_ui.actionAdvancedSpringlayout, m_advancedwidget, this);
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
  connect(m_ui.actExit, SIGNAL(triggered()), this, SLOT(close()));
  connect(m_ui.actLayoutControl, SIGNAL(toggled(bool)), springlayoutui, SLOT(setVisible(bool)));
  connect(m_ui.actVisualization, SIGNAL(toggled(bool)), glwidgetui, SLOT(setVisible(bool)));
  connect(m_ui.actInformation, SIGNAL(toggled(bool)), informationui, SLOT(setVisible(bool)));
  connect(m_ui.actionAdvancedSpringlayout, SIGNAL(toggled(bool)),
          springlayoutui, SLOT(onAdvancedDialogShow(bool)));
  connect(m_ui.actOutput, SIGNAL(toggled(bool)), m_ui.dockOutput, SLOT(setVisible(bool)));
  connect(m_ui.act3D, SIGNAL(toggled(bool)), this, SLOT(on3DChanged(bool)));
  connect(m_ui.act3D, SIGNAL(toggled(bool)),
          glwidgetui->m_ui.cbThreeDimensional, SLOT(setChecked(bool)));
  connect(glwidgetui->m_ui.cbThreeDimensional, SIGNAL(toggled(bool)), m_ui.act3D, SLOT(setChecked(bool)));

  connect(m_ui.act3D, SIGNAL(toggled(bool)), this, SLOT(updateStatusBar()));
  connect(m_ui.actExplorationMode, SIGNAL(toggled(bool)), this, SLOT(onExplore(bool)));
  connect(m_ui.actLayout, SIGNAL(toggled(bool)), springlayoutui, SLOT(setActive(bool)));
  connect(m_ui.actReset, SIGNAL(triggered()), m_glwidget, SLOT(resetViewpoint()));
  connect(m_ui.actOpenFile, SIGNAL(triggered()), this, SLOT(onOpenFile()));
  connect(m_ui.actExportImage, SIGNAL(triggered()), this, SLOT(onExportImage()));
  connect(m_ui.actImport_XML, SIGNAL(triggered()), this, SLOT(onImportXML()));
  connect(m_ui.actExport_XML, SIGNAL(triggered()), this, SLOT(onExportXML()));
  connect(m_ui.dockWidgetOutput, SIGNAL(logMessage(QString, QDateTime, QString, QString)), this, SLOT(onLogOutput(QString, QDateTime, QString, QString)));

  QSettings settings("mCRL2", "LTSGraph");
  restoreGeometry(settings.value("geometry").toByteArray());
  restoreState(settings.value("windowState").toByteArray());
  springlayoutui->setSettings(settings.value("settings").toByteArray());
  glwidgetui->setSettings(settings.value("visualisation").toByteArray());
  connect(m_ui.actFullscreen, SIGNAL(triggered()), this, SLOT(onFullscreen()));

  m_ui.actLayoutControl->setChecked(!springlayoutui->isHidden());
  m_ui.actVisualization->setChecked(!glwidgetui->isHidden());
  m_ui.actInformation->setChecked(!informationui->isHidden());
  m_ui.actOutput->setChecked(!m_ui.dockOutput->isHidden());

#ifdef MCRL2_PLATFORM_MAC
  m_ui.actFullscreen->setShortcut(QString("Meta+Ctrl+F"));
#else
  m_ui.actFullscreen->setShortcut(QString("F11"));
#endif
  updateStatusBar();
}

void MainWindow::onFullscreen()
{
  if (isFullScreen())
  {
    showNormal();
  }
  else
  {
    showFullScreen();
  }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
  if (m_layout && m_layout->ui())
  {
    m_layout->ui()->setActive(false);
  }
  QSettings settings("mCRL2", "LTSGraph");
  settings.setValue("geometry", saveGeometry());
  settings.setValue("windowState", saveState());
  settings.setValue("settings", m_layout->ui()->settings());
  settings.setValue("visualisation", m_glwidget->ui()->settings());
  m_advancedwidget->close();
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

void MainWindow::on3DChanged(bool enabled)
{
  m_glwidget->set3D(enabled);
  
  // For 3D mode there is no limit and otherwise the z-dimension is limited to 0.
  QVector3D limit{INFINITY, INFINITY, INFINITY};
  if (!enabled)
  {
    limit.setZ(0.0);
  }
  m_graph.clip(-limit, limit);
  m_graph.setStable(false); // Probably no longer stable

  if(enabled)
  {
    m_graph.scrambleZ() = true;
  }
}

void MainWindow::onExplore(bool enabled)
{
  if (enabled)
  {
    m_graph.makeExploration();
    m_graph.toggleOpen(m_graph.initialState());
  }
  else
  {
    m_graph.discardExploration();
  }

  m_glwidget->update();
}

void MainWindow::onLogOutput(const QString& /*level*/, const QDateTime& /*timestamp*/, const QString& /*message*/, const QString& formattedMessage)
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
      // Indicates that exploration mode was previously enabled
      bool hadExploration = m_graph.hasExploration();

      // Disable layouting and reset viewport.
      m_ui.actLayout->setChecked(false);

      m_glwidget->resetViewpoint(0);
      
      // We limit the initial positions of the nodes.
      QVector3D limit = QVector3D(2500.0, 2500.0f, (m_glwidget->get3D() ? 2500.0f : 0.0f));

      // The argument '-' means we should read from stdin, the lts library
      // does that when supplied an empty string as the input file name
      m_graph.load(fileName == "-" ? "" : fileName, -limit, limit);

      m_layout->resetPositions();
      m_glwidget->rebuild();
      on3DChanged(m_ui.act3D->isChecked());

      if (m_graph.nodeCount() > MAX_NODE_COUNT && !hadExploration)
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
        onExplore(hadExploration);
      }

      m_information->update();
      setWindowTitle(QString("LTSGraph - ") + fileName);
      m_graph.setStable(false);
    }
    catch (const mcrl2::runtime_error& e)
    {
      m_layout->resetPositions();

      m_glwidget->rebuild();
      on3DChanged(m_ui.act3D->isChecked());

      onExplore(false);

      m_information->update();
      m_graph.setStable(false);

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

namespace Export
{
    /** Scalable Vector Graphics vector image format */
    struct SVG;
    /** LaTeX Tikz vector image format */
    struct Tikz;
}

void MainWindow::onExportImage()
{
  QString bitmap = tr("Bitmap images (*.png *.jpg *.jpeg *.gif *.bmp *.pbm *.pgm *.ppm *.xbm *.xpm)");
  QString svg = tr("Scalable Vector Graphics [SVG] image (*.svg)");
  QString tikz = tr("LaTeX TikZ Image (*.tex)");

  QString filter = bitmap + ";;" + svg + ";;" + tikz;
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
    else if (selectedFilter == svg)
    {
      m_glwidget->saveVector<Export::SVG>(fileName);
    }
    else if (selectedFilter == tikz)
    {
      m_glwidget->saveVector<Export::Tikz>(fileName);
    }
  }

}

void MainWindow::onImportXML()
{
  QString fileName(m_fileDialog.getOpenFileName(tr("Open file"),
                   tr("XML Graph (*.xml)")));

  if (!fileName.isNull())
  {
    bool hadExploration = m_graph.hasExploration();
    m_layout->ui()->setActive(false);
    m_glwidget->resetViewpoint(0);
    m_graph.loadXML(fileName);
    onExplore(hadExploration);
    m_glwidget->rebuild();
    on3DChanged(false);
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

void MainWindow::updateStatusBar()
{
  QString ctrlKey = QKeySequence(Qt::ControlModifier).toString(QKeySequence::NativeText);
  QString msg =
    m_glwidget->isPainting() ?
      "Click to paint a node" :
      !m_ui.act3D->isChecked() ?
        ctrlKey + "drag: move camera; scroll: zoom in/out; Esc: reset viewpoint; Right click: fix a node/handle" :
        ctrlKey + "drag: move camera; Shift + drag: rotate camera; scroll: zoom in/out; Esc: reset viewpoint; Right click: fix a node/handle";
  m_ui.statusBar->showMessage(msg);
}
