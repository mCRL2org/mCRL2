// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef REWRITEEXPRESSION_H
#define REWRITEEXPRESSION_H

#include <QDockWidget>

#include "mcrl2/gui/codeeditor.h"
#include "processsystem.h"

namespace Ui
{
  class RewriteExpressionDock;
};

/**
 * @brief The AddEditPropertyDialog class defines the dialog used to add or edit
 *   a property
 */
class RewriteExpressionDock : public QDockWidget
{
  Q_OBJECT

public:
    RewriteExpressionDock(mcrl2::gui::qt::CodeEditor* specificationEditor, ProcessSystem* processSystem, QWidget* parent);


private slots:
  /**
   * Rewrites the current expression.
   */
  void rewriteExpression();

  /**
   * Rewrites the current expression.
   */
  void actionRewriteResult(int processId);

  /**
   * Cancels the rewriting of an expression.
   */
  void cancelRewrite();

private:
  Ui::RewriteExpressionDock* ui;

  mcrl2::gui::qt::CodeEditor* m_specificationEditor = nullptr;
  ProcessSystem* m_processSystem = nullptr;
  int m_processId = 0;
};

#endif // REWRITEEXPRESSION_H