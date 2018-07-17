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

HighlightingRule::HighlightingRule(QRegExp pattern, QTextCharFormat format)
{
  this->pattern = pattern;
  this->format = format;
}

CodeHighlighter::CodeHighlighter(bool spec, QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
  /* identifiers */
  identifierFormat.setFontWeight(QFont::DemiBold);
  identifierFormat.setForeground(Qt::black);
  highlightingRules.push_back(HighlightingRule(
      QRegExp("\\b[a-zA-Z_][a-zA-Z0-9_']*\\b"), identifierFormat));

  /* in case of mcrl2 specification */
  if (spec)
  {
    /* specification keywords */
    specificationKeywordFormat.setForeground(Qt::darkBlue);
    specificationKeywordFormat.setFontWeight(QFont::Bold);
    QStringList specificationKeywordPatterns = {
        "\\bsort\\b", "\\bcons\\b", "\\bmap\\b",  "\\bvar\\b",   "\\beqn\\b",
        "\\bact\\b",  "\\bproc\\b", "\\binit\\b", "\\bstruct\\b"};
    for (const QString& pattern : specificationKeywordPatterns)
    {
      highlightingRules.push_back(
          HighlightingRule(QRegExp(pattern), specificationKeywordFormat));
    }

    /* process keywords */
    processKeywordFormat.setForeground(Qt::darkCyan);
    processKeywordFormat.setFontWeight(QFont::Light);
    QStringList processKeywordPatterns = {"\\bdelta\\b", "\\btau\\b"};
    for (const QString& pattern : processKeywordPatterns)
    {
      highlightingRules.push_back(
          HighlightingRule(QRegExp(pattern), processKeywordFormat));
    }

    /* process operator keywords */
    processOperatorKeywordFormat.setForeground(Qt::blue);
    processOperatorKeywordFormat.setFontWeight(QFont::Bold);
    QStringList processOperatorKeywordPatterns = {
        "\\bsum\\b",  "\\bdist\\b",   "\\bblock\\b", "\\ballow\\b",
        "\\bhide\\b", "\\brename\\b", "\\bcomm\\b"};
    for (const QString& pattern : processOperatorKeywordPatterns)
    {
      highlightingRules.push_back(
          HighlightingRule(QRegExp(pattern), processOperatorKeywordFormat));
    }
  }
  /* in case of mcf formula */
  else
  {
    /* state formula operator keywords */
    stateFormulaOpertorKeywordFormat.setForeground(Qt::blue);
    stateFormulaOpertorKeywordFormat.setFontWeight(QFont::Bold);
    QStringList processOperatorKeywordPatterns = {"\\bmu\\b", "\\bnu\\b",
                                                  "\\bdelay\\b", "\\byelad\\b"};
    for (const QString& pattern : processOperatorKeywordPatterns)
    {
      highlightingRules.push_back(
          HighlightingRule(QRegExp(pattern), stateFormulaOpertorKeywordFormat));
    }
  }

  /* shared syntax */

  /* primitive type keywords */
  primitiveTypeKeywordFormat.setForeground(Qt::darkMagenta);
  primitiveTypeKeywordFormat.setFontWeight(QFont::Bold);
  QStringList primitiveTypeKeywordPatterns = {
      "\\bBool\\b", "\\bPos\\b", "\\bNat\\b", "\\bInt\\b", "\\bReal\\b"};
  for (const QString& pattern : primitiveTypeKeywordPatterns)
  {
    highlightingRules.push_back(
        HighlightingRule(QRegExp(pattern), primitiveTypeKeywordFormat));
  }

  /* container type keywords */
  containerTypeKeywordFormat.setForeground(Qt::darkGreen);
  containerTypeKeywordFormat.setFontWeight(QFont::Bold);
  QStringList containerTypeKeywordPatterns = {
      "\\bList\\b", "\\bSet\\b", "\\bBag\\b", "\\bFSet\\b", "\\bFBag\\b"};
  for (const QString& pattern : containerTypeKeywordPatterns)
  {
    highlightingRules.push_back(
        HighlightingRule(QRegExp(pattern), containerTypeKeywordFormat));
  }

  /* data keywords */
  dataKeywordFormat.setForeground(Qt::darkYellow);
  dataKeywordFormat.setFontWeight(QFont::Bold);
  QStringList dataKeywordPatterns = {"\\btrue\\b", "\\bfalse\\b"};
  for (const QString& pattern : dataKeywordPatterns)
  {
    highlightingRules.push_back(
        HighlightingRule(QRegExp(pattern), dataKeywordFormat));
  }

  /* data operator keywords */
  dataOperatorKeywordFormat.setForeground(Qt::darkRed);
  dataOperatorKeywordFormat.setFontWeight(QFont::Light);
  QStringList dataOperatorKeywordPatterns = {
      "\\bwhr\\b",    "\\bend\\b", "\\blambda\\b", "\\bforall\\b",
      "\\bexists\\b", "\\bdiv\\b", "\\bmod\\b",    "\\bin\\b"};
  for (const QString& pattern : dataOperatorKeywordPatterns)
  {
    highlightingRules.push_back(
        HighlightingRule(QRegExp(pattern), dataOperatorKeywordFormat));
  }

  /* todo keywords */
  todoKeywordFormat.setForeground(Qt::red);
  todoKeywordFormat.setFontWeight(QFont::Bold);
  QStringList todoKeywordPatterns = {"\\bcontained\\b", "\\bTODO\\b",
                                     "\\bFIXME\\b", "\\bXXX\\b"};
  for (const QString& pattern : todoKeywordPatterns)
  {
    highlightingRules.push_back(
        HighlightingRule(QRegExp(pattern), todoKeywordFormat));
  }

  /* defined function keywords */
  functionKeywordFormat.setForeground(Qt::darkCyan);
  functionKeywordFormat.setFontWeight(QFont::Bold);
  QStringList functionKeywordPatterns = {
      "\\bmin\\b",     "\\bmax\\b",     "\\bsucc\\b",   "\\bpred\\b",
      "\\babs\\b",     "\\bfloor\\b",   "\\bceil\\b",   "\\bround\\b",
      "\\bexp\\b",     "\\bA2B\\b",     "\\bhead\\b",   "\\btail\\b",
      "\\brhead\\b",   "\\brtail\\b",   "\\bcount\\b",  "\\bPos2Nat\\b",
      "\\bNat2Pos\\b", "\\bSet2Bag\\b", "\\bBag2Set\\b"};
  for (const QString& pattern : functionKeywordPatterns)
  {
    highlightingRules.push_back(
        HighlightingRule(QRegExp(pattern), functionKeywordFormat));
  }

  /* operators */
  operatorFormat.setFontWeight(QFont::DemiBold);
  operatorFormat.setForeground(Qt::darkGreen);
  highlightingRules.push_back(HighlightingRule(
      QRegExp("[\\.\\+|&<>:;=@(){}\\[\\],\\!\\*/\\\\-]"), operatorFormat));
  highlightingRules.push_back(
      HighlightingRule(QRegExp("\\|\\|_"), operatorFormat));
  highlightingRules.push_back(HighlightingRule(QRegExp("->"), operatorFormat));

  /* numbers */
  numberFormat.setForeground(Qt::darkGreen);
  highlightingRules.push_back(
      HighlightingRule(QRegExp("\\b[0-9]+\\b"), numberFormat));

  /* single line comments */
  commentFormat.setForeground(Qt::gray);
  highlightingRules.push_back(
      HighlightingRule(QRegExp("%[^\n]*"), commentFormat));
}

void CodeHighlighter::highlightBlock(const QString& text)
{
  for (const HighlightingRule& rule : highlightingRules)
  {
    QRegExp expression(rule.pattern);
    int index = expression.indexIn(text);
    while (index >= 0)
    {
      int length = expression.matchedLength();
      setFormat(index, length, rule.format);
      index = expression.indexIn(text, index + length);
    }
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

CodeEditor::CodeEditor(QWidget* parent, bool spec) : QPlainTextEdit(parent)
{
  /* set the font used (monospaced) */
  QFont font = QFont("Courier New");
  font.setPixelSize(13);
  this->document()->setDefaultFont(font);

  /* set the tab width to 4 characters */
  QFontMetrics fm = QFontMetrics(font);
  this->setTabStopWidth(fm.width("1234"));

  lineNumberArea = new LineNumbersArea(this);
  highlighter = new CodeHighlighter(spec, this->document());

  /* signals that need to change the line number area */
  connect(this, SIGNAL(blockCountChanged(int)), this,
          SLOT(updateLineNumberAreaWidth(int)));
  connect(this, SIGNAL(updateRequest(QRect, int)), this,
          SLOT(updateLineNumberArea(QRect, int)));

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
