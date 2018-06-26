// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "codeeditor.h"

#include <QPainter>
#include <QTextBlock>
#include <QWidget>

CodeEditor::CodeEditor(QWidget* parent) : QPlainTextEdit(parent)
{
  /* set the font used (monospaced) */
  QFont font = QFont("Courier New", 10);
  this->document()->setDefaultFont(font);

  /* set the tab width to 4 characters */
  QFontMetrics fm = QFontMetrics(font);
  this->setTabStopWidth(fm.width("1234"));

  lineNumberArea = new LineNumbersArea(this);

  /* signals that need to change the line number area */
  connect(this, SIGNAL(blockCountChanged(int)), 
          this, SLOT(updateLineNumberAreaWidth(int)));
  connect(this, SIGNAL(updateRequest(QRect, int)),
          this, SLOT(updateLineNumberArea(QRect, int)));

  updateLineNumberAreaWidth(0);
}

void CodeEditor::deleteChar()
{
  this->textCursor().deleteChar();
}

int CodeEditor::lineNumberAreaWidth()
{
  int digits = 1;
  int max = std::max(1, blockCount());
  while (max >= 10)
  {
    max /= 10;
    ++digits;
  }

  return 3 + fontMetrics().width(QLatin1Char('9')) * digits;
}

void CodeEditor::updateLineNumberAreaWidth(int)
{
  setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect& rect, int dy)
{
  if (dy)
  {
    lineNumberArea->scroll(0, dy);
  }
  else
  {
    lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
  }

  if (rect.contains(viewport()->rect()))
  {
    updateLineNumberAreaWidth(0);
  }
}

void CodeEditor::resizeEvent(QResizeEvent* e)
{
  QPlainTextEdit::resizeEvent(e);

  QRect cr = contentsRect();
  lineNumberArea->setGeometry(
      QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent* event)
{
  QPainter painter(lineNumberArea);
  painter.fillRect(event->rect(), Qt::lightGray);

  QTextBlock block = firstVisibleBlock();
  int blockNumber = block.blockNumber();
  int top = (int)blockBoundingGeometry(block).translated(contentOffset()).top();
  int bottom = top + (int)blockBoundingRect(block).height();

  while (block.isValid() && top <= event->rect().bottom())
  {
    if (block.isVisible() && bottom >= event->rect().top())
    {
      QString number = QString::number(blockNumber + 1);
      painter.setPen(Qt::black);
      painter.drawText(-2, top, lineNumberArea->width(), fontMetrics().height(),
                       Qt::AlignRight, number);
    }

    block = block.next();
    top = bottom;
    bottom = top + (int)blockBoundingRect(block).height();
    ++blockNumber;
  }
}

LineNumbersArea::LineNumbersArea(CodeEditor* editor) : QWidget(editor)
{
  codeEditor = editor;
}

QSize LineNumbersArea::sizeHint() const
{
  return QSize(codeEditor->lineNumberAreaWidth(), 0);
}

void LineNumbersArea::paintEvent(QPaintEvent* event)
{
  codeEditor->lineNumberAreaPaintEvent(event);
}
