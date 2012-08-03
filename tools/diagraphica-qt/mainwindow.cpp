// Author(s): A.J. (Hannes) Pretorius, Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "mainwindow.h"
#include "parser.h"
#include "mcrl2/lts/lts_io.h"
#include <QTableWidgetItem>


MainWindow::MainWindow():
  m_settingsDialog(new SettingsDialog(this, &m_settings)),
  m_graph(0)
{
  m_ui.setupUi(this);

  m_ui.attributes->resizeColumnsToContents();
  m_ui.domain->resizeColumnsToContents();

  connect(m_ui.actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
  connect(m_ui.actionSave, SIGNAL(triggered()), this, SLOT(saveFile()));
  connect(m_ui.actionQuit, SIGNAL(triggered()), QCoreApplication::instance(), SLOT(quit()));

  connect(m_ui.actionSettingsGeneral, SIGNAL(triggered()), m_settingsDialog, SLOT(showGeneral()));
  connect(m_ui.actionSettingsArcDiagram, SIGNAL(triggered()), m_settingsDialog, SLOT(showArcDiagram()));
}

static void stretch(QWidget *widget)
{
  QWidget *parent = widget->parentWidget();
  QBoxLayout *layout = new QHBoxLayout(parent);
  layout->addWidget(widget);
}

void MainWindow::open(QString filename)
{
  Graph *graph = new Graph(this);
  graph->setFileName(filename.toStdString());
  try
  {
    Parser parser;
    QProgressDialog dialog(QString("Opening ") + filename, QString(), 0, 1, this);
    connect(&parser, SIGNAL(started(int)), &dialog, SLOT(setMaximum(int)));
    connect(&parser, SIGNAL(progressed(int)), &dialog, SLOT(setValue(int)));
    dialog.show();

    parser.parseFile(filename.toStdString(), graph);

    graph->initGraph();
  }
  catch (const mcrl2::runtime_error& e)
  {
    delete graph;
    return;
  }

  QList<QWidget *> containers = QList<QWidget *>()
    << m_ui.examinerWidget
    << m_ui.arcDiagramWidget
    << m_ui.simulatorWidget
    << m_ui.traceWidget
    << m_ui.diagramEditorWidget;
  for (int i = 0; i < containers.size(); i++)
  {
    QObjectList children = containers[i]->children();
    for (int j = 0; j < children.size(); j++)
    {
      if (children[j]->isWidgetType())
      {
        delete children[j];
      }
    }
  }

  for (QSet<QDialog *>::iterator i = m_plots.begin(); i != m_plots.end(); i++)
  {
    delete *i;
  }
  m_plots.clear();

  m_ui.domain->clearContents();
  m_ui.attributes->clearContents();

  delete m_graph;
  m_graph = graph;

  m_ui.numberOfStates->setText(QString::number(graph->getSizeNodes()));
  m_ui.numberOfTransitions->setText(QString::number(graph->getSizeEdges()));

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
  QString filter = QString("FSM files (*.fsm);;All files (*.*)");
  QString filename = QFileDialog::getSaveFileName(this, "Save LTS", QString(), filter);
  if (filename.isNull())
  {
    return;
  }

}
