// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "rewriter.h"
#include "rewriterthread.h"

Rewriter::Rewriter(QObject *parent):
    QObject(parent)
{
  m_rewriterthread = new RewriterThread();
}

Rewriter::~Rewriter()
{
  m_rewriterthread->deleteLater();
}

RewriterThread *Rewriter::getThread()
{
  return m_rewriterthread;
}
