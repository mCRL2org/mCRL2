// Author(s): Bas Ploeger, Carst Tankink, Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "infodialog.h"
#include <QTableWidget>
#include <QTableWidgetItem>

static inline QTableWidgetItem *item()
{
  QTableWidgetItem *item = new QTableWidgetItem();
  item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  return item;
}

static void initTable(QTableWidget *table)
{
  for (int i = 0; i < table->rowCount(); i++)
  {
    for (int j = 0; j < table->columnCount(); j++)
    {
      if (!table->item(i, j))
      {
        table->setItem(i, j, item());
      }
      else
      {
        table->item(i, j)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
      }
    }
  }
}

InfoDialog::InfoDialog(QWidget *parent):
  QDialog(parent)
{
  m_ui.setupUi(this);

  initTable(m_ui.ltsTable);
  initTable(m_ui.clusterTable);
  initTable(m_ui.stateTable);
}

void InfoDialog::setLTSInfo(int states, int transitions, int clusters, int ranks)
{
  m_ui.ltsTable->item(0, 1)->setText(QString::number(states));
  m_ui.ltsTable->item(1, 1)->setText(QString::number(transitions));
  m_ui.ltsTable->item(2, 1)->setText(QString::number(clusters));
  m_ui.ltsTable->item(3, 1)->setText(QString::number(ranks));
}

void InfoDialog::setNumMarkedStates(int markedStates)
{
  m_ui.ltsTable->item(4, 1)->setText(QString::number(markedStates));
}

void InfoDialog::setNumMarkedTransitions(int markedTransitions)
{
  m_ui.ltsTable->item(5, 1)->setText(QString::number(markedTransitions));
}

void InfoDialog::setParameterNames(QStringList names)
{
  m_ui.clusterTable->setRowCount(names.size() + 3);
  m_ui.stateTable->setRowCount(names.size());
  for (int i = 0; i < names.size(); i++)
  {
    m_ui.clusterTable->setItem(i + 3, 0, item());
    m_ui.clusterTable->setItem(i + 3, 1, item());
    m_ui.clusterTable->item(i + 3, 0)->setText(names[i]);

    m_ui.stateTable->setItem(i, 0, item());
    m_ui.stateTable->setItem(i, 1, item());
    m_ui.stateTable->item(i, 0)->setText(names[i]);
  }
}

void InfoDialog::setParameterValue(int parameter, QString value)
{
  m_ui.stateTable->item(parameter, 1)->setText(value);
}

void InfoDialog::setParameterValues(int parameter, QStringList values)
{
  m_ui.clusterTable->item(parameter + 3, 1)->setText(QString("{") + values.join(", ") + QString("}"));
}

void InfoDialog::setStatesInCluster(int states)
{
  m_ui.clusterTable->item(0, 1)->setText(QString::number(states));
}

void InfoDialog::resetParameterNames()
{
  m_ui.clusterTable->setRowCount(3);
  m_ui.stateTable->setRowCount(0);
}

void InfoDialog::resetParameterValues()
{
  for (int i = 0; i < m_ui.stateTable->rowCount(); i++)
  {
    m_ui.clusterTable->item(i + 3, 1)->setText("");
    m_ui.stateTable->item(i, 1)->setText("");
  }
}

