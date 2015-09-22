// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <QMutex>
#include <QMutexLocker>
#include "mcrl2/gui/atermthread.h"

using namespace mcrl2::gui::qt::detail;

AtermThread *aterm_thread = 0;
QMutex aterm_thread_mutex;

QThread *mcrl2::gui::qt::get_aterm_thread()
{
  QMutexLocker locker(&aterm_thread_mutex);
  if (aterm_thread)
  {
    return aterm_thread;
  }

  aterm_thread = new AtermThread();
  aterm_thread->start();
  return aterm_thread;
}
