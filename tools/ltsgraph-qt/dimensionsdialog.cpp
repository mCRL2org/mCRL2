// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "dimensionsdialog.h"
#include "ui_dimensionsdialog.h"

DimensionsDialog::DimensionsDialog(QWidget *parent) :
  QDialog(parent),
  m_ui(new Ui::DimensionsDialog)
{
  m_ui->setupUi(this);
}

DimensionsDialog::~DimensionsDialog()
{
  delete m_ui;
}

int DimensionsDialog::resultWidth()
{
  return m_ui->spinWidth->value();
}

int DimensionsDialog::resultHeight()
{
  return m_ui->spinHeight->value();
}
