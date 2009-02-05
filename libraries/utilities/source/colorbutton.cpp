// Author(s): Hannes Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "colorbutton.h"

namespace mcrl2 {
  namespace utilities {
    namespace wx {
      /**
       * \param[in] parent the wxWidgets window object of which this object will be a child
       * \param[in] topframe the window that is used to position the colour dialog
       * \param[in] id a wxWidgets identifier for this widget
       * \param[in] pos the position for this widget
       * \param[in] size the size of this widget
       **/
      wxColorButton::wxColorButton( wxWindow* parent, wxWindow* topframe, wxWindowID
          id, const wxPoint& pos, const wxSize& size )
        : wxPanel( parent, id, pos, size, wxSUNKEN_BORDER ), appFrame(topframe) {
      }

      void wxColorButton::OnMouseDown( wxMouseEvent& /*event*/ ) {
        wxColourData coldat;
        coldat.SetColour( GetBackgroundColour() );
        wxColourDialog* coldlg = new wxColourDialog( appFrame, &coldat );
        coldlg->CentreOnParent();
        if ( coldlg->ShowModal() == wxID_OK )
        {
          coldat = coldlg->GetColourData();
          SetBackgroundColour( coldat.GetColour() );
          ClearBackground();

          wxCommandEvent cmdEvent( wxEVT_COMMAND_BUTTON_CLICKED, GetId() );
          cmdEvent.SetEventObject( this );
          GetParent()->ProcessEvent( cmdEvent );
        }
        coldlg->Close();
        coldlg->Destroy();
      }

      BEGIN_EVENT_TABLE( wxColorButton, wxPanel )
        EVT_LEFT_DOWN( wxColorButton::OnMouseDown )
      END_EVENT_TABLE()
    }
  }
}
