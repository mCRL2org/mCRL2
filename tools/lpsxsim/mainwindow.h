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
#include <QSemaphore>
#include <QTimer>
#include "ui_mainwindow.h"

#ifndef Q_MOC_RUN // Workaround for QTBUG-22829
#include "mcrl2/data/rewriter.h"
#include "mcrl2/gui/persistentfiledialog.h"
#endif // Q_MOC_RUN

#include "simulation.h"

class TraceTableModel;

class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:
    MainWindow(mcrl2::data::rewrite_strategy strategy,const bool do_not_use_dummies);
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
    void undoLast();

    /**
     * @brief Updates the statusbar with the latest log output
     */
    void onLogOutput(QString level, QString hint, QDateTime timestamp, QString message, QString formattedMessage);

  protected:
    void reset(unsigned int selectedState);
    void select(unsigned int transition);
    void waitForResponse(QEventLoop *eventLoop, QSemaphore *semaphore, int timeout = 50);
    /**
     * @brief Saves window information
     */
    void closeEvent(QCloseEvent *event);
    QString renderStateChange(Simulation::State source, Simulation::State destination);

  private:
    Ui::MainWindow m_ui;
    mcrl2::data::rewrite_strategy m_strategy;
    Simulation *m_simulation;
    Simulation::Trace m_trace;
    int m_selectedState;
    QTimer *m_animationTimer;
    bool m_randomAnimation;
    bool m_animationDisabled;
    const bool m_do_not_use_dummies;

    mcrl2::gui::qt::PersistentFileDialog m_fileDialog;
};

#endif
