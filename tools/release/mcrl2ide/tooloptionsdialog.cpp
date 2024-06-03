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

ToolOptionsDialog::ToolOptionsDialog(QWidget* parent)
  : QDialog(parent),
    ui(new Ui::ToolOptionsDialog())
{
  ui->setupUi(this);
}