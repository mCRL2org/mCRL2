// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QRegularExpression>

#include "mcrl2/gui/utilities.h"

namespace mcrl2::gui::qt
{

/**
 * @brief The HighlightingRule class defines a rule on how to highlight a given
 *   pattern of syntactic elements
 */
class HighlightingRule
{
public:
  QRegularExpression pattern;
  QTextCharFormat format;

  /**
   * @brief HighlightingRule Constructor
   * @param pattern The pattern of syntactic elements to highlight
   * @param format The highlighting format for this pattern
   */
  HighlightingRule(QRegularExpression pattern, QTextCharFormat format);
};

/**
 * @brief The CodeHighlighter class defines a syntax highlighter for mCRL2
 *   specifications or mu-calculus formulae
 */
class CodeHighlighter : public QSyntaxHighlighter
{
  Q_OBJECT

  public:
  /**
   * @brief CodeHighlighter Constructor
   * @param spec Whether this code editor is for a mcrl2 specification or a
   *   mu-calculus formula
   * @param light Whether the application has a light colour palette
   * @param parent The document on which the code highlighter should operate
   */
  CodeHighlighter(bool spec, bool light, QTextDocument* parent = 0);

  protected:
  /**
   * @brief highlightBlock Highlights a single block of text
   * @param text The text to highlight
   */
  void highlightBlock(const QString& text);

  private:
  std::vector<HighlightingRule> highlightingRules;

  QTextCharFormat identifierFormat;
  QTextCharFormat specificationKeywordFormat;
  QTextCharFormat processKeywordFormat;
  QTextCharFormat processOperatorKeywordFormat;
  QTextCharFormat stateFormulaOpertorKeywordFormat;
  QTextCharFormat primitiveTypeKeywordFormat;
  QTextCharFormat containerTypeKeywordFormat;
  QTextCharFormat dataKeywordFormat;
  QTextCharFormat dataOperatorKeywordFormat;
  QTextCharFormat todoKeywordFormat;
  QTextCharFormat functionKeywordFormat;
  QTextCharFormat operatorFormat;
  QTextCharFormat numberFormat;
  QTextCharFormat commentFormat;
};

class CodeEditor;

/**
 * @brief The LineNumberArea class defines the area with line numbers in the
 * code editor
 */
class LineNumberArea : public QWidget
{
  public:
  /**
   * @brief LineNumberArea Constructor
   * @param editor The code editor this line number area belongs to
   */
  LineNumberArea(CodeEditor* editor);

  /**
   * @brief sizeHint Returns the recommended size of the widget
   * @return The recommended size of the widget
   */
  QSize sizeHint() const override;

  protected:
  /**
   * @brief paintEvent Handles paint events
   * @param event A paint event
   */
  void paintEvent(QPaintEvent* event) override;

  private:
  CodeEditor* codeEditor;
};

/**
 * @brief The CodeEditor class defines a text editor for code (used for
 *   specification and properties)
 */
class CodeEditor : public QPlainTextEdit
{
  Q_OBJECT

  public:
  /**
   * @brief CodeEditor Constructor
   * @param parent The parent of this widget
   */
  explicit CodeEditor(QWidget* parent = 0);
  ~CodeEditor();

  /**
   * @brief setPurpose Set whether this code editor is for editing
   *   specifications or mu-calculus formulae
   * @param isSpecificationEditor Whether this code editor is for editing
   *   specifications
   */
  void setPurpose(bool isSpecificationEditor);

  /**
   * @brief changeHighlightingRules Change the highlighting rules depending on
   *   the purpose of the code editor and its colour palette
   */
  void changeHighlightingRules();

  /**
   * @brief lineNumberAreaPaintEvent Paints the line number area on the screen
   * @param event A paint event
   */
  void lineNumberAreaPaintEvent(QPaintEvent* event);

  /**
   * @brief lineNumberAreaWidth Computes the width needed for the line number
   *   area
   * @return The width needed for the line number area
   */
  int lineNumberAreaWidth();

  public slots:
  /**
   * @brief deleteChar Allows the user to delete text
   */
  void deleteChar();

  /**
   * @brief zoomIn Allows the user to zoom in on the text
   * @param range How much to zoom in
   */
  void zoomIn(int range = 1);

  /**
   * @brief zoomOut Allows the user to zoom out from the text
   * @param range How much to zoom out
   */
  void zoomOut(int range = 1);

  protected:
  /**
   * @brief paintEvent Adds placeholder text
   * @param event The paint event
   */
  void paintEvent(QPaintEvent* event) override;

  /**
   * @brief keyPressEvent Add or remove key events
   * @param event The key event
   */
  void keyPressEvent(QKeyEvent* event) override;

  /**
   * @brief wheelEvent Adds mouse wheel events for zooming
   * @param event The mouse wheel event
   */
  void wheelEvent(QWheelEvent* event) override;

  /**
   * @brief resizeEvent Resizes the line number area when the window is resized
   * @param event The resize event
   */
  void resizeEvent(QResizeEvent* event) override;

  /**
   * @brief changeEvent Changes the syntax highlighting when the colour palette
   *   of the window changes
   * @brief event The change event
   */
  void changeEvent(QEvent* event) override;

  private:
  bool isSpecificationEditor = false;
  bool lightPalette = false;
  QFont codeFont;
  QFont lineNumberFont;
  LineNumberArea* lineNumberArea = nullptr;
  CodeHighlighter* highlighter = nullptr;
  int lastCursor;

  QAction* zoomInAction = nullptr;
  QAction* zoomOutAction = nullptr;

  /**
   * @brief setFontSize Sets the font size and tab width
   * @param pixelSize The desired font size in pixels
   */
  void setFontSize(int pixelSize);

  /**
   * @brief highlightCurrentLine Highlights the line the cursor is on
   */
  void highlightCurrentLine();

  /**
   * @brief highlightParentheses Highlights the parenthesis that corresponds to
   *   the one the cursor is next to
   */
  void highlightParentheses();

  /**
   * @brief matchingParenthesisPosition Finds the position of the parenthesis
   *   that matches the one on the given position in the given direction
   * @param toMatchPos The position in the text of the parenthesis to match with
   * @param direction In which direction to look for the matching parenthesis.
   *   Equals 1 if we need to look forward for a ')', equals -1 if we need to
   *   look back for a '('.
   */
  int matchingParenthesisPosition(int toMatchPos, int direction);

  /**
   * @brief parenthesisHighlighting Creates a highlighting for a single
   *   character on a given position for highlighting matching parentheses
   * @param position The position of the character to highlight
   * @returns A selection that defines a highlighted parenthesis
   */
  QTextEdit::ExtraSelection parenthesisHighlighting(int position);

  /**
   * @brief characterIsCommentedOut Checks whether the character at the given
   *   position in the given string is commented out
   * @param text The text to check the character in
   * @param pos The position of the character to check for in the text
   * return Whether the character is commented out
   */
  bool characterIsCommentedOut(const QString& text, int pos);

  private slots:
  /**
   * @brief showContextMenu Creates and shows a context menu
   * @param position The position where to create the context menu
   */
  void showContextMenu(const QPoint& position);

  /**
   * @brief updateLineNumberAreaWidth Updates the width of the line number area
   */
  void updateLineNumberAreaWidth(int);

  /**
   * @brief updateLineNumberArea Updates the line number area after the
   *   scrollbar has been used
   * @param rect The rectangle that covers the line number area
   * @param dy The amount of pixels scrolled
   */
  void updateLineNumberArea(const QRect& rect, int dy);
};

} // namespace mcrl2::gui::qt

#endif // CODEEDITOR_H
