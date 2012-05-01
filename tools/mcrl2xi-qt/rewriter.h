// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file rewriter.h
/// \brief A rewriter object that moves itself to the aTerm Thread upon construction

#ifndef MCRL2XI_REWRITER_H
#define MCRL2XI_REWRITER_H

#include <QObject>
#include "mcrl2/data/rewrite_strategy.h"
#include "mcrl2/data/classic_enumerator.h"

class Rewriter : public QObject
{
    Q_OBJECT
  public:
    // Constructor
    Rewriter();

    // Extra variable to save the className that was needed for FindChild (see mainwindow.cpp)
    static const std::string className;

  signals:
    // Signal to indicate the rewrite process is finished
    void rewritten(QString output);
    // Signal to indicate an epression error
    void exprError();
    
  public slots:
    // Slots to set-up and start the rewriting process
    void setRewriter(QString rewriter);
    void rewrite(QString specification, QString dataExpression);
    
  private:
    // Variables to save the rewrite stategy, specification and variables
    mcrl2::data::rewrite_strategy m_rewrite_strategy;
    mcrl2::data::data_specification m_data_spec;
    atermpp::set <mcrl2::data::variable > m_vars;

    // Boolean indicating if the last specification was successfully parsed (used to cache the parsing step)
    bool m_parsed;

    // String containing the last specification that was parsed
    QString m_specification;
};

#endif // MCRL2XI_REWRITER_H
