// Author(s): S.W.C. Ploeger, D. Reniers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cmath>
#include <string>

#include "spinctrlfloat.h"

using namespace std;

/**
 * \brief Rounds a double to an integer value
 **/
static int roundToInt( double f ) {
  double intpart;
  modf( f + 0.5, &intpart );
  return static_cast< int > ( intpart );
}

namespace mcrl2 {
  namespace utilities {
    namespace wx {
      /**
       * \param[in] parent the wxWidgets window object of which this object will be a child
       * \param[in] id a wxWidgets identifier for this widget
       * \param[in] p_Min the minimum value
       * \param[in] p_Max the maximum value
       * \param[in] p_Rate the resolution of the spin control
       * \param[in] p_Init the initial value
       * \param[in] pos the position for this widget
       * \param[in] size the size of this widget
       **/
      wxSpinCtrlFloat::wxSpinCtrlFloat(
          wxWindow* parent,
          wxWindowID id,
          double p_Min,
          double p_Max,
          double p_Rate,
          double p_Init,
          const wxPoint& pos,
          const wxSize&  size)
          : wxPanel(
              parent,
              wxID_ANY,
              pos,
              size,
              wxNO_BORDER ),
            m_Rate(p_Rate)
      {
        wxBoxSizer* Sizer = new wxBoxSizer( wxHORIZONTAL );
        m_TextCtrl = new wxTextCtrl( this, 0, wxEmptyString, wxDefaultPosition,
            wxDefaultSize, wxTE_PROCESS_ENTER );
        m_SpinButton = new wxSpinButton( this, 1, wxDefaultPosition, wxDefaultSize,
            wxSP_VERTICAL );

        // spin button should be at most as high as the text control
        int w; int h;
        m_TextCtrl->GetSize( &w, &h );
        m_SpinButton->SetSizeHints( -1, h, -1, h );

        Sizer->Add( m_TextCtrl, 1, wxEXPAND, 0 );
        Sizer->Add( m_SpinButton , 0, wxEXPAND, 0 );

        m_SpinButton->SetRange( roundToInt( p_Min/p_Rate ), roundToInt( p_Max/p_Rate ) );
        SetValue( p_Init );

        Sizer->SetMinSize( size );
        this->SetSizer( Sizer );
        Sizer->SetSizeHints( this );
      }

      BEGIN_EVENT_TABLE(wxSpinCtrlFloat, wxPanel)
        EVT_TEXT_ENTER( 0, wxSpinCtrlFloat::OnEnter )
        EVT_SPIN( 1, wxSpinCtrlFloat::OnSpin )
      END_EVENT_TABLE()

      void wxSpinCtrlFloat::OnEnter( wxCommandEvent& event )
      {
        string str( event.GetString().fn_str() );

        float f;
        sscanf( str.c_str(), "%f", &f );

        f = (std::min)( m_SpinButton->GetMax() * m_Rate, (std::max)( static_cast < double > (f), m_SpinButton->GetMin() * m_Rate ) );
        SetValue( f );

        wxCommandEvent NewEvent( wxEVT_COMMAND_SPINCTRL_UPDATED, GetId());
        NewEvent.SetEventObject( this );
        GetParent()->GetEventHandler()->ProcessEvent( NewEvent );
      }

      void wxSpinCtrlFloat::OnSpin( wxSpinEvent& event )
      {
        int i = event.GetInt();
        m_TextCtrl->SetValue( wxString::Format( wxT("%2.1f"), i * m_Rate ) );

        wxCommandEvent NewEvent( wxEVT_COMMAND_SPINCTRL_UPDATED, GetId());
        NewEvent.SetEventObject( this );
        GetParent()->GetEventHandler()->ProcessEvent( NewEvent );
      }

      double wxSpinCtrlFloat::GetValue()
      {
        return m_Rate * m_SpinButton->GetValue();
      }

      void wxSpinCtrlFloat::SetValue( float v )
      {
        m_TextCtrl->SetValue( wxString::Format( wxT("%2.1f"), v ) );
        m_SpinButton->SetValue( roundToInt( v/m_Rate ) );
      }
    }
  }
}

// -- end -----------------------------------------------------------
