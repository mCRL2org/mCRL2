// Author(s): Jan Friso Groote (based on mcrl2/bes/pbes_rewriter_tool.h by Wieger Wesselink)
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/lps_rewriter_tool.h
/// \brief Base class for tools that use an lps rewriter.

#ifndef MCRL2_LPS_LPS_REWRITER_TOOL_H
#define MCRL2_LPS_LPS_REWRITER_TOOL_H

#include <set>
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/lps/lps_rewriter_type.h"

namespace mcrl2::lps::tools
{

/// \brief Base class for filter tools that use a lps rewriter.
template <typename Tool>
class lps_rewriter_tool: public Tool
{
  protected:
    /// \brief The type of the lps rewriter
    lps::lps_rewriter_type m_lps_rewriter_type;

    /// \brief Returns the types of rewriters that are available for this tool.
    /// Override this method to change the standard behavior.
    /// \return The set { simplify, quantifier_all, quantifier_finite }
    virtual std::set<lps::lps_rewriter_type> available_rewriters() const
    {
      std::set<lps::lps_rewriter_type> result;
      result.insert(lps::simplify);
      result.insert(lps::quantifier_one_point);
      result.insert(lps::condition_one_point);
      result.insert(lps::prune_dataspec);
      return result;
    }

    /// \brief Returns the default lps rewriter.
    /// Override this method to change the standard behavior.
    /// \return The string "simplify"
    virtual lps::lps_rewriter_type default_rewriter() const
    {
      return lps::simplify;
    }

    /// \brief Add options to an interface description. Also includes
    /// rewriter options.
    /// \param desc An interface description
    void add_options(utilities::interface_description& desc) override
    {
      Tool::add_options(desc);

      utilities::interface_description::enum_argument<lps::lps_rewriter_type> arg(utilities::make_enum_argument<lps::lps_rewriter_type>("NAME"));

      // Compute the available rewriters, and add the approriate arguments
      for (const lps::lps_rewriter_type& type: available_rewriters())
      {
        arg.add_value(type, type==default_rewriter());
      }

      desc.add_option(
        "lps-rewriter",
        arg,
        "use lps rewrite strategy NAME:",
        'p'
      );
    }

    /// \brief Parse non-standard options
    /// \param parser A command line parser
    void parse_options(const utilities::command_line_parser& parser) override
    {
      Tool::parse_options(parser);
      m_lps_rewriter_type = parser.option_argument_as<lps::lps_rewriter_type>("lps-rewriter");
    }


  public:
    /// \brief Constructor.
    /// \param name The name of the tool
    /// \param author The author(s) of the tool
    /// \param what_is One-line "what is" description of the tool
    /// \param tool_description The description of the tool
    /// \param known_issues Known issues with the tool
    lps_rewriter_tool(const std::string& name,
                       const std::string& author,
                       const std::string& what_is,
                       const std::string& tool_description,
                       std::string known_issues = ""
                      )
      : Tool(name, author, what_is, tool_description, known_issues)
    {}

    /// \brief Destructor.
    ~lps_rewriter_tool() override = default;

    /// \brief Returns the rewriter type
    /// \return The rewriter type
    lps::lps_rewriter_type rewriter_type() const
    {
      return m_lps_rewriter_type;
    }
};

} // namespace mcrl2::lps::tools

#endif // MCRL2_LPS_LPS_REWRITER_TOOL_H
