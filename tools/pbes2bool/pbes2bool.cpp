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
// author(s)     : Jan Friso Groote <J.F.Groote@tue.nl>
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
#include "mcrl2/bes/pbes_rewriter_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/bes/pbes_input_tool.h"
#include "mcrl2/utilities/execution_timer.h"

//Data Framework
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/selection.h"
#include "mcrl2/data/data_equation.h"

// Parameterised boolean equation systems
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/detail/instantiate_global_variables.h"
#include "mcrl2/pbes/pbesinst_alternative_lazy_algorithm.h"
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/transformation_strategy.h"
#include "mcrl2/pbes/search_strategy.h"

//Boolean equation systems
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/bes2pbes.h"
#include "mcrl2/bes/remove_level.h"
#include "mcrl2/bes/solution_strategy.h"
#include "mcrl2/bes/small_progress_measures.h"
#include "mcrl2/bes/local_fixpoints.h"
#include "mcrl2/bes/pbesinst_conversion.h"
#include "mcrl2/bes/justification.h"

using namespace std;
using namespace mcrl2::utilities;
using namespace mcrl2::core;

//Function declarations used by main program
//------------------------------------------

using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::bes;
using namespace mcrl2::pbes_system;
using namespace mcrl2::utilities::tools;
using mcrl2::bes::tools::pbes_input_tool;
using mcrl2::bes::tools::pbes_rewriter_tool;
using mcrl2::data::tools::rewriter_tool;
using mcrl2::utilities::tools::input_tool;

class pbes2bool_tool: public rewriter_tool<pbes_input_tool<input_tool> >
{
  protected:
    // Tool options.
    transformation_strategy m_transformation_strategy; // The strategy to substitute the value of variables with 
                                                       // a trivial rhs (aka true or false) in other equations when generating a BES.
    search_strategy m_search_strategy;                 // The search strategy (depth first/breadth first)
    solution_strategy_t m_solution_strategy;           // Indicates the solver used to solve the generated BES.
    bool m_construct_counter_example;                  // The counter example option
    mcrl2::bes::remove_level m_erase_unused_bes_variables;       // Remove unused bes variables if true
    bool m_data_elm;                                   // The data elimination option
    size_t m_maximal_todo_size;                        // The maximal size of the todo queue when generating a bes
    bool m_approximate_true;                           // If approximate_true holds, rhs's of variables that cannot
                                                       // be put in the todo queue are set to false, assuring that
                                                       // a true answer is correct, but false might be incorrect.
                                                       // If approximate_true is false, true is used, meaning that
                                                       // the answer false is correct, and true might be incorrect.


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
      m_transformation_strategy(lazy),
      m_search_strategy(breadth_first),
      m_solution_strategy(local_fixed_point),
      m_construct_counter_example(false),
      m_erase_unused_bes_variables(mcrl2::bes::none),
      m_data_elm(true),
      m_maximal_todo_size(atermpp::npos),
      m_approximate_true(true)
    {}

  protected:
    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);

      input_tool::parse_options(parser);

      m_construct_counter_example = 0 < parser.options.count("counter");
      m_erase_unused_bes_variables= parser.option_argument_as<mcrl2::bes::remove_level>("erase");
      m_data_elm                  = parser.options.count("unused-data") == 0;
      m_transformation_strategy   = parser.option_argument_as<mcrl2::pbes_system::transformation_strategy>("strategy");
      m_solution_strategy         = parser.option_argument_as<mcrl2::bes::solution_strategy_t>("solver");
      m_search_strategy           = parser.option_argument_as<mcrl2::pbes_system::search_strategy>("search");
      if (parser.options.count("todo-max"))
      {
        m_maximal_todo_size         = parser.option_argument_as< unsigned long >("todo-max");
      }
      m_approximate_true          = 0 == parser.options.count("approximate-false");

      if (parser.options.count("output")) // Output format is deprecated.
      {
        throw parser.error("The option --output or -o is deprecated. Use the tool pbes2bes for this functionality. ");
      }

      if (m_construct_counter_example && m_erase_unused_bes_variables!=mcrl2::bes::none)
      {
        throw parser.error("Generating a counter example cannot be combined with erasing bes variables. ");
      }

      if (m_construct_counter_example && m_solution_strategy!=local_fixed_point)
      {
        throw parser.error("Generating a counter example only works with the local fixed point algorithm to solve the boolean equation system. ");
      }
    }

    void add_options(interface_description& desc)
    {
      using namespace mcrl2::bes;
      super::add_options(desc);
      desc.
      add_option("strategy", make_enum_argument<transformation_strategy>("STRAT")
                 .add_value(lazy, true)
                 .add_value(optimize)
                 .add_value(on_the_fly)
                 .add_value(on_the_fly_with_fixed_points),
                 "use substitution strategy STRAT:",
                 's').
     add_option("search", 
                 make_enum_argument<search_strategy>("SEARCH")
                   .add_value(breadth_first, true)
                   .add_value(depth_first)
                   .add_value(breadth_first_short)
                   .add_value(depth_first_short),
                 "use search strategy SEARCH:",
                 'z').
      add_option("counter",
                 "print evidence why the formula is valid or invalid. If invalid "
                 "this information can be used to construct a counterexample using the tool "
                 "lpsxsim. The evidence consists of a tree labelled with instantiations "
                 "of the left hand side of PBES equations. These instantiations largely match the states "
                 "of the lps. An indentation corresponds in general with one (sometimes zero, sometimes more) "
                 "transition. ",
                 'c').
      add_option("erase", 
                 make_enum_argument<remove_level>("LEVEL")
                   .add_value(none, true)
                   .add_value(some)
                   .add_value(all),
                 "use remove level LEVEL to remove bes variables",
                 'e').
      add_option("output",
                 make_mandatory_argument("FORMAT"),
                 "use output format FORMAT (this option is deprecated. Use the tool pbes2bes instead).\n",
                 'o').
      add_option("todo-max", make_mandatory_argument("NUM"),
                 "limit the number of boolean variables that can reside in the todo buffer. If the todo "
                 "stack is full, a random element in the buffer is removed and the rhs of this variable is "
                 "set to false (or true if --approximate-false is used). In this case the result true is reliable, "
                 "and a result false could mean that the formula is valid (or vice versa, if --approximate-false is used). "
                 "By replacing the variables in the queue at random, the breadth/depth-first structure is mixed up. ").
      add_option("approximate-false",
                 "If set, variables that are removed from the todo buffer are set to true. This means that the result "
                 "false is reliable, and the result true can still mean that the formula is false. Without this flag "
                 "true is appromated, meaning that true is the reliable answer, and false is not. ").
      add_option("unused_data",
                 "do not remove unused parts of the data specification. ",
                 'u').
      add_option("solver",
                 make_enum_argument<solution_strategy_t>("STRATEGY")
                    .add_value(local_fixed_point, true)
                    .add_value(small_progr_measures),
                 "This flag selects the solver using which the generated bes is solved.",
                 'g');
    }

  public:
    bool run()
    {
      using namespace pbes_system;
      using namespace utilities;

      mCRL2log(verbose) << "pbes2bool parameters:" << std::endl;
      mCRL2log(verbose) << "  input file:            " << (input_filename().empty()?"standard input":input_filename()) << std::endl;
      mCRL2log(verbose) << "  data rewriter:         " << m_rewrite_strategy << std::endl;
      mCRL2log(verbose) << "  substitution strategy: " << m_transformation_strategy << std::endl;
      mCRL2log(verbose) << "  search strategy:       " << m_search_strategy << std::endl;
      mCRL2log(verbose) << "  solution strategy      " << m_solution_strategy << "" << std::endl;
      mCRL2log(verbose) << "  erase level:           " << m_erase_unused_bes_variables << std::endl;
      if (m_maximal_todo_size!=atermpp::npos)
      {
        mCRL2log(verbose) << "  limit the todo buffer to " << m_maximal_todo_size << " bes variables and replace removed variables by " <<
               (m_approximate_true?"false":"true") << std::endl;
      }

      // load the pbes
      mcrl2::pbes_system::pbes p;
      mcrl2::bes::load_pbes(p, input_filename(), pbes_input_format());

      pbes_system::algorithms::normalize(p);
      pbes_system::detail::instantiate_global_variables(p);
      
      // data rewriter
      data::rewriter datar;
      if (m_data_elm)
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
                             m_rewrite_strategy);
      }
      else
      {
        // Create a rewriter with all data equations. This is safer, but increases the time and memory to
        // compile the rewrite system.
        datar=data::rewriter(p.data(), m_rewrite_strategy);
      }

      timer().start("instantiation");

      pbesinst_alternative_lazy_algorithm algorithm(p.data(), datar, m_search_strategy, m_transformation_strategy);
      algorithm.run(p);
      p=algorithm.get_result(!m_construct_counter_example);
      boolean_equation_system bes = pbesinst_conversion(p);
      
      timer().finish("instantiation");

      bool result = false;
      std::vector<bool> full_solution;

      timer().start("solving");
      switch (m_solution_strategy)
      {
        case gauss:
          throw mcrl2::runtime_error("Plain gauss elimination is not supported. Use the local fixpoints algorithm instead."); 
        case small_progr_measures:
          result = small_progress_measures(bes);
          break;
        case local_fixed_point:
          result = local_fixpoints(bes, &full_solution);
          break;
      }
      timer().finish("solving");

      mCRL2log(verbose) << "The solution for the initial variable of the pbes is ";
      mCRL2log(info) << (result ? "true" : "false") << std::endl;

      if (m_construct_counter_example)
      {
        print_justification_tree(bes, full_solution, result); 
      }

      return true;
    }

};

int main(int argc, char* argv[])
{
  return pbes2bool_tool().execute(argc, argv);
}
