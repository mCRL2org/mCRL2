/////////////////////////////////////////////////////////////////////////////
// Name:        mcrl2-gui.cpp
// Purpose:     prototype
// Author:      Frank Stappers
/////////////////////////////////////////////////////////////////////////////

#include "mcrl2-gui.h"
#include "mcrl2-process.h"
#include "initialization.h"
#include "gui/mainframe.h"

// TODO: Build-in test to detect whether a tool actually exists from tool catalog


// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

class mCRL2_gui: public wxApp {
public:
	virtual bool OnInit();

	Initialization* m_initialization_result;
};
IMPLEMENT_APP(mCRL2_gui)

// `Main program' equivalent: the program execution "starts" here
bool mCRL2_gui::OnInit() {
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
