// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "consoledock.h"

#include <QMainWindow>

ConsoleDock::ConsoleDock(QWidget* parent) : QDockWidget("Console", parent)
{
  consoleTabs = new QTabWidget();

  /* define the console widgets and set them to read only */
  parsingConsole = new QPlainTextEdit();
  LTSCreationConsole = new QPlainTextEdit();
  verificationConsole = new QPlainTextEdit();

  parsingConsole->setReadOnly(true);
  LTSCreationConsole->setReadOnly(true);
  verificationConsole->setReadOnly(true);

  /* lay them out */
  consoleTabs->addTab(parsingConsole, "Parsing");
  consoleTabs->addTab(LTSCreationConsole, "LTS creation");
  consoleTabs->addTab(verificationConsole, "Verification");

  this->setWidget(consoleTabs);
}

void ConsoleDock::setConsoleTab(ConsoleTab tab)
{
  switch (tab)
  {
  case Parsing:
    consoleTabs->setCurrentIndex(0);
  case LTSCreation:
    consoleTabs->setCurrentIndex(1);
  case Verification:
    consoleTabs->setCurrentIndex(2);
  }
}

void ConsoleDock::logToParsingConsole()
{
  logToConsole(parsingConsole, qobject_cast<QProcess*>(sender()));
}

void ConsoleDock::logToLTSCreationConsole()
{
  logToConsole(LTSCreationConsole, qobject_cast<QProcess*>(sender()));
}

void ConsoleDock::logToVerificationConsole()
{
  logToConsole(verificationConsole, qobject_cast<QProcess*>(sender()));
}

void ConsoleDock::logToConsole(QPlainTextEdit* console, QProcess* process)
{
  /* print to console line by line */
  process->setReadChannel(QProcess::StandardError);
  while (process->canReadLine())
  {
    console->appendPlainText(process->readLine());
  }
}

void ConsoleDock::writeToConsole(ConsoleTab tab, QString output)
{
  QPlainTextEdit* console;

  switch (tab)
  {
  case Parsing:
    console = parsingConsole;
  case LTSCreation:
    console = LTSCreationConsole;
  case Verification:
    console = verificationConsole;
  }

  console->appendPlainText(output);
}
