// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file findreplacedialog.h
/// \brief Simple Find and Replace dialog that can be used for multiple QTextEdit widgets

#ifndef MCRL2XI_FINDREPLACEDIALOG_H
#define MCRL2XI_FINDREPLACEDIALOG_H

#include <QDialog>
#include <QTextCursor>
#include <QTextEdit>

#include "ui_findreplacedialog.h"

class FindReplaceDialog : public QDialog {
    Q_OBJECT
  public:
    // Constuctor
    FindReplaceDialog(QWidget *parent = 0);

    // Setters
    void setTextEdit(QTextEdit *textEdit);

  public slots:
    // Find and Replace operations
    void find(bool down);
    void find();

    void findNext() { find(true); }
    void findPrev() { find(false); }

    void replace();
    void replaceAll();

  protected:
    // Error and Message setters
    void showError(const QString &error);
    void showMessage(const QString &message);

  protected slots:
    // Slot used to update if the search string changed
    void textToFindChanged();

  protected:
    // UI variable
    Ui::FindReplaceDialog m_ui;

    // QTextCursor used for a search
    QTextCursor m_textCursor;

    // QTextEdit currently used for the search
    QTextEdit *m_textEdit;
};

#endif // MCRL2XI_FINDREPLACEDIALOG_H
