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
#include "mcrl2/bes/pbes_output_tool.h"
#include "mcrl2/bes/pbes_input_tool.h"
#include "mcrl2/bes/pbes_rewriter_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/execution_timer.h"

//Data Framework
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/selection.h"
#include "mcrl2/data/data_equation.h"

//Parameterised boolean equation systems
#include "mcrl2/pbes/normalize.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/detail/instantiate_global_variables.h"
#include "mcrl2/pbes/pbesinst_alternative_lazy_algorithm.h"
#include "mcrl2/pbes/transformation_strategy.h"
#include "mcrl2/pbes/search_strategy.h"

//Boolean equation systems
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/bes2pbes.h"
#include "mcrl2/bes/io.h"
#include "mcrl2/bes/remove_level.h"

using namespace std;
using namespace mcrl2::log;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::bes;
using namespace mcrl2::pbes_system;


//Function declarations used by main program
//------------------------------------------

using mcrl2::bes::tools::pbes_input_tool;
using mcrl2::bes::tools::pbes_output_tool;
using mcrl2::bes::tools::pbes_rewriter_tool;
using mcrl2::bes::tools::bes_output_tool;
using mcrl2::data::tools::rewriter_tool;
using mcrl2::utilities::tools::input_output_tool;

class pbes2bes_tool: public rewriter_tool<pbes_input_tool<bes_output_tool<input_output_tool> > >
{
  protected:
    // Tool options.
    /// The output file name
    transformation_strategy m_transformation_strategy; // The strategy to propagate true/false.
    search_strategy m_search_strategy; // The search strategy (breadth or depth first).
    bool m_use_hashtables;                   // The hashtable option
    remove_level m_erase_unused_bes_variables;       // Remove bes variables whenever they are not used anymore.
    bool m_data_elm;                         // The data elimination option

    typedef rewriter_tool<pbes_input_tool<bes_output_tool<input_output_tool> > > super;

    pbes_rewriter_type default_rewriter() const
    {
      return quantifier_all;
    }

  public:
    pbes2bes_tool()
      : super(
        NAME,
        AUTHOR,
        "Generate a BES from a PBES. ",
        "Reads the PBES from INFILE and writes an equivalent BES to OUTFILE. "
        "If INFILE is not present, stdin is used. If OUTFILE is not present, stdout is used."),
      m_transformation_strategy(mcrl2::pbes_system::lazy),
      m_search_strategy(mcrl2::pbes_system::breadth_first),
      m_use_hashtables(false),
      m_erase_unused_bes_variables(none),
      m_data_elm(true)
    {}


  protected:
    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);

      input_output_tool::parse_options(parser);

      m_use_hashtables            = 0 < parser.options.count("hashtables");
      m_erase_unused_bes_variables= parser.option_argument_as<remove_level>("erase");
      m_data_elm                  = parser.options.count("unused-data") == 0;
      m_transformation_strategy   = parser.option_argument_as<transformation_strategy>("strategy");
      m_search_strategy           = parser.option_argument_as<search_strategy>("search");

    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.
      add_option("strategy", make_enum_argument<transformation_strategy>("STRAT")
                 .add_value(mcrl2::pbes_system::lazy, true)
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
      using namespace mcrl2::data;
      using namespace mcrl2::pbes_system;
      mCRL2log(verbose) << "pbes2bes parameters:" << std::endl;
      mCRL2log(verbose) << "  input file:            " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  output file:           " << m_output_filename << std::endl;
      mCRL2log(verbose) << "  data rewriter:         " << m_rewrite_strategy << std::endl;
      mCRL2log(verbose) << "  substitution strategy: " << m_transformation_strategy << std::endl;
      mCRL2log(verbose) << "  search strategy:       " << m_search_strategy << std::endl;
      mCRL2log(verbose) << "  erase level:           " << m_erase_unused_bes_variables << std::endl;

      // load the pbes
      pbes p;
      mcrl2::pbes_system::load_pbes(p, input_filename(), pbes_input_format());

      normalize(p);
      mcrl2::pbes_system::detail::instantiate_global_variables(p);
  
      // data rewriter
      std::set < function_symbol > eqn_symbol_set=mcrl2::pbes_system::find_function_symbols(p.equations());
      std::set < function_symbol > init_symbol_set=mcrl2::pbes_system::find_function_symbols(p.initial_state());
      std::set < function_symbol > function_symbol_set;
      set_union(eqn_symbol_set.begin(),eqn_symbol_set.end(),
                  init_symbol_set.begin(),init_symbol_set.end(),
                  inserter(function_symbol_set,function_symbol_set.begin()));
      rewriter datar=(m_data_elm) ?
                        rewriter(p.data(),
                             used_data_equation_selector( p.data(), function_symbol_set, p.global_variables()),
                             m_rewrite_strategy) :
                        rewriter(p.data(), rewrite_strategy());


      timer().start("instantiation");

      pbesinst_alternative_lazy_algorithm algorithm(p.data(), datar, m_search_strategy, m_transformation_strategy);
      algorithm.run(p);
      boolean_equation_system bes = pbesinst_conversion(algorithm.get_result());

      timer().finish("instantiation");

      mcrl2::bes::save_bes(bes, output_filename(), bes_output_format());

      return true;
    }

};

int main(int argc, char* argv[])
{
  return pbes2bes_tool().execute(argc, argv);
}
