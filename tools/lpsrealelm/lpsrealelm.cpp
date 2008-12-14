// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsrealelm.cpp

#define NAME "lpsrealelm"
#define AUTHOR "Jeroen Keiren"

//C++
#include <exception>
#include <cstdio>

//Aterms
#include <mcrl2/atermpp/aterm.h>
#include <mcrl2/atermpp/aterm_list.h>
#include <mcrl2/atermpp/table.h>
#include <mcrl2/atermpp/algorithm.h>

//Aterms
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/command_line_messaging.h"

#include "realelm.h"

using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::lps;

struct tool_options {
  std::string input_file; ///< Name of the file to read input from
  std::string output_file; ///< Name of the file to write output to (or stdout)
  int max_iterations;
};

//Squadt connectivity
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <mcrl2/utilities/mcrl2_squadt_interface.h>

//Forward declaration because do_realelm() is called within squadt_interactor class
int do_realelm(const tool_options& options);

class squadt_interactor: public mcrl2::utilities::squadt::mcrl2_tool_interface
{
  private:

    static const char*  lps_file_for_input;  ///< file containing an LPS that can be imported
    static const char*  lps_file_for_output; ///< file used to write the output to

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

void squadt_interactor::set_capabilities(tipi::tool::capabilities& capabilities) const
{
  // The tool has only one main input combination
  gsDebugMsg("squadt_interactor: Setting capabilities\n");
  capabilities.add_input_configuration(lps_file_for_input, tipi::mime_type("lps", tipi::mime_type::application), tipi::tool::category::transformation);
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& configuration)
{
  gsDebugMsg("squadt_interactor: User interactive configuration\n");

  if (configuration.fresh()) {
    if (!configuration.output_exists(lps_file_for_output)) {
      configuration.add_output(lps_file_for_output, tipi::mime_type("lps", tipi::mime_type::application), configuration.get_output_name(".lps"));
    }
  }
}

bool squadt_interactor::check_configuration(tipi::configuration const& configuration) const
{
  gsDebugMsg("squadt_interactor: Checking configuration\n");
  // Check if everything present
  return (configuration.input_exists(lps_file_for_input) &&
          configuration.output_exists(lps_file_for_output)
         );
}

bool squadt_interactor::perform_task(tipi::configuration& configuration)
{
  gsDebugMsg("squadt_interactor: Performing task\n");
  tool_options options;
  options.input_file = configuration.get_input(lps_file_for_input).location();
  options.output_file = configuration.get_output(lps_file_for_output).location();
  options.max_iterations = 5;

  gsDebugMsg("Calling do_realelm through SQuADT, with input: %s and output: %s\n", options.input_file.c_str(), options.output_file.c_str());
  return (do_realelm(options)==0);
}

#endif //ENABLE_SQUADT_CONNECTIVITY

///Reads a specification from input_file,
///applies real time abstraction to it and writes the result to output_file.
int do_realelm(const tool_options& options)
{
  specification lps_specification;

  lps_specification.load(options.input_file);

  // Untime lps_specification and save the output to a binary file
  specification new_spec = realelm(lps_specification, options.max_iterations);

  gsDebugMsg("Real time abstraction completed, saving to %s\n", options.output_file.c_str());
  new_spec.save(options.output_file);

  return 0;
}

///Parses command line and sets settings from command line switches
bool parse_command_line(int ac, char** av, tool_options& t_options) {
  interface_description clinterface(av[0], NAME, AUTHOR, "[OPTION]... [INFILE [OUTFILE]]\n",
                             "Remove Real numbers from the linear process specification (LPS) in "
                             "INFILE and write the result to OUTFILE. If INFILE is not present, stdin is used. "
                             "If OUTFILE is not present, stdout is used.");

  //clinterface.add_rewriting_options();
  clinterface.add_option("max", make_mandatory_argument("NUM"),
    "perform at most NUM iterations");

  command_line_parser parser(clinterface, ac, av);

  if (parser.continue_execution()) {
    if (parser.options.count("max")) {
      t_options.max_iterations = parser.option_argument_as< int > ("max");
    }
    else
    {
      t_options.max_iterations = 5;
    }
    if (2 < parser.arguments.size()) {
      parser.error("too many file arguments");
    }
    else {
      if (0 < parser.arguments.size()) {
        t_options.input_file = parser.arguments[0];
      }
      if (1 < parser.arguments.size()) {
        t_options.output_file = parser.arguments[1];
      }
    }
  }

  return parser.continue_execution();
}

int main(int argc, char** argv)
{
  MCRL2_ATERM_INIT(argc, argv)

  try {
#ifdef ENABLE_SQUADT_CONNECTIVITY
    if (mcrl2::utilities::squadt::interactor< squadt_interactor >::free_activation(argc, argv)) {
      return EXIT_SUCCESS;
    }
#endif

    tool_options options;

    if (parse_command_line(argc, argv, options)) {
      return do_realelm(options);
    }
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
