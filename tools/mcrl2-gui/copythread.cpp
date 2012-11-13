// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <QDir>
#include <QFile>

#include "copythread.h"

void CopyThread::init(QString oldPath, QString newPath, QStringList files, bool move)
{
  m_cancel = false;

  m_oldpath = oldPath;
  m_newpath = newPath;
  m_files = files;
  m_move = move;
}

void CopyThread::cancel()
{
  m_cancel = true;
}

void CopyThread::run()
{
  if (m_oldpath == m_newpath)
    return;

  QDir oldDir(m_oldpath);
  QDir newDir(m_newpath);

  for (int i = 0; i < m_files.size() && !m_cancel; i++)
  {
    emit(busy(i+1, m_files.at(i)));

    QString oldPath = oldDir.absoluteFilePath(m_files.at(i));
    QString newPath = newDir.absoluteFilePath(m_files.at(i));

    if (QFileInfo(oldPath).isDir())
    {
      newDir.mkpath(m_files.at(i));
    }
    else
    {
      if (QFile::exists(newPath))
        QFile::remove(newPath);
      QFile::copy(oldPath, newPath);
    }
  }
  if (!m_cancel && m_move)
  {
    emit(remove(m_oldpath));
  }
}
