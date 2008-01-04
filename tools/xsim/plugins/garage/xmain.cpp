// Author(s): Aad Mathijssen and Hannes Pretorius
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file xmain.cpp

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
  int argc;
  MCRL2_ATERM_INIT()

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
