// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2XI_FINDREPLACEDIALOG_H
#define MCRL2XI_FINDREPLACEDIALOG_H

#include <QDialog>
#include <QTextCursor>
#include <QTextEdit>

#include "ui_findreplacedialog.h"

class FindReplaceDialog : public QDialog {
    Q_OBJECT
  public:
    FindReplaceDialog(QWidget *parent = 0);

    void setTextEdit(QTextEdit *textEdit);

  public slots:
    void find(bool down);
    void find();

    void findNext() { find(true); }
    void findPrev() { find(false); }

    void replace();
    void replaceAll();

  protected:

    void showError(const QString &error);
    void showMessage(const QString &message);

  protected slots:
    void textToFindChanged();

  protected:
    Ui::FindReplaceDialog m_ui;
    QTextCursor m_textCursor;
    QTextEdit *m_textEdit;
};

#endif // MCRL2XI_FINDREPLACEDIALOG_H
