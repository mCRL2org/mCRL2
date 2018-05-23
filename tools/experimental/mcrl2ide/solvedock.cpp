#include "solvedock.h"

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>

SolveDock::SolveDock(QWidget *parent) : QDockWidget("Solve data expression", parent)
{
    /* define the widgets in this dock */
    expressionEntry = new QLineEdit();
    solveButton = new QPushButton("Solve");
    solveAbortButton = new QPushButton("Abort");
    solveResult = new QPlainTextEdit();

    /* set some parameters of the widgets */
    solveResult->setReadOnly(true);

    /* lay them out */
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(solveButton);
    buttonLayout->addWidget(solveAbortButton);

    QVBoxLayout *dockLayout = new QVBoxLayout();
    dockLayout->addWidget(expressionEntry);
    dockLayout->addLayout(buttonLayout);
    dockLayout->addWidget(solveResult);

    QWidget *innerDockWidget = new QWidget();
    innerDockWidget->setLayout(dockLayout);

    this->setWidget(innerDockWidget);
}
