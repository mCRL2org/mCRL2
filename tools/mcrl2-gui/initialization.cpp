// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file initialization.cpp


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
#include <wx/wx.h>
#include <wx/file.h>
#include <wx/mimetype.h>

#include "mcrl2/utilities/basename.h"
#include "mcrl2/utilities/logger.h"

#include <wx/xml/xml.h>
#include <wx/sstream.h>

using namespace std;

Initialization::Initialization()
{
  /* extension (first) is dealt with "formalism" (second) */
  m_extention_tool_mapping.insert(pair<string, string> ("mcrl2", "mcrl2"));
  m_extention_tool_mapping.insert(pair<string, string> ("lps", "lps"));

  m_extention_tool_mapping.insert(pair<string, string> ("lts", "lts"));
  m_extention_tool_mapping.insert(pair<string, string> ("fsm", "lts"));
  m_extention_tool_mapping.insert(pair<string, string> ("aut", "lts"));
  m_extention_tool_mapping.insert(pair<string, string> ("dot", "lts"));
  m_extention_tool_mapping.insert(pair<string, string> ("svc", "lts"));
#ifdef USE_BCG
  m_extention_tool_mapping.insert(pair<string, string> ("bcg", "lts"));
#endif
  m_extention_tool_mapping.insert(pair<string, string> ("pbes", "pbes"));
  m_extention_tool_mapping.insert(pair<string, string> ("txt", "txt"));
  m_extention_tool_mapping.insert(pair<string, string> ("chi", "chi"));
  m_extention_tool_mapping.insert(pair<string, string> ("lysa", "lysa"));
  m_extention_tool_mapping.insert(pair<string, string> ("tbf", "tbf"));
  m_extention_tool_mapping.insert(pair<string, string> ("gra", "gra"));
  m_extention_tool_mapping.insert(pair<string, string> ("bes", "bes"));

  m_extention_tool_mapping.insert(pair<string, string> ("mcf", "mcf"));
  m_extention_tool_mapping.insert(pair<string, string> ("trc", "trc"));
  m_extention_tool_mapping.insert(pair<string, string> ("gm", "bes"));
  m_extention_tool_mapping.insert(pair<string, string> ("cwi", "bes"));
  m_extention_tool_mapping.insert(pair<string, string> ("pbes", "bes"));

  mcrl2::utilities::basename basename;
  m_executable_basename = basename.get_executable_basename();
  m_toolset_basename = basename.get_toolset_basename();

#ifdef __APPLE__
  std::string file = m_executable_basename + "/mcrl2-gui.app/share/mcrl2/mcrl2.png";
#else
  std::string file = m_toolset_basename + "/share/mcrl2/mcrl2.png";
#endif

  /* show splash screen*/
  wxInitAllImageHandlers();

  wxBitmap bitmap;
  wxString wxFile(file.c_str(), wxConvUTF8);
  if (bitmap.LoadFile(wxFile, wxBITMAP_TYPE_PNG))
  {
    new wxSplashScreen(bitmap,
                       wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT, 2000, NULL, -1,
                       wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER
                       | wxSTAY_ON_TOP);
  }
  wxYield();

  /* Read tools */
  std::vector<Tool> p_tools = Read_tools();

  /* for each tool */
  for (std::vector<Tool>::iterator i = p_tools.begin(); i != p_tools.end(); ++i)
  {
    string cmd = "\""+ (*i).m_location + "\" --mcrl2-gui";

    wxArrayString tool_output;
    wxArrayString tool_errors;

    wxString wxCmd = wxString(cmd.c_str(), wxConvUTF8);

    /* Execute tool */
#if wxCHECK_VERSION(2, 9, 0)
    int result = wxExecute(wxCmd, tool_output, tool_errors, wxEXEC_SYNC | wxEXEC_NOEVENTS);
#else
    int result = wxExecute(wxCmd, tool_output, tool_errors, wxEXEC_SYNC);
#endif
    if (result != 0)
    {
      mCRL2log(mcrl2::log::error) << "Failed to execute " << cmd << std::endl;
    }
    else
    {
      Tool c_tool = (*i);

      /* Tool option vector to store tool options */
      vector<Tool_option> vto;

      string tool_output_string;

      for (size_t j = 0; j < tool_output.GetCount(); ++j)
      {
        tool_output_string += string(tool_output[j].mb_str(wxConvUTF8));
      }

      wxString wx_tool_output_string;
      for (size_t j = 0; j < tool_output.GetCount(); ++j)
      {
        wx_tool_output_string.Append(tool_output[j]);
      }

      wxXmlDocument wx_doc;
      wxStringInputStream wx_input_string_stream(wx_tool_output_string);
      wx_doc.Load(wx_input_string_stream);
      if (!wx_doc.IsOk())
      {
        wxString error = _T("Could not parse mcrl2-gui print for:\n") +
                         wxString((*i).m_location.c_str(), wxConvUTF8)
                         ;

        wxMessageDialog* dial = new wxMessageDialog(NULL,
            error, wxT("Error"), wxOK | wxICON_ERROR);
        dial->ShowModal();
      }

      if (!(wx_doc.GetRoot()->GetName() == wxT("tool")))
      {
        cerr << "Expected XML node value \"tool\", got node value: " << wx_doc.GetRoot()->GetName().mb_str() << endl;
      }
      wxXmlNode* child = wx_doc.GetRoot()->GetChildren();
      while (child)
      {
        if (child->GetName() == wxT("arguments"))
        {
          /*
           * Iterate over arguments
           */
          wxXmlNode* args_child = child->GetChildren();

          while (args_child)
          {
            if (args_child->GetName() == wxT("argument"))
            {
              Tool_option to;

              to.m_default_value =0;
              to.m_flag = "";
              to.m_values.clear();
              to.m_widget = none;
              to.m_help ="";

              string str_default_value ;

              wxXmlNode* arg_child = args_child->GetChildren();
              while (arg_child)
              {

                /*
                 * Get long identifier flag
                 */
                if (arg_child->GetName() == wxT("identifier"))
                {
                  to.m_flag = arg_child->GetNodeContent().mb_str();
                }

                /*
                 * Get description
                 */
                if (arg_child->GetName() == wxT("description"))
                {
                  to.m_help.clear();

                  wxXmlNode* line_child = arg_child->GetChildren();
                  while (line_child)
                  {
                    if (line_child->GetName() == wxT("line"))
                    {
                      to.m_help.append(std::string(line_child->GetNodeContent().mb_str()) +"\n");
                    }
                    line_child =line_child->GetNext();
                  }

                }

                /*
                 * Get widget
                 */
                if (arg_child->GetName() == wxT("widget"))
                {
                  /*
                   * Set widget if proper value detected
                   */

                  if (arg_child->GetNodeContent() == wxT("checkbox"))
                  {
                    to.m_widget = checkbox;
                  }
                  if (arg_child->GetNodeContent() == wxT("textctrl"))
                  {
                    to.m_widget = textctrl;
                  }
                  if (arg_child->GetNodeContent() == wxT("radiobox"))
                  {
                    to.m_widget = radiobox;
                  }
                  if (arg_child->GetNodeContent() == wxT("filepicker"))
                  {
                    to.m_widget = filepicker;
                  }
                }

                /*
                 * Get default value and store it temporary.
                 * To determine index value that stores default value
                 */
                if (arg_child->GetName() == wxT("default_value"))
                {
                  str_default_value = arg_child->GetNodeContent().mb_str();
                }

                /*
                 * Get values
                 */
                if (arg_child->GetName() == wxT("values"))
                {

                  /*
                   * Iterate over possible values
                   */
                  wxXmlNode* value_child = arg_child->GetChildren();

                  while (value_child)
                  {
                    if (value_child->GetName() == wxT("value"))
                    {
                      to.m_values.push_back(std::string(value_child->GetNodeContent().mb_str()));
                    }

                    value_child = value_child -> GetNext();
                  }

                }

                arg_child = arg_child->GetNext();
              }

              /*Post Process to get default value*/
              for (vector< string >::iterator i = to.m_values.begin();
                   i != to.m_values.end();
                   ++i)
              {
                if (i->compare(str_default_value) == 0)
                {
                  to.m_default_value = distance(to.m_values.begin(), i);
                }
              }

              /*
               *  Add tool option to vector of tool options
               */
              vto.push_back(to);
            }

            args_child = args_child->GetNext();

          }

        }

        child = child->GetNext();
      }

      /* Add tool option vector too tool catalog */
      c_tool.m_tool_options = vto;
      /* Add options for tool */
      m_tool_catalog.push_back(c_tool);

    }
  } /* End - for each tool */
}

vector<Tool> Initialization::Read_tools()
{

  std::vector<Tool> tools;
  std::string line;


#ifdef __APPLE__
  string tool_catalog_file = m_executable_basename + "/mcrl2-gui.app/share/mcrl2/tool_catalog.xml" ;
#else
  string tool_catalog_file = m_toolset_basename + "/share/mcrl2/tool_catalog.xml" ;
#endif

  wxXmlDocument wx_doc;
  wx_doc.Load(wxString(tool_catalog_file.c_str() , wxConvUTF8));
  if (!wx_doc.IsOk())
  {
    wxString error = _T("Could not load tool catalog file:\n") +
                     wxString(tool_catalog_file.c_str() , wxConvUTF8)
                     ;

    wxMessageDialog* dial = new wxMessageDialog(NULL,
        error, wxT("Error"), wxOK | wxICON_ERROR);
    dial->ShowModal();
  }

  if (!(wx_doc.GetRoot()->GetName() == wxT("tool-catalog")))
  {
    cerr << "Expected XML node value \"tool-catalog\", got node value: " << wx_doc.GetRoot()->GetName().mb_str() << endl;
  }
  wxXmlNode* child = wx_doc.GetRoot()->GetChildren();

  while (child)
  {
    Tool tool;
    tool.m_tool_type = shell;

    if (!(child->GetName() == wxT("tool")))
    {
      cerr << "Expected XML tree value \"tool\"" << endl;
    }

    wxString value;
    value = child->GetPropVal(wxT("name"), wxEmptyString);
    tool.m_name = value.mb_str();

    value = child->GetPropVal(wxT("shell"), wxEmptyString);
    if (!(value.IsEmpty() || value == wxT("false")))
    {
      tool.m_tool_type = ishell;
    }

    value = child->GetPropVal(wxT("input_format"), wxEmptyString);
    if( value != wxEmptyString ){
      tool.m_input_types.push_back( string( value.mb_str() ) );
    }

    value = child->GetPropVal(wxT("input_format1"), wxEmptyString);
    if( value != wxEmptyString ){
      tool.m_input_types.push_back( string( value.mb_str() ) );
    }

    value = child->GetPropVal(wxT("output_format"), wxEmptyString);
    tool.m_output_type = value.mb_str();

    for (multimap<std::string,std::string>::iterator it = m_extention_tool_mapping.begin();
         it !=  m_extention_tool_mapping.end();
         ++it)
    {

      if (it->second.compare(tool.m_output_type) == 0)
      {
        tool.m_extentions.push_back(it->first);
      }
    };

    value = child->GetPropVal(wxT("location"), wxEmptyString);
    std::string location = std::string(value.mb_str());
    if (location.empty())
    {
      location = m_executable_basename + "/"+  tool.m_name;
#ifdef _WIN32
      location.append(".exe");
      value = child->GetPropVal(wxT("gui"), wxEmptyString);
      if (!(value.IsEmpty() || (value == wxT("false"))))
      {
        tool.m_tool_type = gui;
      }
#endif

#ifdef __APPLE__
      value = child->GetPropVal(wxT("gui"), wxEmptyString);
      if (!(value.IsEmpty() || (value == wxT("false"))))
      {
        //Expand to full path
        location.append(".app/Contents/MacOS/"+ tool.m_name);
        tool.m_tool_type = gui;
      }
#endif
    }

    tool.m_location = location;

    if (!wxFile::Exists(wxString(location.c_str(), wxConvUTF8)))
    {
      cout << "File \"" << location << "\" does not exist" << endl;
    }

    value = child->GetPropVal(wxT("category"), wxEmptyString);
    tool.m_category = value.mb_str();

    tools.push_back(tool);
    child = child->GetNext();

  }

  return tools;
}

