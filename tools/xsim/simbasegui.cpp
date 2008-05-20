// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING).
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file simbasegui.cpp

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "simbasegui.h"
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/wx.h>
#include "simbasegui.h"

wxWindow *GetMainWindow(SimulatorInterface *Simulator)
{
  SimulatorInterfaceGUI *g = dynamic_cast<SimulatorInterfaceGUI *>(Simulator);
  if ( g != NULL ) // downcast successful?
  {
    return g->MainWindow();
  } else {
    return NULL;
  }
}
