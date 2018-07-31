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
#include <QMenu>

HighlightingRule::HighlightingRule(QRegExp pattern, QTextCharFormat format)
{
  this->pattern = pattern;
  this->format = format;
}

CodeHighlighter::CodeHighlighter(bool spec, QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
  /* identifiers */
  identifierFormat.setForeground(Qt::black);
  highlightingRules.push_back(HighlightingRule(
      QRegExp("\\b[a-zA-Z_][a-zA-Z0-9_']*\\b"), identifierFormat));

  /* in case of mcrl2 specification */
  if (spec)
  {
    /* specification keywords */
    specificationKeywordFormat.setForeground(Qt::darkBlue);
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
    QStringList processKeywordPatterns = {"\\bdelta\\b", "\\btau\\b"};
    for (const QString& pattern : processKeywordPatterns)
    {
      highlightingRules.push_back(
          HighlightingRule(QRegExp(pattern), processKeywordFormat));
    }

    /* process operator keywords */
    processOperatorKeywordFormat.setForeground(Qt::blue);
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
  QStringList primitiveTypeKeywordPatterns = {
      "\\bBool\\b", "\\bPos\\b", "\\bNat\\b", "\\bInt\\b", "\\bReal\\b"};
  for (const QString& pattern : primitiveTypeKeywordPatterns)
  {
    highlightingRules.push_back(
        HighlightingRule(QRegExp(pattern), primitiveTypeKeywordFormat));
  }

  /* container type keywords */
  containerTypeKeywordFormat.setForeground(Qt::darkGreen);
  QStringList containerTypeKeywordPatterns = {
      "\\bList\\b", "\\bSet\\b", "\\bBag\\b", "\\bFSet\\b", "\\bFBag\\b"};
  for (const QString& pattern : containerTypeKeywordPatterns)
  {
    highlightingRules.push_back(
        HighlightingRule(QRegExp(pattern), containerTypeKeywordFormat));
  }

  /* data keywords */
  dataKeywordFormat.setForeground(Qt::darkYellow);
  QStringList dataKeywordPatterns = {"\\btrue\\b", "\\bfalse\\b"};
  for (const QString& pattern : dataKeywordPatterns)
  {
    highlightingRules.push_back(
        HighlightingRule(QRegExp(pattern), dataKeywordFormat));
  }

  /* data operator keywords */
  dataOperatorKeywordFormat.setForeground(Qt::darkRed);
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
  QStringList todoKeywordPatterns = {"\\bcontained\\b", "\\bTODO\\b",
                                     "\\bFIXME\\b", "\\bXXX\\b"};
  for (const QString& pattern : todoKeywordPatterns)
  {
    highlightingRules.push_back(
        HighlightingRule(QRegExp(pattern), todoKeywordFormat));
  }

  /* defined function keywords */
  functionKeywordFormat.setForeground(Qt::darkCyan);
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
  codeFont.setFamily("Monospace");
  codeFont.setFixedPitch(true);
  codeFont.setWeight(QFont::Light);
  lineNumberFont = this->font();

  setFontSize(13);

  lineNumberArea = new LineNumbersArea(this);
  highlighter = new CodeHighlighter(spec, this->document());

  /* set up the context menu*/
  this->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this,
          SLOT(showContextMenu(const QPoint&)));
  contextMenu = this->createStandardContextMenu();
  contextMenu->addSeparator();
  zoomInAction = contextMenu->addAction("Zoom in", this, SLOT(zoomIn()));
  zoomInAction->setShortcut(QKeySequence::ZoomIn);
  zoomOutAction = contextMenu->addAction("Zoom out", this, SLOT(zoomOut()));
  zoomOutAction->setShortcut(QKeySequence::ZoomOut);

  /* highlight the line that the cursor is on */
  highlightCurrentLine();
  connect(this, SIGNAL(cursorPositionChanged()), this,
          SLOT(highlightCurrentLine()));

  /* change the line number area when needed */
  connect(this, SIGNAL(blockCountChanged(int)), this,
          SLOT(updateLineNumberAreaWidth(int)));
  connect(this, SIGNAL(updateRequest(QRect, int)), this,
          SLOT(updateLineNumberArea(QRect, int)));

  updateLineNumberAreaWidth(0);
}

CodeEditor::~CodeEditor()
{
  lineNumberArea->deleteLater();
  highlighter->deleteLater();
}

void CodeEditor::setFontSize(int pixelSize)
{
  codeFont.setPixelSize(pixelSize);
  this->document()->setDefaultFont(codeFont);
  lineNumberFont.setPixelSize(pixelSize);

  /* set the tab width to 4 characters */
  QFontMetrics codeFontMetrics = QFontMetrics(codeFont);
  this->setTabStopWidth(codeFontMetrics.width("1234"));
}

void CodeEditor::showContextMenu(const QPoint& position)
{
  contextMenu->exec(mapToGlobal(position));
}

void CodeEditor::highlightCurrentLine()
{
  QList<QTextEdit::ExtraSelection> extraSelections;
  QTextEdit::ExtraSelection selection;

  QColor lineColor = QColor(Qt::gray).lighter(140);

  selection.format.setBackground(lineColor);
  selection.format.setProperty(QTextFormat::FullWidthSelection, true);
  selection.cursor = textCursor();
  selection.cursor.clearSelection();
  extraSelections.append(selection);

  setExtraSelections(extraSelections);
}

void CodeEditor::keyPressEvent(QKeyEvent* event)
{
  /* zoom in in case of Ctrl++ or Ctrl+=, zoom out in case of Ctrl +- */
  if (event->matches(QKeySequence::ZoomIn) ||
      (event->modifiers() == Qt::ControlModifier &&
       event->key() == Qt::Key_Equal))
  {
    zoomIn();
  }
  else if (event->matches(QKeySequence::ZoomOut))
  {
    zoomOut();
  }
  else
  {
    QPlainTextEdit::keyPressEvent(event);
  }
}

void CodeEditor::wheelEvent(QWheelEvent* event)
{
  /* zoom in in case of Ctrl+scrollup, zoom out in case of Ctrl+scrolldown */
  if (event->modifiers() == Qt::ControlModifier)
  {
    int numZooms = event->angleDelta().ry() / 120;
    if (numZooms > 0)
    {
      zoomIn(numZooms);
    }
    else if (numZooms < 0)
    {
      zoomOut(-numZooms);
    }
  }
  else
  {
    QPlainTextEdit::wheelEvent(event);
  }
}

void CodeEditor::deleteChar()
{
  this->textCursor().deleteChar();
}

void CodeEditor::zoomIn(int range)
{
  setFontSize(codeFont.pixelSize() + range);
}

void CodeEditor::zoomOut(int range)
{
  setFontSize(codeFont.pixelSize() - range);
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

  return 3 + QFontMetrics(lineNumberFont).width("9") * digits;
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
  int lineNumberHeight = QFontMetrics(lineNumberFont).height();

  while (block.isValid() && top <= event->rect().bottom())
  {
    if (block.isVisible() && bottom >= event->rect().top())
    {
      QString number = QString::number(blockNumber + 1);
      painter.setPen(Qt::black);
      painter.setFont(lineNumberFont);
      painter.drawText(-2, top, lineNumberArea->width(), lineNumberHeight,
                       Qt::AlignRight | Qt::AlignBottom, number);
    }

    block = block.next();
    top = bottom;
    bottom = top + (int)blockBoundingRect(block).height();
    ++blockNumber;
  }
}
