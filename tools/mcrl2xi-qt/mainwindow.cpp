#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{

    m_ui.setupUi(this);

    connect(m_ui.actionOpen, SIGNAL(triggered()), this, SLOT(onOpen()));
    connect(m_ui.documentManager, SIGNAL(documentCreated(QTextEdit)), this, SLOT(setupEditor(QTextEdit)));
}

void MainWindow::onOpen()
{
    QString fileName(QFileDialog::getOpenFileName(this, tr("Open file"), QString(),
      tr("mCRL2 specification (*.mcrl2 *.txt )")));

    QFile file(fileName);
    if (file.open(QFile::ReadOnly | QFile::Text))
    {

    }
}

void MainWindow::setupEditor(QTextEdit *editor)
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);

    editor->setFont(font);

    highlighter = new Highlighter(editor->document());
}
