// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbeschain.cpp

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/pbes/pbes_rewriter_tool.h"
#include "mcrl2/pbes/pbes_input_tool.h"
#include "mcrl2/pbes/pbes_output_tool.h"
#include "mcrl2/pbes/tools/pbeschain.h"

using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::pbes_system;
using namespace mcrl2::core;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using pbes_system::tools::pbes_input_tool;
using pbes_system::tools::pbes_output_tool;
using pbes_system::tools::pbes_rewriter_tool;
using data::tools::rewriter_tool;

class pbeschain_tool: public pbes_input_tool<pbes_output_tool<pbes_rewriter_tool<rewriter_tool<input_output_tool>>>>
{
  protected:
    using super = pbes_input_tool<pbes_output_tool<pbes_rewriter_tool<rewriter_tool<input_output_tool>>>>;

    pbeschain_options m_options;

    void parse_options(const command_line_parser& parser) override
    {
      super::parse_options(parser);
      m_options.back_substitution = !parser.has_option("no-back-substitution");
      m_options.max_depth = parser.option_argument_as<int>("max-depth");
      m_options.count_unique_pvi = parser.has_option("count-unique-pvi");
      m_options.fill_pvi = parser.has_option("fill-pvi");
      m_options.timeout = parser.option_argument_as<double>("timeout");
      m_options.pvi_pp_factor = parser.option_argument_as<double>("pvi-pp-factor");
      m_options.quantifier_free = parser.has_option("quantifier-free");
      m_options.avoid_alternating = parser.has_option("avoid-alternating");
      m_options.srf_factor = parser.option_argument_as<double>("srf-factor");
    }

    void add_options(interface_description& desc) override
    {
      super::add_options(desc);
      desc.add_option("no-back-substitution",
                  "Do not substitute the solution to an equation backwards if the number "
                  "of predicate variable instances in the equation is zero. "
                  "In some cases, this makes solving the equation faster. However, "
                  "less paths can be reduced.", 's');
      desc.add_option("max-depth", utilities::make_optional_argument("DEPTH", "12"),
                  "The maximum depth a single predicate variable instances "
                  "gets unfolded. Default is 12.", 'm');
      desc.add_option("count-unique-pvi",
                  "An unfolding is considered less complex if the number of "
                  "*unique* predicate variable instances is no more than one.",
                  'u');
      desc.add_option("fill-pvi",
                  "Use the guard of a pvi to fill the pvi with concrete values.",
                  'f');
      desc.add_option("timeout", utilities::make_optional_argument("SECONDS", "0.0"),
                  "Set a timeout in seconds for the substitution process per equation. "
                  "0 means no timeout.", 't');
      desc.add_option("pvi-pp-factor", utilities::make_optional_argument("FACTOR", "0.0"),
                  "Set a factor of the maximum size the chained predicate formula should be after chaining compared to the size of the original PVI. "
                  "0 means no limit.");
      desc.add_option("quantifier-free",
                  "Do not chain if the unfolded formula contains any quantifier.");
      desc.add_option("avoid-alternating",
                  "Do not chain if the unfolded formula contains a PVI with a different name than the current equation. So X(a) -> Y(b) is not allowed.");
      desc.add_option("srf-factor", utilities::make_optional_argument("FACTOR", "0.0"),
                  "Set a factor of the maximum size the chained equation in SRF should be after chaining compared to the size of the original equation. "
                  "0 means no limit. Default is 1.2.");
    }

  public:
    pbeschain_tool()
      : super(
        "pbeschain",
        "Jore Booy",
        "Simplify a pbes by backwards chaining unfolded predicate variable instances.",
        "Reads a file containing a PBES. If OUTFILE "
        "is not present, standard output is used. If INFILE is not present, standard input is used."
      )
    {}

    bool run() override
    {
      mCRL2log(verbose) << "pbeschain parameters:" << std::endl;
      mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  output file:        " << m_output_filename << std::endl;
      mCRL2log(verbose) << "  timeout per eq:     " << m_options.timeout << " seconds" << std::endl;

      m_options.rewrite_strategy = rewrite_strategy();
      pbeschain(input_filename(),
                  output_filename(),
                  pbes_input_format(),
                  pbes_output_format(),
                  m_options
                 );

      return true;
    }

};

int main(int argc, char* argv[])
{
  return pbeschain_tool().execute(argc, argv);
}
