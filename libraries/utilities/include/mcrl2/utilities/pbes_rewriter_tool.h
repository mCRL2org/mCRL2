// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/pbes_rewriter_tool.h
/// \brief Base class for tools that use a pbes rewriter.

#ifndef MCRL2_UTILITIES_PBES_REWRITER_TOOL_H
#define MCRL2_UTILITIES_PBES_REWRITER_TOOL_H

#include <set>
#include <string>
#include <iostream>
#include <stdexcept>
#include "mcrl2/pbes/pbes_rewriter_type.h"
#include "mcrl2/utilities/rewriter_tool.h"

namespace mcrl2
{

namespace utilities
{

namespace tools
{

/// \brief Base class for filter tools that use a pbes rewriter.
template <typename Tool>
class pbes_rewriter_tool: public Tool
{
  protected:
    /// \brief The type of the pbes rewriter
    pbes_system::pbes_rewriter_type m_pbes_rewriter_type;

    /// \brief Returns the types of rewriters that are available for this tool.
    /// Override this method to change the standard behavior.
    /// \return The set { simplify, quantifier_all, quantifier_finite }
    virtual std::set<pbes_system::pbes_rewriter_type> available_rewriters() const
    {
      std::set<pbes_system::pbes_rewriter_type> result;
      result.insert(pbes_system::simplify);
      result.insert(pbes_system::quantifier_all);
      result.insert(pbes_system::quantifier_finite);
      result.insert(pbes_system::pfnf);
      return result;
    }

    /// \brief Returns the default pbes rewriter.
    /// Override this method to change the standard behavior.
    /// \return The string "simplify"
    virtual pbes_system::pbes_rewriter_type default_rewriter() const
    {
      return pbes_system::simplify;
    }

    /// \brief Add options to an interface description. Also includes
    /// rewriter options.
    /// \param desc An interface description
    void add_options(interface_description& desc)
    {
      Tool::add_options(desc);

      interface_description::enum_argument<pbes_system::pbes_rewriter_type> arg(make_enum_argument<pbes_system::pbes_rewriter_type>("NAME"));

      // Compute the available rewriters, and add the approriate arguments
      std::set<pbes_system::pbes_rewriter_type> types = available_rewriters();
      for (typename std::set<pbes_system::pbes_rewriter_type>::iterator i = types.begin(); i != types.end(); ++i)
      {
        arg.add_value(*i, *i==default_rewriter());
      }

      desc.add_option(
        "pbes-rewriter",
        arg,
        "use pbes rewrite strategy NAME:",
        'p'
      );
    }

    /// \brief Parse non-standard options
    /// \param parser A command line parser
    void parse_options(const command_line_parser& parser)
    {
      Tool::parse_options(parser);
      m_pbes_rewriter_type = parser.option_argument_as<pbes_system::pbes_rewriter_type>("pbes-rewriter");
    }


  public:
    /// \brief Constructor.
    /// \param name The name of the tool
    /// \param author The author(s) of the tool
    /// \param what_is One-line "what is" description of the tool
    /// \param tool_description The description of the tool
    pbes_rewriter_tool(const std::string& name,
                       const std::string& author,
                       const std::string& what_is,
                       const std::string& tool_description,
                       std::string known_issues = ""
                      )
      : Tool(name, author, what_is, tool_description, known_issues)
    {}

    /// \brief Returns the rewriter type
    /// \return The rewriter type
    pbes_system::pbes_rewriter_type rewriter_type() const
    {
      return m_pbes_rewriter_type;
    }
};

} // namespace tools

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_PBES_REWRITER_TOOL_H
