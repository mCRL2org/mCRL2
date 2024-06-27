// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "rewriteexpressiondock.h"

#include "ui_rewriteexpressiondock.h"

RewriteExpressionDock::RewriteExpressionDock(mcrl2::gui::qt::CodeEditor* specificationEditor, ProcessSystem* processSystem, QWidget* parent)
  : QDockWidget(parent),
    ui(new Ui::RewriteExpressionDock()),
    m_specificationEditor(specificationEditor),
    m_processSystem(processSystem)
{
  ui->setupUi(this);
  
  connect(ui->inputEdit, SIGNAL(returnPressed()), this,
          SLOT(rewriteExpression()));
  connect(ui->rewriteButton, SIGNAL(clicked()), this,
          SLOT(rewriteExpression()));
  connect(ui->cancelButton, SIGNAL(clicked()), this,
          SLOT(cancelRewrite()));
  connect(processSystem, SIGNAL(processFinished(int)), this,
          SLOT(actionRewriteResult(int)));
}

void RewriteExpressionDock::rewriteExpression()
{
  ui->rewriteButton->setEnabled(false);
  ui->resultText->setText("Rewrite in progress...");

  m_processId = m_processSystem->rewriteExpression(ui->inputEdit->text().toStdString());
}

void RewriteExpressionDock::actionRewriteResult(int processId)
{
  ui->rewriteButton->setEnabled(true);
  ui->resultText->setText("");

  ui->resultText->setText(m_processSystem->getResult(m_processId));
}

void RewriteExpressionDock::cancelRewrite()
{
  ui->rewriteButton->setEnabled(true);
  m_processSystem->abortProcess(m_processId);
}