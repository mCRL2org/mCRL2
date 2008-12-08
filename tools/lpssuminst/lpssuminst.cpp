// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpssuminst.cpp

#include "boost.hpp" // precompiled headers

#define NAME "lpssuminst"
#define AUTHOR "Jeroen Keiren"

//C++
#include <exception>
#include <cstdio>

//Aterms
#include <mcrl2/atermpp/aterm.h>

#include <mcrl2/lps/suminst.h>

#include "mcrl2/core/messaging.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/command_line_messaging.h"
#include "mcrl2/utilities/command_line_rewriting.h"

using namespace std;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::lps;

///////////////////////////////////////////////////////////////////////////////
/// \brief structure that holds all options available for the tool.
///
struct tool_options {
  std::string input_file; ///< Name of the file to read input from
  std::string output_file; ///< Name of the file to write output to (or stdout)
  t_suminst_options suminst_opts; ///< Options of the algorithm

  tool_options() {}
};

//Squadt connectivity
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <mcrl2/utilities/mcrl2_squadt_interface.h>

//Forward declaration because do_suminst() is called within squadt_interactor class
int do_suminst(const tool_options& options);

class squadt_interactor: public mcrl2::utilities::squadt::mcrl2_tool_interface
{
  private:

    static const char*  lps_file_for_input;  ///< file containing an LPS that can be imported
    static const char*  lps_file_for_output; ///< file used to write the output to

    static const char*  option_tau_only;
    static const char*  option_finite_only;
    static const char*  option_rewrite_strategy;

  public:

    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities&) const;

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration&);

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const&) const;

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration&);
};

const char* squadt_interactor::lps_file_for_input  = "lps_in";
const char* squadt_interactor::lps_file_for_output = "lps_out";

const char* squadt_interactor::option_tau_only         = "tau_only";
const char* squadt_interactor::option_finite_only      = "finite_only";
const char* squadt_interactor::option_rewrite_strategy = "rewrite_strategy";

void squadt_interactor::set_capabilities(tipi::tool::capabilities& capabilities) const
{
  // The tool has only one main input combination
  capabilities.add_input_configuration(lps_file_for_input, tipi::mime_type("lps", tipi::mime_type::application), tipi::tool::category::transformation);
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& configuration)
{
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::layout::elements;

  /* Set defaults where the supplied configuration does not have values */
  if (!configuration.output_exists(lps_file_for_output)) {
    configuration.add_output(lps_file_for_output, tipi::mime_type("lps", tipi::mime_type::application), configuration.get_output_name(".lps"));
  }
  if (!configuration.option_exists(option_tau_only)) {
    configuration.add_option(option_tau_only).
       set_argument_value< 0 >(true, false);
  }
  if (!configuration.option_exists(option_finite_only)) {
    configuration.add_option(option_finite_only).
       set_argument_value< 0 >(true, false);
  }

  /* Create display */
  tipi::tool_display d;

  // Helper for strategy selection
  mcrl2::utilities::squadt::radio_button_helper < RewriteStrategy > strategy_selector(d);

  layout::vertical_box& m = d.create< vertical_box >();

  m.append(d.create< label >().set_text("Rewrite strategy")).
    append(d.create< horizontal_box >().
                append(strategy_selector.associate(GS_REWR_INNER, "Inner")).
#ifdef MCRL2_INNERC_AVAILABLE
                append(strategy_selector.associate(GS_REWR_INNERC, "Innerc")).
#endif
#ifdef MCRL2_JITTYC_AVAILABLE
                append(strategy_selector.associate(GS_REWR_JITTY, "Jitty", true)).
                append(strategy_selector.associate(GS_REWR_JITTYC, "Jittyc")));
#else
                append(strategy_selector.associate(GS_REWR_JITTY, "Jitty", true)));
#endif

  /* Prepare user interaction */
  checkbox& tau_only = d.create< checkbox >().set_status(configuration.get_option_argument< bool >(option_tau_only));
  m.append(d.create< label >().set_text(" ")).
    append(tau_only.set_label("Only instantiate tau summands"), layout::left);

  checkbox& finite_only = d.create< checkbox >().set_status(configuration.get_option_argument< bool >(option_finite_only));
  m.append(d.create< label >().set_text(" ")).
    append(finite_only.set_label("Only instantiate variables of finite sorts"), layout::left);

  button& okay_button = d.create< button >().set_label("OK");

  m.append(d.create< label >().set_text(" ")).
    append(okay_button, layout::right);

  if (configuration.option_exists(option_rewrite_strategy)) {
    strategy_selector.set_selection(configuration.get_option_argument< RewriteStrategy >(option_rewrite_strategy, 0));
  }

  send_display_layout(d.manager(m));

  okay_button.await_change();

  /* Update configuration */
  configuration.get_option(option_tau_only).
     set_argument_value< 0 >(tau_only.get_status());
  configuration.get_option(option_finite_only).
     set_argument_value< 0 >(finite_only.get_status());

  configuration.get_option(option_rewrite_strategy).set_argument_value< 0 >(strategy_selector.get_selection());
}

bool squadt_interactor::check_configuration(tipi::configuration const& configuration) const
{
  bool result = true;
  result |= configuration.input_exists(lps_file_for_input);
  result |= configuration.output_exists(lps_file_for_output);
  result |= configuration.option_exists(option_rewrite_strategy);

  return result;
}

bool squadt_interactor::perform_task(tipi::configuration& configuration)
{
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::layout::elements;

  tool_options options;
  options.input_file = configuration.get_input(lps_file_for_input).location();
  options.output_file = configuration.get_output(lps_file_for_output).location();
  options.suminst_opts.tau_only = configuration.option_exists(option_tau_only);
  options.suminst_opts.finite_only = configuration.option_exists(option_finite_only);
  options.suminst_opts.strategy = configuration.get_option_argument< RewriteStrategy >(option_rewrite_strategy, 0);

  /* Create display */
  tipi::tool_display d;

  send_display_layout(d.manager(d.create< vertical_box >().
                append(d.create< label >().set_text("Declustering in progress"), layout::left)));

  //Perform instantiation
  bool result = do_suminst(options) == 0;

  send_display_layout(d.manager(d.create< vertical_box >().
                append(d.create< label >().set_text(std::string("Declustering ") + ((result) ? "succeeded" : "failed")), layout::left)));

  return result;
}

#endif //ENABLE_SQUADT_CONNECTIVITY


///Reads a specification from input_file, 
///applies instantiation of sums to it and writes the result to output_file.
int do_suminst(const tool_options& options)
{
  specification lps_specification;

  lps_specification.load(options.input_file);
  Rewriter* r = createRewriter(lps_specification.data(), options.suminst_opts.strategy);

  specification result = instantiate_sums(lps_specification, *r, options.suminst_opts);

  result.save(options.output_file);

  return 0;
}

///Parses command line and sets settings from command line switches
tool_options parse_command_line(int ac, char** av) {
  interface_description clinterface(av[0], NAME, AUTHOR, "[OPTION]... [INFILE [OUTFILE]]\n",
    "Instantiate the summation variables of the linear process specification (LPS) "
    "in INFILE and write the result to OUTFILE. If INFILE is not present, stdin is "
    "used. If OUTFILE is not present, stdout is used.");

  clinterface.add_option("finite", "only instantiate variables whose sorts are finite", 'f').
  add_option("tau", "only instantiate variables in tau summands", 't');

  clinterface.add_rewriting_options();

  command_line_parser parser(clinterface, ac, av);

  tool_options t_options;
  t_options.suminst_opts.tau_only = (0 < parser.options.count("tau"));
  t_options.suminst_opts.finite_only = (0 < parser.options.count("finite"));

  if (2 < parser.arguments.size()) {
    parser.error("too many file arguments");
  }
  else {
    if (0 < parser.arguments.size()) {
      t_options.input_file  = parser.arguments[0];
    }
    if (1 < parser.arguments.size()) {
      t_options.output_file = parser.arguments[1];
    }
  }

  t_options.suminst_opts.strategy = RewriteStrategyFromString(parser.option_argument("rewriter").c_str());

  return t_options;
}

int main(int argc, char** argv) {
  MCRL2_ATERM_INIT(argc, argv)

  try {
#ifdef ENABLE_SQUADT_CONNECTIVITY
    if (mcrl2::utilities::squadt::interactor< squadt_interactor >::free_activation(argc, argv)) {
      return EXIT_SUCCESS;
    }
#endif

    return do_suminst(parse_command_line(argc,argv));
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return EXIT_FAILURE;
}
