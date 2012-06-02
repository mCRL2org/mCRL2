// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "toolaction.h"

ToolAction::ToolAction(ToolInformation information, QObject *parent) :
  QAction(parent), m_info(information)
{
  setText(m_info.getName());
  setToolTip(m_info.getDescription());
}

