// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "documentwidget.h"
#include "ui_documentwidget.h"
#include <QFile>

DocumentWidget::DocumentWidget(QWidget *parent) :
  QWidget(parent)
{
  m_ui.setupUi(this);
  m_ui.textEdit->document()->setModified(false);
}

QTextEdit* DocumentWidget::getEditor()
{
  return m_ui.textEdit;
}

QString DocumentWidget::getFileName()
{
  return m_filename;
}

bool DocumentWidget::isModified()
{
  return m_ui.textEdit->document()->isModified();
}

void DocumentWidget::openFile(QString fileName)
{
  QFile file(fileName);

  if (file.open(QFile::ReadOnly | QFile::Text))
  {
    m_ui.textEdit->setPlainText(file.readAll());
    file.close();
    m_filename = fileName;
    m_ui.textEdit->document()->setModified(false);
  }
}

void DocumentWidget::saveFile(QString fileName)
{
  QFile file(fileName);

  if (file.open(QFile::WriteOnly | QFile::Text))
  {
    file.write((const char *)m_ui.textEdit->toPlainText().toAscii().data());
    file.close();
    m_filename = fileName;
    m_ui.textEdit->document()->setModified(false);
  }
}
