// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/mcrl2_gui_tool.h
/// \brief add your file description here.

#ifndef MCRL2_UTILITIES_MCRL2_GUI_TOOL_H
#define MCRL2_UTILITIES_MCRL2_GUI_TOOL_H

#include <map>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <iostream>

namespace mcrl2 {

namespace utilities {

struct widget_option {
	std::string widget;
	std::string default_value;
	std::vector<std::string> values;
};

/// \brief Base class for the mcrl2 gui tools.
template<typename Tool>
class mcrl2_gui_tool: public Tool {
protected:

	std::map<std::string, widget_option> m_gui_options;
	bool m_gui_options_selected;
	std::map<std::string, std::string> m_get_long_argument_with_description;
	std::string m_toolname;

	/// \brief Add options to an interface description. Also includes
	/// rewriter options.
	/// \param desc An interface description
	void add_options(interface_description& desc) {
		Tool::add_options(desc);
		desc.add_option(
				"mcrl2-gui",
				"outputs information about the visual representation of this option in the mCRL2 GUI");
		m_get_long_argument_with_description
				= desc.get_long_argument_with_description();
		m_toolname = desc.get_toolname();

	}

	/// \brief Parse non-standard options
	/// \param parser A command line parser
	void parse_options(const command_line_parser& parser) {
		Tool::parse_options(parser);
		m_gui_options_selected = parser.options.count("mcrl2-gui") > 0;
	}

	void print_mcrl2_gui_options() const {

		std::cout << "<tool>" << std::endl;
		std::cout << "  <name>" << m_toolname << "</name>" << std::endl;

		std::cout << "  <arguments>" << std::endl;

		for (std::map<std::string, widget_option>::const_iterator i =
				m_gui_options.begin(); i != m_gui_options.end(); ++i) {

			if (!i->second.widget.empty()) {

				std::cout << "    <argument>" << std::endl;
				std::cout << "      <identifier>" << i->first << "</identifier> " << std::endl;
				std::cout << "      <description>" << std::endl;

				std::istringstream iss(m_get_long_argument_with_description.find(i->first)->second);
        std::vector<std::string> tokens;

        std::string temp;
        while (std::getline(iss, temp)) {
        tokens.push_back(temp);}

				for (std::vector<std::string>::iterator j = tokens.begin(); j != tokens.end(); ++j) {
					std::cout << "         <line>"<< (j->empty()?" ":*j) << "</line> " << std::endl;
				}

				std::cout << "      </description>" << std::endl;
				std::cout << "      <widget>" << i->second.widget << "</widget>" << std::endl;

				if (!i->second.values.empty()) {
					std::cout << "      <values>" << std::endl;
					for (std::vector<std::string>::const_iterator j =
							i->second.values.begin(); j
							!= i->second.values.end(); ++j) {
						std::cout << "        <value>" << *j << "</value>" << std::endl;
					}
					std::cout << "      </values>" << std::endl;
					std::cout << "      <default_value>" << i->second.default_value << "</default_value>"
							<< std::endl;
				}
				std::cout << "    </argument>" << std::endl;
			}
		}
		std::cout << "  </arguments>" << std::endl;
		std::cout << "</tool>" << std::endl;
	}

    /**
     * \brief Creates a checkbox widget for mcrl2-gui
     * \param[in] default values. Whenever true, the checkbox is checked.
     * \return struct of a widget_option containing the widget that represent a checkbox
     **/
	widget_option create_checkbox_widget(bool enabled=false){
		widget_option wo;

		wo.default_value = enabled? "true" : "false" ;
		wo.widget = "checkbox";
		std::string values[] = { "true", "false" };
		wo.values = std::vector<std::string>(values, values + sizeof(values)
				/ sizeof(std::string));
		return wo;
	}

	/**
     * \brief Creates a radiobox widget for mcrl2-gui
     * \param[in] vector of values to choose from in the radiobox.
     * \param[in] index that denotes the default value
     * \return struct of a widget_option containing the widget that represent a checkbox
     **/
	widget_option create_radiobox_widget(std::vector<std::string> values, size_t index=0){
		widget_option wo;
		if( index < values.size() ){
			wo.default_value = values[index];
		}
		wo.widget = "radiobox";
		wo.values = values;
		return wo;
	}

	/**
	     * \brief Creates a textcontrol widget for mcrl2-gui
	     * \param[in] default value for the textcontrol.
	     * \return struct of a widget_option containing the widget that represents a textcontrol
	     **/
	widget_option create_textctrl_widget(std::string value = ""){
		widget_option wo;
		wo.widget = "textctrl";
		if (!value.empty()) {
			wo.values.push_back(value);
			wo.default_value = value;
		}
		return wo;
	}

	/**
	     * \brief Creates a filepicker widget for mcrl2-gui
	     * \param[in] default pathfile for the filepicker.
	     * \return struct of a widget_option containing the widget that represents a filepicker
	     **/
	widget_option create_filepicker_widget(std::string value = ""){
		widget_option wo;
		wo.widget = "filepicker";
		if (!value.empty()) {
			wo.values.push_back(value);
			wo.default_value = value;
		}
		return wo;
	}

public:
	mcrl2_gui_tool() :
		m_gui_options_selected(false) {

		// Help is already displayed, therefore not required as separate option
		//m_gui_options["help"] = create_checkbox_widget();
		m_gui_options["verbose"] = create_checkbox_widget(true);
		m_gui_options["debug"] = create_checkbox_widget();

	}

	bool run() {
		if (m_gui_options_selected) {
			print_mcrl2_gui_options();
			exit(EXIT_SUCCESS);
			//return true;
		}
		Tool::run();
		return true;
	}
};

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_MCRL2_GUI_TOOL_H
