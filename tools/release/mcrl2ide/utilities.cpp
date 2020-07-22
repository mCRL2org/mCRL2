// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "utilities.h"

#include <QStandardItemModel>

EquivalenceComboBox::EquivalenceComboBox(QWidget* parent) : QComboBox(parent)
{
  /* add equivalences to the combobox, including some separators to indicate the
   *   use of abstraction */
  QStringList items;
  int secondSeparatorIndex = 2;
  items << "----- CHOOSE EQUIVALENCE -----"
        << "--- WITHOUT ABSTRACTION ---";
  for (mcrl2::lts::lts_equivalence equivalence :
       LTSEQUIVALENCESWITHOUTABSTRACTION)
  {
    items << getEquivalenceName(equivalence);
    secondSeparatorIndex++;
  }

  items << "--- WITH ABSTRACTION ---";
  for (mcrl2::lts::lts_equivalence equivalence : LTSEQUIVALENCESWITHABSTRACTION)
  {
    items << getEquivalenceName(equivalence);
  }

  this->addItems(items);

  /* Set separators to be unselectable */
  QStandardItemModel* model = qobject_cast<QStandardItemModel*>(this->model());
  model->item(0)->setFlags(model->item(0)->flags() & ~Qt::ItemIsEnabled);
  model->item(1)->setFlags(model->item(1)->flags() & ~Qt::ItemIsEnabled);
  model->item(secondSeparatorIndex)
      ->setFlags(model->item(secondSeparatorIndex)->flags() &
                 ~Qt::ItemIsEnabled);
}

mcrl2::lts::lts_equivalence EquivalenceComboBox::getSelectedEquivalence()
{
  return getEquivalenceFromName(this->currentText());
}

void EquivalenceComboBox::setSelectedEquivalence(
    mcrl2::lts::lts_equivalence equivalence)
{
  this->setCurrentText(getEquivalenceName(equivalence));
}

QString getEquivalenceName(mcrl2::lts::lts_equivalence equivalence,
                           bool fillSpaces)
{
  QString name = LTSEQUIVALENCENAMES.at(equivalence);
  if (fillSpaces)
  {
    name = name.replace(' ', '_');
  }
  return name;
}

mcrl2::lts::lts_equivalence getEquivalenceFromName(const QString& name)
{
  for (std::pair<mcrl2::lts::lts_equivalence, QString> item :
       LTSEQUIVALENCENAMES)
  {
    if (item.second == name)
    {
      return item.first;
    }
  }
  return mcrl2::lts::lts_eq_none;
}

void executeInformationBox(QWidget* parent, const QString& title,
                           const QString& message)
{
  QMessageBox msgBox(QMessageBox::Information, title, message, QMessageBox::Ok,
                     parent, Qt::WindowCloseButtonHint);
  msgBox.exec();
}

bool executeBinaryQuestionBox(QWidget* parent, const QString& title,
                              const QString& message)
{
  return QMessageBox::question(parent, title, message,
                               QMessageBox::Yes | QMessageBox::No) ==
         QMessageBox::Yes;
}

QMessageBox::StandardButton executeQuestionBox(QWidget* parent,
                                               const QString& title,
                                               const QString& message)
{
  return QMessageBox::question(parent, title, message,
                               QMessageBox::Yes | QMessageBox::No |
                                   QMessageBox::Cancel);
}
