// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/**

  @file threadparent.h
  @author R. Boudewijns

  A ThreadParent template that can be used to assign the thread a virtual parent such that it is automatically deleted when the parent is destucted.

*/

#ifndef MCRL2XI_THREADPARENT_H
#define MCRL2XI_THREADPARENT_H

#include <QObject>

template <class T>
class ThreadParent : public QObject
{
public:
    /**
     * @brief Constructor that creates a new thread object and sets the ObjectName to the className of the object
     * @param parent The virtual parent for the thread
     */
    ThreadParent(QObject *parent):
        QObject(parent)
    {
        m_thread = new T();
        setObjectName(QString::fromStdString(T::className));
    }

    /**
     * @brief Destructor that deletes the thread in a thread-safe way
     */
    ~ThreadParent()
    {
        m_thread->deleteLater();
    }

    /**
     * @brief Returns a pointer to the thread object
     */
    T *getThread()
    {
        return m_thread;
    }

private:

    T *m_thread;    ///< The thread object

};

#endif // MCRL2XI_THREADPARENT_H
