// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/utilities/toolset_version.h"
#include "processsystem.h"

#include <QEventLoop>

ProcessThread::ProcessThread(QQueue<int>* processQueue, ProcessType processType)
    : processQueue(processQueue), processType(processType), running(false),
      currentProcessid(-1)
{
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
    : fileSystem(fileSystem), pid(0)
{
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
    processThreads[processType]->deleteLater();
  }
}

void ProcessSystem::setConsoleDock(ConsoleDock* consoleDock)
{
  this->consoleDock = consoleDock;
}

void ProcessSystem::testExecutableExistence()
{
  /* get this tool's version */
  QString mcrl2ideVersion =
      QString::fromStdString(mcrl2::utilities::get_toolset_version());

  QStringList tools = {"mcrl22lps",  "lpsxsim",  "lps2lts",  "ltsconvert",
                       "ltscompare", "ltsgraph", "lps2pbes", "pbessolve",
                       "mcrl2i"};

  /* for each necessary executable, check if it exists by trying to run it and
   *   compare its version with mcrl2ide's version */
  for (QString tool : tools)
  {
    /* try to run the tool */
    QProcess process;
    process.start(fileSystem->toolPath(tool), {"--version"});
    bool started = process.waitForStarted();

    if (started)
    {
      /* if found, check if the tool gives expected output */
      process.waitForFinished();
      QString output = process.readAllStandardOutput();
      QStringList splittedOutput = output.split(QRegularExpression("[ \r\n]"));

      if (!(splittedOutput.length() > 2 && splittedOutput[1] == "mCRL2" &&
            splittedOutput[2] == "toolset"))
      {
        consoleDock->broadcast(
            "WARNING: The executable of tool " + tool +
            " produced unexpected version output. Either the version output "
            "has changed or the tool has crashed (using --version).\n");
      }
    }
    else
    {
      consoleDock->broadcast("WARNING: The executable of tool " + tool +
                             " could not be found! Make sure it is in the "
                             "running directory or in PATH.\n");
    }
  }
}

ProcessThread* ProcessSystem::getProcessThread(ProcessType processType)
{
  return processThreads[processType];
}

bool ProcessSystem::isThreadRunning(ProcessType processType)
{
  return processThreads[processType]->isRunning();
}

QProcess*
ProcessSystem::createSubprocess(SubprocessType subprocessType, 
  int processid,
  int subprocessIndex,
  mcrl2::lts::lts_equivalence equivalence)
{
  return createSubprocess(subprocessType, processid, subprocessIndex,
                          Property(), QString(), false, equivalence, SpecType::Main);
}

QProcess*
ProcessSystem::createSubprocess(SubprocessType subprocessType, 
  int processid,
  int subprocessIndex, 
  const Property& property,
  mcrl2::lts::lts_equivalence equivalence)
{
  return createSubprocess(subprocessType, processid, subprocessIndex,  property, QString(), 
                          false, equivalence, SpecType::Main);
}

QProcess* ProcessSystem::createSubprocess(SubprocessType subprocessType,
  int processid,
  int subprocessIndex,
  const Property& property,
  SpecType specType)
{
  return createSubprocess(subprocessType, processid, subprocessIndex, property, QString(), 
                          false, mcrl2::lts::lts_eq_none, specType);
}

QProcess* ProcessSystem::createSubprocess(
    SubprocessType subprocessType, 
    int processid, 
    int subprocessIndex,
    const Property& property,
    const QString& expression,
    bool evidence,
    mcrl2::lts::lts_equivalence equivalence, 
    SpecType specType)
{
  // TODO: This function combines all parameters for the different processes, which is confusing.
  QProcess* subprocess = new QProcess();
  subprocess->setWorkingDirectory(fileSystem->temporaryFolderPath());

  ProcessType processType = processTypes[processid];

  /* connect to logger */
  if (subprocessType == SubprocessType::ParseMcrl2 ||
      subprocessType == SubprocessType::ParseMcf)
  {
    connect(subprocess, SIGNAL(readyReadStandardError()), consoleDock,
            SLOT(logToParsingConsole()));
  }
  else
  {
    switch (processType)
    {
    case ProcessType::Parsing:
      connect(subprocess, SIGNAL(readyReadStandardError()), consoleDock,
              SLOT(logToParsingConsole()));
      break;
    case ProcessType::Simulation:
      connect(subprocess, SIGNAL(readyReadStandardError()), consoleDock,
              SLOT(logToSimulationConsole()));
      break;
    case ProcessType::LtsCreation:
      connect(subprocess, SIGNAL(readyReadStandardError()), consoleDock,
              SLOT(logToLtsCreationConsole()));
      break;
    case ProcessType::Verification:
      connect(subprocess, SIGNAL(readyReadStandardError()), consoleDock,
              SLOT(logToVerificationConsole()));
      break;
    case ProcessType::Rewriting:
      connect(subprocess, SIGNAL(readyReadStandardError()), consoleDock,
              SLOT(logToRewriteConsole()));
      break;
    default:
      break;
    }
  }

  /* connect the subprocess to the subprocess handler to execute the next one
   *   when finished */
  connect(subprocess, SIGNAL(finished(int, QProcess::ExitStatus)), this,
          SLOT(executeNextSubprocess(int)));

  /* the subprocess should delete itself when finished */
  connect(subprocess, SIGNAL(finished(int, QProcess::ExitStatus)), subprocess,
          SLOT(deleteLater()));

  /* add properties we might need */
  subprocess->setProperty("processid", processid);
  subprocess->setProperty("subprocessType", int(subprocessType));
  subprocess->setProperty("subprocessIndex", subprocessIndex);
  subprocess->setProperty("propertyName", property.name);
  subprocess->setProperty("evidence", evidence);
  subprocess->setProperty("specType", int(specType));

  QString program = "";
  QString inputFile = "";
  QString inputFile2 = "";
  QString outputFile = "";
  QStringList arguments = {};

  /* set program, arguments and more connects depending on the type of the
   *   subprocess */
  switch (subprocessType)
  {
  case SubprocessType::ParseMcrl2:
    arguments << "--check-only";
    connect(subprocess, SIGNAL(finished(int, QProcess::ExitStatus)), this,
            SLOT(mcrl2ParsingResult(int)));

    [[fallthrough]];
  case SubprocessType::Mcrl22lps:
    program = "mcrl22lps";
    inputFile = fileSystem->specificationFilePath(specType, property.name);
    outputFile = fileSystem->lpsFilePath(specType, property.name, evidence);
    arguments << inputFile 
              << outputFile 
              << "--verbose"
              << QString("--lin-method=").append(QString::fromStdString(mcrl2::lps::print_lin_method(fileSystem->linearisationMethod())));
    break;

  case SubprocessType::Lpsxsim:
    program = "lpsxsim";
    inputFile = fileSystem->lpsFilePath();
    arguments << inputFile 
              << QString("-Q") + std::to_string(fileSystem->enumerationLimit()).c_str();
    break;

  case SubprocessType::Lps2lts:
    program = "lps2lts";
    inputFile = fileSystem->lpsFilePath(specType, property.name, evidence);
    outputFile = fileSystem->ltsFilePath(mcrl2::lts::lts_eq_none, specType,
                                         property.name, evidence);
    arguments << inputFile << outputFile
              << "--strategy=breadth"
              << "--verbose"
              << QString("-Q") + std::to_string(fileSystem->enumerationLimit()).c_str();
    if (fileSystem->enableJittyc())
    {
#ifdef MCRL2_ENABLE_JITTYC
      arguments<< "--rewriter=jittyc";
#else    
      consoleDock->writeToConsole(
          ProcessType::LtsCreation,
          "The compiling rewriter (-rjittyc) is not available on this platform, tool option will be ignored!\n");
#endif // MCRL2_JITTYC_ENABLED
    }
    break;

  case SubprocessType::Ltsconvert:
    program = "ltsconvert";
    inputFile = fileSystem->ltsFilePath();
    outputFile = fileSystem->ltsFilePath(equivalence);
    arguments << inputFile << outputFile << "--verbose"
              << "--equivalence=" +
                     QString::fromStdString(
                         mcrl2::lts::print_equivalence(equivalence));
    break;

  case SubprocessType::Ltscompare:
    program = "ltscompare";
    inputFile = fileSystem->ltsFilePath(mcrl2::lts::lts_eq_none,
                                        SpecType::First, property.name);
    inputFile2 = fileSystem->ltsFilePath(mcrl2::lts::lts_eq_none,
                                         SpecType::Second, property.name);
    arguments << "--equivalence=" +
                     QString::fromStdString(
                         mcrl2::lts::print_equivalence(equivalence))
              << inputFile << inputFile2;

    connect(subprocess, SIGNAL(finished(int, QProcess::ExitStatus)), this,
            SLOT(verificationResult(int)));
    break;

  case SubprocessType::Ltsgraph:
    program = "ltsgraph";
    inputFile =
        fileSystem->ltsFilePath(equivalence, specType, property.name, evidence);
    arguments << inputFile;
    break;

  case SubprocessType::ParseMcf:
    arguments << "--check-only";
    connect(subprocess, SIGNAL(finished(int, QProcess::ExitStatus)), this,
            SLOT(mcfParsingResult(int)));

    [[fallthrough]];
  case SubprocessType::Lps2pbes:
    program = "lps2pbes";
    inputFile = fileSystem->lpsFilePath();
    inputFile2 = fileSystem->propertyFilePath(property);
    outputFile = fileSystem->pbesFilePath(property.name, evidence);
    arguments << inputFile << outputFile << "--formula=" + inputFile2
              << "--out=pbes"
              << "--verbose";
    if (evidence)
    {
      arguments << "--counter-example";
    }
    break;

  case SubprocessType::Pbessolve:
    program = "pbessolve";
    inputFile = fileSystem->pbesFilePath(property.name, evidence);
    arguments << inputFile << "--in=pbes"
              << "--search-strategy=breadth-first"
              << "--solve-strategy=0"
              << "--verbose"
              << QString("-Q") + std::to_string(fileSystem->enumerationLimit()).c_str();
    if (fileSystem->enableJittyc())
    {
#ifdef MCRL2_ENABLE_JITTYC
      arguments<< "--rewriter=jittyc";
#else    
      consoleDock->writeToConsole(
          ProcessType::Verification,
          "The compiling rewriter (-rjittyc) is not available on this platform, tool option will be ignored!\n");
#endif // MCRL2_JITTYC_ENABLED
    }

    if (evidence)
    {
      inputFile2 = fileSystem->lpsFilePath();
      outputFile = fileSystem->lpsFilePath(specType, property.name, evidence);
      arguments << "--file=" + inputFile2 << "--evidence-file=" + outputFile;
    }
    else
    {
      connect(subprocess, SIGNAL(finished(int, QProcess::ExitStatus)), this,
              SLOT(verificationResult(int)));
    }
    break;

  case SubprocessType::Mcrl2i:
    program = "mcrl2i";
    inputFile = fileSystem->lpsFilePath();
    arguments << inputFile << "-e" << expression 
              << QString("-Q") + std::to_string(fileSystem->enumerationLimit()).c_str();

    connect(subprocess, SIGNAL(finished(int, QProcess::ExitStatus)), this,
            SLOT(rewriteResult(int)));

    break;

  default:
    break;
  }

  subprocess->setProgram(fileSystem->toolPath(program));
  subprocess->setArguments(arguments);
  subprocess->setProperty("inputFile", inputFile);
  subprocess->setProperty("inputFile2", inputFile2);
  subprocess->setProperty("outputFile", outputFile);

  return subprocess;
}

int ProcessSystem::parseSpecification()
{
  if (fileSystem->save())
  {
    /* create the subprocesses */
    int processid = pid++;
    ProcessType processType = ProcessType::Parsing;
    processTypes[processid] = processType;
    consoleDock->setConsoleTab(processType);

    processes[processid] = {
        createSubprocess(SubprocessType::ParseMcrl2, processid, 0)};
    processQueues[processType]->enqueue(processid);
    emit newProcessQueued(processType);

    return processid;
  }
  return -1;
}

int ProcessSystem::simulate()
{
  if (fileSystem->save())
  {
    /* create the subprocesses */
    int processid = pid++;
    ProcessType processType = ProcessType::Simulation;
    processTypes[processid] = processType;
    consoleDock->setConsoleTab(processType);

    processes[processid] = {
        createSubprocess(SubprocessType::ParseMcrl2, processid, 0),
        createSubprocess(SubprocessType::Mcrl22lps, processid, 1),
        createSubprocess(SubprocessType::Lpsxsim, processid, 2)};
    processQueues[processType]->enqueue(processid);
    emit newProcessQueued(processType);

    return processid;
  }
  return -1;
}

int ProcessSystem::rewriteExpression(std::string expression)
{
    /* create the subprocesses */
    int processid = pid++;
    ProcessType processType = ProcessType::Rewriting;
    processTypes[processid] = processType;
    consoleDock->setConsoleTab(processType);

    processes[processid] = {
        createSubprocess(SubprocessType::ParseMcrl2, processid, 0),
        createSubprocess(SubprocessType::Mcrl22lps, processid, 1),
        createSubprocess(SubprocessType::Mcrl2i, processid, 2, Property(), QString::fromStdString(expression))
    };

    processQueues[processType]->enqueue(processid);
    
    emit newProcessQueued(processType);

    return processid;
}

int ProcessSystem::showLts(mcrl2::lts::lts_equivalence reduction)
{
  if (fileSystem->save())
  {
    /* create the subprocesses */
    std::vector<QProcess*> showLtsProcesses = {};
    int processid = pid++;
    ProcessType processType = ProcessType::LtsCreation;
    processTypes[processid] = processType;
    consoleDock->setConsoleTab(processType);
    bool noReduction = reduction == mcrl2::lts::lts_eq_none;

    showLtsProcesses.push_back(
        createSubprocess(SubprocessType::ParseMcrl2, processid, 0));
    showLtsProcesses.push_back(
        createSubprocess(SubprocessType::Mcrl22lps, processid, 1));
    showLtsProcesses.push_back(
        createSubprocess(SubprocessType::Lps2lts, processid, 2));
    /* create the ltsconvert process if we need to reduce */
    if (!noReduction)
    {
      showLtsProcesses.push_back(createSubprocess(SubprocessType::Ltsconvert,
                                                  processid, 3, reduction));
    }
    showLtsProcesses.push_back(createSubprocess(
        SubprocessType::Ltsgraph, processid, noReduction ? 3 : 4, reduction));

    processes[processid] = showLtsProcesses;
    processQueues[processType]->enqueue(processid);
    emit newProcessQueued(processType);

    return processid;
  }
  return -1;
}

int ProcessSystem::parseProperty(const Property& property)
{
  if (fileSystem->save())
  {
    /* create the subprocesses */
    int processid = pid++;
    ProcessType processType = ProcessType::Parsing;
    processTypes[processid] = processType;
    consoleDock->setConsoleTab(processType);

    if (property.mucalculus)
    {
      processes[processid] = {
          createSubprocess(SubprocessType::ParseMcrl2, processid, 0),
          createSubprocess(SubprocessType::Mcrl22lps, processid, 1),
          createSubprocess(SubprocessType::ParseMcf, processid, 2, property)};
    }
    else
    {
      fileSystem->createReinitialisedSpecification(property, SpecType::First);
      fileSystem->createReinitialisedSpecification(property, SpecType::Second);
      processes[processid] = {
          createSubprocess(SubprocessType::ParseMcrl2, processid, 0, property,
                           SpecType::First),
          createSubprocess(SubprocessType::ParseMcrl2, processid, 1, property,
                           SpecType::Second)};
    }

    processQueues[processType]->enqueue(processid);
    emit newProcessQueued(processType);

    return processid;
  }
  return -1;
}

int ProcessSystem::verifyProperty(const Property& property)
{
  if (fileSystem->save())
  {
    /* create the subprocesses */
    int processid = pid++;
    ProcessType processType = ProcessType::Verification;
    processTypes[processid] = processType;
    consoleDock->setConsoleTab(processType);

    if (property.mucalculus)
    {
      processes[processid] = {
          createSubprocess(SubprocessType::ParseMcrl2, processid, 0),
          createSubprocess(SubprocessType::Mcrl22lps, processid, 1),
          createSubprocess(SubprocessType::ParseMcf, processid, 2, property),
          createSubprocess(SubprocessType::Lps2pbes, processid, 3, property),
          createSubprocess(SubprocessType::Pbessolve, processid, 4, property)};
    }
    else
    {
      fileSystem->createReinitialisedSpecification(property, SpecType::First);
      fileSystem->createReinitialisedSpecification(property, SpecType::Second);
      processes[processid] = {
          createSubprocess(SubprocessType::ParseMcrl2, processid, 0, property,
                           SpecType::First),
          createSubprocess(SubprocessType::ParseMcrl2, processid, 1, property,
                           SpecType::Second),
          createSubprocess(SubprocessType::Mcrl22lps, processid, 2, property,
                           SpecType::First),
          createSubprocess(SubprocessType::Mcrl22lps, processid, 3, property,
                           SpecType::Second),
          createSubprocess(SubprocessType::Lps2lts, processid, 4, property,
                           SpecType::First),
          createSubprocess(SubprocessType::Lps2lts, processid, 5, property,
                           SpecType::Second),
          createSubprocess(SubprocessType::Ltscompare, processid, 6, property,
                           property.equivalence)};
    }
    processQueues[processType]->enqueue(processid);
    emit newProcessQueued(processType);

    return processid;
  }
  return -1;
}

int ProcessSystem::showEvidence(const Property& property)
{
  if (fileSystem->save())
  {
    /* create the subprocesses */
    int processid = pid++;
    ProcessType processType = ProcessType::Verification;
    processTypes[processid] = processType;
    consoleDock->setConsoleTab(processType);

    processes[processid] = {
        createSubprocess(SubprocessType::ParseMcrl2, processid, 0),
        createSubprocess(SubprocessType::Mcrl22lps, processid, 1),
        createSubprocess(SubprocessType::ParseMcf, processid, 2, property),
        createSubprocess(SubprocessType::Lps2pbes, processid, 3, property,
                         QString(), true),
        createSubprocess(SubprocessType::Pbessolve, processid, 4, property, QString(),
                         true),
        createSubprocess(SubprocessType::Lps2lts, processid, 5, property, QString(),true),
        createSubprocess(SubprocessType::Ltsgraph, processid, 6, property, QString(),
                         true)};
    processQueues[processType]->enqueue(processid);
    emit newProcessQueued(processType);

    return processid;
  }
  return -1;
}

void ProcessSystem::startProcess(int processid)
{
  executeNextSubprocess(0, processid);
}

void ProcessSystem::executeNextSubprocess(int previousExitCode, int processid)
{
  int nextSubprocessIndex;
  /* if the processid is set we need to run the first subprocess, else get info
   *   from the previous process */
  if (processid >= 0)
  {
    nextSubprocessIndex = 0;
  }
  else
  {
    QProcess* previousSubprocess = qobject_cast<QProcess*>(sender());
    processid = previousSubprocess->property("processid").toInt();
    nextSubprocessIndex =
        previousSubprocess->property("subprocessIndex").toInt() + 1;
    SubprocessType previousSubprocessType = static_cast<SubprocessType>(
        previousSubprocess->property("subprocessType").toInt());

    /* if there are more subprocesses to run, check if the previous subprocess
     *   terminated successfully */
    if (nextSubprocessIndex < int(processes[processid].size()) &&
        previousExitCode > 0)
    {
      /* if not, abort the process */
      /* in case the previous subprocess was for parsing mcrl2, let
       *   parseMcrl2Result handle finishing the process to prevent a race
       *   condition with storing the result of parsing */
      if (previousSubprocessType != SubprocessType::ParseMcrl2)
      {
        consoleDock->writeToConsole(processTypes[processid],
                                    "Process finished unsuccessfully\n");
        emit processFinished(processid);
      }
      deleteProcess(processid, nextSubprocessIndex);
      return;
    }
  }

  /* if the previous process was the last process, we are done */
  if (nextSubprocessIndex < int(processes[processid].size()))
  {

    /* get the process and its properties */
    ProcessType processType = processTypes[processid];
    QProcess* subprocess = processes[processid][nextSubprocessIndex];

    SubprocessType subprocessType = static_cast<SubprocessType>(
        subprocess->property("subprocessType").toInt());
    QString inputFile = subprocess->property("inputFile").toString();
    QString inputFile2 = subprocess->property("inputFile2").toString();
    QString outputFile = subprocess->property("outputFile").toString();
    QString propertyName = subprocess->property("propertyName").toString();
    bool evidence = subprocess->property("evidence").toBool();
    SpecType specType =
        static_cast<SpecType>(subprocess->property("specType").toInt());

    bool noNeedToRun = false;

    /* prepare for execution (mention in console, check if the subprocess can
     *   be skipped, emit processFinished when executing a UI tool) */
    switch (subprocessType)
    {
    case SubprocessType::ParseMcrl2:
      consoleDock->writeToConsole(ProcessType::Parsing,
                                  "##### PARSING SPECIFICATION #####\n");
      if (processType != ProcessType::Parsing)
      {
        consoleDock->writeToConsole(processType,
                                    "##### PARSING SPECIFICATION #####\n");
      }

      /* no need to parse the main mcrl2 file when there is an up to date lps
       *   file */
      if (specType == SpecType::Main &&
          fileSystem->upToDateOutputFileExists(inputFile, outputFile))
      {
        noNeedToRun = true;
        consoleDock->writeToConsole(
            ProcessType::Parsing,
            "Parsing is not needed as specification has not changed\n");
      }
      break;

    case SubprocessType::Mcrl22lps:
      consoleDock->writeToConsole(processType, "##### CREATING LPS #####\n");

      /* no need to run if there is an up to date lps file with respect to the
       *   input mcrl2 file */
      if (fileSystem->upToDateOutputFileExists(inputFile, outputFile))
      {
        noNeedToRun = true;
        consoleDock->writeToConsole(processType,
                                    "Up to date LPS already exists\n");
      }
      break;

    case SubprocessType::Lpsxsim:
      consoleDock->writeToConsole(processType,
                                  "##### SHOWING SIMULATION #####\n");

      emit processFinished(processid);
      break;

    case SubprocessType::Lps2lts:
      consoleDock->writeToConsole(processType, "##### CREATING LTS #####\n");

      /* no need to run if there is an up to date lts file with respect to the
       *   input lps file */
      if (fileSystem->upToDateOutputFileExists(inputFile, outputFile))
      {
        noNeedToRun = true;
        consoleDock->writeToConsole(
            processType, "Up to date" + QString(evidence ? " evidence" : "") +
                             " LTS already exists\n");
      }
      break;

    case SubprocessType::Ltsconvert:
      consoleDock->writeToConsole(processType, "##### REDUCING LTS #####\n");

      /* no need to run if there is an up to date reduced lts file with
       *   respect to the input unreduced lts file */
      if (fileSystem->upToDateOutputFileExists(inputFile, outputFile))
      {
        noNeedToRun = true;
        consoleDock->writeToConsole(processType,
                                    "Up to date LTS already exists\n");
      }
      break;

    case SubprocessType::Ltscompare:
      consoleDock->writeToConsole(processType, "##### COMPARING LTSS #####\n");
      break;

    case SubprocessType::Ltsgraph:
      consoleDock->writeToConsole(processType, "##### SHOWING LTS #####\n");

      emit processFinished(processid);
      break;

    case SubprocessType::ParseMcf:
      consoleDock->writeToConsole(ProcessType::Parsing,
                                  "##### PARSING PROPERTY " +
                                      propertyName.toUpper() + " #####\n");
      if (processType != ProcessType::Parsing)
      {
        consoleDock->writeToConsole(processType, "##### PARSING PROPERTY " +
                                                     propertyName.toUpper() +
                                                     " #####\n");
      }
      break;

    case SubprocessType::Lps2pbes:
      consoleDock->writeToConsole(processType, "##### CREATING PBES #####\n");

      /* no need to run if there is an up to date pbes file with respect to
       *   the input lps and property files */
      if (fileSystem->upToDateOutputFileExists(inputFile, outputFile,
                                               inputFile2))
      {
        noNeedToRun = true;
        consoleDock->writeToConsole(processType,
                                    "Up to date PBES already exists\n");
      }
      break;

    case SubprocessType::Pbessolve:
      consoleDock->writeToConsole(processType, "##### SOLVING PBES #####\n");

      /* no need to run if there is an up to date evidence lps file with
       *   respect to the input pbes and lps files */
      if (evidence && fileSystem->upToDateOutputFileExists(
                          inputFile, outputFile, inputFile2))
      {
        noNeedToRun = true;
        consoleDock->writeToConsole(ProcessType::Verification,
                                    "Up to date evidence LPS already exists\n");
      }
      break;

    case SubprocessType::Mcrl2i:
      consoleDock->writeToConsole(processType, "##### REWRITING EXPRESSION #####\n");

    default:
      break;
    }

    /* if we can skip this subprocess act like it has finished, else execute
     *   it */
    if (noNeedToRun)
    {
      emit subprocess->finished(0, QProcess::ExitStatus::NormalExit);
    }
    else
    {
      subprocess->start();
    }
  }
}

void ProcessSystem::mcrl2ParsingResult(int previousExitCode)
{
  QProcess* mcrl2ParsingProcess = qobject_cast<QProcess*>(sender());
  int processid = mcrl2ParsingProcess->property("processid").toInt();
  ProcessType processType = processTypes[processid];
  int subprocessIndex =
      mcrl2ParsingProcess->property("subprocessIndex").toInt();
  SpecType specType =
      static_cast<SpecType>(mcrl2ParsingProcess->property("specType").toInt());

  /* if parsing resulted in an error, go to the parsing tab */
  if (previousExitCode > 0)
  {
    consoleDock->setConsoleTab(ProcessType::Parsing);
  }

  /* if parsing a reinitialised specification, set the corresponding result */
  if (processType == ProcessType::Parsing && specType != SpecType::Main)
  {
    if (previousExitCode == 0)
    {
      results[processid] = "valid";
    }
    else
    {
      results[processid] = "invalid" + SPECTYPEEXTENSION.at(specType);
    }
  }

  /* if parsing failed or this was the last subprocess, signal that the process
   *   has finished */
  if (previousExitCode > 0 ||
      subprocessIndex + 1 == int(processes[processid].size()))
  {
    emit processFinished(processid);
  }

  /* if parsing the main specification gave an error, move the cursor in the
   *   code editor to the parsing error if possible */
  if (specType == SpecType::Main && previousExitCode > 0)
  {
    consoleDock->writeToConsole(
        ProcessType::Parsing,
        "The given specification is not a valid mCRL2 specification\n");
    QString parsingOutput = consoleDock->getConsoleOutput(ProcessType::Parsing);
    QRegularExpression parsingError = QRegularExpression("Line (\\d+), column (\\d+): syntax error");
    QRegularExpressionMatch match;
    int parsingErrorIndex =
        parsingOutput.indexOf(parsingError, parsingOutput.lastIndexOf("#####"), &match);
    if (parsingErrorIndex >= 0)
    {
      fileSystem->setSpecificationEditorCursor(match.captured(1).toInt(),
        match.captured(2).toInt());
    }
  }
}

void ProcessSystem::mcfParsingResult(int previousExitCode)
{
  int processid =
      qobject_cast<QProcess*>(sender())->property("processid").toInt();
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
  }
}

void ProcessSystem::verificationResult(int previousExitCode)
{
  QProcess* previousProcess = qobject_cast<QProcess*>(sender());
  int processid = previousProcess->property("processid").toInt();
  QString propertyName = previousProcess->property("propertyName").toString();

  if (previousExitCode == 0)
  {
    QString output = QString(previousProcess->readAllStandardOutput());
    if (output.startsWith("true"))
    {
      results[processid] = "true";
      consoleDock->writeToConsole(
          ProcessType::Verification,
          "The property " + propertyName +
              " on this specification evaluates to true\n");
    }
    else if (output.startsWith("false"))
    {
      results[processid] = "false";
      consoleDock->writeToConsole(
          ProcessType::Verification,
          "The property " + propertyName +
              " on this specification evaluates to false\n");
    }
  }
  emit processFinished(processid);
}

void ProcessSystem::rewriteResult(int previousExitCode)
{
  QProcess* previousProcess = qobject_cast<QProcess*>(sender());
  int processid = previousProcess->property("processid").toInt();

  if (previousExitCode == 0)
  {
    results[processid] = QString(previousProcess->readAllStandardOutput());
  }
    
  emit processFinished(processid);
}

void ProcessSystem::abortProcess(int processid)
{
  ProcessType processType = processTypes[processid];
  bool aborted;

  /* if this process is running, terminate it */
  if (processThreads[processType]->getCurrentProcessId() == processid)
  {
    killProcess(processid);
    aborted = true;
  }
  else
  {
    /* if it is not running, simply remove it from the queue */
    aborted = processQueues[processType]->removeOne(processid);
    if (aborted)
    {
      deleteProcess(processid);
    }
  }

  if (aborted)
  {
    emit processFinished(processid);
    consoleDock->writeToConsole(processTypes[processid],
                                "##### ABORTING PROCESS #####\n");
  }
}

void ProcessSystem::abortAllProcesses(ProcessType processType)
{
  /* first empty the queue */
  QQueue<int>* processQueue = processQueues[processType];
  for (int processid : *processQueue)
  {
    emit processFinished(processid);
    deleteProcess(processid);
  }
  processQueue->clear();

  /* then stop the process run by the thread */
  int processid = processThreads[processType]->getCurrentProcessId();
  if (processid >= 0 && processes.count(processid) > 0)
  {
    killProcess(processid);
    emit processFinished(processid);
  }

  consoleDock->writeToConsole(processType,
                              "##### ABORTING ALL " +
                                  PROCESSTYPENAMES.at(processType).toUpper() +
                                  " PROCESSES #####\n");
}

void ProcessSystem::killProcess(int processid)
{
  std::vector<QProcess*> subprocesses = processes[processid];
  int numSubprocesses = int(subprocesses.size());

  int i;
  /* first find which subprocess is running */
  for (i = numSubprocesses - 1; i >= 0; i--)
  {
    QProcess* subprocess = subprocesses[i];
    if (subprocess->state() == QProcess::Running ||
        subprocess->error() != QProcess::UnknownError)
    {
      /* kill the running subprocess */
      subprocess->blockSignals(true);
      subprocess->kill();
      /* remove the output file of the running process if applicable to prevent
       *   that caching uses incomplete output files */
      subprocess->waitForFinished();
      QFile::remove(subprocess->property("outputFile").toString());
      break;
    }
  }
  /* if the running subprocess was not the last subprocess, delete all following
   *   subprocesses */
  if (i < numSubprocesses - 1)
  {
    deleteProcess(processid, i + 1);
  }
}

void ProcessSystem::deleteProcess(int processid, int fromSubprocessIndex)
{
  std::vector<QProcess*> subprocesses = processes[processid];
  for (int i = fromSubprocessIndex; i < int(subprocesses.size()); i++)
  {
    subprocesses[i]->deleteLater();
  }
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
