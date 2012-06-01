// Author(s): Bas Ploeger, Carst Tankink, Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <QActionGroup>
#include <QApplication>
#include <QFileDialog>
#include <QImageWriter>
#include <QMessageBox>
#include <QPixmap>
#include <QString>
#include <gl2ps.h>

#include "mcrl2/lts/lts_io.h"
#include "mainwindow.h"
#include "savepicturedialog.h"
#include "savevectordialog.h"

#include "icons/main_window.xpm"

MainWindow::MainWindow():
  m_messenger(this)
{
  m_ui.setupUi(this);

  setWindowIcon(QPixmap(main_window));

  m_ltsManager = new LtsManager(this, &m_settings);
  m_markManager = new MarkManager(this, m_ltsManager);

  m_settingsDialog = new SettingsDialog(this, &m_settings);
  m_infoDialog = new InfoDialog(this, m_ltsManager, m_markManager);
  m_markDialog = new MarkDialog(this, m_markManager);
  m_simDialog = new SimDialog(this, m_ltsManager);
  m_ltsCanvas = new LtsCanvas(this, &m_settings, m_ltsManager, m_markManager);
  setCentralWidget(m_ltsCanvas);
  m_progressDialog = new QProgressDialog("", QString(), 0, 6, this);
  m_progressDialog->setWindowModality(Qt::WindowModal);
  m_progressDialog->setMinimumDuration(0);

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
  connect(m_ui.settings, SIGNAL(triggered()), m_settingsDialog, SLOT(show()));

  connect(m_ui.select, SIGNAL(triggered()), m_ltsCanvas, SLOT(useSelect()));
  connect(m_ui.pan, SIGNAL(triggered()), m_ltsCanvas, SLOT(usePan()));
  connect(m_ui.zoom, SIGNAL(triggered()), m_ltsCanvas, SLOT(useZoom()));
  connect(m_ui.rotate, SIGNAL(triggered()), m_ltsCanvas, SLOT(useRotate()));
  connect(m_ui.information, SIGNAL(triggered()), m_infoDialog, SLOT(show()));
  connect(m_ui.simulation, SIGNAL(triggered()), m_simDialog, SLOT(show()));
  connect(m_ui.mark, SIGNAL(triggered()), m_markDialog, SLOT(show()));

  connect(m_ltsManager, SIGNAL(loadingLts()), this, SLOT(loadingLts()));
  connect(m_ltsManager, SIGNAL(rankingStates()), this, SLOT(rankingStates()));
  connect(m_ltsManager, SIGNAL(clusteringStates()), this, SLOT(clusteringStates()));
  connect(m_ltsManager, SIGNAL(computingClusterInfo()), this, SLOT(computingClusterInfo()));
  connect(m_ltsManager, SIGNAL(positioningClusters()), this, SLOT(positioningClusters()));
  connect(m_ltsManager, SIGNAL(positioningStates()), this, SLOT(positioningStates()));
  connect(m_ltsManager, SIGNAL(ltsStructured()), this, SLOT(hideProgressDialog()));
  connect(m_ltsManager, SIGNAL(errorLoadingLts()), this, SLOT(hideProgressDialog()));

  connect(m_ltsCanvas, SIGNAL(renderingStarted()), this, SLOT(startRendering()));
  connect(m_ltsCanvas, SIGNAL(renderingFinished()), this, SLOT(clearStatusBar()));

  QActionGroup *toolGroup = new QActionGroup(this);
  toolGroup->addAction(m_ui.select);
  toolGroup->addAction(m_ui.pan);
  toolGroup->addAction(m_ui.zoom);
  toolGroup->addAction(m_ui.rotate);
  m_ui.select->setChecked(true);
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
  QString filename = QFileDialog::getOpenFileName(this, "Open LTS", QString(), filter);
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

  QString filename = QFileDialog::getOpenFileName(this, "Open Trace", QString(), "Traces (*.trc);;All files (*.*)");
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

  QString filename = QFileDialog::getSaveFileName(this, "Save picture", QString(), filter);
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
  QString filename = QFileDialog::getSaveFileName(this, "Save text", QString(), "All files (*.*)");
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
  QString selectedFilter;
  QString filename = QFileDialog::getSaveFileName(this, "Save vector", QString(), filter, &selectedFilter);
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
