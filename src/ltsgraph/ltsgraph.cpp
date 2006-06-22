#include "graph_frame.h"

void print_help() {
  cout << "Usage: ltsgraph [INFILE]\n"
       << "Draw graphs in a graphical environment. If INFILE (LTS file : *.aut or *.svc) is supplied \n"
       << "the tool will use this file as input for drawing.\n"
       << "\n"
	   	 << "Use left click to drag the nodes and right click to fix the nodes. \n"
	   	 << "\n"
       << "Mandatory arguments to long options are mandatory for short options too.\n"
       << "  -h, --help            display this help message\n";
}   

class GraphApp : public wxApp
{
public:
bool OnInit() {

    wxCmdLineParser cmdln(argc,argv);
    cmdln.AddSwitch(wxT("h"),wxT("help"),wxT("displays this message"));
    cmdln.AddParam(wxT("INFILE"),wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL);
    cmdln.SetLogo(wxT("Graphical tool for visualizing graph."));

    if ( cmdln.Parse() ) {
	    return false;
    }

    if ( cmdln.Found(wxT("h")) ) {
            print_help();
	    return false;
    }

		GraphFrame *frame;
	
		frame = new GraphFrame(wxT("ltsgraph"), wxPoint(150, 150), wxSize(INITIAL_WIN_WIDTH, INITIAL_WIN_HEIGHT));
		frame->Show(true);

    frame->GetSizer()->RecalcSizes();

    if ( cmdln.GetParamCount() > 0 )
			frame->Init(cmdln.GetParam(0));	

    return true;
}

int OnExit() {
  return (wxApp::OnExit());
}
};

IMPLEMENT_APP_NO_MAIN(GraphApp)
IMPLEMENT_WX_THEME_SUPPORT

#ifdef __WINDOWS__
extern "C" int WINAPI WinMain(HINSTANCE hInstance,                    
                                  HINSTANCE hPrevInstance,                
                                  wxCmdLineArgType lpCmdLine,             
                                  int nCmdShow)                           
    {                                                                     
        ATerm bot;

        ATinit(NULL,NULL,&bot); // XXX args?
		gsEnableConstructorFunctions();
        return wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);    
    }
#else
int main(int argc, char **argv)
{
	ATerm bot;

	ATinit(argc,argv,&bot);
	gsEnableConstructorFunctions();	
	return wxEntry(argc, argv);
}
#endif






