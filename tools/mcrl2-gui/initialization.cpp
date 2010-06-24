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
#include <wx/file.h>

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

		/* Execute tool */
		wxString wxCmd(cmd.c_str(), wxConvUTF8);
		if(wxExecute(wxCmd, tool_output, tool_errors, wxEXEC_SYNC) != 0)
		{
			std::cerr << "Failed to execute " << cmd << std::endl;
		}
		else
		{
			Tool c_tool = (*i);

			/* Tool option vector to store tool options */
			vector<Tool_option> vto;

			string tool_output_string;

			for (size_t j = 0; j < tool_output.GetCount(); ++j) {
				tool_output_string += string(tool_output[j].mb_str(wxConvUTF8));
			}

			ticpp::Document doc;
			try{
				doc.Parse(tool_output_string);
			}
			catch( ... ){
				wxString error = _T("Could not parse mcrl2-gui print for:\n") +
						wxString((*i).m_location.c_str(), wxConvUTF8)
						;

				   wxMessageDialog *dial = new wxMessageDialog(NULL,
						   error, wxT("Error"), wxOK | wxICON_ERROR);
				   dial->ShowModal();
			}

			ticpp::Element* node = 0;
			node = doc.FirstChildElement();

			/*
			 * Parse tool options
			 */

			if(!((node->Type() == TiXmlNode::ELEMENT) && node->Value() == "tool")){
			  cerr << "Expected XML node value \"tool\", got node value: " << node->Value() << endl;
			}

			for (ticpp::Element* e = node->FirstChildElement(false); e != 0; e
					= e->NextSiblingElement(false)) {

				if (((e->Type() == TiXmlNode::ELEMENT) && e->Value() == "name")) {
					/*
					 * This node is only required for human readability
					 */
				}

				if (((e->Type() == TiXmlNode::ELEMENT) && e->Value() == "arguments")) {
					/*
					 * Iterate over arguments
					 */
					for (ticpp::Element* f = e->FirstChildElement(false); f != 0; f
									= f->NextSiblingElement(false)) {

						if (((f->Type() == TiXmlNode::ELEMENT) && f->Value() == "argument")) {
							Tool_option to;

							to.m_default_value =0;
							to.m_flag = "";
							to.m_values.clear();
							to.m_widget = none;
							to.m_help ="";

							string str_default_value ;

							for (ticpp::Element* g = f->FirstChildElement(false); g != 0; g
									= g->NextSiblingElement(false)) {

								/*
								 * Get long identifier flag
								 */

								if (((g->Type() == TiXmlNode::ELEMENT) && g->Value() == "identifier")) {
									to.m_flag = g->GetText();
								}

								/*
								 * Get description
								 */
								if (((g->Type() == TiXmlNode::ELEMENT) && g->Value() == "description")) {
									to.m_help.append(g->GetText());
								}

								/*
								 * Get widget
								 */
								if (((g->Type() == TiXmlNode::ELEMENT)
										&& g->Value() == "widget")) {

									/*
									 * Set widget if proper value detected
									 */

									if (g->GetText().compare("checkbox") == 0) {
										to.m_widget = checkbox;
									}
									if (g->GetText().compare("textctrl") == 0) {
										to.m_widget = textctrl;
									}
									if (g->GetText().compare("radiobox") == 0) {
										to.m_widget = radiobox;
									}
									if (g->GetText().compare("filepicker") == 0) {
										to.m_widget = filepicker;
									}
								}

								/*
								 * Get default value and store it temporary.
								 * To determine index value that stores default value
								 */
								if (((g->Type() == TiXmlNode::ELEMENT) && g->Value() == "default_value")) {
									str_default_value = g->GetText();
								}

								/*
								 * Get values
								 */
								if (((g->Type() == TiXmlNode::ELEMENT) && g->Value() == "values")) {

									/*
									 * Iterate over possible values
									 */

									for (ticpp::Element* h = g->FirstChildElement(false); h != 0; h
																= h->NextSiblingElement(false)) {
										if (((h->Type() == TiXmlNode::ELEMENT) && h->Value() == "value")) {
											to.m_values.push_back(h->GetText());
										}

									}
								}
							}

							/*Post Process to get default value*/
							for( vector< string >::iterator i = to.m_values.begin();
															i != to.m_values.end();
															++i){
								if (i->compare( str_default_value ) == 0 )
								{
								   to.m_default_value =	distance( to.m_values.begin(), i );
								}
							}

							/*
							 *  Add tool option to vector of tool options
							 */
							vto.push_back(to);
						}
					}
				}
			}

			/* Add tool option vector too tool catalog */
			c_tool.m_tool_options = vto;
			/* Add options for tool */
			m_tool_catalog.push_back(c_tool);

		}
	} /* End - for each tool */


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
				wxString(tool_catalog_file.c_str(), wxConvUTF8)
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

        if(!wxFile::Exists(wxString(location.c_str(), wxConvUTF8))){
        	cout << "File \"" << location << "\" does not exist" << endl;
        }

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
