// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/filter_tool_with_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_UTILITIES_FILTER_TOOL_WITH_REWRITER_H
#define MCRL2_UTILITIES_FILTER_TOOL_WITH_REWRITER_H

#include "mcrl2/data/rewriter.h"
#include "mcrl2/utilities/command_line_rewriting.h"
#include "mcrl2/utilities/filter_tool.h"

namespace mcrl2 {

namespace utilities {

  /// \brief Base class for filter tools that use a rewriter.
  class filter_tool_with_rewriter: public filter_tool
  {
    protected:
      /// The data rewriter strategy
      RewriteStrategy m_rewrite_strategy;

      /// \brief Add options to an interface description. Also includes
      /// rewriter options.
      /// \param desc An interface description
      void add_options(utilities::interface_description& desc)
      {
        filter_tool::add_options(desc);
        desc.add_rewriting_options();
      }

      /// \brief Parse non-standard options
      /// \param parser A command line parser
      void parse_options(const command_line_parser& parser)
      {
        m_rewrite_strategy = parser.option_argument_as<RewriteStrategy>("tool");
      }

    public:

      /// \brief Constructor.
      filter_tool_with_rewriter(const std::string& name,
                                const std::string& author,
                                const std::string& tool_description
                               )
        : filter_tool(name, author, tool_description),
          m_rewrite_strategy(GS_REWR_JITTY)
      {}

      /// \brief Returns the rewrite strategy
      /// \return The rewrite strategy
      data::rewriter::strategy rewrite_strategy() const
      {
        return static_cast<data::rewriter::strategy>(m_rewrite_strategy);
      }
      
      /// \brief Creates a data rewriter as specified on the command line.
      /// \param data_spec A data specification
      /// \return A data rewriter
      data::rewriter create_rewriter(data::data_specification data_spec = data::default_data_specification())
      {
        return data::rewriter(data_spec, rewrite_strategy());
      }
  };

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_FILTER_TOOL_WITH_REWRITER_H
