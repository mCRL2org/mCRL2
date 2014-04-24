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

using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::pbes_system;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

class pbes_stategraph_tool: public rewriter_tool<input_output_tool>
{
  typedef rewriter_tool<input_output_tool> super;

  protected:
    pbesstategraph_options options;

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      options.rewrite_strategy = rewrite_strategy();
      options.simplify = parser.option_argument_as<bool>("simplify");
      options.apply_to_original = parser.option_argument_as<bool>("apply-to-original");
      options.use_local_variant = parser.option_argument_as<bool>("use-local-variant");
      options.print_influence_graph = parser.option_argument_as<bool>("print-influence-graph");
      options.cache_marking_updates = parser.option_argument_as<bool>("cache-marking-updates");
      options.use_marking_optimization = parser.option_argument_as<bool>("use-marking-optimization");
      options.use_alternative_lcfp_criterion = parser.option_argument_as<bool>("use-alternative-lcfp-criterion");
      options.use_alternative_gcfp_relation = parser.option_argument_as<bool>("use-alternative-gcfp-relation");
      options.use_alternative_gcfp_consistency = parser.option_argument_as<bool>("use-alternative-gcfp-consistency");
      options.timer = &timer();
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("simplify", make_optional_argument("NAME", "1"), "simplify the PBES during reduction", 's');
      desc.add_option("apply-to-original", make_optional_argument("NAME", "0"), "apply reduction on the original PBES", 'a');
      desc.add_option("use-local-variant", make_optional_argument("NAME", "0"), "use the local variant of the algorithm", 'l');
      desc.add_option("print-influence-graph", make_optional_argument("NAME", "0"), "print the influence graph", 'i');
      desc.add_option("cache-marking-updates", make_optional_argument("NAME", "0"), "cache rewriter calls in marking updates", 'c');
      desc.add_option("use-marking-optimization", make_optional_argument("NAME", "0"), "apply an optimization during marking", 'm');
      desc.add_option("use-alternative-lcfp-criterion", make_optional_argument("NAME", "0"), "use an alternative criterion for local control flow parameter computation", 'x');
      desc.add_option("use-alternative-gcfp-relation", make_optional_argument("NAME", "0"), "use an alternative global control flow parameter relation", 'y');
      desc.add_option("use-alternative-gcfp-consistency", make_optional_argument("NAME", "0"), "use an alternative global control flow parameter consistency", 'z');
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
      mCRL2log(verbose) << "  apply to original:                " << std::boolalpha << options.apply_to_original << std::endl;
      mCRL2log(verbose) << "  use local variant:                " << std::boolalpha << options.use_local_variant << std::endl;
      mCRL2log(verbose) << "  print influence graph:            " << std::boolalpha << options.print_influence_graph << std::endl;
      mCRL2log(verbose) << "  cache marking updates:            " << std::boolalpha << options.cache_marking_updates << std::endl;
      mCRL2log(verbose) << "  use marking optimization:         " << std::boolalpha << options.use_marking_optimization << std::endl;
      mCRL2log(verbose) << "  use alternative lcfp criterion:   " << std::boolalpha << options.use_alternative_lcfp_criterion << std::endl;
      mCRL2log(verbose) << "  use alternative gcfp relation:    " << std::boolalpha << options.use_alternative_gcfp_relation << std::endl;
      mCRL2log(verbose) << "  use alternative gcfp consistency: " << std::boolalpha << options.use_alternative_gcfp_consistency << std::endl;

      pbesstategraph(input_filename(), output_filename(), options);
      return true;
    }
};

int main(int argc, char* argv[])
{
  return pbes_stategraph_tool().execute(argc, argv);
}

