// Author(s): Didier Le Lann, Carst Tankink, Muck van Weerdenburg and Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./ltsgraph.cpp

#include "wx.hpp" // precompiled headers

#define NAME "ltsgraph"
#define AUTHOR "Didier Le Lann, Carst Tankink, Muck van Weerdenburg and Jeroen van der Wulp"

// Graphical shell
#include "graph_frame.h"

#include <mcrl2/lts/lts.h>
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/utilities/command_line_interface.h"

std::string get_about_message() {
  static const std::string version_information =
        mcrl2::utilities::interface_description("", NAME, AUTHOR, "", "").
                                                        version_information() +
           std::string("\n"
           "This tool is part of the mCRL2 toolset.\n"
           "For information see http://www.mcrl2.org\n"
           "\n"
           "For feature requests or bug reports,\n"
           "please visit http://www.mcrl2.org/issuetracker");

  return version_information;
}

std::string lts_file_argument;

#ifdef ENABLE_SQUADT_CONNECTIVITY
// SQuADT protocol interface
# include <mcrl2/utilities/mcrl2_squadt_interface.h>

using namespace mcrl2::utilities::squadt;

const char* lts_file_for_input  = "lts_in";

class squadt_interactor: public mcrl2::utilities::squadt::mcrl2_wx_tool_interface {

  public:
    // Configures tool capabilities.
    void set_capabilities(tipi::tool::capabilities& c) const {
      c.add_input_configuration(lts_file_for_input,
                 tipi::mime_type("aut", tipi::mime_type::text), tipi::tool::category::visualisation);
      c.add_input_configuration(lts_file_for_input,
                 tipi::mime_type("svc", tipi::mime_type::application), tipi::tool::category::visualisation);
      c.add_input_configuration(lts_file_for_input,
                 tipi::mime_type("svc+mcrl", tipi::mime_type::application), tipi::tool::category::visualisation);
      c.add_input_configuration(lts_file_for_input,
                 tipi::mime_type("mcrl2-lts", tipi::mime_type::application), tipi::tool::category::visualisation);
#ifdef USE_BCG
      c.add_input_configuration(lts_file_for_input,
                 tipi::mime_type("bcg", tipi::mime_type::application), tipi::tool::category::visualisation);
#endif
    }

    // Queries the user via SQuADt if needed to obtain configuration information
    void user_interactive_configuration(tipi::configuration&) { }

    // Check an existing configuration object to see if it is usable
    bool check_configuration(tipi::configuration const& c) const {
      if (c.input_exists(lts_file_for_input)) {
        /* The input object is present, verify whether the specified format is supported */
        if (mcrl2::lts::lts::parse_format(c.get_input(lts_file_for_input).type().sub_type().c_str()) == mcrl2::lts::lts_none) {
          send_error("Invalid configuration: unsupported type `" +
              c.get_input(lts_file_for_input).type().sub_type() + "' for main input");
        }
        else {
          return true;
        }
      }

      return false;
    }

    bool perform_task(tipi::configuration& c) {
      lts_file_argument = c.get_input(lts_file_for_input).location();

      return mcrl2_wx_tool_interface::perform_task(c);
    }
};
#endif

void parse_command_line(int argc, wxChar** argv) {

  using namespace mcrl2::utilities;

  interface_description clinterface(std::string(wxString(argv[0], wxConvLocal).fn_str()),
        NAME, AUTHOR, "[OPTION]... [INFILE]\n",
    "Draw graphs and optimize their layout in a graphical environment. "
    "If INFILE (LTS file: *.aut or *.svc) is supplied, the tool will use this file as input for drawing.");

  command_line_parser parser(clinterface, argc, argv);

  if (0 < parser.arguments.size()) {
    lts_file_argument = parser.arguments[0];
  }
  if (1 < parser.arguments.size()) {
    parser.error("too many file arguments");
  }
}

class GraphApp : public wxApp
{
  private:

    std::string parse_error;

  public:
    bool OnInit() {
      init_frame(lts_file_argument);

      return true;
    }

    bool Initialize(int& argc, wxChar** argv) {
      try {
        parse_command_line(argc, argv);
      }
      catch (std::exception& e) {
        if (wxApp::Initialize(argc, argv)) {
          parse_error = std::string(e.what()).
            append("\n\nNote that other command line options may have been ignored because of this error.");
        }
        else {
          std::cerr << e.what() << std::endl;

          return false;
        }

        return true;
      }

      return wxApp::Initialize(argc, argv);
    }


    int OnExit() {
      return (wxApp::OnExit());
    }

  private:	
    void init_frame(std::string lts_file_argument)
    {
      GraphFrame *frame;
	
      wxSize maximum_size = wxGetClientDisplayRect().GetSize();

      frame = new GraphFrame(wxT(NAME), wxPoint(150, 150),
                     wxSize((std::min)(maximum_size.GetWidth(),INITIAL_WIN_WIDTH), (std::min)(maximum_size.GetHeight(),INITIAL_WIN_HEIGHT)));
      frame->Show(true);

      frame->GetSizer()->RecalcSizes();

      if(!parse_error.empty())
      {
        wxMessageDialog msg_dlg(frame, wxString(parse_error.c_str(), wxConvLocal),
                                wxT("Command line error"), wxOK | wxICON_ERROR);
        msg_dlg.ShowModal();
      }
      else if (!lts_file_argument.empty()) {
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

  MCRL2_ATERM_INIT(0, lpCmdLine)

#ifdef ENABLE_SQUADT_CONNECTIVITY
  if(interactor< squadt_interactor >::free_activation(hInstance, hPrevInstance, lpCmdLine, nCmdShow)) {
    return EXIT_SUCCESS;
  }
#endif

  return wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}
#else
int main(int argc, char **argv)
{
  MCRL2_ATERM_INIT(argc, argv)

#ifdef ENABLE_SQUADT_CONNECTIVITY
  if(interactor< squadt_interactor >::free_activation(argc, argv)) {
    return EXIT_SUCCESS;
  }
#endif

  return wxEntry(argc, argv);
}
#endif
