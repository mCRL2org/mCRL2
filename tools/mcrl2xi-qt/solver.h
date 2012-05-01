// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file solver.h
/// \brief A solver object that moves itself to the aTerm Thread upon construction

#ifndef MCRL2XI_SOLVER_H
#define MCRL2XI_SOLVER_H

#include <QObject>
#include "mcrl2/data/rewrite_strategy.h"
#include "mcrl2/data/classic_enumerator.h"

class Solver : public QObject
{
    Q_OBJECT
  public:
    // Constuctor
    Solver();

    // Extra variable to save the className that was needed for FindChild (see mainwindow.cpp)
    static const std::string className;

  signals:
    // Signal to indicate a part was solved (new output)
    void solvedPart(QString output);
    // Signal to indicate that the complete problem was solved
    void solved();
    
  public slots:
    // Slots to set-up, start and stop the solving process
    void setRewriter(QString rewriter);
    void solve(QString specification, QString dataExpression);
    void abort();
    
  private:
    // Variables to save the rewrite stategy, specification and variables
    mcrl2::data::rewrite_strategy m_rewrite_strategy;
    mcrl2::data::data_specification m_data_spec;
    atermpp::set <mcrl2::data::variable > m_vars;

    // Bool that is set when the solving should be aborted as quickly as possible
    bool m_abort;

    // Boolean indicating if the last specification was successfully parsed (used to cache the parsing step)
    bool m_parsed;

    // String containing the last specification that was parsed
    QString m_specification;
};

#endif // MCRL2XI_SOLVER_H
