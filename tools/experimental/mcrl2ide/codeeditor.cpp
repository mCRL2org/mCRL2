#include "codeeditor.h"

#include <QPainter>
#include <QTextBlock>

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    /* set the font used (monospaced) */
    QFont *font = new QFont("Courier New", 10);
    this->document()->setDefaultFont(*font);

    /* set the tab width to 4 characters */
    QFontMetrics *fm = new QFontMetrics(*font);
    this->setTabStopDistance(fm->width("1234"));

    lineNumberArea = new LineNumberArea(this);

    /* signals that need to change the line number area */
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));

    updateLineNumberAreaWidth(0);
}

/**
 * @brief CodeEditor::lineNumberAreaWidth computes the width needed for the line number area
 * @return the width needed for the line number area
 */
int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = std::max(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    return 3 + fontMetrics().width(QLatin1Char('9')) * digits;
}

/**
 * @brief CodeEditor::updateLineNumberAreaWidth updates the width of the line number area
 */
void CodeEditor::updateLineNumberAreaWidth(int)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

/**
 * @brief CodeEditor::updateLineNumberArea updates the line number area after the scrollbar has been used
 * @param rect the rectangle that covers the line number area
 * @param dy the amount of pixels scrolled
 */
void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

/**
 * @brief CodeEditor::resizeEvent resizes the line number area when the window is resized
 * @param e the resize event
 */
void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

/**
 * @brief CodeEditor::lineNumberAreaPaintEvent paints the line number area on the screen
 * @param event a paint event
 */
void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(-2, top, lineNumberArea->width(), fontMetrics().height(), Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}
