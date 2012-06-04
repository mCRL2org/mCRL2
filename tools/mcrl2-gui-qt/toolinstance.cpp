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

ToolInstance::ToolInstance(QString filename, ToolInformation information, QWidget *parent) :
  QWidget(parent),
  m_filename(filename),
  m_info(information),
  ui(new Ui::ToolInstance)
{
  ui->setupUi(this);
}

ToolInstance::~ToolInstance()
{
  delete ui;
}
