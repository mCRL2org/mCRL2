// Author: Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "mainwindow.h"
#include <QApplication>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMetaObject>
#include <QUrl>

MainWindow::MainWindow()
  : m_simulation(0)
{
  m_ui.setupUi(this);

  m_ui.traceTable->resizeColumnToContents(0);
  m_ui.traceTable->resizeColumnToContents(1);

  connect(m_ui.actionOpen, SIGNAL(triggered()), this, SLOT(open()));
  connect(m_ui.actionLoadTrace, SIGNAL(triggered()), this, SLOT(loadTrace()));
  connect(m_ui.actionSaveTrace, SIGNAL(triggered()), this, SLOT(saveTrace()));
  connect(m_ui.actionQuit, SIGNAL(triggered()), QApplication::instance(), SLOT(quit()));
  connect(m_ui.actionPlayTrace, SIGNAL(triggered()), this, SLOT(playTrace()));
  connect(m_ui.actionRandomPlay, SIGNAL(triggered()), this, SLOT(randomPlay()));
  connect(m_ui.actionStop, SIGNAL(triggered()), this, SLOT(stopPlay()));
  connect(m_ui.actionSetPlayDelay, SIGNAL(triggered()), this, SLOT(setPlayDelay()));
  connect(m_ui.actionContents, SIGNAL(triggered()), this, SLOT(contents()));
  connect(m_ui.actionAbout, SIGNAL(triggered()), this, SLOT(about()));

  connect(m_ui.traceTable, SIGNAL(itemSelectionChanged()), this, SLOT(stateSelected()));
  connect(m_ui.traceTable, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(truncateTrace(int)));
  connect(m_ui.transitionTable, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(selectTransition(int)));
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

void MainWindow::open()
{
  QString filename = QFileDialog::getOpenFileName(this, "Open Process Specification", "", "Process specifications (*.lps)");
  if (filename.isNull())
  {
    return;
  }

  Simulation *simulation = new Simulation(filename, mcrl2::data::rewrite_strategy());
  if (!simulation->initialized())
  {
    delete simulation;
    return;
  }

  if (m_simulation)
  {
    delete m_simulation;
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

  connect(m_simulation, SIGNAL(traceChanged(unsigned int)), this, SLOT(updateSimulation()));
  updateSimulation();
}

void MainWindow::loadTrace()
{
  if (!m_simulation)
  {
    return;
  }

  QString filename = QFileDialog::getOpenFileName(this, "Open Trace", "", "Traces (*.trc)");
  if (filename.isNull())
  {
    return;
  }

  m_selectedState = 0;
  QMetaObject::invokeMethod(m_simulation, "load", Qt::BlockingQueuedConnection, Q_ARG(QString, filename));
}

void MainWindow::saveTrace()
{
  if (!m_simulation)
  {
    return;
  }

  QString filename = QFileDialog::getSaveFileName(this, "Save Trace", "", "Traces (*.trc)");
  if (filename.isNull())
  {
    return;
  }

  QMetaObject::invokeMethod(m_simulation, "save", Qt::BlockingQueuedConnection, Q_ARG(QString, filename));
}

void MainWindow::playTrace()
{
}

void MainWindow::randomPlay()
{
}

void MainWindow::stopPlay()
{
}

void MainWindow::setPlayDelay()
{
}

void MainWindow::contents()
{
  QDesktopServices::openUrl(QUrl("http://mcrl2.org/release/user_manual/tools/lpsxsim.html"));
}

void MainWindow::about()
{
}

void MainWindow::updateSimulation()
{
  assert(m_simulation);

  Simulation::Trace trace = m_simulation->trace();

  int selectedState = m_selectedState < trace.size() ? m_selectedState : trace.size() - 1;

  int oldSize = m_ui.traceTable->rowCount();
  m_ui.traceTable->setRowCount(trace.size());
  for (int i = oldSize; i < trace.size(); i++)
  {
    m_ui.traceTable->setItem(i, 0, item());
    m_ui.traceTable->setItem(i, 1, item());
    m_ui.traceTable->setItem(i, 2, item());
  }
  for (int i = 0; i < trace.size(); i++)
  {
    m_ui.traceTable->item(i, 0)->setText(QString::number(i));
    if (i == 0)
    {
      m_ui.traceTable->item(i, 1)->setText("");
      m_ui.traceTable->item(i, 2)->setText(renderStateChange(Simulation::State(), trace[i].state));
    }
    else
    {
      m_ui.traceTable->item(i, 1)->setText(trace[i - 1].transitions[trace[i - 1].transitionNumber].action);
      m_ui.traceTable->item(i, 2)->setText(renderStateChange(trace[i - 1].state, trace[i].state));
    }
  }
  m_ui.traceTable->setRangeSelected(QTableWidgetSelectionRange(0, 0, trace.size() - 1, 2), false);
  if (selectedState == m_selectedState)
  {
    m_ui.traceTable->setRangeSelected(QTableWidgetSelectionRange(selectedState, 0, selectedState, 2), true);
  }

  m_ui.transitionTable->setRowCount(0);
  m_ui.transitionTable->setRowCount(trace[selectedState].transitions.size());
  for (int i = 0; i < trace[selectedState].transitions.size(); i++)
  {
    m_ui.transitionTable->setItem(i, 0, item());
    m_ui.transitionTable->setItem(i, 1, item());

    m_ui.transitionTable->item(i, 0)->setText(trace[selectedState].transitions[i].action);
    m_ui.transitionTable->item(i, 1)->setText(renderStateChange(trace[selectedState].state, trace[selectedState].transitions[i].destination));
  }

  assert(trace[selectedState].state.size() == m_ui.stateTable->rowCount());
  for (int i = 0; i < trace[selectedState].state.size(); i++)
  {
    m_ui.stateTable->item(i, 1)->setText(trace[selectedState].state[i]);
  }
}

void MainWindow::stateSelected()
{
  QList<QTableWidgetSelectionRange> selection = m_ui.traceTable->selectedRanges();
  if (selection.size() > 0)
  {
    int row = selection[0].topRow();
    if (row != m_selectedState)
    {
      m_selectedState = row;
      updateSimulation();
    }
  }
}

void MainWindow::truncateTrace(int state)
{
  if (!m_simulation)
  {
    return;
  }

  QMetaObject::invokeMethod(m_simulation, "reset", Q_ARG(unsigned int, state));
}

void MainWindow::selectTransition(int transition)
{
  if (!m_simulation)
  {
    return;
  }

  QMetaObject::invokeMethod(m_simulation, "reset", Q_ARG(unsigned int, m_selectedState));
  QMetaObject::invokeMethod(m_simulation, "select", Q_ARG(unsigned int, transition));
  m_selectedState++;
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
