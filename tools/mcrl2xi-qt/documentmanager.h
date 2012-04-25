// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

#include <QWidget>
#include <QTextEdit>

#include "ui_documentmanager.h"
#include "documentwidget.h"

class DocumentManager : public QWidget
{
    Q_OBJECT
    
  public:
    DocumentManager(QWidget *parent = 0);
    //~DocumentManager();

    void newFile();
    void openFile(QString fileName);
    void saveFile(QString fileName);

    int documentCount();
    DocumentWidget* getDocument(int index);
    DocumentWidget* findDocument(QString fileName);
    void closeDocument(int index);

    DocumentWidget* currentDocument();
    QString currentFileName();
    
  signals:
    void documentCreated(DocumentWidget *document);
    void documentSwitched(DocumentWidget *document);
    void tabCloseRequested(int index);

  private:
    DocumentWidget* createDocument(QString title);

    Ui::DocumentManager m_ui;

  private slots:
    void onCloseRequest(int index) { emit tabCloseRequested(index); }

  protected:
    void showEvent(QShowEvent *event);
};

#endif // DOCUMENTMANAGER_H
