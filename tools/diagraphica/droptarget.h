// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./droptarget.h

#ifndef DROPTARGET_H
#define DROPTARGET_H

#include <cstddef>
#include <wx/dnd.h>
#include <wx/listctrl.h>
#include <wx/tokenzr.h>
#include "colleague.h"
#include "utils.h"

class DropTarget : public Colleague, public wxTextDropTarget
{
  public:
    // -- constructors and destructor -------------------------------
    DropTarget(
      wxWindow* ownr,
      Mediator* m);
    virtual ~DropTarget();

    // -- overridden functions from wxTextDropTarget ----------------
    virtual bool OnDropText(
      wxCoord x,
      wxCoord y,
      const wxString& text);

  protected:
    // -- data members ----------------------------------------------
    wxWindow* owner; // association
};

#endif

// -- end -----------------------------------------------------------
