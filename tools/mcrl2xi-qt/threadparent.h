// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2XI_THREADPARENT_H
#define MCRL2XI_THREADPARENT_H

#include <QObject>

template <class T>
class ThreadParent : public QObject
{
public:
    ThreadParent(QObject *parent):
        QObject(parent)
    {
        m_thread = new T();
        setObjectName(QString::fromStdString(T::className));
    }
    ~ThreadParent()
    {
        m_thread->deleteLater();
    }
    T *getThread()
    {
        return m_thread;
    }

private:
    T *m_thread;
};

#endif // MCRL2XI_THREADPARENT_H
