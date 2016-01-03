// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <QtGui>

#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *parent)
  : QSyntaxHighlighter(parent)
{
  HighlightingRule rule;

  //Identifiers
  classFormat.setFontWeight(QFont::DemiBold);
  classFormat.setForeground(Qt::black);
  rule.pattern = QRegExp("\\b[a-zA-Z_][a-zA-Z0-9_']*\\b");
  rule.format = classFormat;
  highlightingRules.append(rule);

  //Spec Keywords
  keywordFormat.setForeground(Qt::darkBlue);
  keywordFormat.setFontWeight(QFont::Bold);
  QStringList SpecKeywordPatterns;
  SpecKeywordPatterns << "\\bsort\\b" << "\\bcons\\b" << "\\bmap\\b"
                      << "\\bvar\\b" << "\\beqn\\b" << "\\bact\\b"
                      << "\\bproc\\b" << "\\binit\\b" << "\\bstruct\\b";
  foreach (const QString &pattern, SpecKeywordPatterns) {
    rule.pattern = QRegExp(pattern);
    rule.format = keywordFormat;
    highlightingRules.append(rule);
  }

  //Process Keywords
  keywordFormat.setForeground(Qt::darkCyan);
  keywordFormat.setFontWeight(QFont::Light);
  QStringList ProcCKeywordPatterns;
  ProcCKeywordPatterns << "\\bdelta\\b" << "\\btau\\b";
  foreach (const QString &pattern, ProcCKeywordPatterns) {
    rule.pattern = QRegExp(pattern);
    rule.format = keywordFormat;
    highlightingRules.append(rule);
  }

  keywordFormat.setForeground(Qt::blue);
  keywordFormat.setFontWeight(QFont::Bold);
  QStringList ProcKeywordPatterns;
  ProcKeywordPatterns << "\\bsum\\b" << "\\bblock\\b" << "\\ballow\\b"
                      << "\\bhide\\b" << "\\brename\\b" << "\\bcomm\\b";
  foreach (const QString &pattern, ProcKeywordPatterns) {
    rule.pattern = QRegExp(pattern);
    rule.format = keywordFormat;
    highlightingRules.append(rule);
  }

  keywordFormat.setForeground(Qt::darkMagenta);
  keywordFormat.setFontWeight(QFont::Bold);
  QStringList SortCKeywordPatterns;
  SortCKeywordPatterns << "\\bBool\\b" << "\\bPos\\b" << "\\bNat\\b"
                       << "\\bInt\\b" << "\\bReal\\b";
  foreach (const QString &pattern, SortCKeywordPatterns) {
    rule.pattern = QRegExp(pattern);
    rule.format = keywordFormat;
    highlightingRules.append(rule);
  }

  keywordFormat.setForeground(Qt::darkGreen);
  keywordFormat.setFontWeight(QFont::Bold);
  QStringList SortKeywordPatterns;
  SortKeywordPatterns << "\\bList\\b" << "\\bSet\\b" << "\\bBag\\b" << "\\bFSet\\b" << "\\bFBag\\b";
  foreach (const QString &pattern, SortKeywordPatterns) {
    rule.pattern = QRegExp(pattern);
    rule.format = keywordFormat;
    highlightingRules.append(rule);
  }

  keywordFormat.setForeground(Qt::darkYellow);
  keywordFormat.setFontWeight(QFont::Bold);
  QStringList DataCKeywordPatterns;
  DataCKeywordPatterns << "\\btrue\\b" << "\\bfalse\\b";
  foreach (const QString &pattern, DataCKeywordPatterns) {
    rule.pattern = QRegExp(pattern);
    rule.format = keywordFormat;
    highlightingRules.append(rule);
  }

  keywordFormat.setForeground(Qt::darkRed);
  keywordFormat.setFontWeight(QFont::Light);
  QStringList DataKeywordPatterns;
  DataKeywordPatterns << "\\bwhr\\b" << "\\bend\\b" << "\\blambda\\b"
                      << "\\bforall\\b" << "\\bexists\\b" << "\\bdiv\\b"
                      << "\\bmod\\b" << "\\bin\\b";
  foreach (const QString &pattern, DataKeywordPatterns) {
    rule.pattern = QRegExp(pattern);
    rule.format = keywordFormat;
    highlightingRules.append(rule);
  }

  keywordFormat.setForeground(Qt::red);
  keywordFormat.setFontWeight(QFont::Bold);
  QStringList TodoKeywordPatterns;
  TodoKeywordPatterns << "\\bcontained\\b" << "\\bTODO\\b" << "\\bFIXME\\b"
                      << "\\bXXX\\b";
  foreach (const QString &pattern, TodoKeywordPatterns) {
    rule.pattern = QRegExp(pattern);
    rule.format = keywordFormat;
    highlightingRules.append(rule);
  }

  keywordFormat.setForeground(Qt::darkCyan);
  keywordFormat.setFontWeight(QFont::Bold);
  QStringList OperatorKeywordPatterns;
  OperatorKeywordPatterns << "\\bmin\\b" << "\\bmax\\b" << "\\bsucc\\b"
                          << "\\bpred\\b" << "\\babs\\b" << "\\bfloor\\b"
                          << "\\bceil\\b" << "\\bround\\b" << "\\bexp\\b"
                          << "\\bA2B\\b" << "\\bhead\\b" << "\\btail\\b"
                          << "\\brhead\\b" << "\\brtail\\b" << "\\bcount\\b"
                          << "\\bSet2Bag\\b" << "\\bBag2Set\\b";
  foreach (const QString &pattern, OperatorKeywordPatterns) {
    rule.pattern = QRegExp(pattern);
    rule.format = keywordFormat;
    highlightingRules.append(rule);
  }

  //Operators
  classFormat.setFontWeight(QFont::DemiBold);
  classFormat.setForeground(Qt::darkGreen);
  rule.pattern = QRegExp("[\\.\\+|&<>:;=@(){}\\[\\],\\!\\*/\\\\-]");
  rule.format = classFormat;
  highlightingRules.append(rule);
  rule.pattern = QRegExp("\\|\\|_");
  rule.format = classFormat;
  highlightingRules.append(rule);
  rule.pattern = QRegExp("->");
  rule.format = classFormat;
  highlightingRules.append(rule);

  //Numbers
  quotationFormat.setForeground(Qt::darkGreen);
  rule.pattern = QRegExp("\\b[0-9]+\\b");
  rule.format = quotationFormat;
  highlightingRules.append(rule);

  //    functionFormat.setFontItalic(true);
  //    functionFormat.setForeground(Qt::blue);
  //    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
  //    rule.format = functionFormat;
  //    highlightingRules.append(rule);


  //Single Line Comment
  singleLineCommentFormat.setForeground(Qt::gray);
  rule.pattern = QRegExp("%[^\n]*");
  rule.format = singleLineCommentFormat;
  highlightingRules.append(rule);

}

void Highlighter::highlightBlock(const QString &text)
{
  foreach (const HighlightingRule &rule, highlightingRules) {
    QRegExp expression(rule.pattern);
    int index = expression.indexIn(text);
    while (index >= 0) {
      int length = expression.matchedLength();
      setFormat(index, length, rule.format);
      index = expression.indexIn(text, index + length);
    }
  }
}
