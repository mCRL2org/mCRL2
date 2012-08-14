// Author(s): A.J. (Hannes) Pretorius, Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "mainwindow.h"
#include "combnplot.h"
#include "corrlplot.h"
#include "distrplot.h"
#include "mcrl2/lts/lts_io.h"
#include <QTableWidgetItem>
#include <QMessageBox>


MainWindow::MainWindow():
  m_settingsDialog(new SettingsDialog(this, &m_settings)),
  m_graph(0),
  m_clusteringProgress("Clustering...", QString(), 0, 1, this),
  m_examiner(0),
  m_arcDiagram(0),
  m_simulator(0),
  m_timeSeries(0),
  m_diagramEditor(0),
  m_routingCluster(0)
{
  m_ui.setupUi(this);

  QActionGroup* groupMode = new QActionGroup(m_ui.menuMode);
  groupMode->addAction(m_ui.actionSimulationMode);
  groupMode->addAction(m_ui.actionTraceMode);
  groupMode->addAction(m_ui.actionEditMode);

  m_ui.attributes->resizeColumnsToContents();
  m_ui.domain->resizeColumnsToContents();

  connect(m_ui.actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
  connect(m_ui.actionSave, SIGNAL(triggered()), this, SLOT(saveFile()));
  connect(m_ui.actionSaveAs, SIGNAL(triggered()), this, SLOT(saveFileAs()));

//  connect(m_ui.actionOpenAttributes, SIGNAL(triggered()), this, SLOT(openAttributeConfiguration()));
//  connect(m_ui.actionSaveAttributes, SIGNAL(triggered()), this, SLOT(saveAttributeConfiguration()));

//  connect(m_ui.actionOpenDiagram, SIGNAL(triggered()), this, SLOT(openDiagram()));
//  connect(m_ui.actionSaveDiagram, SIGNAL(triggered()), this, SLOT(saveDiagram()));

  connect(m_ui.actionQuit, SIGNAL(triggered()), QCoreApplication::instance(), SLOT(quit()));

  connect(groupMode, SIGNAL(triggered(QAction*)), this, SLOT(modeSelected(QAction*)));

  connect(m_ui.actionSettingsGeneral, SIGNAL(triggered()), m_settingsDialog, SLOT(showGeneral()));
  connect(m_ui.actionSettingsArcDiagram, SIGNAL(triggered()), m_settingsDialog, SLOT(showArcDiagram()));

  connect(m_ui.attributes, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showAttributeContextMenu(const QPoint &)));
  m_ui.attributes->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(m_ui.attributes, SIGNAL(itemSelectionChanged()), this, SLOT(updateAttributeOperations()));
  updateAttributeOperations();

  connect(m_ui.actionClusterNodes, SIGNAL(triggered()), this, SLOT(clusterNodes()));
  connect(m_ui.actionViewTrace, SIGNAL(triggered()), this, SLOT(viewTrace()));
  connect(m_ui.actionDistributionPlot, SIGNAL(triggered()), this, SLOT(distributionPlot()));
  connect(m_ui.actionCorrelationPlot, SIGNAL(triggered()), this, SLOT(correlationPlot()));
  connect(m_ui.actionCombinationPlot, SIGNAL(triggered()), this, SLOT(combinationPlot()));
  connect(m_ui.actionDuplicate, SIGNAL(triggered()), this, SLOT(duplicateAttribute()));
  connect(m_ui.actionRenameAttribute, SIGNAL(triggered()), this, SLOT(renameAttribute()));
  connect(m_ui.actionDelete, SIGNAL(triggered()), this, SLOT(deleteAttribute()));

}

static void stretch(QWidget *widget)
{
  widget->parentWidget()->layout()->addWidget(widget);
}

void MainWindow::open(QString filename)
{
  Graph *graph = new Graph;
  graph->setFileName(filename);
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

  if (graph != 0)
  {
    emit closingGraph();
    delete m_graph;
  }

  m_graph = graph;

  connect(m_graph, SIGNAL(startedClusteringNodes(int)), &m_clusteringProgress, SLOT(setMaximum(int)));
  connect(m_graph, SIGNAL(startedClusteringNodes(int)), &m_clusteringProgress, SLOT(show()));
  connect(m_graph, SIGNAL(startedClusteringEdges(int)), &m_clusteringProgress, SLOT(setMaximum(int)));
  connect(m_graph, SIGNAL(startedClusteringEdges(int)), &m_clusteringProgress, SLOT(show()));
  connect(m_graph, SIGNAL(progressedClustering(int)), &m_clusteringProgress, SLOT(setValue(int)));

  m_ui.numberOfStates->setText(QString::number(m_graph->getSizeNodes()));
  m_ui.numberOfTransitions->setText(QString::number(m_graph->getSizeEdges()));

  m_diagramEditor = new DiagramEditor(m_ui.diagramEditorWidget, this, m_graph);
  stretch(m_diagramEditor);
  connect(m_ui.selectTool, SIGNAL(clicked(bool)), m_diagramEditor, SLOT(setSelectMode()));
  connect(m_ui.noteTool, SIGNAL(clicked(bool)), m_diagramEditor, SLOT(setNoteMode()));
  connect(m_ui.configureTool, SIGNAL(clicked(bool)), m_diagramEditor, SLOT(setConfigureMode()));
  connect(m_ui.rectangleTool, SIGNAL(clicked(bool)), m_diagramEditor, SLOT(setRectangleMode()));
  connect(m_ui.ellipseTool, SIGNAL(clicked(bool)), m_diagramEditor, SLOT(setEllipseMode()));
  connect(m_ui.lineTool, SIGNAL(clicked(bool)), m_diagramEditor, SLOT(setLineMode()));
  connect(m_ui.arrowTool, SIGNAL(clicked(bool)), m_diagramEditor, SLOT(setArrowMode()));
  connect(m_ui.doubleArrowTool, SIGNAL(clicked(bool)), m_diagramEditor, SLOT(setDoubleArrowMode()));
  connect(m_ui.colorTool, SIGNAL(clicked(bool)), m_diagramEditor, SLOT(setFillColor()));
  connect(m_ui.lineColorTool, SIGNAL(clicked(bool)), m_diagramEditor, SLOT(setLineColor()));
  connect(m_ui.showGridButton, SIGNAL(toggled(bool)), m_diagramEditor, SLOT(setShowGrid(bool)));
  connect(m_ui.snapToGridButton, SIGNAL(toggled(bool)), m_diagramEditor, SLOT(setSnapGrid(bool)));

  m_examiner = new Examiner(m_ui.examinerWidget, &m_settings, m_graph);
  m_examiner->setDiagram(m_diagramEditor->diagram());
  stretch(m_examiner);
  connect(m_examiner, SIGNAL(routingCluster(Cluster *, QList<Cluster *>, QList<Attribute *>)), this, SLOT(routeCluster(Cluster *, QList<Cluster *>, QList<Attribute *>)));
  connect(m_examiner, SIGNAL(selectionChanged()), this, SLOT(updateArcDiagramMarks()));

  m_arcDiagram = new ArcDiagram(m_ui.arcDiagramWidget, &m_settings, m_graph);
  m_arcDiagram->setDiagram(m_diagramEditor->diagram());
  stretch(m_arcDiagram);
  connect(m_arcDiagram, SIGNAL(routingCluster(Cluster *, QList<Cluster *>, QList<Attribute *>)), this, SLOT(routeCluster(Cluster *, QList<Cluster *>, QList<Attribute *>)));
  connect(m_arcDiagram, SIGNAL(hoverCluster(Cluster *, QList<Attribute *>)), this, SLOT(hoverCluster(Cluster *, QList<Attribute *>)));

  m_simulator = new Simulator(m_ui.simulatorWidget, &m_settings, m_graph);
  m_simulator->setDiagram(m_diagramEditor->diagram());
  stretch(m_simulator);
  connect(m_simulator, SIGNAL(routingCluster(Cluster *, QList<Cluster *>, QList<Attribute *>)), this, SLOT(routeCluster(Cluster *, QList<Cluster *>, QList<Attribute *>)));
  connect(m_simulator, SIGNAL(hoverCluster(Cluster *, QList<Attribute *>)), this, SLOT(hoverCluster(Cluster *, QList<Attribute *>)));
  connect(m_simulator, SIGNAL(hoverCluster(Cluster *, QList<Attribute *>)), this, SLOT(updateArcDiagramMarks()));

  m_timeSeries = new TimeSeries(m_ui.traceWidget, &m_settings, m_graph);
  m_timeSeries->setDiagram(m_diagramEditor->diagram());
  stretch(m_timeSeries);
  connect(m_timeSeries, SIGNAL(routingCluster(Cluster *, QList<Cluster *>, QList<Attribute *>)), this, SLOT(routeCluster(Cluster *, QList<Cluster *>, QList<Attribute *>)));
  connect(m_timeSeries, SIGNAL(hoverCluster(Cluster *, QList<Attribute *>)), this, SLOT(hoverCluster(Cluster *, QList<Attribute *>)));
  connect(m_timeSeries, SIGNAL(hoverCluster(Cluster *, QList<Attribute *>)), this, SLOT(updateArcDiagramMarks()));
  connect(m_timeSeries, SIGNAL(marksChanged()), this, SLOT(updateArcDiagramMarks()));
  connect(m_timeSeries, SIGNAL(animationChanged()), this, SLOT(updateArcDiagramMarks()));

  connect(m_arcDiagram, SIGNAL(clickedCluster(Cluster *)), m_timeSeries, SLOT(markItems(Cluster *)));
  connect(m_graph, SIGNAL(clusteringChanged()), this, SLOT(updateArcDiagramMarks()));

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
    m_ui.attributes->item(i, 1)->setText(attribute->name());
    m_ui.attributes->item(i, 2)->setText(attribute->type());
    m_ui.attributes->item(i, 3)->setText(QString::number(attribute->getSizeCurValues()));
  }

  m_ui.attributes->resizeColumnsToContents();

  updateAttributeOperations();
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

    m_graph->setFileName(filename);
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
  QString filename = m_graph->filename();

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

void MainWindow::modeSelected(QAction *action)
{
  if (action == m_ui.actionSimulationMode)
  {
    m_ui.mainViewStack->setCurrentWidget(m_ui.analysisPage);
    m_ui.analysisStack->setCurrentWidget(m_ui.simulatorWidget);
    m_ui.examinerWidget->show();
  }
  if (action == m_ui.actionTraceMode)
  {
    m_ui.mainViewStack->setCurrentWidget(m_ui.analysisPage);
    m_ui.analysisStack->setCurrentWidget(m_ui.traceWidget);
    m_ui.examinerWidget->show();
  }
  if (action == m_ui.actionEditMode)
  {
    m_ui.mainViewStack->setCurrentWidget(m_ui.editPage);
    m_ui.examinerWidget->hide();
  }
}

void MainWindow::showAttributeContextMenu(const QPoint &position)
{
  m_ui.menuAttributes->popup(m_ui.attributes->viewport()->mapToGlobal(position));
}

void MainWindow::updateAttributeOperations()
{
  QList<int> attributes = selectedAttributes();
  int items = attributes.size();

  m_ui.actionClusterNodes->setEnabled(items > 0);
  m_ui.actionViewTrace->setEnabled(items > 0);
  m_ui.actionDistributionPlot->setEnabled(items == 1);
  m_ui.actionCorrelationPlot->setEnabled(items == 2);
  m_ui.actionCombinationPlot->setEnabled(items > 0);
  m_ui.actionDuplicate->setEnabled(items == 1);
  m_ui.actionRenameAttribute->setEnabled(items == 1);
  m_ui.actionDelete->setEnabled(items > 0);

  m_ui.domain->setRowCount(0);
  if (attributes.size() == 1)
  {
    assert(attributes[0] < int(m_graph->getSizeAttributes()));

    std::vector<size_t> valueDistribution;
    m_graph->calcAttrDistr(attributes[0], valueDistribution);

    Attribute *attribute = m_graph->getAttribute(attributes[0]);
    for (size_t i = 0; i < attribute->getSizeCurValues(); i++)
    {
      Value *value = attribute->getCurValue(i);

      m_ui.domain->insertRow(i);
      for (int j = 0; j < m_ui.attributes->columnCount(); j++)
      {
        m_ui.domain->setItem(i, j, new QTableWidgetItem());
        m_ui.domain->item(i, j)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled);
      }

      m_ui.domain->item(i, 0)->setText(QString::number(i));
      m_ui.domain->item(i, 1)->setText(QString::fromStdString(value->getValue()));
      m_ui.domain->item(i, 2)->setText(QString::number(valueDistribution[i]));
      m_ui.domain->item(i, 3)->setText(QString::number(100 * valueDistribution[i] / (double)m_graph->getSizeNodes()));
    }
  }
}

void MainWindow::clusterNodes()
{
  QList<int> attributes = selectedAttributes();
  std::vector<size_t> attributeVector;
  for (int i = 0; i < attributes.size(); i++)
  {
    attributeVector.push_back(attributes[i]);
  }
  m_graph->clustNodesOnAttr(attributeVector);
}

void MainWindow::viewTrace()
{
  QList<int> attributes = selectedAttributes();
  std::vector<size_t> attributeVector;
  for (int i = 0; i < attributes.size(); i++)
  {
    attributeVector.push_back(attributes[i]);
  }
  m_timeSeries->initAttributes(attributeVector);
}

void MainWindow::distributionPlot()
{
  QList<int> attributes = selectedAttributes();
  if (attributes.size() != 1)
  {
    return;
  }

  DistrPlot *plot = new DistrPlot(0, m_graph, attributes[0]);
  connect(this, SIGNAL(closingGraph()), plot, SLOT(close()));
  plot->setAttribute(Qt::WA_DeleteOnClose);
  plot->setDiagram(m_diagramEditor->diagram());
  plot->show();
}

void MainWindow::correlationPlot()
{
  QList<int> attributes = selectedAttributes();
  if (attributes.size() != 2)
  {
    return;
  }

  CorrlPlot *plot = new CorrlPlot(0, m_graph, attributes[0], attributes[1]);
  connect(this, SIGNAL(closingGraph()), plot, SLOT(close()));
  plot->setAttribute(Qt::WA_DeleteOnClose);
  plot->setDiagram(m_diagramEditor->diagram());
  plot->show();
}

void MainWindow::combinationPlot()
{
  QList<int> attributes = selectedAttributes();
  if (attributes.size() == 0)
  {
    return;
  }
  std::vector<size_t> attributeVector;
  for (int i = 0; i < attributes.size(); i++)
  {
    attributeVector.push_back(attributes[i]);
  }

  CombnPlot *plot = new CombnPlot(0, m_graph, attributeVector);
  connect(this, SIGNAL(closingGraph()), plot, SLOT(close()));
  plot->setAttribute(Qt::WA_DeleteOnClose);
  plot->setDiagram(m_diagramEditor->diagram());
  plot->show();
}

void MainWindow::duplicateAttribute()
{

}

void MainWindow::renameAttribute()
{

}

void MainWindow::deleteAttribute()
{

}


void MainWindow::routeCluster(Cluster *cluster, QList<Cluster *> clusterSet, QList<Attribute *> attributes)
{
  if (m_routingCluster)
  {
    delete m_routingCluster;
  }

  m_routingCluster = (cluster == 0 ? 0 : new Cluster(*cluster));
  m_routingClusterSet = clusterSet;
  m_routingClusterAttributes = attributes;

  QObject *sender = QObject::sender();

  QMenu *menu = new QMenu();
  connect(menu, SIGNAL(aboutToHide()), menu, SLOT(deleteLater()));

  QAction *toSimulator = menu->addAction("Send this to simulator");
  connect(toSimulator, SIGNAL(triggered()), this, SLOT(toSimulator()));
  toSimulator->setEnabled(cluster != 0 && sender != m_simulator);

  menu->addSeparator();

  QAction *toTrace = menu->addAction("Mark this in trace");
  connect(toTrace, SIGNAL(triggered()), this, SLOT(toTrace()));
  toTrace->setEnabled(cluster != 0 && sender != m_timeSeries);

  QAction *allToTrace = menu->addAction("Mark all in trace");
  connect(allToTrace, SIGNAL(triggered()), this, SLOT(allToTrace()));
  allToTrace->setEnabled(!clusterSet.isEmpty() && sender != m_timeSeries);

  menu->addSeparator();

  QAction *toExaminer = menu->addAction("Send this to examiner");
  connect(toExaminer, SIGNAL(triggered()), this, SLOT(toExaminer()));
  toExaminer->setEnabled(cluster != 0 && sender != m_examiner);

  QAction *allToExaminer = menu->addAction("Send all to examiner");
  connect(allToExaminer, SIGNAL(triggered()), this, SLOT(allToExaminer()));
  allToExaminer->setEnabled(!clusterSet.isEmpty() && sender != m_examiner);

  menu->popup(QCursor::pos());
}

void MainWindow::hoverCluster(Cluster *cluster, QList<Attribute *> attributes)
{
  if (cluster)
  {
    m_examiner->setFrame(cluster, attributes.toVector().toStdVector(), VisUtils::coolBlue);
  }
  else
  {
    m_examiner->clrFrame();
  }
}

void MainWindow::updateArcDiagramMarks()
{
  m_arcDiagram->unmarkLeaves();
  m_arcDiagram->unmarkBundles();

  if (simulationMode())
  {
    m_arcDiagram->markLeaf(m_simulator->SelectedClusterIndex(), m_simulator->SelectColor());
  }
  else if (traceMode())
  {
    QColor color;
    std::set<size_t> indices;
    size_t index;

    m_timeSeries->getIdcsClstMarked(indices, color);
    for (std::set<size_t>::iterator i = indices.begin(); i != indices.end(); i++)
    {
      m_arcDiagram->markLeaf(*i, color);
    }

    m_timeSeries->getIdxMseOver(index, indices, color);
    if (index != NON_EXISTING)
    {
      m_arcDiagram->markLeaf(index, color);
      for (std::set<size_t>::iterator i = indices.begin(); i != indices.end(); i++)
      {
        m_arcDiagram->markBundle(*i);
      }
    }

    m_timeSeries->getCurrIdxDgrm(index, indices, color);
    if (index != NON_EXISTING)
    {
      m_arcDiagram->markLeaf(index, color);
      for (std::set<size_t>::iterator i = indices.begin(); i != indices.end(); i++)
      {
        m_arcDiagram->markBundle(*i);
      }
    }

    m_timeSeries->getAnimIdxDgrm(index, indices, color);
    if (index != NON_EXISTING)
    {
      m_arcDiagram->markLeaf(index, color);
      for (std::set<size_t>::iterator i = indices.begin(); i != indices.end(); i++)
      {
        m_arcDiagram->markBundle(*i);
      }
    }
  }

  m_arcDiagram->markLeaf(m_examiner->selectedClusterIndex(), m_examiner->selectionColor());
}



QList<int> MainWindow::selectedAttributes()
{
  QMap<int, int> output;
  QList<QTableWidgetSelectionRange> ranges = m_ui.attributes->selectedRanges();
  for (int i = 0; i < ranges.size(); i++)
  {
    for (int j = ranges[i].topRow(); j <= ranges[i].bottomRow(); j++)
    {
      if (!output.contains(j))
      {
        output[j] = j;
      }
    }
  }
  return output.values();
}

