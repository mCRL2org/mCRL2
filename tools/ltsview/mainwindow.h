// Author(s): Bas Ploeger, Carst Tankink, Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QProgressDialog>
#include "ui_mainwindow.h"

#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/logwidget.h"
#include "mcrl2/utilities/persistentfiledialog.h"

#include "infodock.h"
#include "ltscanvas.h"
#include "ltsmanager.h"
#include "markdock.h"
#include "markmanager.h"
#include "settings.h"
#include "settingsdialog.h"
#include "settingsdock.h"
#include "simdock.h"

class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:
    MainWindow();
    ~MainWindow();

  public slots:
    void open(QString filename);

  protected slots:
    void open();
    void openTrace();
    void exportBitmap();
    void exportText();
    void exportVector();
    void setStatusBar(QString message) { m_ui.statusbar->showMessage(message); }
    void clearStatusBar() { m_ui.statusbar->clearMessage(); }

    void startRendering() { setStatusBar("Rendering..."); }
    void loadingLts() { setProgress(0, "Loading file"); }
    void rankingStates() { setProgress(1, "Ranking states"); }
    void clusteringStates() { setProgress(2, "Clustering states"); }
    void computingClusterInfo() { setProgress(3, "Setting cluster info"); }
    void positioningClusters() { setProgress(4, "Positioning clusters"); }
    void positioningStates() { setProgress(5, "Positioning states"); }
    void hideProgressDialog() { setProgress(6, ""); }
    void setProgress(int phase, QString message);
    void selectionChanged();
    void zoomChanged() { m_ui.zoomOut->setEnabled(m_ltsManager->lts()->getPreviousLevel() != 0); }
    void startStructuring() { setEnabled(false); m_ltsCanvas->setUpdatesEnabled(false); }
    void stopStructuring() { m_ltsCanvas->setUpdatesEnabled(true); setEnabled(true); }
    void logMessage(QString level, QString hint, QDateTime /* timestamp */, QString message) 
    {
      if (log_level_from_string(level.toStdString()) == mcrl2::log::error)
      {
        QMessageBox::critical(this, QString("LTSView - An error occured (%1)").arg(hint), message);
      }
      else
      {
        setStatusBar(message);
      }
    }

  protected:
    /**
     * @brief Saves window information
     */
    void closeEvent(QCloseEvent *event);

  private:
    Ui::MainWindow m_ui;
    Settings m_settings;
    LtsManager *m_ltsManager;
    MarkManager *m_markManager;
    InfoDock *m_infoDock;
    MarkDock *m_markDock;
    SimDock *m_simDock;
    SettingsDock *m_settingsDock;
    SettingsDialog *m_settingsDialog;
    LtsCanvas *m_ltsCanvas;
    QProgressDialog *m_progressDialog;
    mcrl2::utilities::qt::LogRelay m_logRelay;

    mcrl2::utilities::qt::PersistentFileDialog m_fileDialog;
};

#endif
