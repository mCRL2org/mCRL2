#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "xsim.h"
#endif

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <wx/cmdline.h>
#include <aterm2.h>
#include "xsim.h"
#include "xsimmain.h"
#include "gsfunc.h"
#include "libgsrewrite.h"

//------------------------------------------------------------------------------
// XSim
//------------------------------------------------------------------------------


XSim::XSim()
{
}

bool XSim::OnInit()
{
    gsEnableConstructorFunctions();

    wxCmdLineParser cmdln(argc,argv);
    cmdln.AddSwitch("h","help","Displays this message");
    cmdln.AddSwitch("y","dummy","Replace free variables in the LPE with dummy values");
    cmdln.AddOption("R","rewriter","Use specified rewriter (default inner3)");
    cmdln.AddParam("LPE to simulate",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL);
    if ( cmdln.Parse() )
    {
	    return FALSE;
    }

    if ( cmdln.Found("h") )
    {
	    cmdln.Usage();
	    return FALSE;
    }
    bool dummies = false;
    RewriteStrategy strat = GS_REWR_INNER3;
    if ( cmdln.Found("y") )
    {
	    dummies = true;
    }
    wxString s;
    if ( cmdln.Found("R",&s) )
    {
	    if ( s == "inner" )
	    {
		    strat = GS_REWR_INNER;
	    } else if ( s == "inner2" )
	    {
		    strat = GS_REWR_INNER2;
	    } else if ( s == "inner3" )
	    {
		    strat = GS_REWR_INNER3;
	    } else if ( s == "innerc" )
	    {
		    strat = GS_REWR_INNERC;
	    } else if ( s == "innerc2" )
	    {
		    strat = GS_REWR_INNERC2;
	    } else if ( s == "jitty" )
	    {
		    strat = GS_REWR_JITTY;
	    } else {
		    fprintf(stderr,"warning: unknown rewriter '%s', using default\n",s.c_str());
		    strat = GS_REWR_INNER3;
	    }
    }

    XSimMain *frame = new XSimMain( NULL, -1, wxT("XSim"), wxPoint(-1,-1), wxSize(500,400) );
    frame->use_dummies = dummies;
    frame->rewr_strat = strat;
    frame->Show( TRUE );
   
    if ( cmdln.GetParamCount() > 0 )
    {
	frame->LoadFile(cmdln.GetParam(0));
    }

    return TRUE;
}

int XSim::OnExit()
{
    return 0;
}



IMPLEMENT_APP_NO_MAIN(XSim)
IMPLEMENT_WX_THEME_SUPPORT

#ifdef __WINDOWS__
extern "C" int WINAPI WinMain(HINSTANCE hInstance,                    
                                  HINSTANCE hPrevInstance,                
                                  wxCmdLineArgType lpCmdLine,             
                                  int nCmdShow)                           
    {                                                                     
        ATerm bot;

        ATinit(NULL,NULL,&bot); // XXX args?

        return wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);    
    }
#else
int main(int argc, char **argv)
{
	ATerm bot;

	ATinit(argc,argv,&bot);

	return wxEntry(argc, argv);
}
#endif
