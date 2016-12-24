// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "documentwidget.h"
#include <QFile>

DocumentWidget::DocumentWidget(QWidget *parent, QThread *atermThread, mcrl2::data::rewrite_strategy strategy) :
  NumberedTextEdit(parent),
  m_rewriter(atermThread, strategy),
  m_solver(atermThread, strategy)
{
  document()->setModified(false);
}

QString DocumentWidget::getFileName()
{
  return m_filename;
}

bool DocumentWidget::isModified()
{
  return document()->isModified();
}

void DocumentWidget::openFile(QString fileName)
{
  QFile file(fileName);

  if (file.open(QFile::ReadOnly | QFile::Text))
  {
    setPlainText(file.readAll());
    file.close();
    m_filename = fileName;
    document()->setModified(false);
  }
}

void DocumentWidget::saveFile(QString fileName)
{
  QFile file(fileName);

  if (file.open(QFile::WriteOnly | QFile::Text))
  {
    file.write((const char *)toPlainText().toLatin1().data());
    file.close();
    m_filename = fileName;
    document()->setModified(false);
  }
}
