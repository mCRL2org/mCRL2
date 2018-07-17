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

class HighlightingRule
{
  public:
  QRegExp pattern;
  QTextCharFormat format;

  HighlightingRule(QRegExp pattern, QTextCharFormat format);
};

/**
 * @brief The CodeHighlighter class defines a syntax highlioghrt for mCRL2
 *   specifcations
 */
class CodeHighlighter : public QSyntaxHighlighter
{
  Q_OBJECT

  public:
  /**
   * @brief CodeHighlighter Constructor
   * @param spec Whether this code editor is for a mcrl2 specifcation or a mcf
   *   formula
   * @param parent The document on which the code highlighter should operate
   */
  CodeHighlighter(bool spec, QTextDocument* parent = 0);

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
class LineNumbersArea : public QWidget
{
  public:
  /**
   * @brief LineNumberArea Constructor
   * @param editor The code editor this line number area belongs to
   */
  LineNumbersArea(CodeEditor* editor);

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
   * @param spec Whether this code editor is for a mcrl2 specifcation or a mcf
   *   formula
   */
  explicit CodeEditor(QWidget* parent = 0, bool spec = false);

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
   */
  void zoomIn();

  /**
   * @brief zoomOut Allows the user to zoom out from the text
   */
  void zoomOut();

  protected:
  /**
   * @brief keyPressEvent Adds key events for aoom in and zoom out
   * @brief event The key event
   */
  void keyPressEvent(QKeyEvent* event) override;

  /**
   * @brief resizeEvent Resizes the line number area when the window is resized
   * @param event The resize event
   */
  void resizeEvent(QResizeEvent* event) override;

  private:
  QFont codeFont;
  QFont lineNumberFont;
  LineNumbersArea* lineNumberArea;
  CodeHighlighter* highlighter;

  QMenu* contextMenu;
  QAction* zoomInAction;
  QAction* zoomOutAction;

  /**
   * @brief setFontSize Sets the font size and tab width
   * @param pixelSize The desired font size in pixels
   */
  void setFontSize(int pixelSize);

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
  void updateLineNumberArea(const QRect&, int);
};

#endif // CODEEDITOR_H
