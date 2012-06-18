// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./bitmappanel.h

#ifndef BITMAPPANEL_H
#define BITMAPPANEL_H

#include <wx/wx.h>
#include <wx/image.h>

class BitmapPanel : public wxPanel
{
  public:
    // -- constructors and desctructor ------------------------------
    BitmapPanel(
      wxWindow* parent,
      wxSize size,
      const char* const* image);
    virtual ~BitmapPanel();

  protected:
    // -- event handlers --------------------------------------------
    void onEvtPaint(wxPaintEvent& event);

  private:
    // -- data members ----------------------------------------------
    wxBitmap* bitmap;

    // -- declare event table ---------------------------------------
    DECLARE_EVENT_TABLE()
};

#endif

// -- end -----------------------------------------------------------
