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
#include "rewriterthread.h"

class Rewriter : public QObject
{
    Q_OBJECT
  public:
    Rewriter(QObject *parent);
    ~Rewriter();
    RewriterThread *getThread();

  private:
    RewriterThread *m_rewriterthread;
};

#endif // MCRL2XI_REWRITER_H
