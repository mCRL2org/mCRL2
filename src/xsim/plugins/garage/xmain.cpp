// ---------------------------------
// xmain.cpp
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


#include "xmain.h"




// --------------------------------------------------------
// This macro must be defined here for a subclass of wxApp. 
//		This creates a 'main()' method, amongst others.
// --------------------------------------------------------

IMPLEMENT_APP( Bremen )

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^




// ------------------
bool Bremen::OnInit()
// ------------------
// This function is
//		used for 
//		initializa-
//		tion during
//		startup.
//
// ^^^^^^^^^^^^^^^^^^
{
  //initialise ATerm library
  ATerm StackBottom;
  ATinit(0, NULL, &StackBottom);
  //initiliase frame
  frame = new GarageFrame( wxT("Garage State"), 
  			 -1, 
  			 -1, 
  			 800, 
  			 600 );
  
  frame->Show( TRUE );
  SetTopWindow( frame );
  
  frame->Update();
  
  return true;
}




// -----------------
int Bremen::OnExit()
// -----------------
// This function is 
//		used for 
//		cleanup when
//		exiting the 
//		app.
//
// ^^^^^^^^^^^^^^^
{
  	return 0;
}
