// Author: Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "mainwindow.h"

#include <QEventLoop>
#include <QInputDialog>
#include <QMetaObject>
#include <QUrl>
#include <QSettings>
#include <QRandomGenerator>
#include <climits>

MainWindow::MainWindow(QThread *atermThread, mcrl2::data::rewrite_strategy strategy, bool do_not_use_dummies)
  : m_atermThread(atermThread),
    m_strategy(strategy),
    m_simulation(0),
    m_animationTimer(new QTimer(this)),
    m_do_not_use_dummies(do_not_use_dummies),
    m_fileDialog("", this)
{
  m_ui.setupUi(this);

  m_ui.traceTable->resizeColumnToContents(0);

  connect(m_ui.actionOpen, SIGNAL(triggered()), this, SLOT(openSpecification()));
  connect(m_ui.actionLoadTrace, SIGNAL(triggered()), this, SLOT(loadTrace()));
  connect(m_ui.actionSaveTrace, SIGNAL(triggered()), this, SLOT(saveTrace()));
  connect(m_ui.actionQuit, SIGNAL(triggered()), QApplication::instance(), SLOT(quit()));
  connect(m_ui.actionPlayTrace, SIGNAL(triggered()), this, SLOT(playTrace()));
  connect(m_ui.actionRandomPlay, SIGNAL(triggered()), this, SLOT(randomPlay()));
  connect(m_ui.actionStop, SIGNAL(triggered()), this, SLOT(stopPlay()));
  connect(m_ui.actionSetPlayDelay, SIGNAL(triggered()), this, SLOT(setPlayDelay()));
  connect(m_ui.actionAutoSelectProbability, SIGNAL(toggled(bool)), this, SLOT(setAutoSelectProbability()));
  connect(m_ui.actionOutput, SIGNAL(toggled(bool)), m_ui.dockWidget, SLOT(setVisible(bool)));

  connect(m_ui.traceTable, SIGNAL(itemSelectionChanged()), this, SLOT(stateSelected()));
  connect(m_ui.traceTable, SIGNAL(cellActivated(int, int)), this, SLOT(truncateTrace(int, int)));
  connect(m_ui.transitionTable, SIGNAL(cellActivated(int, int)), this, SLOT(selectTransition(int)));

  connect(m_ui.dockWidget->widget(), SIGNAL(logMessage(QString, QDateTime, QString, QString)), this, SLOT(onLogOutput(QString, QDateTime, QString, QString)));

  m_animationTimer->setInterval(1000);
  connect(m_animationTimer, SIGNAL(timeout()), this, SLOT(animationStep()));

  m_ui.transitionTable->setHorizontalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
  m_ui.traceTable->setHorizontalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
  m_ui.stateTable->setHorizontalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
  m_ui.actionPlayTrace->setEnabled(false);
  m_ui.actionRandomPlay->setEnabled(false);
  m_ui.actionStop->setEnabled(false);

  QSettings settings("mCRL2", "LpsXSim");
  restoreGeometry(settings.value("geometry").toByteArray());
  restoreState(settings.value("windowState").toByteArray());

  m_ui.actionOutput->setChecked(!m_ui.dockWidget->isHidden());
}

void MainWindow::undoLast()
{
  selectState(m_selected_state - 1);
  m_ui.actionUndo_last->setEnabled(m_selected_state > 0);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  QSettings settings("mCRL2", "LpsXSim");
  settings.setValue("geometry", saveGeometry());
  settings.setValue("windowState", saveState());
  QMainWindow::closeEvent(event);
}

MainWindow::~MainWindow()
{
  if (m_simulation)
  {
    m_simulation->deleteLater();
  }
}

static
QTableWidgetItem *item()
{
  QTableWidgetItem *item = new QTableWidgetItem();
  item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  return item;
}

void MainWindow::openSpecification()
{
  assert(m_ui.traceTable->isEnabled());

  QString filename = m_fileDialog.getOpenFileName("Open Process Specification", "Process specifications (*.lps)");
  if (filename.isNull())
  {
    return;
  }

  openSpecification(filename);
}

void MainWindow::loadTrace()
{
  if (!m_simulation)
  {
    return;
  }

  assert(m_ui.traceTable->isEnabled());

  QString filename = m_fileDialog.getOpenFileName("Open Trace", "Traces (*.trc)");
  if (filename.isNull())
  {
    return;
  }

  m_selected_state = 0;
  QMetaObject::invokeMethod(m_simulation, "load", Qt::BlockingQueuedConnection, Q_ARG(QString, filename));

  m_trace = m_simulation->trace();
  updateSimulation();
}

void MainWindow::saveTrace()
{
  if (!m_simulation)
  {
    return;
  }

  assert(m_ui.traceTable->isEnabled());

  QString filename = m_fileDialog.getSaveFileName("Save Trace", "Traces (*.trc)");
  if (filename.isNull())
  {
    return;
  }

  QMetaObject::invokeMethod(m_simulation, "save", Qt::BlockingQueuedConnection, Q_ARG(QString, filename));
}

void MainWindow::playTrace()
{
  if (!m_simulation)
  {
    return;
  }

  m_randomAnimation = false;
  m_animationTimer->start();

  m_ui.actionPlayTrace->setEnabled(false);
  m_ui.actionRandomPlay->setEnabled(false);
  m_ui.actionStop->setEnabled(true);
}

void MainWindow::randomPlay()
{
  if (!m_simulation)
  {
    return;
  }

  m_randomAnimation = true;
  m_animationTimer->start();

  m_ui.actionPlayTrace->setEnabled(false);
  m_ui.actionRandomPlay->setEnabled(false);
  m_ui.actionStop->setEnabled(true);
}

void MainWindow::stopPlay()
{
  m_animationTimer->stop();

  m_ui.actionPlayTrace->setEnabled(true);
  m_ui.actionRandomPlay->setEnabled(true);
  m_ui.actionStop->setEnabled(false);
}

void MainWindow::setPlayDelay()
{
  bool success;
  int newValue = QInputDialog::getInt(this, "Set Animation Delay", "Enter the time between two animation steps in milliseconds.", m_animationTimer->interval(), 0, INT_MAX, 1, &success);
  if (success)
  {
    m_animationTimer->setInterval(newValue);
  }
}

void MainWindow::updateSimulation()
{
  assert(m_simulation);
  if (update_simulation_is_running)  // Some activities, such as changing a selection, can be activated within updateSimulation.
                                     // While the simulation is being updated, it should not be updated again.
  {
    return;
  }
  update_simulation_is_running=true;

  int selectedState = static_cast<long long>(m_selected_state) < m_trace.size() ? m_selected_state : m_trace.size() - 1;

  int oldSize = m_ui.traceTable->rowCount();
  m_ui.traceTable->setRowCount(m_trace.size());
  for (int i = oldSize; i < m_trace.size(); i++)
  {
    m_ui.traceTable->setItem(i, 0, item());
    m_ui.traceTable->setItem(i, 1, item());
    m_ui.traceTable->setItem(i, 2, item());
  }
  for (int i = 0; i < m_trace.size(); i++)
  {
    m_ui.traceTable->item(i, 0)->setText(QString::number(i));
    if (i == 0)
    {
      m_ui.traceTable->item(i, 1)->setText("");
      m_ui.traceTable->item(i, 2)->setText(renderStateChange(Simulation::State(), m_trace[i].state));
    }
    else
    {
      m_ui.traceTable->item(i, 1)->setText(m_trace[i - 1].transitions[m_trace[i - 1].transitionNumber].action_or_probability); 
      m_ui.traceTable->item(i, 2)->setText(renderStateChange(m_trace[i - 1].state, m_trace[i].state));
    }
  }
  m_ui.traceTable->setCurrentCell(selectedState, 0);

  m_ui.transitionTable->setRowCount(0);
  m_ui.transitionTable->setRowCount(m_trace[selectedState].transitions.size());
  for (int i = 0; i < m_trace[selectedState].transitions.size(); i++)
  {
    m_ui.transitionTable->setItem(i, 0, item());
    m_ui.transitionTable->setItem(i, 1, item());

    m_ui.transitionTable->item(i, 0)->setText(m_trace[selectedState].transitions[i].action_or_probability); 
    m_ui.transitionTable->item(i, 1)->setText(renderStateChange(m_trace[selectedState].state, m_trace[selectedState].transitions[i].destination));
  }
  if (m_trace[selectedState].transitions.size() > 0)
  {
    m_ui.transitionTable->setCurrentCell(0, 0);
  }

  m_ui.transitionTable->resizeColumnToContents(1);
  m_ui.traceTable->resizeColumnToContents(2);
  m_ui.stateTable->resizeColumnToContents(1);

  assert(m_trace[selectedState].is_probabilistic || m_trace[selectedState].state.size() == m_ui.stateTable->rowCount());
  for (int i = 0; i < m_trace[selectedState].state.size(); i++)
  {
    if (!m_trace[selectedState].is_probabilistic)
    {
      m_ui.stateTable->item(i, 1)->setText(m_trace[selectedState].state[i]);
    }
  }
  update_simulation_is_running=false;
}

void MainWindow::stateSelected()
{
  QList<QTableWidgetSelectionRange> selection = m_ui.traceTable->selectedRanges();
  if (selection.size() > 0)
  {
    selectState(selection[0].topRow());
  }
}

void MainWindow::setAutoSelectProbability()
{
  assert(m_ui.traceTable->isEnabled());

  if (m_simulation)
  {
    QEventLoop loop;
    connect(m_simulation, SIGNAL(finished()), &loop, SLOT(quit()));
    QSemaphore semaphore;
    QMetaObject::invokeMethod(m_simulation, "enable_auto_select_probability", Qt::QueuedConnection, Q_ARG(bool, m_ui.actionAutoSelectProbability->isChecked()), Q_ARG(QSemaphore *, &semaphore));

    waitForResponse(&loop, &semaphore);

    m_trace = m_simulation->trace();
    updateSimulation();
  }
}

void MainWindow::openSpecification(QString filename)
{
  if (m_newSimulation && !m_newSimulation->initialized())
  {
    /* refresh the thread to allow running a new simulation */
    m_atermThread->terminate();
    m_atermThread->deleteLater();
    m_atermThread = new QThread;
    m_atermThread->start();
    m_newSimulation->deleteLater();
  }
  m_newSimulation = new Simulation(m_strategy);
  m_newSimulation->moveToThread(m_atermThread);
  connect(m_newSimulation, SIGNAL(initialisationDone()), this, SLOT(onInitializedSimulation()));
  QMetaObject::invokeMethod(m_newSimulation, "init", Qt::QueuedConnection, Q_ARG(QString, filename), Q_ARG(bool, m_do_not_use_dummies));
  m_ui.statusBar->showMessage("Initializing simulation...");
}

void MainWindow::onInitializedSimulation()
{
  if (m_simulation)
  {
    m_simulation->deleteLater();
  }
  m_simulation = m_newSimulation;
  m_selected_state = 0;

  QStringList parameters = m_simulation->parameters();
  m_ui.stateTable->setRowCount(0);
  m_ui.stateTable->setRowCount(parameters.size());
  for (int i = 0; i < parameters.size(); i++)
  {
    m_ui.stateTable->setItem(i, 0, item());
    m_ui.stateTable->setItem(i, 1, item());

    m_ui.stateTable->item(i, 0)->setText(parameters[i]);
  }
  m_ui.stateTable->resizeColumnToContents(0);

  setAutoSelectProbability();

  m_trace = m_simulation->trace();
  updateSimulation();

  m_ui.actionPlayTrace->setEnabled(true);
  m_ui.actionRandomPlay->setEnabled(true);
  m_ui.actionStop->setEnabled(false);

  m_ui.statusBar->clearMessage();
}

void MainWindow::selectState(unsigned long long state)
{
  if (!m_simulation)
  {
    return;
  }

  assert(m_ui.traceTable->isEnabled());

  if (state != m_selected_state)
  {
    m_selected_state = state;
    updateSimulation();
  }
}

void MainWindow::truncateTrace(int state, int column_on_screen)
{
  if (!m_simulation)
  {
    return;
  }

  assert(m_ui.traceTable->isEnabled());

  reset(state, column_on_screen<2);  // Double clicking in a left column restores this state with the probabilities not yet determined. 
                                     // Double clicking right selects the state. 
  updateSimulation();
}

void MainWindow::selectTransition(int transition)
{
  if (!m_simulation)
  {
    return;
  }

  assert(m_ui.traceTable->isEnabled());

  reset(m_selected_state, false);
  assert(static_cast<long long>(m_selected_state) < m_trace.size());
  bool original_state_is_probabilistic=m_trace[m_selected_state].is_probabilistic;
  select(transition);
  if (!original_state_is_probabilistic) 
  {
    m_selected_state++;
  }
  m_ui.traceTable->scrollToItem(m_ui.traceTable->item(m_selected_state, 1));
  m_ui.actionUndo_last->setEnabled(true);
  updateSimulation();
}

void MainWindow::animationStep()
{
  if (!m_simulation)
  {
    return;
  }

  if (!m_ui.traceTable->isEnabled())
  {
    m_animationTimer->stop();
    m_animationDisabled = true;
    return;
  }

  if (m_randomAnimation)
  {
    if (static_cast<long long>(m_selected_state) >= m_trace.size())
    {
      m_selected_state = m_trace.size() - 1;
    }

    bool original_state_is_probabilistic=m_trace[m_selected_state].is_probabilistic;

    if (!original_state_is_probabilistic && m_trace.last().transitions.size() == 0)
    {
      stopPlay();
      return;
    }

    auto_select_state_or_probability();

    if (!original_state_is_probabilistic)
    {
      m_selected_state++;   
    }
    updateSimulation();
  }
  else
  {
    if (static_cast<long long>(m_selected_state) + 1 < m_trace.size())
    {
      m_selected_state++;
      updateSimulation();
    }
    else
    {
      stopPlay();
    }
  }
}

void MainWindow::reset(unsigned long long selected_state, bool probabilistic)
{
  QMetaObject::invokeMethod(m_simulation, "reset", Qt::BlockingQueuedConnection, Q_ARG(unsigned long long, selected_state), Q_ARG(bool, probabilistic));

  m_trace = m_simulation->trace();
}

void MainWindow::select(unsigned long long transition)
{
  QEventLoop loop;
  connect(m_simulation, SIGNAL(finished()), &loop, SLOT(quit()));
  QSemaphore semaphore;
  QMetaObject::invokeMethod(m_simulation, "select", Qt::QueuedConnection, Q_ARG(unsigned long long, transition), Q_ARG(unsigned long long, m_selected_state), Q_ARG(QSemaphore *, &semaphore));

  waitForResponse(&loop, &semaphore);

  m_trace = m_simulation->trace();
  updateSimulation();
}

void MainWindow::auto_select_state_or_probability()
{
  QEventLoop loop;
  connect(m_simulation, SIGNAL(finished()), &loop, SLOT(quit()));
  QSemaphore semaphore;
  QMetaObject::invokeMethod(m_simulation, "auto_select_state_or_probability", Qt::QueuedConnection, Q_ARG(unsigned long long, m_selected_state), Q_ARG(QSemaphore *, &semaphore));

  waitForResponse(&loop, &semaphore);

  m_trace = m_simulation->trace();
  updateSimulation();
}

void MainWindow::waitForResponse(QEventLoop *eventLoop, QSemaphore *semaphore, int timeout)
{
  m_animationDisabled = false;

  if (!semaphore->tryAcquire(1, timeout))
  {
    m_ui.traceTable->setEnabled(false);
    m_ui.transitionTable->setEnabled(false);
    m_ui.actionOpen->setEnabled(false);
    m_ui.actionLoadTrace->setEnabled(false);
    m_ui.actionSaveTrace->setEnabled(false);
    m_ui.actionAutoSelectProbability->setEnabled(false);

    eventLoop->exec();

    m_ui.traceTable->setEnabled(true);
    m_ui.transitionTable->setEnabled(true);
    m_ui.actionOpen->setEnabled(true);
    m_ui.actionLoadTrace->setEnabled(true);
    m_ui.actionSaveTrace->setEnabled(true);
    m_ui.actionAutoSelectProbability->setEnabled(true);
  }

  if (m_animationDisabled)
  {
    m_animationTimer->start();
  }
}

void MainWindow::onLogOutput(QString /*level*/, QDateTime /*timestamp*/, QString /*message*/, QString formattedMessage)
{ 
  m_ui.statusBar->showMessage(formattedMessage, 5000);
} 

QString MainWindow::renderStateChange(Simulation::State source, Simulation::State destination)
{
  QStringList changes;
  for (int i = 0; i < destination.size(); i++)
  {
    if (destination[i].toStdString().find("Probabilistic state of size")!=std::string::npos) // String is found.
    {
      changes += destination[i];
    }
    else if (i >= source.size() || source[i] != destination[i])
    {
      if (!(destination[i] == "_") || m_ui.actionShowDontCaresInStateChanges->isChecked())
      {
        changes += m_simulation->parameters()[i] + " := " + destination[i];
      }
    }
  }
  return changes.join(", ");
}
