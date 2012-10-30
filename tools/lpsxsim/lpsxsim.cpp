// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsxsim.cpp

#include "wx.hpp" // precompiled headers

#define NAME "lpsxsim"
#define AUTHOR "Muck van Weerdenburg"

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA) && !defined(__clang__)
#pragma implementation "lpsxsim.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include <cstring>
#include <iostream>
#include <wx/msgdlg.h>
#include <wx/string.h>
#include "lpsxsimmain.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/utilities/wx_tool.h"
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"

using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

void xsim_message_handler(mcrl2::log::function_pointer_output<mcrl2::log::formatter>::message_t msg_type, const char* msg);

//------------------------------------------------------------------------------
// XSim
//------------------------------------------------------------------------------
class XSim: public wx::tool< XSim, rewriter_tool< input_tool > >
{
    typedef wx::tool< XSim, rewriter_tool< input_tool > > super;

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

    XSim() : super("LPSXSim",
                     "graphical simulation of an LPS", // what-is
                     "Simulator for linear process specifications.", // GUI specific description
                     "Simulate LPSs in a graphical environment. If INFILE is supplied it will be "
                     "loaded into the simulator.", // description
                     std::vector< std::string >(1, "Muck van Weerdenburg"))
    { }

    // Graphical subsystem needs to be initialised first, run() is the entry point
    bool run()
    {
      XSimMain* frame = new XSimMain(0, -1, wxT("LPSXSim"), wxPoint(-1,-1), wxSize(500,400));
      frame->simulator->use_dummies = dummies;
      frame->simulator->rewr_strat  = rewrite_strategy();
      frame->Show(true);

      if (!this->m_input_filename.empty())
      {
        frame->LoadFile(wxString(this->m_input_filename.c_str(), wxConvLocal));
      }

      instance = this;

      return true;
    }

};

XSim* XSim::instance = NULL;

void xsim_message_handler(mcrl2::log::function_pointer_output<mcrl2::log::formatter>::message_t msg_type, const char* msg)
{
  using namespace ::mcrl2::utilities;
  using namespace mcrl2::core;

  if (XSim::instance == NULL)
  {
    mCRL2log(mcrl2::log::error) << msg << "this message was brought to you by LPSXSim (all rights reserved)" << std::endl;
  }
  else
  {
    const char* msg_end = msg+std::strlen(msg)-1;
    while ((msg <= msg_end) && ((*msg == '\r') || (*msg == '\n')))
    {
      --msg_end;
    }
    wxString wx_msg(msg,wxConvLocal, msg_end - msg);
    switch (msg_type)
    {
      case mcrl2::log::function_pointer_output<mcrl2::log::formatter>::msg_warning:
      {
        wxMessageDialog(NULL,wx_msg,wxT("mCRL2 warning"),wxOK|wxICON_EXCLAMATION).ShowModal();
      }
      break;
      case mcrl2::log::function_pointer_output<mcrl2::log::formatter>::msg_error:
      {
        wxMessageDialog(NULL,wx_msg,wxT("mCRL2 error"),wxOK|wxICON_ERROR).ShowModal();
      }
      break;
      case mcrl2::log::function_pointer_output<mcrl2::log::formatter>::msg_notice:
      default:
      {
        wxMessageDialog(NULL,wx_msg,wxT("mCRL2 notice"),wxOK|wxICON_INFORMATION).ShowModal();
      }
      break;
    }
  }
}

class XSim_gui_tool: public mcrl2::utilities::mcrl2_gui_tool<XSim>
{
  public:
    XSim_gui_tool()
    {
      //m_gui_options["no-state"] = create_checkbox_widget();
    }
};


IMPLEMENT_APP_NO_MAIN(XSim_gui_tool)
IMPLEMENT_WX_THEME_SUPPORT

#ifdef __WINDOWS__
extern "C" int WINAPI WinMain(HINSTANCE hInstance,
                              HINSTANCE hPrevInstance,
                              wxCmdLineArgType lpCmdLine,
                              int nCmdShow)
{

  MCRL2_ATERMPP_INIT(0, lpCmdLine)

  return wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}
#else
int main(int argc, char** argv)
{
  return wxEntry(argc, argv);
}
#endif
