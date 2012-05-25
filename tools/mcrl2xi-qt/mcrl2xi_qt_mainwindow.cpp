#include "mcrl2xi_qt_mainwindow.h"
#include "ui_mcrl2xi_qt_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{

    m_ui->setupUi(this);

    setupEditor();

    connect(m_ui->actOpenFile, SIGNAL(triggered()), this, SLOT(onOpenFile()));
    connect(m_ui->editor, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorChange()));

    setSelectedEditorInChildren();
    statusBar()->showMessage(QString::number(getEditor()->textCursor().position()));
}

MainWindow::~MainWindow()
{
}

void MainWindow::onOpenFile()
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
    return m_ui->editor;
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

void MainWindow::onCursorChange()
{
   statusBar()->showMessage(QString::number(getEditor()->textCursor().position()));
}

void MainWindow::setSelectedEditorInChildren()
{
   m_ui->dockWidgetParseAndTypeCheck->setSelectedEditor( m_ui->editor );
   m_ui->dockWidgetRewriter->setSelectedEditor( m_ui->editor );
   m_ui->dockWidgetSolver->setSelectedEditor( m_ui->editor );
}
