/////////////////////////////////////////////////////////////////////////////
// Name:        mcrl2-gui.cpp
// Purpose:     prototype
// Author:      Frank Stappers
/////////////////////////////////////////////////////////////////////////////

#include "mcrl2-gui.h"
#include "mcrl2-process.h"
#include "initialization.h"
#include "gui/mainframe.h"

#include "mcrl2/utilities/tool.h"
#include "mcrl2/utilities/wx_tool.h"

#define NAME   "mcrl2-gui"
#define AUTHOR "Frank Stappers"

using namespace mcrl2::core;

class mCRL2_gui: public mcrl2::utilities::wx::tool< mCRL2_gui, mcrl2::utilities::tools::tool >
{
  typedef mcrl2::utilities::wx::tool< mCRL2_gui, mcrl2::utilities::tools::tool > super;

  private:

  Initialization* m_initialization_result;

    std::vector< std::string > developers() {

      return std::vector< std::string >(1, "Frank Stappers");
    }

    std::vector< std::string > documenters() {
      return std::vector< std::string >(1, "Frank Stappers");
    }

  public:
    mCRL2_gui() : super("mcrl2-gui",
                  "graphical front-end for mCRL2 tools",
                  "A graphical front-end for mCRL2 tools.",
                  "A graphical front-end for mCRL2 tools",
                  developers(),
                  "",
                  documenters()) {
    }

  bool run()
  {
  		m_initialization_result = new Initialization();

  		vector< Tool > tool_catalog = m_initialization_result->m_tool_catalog;
  		std::multimap<std::string,std::string> extention_tool_mapping = m_initialization_result->m_extention_tool_mapping;

  		// Create the main application window
  		MainFrame *frame = new MainFrame(wxT("mCRL2-gui"), wxDefaultPosition,
  				wxSize(800, 600), tool_catalog, extention_tool_mapping
  		);
  		frame->Show(true);
  		SetTopWindow(frame);

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
  return wxEntry(argc, argv);
}

IMPLEMENT_APP_NO_MAIN(mCRL2_gui)

