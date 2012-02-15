// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/pbes_output_tool.h
/// \brief Base class for tools that produce a (P)BES as output.

#ifndef MCRL2_UTILITIES_PBES_OUTPUT_TOOL_H
#define MCRL2_UTILITIES_PBES_OUTPUT_TOOL_H

#include <set>
#include <string>
#include <iostream>
#include <stdexcept>
#include "mcrl2/pbes/file_formats.h"

namespace mcrl2
{

namespace utilities
{

namespace tools
{

/// \brief Base class for filter tools that produce a pbes as output.
/// \pre Tool provides output_filename()
template <typename Tool>
class pbes_output_tool: public Tool
{
  public:

    /// \brief Returns a description of a pbes file format
    /// \param f a pbes file format
    /// \return A description of the file format
    static
    std::string output_description(pbes_system::pbes_file_format f)
    {
      switch (f)
      {
        case pbes_system::pbes_file_pbes     :
          return "  'pbes'     PBES in internal format";
        case pbes_system::pbes_file_bes      :
          return "  'bes'      BES in internal format";
        case pbes_system::pbes_file_cwi      :
          return "  'cwi'      BES in CWI format";
        case pbes_system::pbes_file_pgsolver :
          return "  'pgsolver' max-parity game in PGSolver format";
        default:
          return "  unknown file format";
      }
    }

  protected:

    /// \brief The type of the pbes output format
    pbes_system::pbes_file_format m_pbes_output_format;

    /// \brief Returns the file formats that are available for this tool.
    /// Override this method to change the standard behavior.
    /// \return The set { pbes, bes, cwi, pgsolver }
    virtual std::set<pbes_system::pbes_file_format> available_output_formats() const
    {
      std::set<pbes_system::pbes_file_format> result;
      result.insert(pbes_system::pbes_file_pbes);
      result.insert(pbes_system::pbes_file_bes);
      result.insert(pbes_system::pbes_file_cwi);
      result.insert(pbes_system::pbes_file_pgsolver);
      return result;
    }

    /// \brief Returns the default file format.
    /// Override this method to change the standard behavior.
    /// \return The string "pbes"
    virtual std::string default_output_format() const
    {
      return "pbes";
    }

    /// \brief Add options to an interface description. Also includes
    /// output format options.
    /// \param desc An interface description
    void add_options(interface_description& desc)
    {
      Tool::add_options(desc);
      std::string text = "use output format FORMAT:\n";
      std::set<pbes_system::pbes_file_format> types = available_output_formats();
      for (typename std::set<pbes_system::pbes_file_format>::iterator i = types.begin(); i != types.end(); ++i)
      {
        text = text + (i == types.begin() ? "" : "\n") + output_description(*i);
        if(pbes_system::file_format_to_string(*i) == default_output_format())
        {
          text = text + " (default)";
        }
      }
      desc.add_option(
        "out",
        make_mandatory_argument("FORMAT"),
        text,
        'o'
      );
    }

    /// \brief Parse non-standard options
    /// \param parser A command line parser
    void parse_options(const command_line_parser& parser)
    {
      Tool::parse_options(parser);
      if(parser.options.count("out"))
      {
        m_pbes_output_format = pbes_system::file_format_from_string(parser.option_argument("out"));
      }
      else
      {
        try
        {
          m_pbes_output_format = pbes_system::guess_format(Tool::output_filename());
        }
        catch(mcrl2::runtime_error&)
        {
          m_pbes_output_format = pbes_system::file_format_from_string(default_output_format());
        }
      }
    }


  public:
    /// \brief Constructor.
    /// \param name The name of the tool
    /// \param author The author(s) of the tool
    /// \param what_is One-line "what is" description of the tool
    /// \param tool_description The description of the tool
    pbes_output_tool(const std::string& name,
                       const std::string& author,
                       const std::string& what_is,
                       const std::string& tool_description,
                       std::string known_issues = ""
                      )
      : Tool(name, author, what_is, tool_description, known_issues)
    {}

    /// \brief Returns the output format
    /// \return The output format
    pbes_system::pbes_file_format pbes_output_format() const
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
    bes_output_tool(const std::string& name,
                       const std::string& author,
                       const std::string& what_is,
                       const std::string& tool_description,
                       std::string known_issues = ""
                      )
      : pbes_output_tool<Tool>(name, author, what_is, tool_description, known_issues)
    {}

  protected:
    /// \brief Returns the default file format.
    /// Override this method to change the standard behavior.
    /// \return The string "pbes"
    virtual std::string default_output_format() const
    {
      return "bes";
    }

  public:
    /// \brief Returns the output format
    /// \return The output file format
    pbes_system::pbes_file_format bes_output_format() const
    {
      return pbes_output_tool<Tool>::pbes_output_format();
    }
};

} // namespace tools

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_PBES_OUTPUT_TOOL_H
