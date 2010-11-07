// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file xstceditor.h

#ifndef MCRL2XI_XSTCEDITOR_H_
#define MCRL2XI_XSTCEDITOR_H_

#include <wx/stc/stc.h>

class xStcEditor: public wxStyledTextCtrl{
  public:
    xStcEditor(wxWindow *parent, wxWindowID id);
    void OnxStcUIUpdate(wxStyledTextEvent &/*event*/);
    void AutoComplete(wxStyledTextEvent &/*event*/);
    bool HasWord(wxString word, wxString &wordlist);

private:
DECLARE_EVENT_TABLE()
};

#endif
