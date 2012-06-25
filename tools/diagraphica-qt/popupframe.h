// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./popupframe.h

#ifndef POPUPFRAME_H
#define POPUPFRAME_H

#include <QtCore>
#include <QtGui>

#include <wx/wx.h>
#include "colleague.h"
#include "mediator.h"

class PopupFrame : public wxFrame, public Colleague
{
  public:
    // -- constructors and desctructor ------------------------------
    PopupFrame(
      Mediator* m,
      wxWindow* parent,
      wxWindowID id,
      wxString title,
      wxPoint position,
      wxSize size);
    virtual ~PopupFrame();

  protected:
    // -- event handlers --------------------------------------------
    virtual void OnClose(wxCloseEvent& event);

  private:
    // -- declare event table ---------------------------------------
    DECLARE_EVENT_TABLE()
};

#endif

// -- end -----------------------------------------------------------
