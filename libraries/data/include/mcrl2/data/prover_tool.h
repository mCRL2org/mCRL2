// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/prover_tool.h
/// \brief Base class for tools that use a data rewriter.

#ifndef MCRL2_DATA_PROVER_TOOL_H
#define MCRL2_DATA_PROVER_TOOL_H

#include "mcrl2/data/detail/prover/solver_type.h"
#include "mcrl2/utilities/command_line_interface.h"

namespace mcrl2
{

namespace data
{

namespace tools
{

/// \brief Base class for tools that use a rewriter.
template <typename Tool>
class prover_tool: public Tool
{
  protected:
    typedef mcrl2::data::detail::smt_solver_type smt_solver_type;

    /// The data rewriter strategy
    smt_solver_type m_solver_type;

    /// \brief Add options to an interface description. Also includes
    /// rewriter options.
    /// \param desc An interface description
    void add_options(utilities::interface_description& desc)
    {
      Tool::add_options(desc);

      desc.add_option("smt-solver", utilities::make_enum_argument< smt_solver_type >("SOLVER")
                      .add_value(data::detail::solver_type_cvc),
                      "use SOLVER to remove inconsistent paths from the internally used "
                      "BDDs (by default, no path elimination is applied):",
                      'z');
    }

    /// \brief Parse non-standard options
    /// \param parser A command line parser
    void parse_options(const utilities::command_line_parser& parser)
    {
      Tool::parse_options(parser);

      if (0 < parser.options.count("smt-solver"))
      {
        m_solver_type = parser.option_argument_as< smt_solver_type >("smt-solver");
      }
    }

  public:

    /// \brief Constructor.
    prover_tool(const std::string& name,
                const std::string& author,
                const std::string& what_is,
                const std::string& tool_description,
                std::string known_issues = ""
               )
      : Tool(name, author, what_is, tool_description, known_issues),
        m_solver_type(mcrl2::data::detail::solver_type_cvc)
    {}

    /// \brief Returns the rewrite strategy
    /// \return The rewrite strategy
    smt_solver_type solver_type() const
    {
      return m_solver_type;
    }
};

} // namespace tools

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_PROVER_TOOL_H
