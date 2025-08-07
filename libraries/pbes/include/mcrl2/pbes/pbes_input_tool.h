// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes_input_tool.h
/// \brief Base class for tools that produce a (P)BES as output.

#ifndef MCRL2_PBES_PBES_INPUT_TOOL_H
#define MCRL2_PBES_PBES_INPUT_TOOL_H

#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/io.h"





namespace mcrl2::pbes_system::tools
{

/// \brief Base class for filter tools that take a pbes as input.
/// \pre Tool provides input_filename()
template <typename Tool>
class pbes_input_tool: public Tool
{
  protected:

    /// \brief The type of the pbes input format
    utilities::file_format m_pbes_input_format;

    /// \brief Returns the file formats that are available for this tool.
    /// Override this method to change the standard behavior.
    /// \return The set { pbes, bes, pgsolver }
    virtual std::set<utilities::file_format> available_input_formats() const
    {
      std::set<utilities::file_format> result;
      result.insert(pbes_system::pbes_format_internal());
      result.insert(pbes_system::pbes_format_text());
      result.insert(pbes_system::pbes_format_pgsolver());
      return result;
    }

    /// \brief Returns the default file format.
    /// Override this method to change the standard behavior.
    /// \return The file format based on the extension of the input file, or if it is not possible to
    //          determine the file format in this way, pbes_format_internal() is returned. 
    virtual utilities::file_format default_input_format() const
    {
      utilities::file_format result = pbes_system::guess_format(Tool::input_filename());
      if (result == utilities::file_format())
      {
        result = pbes_system::guess_format(Tool::input_filename());
      }
      if (result == utilities::file_format())
      {
        result = pbes_system::pbes_format_internal();
      }
      return result;
    }

    /// \brief Add options to an interface description. Also includes
    /// input format options.
    /// \param desc An interface description
    void add_options(utilities::interface_description& desc) override
    {
      Tool::add_options(desc);
      std::set<utilities::file_format> types = available_input_formats();
      auto option_argument = utilities::make_enum_argument<std::string>("FORMAT");
      for (const utilities::file_format& type: types)
      {
        option_argument.add_value_desc(type.shortname(), type.description(), type == default_input_format());
      }
      desc.add_option("in", option_argument, "use input format FORMAT:", 'i');
    }

    /// \brief Parse non-standard options
    /// \param parser A command line parser
    void parse_options(const utilities::command_line_parser& parser) override
    {
      Tool::parse_options(parser);
      m_pbes_input_format = utilities::file_format();
      if(parser.options.count("in"))
      {
        std::set<utilities::file_format> types = available_input_formats();
        std::string arg = parser.option_argument_as<std::string>("in");
        for (const utilities::file_format& type: types)
        {
          if (type.shortname() == arg)
          {
            m_pbes_input_format = type;
          }
        }
        if (m_pbes_input_format == utilities::file_format())
        {
          mCRL2log(log::warning) << "Invalid input format given (" << arg << ").\n";
        }
      }
      if (m_pbes_input_format == utilities::file_format())
      {
        m_pbes_input_format = default_input_format();
        mCRL2log(log::verbose) << "Guessing input format: " << m_pbes_input_format.description()
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
    pbes_input_tool(const std::string& name,
                       const std::string& author,
                       const std::string& what_is,
                       const std::string& tool_description,
                       std::string known_issues = ""
                      )
      : Tool(name, author, what_is, tool_description, known_issues)
    {}

    /// \brief Destructor.
    ~pbes_input_tool() override = default;

    /// \brief Returns the input file format
    /// \return The input format
    utilities::file_format pbes_input_format() const
    {
      return m_pbes_input_format;
    }
};

} // namespace mcrl2::pbes_system::tools





#endif // MCRL2_PBES_PBES_INPUT_TOOL_H
