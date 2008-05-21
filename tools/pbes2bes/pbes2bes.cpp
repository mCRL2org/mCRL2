// Author(s): Alexander van Dam
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes2bes.cpp
/// \brief Add your file description here.

#define NAME "pbes2bes"
#define AUTHOR "Alexander van Dam"

//C++
#include <cstdio>
#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <sstream>

//MCRL2-specific
#include "mcrl2/core/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/pbes2bes.h"
#include "mcrl2/pbes/detail/old_rewriter.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/utilities/command_line_interface.h" // after messaging.h and rewrite.h

using namespace std;
using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::pbes_system;
using namespace mcrl2::utilities;

//Type definitions
//----------------
struct t_tool_options {
	string opt_outputformat;
	string opt_strategy;
	string infilename;
	string outfilename;
};

bool process(t_tool_options tool_options)
{
  typedef data::rewriter data_rewriter;
  typedef pbes_system::detail::rewriter<data_rewriter> pbes_rewriter;

  pbes<> pbes_spec = load_pbes(tool_options.infilename);
  data_rewriter datar(pbes_spec.data());
  pbes_rewriter pbesr(datar, pbes_spec.data());

  if (!pbes_spec.is_well_typed())
  {
    core::gsErrorMsg("The PBES is not well formed. Pbes2bes cannot handle this kind of PBES's\nComputation aborted.\n");
  }

  if (!pbes_spec.is_closed())
  {
    core::gsErrorMsg("The PBES is not closed. Pbes2bes cannot handle this kind of PBES's\nComputation aborted.\n");
  }

  if (tool_options.opt_strategy == "finite")
  {
    pbes_spec = do_finite_algorithm(pbes_spec, pbesr);
  }
  else if (tool_options.opt_strategy == "lazy")
  {
    pbes_spec = do_lazy_algorithm(pbes_spec, pbesr);
  }
  save_pbes(pbes_spec, tool_options.outfilename, tool_options.opt_outputformat);

  return true;
}

// SQuADT protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <mcrl2/utilities/squadt_interface.h>

class squadt_interactor : public mcrl2::utilities::squadt::mcrl2_tool_interface {

  private:

    static const char*  pbes_file_for_input;  ///< file containing an LPS
    static const char*  pbes_file_for_output; ///< file used to write the output to

    enum pbes_output_format {
      binary,
      internal,
      cwi
    };

    enum transformation_strategy {
      lazy,
      finite
    };

    static const char* option_transformation_strategy;
    static const char* option_selected_output_format;

  private:

    boost::shared_ptr < tipi::datatype::enumeration > transformation_method_enumeration;
    boost::shared_ptr < tipi::datatype::enumeration > output_format_enumeration;

  public:

    /** \brief constructor */
    squadt_interactor();

    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities&) const;

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration&);

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const&) const;

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration&);
};

const char* squadt_interactor::pbes_file_for_input  = "pbes_in";
const char* squadt_interactor::pbes_file_for_output = "pbes_out";

const char* squadt_interactor::option_transformation_strategy = "transformation_strategy";
const char* squadt_interactor::option_selected_output_format  = "selected_output_format";

squadt_interactor::squadt_interactor() {
  transformation_method_enumeration.reset(new tipi::datatype::enumeration("lazy"));

  transformation_method_enumeration->add_value("finite");

  output_format_enumeration.reset(new tipi::datatype::enumeration("binary"));

  output_format_enumeration->add_value("internal");
  output_format_enumeration->add_value("cwi");
}

void squadt_interactor::set_capabilities(tipi::tool::capabilities& c) const {
  c.add_input_configuration(pbes_file_for_input, tipi::mime_type("pbes", tipi::mime_type::application), tipi::tool::category::transformation);
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& c) {
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::layout::elements;

  /* Create display */
  tipi::layout::tool_display d;

  // Helper for format selection
  mcrl2::utilities::squadt::radio_button_helper < pbes_output_format > format_selector(d);

  // Helper for strategy selection
  mcrl2::utilities::squadt::radio_button_helper < transformation_strategy > strategy_selector(d);

  layout::vertical_box& m = d.create< vertical_box >();

  m.append(d.create< label >().set_text("Output format : ")).
    append(d.create< horizontal_box >().
                append(format_selector.associate(binary, "binary")).
                append(format_selector.associate(internal, "internal")).
                append(format_selector.associate(cwi, "cwi")),
          margins(0,5,0,5)).
    append(d.create< label >().set_text("Transformation strategy : ")).
    append(strategy_selector.associate(lazy, "lazy: only boolean equations reachable from the initial state")).
    append(strategy_selector.associate(finite, "finite: all possible boolean equations"));

  button& okay_button = d.create< button >().set_label("OK");

  m.append(d.create< label >().set_text(" ")).
    append(okay_button, layout::right);

  /// Copy values from options specified in the configuration
  if (c.option_exists(option_transformation_strategy)) {
    strategy_selector.set_selection(static_cast < transformation_strategy > (
        c.get_option_argument< size_t >(option_transformation_strategy, 0)));
  }
  if (c.option_exists(option_selected_output_format)) {
    format_selector.set_selection(static_cast < pbes_output_format > (
        c.get_option_argument< size_t >(option_selected_output_format, 0)));
  }
  
  send_display_layout(d.set_manager(m));

  /* Wait until the ok button was pressed */
  okay_button.await_change();
  
  /* Add output file to the configuration */
  if (c.output_exists(pbes_file_for_output)) {
    tipi::object& output_file = c.get_output(pbes_file_for_output);
 
    output_file.set_location(c.get_output_name(".pbes"));
  }
  else {
    c.add_output(pbes_file_for_output, tipi::mime_type("pbes", tipi::mime_type::application), c.get_output_name(".pbes"));
  }

  c.add_option(option_transformation_strategy).append_argument(transformation_method_enumeration,
                                static_cast < transformation_strategy > (strategy_selector.get_selection()));
  c.add_option(option_selected_output_format).append_argument(output_format_enumeration,
                                static_cast < pbes_output_format > (format_selector.get_selection()));

  send_clear_display();
}

bool squadt_interactor::check_configuration(tipi::configuration const& c) const {
  bool result = true;

  result &= c.input_exists(pbes_file_for_input);
  result &= c.output_exists(pbes_file_for_output);
  result &= c.option_exists(option_transformation_strategy);
  result &= c.option_exists(option_selected_output_format);

  return (result);
}

bool squadt_interactor::perform_task(tipi::configuration& c) {
  static std::string strategies[] = { "lazy", "finite" };
  static std::string formats[]    = { "binary", "internal", "cwi" };

  t_tool_options tool_options;

  tool_options.opt_outputformat = formats[c.get_option_argument< size_t >(option_selected_output_format)];
  tool_options.opt_strategy     = strategies[c.get_option_argument< size_t >(option_transformation_strategy)];
  tool_options.infilename       = c.get_input(pbes_file_for_input).get_location();
  tool_options.outfilename      = c.get_output(pbes_file_for_output).get_location();

  bool result = process(tool_options);
 
  send_clear_display();

  return (result);
}
#endif

//function parse_command_line
//---------------------------
/// \brief Parse the command line options.
t_tool_options parse_command_line(int ac, char** av)
{
  interface_description clinterface(av[0], NAME, AUTHOR, "[OPTION]... [INFILE [OUTFILE]]\n"
      "Transform the PBES from INFILE into an equivalent BES and write it to OUTFILE. "
      "If INFILE is not present, stdin is used. If OUTFILE is not present, stdout is used.");

  clinterface.add_rewriting_options();

  clinterface.
    add_option("strategy",
      make_mandatory_argument("NAME"),
      "compute the BES using strategy NAME:\n"
      "  'lazy' for computing only boolean equations which can be reached from the initial state (default), or\n"
      "  'finite' for computing all possible boolean equations.",
      's').
    add_option("output",
      make_mandatory_argument("NAME"),
      "store the BES in output format NAME:\n"
      "  'binary' for the internal binary format (default),\n"
      "  'internal' for the internal textual format, or\n"
      "  'cwi' for the format used by the CWI to solve a BES.",
      'o');

  command_line_parser parser(clinterface, ac, av);

  t_tool_options tool_options;

  tool_options.opt_outputformat = "lazy";
  tool_options.opt_strategy     = "binary";
  tool_options.infilename       = "-";
  tool_options.outfilename      = "-";

  if (parser.options.count("output")) { // Output format
    std::string format = parser.option_argument("output");

    if (!((format == "binary") || (format == "internal") || (format == "cwi"))) {
      parser.error("unknown output format specified (got `" + format + "')");
    }

    tool_options.opt_outputformat = format;
  }

  if (parser.options.count("strategy")) { // Strategy
    std::string strategy = parser.option_argument("strategy");

    if (!((strategy == "lazy") || (strategy == "internal") || (strategy == "cwi"))) {
      parser.error("unknown output strategy specified (got `" + strategy + "')");
    }

    tool_options.opt_strategy = strategy;
  }

  if (2 < parser.arguments.size()) {
    parser.error("too many file arguments");
  }
  else {
    if (0 < parser.arguments.size()) {
      tool_options.infilename = parser.arguments[0];
    }
    if (1 < parser.arguments.size()) {
      tool_options.outfilename = parser.arguments[0];
    }
  }

  return tool_options;
}

//Main Program
//------------
/// \brief Main program for pbes2bes
int main(int argc, char** argv)
{
  MCRL2_ATERM_INIT(argc, argv)

  try {
#ifdef ENABLE_SQUADT_CONNECTIVITY
    if (mcrl2::utilities::squadt::interactor< squadt_interactor >::free_activation(argc, argv)) {
      return EXIT_SUCCESS;
    }
#endif

    process(parse_command_line(argc, argv));

    return EXIT_SUCCESS;
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return EXIT_FAILURE;
}
