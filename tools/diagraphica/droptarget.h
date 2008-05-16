// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// LICENSE_1_0.txt)
//
/// \file ./droptarget.h

// --- droptarget ---------------------------------------------------
// (c) 2007  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------

#ifndef DROPTARGET_H
#define DROPTARGET_H

#include <cstddef>
#include <string>
#include <vector>
using namespace std;
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
        Mediator* m );
	virtual ~DropTarget();
	
    // -- overridden functions from wxTextDropTarget ----------------
    virtual bool OnDropText(
        wxCoord x,
        wxCoord y,
        const wxString &text );

protected:
    // -- data members ----------------------------------------------
    wxWindow* owner; // association
};

#endif

// -- end -----------------------------------------------------------
