// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/pbes_output_tool.h
/// \brief Base class for tools that produce a (P)BES as output.

#ifndef MCRL2_BES_PBES_OUTPUT_TOOL_H
#define MCRL2_BES_PBES_OUTPUT_TOOL_H

#include <iostream>
#include <set>
// #include <stdexcept>
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/file_utility.h"
#include <string>
#include "mcrl2/bes/io.h"
#include "mcrl2/pbes/io.h"

namespace mcrl2
{

namespace bes
{

namespace tools
{

/// \brief Base class for filter tools that produce a pbes as output.
/// \pre Tool provides output_filename()
template <typename Tool>
class pbes_output_tool: public Tool
{
  protected:

    /// \brief The type of the pbes output format
    utilities::file_format m_pbes_output_format;

    /// \brief Returns the file formats that are available for this tool.
    /// Override this method to change the standard behavior.
    /// \return The set { pbes, text }
    virtual std::set<utilities::file_format> available_output_formats() const
    {
      std::set<utilities::file_format> result;
      result.insert(pbes_system::pbes_format_internal());
      result.insert(pbes_system::pbes_format_text());
      return result;
    }

    /// \brief Returns the default file format.
    /// Override this method to change the standard behavior.
    /// \return The string "pbes"
    virtual utilities::file_format default_output_format() const
    {
      return pbes_system::guess_format(Tool::output_filename());
    }

    /// \brief Add options to an interface description. Also includes
    /// output format options.
    /// \param desc An interface description
    void add_options(utilities::interface_description& desc)
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
    void parse_options(const utilities::command_line_parser& parser)
    {
      Tool::parse_options(parser);
      m_pbes_output_format = utilities::file_format();
      if(parser.options.count("out"))
      {
        std::set<utilities::file_format> types = available_output_formats();
        std::string arg = parser.option_argument_as<std::string>("out");
        for (const utilities::file_format& type: types)
        {
          if (type.shortname() == arg)
          {
            m_pbes_output_format = type;
          }
        }
        if (m_pbes_output_format == utilities::file_format())
        {
          mCRL2log(log::warning) << "Invalid input format given (" << arg << ").\n";
        }
      }
      if (m_pbes_output_format == utilities::file_format())
      {
        m_pbes_output_format = default_output_format();
        mCRL2log(log::verbose) << "Guessing output format: " << m_pbes_output_format.description()
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
    pbes_output_tool(const std::string& name,
                     const std::string& author,
                     const std::string& what_is,
                     const std::string& tool_description,
                     std::string known_issues = "")
      : Tool(name, author, what_is, tool_description, known_issues)
    {}

    /// \brief Returns the output format
    /// \return The output format
    utilities::file_format pbes_output_format() const
    {
      return m_pbes_output_format;
    }
};

/// \brief Base class for filter tools that produce a bes as output
/// \pre Tool provides output_filename()
template <typename Tool>
class bes_output_tool: public pbes_output_tool<Tool>
{
  public:
    /// \brief Constructor.
    /// \param name The name of the tool
    /// \param author The author(s) of the tool
    /// \param what_is One-line "what is" description of the tool
    /// \param tool_description The description of the tool
    /// \param known_issues Known issues with the tool
    bes_output_tool(const std::string& name,
                    const std::string& author,
                    const std::string& what_is,
                    const std::string& tool_description,
                    std::string known_issues = "")
      : pbes_output_tool<Tool>(name, author, what_is, tool_description, known_issues)
    {}

  protected:

    /// \brief Returns the file formats that are available for this tool.
    /// Override this method to change the standard behavior.
    /// \return The set { pbes, bes, pgsolver }
    virtual std::set<utilities::file_format> available_output_formats() const
    {
      std::set<utilities::file_format> result;
      result.insert(pbes_system::pbes_format_internal());
      result.insert(pbes_system::pbes_format_text());
      result.insert(bes::bes_format_internal());
      result.insert(bes::bes_format_pgsolver());
      return result;
    }

    /// \brief Returns the default file format.
    /// Override this method to change the standard behavior.
    /// \return The string "pbes"
    virtual utilities::file_format default_output_format() const
    {
      utilities::file_format result = bes::guess_format(Tool::output_filename());
      if (result == utilities::file_format())
      {
        result = pbes_system::guess_format(Tool::output_filename());
      }
      return result;
    }

  public:
    /// \brief Returns the output format
    /// \return The output file format
    utilities::file_format bes_output_format() const
    {
      return pbes_output_tool<Tool>::pbes_output_format();
    }
};

} // namespace tools

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_PBES_OUTPUT_TOOL_H
