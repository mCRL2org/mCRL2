#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{

    m_ui.setupUi(this);

    setupEditor();

    connect(m_ui.actionOpen, SIGNAL(triggered()), this, SLOT(onOpen()));

    setSelectedEditorInChildren();
    statusBar()->showMessage(QString::number(getEditor()->textCursor().position()));
}

void MainWindow::onOpen()
{
    QString fileName(QFileDialog::getOpenFileName(this, tr("Open file"), QString(),
      tr("mCRL2 specification (*.mcrl2 *.txt )")));

    QFile file(fileName);
    if (file.open(QFile::ReadOnly | QFile::Text))
    {
       getEditor()->setPlainText(file.readAll());
    }
}

QTextEdit *MainWindow::getEditor()
{
    return m_ui.editor;
}

void MainWindow::setupEditor()
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);

    getEditor()->setFont(font);
    getEditor()->clear();

    highlighter = new Highlighter(getEditor()->document());
}

void MainWindow::setSelectedEditorInChildren()
{
//   m_ui.dockWidgetParseAndTypeCheck->setSelectedEditor( m_ui.editor );
//   m_ui.dockWidgetRewriter->setSelectedEditor( m_ui.editor );
//   m_ui.dockWidgetSolver->setSelectedEditor( m_ui.editor );
}
