// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2XI_DOCUMENTWIDGET_H
#define MCRL2XI_DOCUMENTWIDGET_H

#include <QWidget>
#include <QTextEdit>

#include "ui_documentwidget.h"

class DocumentWidget : public QWidget
{
    Q_OBJECT

  public:
    DocumentWidget(QWidget *parent = 0);

    QTextEdit* getEditor();
    QString getFileName();
    bool isModified();

    void openFile(QString fileName);
    void saveFile(QString fileName);

  private:

    Ui::DocumentWidget m_ui;

    QString m_filename;
};

#endif // MCRL2XI_DOCUMENTWIDGET_H
