// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file documentwidget.h
/// \brief Widget containing a single QTextEdit for a document with built-in file operations

#ifndef MCRL2XI_DOCUMENTWIDGET_H
#define MCRL2XI_DOCUMENTWIDGET_H

#include <QWidget>
#include <QTextEdit>

#include "ui_documentwidget.h"

class DocumentWidget : public QWidget
{
    Q_OBJECT

  public:
    // Constructor
    DocumentWidget(QWidget *parent = 0);

    // Getters
    QTextEdit* getEditor();
    QString getFileName();
    bool isModified();

    // File operations
    void openFile(QString fileName);
    void saveFile(QString fileName);

  private:
    // UI variable
    Ui::DocumentWidget m_ui;

    // Filename of the document, NULL-string if none
    QString m_filename;
};

#endif // MCRL2XI_DOCUMENTWIDGET_H
