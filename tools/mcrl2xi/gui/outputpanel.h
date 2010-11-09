// Author(s): Frank Stappers
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file outputpanel.h
//
// Implements the output class used to display log messages.

#ifndef MCRL2XI_OUTPUTPANEL_H
#define MCRL2XI_OUTPUTPANEL_H

#include <wx/wx.h>
#include <wx/textctrl.h>

    class outputpanel : public wxTextCtrl
    {
      public:
        outputpanel(wxWindow *p_parent);
    };
#endif // MCRL2XI_OUTPUTPANEL_H
