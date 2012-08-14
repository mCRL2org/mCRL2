// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "dofdialog.h"
#include "ui_dofdialog.h"
#include <QDebug>

DofDialog::DofDialog(QWidget *parent) :
  QDialog(parent),
  m_currentGraph(0),
  m_currentShape(0)
{
  m_ui.setupUi(this);

  m_dofNames << "Horizontal position" <<
                "Vertical position" <<
                "Width" <<
                "Height" <<
                "Rotation" <<
                "Color" <<
                "Opacity" <<
                "Text";

  m_comboBoxes.clear();
  for (int i = 0; i < m_dofNames.size(); i++)
  {
    QComboBox* comboBox = new QComboBox(this);
    comboBox->addItem("None");
    m_ui.formLayout->addRow(m_dofNames[i], comboBox);

    m_comboBoxes.insert(m_dofNames[i], comboBox);
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(attributeSelected(int)));
  }

}

void DofDialog::refresh()
{

  for (int i = 0; i < m_dofNames.count(); i++)
  {
    QString dofName = m_dofNames[i];
    QComboBox* comboBox = m_comboBoxes[dofName];
    comboBox->clear();
    comboBox->addItem("None");
    if (m_currentGraph != 0)
    {
      Attribute* currentAttribute = 0;
      DOF* dof = currentDof(dofName);
      if (dof != 0)
      {
        currentAttribute = dof->getAttribute();
      }

      for (size_t j = 0; j < m_currentGraph->getSizeAttributes(); j++)
      {
        Attribute* attribute = m_currentGraph->getAttribute(j);
        comboBox->addItem(attribute->name());
        if (currentAttribute == attribute)
        {
          comboBox->setCurrentIndex(comboBox->count()-1);
        }
      }
    }
  }
}

void DofDialog::attributeSelected(int index)
{
  if (m_currentGraph != 0)
  {
    QObject* sender = QObject::sender();
    QComboBox* comboBox = dynamic_cast<QComboBox*>(sender);
    if (comboBox != 0)
    {
      QString dofName = m_comboBoxes.key(comboBox, QString());
      if (!dofName.isNull())
      {
        DOF* dof = currentDof(dofName);
        if (dof != 0)
        {
          dof->setAttribute(m_currentGraph->getAttribute(index-1));
        }
      }
    }
  }
}

DOF *DofDialog::currentDof(QString name)
{
  if (m_currentShape == 0)
  {
    return 0;
  }
  int index = m_dofNames.indexOf(name);
  switch(index)
  {
    case 0:
      return m_currentShape->getDOFXCtr();
      break;
    case 1:
      return m_currentShape->getDOFYCtr();
      break;
    case 2:
      return m_currentShape->getDOFWth();
      break;
    case 3:
      return m_currentShape->getDOFHgt();
      break;
    case 4:
      return m_currentShape->getDOFAgl();
      break;
    case 5:
      return m_currentShape->getDOFOpa();
      break;
    case 6:
      return m_currentShape->getDOFCol();
      break;
    default:
      return 0;
      break;
  }
}
