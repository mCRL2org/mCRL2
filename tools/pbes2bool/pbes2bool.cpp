// Author(s): Jan Friso Groote // Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes2bool.cpp
/// \brief Add your file description here.

#include "boost.hpp" // precompiled headers

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
#include <ostream>
#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <utility>

#include <sstream>

//LPS-Framework
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/utility.h"
#include "mcrl2/data/rewrite.h"
#include "mcrl2/data/data_operators.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/pbes/pbes2bool.h"

//ATERM-specific
#include "mcrl2/atermpp/substitute.h"
#include "mcrl2/atermpp/utility.h"
#include "mcrl2/atermpp/indexed_set.h"
#include "mcrl2/atermpp/table.h"
#include "_aterm.h"

//MCRL2-specific
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/command_line_messaging.h"
#include "mcrl2/utilities/command_line_rewriting.h"

//Tool-specific
// #include "pbes_rewrite_jfg.h"
// #include "sort_functions.h"
#include "bes.h"

using namespace std;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::lps;
using bes::bes_expression;

using atermpp::make_substitution;

//Function declarations used by main program
//------------------------------------------
static bool parse_command_line(int argc, char** argv, t_tool_options&);
//Post: The command line options are parsed.
//      The program has aborted with a suitable error code, if:
//    - errors were encounterd
//    - non-standard behaviour was requested (help, version)
//Ret:  The parsed command line options

// SQuADT protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <mcrl2/utilities/mcrl2_squadt_interface.h>

class squadt_interactor : public mcrl2::utilities::squadt::mcrl2_tool_interface {

  private:

    enum bes_output_format {
      none,
      vasy,
      cwi
    };

    static bool initialise_types() {
      tipi::datatype::enumeration< transformation_strategy > transformation_strategy_enumeration;

      transformation_strategy_enumeration.
        add(lazy, "lazy").
        add(optimize, "optimize").
        add(on_the_fly, "on-the-fly").
        add(on_the_fly_with_fixed_points, "on-the-fly-with-fixed-points");

      tipi::datatype::enumeration< bes_output_format > output_format_enumeration;

      output_format_enumeration.
        add(none, "none").
        add(vasy, "vasy").
        add(cwi, "cwi");

      return true;
    }

  public:

    /** \brief constructor */
    squadt_interactor() {
      static bool initialised = initialise_types();

      static_cast< void > (initialised); // harmless, and prevents unused variable warnings
    }

    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities&) const;

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration&);

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const&) const;

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration&);
};

const char* pbes_file_for_input             = "pbes_in";
const char* bes_file_for_output             = "bes_out";
const char* counter_example_file_for_output = "counter_example";

const char* option_transformation_strategy = "transformation_strategy";
const char* option_selected_output_format  = "selected_output_format";
const char* option_rewrite_strategy        = "rewrite_strategy";
const char* option_precompile              = "precompile";
const char* option_counter                 = "counter";
const char* option_hash_table              = "hash_table";
const char* option_tree                    = "tree";
const char* option_unused_data             = "unused_data";

void squadt_interactor::set_capabilities(tipi::tool::capabilities& c) const {
  c.add_input_configuration(pbes_file_for_input, tipi::mime_type("pbes", tipi::mime_type::application), tipi::tool::category::transformation);
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& c) {
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::layout::elements;

  if (!c.option_exists(option_precompile)) {
    c.add_option(option_precompile).set_argument_value< 0 >(false);
  }
  if (!c.option_exists(option_counter)) {
    c.add_option(option_counter).set_argument_value< 0 >(false);
  }
  if (!c.option_exists(option_hash_table)) {
    c.add_option(option_hash_table).set_argument_value< 0 >(false);
  }
  if (!c.option_exists(option_tree)) {
    c.add_option(option_tree).set_argument_value< 0 >(false);
  }
  if (!c.option_exists(option_unused_data)) {
    c.add_option(option_unused_data).set_argument_value< 0 >(true);
  }
  if (!c.option_exists(option_rewrite_strategy)) {
    c.add_option(option_rewrite_strategy).set_argument_value< 0 >(GS_REWR_JITTY);
  }
  if (!c.option_exists(option_transformation_strategy)) {
    c.add_option(option_transformation_strategy).set_argument_value< 0 >(lazy);
  }
  if (!c.option_exists(option_selected_output_format)) {
    c.add_option(option_selected_output_format).set_argument_value< 0 >(none);
  }

  /* Create display */
  tipi::tool_display d;

  // Helper for format selection
  mcrl2::utilities::squadt::radio_button_helper < bes_output_format > format_selector(d);

  // Helper for strategy selection
  mcrl2::utilities::squadt::radio_button_helper < RewriteStrategy > rewrite_strategy_selector(d);

  // Helper for strategy selection
  mcrl2::utilities::squadt::radio_button_helper < transformation_strategy > strategy_selector(d);

  layout::vertical_box& m = d.create< vertical_box >().set_default_margins(margins(0,5,0,5));

  checkbox& precompile(d.create< checkbox >().set_status(c.get_option_argument< bool >(option_precompile)));
  checkbox& counter(d.create< checkbox >().set_status(c.get_option_argument< bool >(option_counter)));
  checkbox& hash_table(d.create< checkbox >().set_status(c.get_option_argument< bool >(option_hash_table)));
  checkbox& tree(d.create< checkbox >().set_status(c.get_option_argument< bool >(option_tree)));
  checkbox& unused_data(d.create< checkbox >().set_status(c.get_option_argument< bool >(option_unused_data)));

  m.append(d.create< label >().set_text("Rewrite strategy")).
    append(d.create< horizontal_box >().
                append(rewrite_strategy_selector.associate(GS_REWR_INNER, "Inner")).
#ifdef MCRL2_INNERC_AVAILABLE
                append(rewrite_strategy_selector.associate(GS_REWR_INNERC, "Innerc")).
#endif
#ifdef MCRL2_JITTYC_AVAILABLE
                append(rewrite_strategy_selector.associate(GS_REWR_JITTY, "Jitty")).
                append(rewrite_strategy_selector.associate(GS_REWR_JITTYC, "Jittyc")),
#else
                append(rewrite_strategy_selector.associate(GS_REWR_JITTY, "Jitty")),
#endif
          margins(0,5,0,5)).
    append(d.create< label >().set_text("Output format : ")).
    append(d.create< horizontal_box >().
                append(format_selector.associate(none, "none")).
                append(format_selector.associate(vasy, "vasy")).
                append(format_selector.associate(cwi, "cwi")),
          margins(0,5,0,5)).
    append(d.create< label >().set_text("Strategy to generate a BES from a PBES: "), margins(8,5,0,5)).
    append(d.create< vertical_box >().
        append(strategy_selector.associate(lazy, "0: without optimisation")).
        append(strategy_selector.associate(optimize, "1: forward substitution of true/false")).
        append(strategy_selector.associate(on_the_fly, "2: full substitution of true/false")).
        append(strategy_selector.associate(on_the_fly_with_fixed_points, "3: full substitution and cycle detection")),
          margins(0,5,8,5)).
    append(d.create< horizontal_box >().
        append(d.create< vertical_box >().
            append(precompile.set_label("precompile for faster rewriting")).
            append(counter.set_label("produce a counter example")).
            append(unused_data.set_label("remove unused data"))).
        append(d.create< vertical_box >().
            append(hash_table.set_label("use hash tables and translation to BDDs")).
            append(tree.set_label("store state in a tree (memory efficiency)"))));

  button& okay_button = d.create< button >().set_label("OK");

  m.append(d.create< label >().set_text(" ")).
    append(okay_button, layout::right);

  /// Copy values from options specified in the configuration
  if (c.option_exists(option_transformation_strategy)) {
    strategy_selector.set_selection(
        c.get_option_argument< transformation_strategy >(option_transformation_strategy, 0));
  }
  if (c.option_exists(option_selected_output_format)) {
    format_selector.set_selection(
        c.get_option_argument< bes_output_format >(option_selected_output_format, 0));
  }
  if (c.option_exists(option_rewrite_strategy)) {
    rewrite_strategy_selector.set_selection(
        c.get_option_argument< RewriteStrategy >(option_rewrite_strategy, 0));
  }

  send_display_layout(d.manager(m));

  /* Wait until the ok button was pressed */
  okay_button.await_change();

  c.get_option(option_transformation_strategy).set_argument_value< 0 >(strategy_selector.get_selection());
  c.get_option(option_selected_output_format).set_argument_value< 0 >(format_selector.get_selection());
  c.get_option(option_rewrite_strategy).set_argument_value< 0 >(rewrite_strategy_selector.get_selection());

  if (c.get_option_argument< bes_output_format >(option_selected_output_format) != none)
  {
    /* Add output file to the configuration */
    if (c.output_exists(bes_file_for_output)) {
      tipi::configuration::object& output_file = c.get_output(bes_file_for_output);

      output_file.location(c.get_output_name(".txt"));
    }
    else {
      c.add_output(bes_file_for_output, tipi::mime_type("txt", tipi::mime_type::application),
                   c.get_output_name(".txt"));
    }
  }

  c.get_option(option_precompile).set_argument_value< 0, boolean >(precompile.get_status());
  c.get_option(option_counter).set_argument_value< 0, boolean >(counter.get_status());
  c.get_option(option_hash_table).set_argument_value< 0, boolean >(hash_table.get_status());
  c.get_option(option_tree).set_argument_value< 0, boolean >(tree.get_status());
  c.get_option(option_unused_data).set_argument_value< 0, boolean >(unused_data.get_status());

  send_clear_display();
}

bool squadt_interactor::check_configuration(tipi::configuration const& c) const {
  bool result = true;

  result &= c.input_exists(pbes_file_for_input);
  result &= c.option_exists(option_transformation_strategy);
  result &= c.option_exists(option_selected_output_format);
  if (result && (c.get_option_argument< bes_output_format >(option_selected_output_format)!=none))
  { /* only check for the existence of an outputfile if the output format does
       not equal none */
    result &= c.output_exists(bes_file_for_output);
  }

  return (result);
}

bool squadt_interactor::perform_task(tipi::configuration& c) {
  // static std::string strategies[] = { "lazy", "fly" };
  static std::string formats[]    = { "none", "vasy", "cwi" };

  t_tool_options tool_options;

  tool_options.opt_precompile_pbes           = c.get_option_argument< bool >(option_precompile);
  tool_options.opt_construct_counter_example = c.get_option_argument< bool >(option_counter);
  tool_options.opt_store_as_tree             = c.get_option_argument< bool >(option_tree);
  tool_options.opt_data_elm                  = c.get_option_argument< bool >(option_unused_data);;
  tool_options.opt_use_hashtables            = c.get_option_argument< bool >(option_hash_table);;
  tool_options.rewrite_strategy              = c.get_option_argument< RewriteStrategy >(option_rewrite_strategy, 0);

  if (tool_options.opt_construct_counter_example && !c.output_exists(counter_example_file_for_output)) {
    tool_options.opt_counter_example_file = c.get_output_name(".txt").c_str();

    c.add_output(counter_example_file_for_output, tipi::mime_type("txt", tipi::mime_type::text),
                 tool_options.opt_counter_example_file);
  }

  tool_options.opt_outputformat = formats[c.get_option_argument< bes_output_format >(option_selected_output_format)];

  tool_options.opt_strategy = c.get_option_argument< transformation_strategy >(option_transformation_strategy, 0);

  tool_options.infilename       = c.get_input(pbes_file_for_input).location();

  if (c.output_exists(bes_file_for_output)) {
    tool_options.outfilename = c.get_output(bes_file_for_output).location();
  }

  send_clear_display();

  process(tool_options);

  return true;
}
#endif


//function parse_command_line
//---------------------------
bool parse_command_line(int ac, char** av, t_tool_options& tool_options)
{
  interface_description clinterface(av[0], NAME, AUTHOR, "[OPTION]... [INFILE [OUTFILE]]\n",
      "Solves PBES from INFILE, or writes an equivalent BES to OUTFILE. If INFILE is "
      "not present, stdin is used. If OUTFILE is not present, stdout is used.");

  clinterface.add_rewriting_options();

  clinterface.
    add_option("strategy", make_mandatory_argument("STRAT"),
      "use strategy STRAT (default '0');\n"
      " 0) Compute all boolean equations which can be reached"
      " from the initial state, without optimization"
      " (default). This is is the most data efficient"
      " option per generated equation.\n"
      " 1) Optimize by immediately substituting the right"
      " hand sides for already investigated variables"
      " that are true or false when generating an"
      " expression. This is as memory efficient as 0.\n"
      " 2) In addition to 1, also substitute variables that"
      " are true or false into an already generated right"
      " hand side. This can mean that certain variables"
      " become unreachable (e.g. X0 in X0 && X1, when X1"
      " becomes false, assuming X0 does not occur"
      " elsewhere. It will be maintained which variables"
      " have become unreachable as these do not have to be"
      " investigated. Depending on the PBES, this can"
      " reduce the size of the generated BES substantially"
      " but requires a larger memory footprint.\n"
      " 3) In addition to 2, investigate for generated"
      " variables whether they occur on a loop, such that"
      " they can be set to true or false, depending on the"
      " fixed point symbol. This can increase the time"
      " needed to generate an equation substantially",
      's').
    add_option("counter",
      "print at the end a tree labelled with instantiations "
      "of the left hand side of equations; this tree is an "
      "indication of how pbes2bool came to the validity or "
      "invalidity of the PBES",
      'c').
    add_option("precompile",
      "precompile the pbes for faster rewriting; does not "
      "work when the toolset is compiled in debug mode",
      'p').
    add_option("hashtables",
      "use hashtables when substituting in bes equations, "
      "and translate internal expressions to binary decision "
      "diagrams (discouraged, due to performance)",
      'H').
    add_option("output",
      make_mandatory_argument("FORMAT"),
      "use output format FORMAT:\n"
      " 'none' (default),\n"
      " 'vasy',\n"
      " 'cwi'",
      'o').
    add_option("tree",
      "store state in a tree (for memory efficiency)",
      't').
    add_option("unused_data",
      "do not remove unused parts of the data specification",
      'u');

  command_line_parser parser(clinterface, ac, av);

  if (parser.continue_execution()) {
    tool_options.opt_precompile_pbes           = 0 < parser.options.count("precompile");
    tool_options.opt_use_hashtables            = 0 < parser.options.count("hashtables");
    tool_options.opt_construct_counter_example = 0 < parser.options.count("counter");
    tool_options.opt_store_as_tree             = 0 < parser.options.count("tree");
    tool_options.opt_data_elm                  = parser.options.count("unused-data") == 0;
    tool_options.opt_outputformat              = "none";
    tool_options.opt_strategy                  = lazy;
    tool_options.infilename                    = "";
    tool_options.outfilename                   = "";

    if (parser.options.count("output")) { // Output format
      std::string format = parser.option_argument("output");

      if (!((format == "none") || (format == "vasy") || (format == "cwi"))) {
        parser.error("unknown output format specified (got `" + format + "')");
      }

      tool_options.opt_outputformat = format;
    }

    if (parser.options.count("strategy")) { // Bes solving strategy (currently only one available)
      int strategy = parser.option_argument_as< int >("strategy");

      switch (strategy) {
        case 0:
         tool_options.opt_strategy = lazy;
         break;
        case 1:
         tool_options.opt_strategy = optimize;
         break;
        case 2:
         tool_options.opt_strategy = on_the_fly;
         break;
        case 3:
         tool_options.opt_strategy = on_the_fly_with_fixed_points;
         break;
        default:
          parser.error("unknown strategy specified: available strategies are '0', '1', '2', and '3'");
      }
    }

    if (2 < parser.arguments.size()) {
      parser.error("too many file arguments");
    }
    else {
      if (0 < parser.arguments.size()) {
        tool_options.infilename = parser.arguments[0];
      }
      if (1 < parser.arguments.size()) {
        tool_options.outfilename = parser.arguments[1];
      }
    }

    tool_options.rewrite_strategy = parser.option_argument_as< RewriteStrategy >("rewriter");
  }

  return parser.continue_execution();
}

//Main Program
//------------
int main(int argc, char** argv)
{
  MCRL2_ATERM_INIT(argc, argv)

  try {
#ifdef ENABLE_SQUADT_CONNECTIVITY
    if (mcrl2::utilities::squadt::interactor< squadt_interactor >::free_activation(argc, argv)) {
      return EXIT_SUCCESS;
    }
#endif

    t_tool_options options;

    if (parse_command_line(argc, argv, options)) {
      process(options);
    }
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

