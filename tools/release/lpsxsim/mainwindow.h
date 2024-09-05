// Author: Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef LPSXSIM_MAINWINDOW_H
#define LPSXSIM_MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "ui_mainwindow.h"

#ifndef Q_MOC_RUN // Workaround for QTBUG-22829
#include "mcrl2/gui/persistentfiledialog.h"
#endif // Q_MOC_RUN

#include "simulation.h"

class TraceTableModel;

class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:
    MainWindow(QThread *aterm_thread, mcrl2::data::rewrite_strategy strategy, bool do_not_use_dummies);
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
    void setAutoSelectProbability();

  public slots:
    void openSpecification(QString filename);
    void onInitializedSimulation();
    void selectState(unsigned long long state);
    void truncateTrace(int state, int column_on_screen);
    void selectTransition(int transition);
    void animationStep();
    void undoLast();

    /**
     * @brief Updates the statusbar with the latest log output
     */
    void onLogOutput(QString level, QDateTime timestamp, QString message, QString formattedMessage);

  protected:
    void reset(unsigned long long selectedState, bool probabilistic);
    void select(unsigned long long transition);
    void waitForResponse(QEventLoop *eventLoop, QSemaphore *semaphore, int timeout = 50);
    /**
     * @brief Saves window information
     */
    void closeEvent(QCloseEvent *event);
    QString renderStateChange(Simulation::State source, Simulation::State destination);
    void auto_select_state_or_probability();

  private:
    Ui::MainWindow m_ui;
    QThread *m_atermThread;
    mcrl2::data::rewrite_strategy m_strategy;
    Simulation *m_simulation;
    Simulation *m_newSimulation = nullptr;
    Simulation::Trace m_trace;
    unsigned long long m_selected_state;
    QTimer *m_animationTimer;
    bool m_randomAnimation;
    bool m_animationDisabled;
    const bool m_do_not_use_dummies;
    bool update_simulation_is_running=false;  

    mcrl2::gui::qt::PersistentFileDialog m_fileDialog;
};

#endif
