// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file documentmanager.h
/// \brief Manager for multiple DocumentWidget elements

#ifndef MCRL2XI_DOCUMENTMANAGER_H
#define MCRL2XI_DOCUMENTMANAGER_H

#include <QWidget>
#include <QTextEdit>

#include "ui_documentmanager.h"
#include "documentwidget.h"

class DocumentManager : public QWidget
{
    Q_OBJECT
    
  public:
    // Constructor
    DocumentManager(QWidget *parent = 0);

    // File operations
    void newFile();
    void openFile(QString fileName);
    void saveFile(QString fileName);

    // Document operators
    int documentCount();
    int indexOf(DocumentWidget *document);
    DocumentWidget *getDocument(int index);
    DocumentWidget *findDocument(QString fileName);
    void closeDocument(int index);

    // Current information
    DocumentWidget *currentDocument();
    QString currentFileName();
    
  signals:
    // Document signals
    void documentCreated(DocumentWidget *document);
    void documentChanged(DocumentWidget *document);
    void documentClosed(DocumentWidget *document);
    void tabCloseRequested(int index);

  private:
    // Create new document/tab
    DocumentWidget *createDocument(QString title);

    // UI variable
    Ui::DocumentManager m_ui;

  private slots:
    // Slots to redirect signals of the QTabWidget
    void onCloseRequest(int index) { emit tabCloseRequested(index); }
    void onCurrentChanged(int index) { emit documentChanged(getDocument(index)); }

  protected:
    // Overridden function used to guarantee there is at least 1 document at all times
    void showEvent(QShowEvent *event);
};

#endif // MCRL2XI_DOCUMENTMANAGER_H
