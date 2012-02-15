// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file simulatorgui.h

#ifndef __simulatorgui_H__
#define __simulatorgui_H__

#include <wx/wx.h>
#include <string>
#include "simbasegui.h"
#include "mcrl2/lps/simulator.h"

class StandardSimulatorGUI: public StandardSimulator, public SimulatorInterfaceGUI
{
  public:
    // constructors and destructors
    StandardSimulatorGUI(wxWindow* window);
    virtual ~StandardSimulatorGUI();
    virtual void LoadView(const std::string& filename);
    virtual wxWindow* MainWindow();
  private:
    wxWindow* w;
};

#endif
