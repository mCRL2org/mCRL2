// Author(s): Wieger Wesselink, Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesstategraph.cpp

#include "mcrl2/pbes/tools.h"
#include "mcrl2/pbes/tools/pbesstategraph_options.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/pbes_input_tool.h"
#include "mcrl2/utilities/pbes_output_tool.h"

using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::pbes_system;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

class pbes_stategraph_tool: public pbes_input_tool<pbes_output_tool<rewriter_tool<input_output_tool> > >
{
  typedef pbes_input_tool<pbes_output_tool<rewriter_tool<input_output_tool> > > super;

  protected:
    pbesstategraph_options options;

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      options.rewrite_strategy = rewrite_strategy();
      options.simplify = 0 == parser.options.count("no-simplify");
      options.use_global_variant = 0 < parser.options.count("use-global-variant");
      options.print_influence_graph = 0 < parser.options.count("print-influence-graph");
      options.cache_marking_updates = 0 == parser.options.count("disable-cache-marking-updates");
      options.marking_algorithm = parser.option_argument_as<int>("marking-algorithm");
      if (options.marking_algorithm < 0 || options.marking_algorithm > 2)
      {
        throw mcrl2::runtime_error("invalid value for marking-algorithm!");
      }
      options.use_alternative_lcfp_criterion = 0 < parser.options.count("use-alternative-lcfp-criterion");
      options.use_alternative_gcfp_relation = 0 < parser.options.count("use-alternative-gcfp-relation");
      options.use_alternative_gcfp_consistency = 0 < parser.options.count("use-alternative-gcfp-consistency");
      options.timer = &timer();
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_hidden_option("no-simplify", "do not simplify the PBES during reduction (works only in combination with -g)", 's');
      desc.add_option("use-global-variant", "use the global variant of the algorithm", 'g');
      desc.add_hidden_option("print-influence-graph", "print the influence graph", 'I');
      desc.add_hidden_option("disable-cache-marking-updates", "disable caching of rewriter calls in marking updates", 'c');
      desc.add_option("marking-algorithm", make_optional_argument("NAME", "0"), "specifies the algorithm that is used for the marking computation 0 (default), 1 or 2. In certain cases this choice can have a significant impact on the performance.", 'm');
      desc.add_hidden_option("use-alternative-lcfp-criterion", "use an alternative criterion for local control flow parameter computation", 'x');
      desc.add_hidden_option("use-alternative-gcfp-relation", "use an alternative global control flow parameter relation", 'y');
      desc.add_hidden_option("use-alternative-gcfp-consistency", "use an alternative global control flow parameter consistency", 'z');
    }

  public:
    pbes_stategraph_tool()
      : super(
        "pbesstategraph",
        "Wieger Wesselink; Jeroen Keiren",
        "reduces a PBES",
        "Reads a file containing a PBES, and reduces it based on an analysis of control flow parameters."
        "If OUTFILE is not present, standard output is used. If INFILE is not present, standard input is used."
      )
    {}

    bool run()
    {
      mCRL2log(verbose) << "pbesstategraph parameters:" << std::endl;
      mCRL2log(verbose) << "  input file:                       " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  output file:                      " << m_output_filename << std::endl;
      mCRL2log(verbose) << "  simplify:                         " << std::boolalpha << options.simplify << std::endl;
      mCRL2log(verbose) << "  use global variant:               " << std::boolalpha << options.use_global_variant << std::endl;
      mCRL2log(verbose) << "  print influence graph:            " << std::boolalpha << options.print_influence_graph << std::endl;
      mCRL2log(verbose) << "  cache marking updates:            " << std::boolalpha << options.cache_marking_updates << std::endl;
      mCRL2log(verbose) << "  margking algorithm:               " << options.marking_algorithm << std::endl;
      mCRL2log(verbose) << "  use alternative lcfp criterion:   " << std::boolalpha << options.use_alternative_lcfp_criterion << std::endl;
      mCRL2log(verbose) << "  use alternative gcfp relation:    " << std::boolalpha << options.use_alternative_gcfp_relation << std::endl;
      mCRL2log(verbose) << "  use alternative gcfp consistency: " << std::boolalpha << options.use_alternative_gcfp_consistency << std::endl;

      pbesstategraph(input_filename(),
                     output_filename(),
                     pbes_input_format(),
                     pbes_output_format(),
                     options);
      return true;
    }
};

int main(int argc, char* argv[])
{
  return pbes_stategraph_tool().execute(argc, argv);
}

