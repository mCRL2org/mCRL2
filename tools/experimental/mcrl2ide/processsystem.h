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

#include <QObject>
#include <QProcess>

class FileSystem;

class ProcessSystem : public QObject
{
  Q_OBJECT

  public:
  ProcessSystem(FileSystem* fileSystem);

  /**
   * @brief setConsoleDock Assigns the console dock to the file system for
   *   logging
   * @param consoleDock The console dock
   */
  void setConsoleDock(ConsoleDock* consoleDock);

  /**
   * @brief verifyProperty Verifies a property using mcrl22lps, lps2pbes and
   *   pbes2bool
   * @param property The property to verify
   * @return The process id of the verification process
   */
  int verifyProperty(Property* property);

  /**
   * @brief getResult Gets the result of a process
   *   for a verification process, the result is either "" (in case of error),
   *   "false" or "true"
   * @param processid The id of the proces to get the result from
   * @return The result of the process
   */
  QString getResult(int processid);

  signals:
  void processFinished(int processid);

  private:
  FileSystem* fileSystem;
  ConsoleDock* consoleDock;
  int pid;
  std::map<int, std::vector<QProcess*>> processes;
  std::map<int, QString> results;

  /**
   * @brief mcrl22lps Executes mcrl22lps on the current specification
   * @param verification Determines what console dock tab to use
   */
  QProcess* createMcrl22lpsProcess(bool verification);

  /**
   * @brief lpsxsim Executes lpsxsim on the lps that corresponds to the current
   *   specification
   */
  QProcess* createLpsxsimProcess();

  /**
   * @brief lps2lts Executes lps2lts on the lps that corresponds to the current
   *   specification
   */
  QProcess* createLps2ltsProcess();

  /**
   * @brief ltsconvert Executes ltsconvert on the lts that corresponds to the
   *   current specification
   */
  QProcess* createLtsconvertProcess();

  /**
   * @brief lps2pbes Executes lps2pbes on the lps that corresponds to the
   *   current specification and the given property
   * @param propertyName The name of the property to include
   */
  QProcess* createLps2pbesProcess(QString propertyName);

  /**
   * @brief pbes2bool Executes pbes2bool on the pbes that corresponds to the
   *   current specification and the given property
   * @param propertyName The name of the property to include
   */
  QProcess* createPbes2boolProcess(QString propertyName);

  private slots:
  /**
   * @brief verifyProperty2 The second step of verification, creating the pbes
   */
  void verifyProperty2();

  /**
   * @brief verifyPoperty3 The third step of verification, solving the pbes
   */
  void verifyProperty3();

  /**
   * @brief actionVerifyResult Applies the result of the verification
   */
  void verifyPropertyResult();
};

#endif // PROCESSSYSTEM_H
