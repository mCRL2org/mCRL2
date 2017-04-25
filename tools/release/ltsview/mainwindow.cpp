// Author(s): Bas Ploeger, Carst Tankink, Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <QActionGroup>
#include <QApplication>
#include <QImageWriter>
#include <QMessageBox>
#include <QPixmap>
#include <QString>
#include <QSettings>
#include <gl2ps.h>

#include "mcrl2/lts/lts_io.h"
#include "mainwindow.h"
#include "savepicturedialog.h"
#include "savevectordialog.h"

MainWindow::MainWindow(QThread *atermThread):
  m_fileDialog("", this)
{
  m_ui.setupUi(this);

  m_ltsManager = new LtsManager(this, &m_settings, atermThread);
  m_markManager = new MarkManager(this, m_ltsManager);

  m_infoDock = new InfoDock(this, m_ltsManager, m_markManager);
  m_markDock = new MarkDock(this, m_markManager);
  m_simDock = new SimDock(this, m_ltsManager);
  m_settingsDock = new SettingsDock(this, &m_settings);
  m_settingsDialog = new SettingsDialog(this, &m_settings);
  m_ltsCanvas = new LtsCanvas(this, &m_settings, m_ltsManager, m_markManager);
  setCentralWidget(m_ltsCanvas);
  m_progressDialog = new QProgressDialog("", QString(), 0, 6, this);
  m_progressDialog->setMinimumDuration(0);

  m_ui.informationDock->setWidget(m_infoDock);
  m_ui.simulationDock->setWidget(m_simDock);
  m_ui.markDock->setWidget(m_markDock);
  m_ui.settingsDock->setWidget(m_settingsDock);

  m_ui.viewMenu->insertAction(m_ui.preferences, m_ui.informationDock->toggleViewAction());
  m_ui.viewMenu->insertAction(m_ui.preferences, m_ui.simulationDock->toggleViewAction());
  m_ui.viewMenu->insertAction(m_ui.preferences, m_ui.markDock->toggleViewAction());
  m_ui.viewMenu->insertAction(m_ui.preferences, m_ui.settingsDock->toggleViewAction());
  m_ui.viewMenu->insertSeparator(m_ui.preferences);

  mcrl2::log::logger::register_output_policy(m_logRelay);
  connect(&m_logRelay, SIGNAL(logMessage(QString, QString, QDateTime, QString)), this, SLOT(logMessage(QString, QString, QDateTime, QString)));

  connect(m_ui.open, SIGNAL(triggered()), this, SLOT(open()));
  connect(m_ui.openTrace, SIGNAL(triggered()), this, SLOT(openTrace()));
  connect(m_ui.exportBitmap, SIGNAL(triggered()), this, SLOT(exportBitmap()));
  connect(m_ui.exportText, SIGNAL(triggered()), this, SLOT(exportText()));
  connect(m_ui.exportVector, SIGNAL(triggered()), this, SLOT(exportVector()));
  connect(m_ui.exit, SIGNAL(triggered()), QApplication::instance(), SLOT(quit()));

  connect(m_ui.resetViewpoint, SIGNAL(triggered()), m_ltsCanvas, SLOT(resetView()));
  connect(m_ui.zoomIntoAbove, SIGNAL(triggered()), m_ltsManager, SLOT(zoomInAbove()));
  connect(m_ui.zoomIntoBelow, SIGNAL(triggered()), m_ltsManager, SLOT(zoomInBelow()));
  connect(m_ui.zoomOut, SIGNAL(triggered()), m_ltsManager, SLOT(zoomOut()));

  connect(m_ui.displayStates, SIGNAL(triggered(bool)), &m_settings.displayStates, SLOT(setValue(bool)));
  connect(&m_settings.displayStates, SIGNAL(changed(bool)), m_ui.displayStates, SLOT(setChecked(bool)));
  connect(m_ui.displayTransitions, SIGNAL(triggered(bool)), &m_settings.displayTransitions, SLOT(setValue(bool)));
  connect(&m_settings.displayTransitions, SIGNAL(changed(bool)), m_ui.displayTransitions, SLOT(setChecked(bool)));
  connect(m_ui.displayBackpointers, SIGNAL(triggered(bool)), &m_settings.displayBackpointers, SLOT(setValue(bool)));
  connect(&m_settings.displayBackpointers, SIGNAL(changed(bool)), m_ui.displayBackpointers, SLOT(setChecked(bool)));
  connect(m_ui.displayWireframe, SIGNAL(triggered(bool)), &m_settings.displayWireframe, SLOT(setValue(bool)));
  connect(&m_settings.displayWireframe, SIGNAL(changed(bool)), m_ui.displayWireframe, SLOT(setChecked(bool)));

  connect(m_ui.preferences, SIGNAL(triggered()), m_settingsDialog, SLOT(show()));

  connect(m_ltsManager, SIGNAL(loadingLts()), this, SLOT(loadingLts()));
  connect(m_ltsManager, SIGNAL(rankingStates()), this, SLOT(rankingStates()));
  connect(m_ltsManager, SIGNAL(clusteringStates()), this, SLOT(clusteringStates()));
  connect(m_ltsManager, SIGNAL(computingClusterInfo()), this, SLOT(computingClusterInfo()));
  connect(m_ltsManager, SIGNAL(positioningClusters()), this, SLOT(positioningClusters()));
  connect(m_ltsManager, SIGNAL(positioningStates()), this, SLOT(positioningStates()));
  connect(m_ltsManager, SIGNAL(ltsStructured()), this, SLOT(hideProgressDialog()));
  connect(m_ltsManager, SIGNAL(errorLoadingLts()), this, SLOT(hideProgressDialog()));
  connect(m_ltsManager, SIGNAL(startStructuring()), this, SLOT(startStructuring()));
  connect(m_ltsManager, SIGNAL(stopStructuring()), this, SLOT(stopStructuring()));

  connect(m_ltsManager, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
  connect(m_ltsManager, SIGNAL(ltsZoomed(LTS *)), this, SLOT(zoomChanged()));
  m_ui.zoomIntoAbove->setEnabled(false);
  m_ui.zoomIntoBelow->setEnabled(false);
  m_ui.zoomOut->setEnabled(false);

  connect(m_ltsCanvas, SIGNAL(renderingStarted()), this, SLOT(startRendering()));
  connect(m_ltsCanvas, SIGNAL(renderingFinished()), this, SLOT(clearStatusBar()));

  QSettings settings("mCRL2", "LTSView");
  restoreGeometry(settings.value("geometry").toByteArray());
  restoreState(settings.value("windowState").toByteArray());
}

MainWindow::~MainWindow()
{
  mcrl2::log::logger::unregister_output_policy(m_logRelay);
}

void MainWindow::closeEvent(QCloseEvent */*event*/)
{
  {
    QSettings settings("mCRL2", "LTSView");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
  }

  // It's possible we're in the middle of an LTS restructuring operation that can't be stopped in any friendly way, so hard-terminate the program.
  _exit(0);
}

void MainWindow::open(QString filename)
{
  loadingLts();
  m_progressDialog->setWindowTitle("Opening file");

  if (m_ltsManager->openLts(filename))
  {
    setWindowTitle(filename + " - LTSView");
  }
}

void MainWindow::open()
{
  QString filter = QString("All supported files (") + QString::fromStdString(mcrl2::lts::detail::lts_extensions_as_string(" ")) + ");;All files (*.*)";
  QString filename = m_fileDialog.getOpenFileName("Open LTS", filter);
  if (filename.isNull())
  {
    return;
  }

  open(filename);
}

void MainWindow::openTrace()
{
  if (!m_ltsManager->lts())
  {
    return;
  }

  QString filename = m_fileDialog.getOpenFileName("Open Trace", "Traces (*.trc);;All files (*.*)");
  if (filename.isNull())
  {
    return;
  }

  m_ltsManager->loadTrace(filename);
}

void MainWindow::exportBitmap()
{
  QString filter = "Images (";
  QList<QByteArray> formats = QImageWriter::supportedImageFormats();
  for(QList<QByteArray>::iterator i = formats.begin(); i != formats.end(); i++)
  {
    if(i != formats.begin())
    {
      filter += " ";
    }
    filter += "*." + *i;
  }
  filter += ")";

  QString filename = m_fileDialog.getSaveFileName("Save picture", filter);
  if (filename.isNull())
  {
    return;
  }

  SavePictureDialog dialog(this, m_ltsCanvas, filename);
  connect(&dialog, SIGNAL(statusMessage(QString)), this, SLOT(setStatusBar(QString)));
  dialog.exec();
  clearStatusBar();
}

void MainWindow::exportText()
{
  QString filename = m_fileDialog.getSaveFileName("Save text", "All files (*.*)");
  if (filename.isNull())
  {
    return;
  }
  m_ltsCanvas->exportToText(filename);
}

void MainWindow::exportVector()
{
  QString all = "All supported files (*.ps *.eps *.pdf *.svg)";
  QString ps = "PostScript (*.ps)";
  QString eps = "Encapsulated PostScript (*.eps)";
  QString pdf = "Portable Document Format (*.pdf)";
  QString svg = "Scalable Vector Graphics (*.svg)";

  QString filter = all + ";;" + ps + ";;" + eps + ";;" + pdf + ";;" + svg;
  QString selectedFilter = all;
  QString filename = m_fileDialog.getSaveFileName("Save vector", filter, &selectedFilter);
  if (filename.isNull())
  {
    return;
  }

  GLint format;
  if (selectedFilter == ps || (selectedFilter == all && filename.right(3).toLower() == ".ps"))
  {
    format = GL2PS_PS;
  }
  else if (selectedFilter == eps || (selectedFilter == all && filename.right(4).toLower() == ".eps"))
  {
    format = GL2PS_EPS;
  }
  else if (selectedFilter == pdf || (selectedFilter == all && filename.right(4).toLower() == ".pdf"))
  {
    format = GL2PS_PDF;
  }
  else if (selectedFilter == svg || (selectedFilter == all && filename.right(4).toLower() == ".svg"))
  {
    format = GL2PS_SVG;
  }
  else
  {
    QMessageBox::critical(this, "Error writing file", "Saving picture failed: unsupported file format.");
    return;
  }

  SaveVectorDialog dialog(this, m_ltsCanvas, filename, format);
  connect(&dialog, SIGNAL(statusMessage(QString)), this, SLOT(setStatusBar(QString)));
  dialog.exec();
  clearStatusBar();
}

void MainWindow::setProgress(int phase, QString message)
{
  if (!m_progressDialog->isVisible())
  {
    m_progressDialog->setWindowTitle("Structuring LTS");
    m_progressDialog->show();
  }
  m_progressDialog->setLabelText(message);
  m_progressDialog->setValue(phase);
}

void MainWindow::selectionChanged()
{
  m_ui.zoomIntoAbove->setEnabled(m_ltsManager->selectedCluster() != 0);
  m_ui.zoomIntoBelow->setEnabled(m_ltsManager->selectedCluster() != 0);
}
