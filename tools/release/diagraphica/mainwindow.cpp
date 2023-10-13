// Author(s): A.J. (Hannes) Pretorius, Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
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
#include <QSettings>


MainWindow::MainWindow():
  m_settingsDialog(new SettingsDialog(this, &m_settings)),
  m_graph(0),
  m_examiner(0),
  m_arcDiagram(0),
  m_simulator(0),
  m_diagramEditor(0),
  m_routingCluster(0),
  m_fileDialog("", this)
{
  m_ui.setupUi(this);

  QActionGroup* groupMode = new QActionGroup(m_ui.menuMode);
  groupMode->addAction(m_ui.actionSimulationMode);
  groupMode->addAction(m_ui.actionEditMode);

  m_ui.attributes->resizeColumnsToContents();
  m_ui.domain->resizeColumnsToContents();

  connect(m_ui.actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
  connect(m_ui.actionSave, SIGNAL(triggered()), this, SLOT(saveFile()));
  connect(m_ui.actionSaveAs, SIGNAL(triggered()), this, SLOT(saveFileAs()));

  connect(m_ui.actionOpenAttributes, SIGNAL(triggered()), this, SLOT(openAttributeConfiguration()));
  connect(m_ui.actionSaveAttributes, SIGNAL(triggered()), this, SLOT(saveAttributeConfiguration()));

  connect(m_ui.actionOpenDiagram, SIGNAL(triggered()), this, SLOT(openDiagram()));
  connect(m_ui.actionSaveDiagram, SIGNAL(triggered()), this, SLOT(saveDiagram()));

  connect(m_ui.actionQuit, SIGNAL(triggered()), QCoreApplication::instance(), SLOT(quit()));

  connect(groupMode, SIGNAL(triggered(QAction*)), this, SLOT(modeSelected(QAction*)));

  connect(m_ui.actionSettingsGeneral, SIGNAL(triggered()), m_settingsDialog, SLOT(showGeneral()));
  connect(m_ui.actionSettingsArcDiagram, SIGNAL(triggered()), m_settingsDialog, SLOT(showArcDiagram()));

  connect(m_ui.attributes, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showAttributeContextMenu(const QPoint &)));
  m_ui.attributes->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(m_ui.attributes, SIGNAL(itemSelectionChanged()), this, SLOT(updateAttributeOperations()));
  connect(m_ui.domain, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showDomainContextMenu(const QPoint&)));
  m_ui.domain->setContextMenuPolicy(Qt::CustomContextMenu);
  updateAttributeOperations();

  connect(m_ui.actionClusterNodes, SIGNAL(triggered()), this, SLOT(clusterNodes()));
  connect(m_ui.actionDistributionPlot, SIGNAL(triggered()), this, SLOT(distributionPlot()));
  connect(m_ui.actionCorrelationPlot, SIGNAL(triggered()), this, SLOT(correlationPlot()));
  connect(m_ui.actionCombinationPlot, SIGNAL(triggered()), this, SLOT(combinationPlot()));
  connect(m_ui.actionDuplicate, SIGNAL(triggered()), this, SLOT(duplicateAttribute()));
  connect(m_ui.actionRenameAttribute, SIGNAL(triggered()), this, SLOT(renameAttribute()));
  connect(m_ui.actionDelete, SIGNAL(triggered()), this, SLOT(deleteAttribute()));
  connect(m_ui.attributes, SIGNAL(itemMoved(int, int)), this, SLOT(moveAttribute(int, int)));

  connect(m_ui.actionGroup, SIGNAL(triggered()), this, SLOT(groupValues()));
  connect(m_ui.actionUngroup, SIGNAL(triggered()), this, SLOT(ungroupValues()));
  connect(m_ui.actionRenameValue, SIGNAL(triggered()), this, SLOT(renameValue()));
  connect(m_ui.domain, SIGNAL(itemMoved(int, int)), this, SLOT(moveValue(int, int)));
  connect(m_ui.domain, SIGNAL(itemSelectionChanged()), this, SLOT(updateValue()));


  QSettings settings("mCRL2", "DiaGraphica");
  restoreGeometry(settings.value("geometry").toByteArray());
  restoreState(settings.value("windowState").toByteArray());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  QSettings settings("mCRL2", "DiaGraphica");
  settings.setValue("geometry", saveGeometry());
  settings.setValue("windowState", saveState());
  QMainWindow::closeEvent(event);
  QApplication::quit();
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
      << m_simulator;

  for (int i = 0; i < oldWidgets.size(); ++i)
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

  m_ui.numberOfStates->setText(QString::number(m_graph->getSizeNodes()));
  m_ui.numberOfTransitions->setText(QString::number(m_graph->getSizeEdges()));

  m_diagramEditor = new DiagramEditor(m_ui.diagramEditorWidget, m_graph);
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

  connect(m_graph, SIGNAL(clusteringChanged()), this, SLOT(updateArcDiagramMarks()));

  m_ui.actionSave->setEnabled(true);
  m_ui.actionSaveAs->setEnabled(true);
  m_ui.actionOpenAttributes->setEnabled(true);
  m_ui.actionSaveAttributes->setEnabled(true);
  m_ui.actionOpenDiagram->setEnabled(true);
  m_ui.actionSaveDiagram->setEnabled(true);

  updateAttributes();
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

void MainWindow::updateAttributes()
{
  m_ui.attributes->setRowCount((int)m_graph->getSizeAttributes());
  for (std::size_t i = 0; i < m_graph->getSizeAttributes(); ++i)
  {
    for (int j = 0; j < m_ui.attributes->columnCount(); ++j)
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

void MainWindow::updateAttributeOperations()
{
  QList<int> attributes = selectedAttributes();
  int items = attributes.size();

  m_ui.actionClusterNodes->setEnabled(items > 0);
  m_ui.actionDistributionPlot->setEnabled(items == 1);
  m_ui.actionCorrelationPlot->setEnabled(items == 2);
  m_ui.actionCombinationPlot->setEnabled(items > 0);
  m_ui.actionDuplicate->setEnabled(items == 1);
  m_ui.actionRenameAttribute->setEnabled(items == 1);
  m_ui.actionDelete->setEnabled(items > 0);

  updateValues();
}

void MainWindow::updateValues()
{
  QList<int> attributes = selectedAttributes();
  if (attributes.size() == 1)
  {
    assert(attributes[0] < int(m_graph->getSizeAttributes()));

    std::vector<std::size_t> valueDistribution;
    m_graph->calcAttrDistr(attributes[0], valueDistribution);

    Attribute *attribute = m_graph->getAttribute(attributes[0]);
    m_ui.domain->setRowCount((int)attribute->getSizeCurValues());
    for (std::size_t i = 0; i < attribute->getSizeCurValues(); ++i)
    {
      Value *value = attribute->getCurValue(i);

      for (int j = 0; j < m_ui.attributes->columnCount(); ++j)
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
  else
  {
    m_ui.domain->setRowCount(0);
  }

  updateValueOperations();
}

void MainWindow::updateValueOperations()
{
  QList<int> values = selectedValues();
  int items = values.size();
  m_ui.actionGroup->setEnabled(items > 1);
  m_ui.actionUngroup->setEnabled(items > 0);
  m_ui.actionRenameValue->setEnabled(items == 1);
}

void MainWindow::updateValueSelection(int count)
{
  QList<QTableWidgetSelectionRange> ranges = m_ui.domain->selectedRanges();
  m_ui.domain->clearSelection();
  if (ranges.size() == 0 || count < 1)
    return;
  int index = ranges[0].topRow();
  for (const QTableWidgetSelectionRange& range : ranges)
  {
    index = qMin(index, range.topRow());
  }
  QTableWidgetSelectionRange range(index, ranges[0].leftColumn(), index + count - 1, ranges[0].rightColumn());
  m_ui.domain->setRangeSelected(range, true);
}

void MainWindow::openFile()
{
  QString filter = QString("All supported files (") + QString::fromStdString(mcrl2::lts::detail::lts_extensions_as_string(" ")) + ");;All files (*.*)";
  QString filename = m_fileDialog.getOpenFileName("Open LTS", filter);
  if (filename.isNull())
  {
    return;
  }

  open(filename);
}

void MainWindow::saveFile()
{
  QString filter = QString("FSM files (*.fsm);;All files (*.*)");
  QString filename = m_graph->filename();

  if (filename.isNull() || QFileInfo(filename).suffix().toLower() != "fsm")
  {
    filename = m_fileDialog.getSaveFileName("Save LTS", filter);
  }
  if (filename.isNull())
  {
    return;
  }
  save(filename);
}

void MainWindow::saveFileAs()
{
  QString filter = QString("FSM files (*.fsm);;All files (*.*)");
  QString filename = m_fileDialog.getSaveFileName("Save LTS", filter);

  if (filename.isNull())
  {
    return;
  }
  save(filename);
}

void MainWindow::openAttributeConfiguration()
{
  QString filter = "Attribute configuration files (*.dgc);;All files (*.*)";
  QString filename = m_fileDialog.getOpenFileName("Open Attribute Configuration", filter);
  if (filename.isNull())
  {
    return;
  }

  try
  {
    std::map<std::size_t, std::size_t > attrIdxFrTo;
    std::map<std::size_t, std::vector< std::string > > attrCurDomains;
    std::map<std::size_t, std::map< std::size_t, std::size_t  > > attrOrigToCurDomains;

    m_parser.parseAttrConfig(filename, m_graph, attrIdxFrTo, attrCurDomains, attrOrigToCurDomains);

    m_graph->configAttributes(attrIdxFrTo, attrCurDomains, attrOrigToCurDomains);

    updateAttributes();
  }
  catch (const mcrl2::runtime_error& e)
  {
    QMessageBox::critical(this, "Error", QString::fromStdString(e.what()));
    return;
  }
}

void MainWindow::saveAttributeConfiguration()
{
  QString filter = "Attribute configuration files (*.dgc);;All files (*.*)";
  QString filename = m_fileDialog.getSaveFileName("Save Attribute Configuration", filter);
  if (filename.isNull())
  {
    return;
  }

  try
  {
    m_parser.writeAttrConfig(filename, m_graph);
  }
  catch (const mcrl2::runtime_error& e)
  {
    QMessageBox::critical(this, "Error", QString::fromStdString(e.what()));
    return;
  }
}

void MainWindow::openDiagram()
{
  QString filter = "Diagram files (*.dgd);;All files (*.*)";
  QString filename = m_fileDialog.getOpenFileName("Open Diagram", filter);
  if (filename.isNull())
  {
    return;
  }

  Diagram diagram;
  try
  {
    m_parser.parseDiagram(filename, m_graph, &diagram);
  }
  catch (const mcrl2::runtime_error& e)
  {
    QMessageBox::critical(this, "Error", QString::fromStdString(e.what()));
    return;
  }

  *m_diagramEditor->diagram() = diagram;

  update();
}

void MainWindow::saveDiagram()
{
  QString filter = "Diagram files (*.dgd);;All files (*.*)";
  QString filename = m_fileDialog.getSaveFileName("Open Diagram", filter);
  if (filename.isNull())
  {
    return;
  }

  try
  {
    m_parser.writeDiagram(filename, m_graph, m_diagramEditor->diagram());
  }
  catch (const mcrl2::runtime_error& e)
  {
    QMessageBox::critical(this, "Error", QString::fromStdString(e.what()));
    return;
  }
}

void MainWindow::modeSelected(QAction *action)
{
  if (action == m_ui.actionSimulationMode)
  {
    m_ui.mainViewStack->setCurrentWidget(m_ui.analysisPage);
    m_ui.analysisStack->setCurrentWidget(m_ui.simulatorWidget);
    m_ui.examinerWidget->show();
  }
  if (action == m_ui.actionEditMode)
  {
    m_ui.mainViewStack->setCurrentWidget(m_ui.editPage);
    m_ui.examinerWidget->hide();
  }
  updateAttributeOperations();
}

void MainWindow::showAttributeContextMenu(const QPoint &position)
{
  m_ui.menuAttributes->popup(m_ui.attributes->viewport()->mapToGlobal(position));
}

void MainWindow::showDomainContextMenu(const QPoint& position)
{
    m_ui.menuDomain->popup(m_ui.domain->viewport()->mapToGlobal(position));
}

void MainWindow::clusterNodes()
{
  QList<int> attributes = selectedAttributes();
  std::vector<std::size_t> attributeVector;
  for (int i = 0; i < attributes.size(); ++i)
  {
    attributeVector.push_back(attributes[i]);
  }
  m_graph->clustNodesOnAttr(attributeVector);
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
  std::vector<std::size_t> attributeVector;
  for (int i = 0; i < attributes.size(); ++i)
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
  std::vector<std::size_t> attributes;
  attributes.push_back(selectedAttributes().first());
  m_graph->duplAttributes(attributes);
  updateAttributes();
}

void MainWindow::renameAttribute()
{
  Attribute *attribute = m_graph->getAttribute(selectedAttributes().first());
  bool ok;
  QString name = QInputDialog::getText(this, "Rename attribute", "Please enter a new name for attribute '" + attribute->name() + "'.", QLineEdit::Normal, attribute->name(), &ok);
  if (ok)
  {
    attribute->setName(name);
    updateAttributes();
  }
}

void MainWindow::deleteAttribute()
{
  int attribute = selectedAttributes().first();
  if(QMessageBox::question(this, "Confirm attribute delete", "Are you sure you want to delete attribute '" + m_graph->getAttribute(attribute)->name() + "'?", QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
  {
    m_graph->deleteAttribute(attribute);
    updateAttributes();
  }
}

void MainWindow::moveAttribute(int index, int newPosition)
{
  m_graph->moveAttribute(index, newPosition);
  updateAttributes();
}

void MainWindow::groupValues()
{
  bool ok;
  QString name = QInputDialog::getText(this, "Group values", "Enter a new name for the value group.", QLineEdit::Normal, QString(), &ok);
  if (ok)
  {
    QList<int> temp_selected_values = selectedValues();
    m_graph->getAttribute(selectedAttributes().first())->
                clusterValues(std::vector<int>(temp_selected_values.begin(),
                                               temp_selected_values.end()), name.toStdString());
    updateValues();
    updateValueSelection(1);
  }
}

void MainWindow::ungroupValues()
{
  m_graph->getAttribute(selectedAttributes().first())->clearClusters();
  updateValues();
  updateValueSelection(0);
}

void MainWindow::renameValue()
{
  Value *value = m_graph->getAttribute(selectedAttributes().first())->getCurValue(selectedValues().first());
  bool ok;
  QString oldName = QString::fromStdString(value->getValue());
  QString name = QInputDialog::getText(this, "Rename attribute", "Please enter a new name for attribute '" + oldName + "'.", QLineEdit::Normal, oldName, &ok);
  if (ok)
  {
    value->setValue(name.toStdString());
    updateValues();
  }
}

void MainWindow::moveValue(int index, int newPosition)
{
  m_graph->getAttribute(selectedAttributes().first())->moveValue(index, newPosition);
  updateValues();
}

void MainWindow::updateValue()
{
  MainWindow::updateValueOperations();
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
    m_examiner->setFrame(cluster, std::vector<Attribute*>(attributes.begin(),attributes.end()), VisUtils::coolBlue);
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

  m_arcDiagram->markLeaf(m_examiner->selectedClusterIndex(), m_examiner->selectionColor());
}



QList<int> MainWindow::selectedAttributes()
{
  QMap<int, int> output;
  QList<QTableWidgetSelectionRange> ranges = m_ui.attributes->selectedRanges();
  for (int i = 0; i < ranges.size(); ++i)
  {
    for (int j = ranges[i].topRow(); j <= ranges[i].bottomRow(); ++j)
    {
      if (!output.contains(j))
      {
        output[j] = j;
      }
    }
  }
  return output.values();
}

QList<int> MainWindow::selectedValues()
{
  QMap<int, int> output;
  QList<QTableWidgetSelectionRange> ranges = m_ui.domain->selectedRanges();
  for (int i = 0; i < ranges.size(); ++i)
  {
    for (int j = ranges[i].topRow(); j <= ranges[i].bottomRow(); ++j)
    {
      if (!output.contains(j))
      {
        output[j] = j;
      }
    }
  }
  return output.values();
}

