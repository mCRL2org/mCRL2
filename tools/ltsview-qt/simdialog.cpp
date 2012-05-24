// Author(s): Bas Ploeger, Carst Tankink, Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "simdialog.h"
#include <QList>

SimDialog::SimDialog(QWidget *parent)
  : QDialog(parent),
    m_simulation(0)
{
  m_ui.setupUi(this);

  connect(m_ui.start, SIGNAL(clicked()), this, SLOT(start()));
  connect(m_ui.stop, SIGNAL(clicked()), this, SLOT(stop()));
  connect(m_ui.backtrace, SIGNAL(clicked()), this, SLOT(backtrace()));
  connect(m_ui.reset, SIGNAL(clicked()), this, SLOT(reset()));
  connect(m_ui.trigger, SIGNAL(clicked()), this, SLOT(trigger()));
  connect(m_ui.undo, SIGNAL(clicked()), this, SLOT(undo()));
  connect(m_ui.transitionTable, SIGNAL(itemSelectionChanged()), this, SLOT(select()));
  connect(m_ui.transitionTable, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(trigger()));

  changed();
}

void SimDialog::setSimulation(Simulation *simulation)
{
  m_simulation = simulation;
  connect(m_simulation, SIGNAL(changed()), this, SLOT(changed()));
  connect(m_simulation, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
  changed();
}

static inline QTableWidgetItem *item()
{
  QTableWidgetItem *item = new QTableWidgetItem();
  item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  return item;
}

void SimDialog::changed()
{
  if (!m_simulation)
  {
    m_ui.start->setEnabled(false);
    m_ui.stop->setEnabled(false);
    m_ui.backtrace->setEnabled(false);
    m_ui.reset->setEnabled(false);
    m_ui.undo->setEnabled(false);
    m_ui.transitionTable->setRowCount(0);
  }
  else if (!m_simulation->getStarted())
  {
    m_ui.start->setEnabled(true);
    m_ui.stop->setEnabled(false);
    m_ui.backtrace->setEnabled(false);
    m_ui.reset->setEnabled(false);
    m_ui.undo->setEnabled(false);
    m_ui.transitionTable->setRowCount(0);
  }
  else
  {
    m_ui.start->setEnabled(false);
    m_ui.stop->setEnabled(true);
    m_ui.backtrace->setEnabled(true);
    m_ui.reset->setEnabled(true);
    m_ui.undo->setEnabled(!m_simulation->getTransHis().empty());

    m_ui.transitionTable->setRowCount(0);
    m_ui.transitionTable->setRowCount(m_simulation->getPosTrans().size());
    State *state = m_simulation->getCurrState();
    for (size_t i = 0; i < m_simulation->getPosTrans().size(); i++)
    {
      m_ui.transitionTable->setItem(i, 0, item());
      m_ui.transitionTable->setItem(i, 1, item());
      m_ui.transitionTable->item(i, 0)->setText(QString::fromStdString(m_simulation->getLTS()->getLabel(m_simulation->getPosTrans()[i]->getLabel())));

      State *destination = m_simulation->getPosTrans()[i]->getEndState();
      QStringList assignments;
      for (size_t j = 0; j < m_simulation->getLTS()->getNumParameters(); j++)
      {
        if (m_simulation->getLTS()->getStateParameterValue(state, j) != m_simulation->getLTS()->getStateParameterValue(destination, j))
        {
          assignments += QString::fromStdString(m_simulation->getLTS()->getParameterName(j)) + QString(":=") + QString::fromStdString(m_simulation->getLTS()->getStateParameterValueStr(destination, j));
        }
      }
      m_ui.transitionTable->item(i, 1)->setText(assignments.join(", "));
    }

  }

  selectionChanged();
}

void SimDialog::selectionChanged()
{
  if (!m_simulation)
  {
    m_ui.trigger->setEnabled(false);
  }
  else if (!m_simulation->getStarted())
  {
    m_ui.trigger->setEnabled(false);
  }
  else
  {
    m_ui.transitionTable->setRangeSelected(QTableWidgetSelectionRange(0, 0, m_ui.transitionTable->rowCount() - 1, 1), false);
    int selectedTransition = m_simulation->getChosenTransi();
    if (selectedTransition == -1)
    {
      m_ui.trigger->setEnabled(false);
    }
    else
    {
      m_ui.transitionTable->setRangeSelected(QTableWidgetSelectionRange(selectedTransition, 0, selectedTransition, 1), true);
      m_ui.trigger->setEnabled(true);
    }
  }
}

void SimDialog::start()
{
  m_simulation->start();
}

void SimDialog::stop()
{
  m_simulation->stop();
}

void SimDialog::backtrace()
{
  m_simulation->getLTS()->generateBackTrace();
}

void SimDialog::reset()
{
  m_simulation->resetSim();
}

void SimDialog::trigger()
{
  m_simulation->followTrans();
}

void SimDialog::undo()
{
  m_simulation->undoStep();
}

void SimDialog::select()
{
  QList<QTableWidgetSelectionRange> selection = m_ui.transitionTable->selectedRanges();
  if (selection.size() > 0)
  {
    m_simulation->chooseTrans(selection[0].topRow());
  }
}
