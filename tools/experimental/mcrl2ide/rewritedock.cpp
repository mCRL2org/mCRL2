// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "rewritedock.h"

#include <QMainWindow>

RewriteDock::RewriteDock(QWidget* parent)
    : QDockWidget("Rewrite data expression", parent)
{
  /* define the widgets in this dock */
  expressionEntry = new QLineEdit();
  rewriteButton = new QPushButton("Rewrite");
  rewriteAbortButton = new QPushButton("Abort");
  rewriteResult = new QPlainTextEdit();

  /* set some parameters of the widgets */
  rewriteResult->setReadOnly(true);

  /* lay them out */
  buttonLayout = new QHBoxLayout();
  buttonLayout->addWidget(rewriteButton);
  buttonLayout->addWidget(rewriteAbortButton);

  dockLayout = new QVBoxLayout();
  dockLayout->addWidget(expressionEntry);
  dockLayout->addLayout(buttonLayout);
  dockLayout->addWidget(rewriteResult);

  innerDockWidget = new QWidget();
  innerDockWidget->setLayout(dockLayout);

  this->setWidget(innerDockWidget);
}

RewriteDock::~RewriteDock()
{
  delete rewriteResult;
  delete rewriteButton;
  delete rewriteAbortButton;
  delete buttonLayout;
  delete expressionEntry;
  delete dockLayout;
  delete innerDockWidget;
}
