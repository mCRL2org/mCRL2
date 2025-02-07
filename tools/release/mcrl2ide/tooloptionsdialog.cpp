// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "tooloptionsdialog.h"

#include "ui_tooloptionsdialog.h"

ToolOptionsDialog::ToolOptionsDialog(QWidget* parent, FileSystem* fileSystem)
  : QDialog(parent),
    ui(new Ui::ToolOptionsDialog()),
    m_fileSystem(fileSystem)
{
  ui->setupUi(this);

  updateToolOptions();

  connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(close()));
  connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(saveToolOptions()));
  connect(ui->resetButton, SIGNAL(clicked()), this, SLOT(resetToolOptions()));
}

void ToolOptionsDialog::updateToolOptions()
{
  // Load the existing tool options
  ui->enableJittyc->setCheckState(m_fileSystem->enableJittyc() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
  ui->linearsationComboBox->setCurrentText(QString::fromStdString(mcrl2::lps::print_lin_method(m_fileSystem->linearisationMethod())));  
  ui->enumerationAmountEdit->setText(QString::fromStdString(std::to_string(m_fileSystem->enumerationLimit())));  
}

void ToolOptionsDialog::saveToolOptions()
{
  m_fileSystem->setEnableJittyc(ui->enableJittyc->checkState() == Qt::CheckState::Checked);  
  m_fileSystem->setLinearisationMethod(mcrl2::lps::parse_lin_method(ui->linearsationComboBox->currentText().toStdString()));
  m_fileSystem->setEnumerationLimit(std::stoi(ui->enumerationAmountEdit->text().toStdString()));
    

  // Close the dialog after saving the options.
  close();
}

void ToolOptionsDialog::resetToolOptions()
{
  ui->enableJittyc->setCheckState(Qt::CheckState::Unchecked);
  ui->linearsationComboBox->setCurrentIndex(0);
  ui->enumerationAmountEdit->setText("10");

  saveToolOptions();
}