// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2xi.cpp


#include "boost.hpp" // precompiled headers

#define TOOLNAME "mcrl2xi"
#define AUTHORS "Frank Stappers"

#include "gui/mainframe.h"

#include "mcrl2/utilities/tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/wx_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"


#define NAME   "mcrl2-gui"
#define AUTHOR "Frank Stappers"

using namespace mcrl2::core;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;


class mCRL2xi: public mcrl2::utilities::wx::tool< mCRL2xi, mcrl2::utilities::tools::rewriter_tool<mcrl2::utilities::tools::input_tool> >
{
    typedef mcrl2::utilities::wx::tool< mCRL2xi, mcrl2::utilities::tools::rewriter_tool<mcrl2::utilities::tools::input_tool> > super;



  private:

    std::vector< std::string > developers()
    {

      return std::vector< std::string >(1, "Frank Stappers");
    }

    std::vector< std::string > documenters()
    {
      return std::vector< std::string >(1, "Frank Stappers");
    }

  public:
    mCRL2xi() : super("mcrl2xi",
                        "graphical mCRL2 data specification editor",
                        "A graphical mCRL2 data specification editor.",
                        "A graphical mCRL2 data specification editor.",
                        developers(),
                        "",
                        documenters())
    {
    }

    bool run()
    {
      // Create the main application window
      MainFrame* frame = new MainFrame(wxT("mCRL2xi"), wxDefaultPosition,
                                       wxSize(800, 600), m_rewrite_strategy);
      frame->Show(true);
      SetTopWindow(frame);

      if (!this->m_input_filename.empty())
      {
        frame->LoadFile(wxString(this->m_input_filename.c_str(), wxConvLocal));
      }

      return true;
    }
};

class mCRL2xi_gui_tool: public mcrl2::utilities::mcrl2_gui_tool<mCRL2xi>
{
  public:
    mCRL2xi_gui_tool()
    {
      //m_gui_options["no-state"] = create_checkbox_widget();
    }
};

IMPLEMENT_APP_NO_MAIN(mCRL2xi_gui_tool)
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

