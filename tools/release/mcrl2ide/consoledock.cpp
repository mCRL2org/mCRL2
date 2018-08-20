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
  for (ProcessType processType : PROCESSTYPES)
  {
    consoles[processType] = new QPlainTextEdit();
    consoles[processType]->setReadOnly(true);

    QFont logFont;
    logFont.setFamily("Monospace");
    logFont.setFixedPitch(true);
    logFont.setWeight(QFont::Light);
    consoles[processType]->setFont(logFont);

    consoleTabs->addTab(consoles[processType],
                        PROCESSTYPENAMES.at(processType));
  }

  this->setWidget(consoleTabs);
}

ConsoleDock::~ConsoleDock()
{
  for (ProcessType processType : PROCESSTYPES)
  {
    consoles[processType]->deleteLater();
  }
}

void ConsoleDock::setConsoleTab(ProcessType processType)
{
  consoleTabs->setCurrentWidget(consoles[processType]);
}

void ConsoleDock::logToParsingConsole()
{
  logToConsole(ProcessType::Parsing, qobject_cast<QProcess*>(sender()));
}

void ConsoleDock::logToSimulationConsole()
{
  logToConsole(ProcessType::Simulation, qobject_cast<QProcess*>(sender()));
}

void ConsoleDock::logToLtsCreationConsole()
{
  logToConsole(ProcessType::LtsCreation, qobject_cast<QProcess*>(sender()));
}

void ConsoleDock::logToVerificationConsole()
{
  logToConsole(ProcessType::Verification, qobject_cast<QProcess*>(sender()));
}

void ConsoleDock::logToConsole(ProcessType processType, QProcess* process)
{
  writeToConsole(processType, process->readAllStandardError());
}

void ConsoleDock::writeToConsole(ProcessType processType, const QString& output)
{
  consoles[processType]->moveCursor(QTextCursor::End);
  consoles[processType]->insertPlainText(output);
  consoles[processType]->ensureCursorVisible();
}

QString ConsoleDock::getConsoleOutput(ProcessType processType)
{
  return consoles[processType]->toPlainText();
}
