// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "solvedock.h"

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>

SolveDock::SolveDock(QWidget* parent)
    : QDockWidget("Solve data expression", parent)
{
  /* define the widgets in this dock */
  expressionEntry = new QLineEdit();
  solveButton = new QPushButton("Solve");
  solveAbortButton = new QPushButton("Abort");
  solveResult = new QPlainTextEdit();

  /* set some parameters of the widgets */
  solveResult->setReadOnly(true);

  /* lay them out */
  QHBoxLayout* buttonLayout = new QHBoxLayout();
  buttonLayout->addWidget(solveButton);
  buttonLayout->addWidget(solveAbortButton);

  QVBoxLayout* dockLayout = new QVBoxLayout();
  dockLayout->addWidget(expressionEntry);
  dockLayout->addLayout(buttonLayout);
  dockLayout->addWidget(solveResult);

  QWidget* innerDockWidget = new QWidget();
  innerDockWidget->setLayout(dockLayout);

  this->setWidget(innerDockWidget);
}
