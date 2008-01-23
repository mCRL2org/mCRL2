// Author(s): Jeroen Keiren
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsbinary.cpp
/// \brief The binary tool, this runs the binary algorithm. 

// ======================================================================
//
// Copyright (c) 2006 TU/e
//
// ----------------------------------------------------------------------
//
// file          : lpsbinary
// date          : 22-12-2006
// version       : 0.25
//
// author(s)     : Jeroen Keiren <j.j.a.keiren@student.tue.nl>
//
// ======================================================================
//

#define NAME "lpsbinary"
#define AUTHOR "Jeroen Keiren"

//C++
#include <cstdio>
#include <exception>

//Boost
#include <boost/program_options.hpp>
#include "mcrl2/lps/binary.h"

//Aterms
#include <mcrl2/atermpp/aterm.h>

#include "mcrl2/utilities/version_info.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"

using namespace std;
using namespace ::mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2;

namespace po = boost::program_options;

#define VERSION "July 2007"

///////////////////////////////////////////////////////////////////////////////
/// \brief structure that holds all options available for the tool.
///
typedef struct
{
  std::string input_file; ///< Name of the file to read input from
  std::string output_file; ///< Name of the file to write output to (or stdout)
  RewriteStrategy strategy; ///< Rewrite strategy to use
}tool_options;


#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <mcrl2/utilities/squadt_interface.h>

//Forward declaration because do_binary() is called within squadt_interactor class
int do_binary(const tool_options& options);

class squadt_interactor: public mcrl2::utilities::squadt::mcrl2_tool_interface
{
  private:

    static const char*  lps_file_for_input;  ///< file containing an LPS that can be imported
    static const char*  lps_file_for_output; ///< file used to write the output to

    static const char*  option_rewrite_strategy;

  private:
    boost::shared_ptr < tipi::datatype::enumeration > rewrite_strategy_enumeration;

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

const char* squadt_interactor::lps_file_for_input  = "lps_in";
const char* squadt_interactor::lps_file_for_output = "lps_out";

const char* squadt_interactor::option_rewrite_strategy = "rewrite_strategy";

squadt_interactor::squadt_interactor() {
  rewrite_strategy_enumeration.reset(new tipi::datatype::enumeration("inner"));
  *rewrite_strategy_enumeration % "innerc" % "jitty" % "jittyc";
}

void squadt_interactor::set_capabilities(tipi::tool::capabilities& capabilities) const
{
  // The tool has only one main input combination
  capabilities.add_input_configuration(lps_file_for_input, tipi::mime_type("lps", tipi::mime_type::application), tipi::tool::category::transformation);
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& configuration)
{
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::datatype;
  using namespace tipi::layout::elements;

  /* Set defaults where the supplied configuration does not have values */
  if (!configuration.output_exists(lps_file_for_output)) {
    configuration.add_output(lps_file_for_output, tipi::mime_type("lps", tipi::mime_type::application), configuration.get_output_name(".lps"));
  }
  if (!configuration.option_exists(option_rewrite_strategy)) {
    configuration.add_option(option_rewrite_strategy).append_argument(rewrite_strategy_enumeration, 0);
  }

  /* Create display */
  tipi::layout::tool_display d;

  // Helper for strategy selection
  mcrl2::utilities::squadt::radio_button_helper < RewriteStrategy > strategy_selector(d);

  layout::vertical_box& m = d.create< vertical_box >();

  m.append(d.create< label >().set_text("Rewrite strategy")).
    append(d.create< horizontal_box >().
                append(strategy_selector.associate(GS_REWR_INNER, "Inner")).
                append(strategy_selector.associate(GS_REWR_INNERC, "Innerc")).
                append(strategy_selector.associate(GS_REWR_JITTY, "Jitty")).
                append(strategy_selector.associate(GS_REWR_JITTYC, "Jittyc")));

  button& okay_button = d.create< button >().set_label("OK");

  m.append(d.create< label >().set_text(" ")).
    append(okay_button, layout::right);

  if (configuration.option_exists(option_rewrite_strategy)) {
    strategy_selector.set_selection(static_cast < RewriteStrategy > (
        configuration.get_option_argument< size_t >(option_rewrite_strategy, 0)));
  }

  send_display_layout(d.set_manager(m));

  okay_button.await_change();

  configuration.get_option(option_rewrite_strategy).replace_argument(0, rewrite_strategy_enumeration, strategy_selector.get_selection());
}

//bool squadt_interactor::extract_task_options(tipi::configuration const& configuration,

bool squadt_interactor::check_configuration(tipi::configuration const& configuration) const
{
  bool result = true;

  result |= configuration.input_exists(lps_file_for_input);
  result |= configuration.input_exists(lps_file_for_output);
  result |= configuration.option_exists(option_rewrite_strategy);

  return result;
}

bool squadt_interactor::perform_task(tipi::configuration& configuration)
{
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::datatype;
  using namespace tipi::layout::elements;

  tool_options options;
  options.input_file = configuration.get_input(lps_file_for_input).get_location();
  options.output_file = configuration.get_output(lps_file_for_output).get_location();
  options.strategy = static_cast < RewriteStrategy > (boost::any_cast < size_t > (configuration.get_option_argument(option_rewrite_strategy, 0)));

  /* Create display */
  tipi::layout::tool_display d;

  send_display_layout(d.set_manager(d.create< vertical_box >().
                append(d.create< label >().set_text("Binary in progress"), layout::left)));

  //Perform declustering
  bool result = do_binary(options) == 0;

  send_display_layout(d.set_manager(d.create< vertical_box >().
                append(d.create< label >().set_text(std::string("Binary ") + ((result) ? "succeeded" : "failed")), layout::left)));

  return result;
}

#endif //ENABLE_SQUADT_CONNECTIVITY

///Reads a specification from input_file,
///applies binary to it and writes the result to output_file.
int do_binary(const tool_options& options)
{
  lps::specification lps_specification;
  try
  {
    lps_specification.load(options.input_file);
    // apply binary on lps_specification and save the output to a binary file

    Rewriter* r = createRewriter(lps_specification.data(), options.strategy);

    lps::specification result;
    result = lps::binary(lps_specification, *r);

    if (!result.save(options.output_file, true))
    {
      // An error occurred when saving
      gsErrorMsg("Could not save to '%s'\n", options.output_file.c_str());
      return (1);
    }
  }
  catch (std::runtime_error e)
  {
    gsErrorMsg("lpsbinary: Unable to load LPS from `%s'\n", options.input_file.c_str());
    return (1);
  }

  return 0;
}

///Parses command line and sets settings from command line switches
void parse_command_line(int ac, char** av, tool_options& t_options) {
  po::options_description desc;
  std::string rewriter;

  desc.add_options()
      ("help,h",      "display this help")
      ("version",     "display version information")
      ("verbose,v",   "turn on the display of short intermediate messages")
      ("debug,d",     "turn on the display of detailed intermediate messages")
      ("rewriter,R",   po::value<std::string>(&rewriter)->default_value("jitty"),
                       "use rewriter arg (default 'jitty');"
                      "available rewriters are inner, jitty, innerc and jittyc")
  ;
  po::options_description hidden("Hidden options");
  hidden.add_options()
      ("INFILE", po::value< string >(), "input file")
      ("OUTFILE", po::value< string >(), "output file")
  ;
  po::options_description cmdline_options;
  cmdline_options.add(desc).add(hidden);

  po::options_description visible("Allowed options");
  visible.add(desc);

  po::positional_options_description p;
  p.add("INFILE", 1);
  p.add("OUTFILE", -1);

  po::variables_map vm;
  po::store(po::command_line_parser(ac, av).
    options(cmdline_options).positional(p).run(), vm);
  po::notify(vm);

  if (vm.count("help")) {
    cout << "Usage: "<< av[0] << " [OPTION]... [INFILE [OUTFILE]]" << endl;
    cout << "Replace finite sort variables by vectors of boolean variables in the LPS in" << endl;
    cout << "INFILE and write the result to OUTFILE. If INFILE is not present, stdin is used." << endl;
    cout << "If OUTFILE is not present, stdout is used" << endl;

    cout << endl;
    cout << "Options:" << endl;
    cout << desc;
    cout << endl;
    cout << "Report bugs at <http://www.mcrl2.org/issuetracker>." << endl;

    exit (0);
  }

  if (vm.count("version")) {
    print_version_information(NAME, AUTHOR);
    exit (0);
  }

  if (vm.count("debug")) {
    gsSetDebugMsg();
  }

  if (vm.count("verbose")) {
    gsSetVerboseMsg();
  }

  t_options.strategy = RewriteStrategyFromString(rewriter.c_str());
  if (t_options.strategy == GS_REWR_INVALID)
  {
    cerr << rewriter << " is not a valid rewriter strategy" << endl;
    exit(EXIT_FAILURE);
  }

  t_options.input_file = (0 < vm.count("INFILE")) ? vm["INFILE"].as< string >() : "-";
  t_options.output_file = (0 < vm.count("OUTFILE")) ? vm["OUTFILE"].as< string >() : "-";
}

int main(int argc, char** argv)
{
  MCRL2_ATERM_INIT(argc, argv)

  tool_options options;

#ifdef ENABLE_SQUADT_CONNECTIVITY
  if (mcrl2::utilities::squadt::interactor< squadt_interactor >::free_activation(argc, argv)) {
    return 0;
  }
#endif

  parse_command_line(argc,argv, options);
  return do_binary(options);
}
