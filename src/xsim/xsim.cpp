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
#include "xsimmain.h"
#include "libstruct.h"
#include "librewrite.h"

/* The optional input file that should contain an LPD */
std::string lpd_file_argument;
 
// Squadt protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <utility/squadt_utility.h>

class squadt_interactor: public squadt_tool_interface {
  
  private:

    // Identifier for main input file that contains an LTS
    static const char* lpd_file_for_input;
 
    // Wrapper for wxEntry invocation
    squadt_utility::entry_wrapper& starter;

  public:

    // Constructor
    squadt_interactor(squadt_utility::entry_wrapper&);

    // Configures tool capabilities.
    void set_capabilities(sip::tool::capabilities&) const;

    // Queries the user via SQuADt if needed to obtain configuration information
    void user_interactive_configuration(sip::configuration&);

    // Check an existing configuration object to see if it is usable
    bool check_configuration(sip::configuration const&) const;

    // Performs the task specified by a configuration
    bool perform_task(sip::configuration&);
};

const char* squadt_interactor::lpd_file_for_input = "lpd_in";

squadt_interactor::squadt_interactor(squadt_utility::entry_wrapper& w): starter(w) {
}

void squadt_interactor::set_capabilities(sip::tool::capabilities& c) const {
  /* The tool has only one main input combination it takes an LPE and then behaves as a reporter */
  c.add_input_combination(lpd_file_for_input, sip::mime_type("lpe"), sip::tool::category::simulation);
}

void squadt_interactor::user_interactive_configuration(sip::configuration& c) {
}

bool squadt_interactor::check_configuration(sip::configuration const& c) const {
  bool valid = c.input_exists(lpd_file_for_input);

  if (!valid) {
    send_error("Invalid input combination!");
  }

  return valid;
}

bool squadt_interactor::perform_task(sip::configuration& c) {
  lpd_file_argument = c.get_input(lpd_file_for_input).get_location();

  return starter.perform_entry();
}

squadt_interactor* interactor;
#endif

#define PROGRAM_NAME "xsim"

//------------------------------------------------------------------------------
// XSim
//------------------------------------------------------------------------------
class XSim: public wxApp
{
public:
    virtual bool OnInit();
    virtual int OnExit();
};

bool parse_command_line(int argc, wxChar** argv, RewriteStrategy& rewrite_strategy,
                        bool& dummies, std::string& lpd_file_argument) {
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
    std::cout << "Usage: " << PROGRAM_NAME << " [OPTION]... [INFILE]\n"
              << "Simulate LPEs in a graphical environment. If INFILE is supplied it will be\n"
              << "loaded into the simulator.\n"
              << "\n"
              << "Mandatory arguments to long options are mandatory for short options too.\n"
              << "  -h, --help            display this help message\n"
              << "  -y, --dummy           replace free variables in the LPE with dummy values\n"
              << "  -R, --rewriter=NAME   use rewriter NAME (default 'inner')\n";

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

  return (true);
}

bool XSim::OnInit()
{
  gsEnableConstructorFunctions();

#ifdef ENABLE_SQUADT_CONNECTIVITY
  if (interactor->is_active()) {
    XSimMain *frame = new XSimMain( 0, -1, wxT("XSim"), wxPoint(-1,-1), wxSize(500,400) );
    frame->Show(true);
    frame->LoadFile(wxString(lpd_file_argument.c_str(), wxConvLocal));
  }
  else {
#endif
    /* Whether to replace free variables in the LPE with dummies */
    bool dummies = false;
 
    /* The rewrite strategy that will be used */
    RewriteStrategy rewrite_strategy = GS_REWR_INNER;
 
    if (!parse_command_line(argc, argv, rewrite_strategy, dummies, lpd_file_argument)) {
      return (false);
    }
 
    XSimMain *frame = new XSimMain( 0, -1, wxT("XSim"), wxPoint(-1,-1), wxSize(500,400) );
    frame->use_dummies = dummies;
    frame->rewr_strat  = rewrite_strategy;
    frame->Show(true);
     
    if (!lpd_file_argument.empty()) {
      frame->LoadFile(wxString(lpd_file_argument.c_str(), wxConvLocal));
    }
#ifdef ENABLE_SQUADT_CONNECTIVITY
  }
#endif

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
                                  int nCmdShow) {                                                                     
  ATerm bot;

  ATinit(0,0,&bot); // XXX args?

#ifdef ENABLE_SQUADT_CONNECTIVITY
  squadt_utility::entry_wrapper starter(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

  interactor = new squadt_interactor(starter);

  if (!interactor->try_interaction(lpCmdLine)) {
#endif
    return wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
#ifdef ENABLE_SQUADT_CONNECTIVITY
  }
#endif

  return (0);
}
#else
int main(int argc, char **argv) {
  ATerm bot;

  /* Initialise aterm library */
  ATinit(argc,argv,&bot);

#ifdef ENABLE_SQUADT_CONNECTIVITY
  squadt_utility::entry_wrapper starter(argc, argv);

  interactor = new squadt_interactor(starter);

  if(!interactor->try_interaction(argc, argv)) {
#endif
    return wxEntry(argc, argv);
#ifdef ENABLE_SQUADT_CONNECTIVITY
  }
#endif

  return 0;
}
#endif
