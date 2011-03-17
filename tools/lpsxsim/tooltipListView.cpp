// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tooltipListView.cpp

#include "tooltipListView.h"
#include <wx/menuitem.h>

BEGIN_EVENT_TABLE(tooltipListView,wxListView)
  EVT_MOTION(tooltipListView::OnMouseMotion)
END_EVENT_TABLE()
 
void tooltipListView::OnMouseMotion(wxMouseEvent& event)
{
  if( !control->IsChecked() )
  {
    this->SetToolTip( wxEmptyString );
    return;
  }

  // Display state transitions
  if( display_type == DISPLAY_TRANSITION )
  {
    long row=-1;
    int flags=0;
    wxPoint pt;

    pt = event.GetPosition();
    row = this->HitTest(pt, flags);

    if( row >= 0 )
    {
      wxString TransTooltip;
      TransTooltip.Append( wxT("- Action:\n") );

      wxListItem     info;

      //Prepare Cell: (x,0)
      info.m_itemId = row;
      info.m_col = 0;
      info.m_mask = wxLIST_MASK_TEXT;
      this->GetItem( info );

      // Extract the text from cell
      TransTooltip.Append( wxT("  ") + info.m_text + wxT("\n") );

      TransTooltip.Append( wxT("- State Change:\n") );

      //Prepare Cell: (x,1)
      info.m_col = 1;
      this->GetItem( info );
      wxString tmp = info.m_text;
      if( !tmp.IsEmpty() )
      {
        tmp.Replace(wxT(","), wxT("\n  "));
        TransTooltip.Append( wxT("  ") + tmp ) ;
      }
      this->SetToolTip( TransTooltip );
    }
    else
    {
      this->SetToolTip( NULL );
    }
  }

  // Display current state
  if( display_type == DISPLAY_CURRENT_STATE )
  {

    wxString TransTooltip = wxT("Current state:");
    for( int row = 0; row < this->GetItemCount(); ++row )
    {
      wxListItem     info;
      info.m_itemId = row;
      info.m_col = 0;
      info.m_mask = wxLIST_MASK_TEXT;
      this->GetItem( info );
      // Extract the text from cell
      TransTooltip.Append( wxT("\n  ") + info.m_text + wxT(" = ") );

      //Prepare Cell: (x,1)
      info.m_col = 1;
      this->GetItem( info );
      TransTooltip.Append( info.m_text );
    }

    this->SetToolTip( TransTooltip );
  }
};

void tooltipListView::SetControl(wxMenuItem* i, displayTooltip type)
{
  control = i;
  display_type = type;
};
