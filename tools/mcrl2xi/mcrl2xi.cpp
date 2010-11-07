// Author(s): Muck van Weerdenburg; adapted by Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2i.cpp


#include "boost.hpp" // precompiled headers

#define TOOLNAME "mcrl2xi"
#define AUTHORS "Frank Stappers"

#include "gui/mainframe.h"

#include "mcrl2/utilities/tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/wx_tool.h"
#include "mcrl2/atermpp/aterm_init.h"

#define NAME   "mcrl2-gui"
#define AUTHOR "Frank Stappers"

using namespace mcrl2::core;

class mCRL2xi: public mcrl2::utilities::wx::tool< mCRL2xi, mcrl2::utilities::tools::rewriter_tool<mcrl2::utilities::tools::input_tool> >
{
  typedef mcrl2::utilities::wx::tool< mCRL2xi, mcrl2::utilities::tools::rewriter_tool<mcrl2::utilities::tools::input_tool> > super;



  private:

    std::vector< std::string > developers() {

      return std::vector< std::string >(1, "Frank Stappers");
    }

    std::vector< std::string > documenters() {
      return std::vector< std::string >(1, "Frank Stappers");
    }

  public:
    mCRL2xi() : super("mcrl2xi",
                  "graphical mCRL2 data specification editor",
                  "A graphical mCRL2 data specification editor.",
                  "A graphical mCRL2 data specification editor.",
                  developers(),
                  "",
                  documenters()) {
    }

  bool run()
  {
  		// Create the main application window
  		MainFrame *frame = new MainFrame(wxT("mCRL2xi"), wxDefaultPosition,
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

#ifdef __WINDOWS__
extern "C" int WINAPI WinMain(HINSTANCE hInstance,
                                  HINSTANCE hPrevInstance,
                                  wxCmdLineArgType lpCmdLine,
                                  int nCmdShow) {
  return wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}
#endif

int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)
  return wxEntry(argc, argv);
}

IMPLEMENT_APP_NO_MAIN(mCRL2xi)

