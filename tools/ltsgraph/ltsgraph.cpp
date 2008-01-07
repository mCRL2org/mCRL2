//  Copyright 2007 Didier Le Lann, Carst Tankink, Muck van Weerdenburg and Jeroen van der Wulp. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./ltsgraph.cpp

#define NAME "ltsgraph"

// Graphical shell
#include "graph_frame.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <mcrl2/lts/liblts.h>
#include "mcrl2/utilities/version_info.h"
#include "mcrl2/core/core_init.h"

std::string lts_file_argument;

#ifdef ENABLE_SQUADT_CONNECTIVITY
// SQuADT protocol interface
# include <mcrl2/utilities/squadt_interface.h>

class squadt_interactor: public mcrl2::utilities::squadt::mcrl2_tool_interface {
  
  private:

    static const char*  lts_file_for_input;  ///< file containing an LTS that can be imported
 
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

const char* squadt_interactor::lts_file_for_input  = "lts_in";

squadt_interactor::squadt_interactor(mcrl2::utilities::squadt::entry_wrapper& w): starter(w) {
}

void squadt_interactor::set_capabilities(tipi::tool::capabilities& c) const {
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("aut", tipi::mime_type::text), tipi::tool::category::visualisation);
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("svc", tipi::mime_type::application), tipi::tool::category::visualisation);
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("svc+mcrl", tipi::mime_type::application), tipi::tool::category::visualisation);
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("svc+mcrl2", tipi::mime_type::application), tipi::tool::category::visualisation);
#ifdef MCRL2_BCG
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("bcg", tipi::mime_type::application), tipi::tool::category::visualisation);
#endif  
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& c) {
  //skip
}

bool squadt_interactor::check_configuration(tipi::configuration const& c) const {
  if (c.input_exists(lts_file_for_input)) {
    /* The input object is present, verify whether the specified format is supported */
    tipi::configuration::object input_object(c.get_input(lts_file_for_input));

    lts_file_argument = input_object.get_location();

    if (lts::parse_format(input_object.get_mime_type().get_sub_type().c_str()) == lts_none) {
      send_error(boost::str(boost::format("Invalid configuration: unsupported type `%s' for main input") % input_object.get_mime_type().get_sub_type().c_str()));
      return false;
    }
  }
  else {
    return false;
  }
  
  return true;
}

bool squadt_interactor::perform_task(tipi::configuration&) {
  return starter.perform_entry();
}
#endif

void print_help() {
  cout << "Usage: " << NAME << " [INFILE]\n"
       << "Draw graphs and optimize their layout in a graphical environment. If INFILE (LTS file : *.aut or *.svc) is supplied \n"
       << "the tool will use this file as input for drawing.\n"
       << "\n"
       << "Use left click to drag the nodes and right click to fix the nodes. \n"
       << "\n"
       << "Mandatory arguments to long options are mandatory for short options too.\n"
       << "  -h, --help            display this help message\n"
       << "  --version             displays version information and exits \n";
}

class GraphApp : public wxApp
{
  public:
    bool OnInit() {

      wxCmdLineParser cmdln(argc,argv);
         
      cmdln.AddSwitch(wxT("h"),wxT("help"),wxT("displays this message"));
      cmdln.AddSwitch(wxEmptyString,wxT("version"), wxT("displays version information and exits"));
      cmdln.AddParam(wxT("INFILE"),wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL);
      cmdln.SetLogo(wxT("Graphical tool for visualizing graph."));

      if ( cmdln.Parse()) {
        return false;
      }

      if ( cmdln.Found(wxT("h")) ) {
        print_help();
        return false;
      }

      if (cmdln.Found(wxT("version")) ) {
        print_version_information(NAME);
        return false;
      }

      if ( cmdln.GetParamCount() > 0 ) {
        lts_file_argument = std::string(cmdln.GetParam(0).fn_str());
      }

      init_frame(lts_file_argument);
      return true;
    }

    int OnExit() {
      return (wxApp::OnExit());
    }

  private:	
    void init_frame(std::string lts_file_argument) {
      GraphFrame *frame;
	
      wxSize maximum_size = wxGetClientDisplayRect().GetSize();

      frame = new GraphFrame(wxT(NAME), wxPoint(150, 150),
                     wxSize((std::min)(maximum_size.GetWidth(),INITIAL_WIN_WIDTH), (std::min)(maximum_size.GetHeight(),INITIAL_WIN_HEIGHT)));
      frame->Show(true);
      frame->GetSizer()->RecalcSizes();
      if (!lts_file_argument.empty()) {
        frame->Init(wxString(lts_file_argument.c_str(), wxConvLocal));
      }
    }
};


IMPLEMENT_APP_NO_MAIN(GraphApp)
IMPLEMENT_WX_THEME_SUPPORT

#ifdef __WINDOWS__
extern "C" int WINAPI WinMain(HINSTANCE hInstance,                    
                                  HINSTANCE hPrevInstance,                
                                  wxCmdLineArgType lpCmdLine,             
                                  int nCmdShow) {

  MCRL2_CORE_LIBRARY_INIT(&lpCmdLine)

#ifdef ENABLE_SQUADT_CONNECTIVITY
  using namespace mcrl2::utilities::squadt;

  if(!interactor< squadt_interactor >::free_activation(hInstance, hPrevInstance, lpCmdLine, nCmdShow)) {
#endif
      return wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);    
#ifdef ENABLE_SQUADT_CONNECTIVITY
    }

    return 0;
#endif
}
#else
int main(int argc, char **argv)
{
  MCRL2_CORE_LIBRARY_INIT(argv)

# ifdef ENABLE_SQUADT_CONNECTIVITY
  using namespace mcrl2::utilities::squadt;

  if(!interactor< squadt_interactor >::free_activation(argc, argv)) {
# endif
    return wxEntry(argc, argv);
# ifdef ENABLE_SQUADT_CONNECTIVITY
  }

  return 0;
# endif
}
#endif
