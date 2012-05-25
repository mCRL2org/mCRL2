// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mainwindow.h
/// \brief Main Window of mCRL2xi used as GUI

#ifndef MCRL2XI_MAINWINDOW_H
#define MCRL2XI_MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

#include "ui_mainwindow.h"

#include "highlighter.h"
#include "documentwidget.h"
#include "documentmanager.h"
#include "parser.h"

#include "findreplacedialog.h"

class QTextEdit;

class MainWindow : public QMainWindow
{
    Q_OBJECT
  public:
    // Constructor
    MainWindow(QWidget *parent = 0);

    // Destructor
    ~MainWindow();

    // Save/Open operations
    bool saveDocument(DocumentWidget *document);
    void openDocument(QString fileName);

    void setRewriter(QString name);

  public slots:
    // Slots that receive the signals of the DocumentManager
    void formatDocument(DocumentWidget *document);
    void changeDocument(DocumentWidget *document);
    bool onCloseRequest(int index);

    // Slot to update the statusbar using the log widget
    void onLogOutput(QString level, QString hint, QDateTime timestamp, QString message, QString formattedMessage);

    // Slot to remove the highlights
    void textChanged();

  private:
    // UI variable
    Ui::MainWindow m_ui;

    // Parser object pointer
    Parser *m_parser;

    // Variable to save the state/perspective of the window
    QByteArray m_state;

    // Find and Replace dialog object pointer
    FindReplaceDialog *m_findReplaceDialog;

    // Current rewiter
    QString m_current_rewriter;

  private slots:
    // Slots for all menu items
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

    // Slots for the parser
    void onParse();
    void parsed();

    // Slots for the rewriter
    void onRewrite();
    void rewritten(QString output);

    // Slots for the solver
    void onSolve();
    void onSolveAbort();
    void solvedPart(QString output);
    void solved();

  protected:
    // Overridden function that asks to save modified specifications
    void closeEvent(QCloseEvent *event);
};

#endif // MCRL2XI_MAINWINDOW_H
