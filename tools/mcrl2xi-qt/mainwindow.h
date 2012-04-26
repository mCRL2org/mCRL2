// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2XI_MAINWINDOW_H
#define MCRL2XI_MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

#include "ui_mainwindow.h"

#include "highlighter.h"
#include "documentwidget.h"
#include "documentmanager.h"
#include "parser.h"
#include "rewriter.h"

class QTextEdit;

class MainWindow : public QMainWindow
{
    Q_OBJECT
  public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool saveDocument(DocumentWidget *document);

  public slots:
    void formatDocument(DocumentWidget *document);
    void cleanupDocument(DocumentWidget *document);
    bool onCloseRequest(int index);
    void onLogOutput(QString level, QString hint, QDateTime timestamp, QString message, QString formattedMessage);

  private slots:
    void onNew();
    void onOpen();
    void onSave();
    void onSaveAs();
    void onExit();

    void onUndo();
    void onRedo();
    void onCut();
    void onCopy();
    void onPaste();
    void onDelete();
    void onSelectAll();

    void onFind();
    void onWrapMode();
    void onResetPerspective();

    void onAbout();

  private:
    Ui::MainWindow m_ui;
    Parser *m_parser;

  private slots:
    void onParse();
    void parsed();
    void onRewrite();
    void rewritten(QString output);

  protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MCRL2XI_MAINWINDOW_H
