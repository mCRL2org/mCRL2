// ---------------------------------
// xmain.h
// (c) 2005
// A.J. Pretorius
// 
// Dept of Math and Computer Science 
// Technische Universitiet Eindhoven
// a.j.pretorius@tue.nl
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
