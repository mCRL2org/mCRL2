#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "xsim.h"
#endif

# include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <cstring>
#include <iostream>
#include <wx/cmdline.h>
#include <wx/msgdlg.h>
#include <wx/string.h>
#include <aterm2.h>
#include "xsimmain.h"
#include "libstruct.h"
#include "librewrite.h"
#include "print/messaging.h"

/* The optional input file that should contain an LPS */
std::string lps_file_argument;
 
// Squadt protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <mcrl2/utilities/squadt_interface.h>

class squadt_interactor: public mcrl2::utilities::squadt::tool_interface {
  
  private:

    // Identifier for main input file that contains an LTS
    static const char* lps_file_for_input;
 
    // Wrapper for wxEntry invocation
    mcrl2::utilities::squadt::entry_wrapper& starter;

  public:

    // Constructor
    squadt_interactor(mcrl2::utilities::squadt::entry_wrapper&);

    // Configures tool capabilities.
    void set_capabilities(tipi::tool::capabilities&) const;

    // Queries the user via SQuADt if needed to obtain configuration information
    void user_interactive_configuration(tipi::configuration&);

    // Check an existing configuration object to see if it is usable
    bool check_configuration(tipi::configuration const&) const;

    // Performs the task specified by a configuration
    bool perform_task(tipi::configuration&);
};

const char* squadt_interactor::lps_file_for_input = "lps_in";

squadt_interactor::squadt_interactor(mcrl2::utilities::squadt::entry_wrapper& w): starter(w) {
}

void squadt_interactor::set_capabilities(tipi::tool::capabilities& c) const {
  /* The tool has only one main input combination it takes an LPS and then behaves as a reporter */
  c.add_input_combination(lps_file_for_input, tipi::mime_type("lps", tipi::mime_type::application), tipi::tool::category::simulation);
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& c) {
}

bool squadt_interactor::check_configuration(tipi::configuration const& c) const {
  bool valid = c.input_exists(lps_file_for_input);

  if (!valid) {
    send_error("Invalid input combination!");
  }

  return valid;
}

bool squadt_interactor::perform_task(tipi::configuration& c) {
  lps_file_argument = c.get_input(lps_file_for_input).get_location();

  return starter.perform_entry();
}

std::auto_ptr < squadt_interactor > interactor;
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
                        bool& dummies, std::string& lps_file_argument) {

  using namespace ::mcrl2::utilities;

  wxCmdLineParser cmdln(argc,argv);

  cmdln.AddSwitch(wxT("h"),wxT("help"),wxT("displays this message"));
  cmdln.AddSwitch(wxT("q"),wxT("quiet"),wxT("do not display any unrequested information"));
  cmdln.AddSwitch(wxT("v"),wxT("verbose"),wxT("display concise intermediate messages"));
  cmdln.AddSwitch(wxT("d"),wxT("debug"),wxT("display detailed intermediate messages"));
  cmdln.AddSwitch(wxT("y"),wxT("dummy"),wxT("replace free variables in the LPS with dummy values"));
  cmdln.AddOption(wxT("R"),wxT("rewriter"),wxT("use specified rewriter (default 'inner')"));
  cmdln.AddParam(wxT("INFILE"),wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL);
  cmdln.SetLogo(wxT("Graphical simulator for mCRL2 LPSs."));

  if (cmdln.Parse()) {
    return false;
  }

  if (cmdln.Found(wxT("h"))) {
    std::cout << "Usage: " << PROGRAM_NAME << " [OPTION]... [INFILE]\n"
              << "Simulate LPSs in a graphical environment. If INFILE is supplied it will be\n"
              << "loaded into the simulator.\n"
              << "\n"
              << "Options:\n"
              << "  -h, --help               display this help message\n"
              << "  -q, --quiet              do not display any unrequested information\n"
              << "  -v, --verbose            display concise intermediate messages\n"
              << "  -d, --debug              display detailed intermediate messages\n"
              << "  -y, --dummy              replace free variables in the LPS with dummy values\n"
              << "  -RNAME, --rewriter=NAME  use rewriter NAME (default 'inner')\n";

    return false;
  }

  if (cmdln.Found(wxT("q")) && cmdln.Found(wxT("v"))) {
    gsErrorMsg("options -q/--quiet and -v/--verbose cannot be used together\n");
    return false;
  }
  if (cmdln.Found(wxT("q")) && cmdln.Found(wxT("d"))) {
    gsErrorMsg("options -q/--quiet and -d/--debug cannot be used together\n");
    return false;
  }
  if (cmdln.Found(wxT("q"))) {
    gsSetQuietMsg();
  }
  if (cmdln.Found(wxT("v"))) {
    gsSetVerboseMsg();
  }
  if (cmdln.Found(wxT("d"))) {
    gsSetDebugMsg();
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
    lps_file_argument = std::string(cmdln.GetParam(0).fn_str());
  }

  return (true);
}

static XSim *this_xsim = NULL;
void xsim_message_handler(mcrl2::utilities::messageType msg_type, const char *msg)
{
  using namespace ::mcrl2::utilities;

  if ( this_xsim == NULL )
  {
    fprintf(stderr,msg);
    fprintf(stderr,"this message was brought to you by XSim (all rights reserved)\n");
  } else {
    const char *msg_end = msg+std::strlen(msg)-1;
    while ( (msg <= msg_end) && ((*msg == '\r') || (*msg == '\n')) )
    {
      --msg_end;
    }
    wxString wx_msg(msg,wxConvLocal, msg_end - msg);
    switch (msg_type)
    {
      case gs_warning:
        {
        wxMessageDialog dlg(NULL,wx_msg,wxT("mCRL2 warning"),wxOK|wxICON_EXCLAMATION);
        dlg.ShowModal();
        }
        break;
      case gs_error:
        {
        wxMessageDialog dlg(NULL,wx_msg,wxT("mCRL2 error"),wxOK|wxICON_ERROR);
        dlg.ShowModal();
        }
        break;
      case gs_notice:
      default:
        {
        wxMessageDialog dlg(NULL,wx_msg,wxT("mCRL2 notice"),wxOK|wxICON_INFORMATION);
        dlg.ShowModal();
        }
        break;
    }
  }
}


bool XSim::OnInit()
{
  gsEnableConstructorFunctions();

  this_xsim = this;
  gsSetCustomMessageHandler(xsim_message_handler);

  /* Whether to replace free variables in the LPS with dummies */
  bool dummies = false;
 
  /* The rewrite strategy that will be used */
  RewriteStrategy rewrite_strategy = GS_REWR_INNER;
 
#ifdef ENABLE_SQUADT_CONNECTIVITY
  if (!interactor->is_active()) {
#endif
    if (!parse_command_line(argc, argv, rewrite_strategy, dummies, lps_file_argument)) {
      return (false);
    }
#ifdef ENABLE_SQUADT_CONNECTIVITY
  }
#endif
 
  XSimMain *frame = new XSimMain( 0, -1, wxT("XSim"), wxPoint(-1,-1), wxSize(500,400) );
  frame->use_dummies = dummies;
  frame->rewr_strat  = rewrite_strategy;
  frame->Show(true);
   
  if (!lps_file_argument.empty()) {
    frame->LoadFile(wxString(lps_file_argument.c_str(), wxConvLocal));
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
                                  int nCmdShow) {                                                                     
  ATerm bot;

  ATinit(0,0,&bot); // XXX args?

#ifdef ENABLE_SQUADT_CONNECTIVITY
  mcrl2::utilities::squadt::entry_wrapper starter(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

  interactor.reset(new squadt_interactor(starter));

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
  mcrl2::utilities::squadt::entry_wrapper starter(argc, argv);

  interactor.reset(new squadt_interactor(starter));

  if(!interactor->try_interaction(argc, argv)) {
#endif
    return wxEntry(argc, argv);
#ifdef ENABLE_SQUADT_CONNECTIVITY
  }
#endif

  return 0;
}
#endif
