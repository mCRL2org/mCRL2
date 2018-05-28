#include "consoledock.h"

#include <QMainWindow>

ConsoleDock::ConsoleDock(QWidget *parent) : QDockWidget("Console", parent)
{
    consoleTabs = new QTabWidget();

    /* define the console widgets and set them to read only*/
    parseConsole = new QPlainTextEdit();
    LTSConsole = new QPlainTextEdit();
    verificationConsole = new QPlainTextEdit();

    parseConsole->setReadOnly(true);
    LTSConsole->setReadOnly(true);
    verificationConsole->setReadOnly(true);

    /* lay them out */
    consoleTabs->addTab(parseConsole, "Parsing");
    consoleTabs->addTab(LTSConsole, "LTS creation");
    consoleTabs->addTab(verificationConsole, "Verification");

    this->setWidget(consoleTabs);
}
