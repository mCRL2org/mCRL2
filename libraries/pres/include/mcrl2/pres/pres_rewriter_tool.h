// Author(s): Jan Friso Groote. Based on mcrl2/bes/pbes_rewriter_tool.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/pres_rewriter_tool.h
/// \brief Base class for tools that use a pres rewriter.

#ifndef MCRL2_RES_PRES_REWRITER_TOOL_H
#define MCRL2_RES_PRES_REWRITER_TOOL_H

#include "mcrl2/pres/pres_rewriter_type.h"
#include "mcrl2/utilities/command_line_interface.h"
#include <set>

namespace mcrl2::pres_system::tools
{

/// \brief Base class for filter tools that use a pres rewriter.
template <typename Tool>
class pres_rewriter_tool: public Tool
{
  protected:
    /// \brief The type of the pres rewriter
    pres_system::pres_rewriter_type m_pres_rewriter_type;

    /// \brief Returns the types of rewriters that are available for this tool.
    /// Override this method to change the standard behavior.
    /// \return The set { simplify, quantifier_all, quantifier_finite }
    virtual std::set<pres_system::pres_rewriter_type> available_rewriters() const
    {
      std::set<pres_system::pres_rewriter_type> result;
      result.insert(pres_system::simplify);
      result.insert(pres_system::quantifier_all);
      result.insert(pres_system::quantifier_finite);
      result.insert(pres_system::quantifier_inside);
      result.insert(pres_system::quantifier_one_point);
      return result;
    }

    /// \brief Returns the default pres rewriter.
    /// Override this method to change the standard behavior.
    /// \return The string "simplify"
    virtual pres_system::pres_rewriter_type default_rewriter() const
    {
      return pres_system::simplify;
    }

    /// \brief Add options to an interface description. Also includes
    /// rewriter options.
    /// \param desc An interface description
    void add_options(utilities::interface_description& desc) override
    {
      Tool::add_options(desc);

      utilities::interface_description::enum_argument<pres_system::pres_rewriter_type> arg(utilities::make_enum_argument<pres_system::pres_rewriter_type>("NAME"));

      // Compute the available rewriters, and add the approriate arguments
      std::set<pres_system::pres_rewriter_type> types = available_rewriters();
      for (auto type : types)
      {
        arg.add_value(type, type==default_rewriter());
      }

      desc.add_option(
        "pres-rewriter",
        arg,
        "use pres rewrite strategy NAME:",
        'p'
      );
    }

    /// \brief Parse non-standard options
    /// \param parser A command line parser
    void parse_options(const utilities::command_line_parser& parser) override
    {
      Tool::parse_options(parser);
      m_pres_rewriter_type = parser.option_argument_as<pres_system::pres_rewriter_type>("pres-rewriter");
    }


  public:
    /// \brief Constructor.
    /// \param name The name of the tool
    /// \param author The author(s) of the tool
    /// \param what_is One-line "what is" description of the tool
    /// \param tool_description The description of the tool
    /// \param known_issues Known issues with the tool
    pres_rewriter_tool(const std::string& name,
                       const std::string& author,
                       const std::string& what_is,
                       const std::string& tool_description,
                       std::string known_issues = ""
                      )
      : Tool(name, author, what_is, tool_description, known_issues)
    {}

    /// \brief Destructor.
    ~pres_rewriter_tool() override = default;

    /// \brief Returns the rewriter type
    /// \return The rewriter type
    pres_system::pres_rewriter_type rewriter_type() const
    {
      return m_pres_rewriter_type;
    }
};

} // namespace mcrl2::pres_system::tools

#endif // MCRL2_RES_PRES_REWRITER_TOOL_H
