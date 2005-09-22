#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "mcrl2studio.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "mcrl2studio.h"
#include "proj.h"
#include "aterm2.h"
#include "gsfunc.h"

//------------------------------------------------------------------------------
// Studio
//------------------------------------------------------------------------------


Studio::Studio()
{
}

bool Studio::OnInit()
{
    StudioProj *frame = new StudioProj( NULL, -1, wxT("mCRL2 Project"), wxPoint(-1,-1), wxSize(500,400) );

    frame->Show( TRUE );
    
    gsEnableConstructorFunctions();

    if ( argc > 1 )
    {
	frame->LoadFile(wxString(argv[1]));
    }

    return TRUE;
}

int Studio::OnExit()
{
    return 0;
}



IMPLEMENT_APP_NO_MAIN(Studio)
IMPLEMENT_WX_THEME_SUPPORT

int main(int argc, char **argv)
{
	ATerm bot;

	ATinit(argc,argv,&bot);

	return wxEntry(argc, argv);
}
