// Author(s): Jan Friso Groote // Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes2bool.cpp
/// \brief Add your file description here.

// ======================================================================
//
// file          : pbes2bool
// date          : 15-04-2007
// version       : 0.1.3
//
// author(s)     : Alexander van Dam <avandam@damdonk.nl>
//                 Jan Friso Groote <J.F.Groote@tue.nl>
//
// ======================================================================


#define NAME "pbes2bool"
#define AUTHOR "Jan Friso Groote"

//C++
#include <iostream>
#include <string>
#include <utility>

#include <sstream>

//Tool framework
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/utilities/pbes_input_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/pbes_rewriter_tool.h"
#include "mcrl2/utilities/execution_timer.h"

//Data Framework
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/selection.h"
#include "mcrl2/data/data_equation.h"

//Boolean equation systems
#include "mcrl2/pbes/normalize.h"
#include "mcrl2/bes/io.h"
#include "mcrl2/bes/bes_deprecated.h"
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/bes2pbes.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/detail/instantiate_global_variables.h"

using namespace std;
using namespace ::bes;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using bes::bes_expression;

//Function declarations used by main program
//------------------------------------------

using namespace mcrl2;
using namespace mcrl2::log;
using utilities::tools::input_tool;
using utilities::tools::rewriter_tool;
using utilities::tools::pbes_rewriter_tool;
using namespace mcrl2::utilities::tools;

class pbes2bool_tool: public rewriter_tool<pbes_input_tool<input_tool> > 
{
  protected:
    // Tool options.
    ::bes::transformation_strategy opt_transformation_strategy; // The transformation strategy
    ::bes::search_strategy opt_search_strategy;                 // The search strategy
    bool opt_use_hashtables;                   // The hashtable option
    bool opt_construct_counter_example;        // The counter example option
    remove_level opt_erase_unused_bes_variables;       // Remove unused bes variables if true
    bool opt_store_as_tree;                    // The tree storage option
    bool opt_data_elm;                         // The data elimination option
    std::string opt_counter_example_file;      // The counter example file name

    typedef rewriter_tool<pbes_input_tool<input_tool> > super;

    pbes_system::pbes_rewriter_type default_rewriter() const
    {
      return pbes_system::quantifier_all;
    }

    // Overload synopsis to cope with optional OUTFILE
    std::string synopsis() const
    {
      return "[OPTION]...[INFILE]\n";
    }

  public:
    pbes2bool_tool()
      : super(
        NAME,
        AUTHOR,
        "Generate a BES from a PBES and solve it. ",
        "Solves (P)BES from INFILE. "
        "If INFILE is not present, stdin is used. "),
      opt_transformation_strategy(::bes::lazy),
      opt_search_strategy(::bes::breadth_first),
      opt_use_hashtables(false),
      opt_construct_counter_example(false),
      opt_erase_unused_bes_variables(none),
      opt_store_as_tree(false),
      opt_data_elm(true),
      opt_counter_example_file("")
    {}

  protected:
    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);

      input_tool::parse_options(parser);

      opt_use_hashtables            = 0 < parser.options.count("hashtables");
      opt_construct_counter_example = 0 < parser.options.count("counter");
      opt_erase_unused_bes_variables= parser.option_argument_as<remove_level>("erase");
      opt_store_as_tree             = 0 < parser.options.count("tree");
      opt_data_elm                  = parser.options.count("unused-data") == 0;
      opt_transformation_strategy   = parser.option_argument_as<transformation_strategy>("strategy");
      opt_search_strategy           = parser.option_argument_as<search_strategy>("search");

      if (parser.options.count("output")) // Output format is deprecated.
      {
        throw parser.error("the option --output or -o is deprecated. Use the tool pbes2bes for this functionality. ");
      }

      if (opt_construct_counter_example && opt_erase_unused_bes_variables!=none)
      {
        throw parser.error("generating a counter example cannot be combined with erasing bes variables. ");
      }
   
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.
      add_option("strategy", make_enum_argument<transformation_strategy>("STRAT")
                 .add_value(::lazy, true)
                 .add_value(::optimize)
                 .add_value(::on_the_fly)
                 .add_value(::on_the_fly_with_fixed_points),
                 "use substitution strategy STRAT:",
                 's').
     add_option("search", make_enum_argument<search_strategy>("SEARCH")
                 .add_value(breadth_first, true)
                 .add_value(depth_first)
                 .add_value(breadth_first_short)
                 .add_value(depth_first_short),
                 "use search strategy SEARCH:",
                 'z').
      add_option("counter",
                 "print at the end a tree labelled with instantiations "
                 "of the left hand side of equations; this tree is an "
                 "indication of how pbes2bool came to the validity or "
                 "invalidity of the PBES",
                 'c').
      add_option("hashtables",
                 "use hashtables when substituting in bes equations, "
                 "and translate internal expressions to binary decision "
                 "diagrams (discouraged, due to performance)",
                 'H').
      add_option("erase", make_enum_argument<remove_level>("LEVEL")
                 .add_value(none, true)
                 .add_value(some)
                 .add_value(all),
                 "use remove level LEVEL to remove bes variables",
                 'e').
      add_option("output",
                 make_mandatory_argument("FORMAT"),
                 "use output format FORMAT (this option is deprecated. Use the tool pbes2bes instead).\n",
                 'o').
      add_option("tree",
                 "store state in a tree (for memory efficiency)",
                 't').
      add_option("unused_data",
                 "do not remove unused parts of the data specification",
                 'u');
    }

  public:
    bool run()
    {
      using namespace pbes_system;
      using namespace utilities;

      mCRL2log(verbose) << "pbes2bool parameters:" << std::endl;
      mCRL2log(verbose) << "  input file:            " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  data rewriter:         " << m_rewrite_strategy << std::endl;
      mCRL2log(verbose) << "  substitution strategy: " << opt_transformation_strategy << std::endl;
      mCRL2log(verbose) << "  search strategy:       " << opt_search_strategy << std::endl;
      mCRL2log(verbose) << "  erase level:           " << opt_erase_unused_bes_variables << std::endl;

      // load the pbes
      mcrl2::pbes_system::pbes p;
      mcrl2::bes::load_pbes(p, input_filename(), pbes_input_format());

      pbes_system::normalize(p);
      pbes_system::detail::instantiate_global_variables(p);
      // data rewriter

      data::rewriter datar;
      if (opt_data_elm)
      {
        // Create a rewriter with only the necessary data equations.
        using namespace mcrl2::data;
        std::set < data::function_symbol > eqn_symbol_set=pbes_system::find_function_symbols(p.equations());
        std::set < data::function_symbol > init_symbol_set=pbes_system::find_function_symbols(p.initial_state());
        std::set < data::function_symbol > function_symbol_set;
        set_union(eqn_symbol_set.begin(),eqn_symbol_set.end(),
                  init_symbol_set.begin(),init_symbol_set.end(),
                  inserter(function_symbol_set,function_symbol_set.begin()));
        datar=data::rewriter(p.data(),
                             mcrl2::data::used_data_equation_selector( p.data(), function_symbol_set, p.global_variables()),
                             rewrite_strategy());
      }
      else
      {
        // Create a rewriter with all data equations. This is safer, but increases the time and memory to
        // compile the rewrite system.
        data::rewriter(p.data(), rewrite_strategy());
      }


      timer().start("instantiation");
      ::bes::boolean_equation_system bes_equations=
        ::bes::boolean_equation_system(
          p,
          datar,
          opt_transformation_strategy,
          opt_search_strategy,
          opt_store_as_tree,
          opt_construct_counter_example,
          opt_erase_unused_bes_variables,
          opt_use_hashtables);
      timer().finish("instantiation");

      timer().start("solving");
      bool result=solve_bes(bes_equations,
                            opt_use_hashtables,
                            opt_construct_counter_example);
      timer().finish("solving");

      mCRL2log(verbose) << "The solution for the initial variable of the pbes is " <<
                      (result ? "true" : "false") << std::endl;
      std::cout << (result ? "true" : "false") << std::endl;

      if (opt_construct_counter_example)
      {
        bes_equations.print_counter_example(opt_counter_example_file,result);
      }
      return true;
    }

};

int main(int argc, char* argv[])
{
  return pbes2bool_tool().execute(argc, argv);
}
