// Author: Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef LPSXSIM_MAINWINDOW_H
#define LPSXSIM_MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "ui_mainwindow.h"

#include "mcrl2/utilities/persistentfiledialog.h"

#include "simulation.h"

class TraceTableModel;

class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:
    MainWindow();
    ~MainWindow();

  protected slots:
    void openSpecification();
    void loadTrace();
    void saveTrace();
    void playTrace();
    void randomPlay();
    void stopPlay();
    void setPlayDelay();
    void updateSimulation();
    void stateSelected();
    void setTauPrioritization();

  public slots:
    void openSpecification(QString filename);
    void selectState(int state);
    void truncateTrace(int state);
    void selectTransition(int transition);
    void animationStep();

    /**
     * @brief Updates the statusbar with the latest log output
     */
    void onLogOutput(QString level, QString hint, QDateTime timestamp, QString message, QString formattedMessage);

  protected:
    /**
     * @brief Saves window information
     */
    void closeEvent(QCloseEvent *event);
    QString renderStateChange(Simulation::State source, Simulation::State destination);

  private:
    Ui::MainWindow m_ui;
    Simulation *m_simulation;
    Simulation::Trace m_trace;
    int m_selectedState;
    QTimer *m_animationTimer;
    bool m_randomAnimation;

    mcrl2::utilities::qt::PersistentFileDialog m_fileDialog;
};

#endif
