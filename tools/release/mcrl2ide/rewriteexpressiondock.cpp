// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "rewriteexpressiondock.h"

#include "ui_RewriteExpressionDock.h"

RewriteExpressionDock::RewriteExpressionDock(mcrl2::gui::qt::CodeEditor* specificationEditor, QWidget* parent)
  : QDockWidget(parent),
    ui(new Ui::RewriteExpressionDock()),
    m_specificationEditor(specificationEditor)
{
  ui->setupUi(this);
  
  connect(ui->rewriteButton, SIGNAL(click()), this,
          SLOT(rewriteExpression()));
}

void RewriteExpressionDock::rewriteExpression()
{


}
