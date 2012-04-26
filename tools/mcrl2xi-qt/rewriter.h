// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2XI_REWRITER_H
#define MCRL2XI_REWRITER_H

#include <QObject>
#include "mcrl2/data/rewrite_strategy.h"

class Rewriter : public QObject
{
    Q_OBJECT
  public:
    explicit Rewriter();

  signals:
    void rewritten(QString output);
    
  public slots:
    void setRewriter(QString rewriter);
    void setSpecification(QString specification);
    void rewrite(QString dataExpression);
    
  private:
    mcrl2::data::rewrite_strategy m_rewrite_strategy;
    std::string m_specification;
};

#endif // MCRL2XI_REWRITER_H
