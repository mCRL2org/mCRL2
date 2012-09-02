// Author(s): Frank Stappers and Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/**

  @file highlighter.h
  @author R. Boudewijns and F. Stappers

  Highlicher class that implements syntax highlighting for a QTextEdit

*/

#ifndef MCRL2XI_HIGHLIGHTER_H
#define MCRL2XI_HIGHLIGHTER_H

#include <QSyntaxHighlighter>

#include <QHash>
#include <QTextCharFormat>

class QTextDocument;

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

  public:
    /**
     * @brief Constructor
     * @param parent The QTextDocument on which the highlighter should operate
     */
    Highlighter(QTextDocument *parent = 0);

  protected:
    /**
     * @brief Highlights a single block of text
     */
    void highlightBlock(const QString &text);

  private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
};

#endif // MCRL2XI_HIGHLIGHTER_H

