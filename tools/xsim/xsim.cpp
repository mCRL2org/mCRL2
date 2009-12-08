// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file xsim.cpp

#include "wx.hpp" // precompiled headers

#define NAME "xsim"
#define AUTHOR "Muck van Weerdenburg"

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "xsim.h"
#endif

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <cstring>
#include <iostream>
#include <wx/msgdlg.h>
#include <wx/string.h>
#include <aterm2.h>
#include "xsimmain.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/utilities/wx_tool.h"
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/squadt_tool.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

void xsim_message_handler(mcrl2::core::messageType msg_type, const char *msg);

//------------------------------------------------------------------------------
// XSim
//------------------------------------------------------------------------------
class XSim: public wx::tool< XSim, squadt_tool< rewriter_tool< input_tool > > >
{
  typedef wx::tool< XSim, squadt_tool< rewriter_tool< input_tool > > > super;

private:
    bool dummies;

protected:
    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("dummy", "replace free variables in the LPS with dummy values", 'y');
    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      dummies = 0 < parser.options.count("dummy");
    }

public:

    static XSim* instance;

    XSim() : super("XSim",
      "graphical simulation of an LPS", // what-is
      "Simulator for linear process specifications.", // GUI specific description
      "Simulate LPSs in a graphical environment. If INFILE is supplied it will be "
      "loaded into the simulator.", // description
       std::vector< std::string >(1, "Muck van Weerdenburg"))
    { }

    // Graphical subsystem needs to be initialised first, DoInit starts the application instead of run()
    bool run()
    {
      XSimMain *frame = new XSimMain( 0, -1, wxT("XSim"), wxPoint(-1,-1), wxSize(500,400) );
      frame->simulator->use_dummies = dummies;
      frame->simulator->rewr_strat  = rewrite_strategy();
      frame->Show(true);
    
      if (!this->m_input_filename.empty()) 
      { try
        { frame->LoadFile(wxString(this->m_input_filename.c_str(), wxConvLocal));
        }
        catch(mcrl2::runtime_error e)
        { wxString mystring(e.what(), wxConvUTF8);
          wxMessageDialog msg(NULL, mystring ,wxT("Error"),wxOK|wxICON_ERROR); 
          msg.ShowModal();
          exit(EXIT_FAILURE);
        }
      }
    
      instance = this;

      return true;
    }

#ifdef ENABLE_SQUADT_CONNECTIVITY
    // Special initialisation
    void initialise() {
      gsSetCustomMessageHandler(xsim_message_handler);
    }

    // Configures tool capabilities.
    void set_capabilities(tipi::tool::capabilities& c) const {
      /* The tool has only one main input combination it takes an LPS and then behaves as a reporter */
      c.add_input_configuration("main-input",
           tipi::mime_type("lps", tipi::mime_type::application), tipi::tool::category::simulation);
    }

    // Queries the user via SQuADt if needed to obtain configuration information
    void user_interactive_configuration(tipi::configuration&) { }

    // Check an existing configuration object to see if it is usable
    bool check_configuration(tipi::configuration const& c) const {
      bool valid = c.input_exists("main-input");

      if (!valid) {
        send_error("Invalid input combination!");
      }

     return valid;
   }

   bool perform_task(tipi::configuration& c) {
     this->m_input_filename = c.get_input("main-input").location();

     return true;
   }
#endif
};

XSim* XSim::instance = NULL;

void xsim_message_handler(mcrl2::core::messageType msg_type, const char *msg)
{
  using namespace ::mcrl2::utilities;
  using namespace mcrl2::core;

  if ( XSim::instance == NULL )
  {
    std::cerr << msg << "this message was brought to you by XSim (all rights reserved)" << std::endl;
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
        wxMessageDialog(NULL,wx_msg,wxT("mCRL2 warning"),wxOK|wxICON_EXCLAMATION).ShowModal();
        }
        break;
      case gs_error:
        {
        wxMessageDialog(NULL,wx_msg,wxT("mCRL2 error"),wxOK|wxICON_ERROR).ShowModal();
        }
        break;
      case gs_notice:
      default:
        {
        wxMessageDialog(NULL,wx_msg,wxT("mCRL2 notice"),wxOK|wxICON_INFORMATION).ShowModal();
        }
        break;
    }
  }
}

IMPLEMENT_APP_NO_MAIN(XSim)
IMPLEMENT_WX_THEME_SUPPORT

#ifdef __WINDOWS__
extern "C" int WINAPI WinMain(HINSTANCE hInstance,
                                  HINSTANCE hPrevInstance,
                                  wxCmdLineArgType lpCmdLine,
                                  int nCmdShow) {

  MCRL2_ATERMPP_INIT(0, lpCmdLine)

  return wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}
#else
int main(int argc, char **argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return wxEntry(argc, argv);
}
#endif
