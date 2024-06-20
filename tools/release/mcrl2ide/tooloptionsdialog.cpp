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
    m_fileSystem(fileSystem),
    ui(new Ui::ToolOptionsDialog())
{
  ui->setupUi(this);

  
  //connect(ui->cancelButton, SIGNAL(click()), this, SLOT(close()));
  //connect(ui->saveButton, SIGNAL(click()), this, SLOT(saveToolOptions()));
  //connect(ui->resetButton, SIGNAL(click()), this, SLOT(resetToolOptions()));
}

void ToolOptionsDialog::saveToolOptions()
{
  m_fileSystem->setEnableJittyc(ui->enableJittyc->checkState() == Qt::CheckState::Checked);  
  m_fileSystem->setLinearisationMethod(mcrl2::lps::parse_lin_method(ui->linearsationComboBox->currentText().toStdString()));

  // Close the dialog after saving the options.
  close();
}

void ToolOptionsDialog::resetToolOptions()
{
  ui->enableJittyc->setCheckState(Qt::CheckState::Unchecked);
  ui->linearsationComboBox->setCurrentIndex(0);

  saveToolOptions();
}