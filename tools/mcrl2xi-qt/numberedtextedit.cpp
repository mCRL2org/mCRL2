// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "numberedtextedit.h"
#include <QPainter>
#include <QScrollBar>
#include <QTextBlock>
#include <QAbstractTextDocumentLayout>

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(NumberedTextEdit *editor) : QWidget(editor) {
        m_edit = editor;
        setStyleSheet("Background: #DDD;");
        setAutoFillBackground(true);
    }

    QSize sizeHint() const {
        return QSize(m_edit->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event)
    {
        m_edit->lineNumberAreaPaintEvent(event);
    }

private:
    NumberedTextEdit* m_edit;
};















NumberedTextEdit::NumberedTextEdit(QWidget *parent) :
    QTextEdit(parent)
{
    m_numberArea = new LineNumberArea(this);
    connect(document(), SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth()));

    updateLineNumberAreaWidth();
}

int NumberedTextEdit::lineNumberAreaWidth()
{
    int blocks = document()->blockCount();
    int digits = 1;
    while (blocks >= 10)
    {
        blocks /= 10;
        digits++;
    }
    return (5+fontMetrics().width('9')*digits);
}

void NumberedTextEdit::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(m_numberArea);
    painter.setPen(Qt::black);

    QTextBlock block = document()->firstBlock();
    int line = 1;
    int offset = verticalScrollBar()->value();
    int lineheight = fontMetrics().height();

    while (block.isValid())
    {
        QPointF pos = document()->documentLayout()->blockBoundingRect(block).topLeft();

        // Stop if we have done the last visible line
        if (pos.y() > offset+viewport()->height())
        {
            break;
        }

        // Only draw if the line is visible
        if (pos.y()+lineheight > offset)
        {
            painter.drawText(0, pos.y()-offset, m_numberArea->width()-3, lineheight, Qt::AlignRight, QString::number(line));
        }

        block = block.next();
        line++;
    }
}

void NumberedTextEdit::updateLineNumberAreaWidth()
{
    // Offset the contents
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);

    // And update the line number area
    m_numberArea->repaint();
}

void NumberedTextEdit::resizeEvent(QResizeEvent *e)
{
    QTextEdit::resizeEvent(e);

    // Resize the line number area
    QRect cr = contentsRect();
    m_numberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void NumberedTextEdit::scrollContentsBy(int dx, int dy)
{
    QTextEdit::scrollContentsBy(dx, dy);

    // Update the line number area
    m_numberArea->repaint();
}
