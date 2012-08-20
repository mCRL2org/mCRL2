// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NUMBEREDTEXTEDIT_H
#define NUMBEREDTEXTEDIT_H

#include <QTextEdit>

class NumberedTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit NumberedTextEdit(QWidget *parent = 0);

    int lineNumberAreaWidth();
    void lineNumberAreaPaintEvent(QPaintEvent* event);

public slots:
    void updateLineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *e);
    void scrollContentsBy(int dx, int dy);

private:
    QWidget* m_numberArea;
};

#endif // NUMBEREDTEXTEDIT_H
