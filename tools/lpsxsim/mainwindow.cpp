// Author: Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "mainwindow.h"
#include <QApplication>
#include <QEventLoop>
#include <QInputDialog>
#include <QMetaObject>
#include <QUrl>
#include <QSettings>
#include <climits>

MainWindow::MainWindow()
  : m_simulation(0),
    m_animationTimer(new QTimer(this)),
    m_fileDialog("", this)
{
  m_ui.setupUi(this);

  m_ui.traceTable->resizeColumnToContents(0);
  m_ui.traceTable->resizeColumnToContents(1);

  connect(m_ui.actionOpen, SIGNAL(triggered()), this, SLOT(openSpecification()));
  connect(m_ui.actionLoadTrace, SIGNAL(triggered()), this, SLOT(loadTrace()));
  connect(m_ui.actionSaveTrace, SIGNAL(triggered()), this, SLOT(saveTrace()));
  connect(m_ui.actionQuit, SIGNAL(triggered()), QApplication::instance(), SLOT(quit()));
  connect(m_ui.actionPlayTrace, SIGNAL(triggered()), this, SLOT(playTrace()));
  connect(m_ui.actionRandomPlay, SIGNAL(triggered()), this, SLOT(randomPlay()));
  connect(m_ui.actionStop, SIGNAL(triggered()), this, SLOT(stopPlay()));
  connect(m_ui.actionSetPlayDelay, SIGNAL(triggered()), this, SLOT(setPlayDelay()));
  connect(m_ui.actionEnableTauPrioritisation, SIGNAL(toggled(bool)), this, SLOT(setTauPrioritization()));
  connect(m_ui.actionOutput, SIGNAL(toggled(bool)), m_ui.dockWidget, SLOT(setVisible(bool)));

  connect(m_ui.traceTable, SIGNAL(itemSelectionChanged()), this, SLOT(stateSelected()));
  connect(m_ui.traceTable, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(truncateTrace(int)));
  connect(m_ui.transitionTable, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(selectTransition(int)));

  connect(m_ui.dockWidget->widget(), SIGNAL(logMessage(QString, QString, QDateTime, QString, QString)), this, SLOT(onLogOutput(QString, QString, QDateTime, QString, QString)));

  m_animationTimer->setInterval(1000);
  connect(m_animationTimer, SIGNAL(timeout()), this, SLOT(animationStep()));

  m_ui.actionPlayTrace->setEnabled(false);
  m_ui.actionRandomPlay->setEnabled(false);
  m_ui.actionStop->setEnabled(false);

  QSettings settings("mCRL2", "LpsXSim");
  restoreGeometry(settings.value("geometry").toByteArray());
  restoreState(settings.value("windowState").toByteArray());

  m_ui.actionOutput->setChecked(!m_ui.dockWidget->isHidden());
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

  m_selectedState = 0;
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

  int selectedState = m_selectedState < m_trace.size() ? m_selectedState : m_trace.size() - 1;

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
#ifdef __APPLE_CC__
#ifdef __GNUG__
      /* This is a workaround for a compiler bug in                          *
       *   i686-apple-darwin9-g++-4.0.1 (GCC) 4.0.1 (Apple Inc. build 5493)  *
       * TODO: Remove this code when this compiler is no longer supported    *
       *       by mCRL2                                                      */
      mCRL2log(debug) << (i - 1) << std::endl;
#endif // __GNUG__
#endif // __APPLE_CC__
      m_ui.traceTable->item(i, 1)->setText(m_trace[i - 1].transitions[m_trace[i - 1].transitionNumber].action);
      m_ui.traceTable->item(i, 2)->setText(renderStateChange(m_trace[i - 1].state, m_trace[i].state));
    }
  }
  m_ui.traceTable->setRangeSelected(QTableWidgetSelectionRange(0, 0, m_trace.size() - 1, 2), false);
  if (selectedState == m_selectedState)
  {
    m_ui.traceTable->setRangeSelected(QTableWidgetSelectionRange(selectedState, 0, selectedState, 2), true);
  }

  m_ui.transitionTable->setRowCount(0);
  m_ui.transitionTable->setRowCount(m_trace[selectedState].transitions.size());
  for (int i = 0; i < m_trace[selectedState].transitions.size(); i++)
  {
    m_ui.transitionTable->setItem(i, 0, item());
    m_ui.transitionTable->setItem(i, 1, item());

    m_ui.transitionTable->item(i, 0)->setText(m_trace[selectedState].transitions[i].action);
    m_ui.transitionTable->item(i, 1)->setText(renderStateChange(m_trace[selectedState].state, m_trace[selectedState].transitions[i].destination));
  }

  assert(m_trace[selectedState].state.size() == m_ui.stateTable->rowCount());
  for (int i = 0; i < m_trace[selectedState].state.size(); i++)
  {
    m_ui.stateTable->item(i, 1)->setText(m_trace[selectedState].state[i]);
  }
}

void MainWindow::stateSelected()
{
  QList<QTableWidgetSelectionRange> selection = m_ui.traceTable->selectedRanges();
  if (selection.size() > 0)
  {
    selectState(selection[0].topRow());
  }
}

void MainWindow::setTauPrioritization()
{
  assert(m_ui.traceTable->isEnabled());

  if (m_simulation)
  {
    QEventLoop loop;
    connect(m_simulation, SIGNAL(finished()), &loop, SLOT(quit()));
    QSemaphore semaphore;
    QMetaObject::invokeMethod(m_simulation, "enable_tau_prioritization", Qt::QueuedConnection, Q_ARG(bool, m_ui.actionEnableTauPrioritisation->isChecked()), Q_ARG(QSemaphore *, &semaphore));

    waitForResponse(&loop, &semaphore);

    m_trace = m_simulation->trace();
    updateSimulation();
  }
}

void MainWindow::openSpecification(QString filename)
{
  Simulation *simulation = new Simulation(filename, mcrl2::data::rewrite_strategy());
  if (!simulation->initialized())
  {
    simulation->deleteLater();
    return;
  }

  if (m_simulation)
  {
    m_simulation->deleteLater();
  }
  m_simulation = simulation;
  m_selectedState = 0;

  QStringList parameters = m_simulation->parameters();
  m_ui.stateTable->setRowCount(0);
  m_ui.stateTable->setRowCount(parameters.size());
  for (int i = 0; i < parameters.size(); i++)
  {
    m_ui.stateTable->setItem(i, 0, item());
    m_ui.stateTable->setItem(i, 1, item());

    m_ui.stateTable->item(i, 0)->setText(parameters[i]);
  }

  setTauPrioritization();

  m_trace = m_simulation->trace();
  updateSimulation();

  m_ui.actionPlayTrace->setEnabled(true);
  m_ui.actionRandomPlay->setEnabled(true);
  m_ui.actionStop->setEnabled(false);
}

void MainWindow::selectState(int state)
{
  if (!m_simulation)
  {
    return;
  }

  assert(m_ui.traceTable->isEnabled());

  if (state != m_selectedState)
  {
    m_selectedState = state;
    updateSimulation();
  }
}

void MainWindow::truncateTrace(int state)
{
  if (!m_simulation)
  {
    return;
  }

  assert(m_ui.traceTable->isEnabled());

  reset(state);
}

void MainWindow::selectTransition(int transition)
{
  if (!m_simulation)
  {
    return;
  }

  assert(m_ui.traceTable->isEnabled());

  reset(m_selectedState);
  m_selectedState++;
  select(transition);
  m_ui.traceTable->scrollToItem(m_ui.traceTable->item(m_selectedState, 1));
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
    if (m_selectedState >= m_trace.size())
    {
      m_selectedState = m_trace.size() - 1;
    }

    if (m_trace.last().transitions.size() == 0)
    {
      stopPlay();
      return;
    }

    if (m_selectedState == m_trace.size() - 1)
    {
      m_selectedState++;
    }
    select(qrand() % m_trace.last().transitions.size());
  }
  else
  {
    if (m_selectedState + 1 < m_trace.size())
    {
      m_selectedState++;
      updateSimulation();
    }
    else
    {
      stopPlay();
    }
  }
}

void MainWindow::reset(unsigned int selectedState)
{
  QMetaObject::invokeMethod(m_simulation, "reset", Qt::BlockingQueuedConnection, Q_ARG(unsigned int, selectedState));

  m_trace = m_simulation->trace();
  updateSimulation();
}

void MainWindow::select(unsigned int transition)
{
  QEventLoop loop;
  connect(m_simulation, SIGNAL(finished()), &loop, SLOT(quit()));
  QSemaphore semaphore;
  QMetaObject::invokeMethod(m_simulation, "select", Qt::QueuedConnection, Q_ARG(unsigned int, transition), Q_ARG(QSemaphore *, &semaphore));

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
    m_ui.actionEnableTauPrioritisation->setEnabled(false);

    eventLoop->exec();

    m_ui.traceTable->setEnabled(true);
    m_ui.transitionTable->setEnabled(true);
    m_ui.actionOpen->setEnabled(true);
    m_ui.actionLoadTrace->setEnabled(true);
    m_ui.actionSaveTrace->setEnabled(true);
    m_ui.actionEnableTauPrioritisation->setEnabled(true);
  }

  if (m_animationDisabled)
  {
    m_animationTimer->start();
  }
}

void MainWindow::onLogOutput(QString /*level*/, QString /*hint*/, QDateTime /*timestamp*/, QString /*message*/, QString formattedMessage)
{
  m_ui.statusBar->showMessage(formattedMessage, 5000);
}

QString MainWindow::renderStateChange(Simulation::State source, Simulation::State destination)
{
  QStringList changes;
  for (int i = 0; i < destination.size(); i++)
  {
    if (i >= source.size() || source[i] != destination[i])
    {
      if (destination[i] == "_" && !m_ui.actionShowDontCaresInStateChanges->isChecked())
      {
        continue;
      }
      changes += m_simulation->parameters()[i] + " := " + destination[i];
    }
  }
  return changes.join(", ");
}
