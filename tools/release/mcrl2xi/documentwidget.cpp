// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "documentwidget.h"
#include "highlighter.h"

#include <QFile>
#include <QTextBlock>

void DocumentWidget::highlightParenthesis(int pos)
{
  QList<QTextEdit::ExtraSelection> selections = extraSelections();
  QTextEdit::ExtraSelection selection;
  selection.format.setBackground(Qt::green);
  QTextCursor cursor = textCursor();
  cursor.setPosition(pos);
  cursor.movePosition(QTextCursor::NextCharacter,
          QTextCursor::KeepAnchor);
  selection.cursor = cursor;
  selections.append(selection);
  setExtraSelections(selections);
}

void DocumentWidget::matchParenthesis(int which) {
  const QTextBlock& block = textCursor().block();
  TextBlockData *data = static_cast<TextBlockData *>(block.userData());
  if (data == nullptr)
    return;
  const ParenthesisInfo& my = data->parentheses().at(which);
  char other;
  int increment;
  QTextDocument::FindFlags flags;
  int pos = block.position() + my.position;
  if (my.character == '(')
  {
    other = ')';
    increment = 1;
    flags = 0;
  }
  else
  {
    other = '(';
    increment = -1;
    flags = QTextDocument::FindBackward;
    pos++;
  }
  QRegExp rx("[()]");
  int patternLength = 1;
  int depth = 1;
  while((pos = document()->find(rx, pos + increment, flags).position()) != -1) {
    char c = document()->characterAt(pos - patternLength).toLatin1();
    depth += c == my.character;
    depth -= c == other;
    if (depth == 0)
    {
      highlightParenthesis(pos - patternLength);
      highlightParenthesis(block.position() + my.position);
      return;
    }
  }
}

void DocumentWidget::onCursorPositionChanged() {
  QList<QTextEdit::ExtraSelection> selections;
  setExtraSelections(selections);
  TextBlockData *data = static_cast<TextBlockData *>(textCursor().block().userData());
  if (data)
  {
    const QVector<ParenthesisInfo> infos = data->parentheses();
    for (int i = 0; i < infos.size(); ++i)
    {
      const ParenthesisInfo& info = infos.at(i);
      int curPos = textCursor().position() - textCursor().block().position();
      if (info.position == curPos - 1)
      {
        matchParenthesis(i);
        return;
      }
    }
  }
}

DocumentWidget::DocumentWidget(QWidget *parent, QThread *atermThread, mcrl2::data::rewrite_strategy strategy) :
  NumberedTextEdit(parent),
  m_rewriter(atermThread, strategy),
  m_solver(atermThread, strategy)
{
  document()->setModified(false);
  connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChanged()));
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

