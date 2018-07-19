// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef SOLVEDOCK_H
#define SOLVEDOCK_H

#include <QDockWidget>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

/**
 * @brief The SolveDock class defines the dock where the user can solve data
 *   expressions
 */
class SolveDock : public QDockWidget
{
  Q_OBJECT

  public:
  const Qt::DockWidgetArea defaultArea = Qt::RightDockWidgetArea;

  /**
   * @brief SolveDock Constructor
   * @param parent The parent of this widget
   */
  explicit SolveDock(QWidget* parent);
  ~SolveDock();

  private:
  QWidget* innerDockWidget;
  QVBoxLayout* dockLayout;
  QLineEdit* expressionEntry;
  QHBoxLayout* buttonLayout;
  QPushButton* solveButton;
  QPushButton* solveAbortButton;
  QPlainTextEdit* solveResult;
};

#endif // SOLVEDOCK_H
