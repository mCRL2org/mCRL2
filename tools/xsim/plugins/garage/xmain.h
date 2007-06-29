// Author(s): Aad Mathijssen and Hannes Pretorius
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


// For compilers that support precompiled headers
#include <wx/wxprec.h>
// Else include 'wx/wx.h'
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif


#include "garageframe.h"


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
