// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef COPYTHREAD_H
#define COPYTHREAD_H

#include <QThread>
#include <QStringList>

class CopyThread : public QThread
{
    Q_OBJECT
  public:
    void run();
  signals:
    void busy(int num, QString filename);
    void remove(QString filename);

  public slots:
    void init(QString oldPath, QString newPath, QStringList files, bool move = false);
    void cancel();

  private:
    bool m_cancel;
    QString m_oldpath, m_newpath;
    QStringList m_files;
    bool m_move;

};

#endif // COPYTHREAD_H
