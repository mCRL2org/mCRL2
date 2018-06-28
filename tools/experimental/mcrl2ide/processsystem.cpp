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

QProcess* ProcessSystem::createMcrl2ParsingProcess()
{
  QProcess* mcrl22lpsProcess = new QProcess();

  /* create the process */
  mcrl22lpsProcess->setProgram("mcrl22lps");
  mcrl22lpsProcess->setArguments({fileSystem->specificationFilePath(),
                                  fileSystem->lpsFilePath(), "--check-only",
                                  "--lin-method=regular", "--rewriter=jitty",
                                  "--verbose"});

  connect(mcrl22lpsProcess, SIGNAL(readyReadStandardError()), consoleDock,
          SLOT(logToParsingConsole()));

  return mcrl22lpsProcess;
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
  switch (processType)
  {
  case ProcessType::Simulation:
    connect(mcrl22lpsProcess, SIGNAL(readyReadStandardError()), consoleDock,
            SLOT(logToSimulationConsole()));
    break;
  case ProcessType::LtsCreation:
    connect(mcrl22lpsProcess, SIGNAL(readyReadStandardError()), consoleDock,
            SLOT(logToLtsCreationConsole()));
    break;
  case ProcessType::Verification:
    connect(mcrl22lpsProcess, SIGNAL(readyReadStandardError()), consoleDock,
            SLOT(logToVerificationConsole()));
    break;
  default:
    break;
  }

  return mcrl22lpsProcess;
}

QProcess* ProcessSystem::createLpsxsimProcess()
{
  QProcess* lpsxsimProcess = new QProcess();

  /* create the process */
  lpsxsimProcess->setProgram("lpsxsim");
  lpsxsimProcess->setArguments({fileSystem->lpsFilePath(), "--rewriter=jitty"});

  return lpsxsimProcess;
}

QProcess* ProcessSystem::createLps2ltsProcess()
{
  QProcess* lps2ltsProcess = new QProcess();

  /* create the process */
  lps2ltsProcess->setProgram("lps2lts");
  lps2ltsProcess->setArguments(
      {fileSystem->lpsFilePath(), fileSystem->ltsFilePath(LtsReduction::None),
       "--rewriter=jitty", "--strategy=breadth", "--verbose"});

  /* connect to logger */
  connect(lps2ltsProcess, SIGNAL(readyReadStandardError()), consoleDock,
          SLOT(logToLtsCreationConsole()));

  return lps2ltsProcess;
}

QProcess* ProcessSystem::createLtsconvertProcess(LtsReduction reduction)
{
  QProcess* ltsconvertProcess = new QProcess();

  /* create the process */
  ltsconvertProcess->setProgram("ltsconvert");

  QString equivalence = "--equivalence=";
  switch (reduction)
  {
  case LtsReduction::StrongBisimulation:
    equivalence += "bisim";
    break;
  case LtsReduction::BranchingBisimulation:
    equivalence += "branching-bisim";
    break;
  case LtsReduction::None:
    equivalence += "none";
    break;
  }

  ltsconvertProcess->setArguments({fileSystem->ltsFilePath(LtsReduction::None),
                                   fileSystem->ltsFilePath(reduction),
                                   equivalence, "--verbose"});

  ltsconvertProcess->setProperty("reduction", int(reduction));

  /* connect to logger */
  connect(ltsconvertProcess, SIGNAL(readyReadStandardError()), consoleDock,
          SLOT(logToLtsCreationConsole()));

  return ltsconvertProcess;
}

QProcess* ProcessSystem::createLtsgraphProcess(LtsReduction reduction)
{
  QProcess* ltsgraphProcess = new QProcess();

  /* create the process */
  ltsgraphProcess->setProgram("ltsgraph");
  ltsgraphProcess->setArguments({fileSystem->ltsFilePath(reduction)});

  return ltsgraphProcess;
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

int ProcessSystem::parseSpecification()
{
  if (!fileSystem->saveProject().isEmpty())
  {
    /* create the subprocesses */
    int processid = pid++;
    ProcessType processType = ProcessType::Parsing;
    consoleDock->setConsoleTab(processType);

    QProcess* mcrl2ParsingProcess = createMcrl2ParsingProcess();
    mcrl2ParsingProcess->setProperty("pid", processid);
    connect(mcrl2ParsingProcess, SIGNAL(finished(int)), this,
            SLOT(mcrl2ParsingResult(int)));

    processes[processid] = {mcrl2ParsingProcess};
    processTypes[processid] = processType;
    processQueues[processType]->enqueue(processid);
    emit newProcessQueued(processType);

    return processid;
  }
  return -1;
}

int ProcessSystem::simulate()
{
  if (!fileSystem->saveProject().isEmpty())
  {
    /* create the subprocesses */
    int processid = pid++;
    ProcessType processType = ProcessType::Simulation;
    consoleDock->setConsoleTab(processType);

    QProcess* mcrl2ParsingProcess = createMcrl2ParsingProcess();
    mcrl2ParsingProcess->setProperty("pid", processid);
    connect(mcrl2ParsingProcess, SIGNAL(finished(int)), this,
            SLOT(mcrl2ParsingResult(int)));
    connect(mcrl2ParsingProcess, SIGNAL(finished(int)), this,
            SLOT(createLps(int)));

    QProcess* mcrl22lpsProcess = createMcrl22lpsProcess(processType);
    mcrl22lpsProcess->setProperty("pid", processid);
    connect(mcrl22lpsProcess, SIGNAL(finished(int)), this,
            SLOT(simulateLps(int)));

    QProcess* lpsxsimProcess = createLpsxsimProcess();
    lpsxsimProcess->setProperty("pid", processid);

    processes[processid] = {mcrl2ParsingProcess, mcrl22lpsProcess,
                            lpsxsimProcess};
    processTypes[processid] = processType;
    processQueues[processType]->enqueue(processid);
    emit newProcessQueued(processType);

    return processid;
  }
  return -1;
}

int ProcessSystem::createLts(LtsReduction reduction)
{
  if (!fileSystem->saveProject().isEmpty())
  {
    /* create the subprocesses */
    std::vector<QProcess*> ltsCreationProcesses = {};
    int processid = pid++;
    ProcessType processType = ProcessType::LtsCreation;
    consoleDock->setConsoleTab(processType);

    QProcess* mcrl2ParsingProcess = createMcrl2ParsingProcess();
    mcrl2ParsingProcess->setProperty("pid", processid);
    connect(mcrl2ParsingProcess, SIGNAL(finished(int)), this,
            SLOT(mcrl2ParsingResult(int)));
    connect(mcrl2ParsingProcess, SIGNAL(finished(int)), this,
            SLOT(createLps(int)));
    ltsCreationProcesses.push_back(mcrl2ParsingProcess);

    QProcess* mcrl22lpsProcess = createMcrl22lpsProcess(processType);
    mcrl22lpsProcess->setProperty("pid", processid);
    connect(mcrl22lpsProcess, SIGNAL(finished(int)), this,
            SLOT(createLts(int)));
    ltsCreationProcesses.push_back(mcrl22lpsProcess);

    QProcess* lps2ltsProcess = createLps2ltsProcess();
    lps2ltsProcess->setProperty("pid", processid);
    connect(lps2ltsProcess, SIGNAL(finished(int)), this,
            reduction == LtsReduction::None ? SLOT(showLts(int))
                                            : SLOT(reduceLts(int)));
    ltsCreationProcesses.push_back(lps2ltsProcess);

    /* create the ltsconvert process if we need to reduce */
    if (!(reduction == LtsReduction::None))
    {
      QProcess* ltsconvertProcess = createLtsconvertProcess(reduction);
      ltsconvertProcess->setProperty("pid", processid);
      connect(ltsconvertProcess, SIGNAL(finished(int)), this,
              SLOT(showLts(int)));
      ltsCreationProcesses.push_back(ltsconvertProcess);
    }

    QProcess* ltsgraphProcess = createLtsgraphProcess(reduction);
    ltsgraphProcess->setProperty("pid", processid);
    ltsCreationProcesses.push_back(ltsgraphProcess);

    processes[processid] = ltsCreationProcesses;
    processTypes[processid] = processType;
    processQueues[processType]->enqueue(processid);
    emit newProcessQueued(processType);

    return processid;
  }
  return -1;
}

int ProcessSystem::verifyProperty(Property* property)
{
  if (!fileSystem->saveProject().isEmpty())
  {
    /* create the subprocesses */
    int processid = pid++;
    ProcessType processType = ProcessType::Verification;
    consoleDock->setConsoleTab(processType);

    QProcess* mcrl2ParsingProcess = createMcrl2ParsingProcess();
    mcrl2ParsingProcess->setProperty("pid", processid);
    connect(mcrl2ParsingProcess, SIGNAL(finished(int)), this,
            SLOT(mcrl2ParsingResult(int)));
    connect(mcrl2ParsingProcess, SIGNAL(finished(int)), this,
            SLOT(createLps(int)));

    QProcess* mcrl22lpsProcess = createMcrl22lpsProcess(processType);
    mcrl22lpsProcess->setProperty("pid", processid);
    connect(mcrl22lpsProcess, SIGNAL(finished(int)), this,
            SLOT(createPbes(int)));

    QProcess* lps2pbesProcess = createLps2pbesProcess(property->name);
    lps2pbesProcess->setProperty("pid", processid);
    connect(lps2pbesProcess, SIGNAL(finished(int)), this, SLOT(solvePbes(int)));

    QProcess* pbes2boolProcess = createPbes2boolProcess(property->name);
    pbes2boolProcess->setProperty("pid", processid);
    connect(pbes2boolProcess, SIGNAL(finished(int)), this,
            SLOT(verificationResult(int)));

    processes[processid] = {mcrl2ParsingProcess, mcrl22lpsProcess,
                            lps2pbesProcess, pbes2boolProcess};
    processTypes[processid] = processType;
    processQueues[processType]->enqueue(processid);
    emit newProcessQueued(processType);

    return processid;
  }
  return -1;
}

void ProcessSystem::startProcess(int processid)
{
  /* (for now) all processes start with parsing the specification */
  parseMcrl2(processid);
}

void ProcessSystem::parseMcrl2(int processid)
{
  consoleDock->writeToConsole(ProcessType::Parsing,
                              "##### PARSING SPECIFICATION #####\n");
  if (processTypes[processid] != ProcessType::Parsing)
  {
    consoleDock->writeToConsole(processTypes[processid],
                                "##### PARSING SPECIFICATION #####\n");
  }

  processes[processid][0]->start();
}

void ProcessSystem::mcrl2ParsingResult(int previousExitCode)
{
  QProcess* mcrl2ParsingProcess = qobject_cast<QProcess*>(sender());
  int processid = mcrl2ParsingProcess->property("pid").toInt();
  if (processTypes[processid] == ProcessType::Parsing)
  {
    emit processFinished(processid);
  }

  /* if parsing gave an error, move to the parsing tab and move the cursor in
   *   the code editor to the parsing error if possible */
  if (previousExitCode > 0)
  {
    consoleDock->setConsoleTab(ProcessType::Parsing);
    QString parsingOutput = consoleDock->getConsoleOutput(ProcessType::Parsing);
    QRegExp parsingError = QRegExp("Line (\\d+), column (\\d+): syntax error");
    int parsingErrorIndex =
        parsingOutput.indexOf(parsingError, parsingOutput.lastIndexOf("#####"));
    if (parsingErrorIndex >= 0)
    {
      fileSystem->setSpecificationEditorCursor(parsingError.cap(1).toInt(),
                                               parsingError.cap(2).toInt());
    }
  }
}

void ProcessSystem::createLps(int previousExitCode)
{
  int processid = qobject_cast<QProcess*>(sender())->property("pid").toInt();
  ProcessType processType = processTypes[processid];

  /* if the previous subprocess has failed, the process is discontinued */
  if (previousExitCode > 0)
  {
    consoleDock->writeToConsole(processType, "Process finished with an error");
    emit processFinished(processid);
  }
  else
  {
    QProcess* mcrl22lpsProcess = processes[processid][1];

    consoleDock->writeToConsole(processType, "##### CREATING LPS #####\n");

    /* check if we need to run this */
    if (fileSystem->upToDateLpsFileExists())
    {
      consoleDock->writeToConsole(processType,
                                  "Up to date LPS already exists\n");
      emit mcrl22lpsProcess->finished(0);
    }
    else
    {
      mcrl22lpsProcess->start();
    }
  }
}

void ProcessSystem::simulateLps(int previousExitCode)
{
  int processid = qobject_cast<QProcess*>(sender())->property("pid").toInt();

  /* if the previous subprocess has failed, the process is discontinued */
  if (previousExitCode > 0)
  {
    consoleDock->writeToConsole(ProcessType::Simulation,
                                "Process finished with an error");
    emit processFinished(processid);
  }
  else
  {
    QProcess* lpsxsimProcess = processes[processid][2];

    consoleDock->writeToConsole(ProcessType::Simulation,
                                "##### SHOWING SIMULATION #####\n");
    lpsxsimProcess->start();
    emit processFinished(processid);
  }
}

void ProcessSystem::createLts(int previousExitCode)
{
  int processid = qobject_cast<QProcess*>(sender())->property("pid").toInt();

  /* if the previous subprocess has failed, the process is discontinued */
  if (previousExitCode > 0)
  {
    consoleDock->writeToConsole(ProcessType::LtsCreation,
                                "Process finished with an error");
    emit processFinished(processid);
  }
  else
  {
    QProcess* lps2ltsProcess = processes[processid][2];

    consoleDock->writeToConsole(ProcessType::LtsCreation,
                                "##### CREATING LTS #####\n");

    /* check if we need to run this */
    if (fileSystem->upToDateLtsFileExists(LtsReduction::None))
    {
      consoleDock->writeToConsole(ProcessType::LtsCreation,
                                  "Up to date LTS already exists");
      emit lps2ltsProcess->finished(0);
    }
    else
    {
      lps2ltsProcess->start();
    }
  }
}

void ProcessSystem::reduceLts(int previousExitCode)
{
  int processid = qobject_cast<QProcess*>(sender())->property("pid").toInt();

  /* if the previous subprocess has failed, the process is discontinued */
  if (previousExitCode > 0)
  {
    consoleDock->writeToConsole(ProcessType::LtsCreation,
                                "Process finished with an error");
    emit processFinished(processid);
  }
  else
  {
    QProcess* ltsconvertProcess = processes[processid][3];
    LtsReduction reduction = static_cast<LtsReduction>(
        ltsconvertProcess->property("reduction").toInt());

    consoleDock->writeToConsole(ProcessType::LtsCreation,
                                "##### REDUCING LTS #####\n");

    /* check if we need to run this */
    if (fileSystem->upToDateLtsFileExists(reduction))
    {
      consoleDock->writeToConsole(ProcessType::LtsCreation,
                                  "Up to date LTS already exists");
      emit ltsconvertProcess->finished(0);
    }
    else
    {
      ltsconvertProcess->start();
    }
  }
}

void ProcessSystem::showLts(int previousExitCode)
{
  int processid = qobject_cast<QProcess*>(sender())->property("pid").toInt();

  /* if the previous subprocess has failed, the process is discontinued */
  if (previousExitCode > 0)
  {
    consoleDock->writeToConsole(ProcessType::LtsCreation,
                                "Process finished with an error");
    emit processFinished(processid);
  }
  else
  {
    QProcess* ltsgraphProcess = processes[processid].back();

    consoleDock->writeToConsole(ProcessType::LtsCreation,
                                "##### SHOWING LTS #####\n");
    ltsgraphProcess->start();
    emit processFinished(processid);
  }
}

void ProcessSystem::createPbes(int previousExitCode)
{
  int processid = qobject_cast<QProcess*>(sender())->property("pid").toInt();

  /* if the previous subprocess has failed, the process is discontinued */
  if (previousExitCode > 0)
  {
    consoleDock->writeToConsole(ProcessType::Verification,
                                "Process finished with an error");
    emit processFinished(processid);
  }
  else
  {
    QProcess* lps2pbesProcess = processes[processid][2];

    consoleDock->writeToConsole(ProcessType::Verification,
                                "##### CREATING PBES #####\n");

    /* check if we need to run this */
    if (fileSystem->upToDatePbesFileExists(
            lps2pbesProcess->property("propertyName").toString()))
    {
      consoleDock->writeToConsole(ProcessType::Verification,
                                  "Up to date PBES already exists");
      emit lps2pbesProcess->finished(0);
    }
    else
    {
      lps2pbesProcess->start();
    }
  }
}

void ProcessSystem::solvePbes(int previousExitCode)
{
  int processid = qobject_cast<QProcess*>(sender())->property("pid").toInt();

  /* if the previous subprocess has failed, the process is discontinued */
  if (previousExitCode > 0)
  {
    consoleDock->writeToConsole(ProcessType::Verification,
                                "Process finished with an error");
    emit processFinished(processid);
  }
  else
  {
    consoleDock->writeToConsole(ProcessType::Verification,
                                "##### SOLVING PBES #####\n");

    processes[processid][3]->start();
  }
}

void ProcessSystem::verificationResult(int previousExitCode)
{
  int processid = qobject_cast<QProcess*>(sender())->property("pid").toInt();

  /* if the previous subprocess has failed, the process is discontinued */
  if (previousExitCode > 0)
  {
    consoleDock->writeToConsole(ProcessType::Verification,
                                "Process finished with an error");
    emit processFinished(processid);
  }
  else
  {
    std::string output =
        processes[processid][3]->readAllStandardOutput().toStdString();
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
