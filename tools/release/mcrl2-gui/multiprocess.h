// Author(s): Ferry Timmers
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MULTIPROCESS_H
#define MULTIPROCESS_H

#include <memory>
#include <vector>
#include <QProcess>

/// Process that does not terminate when destructed
class QPersistentProcess : public QProcess
{
  public:
    using QProcess::QProcess;

    ~QPersistentProcess() { setProcessState(QProcess::NotRunning); }
};

/// Prototype process that spawns new process instances when started
class QMultiProcess : public QProcess
{
  public:
    using Process = QPersistentProcess;
    using ProcessPtr = std::unique_ptr<Process>;
    using Processes = std::vector<ProcessPtr>;

    QMultiProcess() {}
    ~QMultiProcess() {}

    Process* start(QIODevice::OpenMode mode = ReadWrite);

    QString readAllStandardError();
    QString readAllStandardOutput();

  private:

    Processes m_processes;
    Processes::size_type m_last = 0;

};

#endif /* MULTIPROCESS_H */
