// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file simulatorgui.cpp

#include "wx.hpp" // precompiled headers

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <wx/wx.h>
#include <wx/dynlib.h>
#include "simulatorgui.h"

using namespace std;

StandardSimulatorGUI::StandardSimulatorGUI(wxWindow *window)
{
  w = window;
}

StandardSimulatorGUI::~StandardSimulatorGUI()
{
}

void StandardSimulatorGUI::LoadView(const string &filename)
{
	wxDynamicLibrary lib(wxConvLocal.cMB2WX(filename.c_str()));

	if ( lib.IsLoaded() )
	{
		void (*f)(SimulatorInterface *);

		f = (void (*)(SimulatorInterface *)) lib.GetSymbol(wxT("SimulatorViewDLLAddView"));
		if ( f != NULL )
		{
			f(this);
			lib.Detach(); //XXX
		} else {
			wxMessageDialog msg(w, wxT("DLL does not appear to contain a View."), wxT("Error"), wxOK|wxICON_ERROR);
			msg.ShowModal();
		}
	} else {
		/*wxMessageDialog msg(this, wxT("Failed to open DLL."), wxT("Error"), wxOK|wxICON_ERROR);
		msg.ShowModal();*/
       }
}

wxWindow *StandardSimulatorGUI::MainWindow()
{
  return w;
}
