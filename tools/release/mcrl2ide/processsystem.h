// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef PROCESSSYSTEM_H
#define PROCESSSYSTEM_H

#include "filesystem.h"
#include "consoledock.h"

#include <QQueue>
#include <QThread>

/**
 * @brief SubProcessType Defines all possible subprocess
 */
enum class SubprocessType
{
  ParseMcrl2 = 0,
  Mcrl22lps = 1,
  Lpsxsim = 2,
  Lps2lts = 3,
  Ltsconvert = 4,
  Ltscompare = 5,
  Ltsgraph = 6,
  ParseMcf = 7,
  Lps2pbes = 8,
  Pbessolve = 9,
  Mcrl2i = 10,
};

/**
 * @brief The ProcessThread class defines a thread that makes sure that
 *   processes of certain types (such as verification) happen after each other
 */
class ProcessThread : public QThread
{
  Q_OBJECT

public:
  /**
   * @brief ProcessThread Constructor
   * @param processQueue The queue this thread needs to take the processes from
   * @param processType What process type this thread is for
   */
  ProcessThread(QQueue<int>* processQueue, ProcessType processType);

  /**
   * @brief run The body of the thread
   */
  void run() override;

  /**
   * @brief isRunning Returns whether this thread is running
   * @return Whether this thread is running
   */
  bool isRunning();

  /**
   * @brief getCurrentProcessId Returns the id of the process this thread is
   *   currently running
   * @return The id of the process this thread is currently running
   */
  int getCurrentProcessId();

  public slots:
  /**
   * @brief newProcessQueued Activates this thread if it is waiting for a new
   *   process
   * @param processType The type of the newly added process
   */
  void newProcessQueued(ProcessType processType);

  /**
   * @brief processFinished Activates this thread if it is waiting for a process
   *   to finish
   * @param processid The id of the process that has finished
   */
  void processFinished(int processid);

  signals:
  /**
   * @brief newProcessInQueue Activates this thread if it is waiting for a new
   *   process
   */
  void newProcessInQueue();

  /**
   * @brief startProcess Tells the process system that a process needs to be
   *   started
   * @param processid The id of the process that needs to be started
   */
  void startProcess(int processid);

  /**
   * @brief currentProcessFinished Activates this thread if it is waiting for a
   *   process to finish
   */
  void currentProcessFinished();

  /**
   * @brief statusChanged Is emitted when this thread moves from running to
   *   waiting or vice versa
   * @param running Whether the thread is running (or waiting)
   * @param processType The process type of this thread
   */
  void statusChanged(bool running, ProcessType processType);

  private:
  QQueue<int>* processQueue;
  ProcessType processType;
  bool running;
  int currentProcessid;
};

/**
 * @brief The ProcessSystem class handles all processes related to mCRL2 tools
 */
class ProcessSystem : public QObject
{
  Q_OBJECT

public:
  /**
   * @brief ProcessSystem Constructor
   * @param fileSystem The file system
   */
  ProcessSystem(FileSystem* fileSystem);
  ~ProcessSystem();

  /**
   * @brief setConsoleDock Assigns the console dock to the process system for
   *   logging
   * @param consoleDock The console dock
   */
  void setConsoleDock(ConsoleDock* consoleDock);

  /**
   * @brief testExecutableExistence Tests whether the needed tool executables
   *   can be found and warns the user if any cannot be found
   */
  void testExecutableExistence();

  /**
   * @brief getProcessThread Returns the process thread of type processType
   * @param processType The type of the processThread
   * @return The process thread of type processType
   */
  ProcessThread* getProcessThread(ProcessType processType);

  /**
   * @brief isThreadRunning Returns whether the thread of the given process type
   *   is running
   * @param processType The process type of the thread
   * @return Whether the thread of the given process type is running
   */
  bool isThreadRunning(ProcessType processType);

  /**
   * @brief parseSpecification Parses the current specification
   * @return The process id of the parsing process
   */
  int parseSpecification();

  /**
   * @brief simulate Simulates the current specification using mcrl22lps and
   *   lpsxsim
   * @return The process id of the simulation process
   */
  int simulate();

  /**
   * Rewrites the given data expression in the context of the g
   */
  int rewriteExpression(std::string expression);

  /**
   * @brief showLts Creates and visualizes the lts of the current specification
   *   using mcrl22lps, lps2lts, optionally ltsconvert and ltsgraph
   * @param reduction What reduction to apply
   * @return The process id of the lts creation process
   */
  int showLts(mcrl2::lts::lts_equivalence reduction);

  /**
   * @brief parseProperty Parses the given property
   * @param property The property to parse
   * @return The process id of the parsing process
   */
  int parseProperty(const Property& property);

  /**
   * @brief verifyProperty Verifies a property using mcrl22lps, lps2pbes and
   *   pbessolve in case of a mu-calculus property or mcrl22lps, lps2lts and
   *   ltscompare in case of an equivalence property
   * @param property The property to verify
   * @return The process id of the verification process
   */
  int verifyProperty(const Property& property);

  /**
   * @brief showEvidence Creates and shows evidence for a (verified) property;
   *   a witness if the property is true, a counterexample if the property is
   *   false. Uses mcrl22lps, lps2pbes, pbessolve, lps2lts and ltsgraph.
   * @param property The property to create evidence for
   * @return the process id of the evidence creation process
   */
  int showEvidence(const Property& property);

  /**
   * @brief abortProcess Aborts a process by making the running subprocess
   *   terminate if it is running, else by removing it from the queue
   * @param processid The process id of the process to abort
   */
  void abortProcess(int processid);

  /**
   * @brief abortAllProcesses Aborts all processes of type processType by
   *   clearing the corresponding queue and killing the corresponding currently
   *   running process
   * @param processType The processType to abort all processes of
   */
  void abortAllProcesses(ProcessType processType);

  /**
   * @brief killProcess Kills a process by killing all its subprocesses
   * @param processid The id of the process to kill
   */
  void killProcess(int processid);

  /**
   * @brief deleteProcess Deletes a process by deleting its subprocesses
   * @param processid The id of the process to delete
   * @param fromSubprocessIndex The subprocess index from which we need to
   *   delete the subprocesses
   */
  void deleteProcess(int processid, int fromSubprocessIndex = 0);

  /**
   * @brief getResult Gets the result of a process; for instance for a
   *   verification process, the result is either "" (in case of error), "false"
   *   or "true"
   * @param processid The id of the process to get the result from
   * @return The result of the process
   */
  QString getResult(int processid);

  signals:
  /**
   * @brief newProcessQueued Is emitted when a new process is added to a queue
   * @param processtype The type of the new process
   */
  void newProcessQueued(ProcessType processType);

  /**
   * @brief processFinished Is emitted when a process has finished
   * @param processid The id of the process that has finished
   */
  void processFinished(int processid);

  private:
  FileSystem* fileSystem;
  ConsoleDock* consoleDock;
  int pid;
  std::map<int, std::vector<QProcess*>> processes;
  std::map<int, ProcessType> processTypes;
  std::map<int, QString> results;
  std::map<ProcessType, QQueue<int>*> processQueues;
  std::map<ProcessType, ProcessThread*> processThreads;

  /**
   * @brief createSubprocess Creates a subprocess (overload: for doing
   *   reductions only)
   * @param subprocessType The subprocess type of the subprocess
   * @param processid The id of the process corresponding to this subprocess
   * @param subprocessIndex The index of this subprocess in the corresponding
   *   process
   * @param equivalence An equivalence
   */
  QProcess* createSubprocess(SubprocessType subprocessType, 
    int processid,
    int subprocessIndex,
    mcrl2::lts::lts_equivalence equivalence);

  /**
   * @brief createSubprocess Creates a subprocess (overload: for verifying
   * equivalence properties only)
   * @param subprocessType The subprocess type of the subprocess
   * @param processid The id of the process corresponding to this subprocess
   * @param subprocessIndex The index of this subprocess in the corresponding
   *   process
   * @param property A property
   * @param equivalence An equivalence
   */
  QProcess* createSubprocess(SubprocessType subprocessType, 
    int processid,
    int subprocessIndex, 
    const Property& property,
    mcrl2::lts::lts_equivalence equivalence);

  /**
   * @brief createSubprocess Creates a subprocess (overload: for handling
   *   reinitialised specifications only)
   * @param subprocessType The subprocess type of the subprocess
   * @param processid The id of the process corresponding to this subprocess
   * @param subprocessIndex The index of this subprocess in the corresponding
   *   process
   * @param property A property
   * @param specType The type of the specification that the process uses
   */
  QProcess* createSubprocess(SubprocessType subprocessType, 
    int processid,    
    int subprocessIndex, 
    const Property& property,
    SpecType specType);

  /**
   * @brief createSubprocess Creates a subprocess
   * @param subprocessType The subprocess type of the subprocess
   * @param processid The id of the process corresponding to this subprocess
   * @param subprocessIndex The index of this subprocess in the corresponding
   *   process
   * @param property A property (if needed)
   * @param evidence Whether the subprocess is for creating evidence
   * @param equivalence An equivalence (if needed)
   * @param specType The type of the specification that the process uses (if
   *   needed)
   */
  QProcess* createSubprocess(
    SubprocessType subprocessType, 
    int processid, 
    int subprocessIndex,
    const Property& property = Property(), 
    const QString& expression = QString(), 
    bool evidence = false,
    mcrl2::lts::lts_equivalence equivalence = mcrl2::lts::lts_eq_none,
    SpecType specType = SpecType::Main);

  private slots:
  /**
   * @brief startProcess Starts a process
   * @param processid The id of the process to run
   */
  void startProcess(int processid);

  /**
   * @brief executeNextSubprocess Executes the next subprocess
   * @param previousExitCode The exit code of the previous subprocess
   * @param processid The id of the process to run. This is only set when we
   *   want to execute the first subprocess
   */
  void executeNextSubprocess(int previousExitCode, int processid = -1);

  /**
   * @brief mcrl2ParsingResult Handles the result of parsing a specification
   * @param previousExitCode The exit code of the previous subprocess
   */
  void mcrl2ParsingResult(int previousExitCode);

  /**
   * @brief mcfParsingResult Handles the result of parsing a property
   * @param previousExitCode The exit code of the previous subprocess
   */
  void mcfParsingResult(int previousExitCode);

  /**
   * @brief verificationResult Extracts and stores the result of the
   *   verification
   * @param previousExitCode The exit code of the previous subprocess
   */
  void verificationResult(int previousExitCode);
  
  /**
   * @brief Extracts and stores the result of the rewrite performed
   * @param previousExitCode The exit code of the previous subprocess
   */
  void rewriteResult(int previousExitCode);
};

#endif // PROCESSSYSTEM_H
