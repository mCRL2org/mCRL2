// Author(s): A.J. (Hannes) Pretorius, Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "mainwindow.h"
#include "mcrl2/lts/lts_io.h"
#include <QTableWidgetItem>
#include <QMessageBox>


MainWindow::MainWindow():
  m_settingsDialog(new SettingsDialog(this, &m_settings)),
  m_graph(NULL), m_diagramEditor(NULL), m_examiner(NULL), m_arcDiagram(NULL), m_simulator(NULL), m_timeSeries(NULL)
{
  m_ui.setupUi(this);

  m_ui.attributes->resizeColumnsToContents();
  m_ui.domain->resizeColumnsToContents();

  connect(m_ui.actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
  connect(m_ui.actionSave, SIGNAL(triggered()), this, SLOT(saveFile()));
  connect(m_ui.actionSaveAs, SIGNAL(triggered()), this, SLOT(saveFileAs()));

  connect(m_ui.actionOpenAttributes, SIGNAL(triggered()), this, SLOT(openDgcFile()));
  connect(m_ui.actionSaveAttributes, SIGNAL(triggered()), this, SLOT(saveDgcFile()));

  connect(m_ui.actionOpenDiagram, SIGNAL(triggered()), this, SLOT(openDgdFile()));
  connect(m_ui.actionSaveDiagram, SIGNAL(triggered()), this, SLOT(saveDgdFile()));

  connect(m_ui.actionQuit, SIGNAL(triggered()), QCoreApplication::instance(), SLOT(quit()));

  connect(m_ui.actionSettingsGeneral, SIGNAL(triggered()), m_settingsDialog, SLOT(showGeneral()));
  connect(m_ui.actionSettingsArcDiagram, SIGNAL(triggered()), m_settingsDialog, SLOT(showArcDiagram()));
}

static void stretch(QWidget *widget)
{
  widget->parentWidget()->layout()->addWidget(widget);
}

void MainWindow::refreshWidgets(Graph* newGraph)
{
  QList<QWidget *> oldWidgets = QList<QWidget *>()
      << m_diagramEditor
      << m_examiner
      << m_arcDiagram
      << m_simulator
      << m_timeSeries;

  for (int i = 0; i < oldWidgets.size(); i++)
  {
    delete oldWidgets[i];
  }
  oldWidgets.clear();

  for (QSet<QDialog *>::iterator i = m_plots.begin(); i != m_plots.end(); i++)
  {
    delete *i;
  }
  m_plots.clear();

  if (newGraph != 0)
  {
    delete m_graph;
    m_graph = newGraph;

    m_ui.numberOfStates->setText(QString::number(m_graph->getSizeNodes()));
    m_ui.numberOfTransitions->setText(QString::number(m_graph->getSizeEdges()));
  }

  m_diagramEditor = new DiagramEditor(m_ui.diagramEditorWidget, this, m_graph);
  stretch(m_diagramEditor);

  m_examiner = new Examiner(m_ui.examinerWidget, this, &m_settings, m_graph);
  m_examiner->setDiagram(m_diagramEditor->getDiagram());
  stretch(m_examiner);

  m_arcDiagram = new ArcDiagram(m_ui.arcDiagramWidget, this, &m_settings, m_graph);
  m_arcDiagram->setDiagram(m_diagramEditor->getDiagram());
  stretch(m_arcDiagram);

  m_simulator = new Simulator(m_ui.simulatorWidget, this, &m_settings, m_graph);
  m_simulator->setDiagram(m_diagramEditor->getDiagram());
  stretch(m_simulator);

  m_timeSeries = new TimeSeries(m_ui.traceWidget, this, &m_settings, m_graph);
  m_timeSeries->setDiagram(m_diagramEditor->getDiagram());
  stretch(m_timeSeries);

  refreshAttributes();
  refreshDomain();
}

void MainWindow::refreshAttributes()
{
  m_ui.attributes->setRowCount(0);

  for (size_t i = 0; i < m_graph->getSizeAttributes(); i++)
  {
    m_ui.attributes->insertRow(i);
    for (int j = 0; j < m_ui.attributes->columnCount(); j++)
    {
      m_ui.attributes->setItem(i, j, new QTableWidgetItem());
      m_ui.attributes->item(i, j)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled);
    }

    Attribute *attribute = m_graph->getAttribute(i);
    m_ui.attributes->item(i, 0)->setText(QString::number(i));
    m_ui.attributes->item(i, 1)->setText(QString::fromStdString(attribute->getName()));
    m_ui.attributes->item(i, 2)->setText(QString::fromStdString(attribute->getType()));
    m_ui.attributes->item(i, 3)->setText(QString::number(attribute->getSizeCurValues()));
  }

  m_ui.attributes->resizeColumnsToContents();
}

void MainWindow::refreshDomain()
{
  m_ui.domain->setRowCount(0);

  // ...
}

void MainWindow::open(QString filename)
{
  Graph *graph = new Graph(this);
  graph->setFileName(filename.toStdString());
  try
  {
    QProgressDialog dialog(QString("Opening ") + filename, QString(), 0, 1, this);
    connect(&m_parser, SIGNAL(started(int)), &dialog, SLOT(setMaximum(int)));
    connect(&m_parser, SIGNAL(progressed(int)), &dialog, SLOT(setValue(int)));
    dialog.show();

    m_parser.parseFile(filename, graph);

    graph->initGraph();
  }
  catch (const mcrl2::runtime_error& e)
  {
    QMessageBox::critical(this, "Error", QString::fromStdString(e.what()));
    delete graph;
    return;
  }
  refreshWidgets(graph);
}

void MainWindow::save(QString filename)
{
  try
  {
    QProgressDialog dialog(QString("Saving ") + filename, QString(), 0, 1, this);
    connect(&m_parser, SIGNAL(started(int)), &dialog, SLOT(setMaximum(int)));
    connect(&m_parser, SIGNAL(progressed(int)), &dialog, SLOT(setValue(int)));
    dialog.show();

    m_parser.writeFSMFile(filename, m_graph);

    m_graph->setFileName(filename.toStdString());
  }
  catch (const mcrl2::runtime_error& e)
  {
    QMessageBox::critical(this, "Error", QString::fromStdString(e.what()));
    return;
  }
}

void MainWindow::openFile()
{
  QString filter = QString("All supported files (") + QString::fromStdString(mcrl2::lts::detail::lts_extensions_as_string(" ")) + ");;All files (*.*)";
  QString filename = QFileDialog::getOpenFileName(this, "Open LTS", QString(), filter);
  if (filename.isNull())
  {
    return;
  }

  open(filename);
}

void MainWindow::saveFile()
{
  if (m_graph == NULL)
  {
    return;
  }
  QString filter = QString("FSM files (*.fsm);;All files (*.*)");
  QString filename = QString::fromStdString(m_graph->getFileName());

  if (filename.isNull() || QFileInfo(filename).suffix().toLower() != "fsm")
  {
    filename = QFileDialog::getSaveFileName(this, "Save LTS", QString(), filter);
  }
  if (filename.isNull())
  {
    return;
  }
  save(filename);
}

void MainWindow::saveFileAs()
{
  if (m_graph == NULL)
  {
    return;
  }
  QString filter = QString("FSM files (*.fsm);;All files (*.*)");
  QString filename = QFileDialog::getSaveFileName(this, "Save LTS", QString(), filter);

  if (filename.isNull())
  {
    return;
  }
  save(filename);
}

void MainWindow::openDgcFile()
{
  if (m_graph == NULL)
  {
    return;
  }
  QString filter = QString("DGC files (*.dgc);;All files (*.*)");
  QString filename = QFileDialog::getOpenFileName(this, "Open Attribute Configuration", QString(), filter);
  if (filename.isNull())
  {
    return;
  }


  try
  {

    std::map< size_t , size_t > attrIdxFrTo;
    std::map< size_t , std::vector< std::string > > attrCurDomains;
    std::map< size_t , std::map< size_t, size_t  > > attrOrigToCurDomains;

    m_parser.parseAttrConfig(
          filename,
          m_graph,
          attrIdxFrTo,
          attrCurDomains,
          attrOrigToCurDomains);

    m_graph->configAttributes(
          attrIdxFrTo,
          attrCurDomains,
          attrOrigToCurDomains);

    refreshWidgets();
  }
  catch (const mcrl2::runtime_error& e)
  {
    QMessageBox::critical(this, "Error", QString::fromStdString(e.what()));
    return;
  }
}

void MainWindow::saveDgcFile()
{
  if (m_graph == NULL)
  {
    return;
  }
  QString filter = QString("DGC files (*.dgc);;All files (*.*)");
  QString filename = QFileDialog::getSaveFileName(this, "Save Attribute Configuration", QString(), filter);

  if (filename.isNull())
  {
    return;
  }

  try
  {
    m_parser.writeAttrConfig(
          filename,
          m_graph);
  }
  catch (const mcrl2::runtime_error& e)
  {
    QMessageBox::critical(this, "Error", QString::fromStdString(e.what()));
    return;
  }
}

void MainWindow::openDgdFile()
{
  if (m_graph == NULL)
  {
    return;
  }
  QString filter = QString("DGD files (*.dgd);;All files (*.*)");
  QString filename = QFileDialog::getOpenFileName(this, "Open Diagram", QString(), filter);
  if (filename.isNull())
  {
    return;
  }

  Diagram* dgrmNew = new Diagram(this);

  try
  {
    m_parser.parseDiagram(
          filename,
          m_graph,
          dgrmNew);

    m_diagramEditor->setDiagram(dgrmNew);

    m_arcDiagram->setDiagram(dgrmNew);
    m_arcDiagram->hideAllDiagrams();

    m_simulator->clearData();
    m_simulator->setDiagram(dgrmNew);

    m_timeSeries->clearData();
    m_timeSeries->setDiagram(dgrmNew);

    m_examiner->clearData();
    m_examiner->setDiagram(dgrmNew);

    if (m_diagramEditor->isVisible()) // Edit Mode
    {
      m_diagramEditor->updateGL();
    }

    if (m_arcDiagram->isVisible())
    {
      m_examiner->updateGL();
      m_arcDiagram->updateGL();
      m_simulator->updateGL();
      m_timeSeries->updateGL();
    }
  }
  catch (const mcrl2::runtime_error& e)
  {
    delete dgrmNew;
    dgrmNew = NULL;

    QMessageBox::critical(this, "Error", QString::fromStdString(e.what()));
    return;
  }

  dgrmNew = NULL;
}

void MainWindow::saveDgdFile()
{
  if (m_graph == NULL)
  {
    return;
  }
  QString filter = QString("DGD files (*.dgd);;All files (*.*)");
  QString filename = QFileDialog::getSaveFileName(this, "Save Diagram", QString(), filter);

  if (filename.isNull())
  {
    return;
  }

  try
  {
    m_parser.writeDiagram(
      filename,
      m_graph,
      m_diagramEditor->getDiagram());
  }
  catch (const mcrl2::runtime_error& e)
  {
    QMessageBox::critical(this, "Error", QString::fromStdString(e.what()));
    return;
  }
}
