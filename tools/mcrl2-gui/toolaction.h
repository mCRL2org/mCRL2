// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef TOOLACTION_H
#define TOOLACTION_H

#include <QAction>
#include "toolinformation.h"

class ToolAction : public QAction
{
    Q_OBJECT
public:
    explicit ToolAction(ToolInformation information, QObject *parent = 0);
    ToolInformation information() { return m_info; }
private:
    ToolInformation m_info;
};

#endif // TOOLACTION_H
