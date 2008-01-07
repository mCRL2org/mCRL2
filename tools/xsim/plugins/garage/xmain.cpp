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

// --------------------------------------------------------
// This macro must be defined here for a subclass of wxApp. 
//		This creates a 'main()' method, amongst others.
// --------------------------------------------------------

IMPLEMENT_APP_NO_MAIN(Bremen)
IMPLEMENT_WX_THEME_SUPPORT

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#ifdef __WINDOWS__
extern "C" int WINAPI WinMain(HINSTANCE hInstance,                    
                                  HINSTANCE hPrevInstance,                
                                  wxCmdLineArgType lpCmdLine,             
                                  int nCmdShow) {                                                                     

  MCRL2_ATERM_INIT(&lpCmdLine)

  return wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}
#else
int main(int argc, char **argv) {

  MCRL2_ATERM_INIT(argv)

  return wxEntry(argc, argv);
}
#endif
