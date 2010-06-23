/*
 * initialization.cpp
 *
 *  Created on: Jun 4, 2010
 *      Author: fstapper
 */

#include "initialization.h"
#include <iostream>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <stdexcept>

#include <sstream>
#include <algorithm>
#include <iterator>

#include "wx/image.h"
#include "wx/splash.h"
#include "tinyxml.h"
#include <wx/wx.h>

#include "mcrl2/utilities/basename.h"

#include "ticpp.h"

using namespace std;

Initialization::Initialization() {
	/* extension (first) is dealt with "formalism" (second) */
	m_extention_tool_mapping.insert(pair<string, string> ("mcrl2", "mcrl2"));
	m_extention_tool_mapping.insert(pair<string, string> ("lps", "lps"));

	m_extention_tool_mapping.insert(pair<string, string> ("lts", "lts"));
	m_extention_tool_mapping.insert(pair<string, string> ("fsm", "lts"));
	m_extention_tool_mapping.insert(pair<string, string> ("aut", "lts"));
	m_extention_tool_mapping.insert(pair<string, string> ("dot", "lts"));
	m_extention_tool_mapping.insert(pair<string, string> ("svc", "lts"));
	m_extention_tool_mapping.insert(pair<string, string> ("bcg", "lts"));
	m_extention_tool_mapping.insert(pair<string, string> ("pbes", "pbes"));
	m_extention_tool_mapping.insert(pair<string, string> ("txt", "txt"));
	m_extention_tool_mapping.insert(pair<string, string> ("chi", "chi"));
	m_extention_tool_mapping.insert(pair<string, string> ("lysa", "lysa"));
	m_extention_tool_mapping.insert(pair<string, string> ("pnml", "pnml"));
	m_extention_tool_mapping.insert(pair<string, string> ("tbf", "tbf"));
	m_extention_tool_mapping.insert(pair<string, string> ("gra", "gra"));

	mcrl2::utilities::basename basename;
	m_executable_basename = basename.get_executable_basename();

	m_toolset_basename = basename.get_toolset_basename();
	std::string file = m_toolset_basename + "/share/mcrl2/mcrl2.png";

	/* show splash screen*/
	wxInitAllImageHandlers();

	wxBitmap bitmap;
	wxString wxFile(file.c_str(), wxConvUTF8);
	if (bitmap.LoadFile(wxFile, wxBITMAP_TYPE_PNG)) {
		new wxSplashScreen(bitmap,
				wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT, 2000, NULL, -1,
				wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER
						| wxSTAY_ON_TOP);
	}
	wxYield();

	/* Read tools */
	std::vector<Tool> p_tools = Read_tools();

	/* for each tool */

	for (std::vector<Tool>::iterator i = p_tools.begin(); i != p_tools.end(); ++i) {
		string cmd = (*i).m_location + " --mcrl2-gui";

		wxArrayString tool_output;
		wxArrayString tool_errors;

		wxString wxCmd(cmd.c_str(), wxConvUTF8);
		wxExecute(wxCmd, tool_output, tool_errors, wxEXEC_SYNC);

		Tool c_tool = (*i);
		Tool_option to;
		vector<Tool_option> vto;

		bool first = true;
		for (size_t i = 0; i < tool_output.GetCount(); ++i) {
			string str = string(tool_output[i].mb_str(wxConvUTF8));

			/* New flag found */
			if (str.substr(0, 2).compare("--") == 0) {
				if (first) {
					first = false;
				} else {
					vto.push_back(to);
				}
				/* Create flag name */
				to.m_flag = str.substr(2, str.size());
				/* Reset fields */
				to.m_widget = none;
				to.m_values.clear();
				to.m_default_value = 0;
				to.m_help.clear();

			} else {

				bool is_widget = false;
				bool is_help = false;
				/* Widget detection */
				if (str.compare(">checkbox") == 0) {
					to.m_widget = checkbox;
					is_widget = true;
				}
				if (str.compare(">textbox") == 0) {
					to.m_widget = textbox;
					is_widget = true;
				}
				if (str.compare(">radiobox") == 0) {
					to.m_widget = radiobox;
					is_widget = true;
				}
				if (str.compare(">filepicker") == 0) {
					to.m_widget = filepicker;
					is_widget = true;
				}

				if (str[0] == ':') {
					is_widget = true;
				}

				if (str[0] == '?') {
					to.m_help.append(str.substr(1, str.size()));
					to.m_help.append("\n");
					is_help = true;
				}

				/* Parse values */
				if ((!is_widget) && (!is_help)) {
					if (str[0] == '*') {
						to.m_default_value = to.m_values.size();
						to.m_values.push_back(str.substr(1, str.size()));
					} else {
						to.m_values.push_back(str);
					}
				}
			}

		}

		/* Add tool option */
		vto.push_back(to);
		/* Add tool option vector too tool catalog */
		c_tool.m_tool_options = vto;
		/* Add options for tool */
		m_tool_catalog.push_back(c_tool);

	}


}

vector<Tool> Initialization::Read_tools() {

	std::vector<Tool> tools;
	std::string line;


	string tool_catalog_file = m_toolset_basename + "/share/mcrl2/tool_catalog.xml" ;
	ticpp::Document doc( tool_catalog_file );
	try{
		doc.LoadFile();
	}
	catch( ... ){
		wxString error = _T("Could not load tool catalog file :\n") +
				wxString(tool_catalog_file.c_str(), wxConvUTF8) +
				_T("\'.\n\nTool will now exit.");
				;

		   wxMessageDialog *dial = new wxMessageDialog(NULL,
				   error, wxT("Error"), wxOK | wxICON_ERROR);
		   dial->ShowModal();
	}

	ticpp::Element* node = 0;

	node = doc.FirstChildElement();

    if(!((node->Type() == TiXmlNode::ELEMENT) && node->Value() == "tool-catalog")){
      cerr << "Expected XML tree value \"tool-catalog\"" << endl;
    }

    for (ticpp::Element* e = node->FirstChildElement(false);
    		e != 0; e = e->NextSiblingElement(false)) {
    	Tool tool;

        if(!((e->Type() == TiXmlNode::ELEMENT) && e->Value() == "tool")){
          cerr << "Expected XML tree value \"tool\"" << endl;
        }

    	e->GetAttribute("name", &tool.m_name);

        std::string location;

        try{
        	node->GetAttribute("location", &location, true );
        }
        catch(...){
          location = m_executable_basename + "/"+  tool.m_name;
    #ifdef _WIN32
          location.append(".exe");
    #endif

    #ifdef __APPLE__
          std::string app;
          app = itemElement->GetAttribute("macosx_bundle");
          if (!(app.empty() || app.compare( "false" ) == 0) )
          {
        	  //TODO: add full apple path : Requires a mac for testing
            location.append(".app");
          }
    #endif
        }

        tool.m_location = location;

        try{
    	e->GetAttribute("input_format", &tool.m_input_type);
        }
        catch(...){ tool.m_input_type = "";  }

        try{
    	e->GetAttribute("output_format", &tool.m_output_type);
        }
        catch(...){tool.m_output_type = "";}
        tools.push_back(tool);

    }

	return tools;
}
