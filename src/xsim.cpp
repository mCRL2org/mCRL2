#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "xsim.h"
#endif

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <iostream>
#include <wx/cmdline.h>
#include <aterm2.h>
#include "xsim.h"
#include "xsimmain.h"
#include "libstruct.h"
#include "librewrite_c.h"

#define PROGRAM_NAME "xsim"

//------------------------------------------------------------------------------
// XSim
//------------------------------------------------------------------------------

void print_help() {
  cout << "Usage: " << PROGRAM_NAME << " [OPTION]... [INFILE]\n"
       << "Simulate LPEs in a graphical environment. If INFILE is supplied it will be\n"
       << "loaded into the simulator.\n"
       << "\n"
       << "Mandatory arguments to long options are mandatory for short options too.\n"
       << "  -h, --help            display this help message\n"
       << "  -y, --dummy           replace free variables in the LPE with dummy values\n"
       << "  -R, --rewriter=NAME   use rewriter NAME (default 'inner')\n";
}

XSim::XSim()
{
}

bool XSim::OnInit()
{
    bool     dummies = false;
    wxString strategy;

    gsEnableConstructorFunctions();

    wxCmdLineParser cmdln(argc,argv);
    cmdln.AddSwitch(wxT("h"),wxT("help"),wxT("displays this message"));
    cmdln.AddSwitch(wxT("y"),wxT("dummy"),wxT("replace free variables in the LPE with dummy values"));
    cmdln.AddOption(wxT("R"),wxT("rewriter"),wxT("use specified rewriter (default 'inner')"));
    cmdln.AddParam(wxT("INFILE"),wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL);
    cmdln.SetLogo(wxT("Graphical simulator for mCRL2 LPEs."));

    if ( cmdln.Parse() )
    {
	    return FALSE;
    }

    if ( cmdln.Found(wxT("h")) )
    {
            print_help();
	    return FALSE;
    }

    RewriteStrategy strat = GS_REWR_INNER;

    if ( cmdln.Found(wxT("y")) )
    {
	    dummies = true;
    }

    if ( cmdln.Found(wxT("R"),&strategy) )
    {
	    strat = RewriteStrategyFromString(strategy.mb_str());
	    if ( strat == GS_REWR_INVALID )
	    {
		    cerr << "error: invalid rewrite strategy '" << strategy << "'" << endl;;
		    return FALSE;
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
