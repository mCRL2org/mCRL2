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
#include "combnplot.h"
#include "corrlplot.h"
#include "diagrameditor.h"
#include "distrplot.h"
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

  public slots:
    void open(QString filename);
    void save(QString filename);

  protected slots:
    void openFile();
    void saveFile();
    void saveFileAs();

  private:
    Ui::MainWindow m_ui;

    Parser m_parser;
    Settings m_settings;
    SettingsDialog *m_settingsDialog;
    QSet<QDialog *> m_plots;

    Graph *m_graph;

    Examiner *m_examiner;
    ArcDiagram *m_arcDiagram;
    Simulator *m_simulator;
    TimeSeries *m_timeSeries;
    DiagramEditor *m_diagramEditor;









  public:
    virtual void initProgress(
      const std::string& title,
      const std::string& msg,
      const size_t& max) {}
    virtual void updateProgress(const size_t& val) {}
    virtual void closeProgress() {}

    virtual QColor getColor(QColor col) { return col; }

    virtual void handleAttributeCluster(const std::vector< size_t > &indcs) {}

    virtual int getMode() { return 0; }
    virtual int getView() { return 0; }

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

    virtual void initSimulator(
      Cluster* currFrame,
      const std::vector< Attribute* > &attrs) {}

    virtual void initTimeSeries(const std::vector< size_t > attrIdcs) {}
    virtual void markTimeSeries(
      Colleague* sender,
      Cluster* currFrame) {}
    virtual void markTimeSeries(
      Colleague* sender,
      const std::vector< Cluster* > frames) {}

    virtual void addToExaminer(
      Cluster* currFrame,
      const std::vector< Attribute* > &attrs) {}
    virtual void addToExaminer(
      const std::vector< Cluster* > frames,
      const std::vector< Attribute* > &attrs) {}

    virtual void handleSendDgrm(
      Colleague* sender,
      const bool& sendSglToSiml,
      const bool& sendSglToTrace,
      const bool& sendSetToTrace,
      const bool& sendSglToExnr,
      const bool& sendSetToExnr) {}
    virtual void handleSendDgrmSglToSiml() {}
    virtual void handleSendDgrmSglToTrace() {}
    virtual void handleSendDgrmSetToTrace() {}
    virtual void handleSendDgrmSglToExnr() {}
    virtual void handleSendDgrmSetToExnr() {}

    virtual void handleClearSim(void* sender) {}
    virtual void handleClearExnr(void* sender) {}
    virtual void handleClearExnrCur(void* sender) {}

    virtual void handleAnimFrameClust(Colleague* sender) {}

    virtual void handleMarkFrameClust(Colleague* sender) {}
    virtual void handleUnmarkFrameClusts(Colleague* sender) {}

    virtual void handleShowFrame(
      Cluster* frame,
      const std::vector< Attribute* > &attrs,
      QColor col) {}
    virtual void handleUnshowFrame() {}
};

#endif
