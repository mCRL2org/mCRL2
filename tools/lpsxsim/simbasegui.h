// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file simbasegui.h

#ifndef __simbasegui_H__
#define __simbasegui_H__

#include <wx/wx.h>
#include "simbase.h"

class SimulatorInterfaceGUI : virtual public SimulatorInterface
{
public:
	virtual wxWindow *MainWindow() = 0;
	/* Returns the main window of the simulator. */
};

inline wxWindow *GetMainWindow(SimulatorInterface *Simulator)
{
  SimulatorInterfaceGUI *g = dynamic_cast<SimulatorInterfaceGUI *>(Simulator);
  if ( g != NULL ) // downcast successful?
  {
    return g->MainWindow();
  }

  return NULL;
}

wxWindow *GetMainWindow(SimulatorInterface *Simulator);
/* Returns Simulator->MainWindow() if Simulator is a SimulatorInterfaceGUI
 * and NULL otherwise */

#endif
