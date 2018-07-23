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
  running = false;
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
      running = true;
      emit statusChanged(true, processType);

      /* wait until it has finished */
      finishLoop.exec();
      currentProcessid = -1;
    }
    else
    {
      /* wait until a process is added to the queue */
      running = false;
      emit statusChanged(false, processType);
      queueLoop.exec();
    }
  }
}

bool ProcessThread::isRunning()
{
  return running;
}

int ProcessThread::getCurrentProcessId()
{
  return currentProcessid;
}

ProcessSystem::ProcessSystem(FileSystem* fileSystem)
{
  this->fileSystem = fileSystem;
  pid = 0;

  qRegisterMetaType<ProcessType>("ProcessType");

  /* create, connect and start all process threads */
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

ProcessSystem::~ProcessSystem()
{
  for (ProcessType processType : PROCESSTYPES)
  {
    delete processQueues[processType];
    delete processThreads[processType];
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

bool ProcessSystem::isThreadRunning(ProcessType processType)
{
  return processThreads[processType]->isRunning();
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
  case ProcessType::Parsing:
    connect(mcrl22lpsProcess, SIGNAL(readyReadStandardError()), consoleDock,
            SLOT(logToParsingConsole()));
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

QProcess* ProcessSystem::createLps2ltsProcess(bool evidence,
                                              QString propertyName)
{
  QProcess* lps2ltsProcess = new QProcess();

  /* create the process */
  lps2ltsProcess->setProgram("lps2lts");
  lps2ltsProcess->setArguments(
      {fileSystem->lpsFilePath(evidence, propertyName),
       fileSystem->ltsFilePath(LtsReduction::None, evidence, propertyName),
       "--rewriter=jitty", "--strategy=breadth", "--verbose"});
  lps2ltsProcess->setProperty("evidence", evidence);
  lps2ltsProcess->setProperty("propertyName", propertyName);

  /* connect to logger */
  if (evidence)
  {
    connect(lps2ltsProcess, SIGNAL(readyReadStandardError()), consoleDock,
            SLOT(logToVerificationConsole()));
  }
  else
  {
    connect(lps2ltsProcess, SIGNAL(readyReadStandardError()), consoleDock,
            SLOT(logToLtsCreationConsole()));
  }

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

QProcess* ProcessSystem::createLtsgraphProcess(LtsReduction reduction,
                                               bool evidence,
                                               QString propertyName)
{
  QProcess* ltsgraphProcess = new QProcess();

  /* create the process */
  ltsgraphProcess->setProgram("ltsgraph");
  ltsgraphProcess->setArguments(
      {fileSystem->ltsFilePath(reduction, evidence, propertyName)});

  return ltsgraphProcess;
}

QProcess* ProcessSystem::createPropertyParsingProcess(QString propertyName)
{
  QProcess* lps2pbesProcess = new QProcess();

  /* create the process */
  lps2pbesProcess->setProgram("lps2pbes");
  lps2pbesProcess->setArguments(
      {fileSystem->lpsFilePath(), fileSystem->pbesFilePath(propertyName),
       "--formula=" + fileSystem->propertyFilePath(propertyName), "--out=pbes",
       "--check_only", "--verbose"});

  connect(lps2pbesProcess, SIGNAL(readyReadStandardError()), consoleDock,
          SLOT(logToParsingConsole()));

  return lps2pbesProcess;
}

QProcess* ProcessSystem::createLps2pbesProcess(QString propertyName,
                                               bool evidence)
{
  QProcess* lps2pbesProcess = new QProcess();

  /* create the process */
  lps2pbesProcess->setProgram("lps2pbes");
  QStringList arguments = {fileSystem->lpsFilePath(),
                           fileSystem->pbesFilePath(propertyName, evidence),
                           "--formula=" +
                               fileSystem->propertyFilePath(propertyName),
                           "--out=pbes", "--verbose"};
  if (evidence)
  {
    arguments << "--counter-example";
  }
  lps2pbesProcess->setArguments(arguments);
  lps2pbesProcess->setProperty("propertyName", propertyName);
  lps2pbesProcess->setProperty("evidence", evidence);

  /* connect to logger */
  connect(lps2pbesProcess, SIGNAL(readyReadStandardError()), consoleDock,
          SLOT(logToVerificationConsole()));

  return lps2pbesProcess;
}

QProcess* ProcessSystem::createPbessolveProcess(QString propertyName,
                                                bool evidence)
{
  /* create the process */
  QProcess* pbessolveProcess = new QProcess();
  pbessolveProcess->setProgram("pbessolve");
  QStringList arguments = {fileSystem->pbesFilePath(propertyName, evidence),
                           "--in=pbes",
                           "--rewriter=jitty",
                           "--search=breadth-first",
                           "--strategy=0",
                           "--verbose"};
  if (evidence)
  {
    arguments << "--file=" + fileSystem->lpsFilePath(false, "")
              << "--evidence-file=" +
                     fileSystem->lpsFilePath(evidence, propertyName);
  }
  pbessolveProcess->setArguments(arguments);
  pbessolveProcess->setProperty("propertyName", propertyName);
  pbessolveProcess->setProperty("evidence", evidence);

  /* connect to logger */
  connect(pbessolveProcess, SIGNAL(readyReadStandardError()), consoleDock,
          SLOT(logToVerificationConsole()));

  return pbessolveProcess;
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
    connect(lpsxsimProcess, SIGNAL(finished(int)), this,
            SLOT(afterClosingUiTool()));

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
    connect(ltsgraphProcess, SIGNAL(finished(int)), this,
            SLOT(afterClosingUiTool()));
    ltsCreationProcesses.push_back(ltsgraphProcess);

    processes[processid] = ltsCreationProcesses;
    processTypes[processid] = processType;
    processQueues[processType]->enqueue(processid);
    emit newProcessQueued(processType);

    return processid;
  }
  return -1;
}

int ProcessSystem::parseProperty(Property* property)
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
    connect(mcrl2ParsingProcess, SIGNAL(finished(int)), this,
            SLOT(createLps(int)));

    QProcess* mcrl22lpsProcess = createMcrl22lpsProcess(processType);
    mcrl22lpsProcess->setProperty("pid", processid);
    connect(mcrl22lpsProcess, SIGNAL(finished(int)), this, SLOT(parseMcf(int)));

    QProcess* propertyParsingProcess =
        createPropertyParsingProcess(property->name);
    propertyParsingProcess->setProperty("pid", processid);
    connect(propertyParsingProcess, SIGNAL(finished(int)), this,
            SLOT(mcfParsingResult(int)));

    processes[processid] = {mcrl2ParsingProcess, mcrl22lpsProcess,
                            propertyParsingProcess};
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
    connect(mcrl22lpsProcess, SIGNAL(finished(int)), this, SLOT(parseMcf(int)));

    QProcess* propertyParsingProcess =
        createPropertyParsingProcess(property->name);
    propertyParsingProcess->setProperty("pid", processid);
    connect(propertyParsingProcess, SIGNAL(finished(int)), this,
            SLOT(mcfParsingResult(int)));
    connect(propertyParsingProcess, SIGNAL(finished(int)), this,
            SLOT(createPbes(int)));

    QProcess* lps2pbesProcess = createLps2pbesProcess(property->name);
    lps2pbesProcess->setProperty("pid", processid);
    connect(lps2pbesProcess, SIGNAL(finished(int)), this, SLOT(solvePbes(int)));

    QProcess* pbessolveProcess = createPbessolveProcess(property->name);
    pbessolveProcess->setProperty("pid", processid);
    connect(pbessolveProcess, SIGNAL(finished(int)), this,
            SLOT(verificationResult(int)));

    processes[processid] = {mcrl2ParsingProcess, mcrl22lpsProcess,
                            propertyParsingProcess, lps2pbesProcess,
                            pbessolveProcess};
    processTypes[processid] = processType;
    processQueues[processType]->enqueue(processid);
    emit newProcessQueued(processType);

    return processid;
  }
  return -1;
}

int ProcessSystem::createEvidence(Property* property)
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
    connect(mcrl22lpsProcess, SIGNAL(finished(int)), this, SLOT(parseMcf(int)));

    QProcess* propertyParsingProcess =
        createPropertyParsingProcess(property->name);
    propertyParsingProcess->setProperty("pid", processid);
    connect(propertyParsingProcess, SIGNAL(finished(int)), this,
            SLOT(mcfParsingResult(int)));
    connect(propertyParsingProcess, SIGNAL(finished(int)), this,
            SLOT(createPbes(int)));

    QProcess* lps2pbesProcess = createLps2pbesProcess(property->name, true);
    lps2pbesProcess->setProperty("pid", processid);
    connect(lps2pbesProcess, SIGNAL(finished(int)), this, SLOT(solvePbes(int)));

    QProcess* pbessolveProcess = createPbessolveProcess(property->name, true);
    pbessolveProcess->setProperty("pid", processid);
    connect(pbessolveProcess, SIGNAL(finished(int)), this,
            SLOT(createLts(int)));

    QProcess* lps2ltsProcess = createLps2ltsProcess(true, property->name);
    lps2ltsProcess->setProperty("pid", processid);
    connect(lps2ltsProcess, SIGNAL(finished(int)), this, SLOT(showLts(int)));

    QProcess* ltsgraphProcess =
        createLtsgraphProcess(LtsReduction::None, true, property->name);
    ltsgraphProcess->setProperty("id", processid);
    connect(ltsgraphProcess, SIGNAL(finished(int)), this,
            SLOT(afterClosingUiTool()));

    processes[processid] = {mcrl2ParsingProcess,    mcrl22lpsProcess,
                            propertyParsingProcess, lps2pbesProcess,
                            pbessolveProcess,       lps2ltsProcess,
                            ltsgraphProcess};
    processTypes[processid] = processType;
    processQueues[processType]->enqueue(processid);
    emit newProcessQueued(processType);

    return processid;
  }
  return -1;
}

void ProcessSystem::startProcess(int processid)
{
  /* all processes start with parsing the specification */
  parseMcrl2(processid);
}

bool ProcessSystem::subprocessSuccessfullyTerminated(int previousExitCode,
                                                     int processid)
{
  /* if the subprocess terminated unsuccessfully, tell everyone and delete the
   *   corresponding process */
  if (previousExitCode > 0)
  {
    consoleDock->writeToConsole(processTypes[processid],
                                "Process finished with an error\n");
    emit processFinished(processid);
    deleteProcess(processid);
  }
  return previousExitCode == 0;
}

void ProcessSystem::parseMcrl2(int processid)
{
  ProcessType processType = processTypes[processid];

  consoleDock->writeToConsole(ProcessType::Parsing,
                              "##### PARSING SPECIFICATION #####\n");
  if (processType != ProcessType::Parsing)
  {
    consoleDock->writeToConsole(processType,
                                "##### PARSING SPECIFICATION #####\n");
  }

  QProcess* mcrl2ParsingProcess = processes[processid][0];
  /* check if we need to run this */
  if (fileSystem->upToDateLpsFileExists())
  {
    consoleDock->writeToConsole(
        ProcessType::Parsing,
        "Parsing is not needed as specification has not changed\n");
    emit mcrl2ParsingProcess->finished(0);
  }
  else
  {
    mcrl2ParsingProcess->start();
  }
}

void ProcessSystem::mcrl2ParsingResult(int previousExitCode)
{
  QProcess* mcrl2ParsingProcess = qobject_cast<QProcess*>(sender());
  int processid = mcrl2ParsingProcess->property("pid").toInt();

  /* if the full process was only for parsing the specification, signal that the
   *   process has finished */
  if (processTypes[processid] == ProcessType::Parsing &&
      processes[processid].size() == 1)
  {
    emit processFinished(processid);
    deleteProcess(processid);
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

  if (subprocessSuccessfullyTerminated(previousExitCode, processid))
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

  if (subprocessSuccessfullyTerminated(previousExitCode, processid))
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
  ProcessType processType = processTypes[processid];

  if (subprocessSuccessfullyTerminated(previousExitCode, processid))
  {
    QProcess* lps2ltsProcess =
        processes[processid][processes[processid].size() - 2];

    consoleDock->writeToConsole(processType, "##### CREATING LTS #####\n");

    /* check if we need to run this */
    bool evidence = lps2ltsProcess->property("evidence").toBool();
    if (fileSystem->upToDateLtsFileExists(
            LtsReduction::None, evidence,
            lps2ltsProcess->property("propertyName").toString()))
    {
      consoleDock->writeToConsole(
          processType, "Up to date" + QString(evidence ? " evidence" : "") +
                           " LTS already exists\n");
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

  if (subprocessSuccessfullyTerminated(previousExitCode, processid))
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
                                  "Up to date LTS already exists\n");
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

  if (subprocessSuccessfullyTerminated(previousExitCode, processid))
  {
    QProcess* ltsgraphProcess = processes[processid].back();

    consoleDock->writeToConsole(processTypes[processid],
                                "##### SHOWING LTS #####\n");
    ltsgraphProcess->start();
    emit processFinished(processid);
  }
}

void ProcessSystem::parseMcf(int previousExitCode)
{
  int processid = qobject_cast<QProcess*>(sender())->property("pid").toInt();
  ProcessType processType = processTypes[processid];

  if (subprocessSuccessfullyTerminated(previousExitCode, processid))
  {
    consoleDock->writeToConsole(ProcessType::Parsing,
                                "##### PARSING PROPERTY #####\n");
    if (processType != ProcessType::Parsing)
    {
      consoleDock->writeToConsole(processType,
                                  "##### PARSING PROPERTY #####\n");
    }

    processes[processid][2]->start();
  }
}

void ProcessSystem::mcfParsingResult(int previousExitCode)
{
  int processid = qobject_cast<QProcess*>(sender())->property("pid").toInt();
  ProcessType processType = processTypes[processid];

  /* if parsing gave an error, move to parsing tab */
  if (previousExitCode > 0)
  {
    consoleDock->setConsoleTab(ProcessType::Parsing);
  }

  /* if this belonged to a parsing process, write the result and emit that the
   *   process has finished */
  if (processType == ProcessType::Parsing)
  {
    if (previousExitCode == 0)
    {
      results[processid] = "valid";
    }
    else
    {
      results[processid] = "invalid";
    }

    emit processFinished(processid);
    deleteProcess(processid);
  }
}

void ProcessSystem::createPbes(int previousExitCode)
{
  int processid = qobject_cast<QProcess*>(sender())->property("pid").toInt();

  if (subprocessSuccessfullyTerminated(previousExitCode, processid))
  {
    QProcess* lps2pbesProcess = processes[processid][3];

    consoleDock->writeToConsole(ProcessType::Verification,
                                "##### CREATING PBES #####\n");

    /* check if we need to run this */
    if (fileSystem->upToDatePbesFileExists(
            lps2pbesProcess->property("propertyName").toString(),
            lps2pbesProcess->property("evidence").toBool()))
    {
      consoleDock->writeToConsole(ProcessType::Verification,
                                  "Up to date PBES already exists\n");
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
  QProcess* pbessolveProcess = processes[processid][4];

  if (subprocessSuccessfullyTerminated(previousExitCode, processid))
  {
    consoleDock->writeToConsole(ProcessType::Verification,
                                "##### SOLVING PBES #####\n");

    /* in case of generating evidence, check if we need to run this */
    bool evidence = pbessolveProcess->property("evidence").toBool();
    if (evidence)
    {
      if (fileSystem->upToDateLpsFileExists(
              evidence, pbessolveProcess->property("propertyName").toString()))
      {
        consoleDock->writeToConsole(ProcessType::Verification,
                                    "Up to date evidence LPS already exists\n");
        emit pbessolveProcess->finished(0);
      }
      else
      {
        pbessolveProcess->start();
      }
    }
    else
    {
      pbessolveProcess->start();
    }
  }
}

void ProcessSystem::verificationResult(int previousExitCode)
{
  int processid = qobject_cast<QProcess*>(sender())->property("pid").toInt();

  if (subprocessSuccessfullyTerminated(previousExitCode, processid))
  {
    std::string output =
        processes[processid][4]->readAllStandardOutput().toStdString();
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
    deleteProcess(processid);
  }
}

void ProcessSystem::afterClosingUiTool()
{
  int processid = qobject_cast<QProcess*>(sender())->property("pid").toInt();
  deleteProcess(processid);
}

void ProcessSystem::abortProcess(int processid)
{
  ProcessType processType = processTypes[processid];
  bool aborted;

  /* if this process is running, terminate it */
  if (processThreads[processType]->getCurrentProcessId() == processid)
  {
    for (QProcess* process : processes[processid])
    {
      process->blockSignals(true);
      process->kill();
    }
    aborted = true;
  }
  else
  {
    /* if it is not running, simply remove it from the queue */
    aborted = processQueues[processType]->removeOne(processid);
  }

  if (aborted)
  {
    emit processFinished(processid);
    deleteProcess(processid);
    consoleDock->writeToConsole(processTypes[processid],
                                "##### PROCESS WAS ABORTED #####\n");
  }
}

void ProcessSystem::abortAllProcesses(ProcessType processType)
{
  /* first empty the queue */
  QQueue<int>* processQueue = processQueues[processType];
  foreach (int processid, *processQueue)
  {
    emit processFinished(processid);
    deleteProcess(processid);
  }
  processQueues[processType]->clear();

  /* then stop the process run by the thread */
  int processid = processThreads[processType]->getCurrentProcessId();
  if (processid >= 0)
  {
    if (processes.count(processid) > 0)
    {
      for (QProcess* process : processes[processid])
      {
        process->blockSignals(true);
        process->kill();
      }
    }
    emit processFinished(processid);
    deleteProcess(processid);
  }

  consoleDock->writeToConsole(processType,
                              "##### ABORTED ALL PROCESSES #####\n");
}

void ProcessSystem::deleteProcess(int processid)
{
  for (QProcess* subprocess : processes[processid])
  {
    delete subprocess;
  }
  processes.erase(processid);
  processTypes.erase(processid);
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
