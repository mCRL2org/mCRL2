// Squadt protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
  #include <squadt_utility.h>
#endif

// Graphical shell
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
    sip::configuration::sptr configuration;
    bool valid_aut = false;
    bool valid_svc = false;
    std::string file_name = "";
	   
    #ifdef ENABLE_SQUADT_CONNECTIVITY
      sip::tool::communicator tc;

      /* Constants for identifier of options and objects */
      const unsigned int aut_file_for_input = 0;
      const unsigned int svc_file_for_input = 1;

      /* Get tool capabilities in order to modify settings */
      sip::tool::capabilities& cp = tc.get_tool_capabilities();

      /* The tool has two main input combinations:
       * It takes an aut file and acts as a visualiser
       * It takes an svc file and acts as a visualiser
       */
      cp.add_input_combination(aut_file_for_input, "Visualising", "aut");
      cp.add_input_combination(svc_file_for_input, "Visualising", "svc");

      /* On purpose we do not catch exceptions */
      if (tc.activate(argc, argv)) {
        
        /* Initialise utility pseudo-library */
        squadt_utility::initialise(tc);
		
        /* Static configuration cycle */
        while (!(valid_aut || valid_svc)) { 
          /* Wait for configuration data to be sent (either a previous 
	   * configuration, or only an input combination
           */
          configuration = tc.await_configuration();
          /* Validate configuration specification, should contain a file name 
	   * of an AUT file or an SVC file */
          valid_aut = configuration.get() != 0;
          valid_svc = configuration.get() != 0;
          valid_aut &= configuration->object_exists(aut_file_for_input);
          valid_svc &= configuration->object_exists(svc_file_for_input);

          /* If the combination is invalid, report as such */
          if(!(valid_aut || valid_svc)) {
            tc.send_status_report(sip::report::error, "Invalid input combination");
          }
        }     
     
        /* Send the controller the signal that we are ready to rumble (no further
         * configuration necessary)
         */
        tc.send_accept_configuration();
        /* Wait for start message */
        tc.await_message(sip::message_signal_start);
      } 
    else {
#endif
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
#ifdef ENABLE_SQUADT_CONNECTIVITY
    }
#endif
    
    GraphFrame *frame;
	
    frame = new GraphFrame(wxT("ltsgraph"), wxPoint(150, 150), wxSize(INITIAL_WIN_WIDTH, INITIAL_WIN_HEIGHT));
    frame->Show(true);

    frame->GetSizer()->RecalcSizes();
#ifdef ENABLE_SQUADT_CONNECTIVITY
    /* If there is a valid svc or aut, retrieve its filename*/
    if (valid_aut) {
      file_name = configuration->get_object(aut_file_for_input)->get_location();
      frame->Init(file_name);
    }
      
    if (valid_svc) {
      file_name = configuration->get_object(svc_file_for_input)->get_location();
      frame->Init(file_name);
    }

    if (!(valid_aut || valid_svc)) { 
#endif
    if ( cmdln.GetParamCount() > 0 ) {
      frame->Init(cmdln.GetParam(0));	
    }
#ifdef ENABLE_SQUADT_CONNECTIVITY
    }
#endif
   

    return true;
  }

  int OnExit() {
    return (wxApp::OnExit());
  }

  private:
    std::string file_name;
	 
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






