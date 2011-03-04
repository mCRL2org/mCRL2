// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tooltipListView.h

#include <wx/listctrl.h>
#include <wx/menuitem.h>

enum displayTooltip { DISPLAY_CURRENT_STATE, DISPLAY_TRANSITION };

class tooltipListView : public wxListView
{

public:
   tooltipListView() { };
   tooltipListView( wxWindow *parent,
                wxWindowID winid = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxLC_REPORT,
                const wxValidator& validator = wxDefaultValidator,
                const wxString &name = wxListCtrlNameStr)
   {
       Create(parent, winid, pos, size, style, validator, name);
			 showToolTip = true;
   }
 
   void OnMouseMotion(wxMouseEvent& event);

	 void SetControl(wxMenuItem* i, displayTooltip type);

   private:
   wxMenuItem* control;
   displayTooltip display_type;


   bool showToolTip;
   DECLARE_EVENT_TABLE()

};
