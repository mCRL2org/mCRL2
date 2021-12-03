// Author(s): Ferry Timmers
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <QRegularExpression>

#include "multiprocess.h"

using Process = QMultiProcess::Process;
using Processes = QMultiProcess::Processes;
using size_type = Processes::size_type;
using ReadMethod = QByteArray(Process::*)();

QMultiProcess::Process* QMultiProcess::start(QIODevice::OpenMode mode)
{
  QPersistentProcess *process = new QPersistentProcess();
  connect(process, SIGNAL(readyReadStandardError()), this, SIGNAL(readyReadStandardError()));
  connect(process, SIGNAL(readyReadStandardOutput()), this, SIGNAL(readyReadStandardOutput()));
  process->setWorkingDirectory(workingDirectory());
  process->setProgram(program());
  process->setArguments(arguments());
  process->start(mode);
  m_processes.emplace_back(process);
  return process;
}

static inline QString readAll(ReadMethod method, Processes &processes,
  size_type &last)
{
  QString output;
  size_type current = 0;
  for (size_type i = 0; i < processes.size(); ++i)
  {
    size_type index = (i + last) % processes.size();
    QString str = ((*processes[index]).*method)();
    if (str.isEmpty())
      continue;

    const QString label = QString("%1#  ").arg(index + 1);
    output += label + str;
    current = index;
  }
  last = current;
  return output;
}

QString QMultiProcess::readAllStandardError()
{
  return ::readAll(&Process::readAllStandardError, m_processes, m_last);
}

QString QMultiProcess::readAllStandardOutput()
{
  return ::readAll(&Process::readAllStandardOutput, m_processes, m_last);
}
