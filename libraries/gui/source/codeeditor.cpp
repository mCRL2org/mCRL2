// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/gui/codeeditor.h"

#include "mcrl2/utilities/platform.h"

#include <QPainter>
#include <QTextBlock>
#include <QMenu>
#include <QRegularExpression>

using namespace mcrl2::gui::qt;

HighlightingRule::HighlightingRule(QRegularExpression pattern, QTextCharFormat format)
    : pattern(pattern), format(format)
{
}

CodeHighlighter::CodeHighlighter(bool spec, bool light, QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
  /* identifiers */
  identifierFormat.setForeground(light ? Qt::black : Qt::white);
  highlightingRules.push_back(HighlightingRule(
    QRegularExpression("\\b[a-zA-Z_][a-zA-Z0-9_']*\\b"), identifierFormat));

  /* in case of mcrl2 specification */
  if (spec)
  {
    /* specification keywords */
    specificationKeywordFormat.setForeground(light ? Qt::darkBlue
                                                   : QColor(38, 139, 210));
    QStringList specificationKeywordPatterns = {
        "\\bsort\\b", "\\bcons\\b", "\\bmap\\b",  "\\bvar\\b",   "\\beqn\\b",
        "\\bact\\b",  "\\bproc\\b", "\\binit\\b", "\\bstruct\\b"};
    for (const QString& pattern : specificationKeywordPatterns)
    {
      highlightingRules.push_back(
          HighlightingRule(QRegularExpression(pattern), specificationKeywordFormat));
    }

    /* process keywords */
    processKeywordFormat.setForeground(light ? Qt::darkCyan : Qt::cyan);
    QStringList processKeywordPatterns = {"\\bdelta\\b", "\\btau\\b"};
    for (const QString& pattern : processKeywordPatterns)
    {
      highlightingRules.push_back(
          HighlightingRule(QRegularExpression(pattern), processKeywordFormat));
    }

    /* process operator keywords */
    processOperatorKeywordFormat.setForeground(light ? Qt::blue
                                                     : QColor(108, 113, 196));
    QStringList processOperatorKeywordPatterns = {
        "\\bsum\\b",  "\\bdist\\b",   "\\bblock\\b", "\\ballow\\b",
        "\\bhide\\b", "\\brename\\b", "\\bcomm\\b"};
    for (const QString& pattern : processOperatorKeywordPatterns)
    {
      highlightingRules.push_back(
          HighlightingRule(QRegularExpression(pattern), processOperatorKeywordFormat));
    }
  }
  /* in case of mcf formula */
  else
  {
    /* state formula operator keywords */
    stateFormulaOpertorKeywordFormat.setForeground(
        light ? Qt::blue : QColor(128, 128, 255));
    QStringList processOperatorKeywordPatterns = {"\\bmu\\b", "\\bnu\\b",
                                                  "\\bdelay\\b", "\\byelad\\b"};
    for (const QString& pattern : processOperatorKeywordPatterns)
    {
      highlightingRules.push_back(
          HighlightingRule(QRegularExpression(pattern), stateFormulaOpertorKeywordFormat));
    }
  }

  /* shared syntax */

  /* primitive type keywords */
  primitiveTypeKeywordFormat.setForeground(light ? Qt::darkMagenta
                                                 : Qt::magenta);
  QStringList primitiveTypeKeywordPatterns = {
      "\\bBool\\b", "\\bPos\\b", "\\bNat\\b", "\\bInt\\b", "\\bReal\\b"};
  for (const QString& pattern : primitiveTypeKeywordPatterns)
  {
    highlightingRules.push_back(
        HighlightingRule(QRegularExpression(pattern), primitiveTypeKeywordFormat));
  }

  /* container type keywords */
  containerTypeKeywordFormat.setForeground(light ? Qt::darkGreen : Qt::green);
  QStringList containerTypeKeywordPatterns = {
      "\\bList\\b", "\\bSet\\b", "\\bBag\\b", "\\bFSet\\b", "\\bFBag\\b"};
  for (const QString& pattern : containerTypeKeywordPatterns)
  {
    highlightingRules.push_back(
        HighlightingRule(QRegularExpression(pattern), containerTypeKeywordFormat));
  }

  /* data keywords */
  dataKeywordFormat.setForeground(light ? Qt::darkYellow : Qt::yellow);
  QStringList dataKeywordPatterns = {"\\btrue\\b", "\\bfalse\\b"};
  for (const QString& pattern : dataKeywordPatterns)
  {
    highlightingRules.push_back(
        HighlightingRule(QRegularExpression(pattern), dataKeywordFormat));
  }

  /* data operator keywords */
  dataOperatorKeywordFormat.setForeground(light ? Qt::darkRed : Qt::red);
  QStringList dataOperatorKeywordPatterns = {
      "\\bwhr\\b",    "\\bend\\b", "\\blambda\\b", "\\bforall\\b",
      "\\bexists\\b", "\\bdiv\\b", "\\bmod\\b",    "\\bin\\b"};
  for (const QString& pattern : dataOperatorKeywordPatterns)
  {
    highlightingRules.push_back(
        HighlightingRule(QRegularExpression(pattern), dataOperatorKeywordFormat));
  }

  /* to do keywords */
  todoKeywordFormat.setForeground(light ? Qt::red : QColor(255, 128, 128));
  QStringList todoKeywordPatterns = {"\\bcontained\\b", "\\bTODO\\b",
                                     "\\bFIXME\\b", "\\bXXX\\b"};
  for (const QString& pattern : todoKeywordPatterns)
  {
    highlightingRules.push_back(
        HighlightingRule(QRegularExpression(pattern), todoKeywordFormat));
  }

  /* defined function keywords */
  functionKeywordFormat.setForeground(light ? Qt::darkCyan : Qt::cyan);
  QStringList functionKeywordPatterns = {
      "\\bmin\\b",     "\\bmax\\b",     "\\bsucc\\b",   "\\bpred\\b",
      "\\babs\\b",     "\\bfloor\\b",   "\\bceil\\b",   "\\bround\\b",
      "\\bexp\\b",     "\\bA2B\\b",     "\\bhead\\b",   "\\btail\\b",
      "\\brhead\\b",   "\\brtail\\b",   "\\bcount\\b",  "\\bPos2Nat\\b",
      "\\bNat2Pos\\b", "\\bSet2Bag\\b", "\\bBag2Set\\b"};
  for (const QString& pattern : functionKeywordPatterns)
  {
    highlightingRules.push_back(
        HighlightingRule(QRegularExpression(pattern), functionKeywordFormat));
  }

  /* operators */
  operatorFormat.setForeground(light ? Qt::darkGreen : Qt::green);
  highlightingRules.push_back(HighlightingRule(
    QRegularExpression("[\\.\\+|&<>:;=@(){}\\[\\],\\!\\*/\\\\-]"), operatorFormat));
  highlightingRules.push_back(
      HighlightingRule(QRegularExpression("\\|\\|_"), operatorFormat));
  highlightingRules.push_back(HighlightingRule(QRegularExpression("->"), operatorFormat));

  /* numbers */
  numberFormat.setForeground(light ? Qt::darkGreen : Qt::green);
  highlightingRules.push_back(
      HighlightingRule(QRegularExpression("\\b[0-9]+\\b"), numberFormat));

  /* single line comments */
  commentFormat.setForeground(light ? Qt::darkGray : Qt::lightGray);
  highlightingRules.push_back(
      HighlightingRule(QRegularExpression("%[^\n]*"), commentFormat));
}

void CodeHighlighter::highlightBlock(const QString& text)
{
  for (const HighlightingRule& rule : highlightingRules)
  {
    for (const auto& match : rule.pattern.globalMatch(text))
    {      
      setFormat(match.capturedStart(), match.capturedEnd() - match.capturedStart(), rule.format);
    }
  }
}

LineNumberArea::LineNumberArea(CodeEditor* editor)
    : QWidget(editor), codeEditor(editor)
{
}

QSize LineNumberArea::sizeHint() const
{
  return QSize(codeEditor->lineNumberAreaWidth(), 0);
}

void LineNumberArea::paintEvent(QPaintEvent* event)
{
  codeEditor->lineNumberAreaPaintEvent(event);
}

CodeEditor::CodeEditor(QWidget* parent) : QPlainTextEdit(parent)
{
  lightPalette = hasLightBackground(this);

  /* set the font used (monospaced) */
  codeFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
  codeFont.setWeight(QFont::Light);
  lineNumberFont = this->font();

  /* set the selection colour while inactive the same as when active */
  QPalette palette = this->palette();
  palette.setColor(QPalette::Inactive, QPalette::Highlight,
                   palette.color(QPalette::Active, QPalette::Highlight));
  palette.setColor(QPalette::Inactive, QPalette::HighlightedText,
                   palette.color(QPalette::Active, QPalette::HighlightedText));
  this->setPalette(palette);

  setFontSize(13);

  lineNumberArea = new LineNumberArea(this);
  updateLineNumberAreaWidth(0);

  this->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this,
          SLOT(showContextMenu(const QPoint&)));

  /* change the line number area when needed */
  connect(this, SIGNAL(blockCountChanged(int)), this,
          SLOT(updateLineNumberAreaWidth(int)));
  connect(this, SIGNAL(updateRequest(QRect, int)), this,
          SLOT(updateLineNumberArea(QRect, int)));
}

CodeEditor::~CodeEditor()
{
  lineNumberArea->deleteLater();
  highlighter->deleteLater();
}

void CodeEditor::setPurpose(bool isSpecificationEditor)
{
  this->isSpecificationEditor = isSpecificationEditor;
  changeHighlightingRules();
}

void CodeEditor::setFontSize(int pixelSize)
{
  codeFont.setPixelSize(pixelSize);
  this->setFont(codeFont);
  lineNumberFont.setPixelSize(pixelSize);
  
  int big_number = 1000; // arbitrary big number.
  const QString test_string(" ");

  // compute the size of a char in double-precision, needed because maxWidth and
  // averageCharWidth only return integer values. However, the width of a monospaced
  // character apparently does not have to be an exact pixel width. Various attempts with
  // setStrategy and the like did not yield satisfying results.
  QFontMetrics codeFontMetrics = QFontMetrics(codeFont);
  const int many_char_width = codeFontMetrics.horizontalAdvance(test_string.repeated(big_number));
  const double single_char_width_double = many_char_width / double(big_number);

  // set the tab width to 4 characters
  this->setTabStopDistance(single_char_width_double * 4);
}

void CodeEditor::changeHighlightingRules()
{
  highlighter = new CodeHighlighter(isSpecificationEditor, lightPalette,
                                    this->document());
}

void CodeEditor::showContextMenu(const QPoint& position)
{
  QMenu* contextMenu = this->createStandardContextMenu();
  contextMenu->addSeparator();
  zoomInAction = contextMenu->addAction("Zoom in", this, SLOT(zoomIn()));
  zoomOutAction = contextMenu->addAction("Zoom out", this, SLOT(zoomOut()));
  contextMenu->exec(mapToGlobal(position));
  delete contextMenu;
}

void CodeEditor::highlightCurrentLine()
{
  QList<QTextEdit::ExtraSelection> selections = extraSelections();
  QTextEdit::ExtraSelection selection;

  QColor lineColor =
      lightPalette ? QColor(Qt::lightGray) : QColor(Qt::darkGray);

  selection.format.setBackground(lineColor);
  selection.format.setProperty(QTextFormat::FullWidthSelection, true);
  selection.cursor = textCursor();
  selection.cursor.clearSelection();
  selections.append(selection);

  setExtraSelections(selections);
}

void CodeEditor::highlightParentheses()
{
  QString text = toPlainText();
  QTextCursor cursor = textCursor();
  int toMatchPos = cursor.position();
  int matchingParenthesisPos = -1;

  /* find a matching parenthesis */
  if (!cursor.atEnd())
  {
    /* first look on the right, otherwise on the left */
    QChar rightChar = text.at(toMatchPos);
    if ((rightChar == '(' || rightChar == ')') &&
        !characterIsCommentedOut(text, toMatchPos))
    {
      matchingParenthesisPos =
          matchingParenthesisPosition(toMatchPos, rightChar == '(' ? 1 : -1);
    }
    else
    {
      if (!cursor.atStart())
      {
        toMatchPos -= 1;
        QChar leftChar = text.at(toMatchPos);
        if ((leftChar == '(' || leftChar == ')') &&
            !characterIsCommentedOut(text, toMatchPos))
        {
          matchingParenthesisPos =
              matchingParenthesisPosition(toMatchPos, leftChar == '(' ? 1 : -1);
        }
      }
    }
  }

  /* highlight both the matched and matching parenthesis */
  if (matchingParenthesisPos > -1)
  {
    QList<QTextEdit::ExtraSelection> selections = extraSelections();
    selections.append(parenthesisHighlighting(toMatchPos));
    selections.append(parenthesisHighlighting(matchingParenthesisPos));
    setExtraSelections(selections);
  }
}

int CodeEditor::matchingParenthesisPosition(int toMatchPos, int direction)
{
  QString text = toPlainText();
  QTextCursor cursor = textCursor();
  int pos = toMatchPos;
  int depth = direction;

  while (depth != 0 && pos > 0)
  {
    /* find the next first parenthesis */
    if (direction > 0)
    {
      pos = text.indexOf(QRegularExpression("[()]"), pos + 1);
    }
    else
    {
      pos = text.lastIndexOf(QRegularExpression("[()]"), pos - 1);
    }
    /* update how deeply nested we are if the parenthesis is not a comment */
    if (pos > -1 && !characterIsCommentedOut(text, pos))
    {
      depth += text.at(pos) == '(' ? 1 : -1;
    }
  }

  return depth == 0 ? pos : -1;
}

QTextEdit::ExtraSelection CodeEditor::parenthesisHighlighting(int position)
{
  QTextCursor cursor = textCursor();
  QTextEdit::ExtraSelection selection;

  /* give the character a red colour */
  QTextCharFormat format = selection.format;
  format.setForeground(Qt::red);
  format.setBackground(lightPalette ? QColor(255, 192, 192) : QColor(64, 0, 0));
  selection.format = format;

  /* select the character to highlight by moving the cursor */
  cursor.setPosition(position);
  cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
  selection.cursor = cursor;
  return selection;
}

bool CodeEditor::characterIsCommentedOut(const QString& text, int pos)
{
  return text.lastIndexOf('\n', pos) < text.lastIndexOf('%', pos);
}

void CodeEditor::paintEvent(QPaintEvent* event)
{
  // Only update the highlighting when the cursor has changed to avoid excessive updates.
  if (lastCursor != this->textCursor().position())
  {
    setExtraSelections({});
    highlightCurrentLine();
    highlightParentheses();
    lastCursor = this->textCursor().position();
  }
  QPlainTextEdit::paintEvent(event);
}

void CodeEditor::keyPressEvent(QKeyEvent* event)
{
  if (event->matches(QKeySequence::ZoomIn) ||
      (event->modifiers() == Qt::ControlModifier &&
       event->key() == Qt::Key_Equal))
  {
    /* zoom in in case of Ctrl++ or Ctrl+= */
    zoomIn();
  }
  else if (event->matches(QKeySequence::ZoomOut))
  {
    /* zoom out in case of Ctrl+- */
    zoomOut();
  }
  else if (event->matches(QKeySequence::InsertLineSeparator))
  {
    /* ignore Shift+Enter because it messes with the editor formatting */
    event->ignore();
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

void CodeEditor::changeEvent(QEvent* event)
{
  if (event->type() == QEvent::PaletteChange)
  {
    lightPalette = hasLightBackground(this);
    changeHighlightingRules();
  }
  QPlainTextEdit::changeEvent(event);
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
  return 3 + QFontMetrics(lineNumberFont).horizontalAdvance('9') * digits;
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
  painter.fillRect(event->rect(),
                   lightPalette ? Qt::lightGray : QColor(64, 64, 64));

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
      painter.setPen(lightPalette ? Qt::black : Qt::white);
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
