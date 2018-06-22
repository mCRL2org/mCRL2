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
  ltsCreationConsole = new QPlainTextEdit();
  verificationConsole = new QPlainTextEdit();

  parsingConsole->setReadOnly(true);
  ltsCreationConsole->setReadOnly(true);
  verificationConsole->setReadOnly(true);

  /* lay them out */
  consoleTabs->addTab(parsingConsole, "Parsing");
  consoleTabs->addTab(ltsCreationConsole, "LTS creation");
  consoleTabs->addTab(verificationConsole, "Verification");

  this->setWidget(consoleTabs);
}

void ConsoleDock::setConsoleTab(ProcessType processType)
{
  switch (processType)
  {
  case ProcessType::Parsing:
    consoleTabs->setCurrentIndex(0);
    break;
  case ProcessType::LtsCreation:
    consoleTabs->setCurrentIndex(1);
    break;
  case ProcessType::Verification:
    consoleTabs->setCurrentIndex(2);
    break;
  }
}

void ConsoleDock::logToParsingConsole()
{
  logToConsole(parsingConsole, qobject_cast<QProcess*>(sender()));
}

void ConsoleDock::logToLtsCreationConsole()
{
  logToConsole(ltsCreationConsole, qobject_cast<QProcess*>(sender()));
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

void ConsoleDock::writeToConsole(ProcessType processType, QString output)
{
  QPlainTextEdit* console;

  switch (processType)
  {
  case ProcessType::Parsing:
    console = parsingConsole;
    break;
  case ProcessType::LtsCreation:
    console = ltsCreationConsole;
    break;
  case ProcessType::Verification:
    console = verificationConsole;
    break;
  }

  console->appendPlainText(output);
}
