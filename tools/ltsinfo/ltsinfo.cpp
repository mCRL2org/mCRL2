// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltsinfo.cpp

#define NAME "ltsinfo"
#define AUTHOR "Muck van Weerdenburg"

#include <string>

#include <boost/lexical_cast.hpp>

#include "aterm2.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/lts/lts.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/utilities/command_line_interface.h" // after messaging.h, rewrite.h and bdd_path_eliminator.h

//Temporary workaround for the passing of the determinism

using namespace mcrl2::utilities;
using namespace mcrl2::core;

struct tool_options {
  std::string                 infilename;
  mcrl2::lts::lts_type        intype;
  mcrl2::lts::lts_equivalence determinism_equivalence;
};

// Squadt protocol interface and utility pseudo-library
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <mcrl2/utilities/mcrl2_squadt_interface.h>

class squadt_interactor : public mcrl2::utilities::squadt::mcrl2_tool_interface {

  private:

    /** \brief compiles a tool_options instance from a configuration */
    bool extract_task_options(tipi::configuration const& c, tool_options&) const;

    static bool initialise_types() {
      using namespace mcrl2::lts;

      tipi::datatype::enumeration< lts_equivalence > determinism_equivalence_enumeration;

      determinism_equivalence_enumeration.
        add(lts_eq_none, "none").
        add(lts_eq_bisim, "bisimilarity").
        add(lts_eq_branching_bisim, "branching-bisimilarity").
        add(lts_eq_sim, "strong-simulation").
        add(lts_eq_trace, "trace").
        add(lts_eq_weak_trace, "weak-trace").
        add(lts_eq_isomorph, "isomorphism");

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

const char* lts_file_for_input  = "lts_in";
const char* option_determinism_equivalence = "determinism_equivalence";

void squadt_interactor::set_capabilities(tipi::tool::capabilities& c) const {
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("aut", tipi::mime_type::text), tipi::tool::category::reporting);
#ifdef USE_BCG
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("bcg", tipi::mime_type::application), tipi::tool::category::reporting);
#endif
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("svc", tipi::mime_type::application), tipi::tool::category::reporting);
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("fsm", tipi::mime_type::text), tipi::tool::category::reporting);
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("dot", tipi::mime_type::text), tipi::tool::category::reporting);
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("mcrl2-lts", tipi::mime_type::application), tipi::tool::category::reporting);
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("svc+mcrl", tipi::mime_type::application), tipi::tool::category::reporting);
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& c) {
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::layout::elements;

  std::string infilename = c.get_input(lts_file_for_input).location();

  /* Create display */
  tipi::tool_display d;

  // Helper for linearisation method selection
  mcrl2::utilities::squadt::radio_button_helper< mcrl2::lts::lts_equivalence > determinism_selector(d);

  layout::vertical_box& m = d.create< vertical_box >().set_default_margins(margins(0,5,0,5));

  m.append(d.create< label >().set_text("Deterministic check:")).
    append(d.create< horizontal_box >().
        append(determinism_selector.associate(mcrl2::lts::lts_eq_none, "None")).
        append(determinism_selector.associate(mcrl2::lts::lts_eq_isomorph, "Isomorphism", true)).
        append(determinism_selector.associate(mcrl2::lts::lts_eq_bisim, "Strong bisimilarity")).
        append(determinism_selector.associate(mcrl2::lts::lts_eq_branching_bisim, "Branching bisimilarity")));

  // Add okay button
  button& okay_button = d.create< button >().set_label("OK");

  m.append(d.create< label >().set_text(" ")).
    append(okay_button, layout::right);

  // Set default values for options if the configuration specifies them
  if (c.option_exists(option_determinism_equivalence)) {
    determinism_selector.set_selection(
        c.get_option_argument< mcrl2::lts::lts_equivalence >(option_determinism_equivalence, 0));
  }

  // Display
  send_display_layout(d.manager(m));

  /* Wait for the OK button to be pressed */
  okay_button.await_change();

  c.add_option(option_determinism_equivalence). set_argument_value< 0 >(determinism_selector.get_selection());

  send_clear_display();
}

bool squadt_interactor::check_configuration(tipi::configuration const& c) const {
  bool result = true;

  result |= c.input_exists(lts_file_for_input);
  result |= c.option_exists(option_determinism_equivalence);

  return (result);
}

bool squadt_interactor::extract_task_options(tipi::configuration const& c, tool_options& task_options) const {
  bool result = true;

  task_options.determinism_equivalence = c.get_option_argument< mcrl2::lts::lts_equivalence >(option_determinism_equivalence, 0);

  return (result);
}

bool squadt_interactor::perform_task(tipi::configuration& c) {
  using mcrl2::lts::lts;
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::layout::elements;

  tipi::configuration::object& input_object = c.get_input(lts_file_for_input);

  lts l;
  mcrl2::lts::lts_type t = lts::parse_format(input_object.type().sub_type());

  tool_options task_options;

  // Extract configuration
  extract_task_options(c, task_options);

  if (l.read_from(input_object.location(), t)) {
    /* Create and add the top layout manager */
    tipi::tool_display d;

    layout::horizontal_box& m = d.create< horizontal_box >();

    std::string deterministic("-");

    if(task_options.determinism_equivalence != mcrl2::lts::lts_eq_none) {
      l.reduce(task_options.determinism_equivalence);

      deterministic = std::string(l.is_deterministic() ? "yes" : "no") + ", modulo " +
                                 lts::name_of_equivalence(task_options.determinism_equivalence);
    }

    m.append(d.create< vertical_box >().set_default_alignment(layout::left).
                append(d.create< label >().set_text("States (#):")).
                append(d.create< label >().set_text("Labels (#):")).
                append(d.create< label >().set_text("Transitions (#):")).
                append(d.create< label >().set_text("")).
                append(d.create< label >().set_text("State information:")).
                append(d.create< label >().set_text("Label information:")).
                append(d.create< label >().set_text("Deterministic:")).
                append(d.create< label >().set_text("")).
                append(d.create< label >().set_text("Created by:")),
             margins(0,5,0,5));

    /* Second column */
    m.append(d.create< vertical_box >().set_default_alignment(layout::right).
                append(d.create< label >().set_text(boost::lexical_cast < std::string > (l.num_states()))).
                append(d.create< label >().set_text(boost::lexical_cast < std::string > (l.num_labels()))).
                append(d.create< label >().set_text(boost::lexical_cast < std::string > (l.num_transitions()))).
                append(d.create< label >().set_text("")).
                append(d.create< label >().set_text(l.has_state_info() ? "present" : "not present")).
                append(d.create< label >().set_text(l.has_label_info() ? "present" : "not present")).
                append(d.create< label >().set_text(deterministic)).
                append(d.create< label >().set_text("")).
                append(d.create< label >().set_text(l.get_creator())),
             margins(0,5,0,5));

    layout::vertical_box& n = d.create< vertical_box >();

    n.append(m).
        append(d.create< label >().
             set_text("Input read from " + input_object.location() + " (" + lts::string_for_type(t) + " format)"),
                        margins(5,0,5,20));

    gsVerboseMsg("checking reachability...\n");
    if (!l.reachability_check()) {
        n.append(d.create< label >().set_text("Warning: some states are not reachable from the initial state!")).
          append(d.create< label >().set_text("(This might result in unspecified behaviour of LTS tools.)"));
    }

    send_display_layout(d.manager(n));
  }
  else {
    send_error("Could not read `" + c.get_input(lts_file_for_input).location() + "', corruption or incorrect format?\n");

    return (false);
  }

  return (true);
}
#endif

tool_options parse_command_line(int argc, char** argv) {
  using namespace mcrl2::lts;
  using mcrl2::lts::lts;

  interface_description clinterface(argv[0], NAME, AUTHOR, "[OPTION]... [INFILE]\n",
    "Print information about the labelled transition system (LTS) in INFILE. "
    "If INFILE is not supplied, stdin is used.\n"
    "\n"
    "The format of INFILE is determined by its contents. "
    "The option --in can be used to force the format for INFILE. "
    "The supported formats are:\n"
    "  'aut' for the Aldebaran format (CADP),\n"
#ifdef USE_BCG
    "  'bcg' for the Binary Coded Graph format (CADP),\n"
#endif
    "  'dot' for the GraphViz format,\n"
    "  'fsm' for the Finite State Machine format,\n"
    "  'mcrl' for the mCRL SVC format,\n"
    "  'mcrl2' for the mCRL2 format (default), or"
    "  'svc' for the (generic) SVC format\n"
  );

  clinterface.
    add_option("equivalence", make_mandatory_argument("NAME"),
      "use equivalence NAME for deterministic check:\n"
      "  '" + lts::string_for_equivalence(lts_eq_isomorph) + "' for " 
            + lts::name_of_equivalence(lts_eq_isomorph) + " (default),\n"
      "  '" + lts::string_for_equivalence(lts_eq_bisim) + "' for " 
            + lts::name_of_equivalence(lts_eq_bisim) + ",\n"
      "  '" + lts::string_for_equivalence(lts_eq_branching_bisim) + "' for " 
            + lts::name_of_equivalence(lts_eq_branching_bisim) + ", or\n"
      "  'none' for not performing the check at all",
      'e').
    add_option("in", make_mandatory_argument("FORMAT"),
      "use FORMAT as the input format", 'i');

  command_line_parser parser(clinterface, argc, argv);

  tool_options opts = { "", mcrl2::lts::lts_none, mcrl2::lts::lts_eq_isomorph };

  if (parser.options.count("equivalence")) {
    opts.determinism_equivalence = lts::parse_equivalence(parser.option_argument("equivalence"));
    if (opts.determinism_equivalence != lts_eq_isomorph &&
        opts.determinism_equivalence != lts_eq_bisim &&
        opts.determinism_equivalence != lts_eq_branching_bisim &&
        (opts.determinism_equivalence != lts_eq_none ||
        parser.option_argument("equivalence") == "none"))
    {
      parser.error("option -e/--equivalence has illegal argument '" +
          parser.option_argument("equivalence") + "'");
    }
  }

  if (0 < parser.arguments.size()) {
    opts.infilename = parser.arguments[0];
  }
  if (1 < parser.arguments.size()) {
    parser.error("too many file arguments");
  }

  if (parser.options.count("in")) {
    if (1 < parser.options.count("in")) {
      parser.error("multiple input formats specified; can only use one");
    }

    opts.intype = lts::parse_format(parser.option_argument("in"));
    if (opts.intype == lts_none || opts.intype == lts_dot)  {
      parser.error("option -i/--in has illegal argument '" +
        parser.option_argument("in") + "'");
    }
  }

  return opts;
}

void process(tool_options const& opts) {
  using namespace mcrl2::lts;

  mcrl2::lts::lts l;

  if (opts.infilename.empty()) {
    gsVerboseMsg("reading LTS from stdin...\n");

    if ( !l.read_from(std::cin, opts.intype) ) {
      throw mcrl2::runtime_error("cannot read LTS from stdin\nretry with -v/--verbose for more information");
    }
  }
  else {
    gsVerboseMsg("reading LTS from '%s'...\n",opts.infilename.c_str());

    if (!l.read_from(opts.infilename,opts.intype)) {
      throw mcrl2::runtime_error("cannot read LTS from file '" + opts.infilename +
                                             "'\nretry with -v/--verbose for more information");
    }
  }

  std::cout << "LTS format: " << lts::string_for_type(l.get_type()) << std::endl
       << "Number of states: " << l.num_states() << std::endl
       << "Number of labels: " << l.num_labels() << std::endl
       << "Number of transitions: " << l.num_transitions() << std::endl;

  if ( l.has_state_info() )
  {
    std::cout << "Has state information." << std::endl;
  } else {
    std::cout << "Does not have state information." << std::endl;
  }
  if ( l.has_label_info() )
  {
    std::cout << "Has label information." << std::endl;
  } else {
    std::cout << "Does not have label information." << std::endl;
  }
  if ( l.has_creator() )
  {
    std::cout << "Created by: " << l.get_creator() << std::endl;
  }
  gsVerboseMsg("checking reachability...\n");
  if ( !l.reachability_check() )
  {
    std::cout << "Warning: some states are not reachable from the initial state! (This might result in unspecified behaviour of LTS tools.)" << std::endl;
  }
  if ( opts.determinism_equivalence != lts_eq_none )
  {
    gsVerboseMsg("checking whether LTS is deterministic (modulo %s)...\n",lts::name_of_equivalence(opts.determinism_equivalence).c_str());
    gsVerboseMsg("minimisation...\n");

    l.reduce(opts.determinism_equivalence);
    gsVerboseMsg("deterministic check...\n");
    if ( l.is_deterministic() )
    {
      std::cout << "LTS is deterministic (modulo " << lts::name_of_equivalence(opts.determinism_equivalence) << ")" << std::endl;
    } else {
      std::cout << "LTS is not deterministic (modulo " << lts::name_of_equivalence(opts.determinism_equivalence) << ")" << std::endl;
    }
  }
}

int main(int argc, char **argv)
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
