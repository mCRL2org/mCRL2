// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "processsystem.h"

#include <QEventLoop>

ProcessThread::ProcessThread(QQueue<int>* processQueue, bool verification)
{
  this->processQueue = processQueue;
  this->verification = verification;
  currentProcessid = -1;
}

void ProcessThread::newProcessQueued(bool verification)
{
  if (this->verification == verification)
  {
    emit newProcessInQueue();
  }
}

void ProcessThread::processFinished(int processid)
{
  if (processid == currentProcessid)
  {
    emit currentProcessFinished();
  }
}

void ProcessThread::run()
{
  QEventLoop queueLoop;
  connect(this, SIGNAL(newProcessInQueue()), &queueLoop, SLOT(quit()));
  QEventLoop finishLoop;
  connect(this, SIGNAL(currentProcessFinished()), &finishLoop, SLOT(quit()));

  while (true)
  {
    if (!processQueue->isEmpty())
    {
      /* start a new process */
      currentProcessid = processQueue->dequeue();
      emit startProcess(currentProcessid);
      /* wait until it has finished */
      finishLoop.exec();
    }
    else
    {
      queueLoop.exec();
    }
  }
}

ProcessSystem::ProcessSystem(FileSystem* fileSystem)
{
  this->fileSystem = fileSystem;
  pid = 0;
  verificationQueue = new QQueue<int>();
  verificationThread = new ProcessThread(verificationQueue, true);

  connect(this, SIGNAL(newProcessQueued(bool)),
          verificationThread, SLOT(newProcessQueued(bool)));
  connect(this, SIGNAL(processFinished(int)),
          verificationThread, SLOT(processFinished(int)));
  connect(verificationThread, SIGNAL(startProcess(int)),
          this, SLOT(createLps(int)));

  verificationThread->start();
}

void ProcessSystem::setConsoleDock(ConsoleDock* consoleDock)
{
  this->consoleDock = consoleDock;
}

QProcess* ProcessSystem::createMcrl22lpsProcess(bool verification)
{
  QProcess* mcrl22lpsProcess = new QProcess();

  /* create the process */
  mcrl22lpsProcess->setProgram("mcrl22lps");
  mcrl22lpsProcess->setArguments(
      {fileSystem->specificationFilePath(), fileSystem->lpsFilePath(),
       "--lin-method=regular", "--rewriter=jitty", "--verbose"});

  /* connect to logger */
  if (verification)
  {
    connect(mcrl22lpsProcess, SIGNAL(readyReadStandardError()),
            consoleDock, SLOT(logToVerificationConsole()));
  }
  else
  {
    connect(mcrl22lpsProcess, SIGNAL(readyReadStandardError()),
            consoleDock, SLOT(logToLTSCreationConsole()));
  }

  return mcrl22lpsProcess;
}

QProcess* ProcessSystem::createLpsxsimProcess()
{
  /* Not implemented yet */
  return new QProcess();
}

QProcess* ProcessSystem::createLps2ltsProcess()
{
  /* Not implemented yet */
  return new QProcess();
}

QProcess* ProcessSystem::createLtsconvertProcess()
{
  /* Not implemented yet */
  return new QProcess();
}

QProcess* ProcessSystem::createLps2pbesProcess(QString propertyName)
{
  QProcess* lps2pbesProcess = new QProcess();
  
  /* create the process */
  lps2pbesProcess->setProgram("lps2pbes");
  lps2pbesProcess->setArguments(
      {fileSystem->lpsFilePath(), fileSystem->pbesFilePath(propertyName),
        "--formula=" + fileSystem->propertyFilePath(propertyName),
        "--out=pbes", "--verbose"});
  lps2pbesProcess->setProperty("propertyName", propertyName);

  /* connect to logger */
  connect(lps2pbesProcess, SIGNAL(readyReadStandardError()),
    consoleDock, SLOT(logToVerificationConsole()));

  return lps2pbesProcess;
}

QProcess* ProcessSystem::createPbes2boolProcess(QString propertyName)
{
  /* create the process */
  QProcess* pbes2boolProcess = new QProcess();
  pbes2boolProcess->setProgram("pbes2bool");
  pbes2boolProcess->setArguments({fileSystem->pbesFilePath(propertyName),
                                  "--erase=none", "--in=pbes",
                                  "--rewriter=jitty", "--search=breadth-first",
                                  "--solver=lf", "--strategy=0", "--verbose"});
  pbes2boolProcess->setProperty("propertyName", propertyName);

  /* connect to logger */
  connect(pbes2boolProcess, SIGNAL(readyReadStandardError()),
          consoleDock, SLOT(logToVerificationConsole()));

  return pbes2boolProcess;
}

int ProcessSystem::verifyProperty(Property* property)
{
  if (fileSystem->saveProject())
  {
    consoleDock->setConsoleTab(ConsoleDock::Verification);

    /* create the subprocesses */
    int processid = pid++;

    QProcess* mcrl22lpsProcess = createMcrl22lpsProcess(true);
    mcrl22lpsProcess->setProperty("pid", processid);
    connect(mcrl22lpsProcess, SIGNAL(finished(int)), this,
            SLOT(verifyProperty2()));

    QProcess* lps2pbesProcess = createLps2pbesProcess(property->name);
    lps2pbesProcess->setProperty("pid", processid);
    connect(lps2pbesProcess, SIGNAL(finished(int)), this,
            SLOT(verifyProperty3()));

    QProcess* pbes2boolProcess = createPbes2boolProcess(property->name);
    pbes2boolProcess->setProperty("pid", processid);
    connect(pbes2boolProcess, SIGNAL(finished(int)), this,
            SLOT(verifyPropertyResult()));

    processes[processid] = {mcrl22lpsProcess, lps2pbesProcess,
                            pbes2boolProcess};

    verificationQueue->enqueue(processid);
    emit newProcessQueued(true);

    return processid;
  }
  return -1;
}

void ProcessSystem::createLps(int processid)
{
  QProcess* mcrl22lpsProcess = processes[processid][0];

  consoleDock->writeToConsole(ConsoleDock::Verification,
                              "##### CREATING LPS #####\n");

  /* check if we need to run this */
  if (fileSystem->upToDateLpsFileExists())
  {
    consoleDock->writeToConsole(ConsoleDock::Verification,
                                "Up to date lps already exists\n");
    emit mcrl22lpsProcess->finished(0);
  }
  else
  {
    mcrl22lpsProcess->start();
  }
}

void ProcessSystem::verifyProperty2()
{
  
  int processid = qobject_cast<QProcess*>(sender())->property("pid").toInt();
  QProcess* lps2pbesProcess = processes[processid][1];

  consoleDock->writeToConsole(ConsoleDock::Verification,
                              "##### CREATING PBES #####\n");
  
  /* check if we need to run this */
  if (fileSystem->upToDatePbesFileExists(lps2pbesProcess->property("propertyName").toString()))
  {
    consoleDock->writeToConsole(ConsoleDock::Verification,
                                "Up to date pbes already exists");
    emit lps2pbesProcess->finished(0);
  }
  else
  {
    lps2pbesProcess->start();
  }
}

void ProcessSystem::verifyProperty3()
{
  consoleDock->writeToConsole(ConsoleDock::Verification,
                              "##### SOLVING PBES #####\n");
  int processid = qobject_cast<QProcess*>(sender())->property("pid").toInt();
  processes[processid][2]->start();
}

void ProcessSystem::verifyPropertyResult()
{
  int processid = qobject_cast<QProcess*>(sender())->property("pid").toInt();
  std::string output =
      processes[processid][2]->readAllStandardOutput().toStdString();
  if (output.find("true") == 0)
  {
    results[processid] = "true";
  }
  else if (output.find("false") == 0)
  {
    results[processid] = "false";
  }
  else
  {
    results[processid] = "";
  }
  emit processFinished(processid);
}

QString ProcessSystem::getResult(int processid)
{
  return results[processid];
}
