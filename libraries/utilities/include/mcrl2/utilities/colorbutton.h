// Author(s): Hannes Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/colorbutton.h
/// \brief Implementation of a colour selection widget shaped as button

#ifndef COLORBUTTON_H
#define COLORBUTTON_H
#include <wx/wx.h>
#include <wx/colordlg.h>

namespace mcrl2 {
  namespace utilities {
    namespace wx {

      /**
       * \brief A widget for choosing colours
       *
       * The widget consists of a small square with the selected colour as
       * background.  Clicking the square will activate a colour picking dialog.
       * The colour that is selected in the dialog is taken as the new colour of
       * the square.
       **/
      class wxColorButton : public wxPanel {

        protected:

          /// \brief Window used to centering a colour dialog
          wxWindow* appFrame;

          DECLARE_EVENT_TABLE()

        private:

          /// \brief Event handler for mouse down events
          void OnMouseDown( wxMouseEvent& event );

        public:

          /// \brief Constructor
          wxColorButton( wxWindow* parent, wxWindow* topframe, wxWindowID id = wxID_ANY,
                          const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize );
      };
    }
  }
}

#endif
