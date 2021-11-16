// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/parallel_tool.h
/// \brief Class that transforms a tool into a parallel tool with a --threads flag. 

#ifndef MCRL2_UTILITIES_PARALLEL_TOOL_H
#define MCRL2_UTILITIES_PARALLEL_TOOL_H

#include "mcrl2/utilities/command_line_interface.h"

namespace mcrl2
{

namespace utilities
{

namespace tools
{

/// \brief Base class for tools that use a rewriter.
template <typename Tool>
class parallel_tool: public Tool
{
  protected:
    /// The number of threads in the parallel tool.
    std::size_t m_number_of_threads=1;

    /// \brief Add the --threads option to an interface description. 
    /// \param desc An interface description
    void add_options(utilities::interface_description& desc)
    {
      Tool::add_options(desc);
      desc.add_option("threads", utilities::make_mandatory_argument("NUM"), "run with NUM threads (default=1). ");
    }


    /// \brief Parse non-standard options
    /// \param parser A command line parser
    void parse_options(const utilities::command_line_parser& parser)
    {
      Tool::parse_options(parser);

      if (parser.has_option("threads"))
      {
        m_number_of_threads = parser.option_argument_as<std::size_t>("threads");
        if (m_number_of_threads<1)
        {
          throw mcrl2::runtime_error("The number of threads should at least be 1.");
        }
      }
    }

  public:

    /// \brief Constructor.
    parallel_tool(const std::string& name,
                  const std::string& author,
                  const std::string& what_is,
                  const std::string& tool_description,
                  std::string known_issues = ""
                 )
      : Tool(name, author, what_is, tool_description, known_issues),
        m_rewrite_strategy(mcrl2::data::jitty)
    {}

    /// \brief Returns the number of threads in this tool.
    /// \return The number of threads.
    std::size_t number_of_threads() const
    {
      return m_rewrite_strategy;
    }
};

} // namespace tools

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_PARALLEL_TOOL_H
