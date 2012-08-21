// Author(s): A.J. (Hannes) Pretorius, Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QtGui>
#include <QMainWindow>

#include "ui_mainwindow.h"

#include "arcdiagram.h"
#include "diagrameditor.h"
#include "examiner.h"
#include "mediator.h"
#include "parser.h"
#include "settings.h"
#include "settingsdialog.h"
#include "simulator.h"
#include "timeseries.h"

class MainWindow : public QMainWindow, public Mediator
{
  Q_OBJECT

  public:
    MainWindow();

    bool simulationMode() const { return m_ui.actionSimulationMode->isChecked(); }
    bool traceMode() const { return m_ui.actionTraceMode->isChecked(); }
    bool editMode() const { return m_ui.actionEditMode->isChecked(); }

  public slots:
    void open(QString filename);
    void save(QString filename);

  protected slots:
    void updateAttributes();
    void updateAttributeOperations();
    void updateValues();
    void updateValueOperations();
    void openFile();
    void saveFile();
    void saveFileAs();

//    void openAttributeConfiguration();
//    void saveAttributeConfiguration();

//    void openDiagram();
//    void saveDiagram();

    void modeSelected(QAction* action);

    void showAttributeContextMenu(const QPoint &position);

    void clusterNodes();
    void viewTrace();
    void distributionPlot();
    void correlationPlot();
    void combinationPlot();
    void duplicateAttribute();
    void renameAttribute();
    void deleteAttribute();
    void moveAttribute(int index, int newPosition);
    void groupValues();
    void ungroupValues();
    void renameValue();
    void moveValue(int index, int newPosition);

    void routeCluster(Cluster *cluster, QList<Cluster *> clusterSet, QList<Attribute *> attributes);
    void toSimulator() { m_simulator->initFrameCurr(m_routingCluster, m_routingClusterAttributes.toVector().toStdVector()); }
    void toTrace() { m_timeSeries->markItems(m_routingCluster); }
    void allToTrace() { m_timeSeries->markItems(m_routingClusterSet); }
    void toExaminer() { m_examiner->addFrameHist(m_routingCluster, m_routingClusterAttributes.toVector().toStdVector()); }
    void allToExaminer() { m_examiner->addFrameHist(m_routingClusterSet, m_routingClusterAttributes.toVector().toStdVector()); }

    void hoverCluster(Cluster *cluster, QList<Attribute *> attributes);

    void updateArcDiagramMarks();

  protected:
    /**
     * @brief Saves window information
     */
    void closeEvent(QCloseEvent *event);

    QList<int> selectedAttributes();
    QList<int> selectedValues();

  signals:
    void closingGraph();

  private:
    Ui::MainWindow m_ui;

    Parser m_parser;
    Settings m_settings;
    SettingsDialog *m_settingsDialog;

    Graph *m_graph;
    QProgressDialog m_clusteringProgress;

    Examiner *m_examiner;
    ArcDiagram *m_arcDiagram;
    Simulator *m_simulator;
    TimeSeries *m_timeSeries;
    DiagramEditor *m_diagramEditor;

    Cluster *m_routingCluster;
    QList<Cluster *> m_routingClusterSet;
    QList<Attribute *> m_routingClusterAttributes;







  public:
    virtual QColor getColor(QColor col) { return col; }

    virtual void handleNote(const size_t& shapeId, const std::string& msg) {}
    virtual void handleEditModeDOF(Colleague* c) {}

    virtual void handleEditShape(
      const bool& cut,
      const bool& copy,
      const bool& paste,
      const bool& clear,
      const bool& bringToFront,
      const bool& sendToBack,
      const bool& bringForward,
      const bool& sendBackward,
      const bool& editDOF,
      const int&  checkedItem) {}

    virtual void handleEditDOF(
      const std::vector< size_t > &degsOfFrdmIds,
      const std::vector< std::string > &degsOfFrdm,
      const std::vector< size_t > &attrIndcs,
      const size_t& selIdx) {}
    virtual void handleDOFSel(const size_t& DOFIdx) {}

    virtual void setDOFColorSelected() {}
    virtual void setDOFOpacitySelected() {}

    virtual void handleSetDOFTextStatus(
      const size_t& DOFIdx,
      const int& status) {}
    virtual size_t handleGetDOFTextStatus(const size_t& DOFIdx) { return 0; }

    virtual void handleDOFColActivate() {}
    virtual void handleDOFColDeactivate() {}
    virtual void handleDOFColAdd(
      const double& hue,
      const double& y) {}
    virtual void handleDOFColUpdate(
      const size_t& idx,
      const double& hue,
      const double& y) {}
    virtual void handleDOFColClear(
      const size_t& idx) {}
    virtual void handleDOFColSetValuesEdt(
      const std::vector< double > &hue,
      const std::vector< double > &y) {}

    virtual void handleDOFOpaActivate() {}
    virtual void handleDOFOpaDeactivate() {}
    virtual void handleDOFOpaAdd(
      const double& hue,
      const double& y) {}
    virtual void handleDOFOpaUpdate(
      const size_t& idx,
      const double& hue,
      const double& y) {}
    virtual void handleDOFOpaClear(
      const size_t& idx) {}
    virtual void handleDOFOpaSetValuesEdt(
      const std::vector< double > &hue,
      const std::vector< double > &y) {}

    virtual void handleLinkDOFAttr(
      const size_t DOFIdx,
      const size_t attrIdx) {}
    virtual void handleUnlinkDOFAttr(const size_t DOFIdx) {}
    virtual void handleDOFFrameDestroy() {}
    virtual void handleDOFDeselect() {}
};

#endif
