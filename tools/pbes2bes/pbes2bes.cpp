// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes2bes.cpp
/// \brief Transform a pbes into a bes

// ======================================================================
//
// file          : pbes2bes
// date          : 15-04-2007
// version       : 0.1.3
//
// author(s)     : Alexander van Dam <avandam@damdonk.nl>
//                 Jan Friso Groote <J.F.Groote@tue.nl>
//
// ======================================================================


#define NAME "pbes2bes"
#define AUTHOR "Jan Friso Groote"

//C++
#include <iostream>
#include <string>
#include <utility>

#include <sstream>

//Tool framework
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/pbes_output_tool.h"
#include "mcrl2/utilities/pbes_input_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/utilities/pbes_rewriter_tool.h"
#include "mcrl2/utilities/execution_timer.h"

//Data Framework
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/selection.h"
#include "mcrl2/data/data_equation.h"

//Boolean equation systems
#include "mcrl2/pbes/normalize.h"
#include "mcrl2/bes/bes_deprecated.h"
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/bes2pbes.h"
#include "mcrl2/bes/io.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/detail/instantiate_global_variables.h"

using namespace std;
using namespace mcrl2::log;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using bes::bes_expression;
using namespace ::bes;

// using atermpp::make_substitution;

//Function declarations used by main program
//------------------------------------------

using namespace mcrl2;
using utilities::tools::input_output_tool;
using utilities::tools::bes_output_tool;
using data::tools::rewriter_tool;
using utilities::tools::pbes_rewriter_tool;
using namespace mcrl2::utilities::tools;

class pbes2bes_tool: public rewriter_tool<pbes_input_tool<bes_output_tool<input_output_tool> > >
{
  protected:
    // Tool options.
    /// The output file name
    ::bes::transformation_strategy opt_transformation_strategy; // The strategy to propagate true/false.
    ::bes::search_strategy opt_search_strategy; // The search strategy (breadth or depth first).
    bool opt_use_hashtables;                   // The hashtable option
    remove_level opt_erase_unused_bes_variables;       // Remove bes variables whenever they are not used anymore.
    bool opt_data_elm;                         // The data elimination option

    typedef rewriter_tool<pbes_input_tool<bes_output_tool<input_output_tool> > > super;

    pbes_system::pbes_rewriter_type default_rewriter() const
    {
      return pbes_system::quantifier_all;
    }

  public:
    pbes2bes_tool()
      : super(
        NAME,
        AUTHOR,
        "Generate a BES from a PBES. ",
        "Reads the PBES from INFILE and writes an equivalent BES to OUTFILE. "
        "If INFILE is not present, stdin is used. If OUTFILE is not present, stdout is used."),
      opt_transformation_strategy(::bes::lazy),
      opt_search_strategy(::bes::breadth_first),
      opt_use_hashtables(false),
      opt_erase_unused_bes_variables(none),
      opt_data_elm(true)
    {}


  protected:
    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);

      input_output_tool::parse_options(parser);

      opt_use_hashtables            = 0 < parser.options.count("hashtables");
      opt_erase_unused_bes_variables= parser.option_argument_as<remove_level>("erase");
      opt_data_elm                  = parser.options.count("unused-data") == 0;
      opt_transformation_strategy   = parser.option_argument_as<transformation_strategy>("strategy");
      opt_search_strategy           = parser.option_argument_as<search_strategy>("search");

    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.
      add_option("strategy", make_enum_argument<transformation_strategy>("STRAT")
                 .add_value(lazy, true)
                 .add_value(optimize)
                 .add_value(on_the_fly)
                 .add_value(on_the_fly_with_fixed_points),
                 "use substitution strategy STRAT:",
                 's').
      add_option("search", make_enum_argument<search_strategy>("SEARCH")
                 .add_value(breadth_first, true)
                 .add_value(depth_first)
                 .add_value(breadth_first_short)
                 .add_value(depth_first_short),
                 "use search strategy SEARCH:",
                 'z').
      add_option("erase", make_enum_argument<remove_level>("LEVEL")
                 .add_value(none, true)
                 .add_value(some)
                 .add_value(all),
                 "use remove level LEVEL to remove bes variables",
                 'e').
      add_option("hashtables",
                 "use hashtables when substituting in bes equations, "
                 "and translate internal expressions to binary decision "
                 "diagrams (discouraged, due to performance)",
                 'H').
      add_option("unused_data",
                 "do not remove unused parts of the data specification",
                 'u');
    }

  public:
    bool run()
    {
      using namespace pbes_system;
      using namespace utilities;

      mCRL2log(verbose) << "pbes2bes parameters:" << std::endl;
      mCRL2log(verbose) << "  input file:            " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  output file:           " << m_output_filename << std::endl;
      mCRL2log(verbose) << "  data rewriter:         " << m_rewrite_strategy << std::endl;
      mCRL2log(verbose) << "  substitution strategy: " << opt_transformation_strategy << std::endl;
      mCRL2log(verbose) << "  search strategy:       " << opt_search_strategy << std::endl;
      mCRL2log(verbose) << "  erase level:           " << opt_erase_unused_bes_variables << std::endl;

      // load the pbes
      mcrl2::pbes_system::pbes p;
      load_pbes(p, input_filename(), pbes_input_format());

      pbes_system::normalize(p);
      pbes_system::detail::instantiate_global_variables(p);
      // data rewriter

      data::rewriter datar= (opt_data_elm) ?
                            data::rewriter(p.data(), mcrl2::data::used_data_equation_selector(p.data(), pbes_system::find_function_symbols(p), p.global_variables()), rewrite_strategy()) :
                            data::rewriter(p.data(), rewrite_strategy());

      timer().start("instantiation");
      ::bes::boolean_equation_system bes_equations=
        ::bes::boolean_equation_system(
          p,
          datar,
          opt_transformation_strategy,
          opt_search_strategy,
          false,  // No counter example
          opt_erase_unused_bes_variables,
          opt_use_hashtables);
      timer().finish("instantiation");

      mcrl2::bes::boolean_equation_system b(convert_to_bes(bes_equations));
      mcrl2::bes::save_bes(b, output_filename(), bes_output_format());

      return true;
    }

};

int main(int argc, char* argv[])
{
  return pbes2bes_tool().execute(argc, argv);
}
