// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/rewriter_tool.h
/// \brief Base class for tools that use a data rewriter.

#ifndef MCRL2_UTILITIES_REWRITER_TOOL_H
#define MCRL2_UTILITIES_REWRITER_TOOL_H

#include <iostream>
#include "mcrl2/data/rewriter.h"
#include "mcrl2/utilities/command_line_interface.h"

namespace mcrl2 {

namespace utilities {

  /// standard conversion from stream
  inline std::istream& operator>>(std::istream& is, data::rewriter::strategy& s) {
    char strategy[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    using namespace mcrl2::data::detail;

    is.readsome(strategy, 9);

    size_t new_s = static_cast< size_t >(RewriteStrategyFromString(strategy));

    s = static_cast< data::rewriter::strategy >(new_s);

    if (static_cast< size_t >(new_s) == static_cast< size_t >(GS_REWR_INVALID)) {
      is.setstate(std::ios_base::failbit);
    }

    return is;
  }

  /// standard conversion to stream
  inline std::ostream& operator<<(std::ostream& os, data::rewriter::strategy& s) {
    static char const* strategies[] = {
      "inner",
#ifdef MCRL2_INNERC_AVAILABLE
      "innerc",
#endif
      "jitty",
#ifdef MCRL2_JITTYC_AVAILABLE
      "jittyc",
#endif
      "innerp",
#ifdef MCRL2_JITTYC_AVAILABLE
      "innerpc",
#endif
#ifdef MCRL2_JITTYC_AVAILABLE
      "jittyp"
#else
      "jittyp",
      "jittypc"
#endif
    };

    os << strategies[s];

    return os;
  }

namespace tools {

  /// \brief Base class for tools that use a rewriter.
  template <typename Tool>
  class rewriter_tool: public Tool
  {
    protected:
      /// The data rewriter strategy
      mcrl2::data::rewriter::strategy m_rewrite_strategy;

      /// \brief Add options to an interface description. Also includes
      /// rewriter options.
      /// \param desc An interface description
      void add_options(interface_description& desc)
      {
        Tool::add_options(desc);

        desc.add_option(
          "rewriter", make_mandatory_argument<data::rewriter::strategy>("NAME", "jitty"),
          "use rewrite strategy NAME:\n"
          "  'jitty' for jitty rewriting (default),\n"
          "  'jittyp' for jitty rewriting with prover,\n"
#ifdef MCRL2_JITTYC_AVAILABLE
          "  'jittyc' for compiled jitty rewriting,\n"
#endif
          "  'inner' for innermost rewriting,\n"
          "  'innerp' for innermost rewriting with prover, or\n"
#ifdef MCRL2_INNERC_AVAILABLE
          "  'innerc' for compiled innermost rewriting"
#endif
          ,'r'
        );
      }

      /// \brief Parse non-standard options
      /// \param parser A command line parser
      void parse_options(const command_line_parser& parser)
      {
        Tool::parse_options(parser);
        m_rewrite_strategy = parser.option_argument_as< mcrl2::data::rewriter::strategy >("rewriter");
      }

    public:

      /// \brief Constructor.
      rewriter_tool(const std::string& name,
                    const std::string& author,
                    const std::string& what_is,
                    const std::string& tool_description
                   )
        : Tool(name, author, what_is, tool_description),
          m_rewrite_strategy(mcrl2::data::rewriter::jitty)
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
      data::rewriter create_rewriter(data::data_specification const& data_spec = data::data_specification())
      {
        return data::rewriter(data_spec, rewrite_strategy());
      }
  };

} // namespace tools

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_REWRITER_TOOL_H
