// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsuntime.cpp

#include "boost.hpp" // precompiled headers

#define NAME "lpsuntime"
#define AUTHOR "Jeroen Keiren"

//C++
#include <exception>
#include <cstdio>

//Aterms
#include <mcrl2/atermpp/aterm.h>

#include <mcrl2/core/messaging.h>
#include <mcrl2/utilities/command_line_interface.h>
#include <mcrl2/utilities/command_line_messaging.h>
#include <mcrl2/core/aterm_ext.h>

//LPS framework
#include <mcrl2/lps/untime.h>

using namespace std;
using namespace mcrl2::utilities;
using namespace mcrl2::core;

struct tool_options {
  std::string input_file; // Name of the file to read input from
  std::string output_file; // Name of the file to write output to (or stdout)
};

//Squadt connectivity
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <mcrl2/utilities/mcrl2_squadt_interface.h>

//Forward declaration needed for use within squadt_interactor class
int do_untime(const tool_options& options);

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
  capabilities.add_input_configuration(lps_file_for_input, tipi::mime_type("lps", tipi::mime_type::application), tipi::tool::category::transformation);
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& configuration)
{
  if (!configuration.output_exists(lps_file_for_output)) {
    configuration.add_output(lps_file_for_output, tipi::mime_type("lps", tipi::mime_type::application), configuration.get_output_name(".lps"));
  }
}

bool squadt_interactor::check_configuration(tipi::configuration const& configuration) const
{
// Check if everything present
  return (configuration.input_exists(lps_file_for_input) &&
          configuration.output_exists(lps_file_for_output)
         );
}

bool squadt_interactor::perform_task(tipi::configuration& configuration)
{
  tool_options options;
  options.input_file = configuration.get_input(lps_file_for_input).location();
  options.output_file = configuration.get_output(lps_file_for_output).location();
  return (do_untime(options)==0);
}

#endif //ENABLE_SQUADT_CONNECTIVITY

int do_untime(const tool_options& options)
{
  mcrl2::lps::specification lps_specification;
  lps_specification.load(options.input_file);
  mcrl2::lps::untime(lps_specification).save(options.output_file);
  return 0;
}

tool_options parse_command_line(int ac, char** av) {
  interface_description clinterface(av[0], NAME, AUTHOR, "[OPTION]... [INFILE [OUTFILE]]\n",
                              "Remove time from the linear process specification (LPS) in INFILE and write the\n"
                              "result to OUTFILE. If INFILE is not present, stdin is used. If OUTFILE is not\n"
                              "present, stdout is used.\n");

  command_line_parser parser(clinterface, ac, av);

  tool_options t_options;

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

  return t_options;
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

    return do_untime(parse_command_line(argc, argv));
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return EXIT_FAILURE;
}
