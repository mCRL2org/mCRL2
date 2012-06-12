// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "toolinstance.h"
#include "ui_toolinstance.h"

#include <QDebug>
#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QSpacerItem>


ToolInstance::ToolInstance(QString filename, ToolInformation information, QWidget *parent) :
  QWidget(parent),
  m_filename(filename),
  m_info(information),
  ui(new Ui::ToolInstance)
{
  ui->setupUi(this);

  QFileInfo fileInfo(filename);

  ui->lblDirectoryValue->setText(fileInfo.absoluteDir().absolutePath());
  ui->lblFileValue->setText(fileInfo.fileName());

  if (m_info.hasOutput())
  {
    QDir dir = fileInfo.absoluteDir();
    QString newfile(fileInfo.baseName().append(".out"));
    int filenr = 0;
    while(dir.exists(newfile))
    {
      filenr++;
      newfile = fileInfo.baseName().append("_%1.out").arg(filenr);
    }
    ui->edtFileOut->setText(newfile);
  }
  else
  {
    ui->lblFileOut->setVisible(false);
    ui->btnFileOutBrowse->setVisible(false);
    ui->edtFileOut->setVisible(false);
  }

  for (int i = 0; i < m_info.options.count(); i++)
  {
    ToolOption option = m_info.options.at(i);
    QCheckBox *cbOpt = new QCheckBox(option.nameLong, this);
    cbOpt->setChecked(option.standard);

    QLabel *lblOpt = new QLabel(option.description, this);
    lblOpt->setWordWrap(true);

    if (!option.hasArgument())
    {
      ui->frmOptions->addRow(cbOpt, lblOpt);
    }
    else
    {
      QVBoxLayout *lytOpt = new QVBoxLayout();
      lytOpt->setSpacing(10);

      lytOpt->addWidget(lblOpt);

      switch (option.argument.type)
      {
        case StringArgument:
        case IntegerArgument:
        case RealArgument:
        case BooleanArgument:
          {
            QHBoxLayout *lytArg = new QHBoxLayout();
            lytArg->setSpacing(6);

            QLabel *lblArg = new QLabel(option.argument.name, this);
            lblArg->setMinimumWidth(100);
            lytArg->addWidget(lblArg);

            QWidget *edtArg = NULL;

            switch (option.argument.type)
            {
              case IntegerArgument:
                edtArg = new QSpinBox(this);
                break;
              case RealArgument:
                edtArg = new QDoubleSpinBox(this);
                break;
              case BooleanArgument:
                edtArg = new QCheckBox("Enabled", this);
                break;
              case StringArgument:
              default:
                edtArg = new QLineEdit(this);
                break;
            }
            edtArg->setMinimumWidth(300);
            lytArg->addWidget(edtArg);

            QSpacerItem *spacer = new QSpacerItem(100, 20, QSizePolicy::Expanding);
            lytArg->addItem(spacer);
            lytOpt->addLayout(lytArg);
          }
          break;
        case FileArgument:
          {
            QHBoxLayout *lytArg = new QHBoxLayout();
            lytArg->setSpacing(6);

            QLabel *lblArg = new QLabel(option.argument.name, this);
            lblArg->setMinimumWidth(100);
            lytArg->addWidget(lblArg);

            QLineEdit *edtArg = new QLineEdit(this);
            edtArg->setMinimumWidth(300);
            lytArg->addWidget(edtArg);

            QPushButton *btnArg = new QPushButton("Browse", this);
            lytArg->addWidget(btnArg);

            QSpacerItem *spacer = new QSpacerItem(100, 20, QSizePolicy::Expanding);
            lytArg->addItem(spacer);
            lytOpt->addLayout(lytArg);
          }
        case EnumArgument:
          {
            QFormLayout *lytValues = new QFormLayout();
            lytValues->setSpacing(6);

            QButtonGroup *grpValues = new QButtonGroup(this);

            for (int j = 0; j < option.argument.values.count(); j++)
            {
              ToolValue val = option.argument.values.at(j);
              QRadioButton *rbVal = new QRadioButton(val.nameLong, this);
              rbVal->setChecked(val.standard);
              grpValues->addButton(rbVal);

              QLabel *lblVal = new QLabel(val.description);

              lytValues->addRow(rbVal, lblVal);
            }

            lytOpt->addLayout(lytValues);
          }
        default:
          break;
      }

      ui->frmOptions->addRow(cbOpt, lytOpt);
    }
  }
}

ToolInstance::~ToolInstance()
{
  delete ui;
}
