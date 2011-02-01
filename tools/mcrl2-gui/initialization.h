// Author(s): Frank Stappers 
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file initialization.h


#ifndef MCRL2_GUI_INITIALIZATION_H_
#define MCRL2_GUI_INITIALIZATION_H_

#include <vector>
#include <string>
#include <map>

using namespace std;

enum widget {
	none,
	checkbox,
	radiobox,
	textctrl,
	filepicker
};

enum tool_type {
    shell,
    gui,
    ishell
};

struct Tool_option{
	string m_flag;
	string m_help;
	widget m_widget;
	vector<string> m_values;
	int m_default_value;
};

struct Tool
{
    string m_name;
    string m_location;
    string m_input_type;
    string m_output_type;
    vector<Tool_option> m_tool_options;
    tool_type m_tool_type;
    string m_category;
    vector<string> m_extentions;
};

struct FileIO {
	string input_file;
	string output_file;
};

class Initialization {
public:

	Initialization();
	//virtual ~Initialization();

	vector< Tool > m_tool_catalog;

	multimap<string,string> m_extention_tool_mapping;

private:
	vector< Tool > Read_tools();
	string m_toolset_basename;
	string m_executable_basename;
};

#endif /* INITIALIZATION_H_ */
