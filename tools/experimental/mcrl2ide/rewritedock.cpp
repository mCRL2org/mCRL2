#include "rewritedock.h"

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>

RewriteDock::RewriteDock(QWidget *parent) : QDockWidget("Rewrite data expression", parent)
{
    /* define the widgets in this dock */
    expressionEntry = new QLineEdit();
    rewriteButton = new QPushButton("Rewrite");
    rewriteAbortButton = new QPushButton("Abort");
    rewriteResult = new QPlainTextEdit();

    /* set some parameters of the widgets */
    rewriteResult->setReadOnly(true);

    /* lay them out */
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(rewriteButton);
    buttonLayout->addWidget(rewriteAbortButton);

    QVBoxLayout *dockLayout = new QVBoxLayout();
    dockLayout->addWidget(expressionEntry);
    dockLayout->addLayout(buttonLayout);
    dockLayout->addWidget(rewriteResult);

    QWidget *innerDockWidget = new QWidget();
    innerDockWidget->setLayout(dockLayout);

    this->setWidget(innerDockWidget);
}
