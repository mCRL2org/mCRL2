#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <squadt_utility.h>
/* The communicator, for communication with SQuADT */
sip::tool::communicator tc;

/* Used to communicate wetehr a connection with SQuADT was established */
bool connection_established = false;

/* Constants for identifiers of options and objects */
enum identifiers {
    lts_file_for_input // Main input file that contains an lts
};

#endif

#include "liblts.h"

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
   
#ifdef ENABLE_SQUADT_CONNECTIVITY
/* Extracts a configuration from a message, and validates its content */
bool try_to_accept_configuration(sip::tool::communicator& tc, sip::messenger::message_ptr const& m) {
  sip::configuration::sptr configuration = tc << m;

  if (configuration.get() == 0) {
    return (false);
  }
  if (configuration->object_exists(lts_file_for_input)) {
    /* The input object is present, verify whether the specified format is supported */
    sip::object::sptr input_object = configuration->get_object(lts_file_for_input);

    lts_type t = lts::parse_format(input_object->get_format().c_str());

    if (t == lts_none) {
      tc.send_status_report(sip::report::error, boost::str(boost::format("Invalid configuration: unsupported type `%s' for main input") % lts::string_for_type(t)));

      return (false);
    }
    if (!boost::filesystem::exists(boost::filesystem::path(input_object->get_location()))) {
      tc.send_status_report(sip::report::error, std::string("Invalid configuration: input object does not exist"));

      return (false);
    }
  }
  else {
    return (false);
  }

  tc.send_accept_configuration();

  return (true);
}
#endif

class GraphApp : public wxApp
{
  public:
    bool OnInit() {

    std::string lts_file_argument;

    gsEnableConstructorFunctions();

#ifdef ENABLE_SQUADT_CONNECTIVITY
  if (connection_established) {
    bool valid_configuration_present = false;
    bool termination_requested       = false;

    /* Initialise utility pseudo-library */
    squadt_utility::initialise(tc);

    /* Main event loop for incoming messages from squadt */
    for (sip::message_ptr m = tc.await_message(sip::message_any); !termination_requested; m = tc.await_message(sip::message_any)) {
      assert(m.get() != 0);

      switch (m->get_type()) {
        case sip::message_offer_configuration:

          /* Insert configuration in tool communicator object */
          valid_configuration_present = try_to_accept_configuration(tc, m);

          break;
        case sip::message_signal_start:
          if (valid_configuration_present) {

            sip::object::sptr input_object = tc.get_configuration().get_object(lts_file_for_input);

	    //TODO (CT): Adapt this in such a way that a file kan be read by wxEntry.
           lts_file_argument = input_object->get_location(); 
           tc.send_signal_done();
            
          }
          else {
            /* Send error report */
            tc.send_status_report(sip::report::error, "Start signal received without valid configuration.");
          }
          break;
        case sip::message_request_termination:
          termination_requested = true;
          break;
        default:
          /* Messages with a type that do not need to be handled */
          break;
      }
    }
  }
  else {
#endif

    wxCmdLineParser cmdln(argc,argv);
    sip::configuration::sptr configuration;
	   
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

    if ( cmdln.GetParamCount() > 0 ) {
      lts_file_argument = std::string(cmdln.GetParam(0).fn_str());
    }

#ifdef ENABLE_SQUADT_CONNECTIVITY
  }
#endif
    
    GraphFrame *frame;
	
    frame = new GraphFrame(wxT("ltsgraph"), wxPoint(150, 150), wxSize(INITIAL_WIN_WIDTH, INITIAL_WIN_HEIGHT));

    frame->Show(true);
    frame->GetSizer()->RecalcSizes();

    if (!lts_file_argument.empty()) {
      frame->Init(wxString(lts_file_argument.c_str(), wxConvLocal));
    }

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
        return wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);    
    }
#else
int main(int argc, char **argv)
{
  ATerm bot;
  ATinit(argc,argv,&bot);
#ifdef ENABLE_SQUADT_CONNECTIVITY

  /* Get tool capabilities object associated to the communicator in order to modify settings */
  sip::tool::capabilities& cp = tc.get_tool_capabilities();

  /* The tool operates on LTSes (stored in some different formats) and its function can be characterised as visualising */
  cp.add_input_combination(lts_file_for_input, "Visualising", "aut");
  cp.add_input_combination(lts_file_for_input, "Visualising", "svc");

  connection_established = tc.activate(argc,argv);
#endif
  /* On purpose we do not catch exceptions */

  return wxEntry(argc, argv);

}
#endif





