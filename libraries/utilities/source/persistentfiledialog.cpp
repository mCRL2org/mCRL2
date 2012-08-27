// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/utilities/persistentfiledialog.h"

using namespace mcrl2::utilities::qt;

PersistentFileDialog::PersistentFileDialog(QString directory, QWidget *parent)
  : QObject(parent),
    m_parent(parent),
    m_directory(directory)
{
  if (m_directory.isEmpty())
    m_directory = QDir::currentPath();
}

QString PersistentFileDialog::getExistingDirectory(const QString &caption, QFileDialog::Options options)
{
  QString result = QFileDialog::getExistingDirectory(m_parent, caption, m_directory, options);
  if (!result.isNull())
  {
    QFileInfo info(result);
    m_directory = info.absolutePath();
  }
  return result;
}

QString PersistentFileDialog::getOpenFileName(const QString &caption, const QString &filter, QString *selectedFilter, QFileDialog::Options options)
{
  QString result = QFileDialog::getOpenFileName(m_parent, caption, m_directory, filter, selectedFilter, options);
  if (!result.isNull())
  {
    QFileInfo info(result);
    m_directory = info.absolutePath();
  }
  return result;
}

QStringList PersistentFileDialog::getOpenFileNames(const QString &caption, const QString &filter, QString *selectedFilter, QFileDialog::Options options)
{
  QStringList result = QFileDialog::getOpenFileNames(m_parent, caption, m_directory, filter, selectedFilter, options);
  if (!result.isEmpty())
  {
    QFileInfo info(result[0]);
    m_directory = info.absolutePath();
  }
  return result;
}

QString PersistentFileDialog::getSaveFileName(const QString &caption, const QString &filter, QString *selectedFilter, QFileDialog::Options options)
{
  QString result = QFileDialog::getSaveFileName(m_parent, caption, m_directory, filter, selectedFilter, options);
  if (!result.isNull())
  {
    QFileInfo info(result);
    m_directory = info.absolutePath();
  }
  return result;
}

