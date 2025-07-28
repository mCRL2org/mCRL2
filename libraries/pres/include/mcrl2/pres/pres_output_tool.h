// Author(s): Jan Friso Groote. Based on bes/pbes_output_tool.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/pres_output_tool.h
/// \brief Base class for tools that produce a (P)RES as output.

#ifndef MCRL2_RES_PRES_OUTPUT_TOOL_H
#define MCRL2_RES_PRES_OUTPUT_TOOL_H

#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/pres/io.h"

namespace mcrl2::pres_system::tools
{

/// \brief Base class for filter tools that produce a pres as output.
/// \pre Tool provides output_filename()
template <typename Tool>
class pres_output_tool: public Tool
{
  protected:

    /// \brief The type of the pres output format
    utilities::file_format m_pres_output_format;

    /// \brief Returns the file formats that are available for this tool.
    /// Override this method to change the standard behavior.
    /// \return The set { pres, text }
    virtual std::set<utilities::file_format> available_output_formats() const
    {
      std::set<utilities::file_format> result;
      result.insert(pres_system::pres_format_internal());
      result.insert(pres_system::pres_format_text());
      return result;
    }

    /// \brief Returns the default file format.
    /// Override this method to change the standard behavior.
    /// \return The default format is determined based on the extension of the output file. 
    //          If this fails, pres_format_internal() is returned. 
    virtual utilities::file_format default_output_format() const
    {
      utilities::file_format result= pres_system::guess_format(Tool::output_filename());
      if (result == utilities::file_format())
      {
        result = pres_system::pres_format_internal();
      }
      return result;
    }

    /// \brief Add options to an interface description. Also includes
    /// output format options.
    /// \param desc An interface description
    void add_options(utilities::interface_description& desc) override
    {
      Tool::add_options(desc);
      std::set<utilities::file_format> types = available_output_formats();
      auto option_argument = utilities::make_enum_argument<std::string>("FORMAT");
      for (const utilities::file_format& type: types)
      {
        option_argument.add_value_desc(type.shortname(), type.description(), type == default_output_format());
      }
      desc.add_option("out", option_argument, "use output format FORMAT:", 'o');
    }

    /// \brief Parse non-standard options
    /// \param parser A command line parser
    void parse_options(const utilities::command_line_parser& parser) override
    {
      Tool::parse_options(parser);
      m_pres_output_format = utilities::file_format();
      if(parser.options.count("out"))
      {
        std::set<utilities::file_format> types = available_output_formats();
        std::string arg = parser.option_argument_as<std::string>("out");
        for (const utilities::file_format& type: types)
        {
          if (type.shortname() == arg)
          {
            m_pres_output_format = type;
          }
        }
        if (m_pres_output_format == utilities::file_format())
        {
          mCRL2log(log::warning) << "Invalid input format given (" << arg << ").\n";
        }
      }
      if (m_pres_output_format == utilities::file_format())
      {
        m_pres_output_format = default_output_format();
        mCRL2log(log::verbose) << "Guessing output format: " << m_pres_output_format.description()
                               << std::endl;
      }
    }


  public:
    /// \brief Constructor.
    /// \param name The name of the tool
    /// \param author The author(s) of the tool
    /// \param what_is One-line "what is" description of the tool
    /// \param tool_description The description of the tool
    /// \param known_issues Known issues with the tool
    pres_output_tool(const std::string& name,
                     const std::string& author,
                     const std::string& what_is,
                     const std::string& tool_description,
                     std::string known_issues = "")
      : Tool(name, author, what_is, tool_description, known_issues)
    {}

    /// \brief Destructor.
    ~pres_output_tool() override = default;

    /// \brief Returns the output format
    /// \return The output format
    utilities::file_format pres_output_format() const
    {
      return m_pres_output_format;
    }
};

} // namespace mcrl2::pres_system::tools

#endif // MCRL2_RES_PRES_OUTPUT_TOOL_H
