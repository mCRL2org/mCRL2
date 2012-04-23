// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_UTILITIES_ATERMTHREAD_H
#define MCRL2_UTILITIES_ATERMTHREAD_H

#include <QMutex>
#include <QMutexLocker>
#include <QThread>
#include "mcrl2/atermpp/aterm_init.h"

namespace mcrl2
{
namespace utilities
{
namespace qt
{

namespace detail
{

class AtermThread : public QThread
{
  Q_OBJECT

  protected:
    void run()
    {
      atermpp::aterm term;
      atermpp::aterm_init(term);
      exec();
    }
};

AtermThread *aterm_thread = 0;
QMutex aterm_thread_mutex;

} // namespace detail

QThread *get_aterm_thread()
{
  QMutexLocker locker(&detail::aterm_thread_mutex);
  if (detail::aterm_thread)
  {
    return detail::aterm_thread;
  }

  detail::aterm_thread = new detail::AtermThread();
  detail::aterm_thread->start();
  return detail::aterm_thread;
}


} // namespace qt
} // namespace utilities
} // namespace mcrl2

#endif // MCRL2_UTILITIES_ATERMTHREAD_H
