// Author(s): Aad Mathijssen and Hannes Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file xmain.h

// ---------------------------------
//
// This class initializes the
//		Bremen parking garage
//		visualization module.
//
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


#ifndef MAIN_H
#define MAIN_H


#include <wx/wx.h>


#include "garageframe.h"
#include "mcrl2/core/aterm_ext.h"


class Bremen : public wxApp
{
public:
	virtual bool OnInit();
	virtual int OnExit();
private:
	GarageFrame* frame;
};

// This macro must be defined here for a subclass of wxApp.
DECLARE_APP( Bremen )

#endif // MAIN_H
