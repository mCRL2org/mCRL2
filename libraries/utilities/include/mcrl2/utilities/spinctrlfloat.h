// Author(s): S.W.C. Ploeger, D. Reniers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/spinctrlfloat.h
/// \brief Interface of a floating point selection widget

#ifndef SPINCTRLFLOAT_H
#define SPINCTRLFLOAT_H

#include <wx/wx.h>
#include <wx/spinbutt.h>
#include <wx/spinctrl.h>

namespace mcrl2 {
  namespace utilities {
    namespace wx {

      /**
       * \brief A spin control widget
       *
       * A widget that consists of a text field with a string that represents a
       * numeric value in it that is editable by the user, and a pair of buttons
       * that can be used to increase or decrease the value in the text field
       * with a configurable amount.
       **/
      class wxSpinCtrlFloat : public wxPanel {

        private:

          /// \brief A wxWidgets text control, used to display the current value
          wxTextCtrl*   m_TextCtrl;
          /// \brief A wxWidgets spin button, used to manipulate the current value
          wxSpinButton* m_SpinButton;

          /// \brief The spin button resolution
          double m_Rate;

          /// \brief The rounded minimum value
          int    m_Min;

          /// \brief The rounded maximum value
          int    m_Max;

          DECLARE_EVENT_TABLE()

        private:

          /// \brief Event handler for SPIN events
          void OnEnter(wxCommandEvent & event);

          /// \brief Event handler for SPIN events
          void OnSpin(wxSpinEvent & event);

        public:

          /// \brief Constructor
          wxSpinCtrlFloat(
              wxWindow* parent,
              wxWindowID id,
              double p_Min,
              double p_Max,
              double p_Rate,
              double p_Init,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize );

          /// \brief Gets current value
          double GetValue();

          /// \brief Sets current value
          void SetValue(float v);
      };
    }
  }
}

#endif
