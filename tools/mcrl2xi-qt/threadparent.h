// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file threadparent.h
/// \brief A ThreadParent template that can be used to assign the thread a virtual parent such that it is automatically deleted when the parent is destucted.

#ifndef MCRL2XI_THREADPARENT_H
#define MCRL2XI_THREADPARENT_H

#include <QObject>

template <class T>
class ThreadParent : public QObject
{
public:
    // Constructor that creates a new thread object and sets the ObjectName to the className of the object
    ThreadParent(QObject *parent):
        QObject(parent)
    {
        m_thread = new T();
        setObjectName(QString::fromStdString(T::className));
    }

    // Destructor that deletes the thread in a thread-safe way
    ~ThreadParent()
    {
        m_thread->deleteLater();
    }

    // Getter to retreive the thread itself
    T *getThread()
    {
        return m_thread;
    }

private:
    // Pointer to the thread itself
    T *m_thread;

};

#endif // MCRL2XI_THREADPARENT_H
