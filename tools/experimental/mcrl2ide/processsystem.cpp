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

ProcessThread::ProcessThread(QQueue<int>* processQueue, ProcessType processType)
{
  this->processQueue = processQueue;
  this->processType = processType;
  currentProcessid = -1;
}

void ProcessThread::newProcessQueued(ProcessType processType)
{
  if (this->processType == processType)
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
      emit isRunning(true);

      /* wait until it has finished */
      finishLoop.exec();
    }
    else
    {
      /* wait until a process is added to the queue */
      emit isRunning(false);
      queueLoop.exec();
    }
  }
}

int ProcessThread::getCurrentProcessId()
{
  return currentProcessid;
}

ProcessSystem::ProcessSystem(FileSystem* fileSystem)
{
  this->fileSystem = fileSystem;
  pid = 0;

  for (ProcessType processType : PROCESSTYPES)
  {
    processQueues[processType] = new QQueue<int>();
    processThreads[processType] =
        new ProcessThread(processQueues[processType], processType);
    connect(this, SIGNAL(newProcessQueued(ProcessType)),
            processThreads[processType], SLOT(newProcessQueued(ProcessType)));
    connect(processThreads[processType], SIGNAL(startProcess(int)), this,
            SLOT(startProcess(int)));
    connect(this, SIGNAL(processFinished(int)), processThreads[processType],
            SLOT(processFinished(int)));

    processThreads[processType]->start();
  }
}

void ProcessSystem::setConsoleDock(ConsoleDock* consoleDock)
{
  this->consoleDock = consoleDock;
}

ProcessThread* ProcessSystem::getProcessThread(ProcessType processType)
{
  return processThreads[processType];
}

QProcess* ProcessSystem::createMcrl22lpsProcess(ProcessType processType)
{
  QProcess* mcrl22lpsProcess = new QProcess();

  /* create the process */
  mcrl22lpsProcess->setProgram("mcrl22lps");
  mcrl22lpsProcess->setArguments(
      {fileSystem->specificationFilePath(), fileSystem->lpsFilePath(),
       "--lin-method=regular", "--rewriter=jitty", "--verbose"});

  /* connect to logger */
  if (processType == ProcessType::Verification)
  {
    connect(mcrl22lpsProcess, SIGNAL(readyReadStandardError()), consoleDock,
            SLOT(logToVerificationConsole()));
  }
  else if (processType == ProcessType::LTSCreation)
  {
    connect(mcrl22lpsProcess, SIGNAL(readyReadStandardError()), consoleDock,
            SLOT(logToLTSCreationConsole()));
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
       "--formula=" + fileSystem->propertyFilePath(propertyName), "--out=pbes",
       "--verbose"});
  lps2pbesProcess->setProperty("propertyName", propertyName);

  /* connect to logger */
  connect(lps2pbesProcess, SIGNAL(readyReadStandardError()), consoleDock,
          SLOT(logToVerificationConsole()));

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
  connect(pbes2boolProcess, SIGNAL(readyReadStandardError()), consoleDock,
          SLOT(logToVerificationConsole()));

  return pbes2boolProcess;
}

int ProcessSystem::verifyProperty(Property* property)
{
  if (fileSystem->saveProject())
  {
    consoleDock->setConsoleTab(ProcessType::Verification);

    /* create the subprocesses */
    int processid = pid++;
    ProcessType processType = ProcessType::Verification;

    QProcess* mcrl22lpsProcess = createMcrl22lpsProcess(processType);
    mcrl22lpsProcess->setProperty("pid", processid);
    connect(mcrl22lpsProcess, SIGNAL(finished(int)), this, SLOT(createPbes()));

    QProcess* lps2pbesProcess = createLps2pbesProcess(property->name);
    lps2pbesProcess->setProperty("pid", processid);
    connect(lps2pbesProcess, SIGNAL(finished(int)), this, SLOT(solvePbes()));

    QProcess* pbes2boolProcess = createPbes2boolProcess(property->name);
    pbes2boolProcess->setProperty("pid", processid);
    connect(pbes2boolProcess, SIGNAL(finished(int)), this,
            SLOT(verificationResult()));

    processes[processid] = {mcrl22lpsProcess, lps2pbesProcess,
                            pbes2boolProcess};
    processTypes[processid] = processType;
    processQueues[processType]->enqueue(processid);
    emit newProcessQueued(processType);

    return processid;
  }
  return -1;
}

void ProcessSystem::startProcess(int processid)
{
  ProcessType processType = processTypes[processid];
  if (processType == ProcessType::LTSCreation ||
      processType == ProcessType::Verification)
  {
    createLps(processid);
  }
}

void ProcessSystem::createLps(int processid)
{
  QProcess* mcrl22lpsProcess = processes[processid][0];

  consoleDock->writeToConsole(ProcessType::Verification,
                              "##### CREATING LPS #####\n");

  /* check if we need to run this */
  if (fileSystem->upToDateLpsFileExists())
  {
    consoleDock->writeToConsole(ProcessType::Verification,
                                "Up to date lps already exists\n");
    emit mcrl22lpsProcess->finished(0);
  }
  else
  {
    mcrl22lpsProcess->start();
  }
}

void ProcessSystem::createPbes()
{

  int processid = qobject_cast<QProcess*>(sender())->property("pid").toInt();
  QProcess* lps2pbesProcess = processes[processid][1];

  consoleDock->writeToConsole(ProcessType::Verification,
                              "##### CREATING PBES #####\n");

  /* check if we need to run this */
  if (fileSystem->upToDatePbesFileExists(
          lps2pbesProcess->property("propertyName").toString()))
  {
    consoleDock->writeToConsole(ProcessType::Verification,
                                "Up to date pbes already exists");
    emit lps2pbesProcess->finished(0);
  }
  else
  {
    lps2pbesProcess->start();
  }
}

void ProcessSystem::solvePbes()
{
  consoleDock->writeToConsole(ProcessType::Verification,
                              "##### SOLVING PBES #####\n");
  int processid = qobject_cast<QProcess*>(sender())->property("pid").toInt();
  processes[processid][2]->start();
}

void ProcessSystem::verificationResult()
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

void ProcessSystem::abortProcess(int processid)
{
  ProcessType processType = processTypes[processid];

  /* if this process is running, terminate it */
  if (processThreads[processType]->getCurrentProcessId() == processid)
  {
    for (QProcess* process : processes[processid])
    {
      process->blockSignals(true);
      process->kill();
    }
  }
  else
  {
    /* if it is not running, simply remove it from the queue */
    processQueues[processType]->removeOne(processid);
  }

  emit processFinished(processid);
  consoleDock->writeToConsole(processTypes[processid],
                              "##### PROCESS WAS ABORTED #####");
}

void ProcessSystem::abortAllProcesses(ProcessType processType)
{
  /* first empty the queue */
  QQueue<int>* processQueue = processQueues[processType];
  foreach (int processid, *processQueue)
  {
    emit processFinished(processid);
  }
  processQueues[processType]->clear();

  /* then stop the process run by the thread */
  int processid = processThreads[processType]->getCurrentProcessId();
  if (processes.count(processid) > 0)
  {
    for (QProcess* process : processes[processid])
    {
      process->blockSignals(true);
      process->kill();
    }
  }
  emit processFinished(processid);

  consoleDock->writeToConsole(processType, "##### ABORTED ALL PROCESSES #####");
}

QString ProcessSystem::getResult(int processid)
{
  if (results.count(processid) > 0)
  {
    return results[processid];
  }
  else
  {
    return "";
  }
}
