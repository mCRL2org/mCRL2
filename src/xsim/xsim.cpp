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

// Squadt protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <sip/tool.h>
#endif

#define PROGRAM_NAME "xsim"

//------------------------------------------------------------------------------
// XSim
//------------------------------------------------------------------------------

#ifdef ENABLE_SQUADT_CONNECTIVITY
/* Constants for identifiers of options and objects */
const unsigned int      lpd_file_for_input = 0;

/* The communicator, for communicating with Squadt */
sip::tool::communicator tc;

/* Used to communicate whether a connection with squadt was established */
bool                    connection_established = false;

#endif

void print_help() {
  std::cout << "Usage: " << PROGRAM_NAME << " [OPTION]... [INFILE]\n"
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
  /* Whether to replace free variables in the LPE with dummies */
  bool dummies = false;

  /* The rewrite strategy that will be used */
  RewriteStrategy rewrite_strategy = GS_REWR_INNER;

  /* The optional input file that should contain an LPD */
  std::string lpd_file_argument;

  gsEnableConstructorFunctions();

#ifdef ENABLE_SQUADT_CONNECTIVITY
  if (connection_established) {
    bool valid = false;

    /* Static configuration cycle */
    while (!valid) {
      /* Wait for configuration data to be send (either a previous configuration, or only an input combination) */
      sip::configuration::sptr configuration = tc.await_configuration();

      /* Validate configuration specification, should contain a file name of an LPD that is to be read as input */
      valid  = configuration.get() != 0;
      valid &= configuration->object_exists(lpd_file_for_input);

      if (valid) {
        /* An object with the correct id exists, assume the URI is relative (i.e. a file name in the local file system) */
        lpd_file_argument = configuration->get_object(lpd_file_for_input)->get_location();

        tc.set_configuration(configuration);
      }
      else {
        sip::report report;

        report.set_error("Invalid input combination!");

        tc.send_report(report);
      }
    }

    /* Send the controller the signal that we're ready to rumble (no further configuration necessary) */
    tc.send_accept_configuration();

    /* Wait for start message */
    tc.await_message(sip::send_signal_start);
  }
  else {
#endif
    wxCmdLineParser cmdln(argc,argv);
    cmdln.AddSwitch(wxT("h"),wxT("help"),wxT("displays this message"));
    cmdln.AddSwitch(wxT("y"),wxT("dummy"),wxT("replace free variables in the LPE with dummy values"));
    cmdln.AddOption(wxT("R"),wxT("rewriter"),wxT("use specified rewriter (default 'inner')"));
    cmdln.AddParam(wxT("INFILE"),wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL);
    cmdln.SetLogo(wxT("Graphical simulator for mCRL2 LPEs."));

    if (cmdln.Parse()) {
      return false;
    }

    if (cmdln.Found(wxT("h"))) {
      print_help();

      return false;
    }

    if (cmdln.Found(wxT("y"))) {
      dummies = true;
    }

    wxString strategy;

    if ( cmdln.Found(wxT("R"), &strategy) ) {
      rewrite_strategy = RewriteStrategyFromString(strategy.fn_str());

      if ( rewrite_strategy == GS_REWR_INVALID ) {
        std::cerr << "error: invalid rewrite strategy '" << strategy << "'" << std::endl;;

        return false;
      }
    }

    if ( cmdln.GetParamCount() > 0 ) {
      lpd_file_argument = std::string(cmdln.GetParam(0).fn_str());
    }
#ifdef ENABLE_SQUADT_CONNECTIVITY
  }
#endif

  XSimMain *frame = new XSimMain( NULL, -1, wxT("XSim"), wxPoint(-1,-1), wxSize(500,400) );

  frame->use_dummies = dummies;
  frame->rewr_strat  = rewrite_strategy;
  frame->Show(true);
   
  if (!lpd_file_argument.empty()) {
    frame->LoadFile(wxString(lpd_file_argument.c_str(), wxConvLocal));
  }

  return true;
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

#ifdef ENABLE_SQUADT_CONNECTIVITY
  /* Get tool capabilities in order to modify settings */
  sip::tool::capabilities& cp = tc.get_tool_capabilities();

  /* The tool has only one main input combination it takes an LPE and then behaves as a reporter */
  cp.add_input_combination(lpd_file_for_input, "Simulation", "lpe");

  connection_established = tc.activate(argc,argv);
#endif

  /* Initialise aterm library */
  ATinit(argc,argv,&bot);

  return wxEntry(argc, argv);
}
#endif
