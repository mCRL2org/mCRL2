//  Copyright 2007 J.van der Wulp, Wieger Wesselink. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./lpsparelm.cpp

#define NAME "lpsparelm"
#define AUTHOR "Jeroen van der Wulp and Wieger Wesselink"

#include <exception>

#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/parelm.h"

#include "mcrl2/utilities/aterm_ext.h"
#include "mcrl2/utilities/command_line_interface.h"

struct tool_configuration {
  std::string input;  ///< Name of the file to read input from
  std::string output; ///< Name of the file to write output to (or stdout)

  tool_configuration(std::string const& in, std::string const& out) : input(in), output(out) {
  }

  bool execute() {
    mcrl2::lps::specification lps_specification;
    
    try {
      lps_specification.load(input);
    }
    catch (std::exception& e) {
      // XXX problem: the exceptions have bad descriptions, resorting to manual handling
      std::cerr << "Fatal: could not read from file `" << input << "'" << std::endl;

      return false;
    }

    try {
      mcrl2::lps::specification r = mcrl2::lps::parelm(lps_specification);

      r.save(output);
    }
    catch (std::exception& e) {
      // XXX problem: the exceptions have bad descriptions, resorting to manual handling
      std::cerr << "Fatal: could not write to file `" << output << "'" << std::endl;

      return false;
    }

    return true;
  }

  tool_configuration(int ac, char** av) : input("-"), output("-") {
    interface_description clinterface(av[0], NAME, AUTHOR, "[OPTION]... [INFILE [OUTFILE]]\n"
                             "Remove unused parameters from the linear process specification (LPS) in INFILE\n"
                             "and write the result to OUTFILE. If INFILE is not present, stdin is used. If\n"
                             "OUTFILE is not present, stdout is used.\n");

    command_line_parser parser(clinterface, ac, av);

    if (2 < parser.arguments.size()) {
      parser.error("too many file arguments");
    }
    else {
      if (0 < parser.arguments.size()) {
        input  = parser.arguments[0];
      }
      if (1 < parser.arguments.size()) {
        output = parser.arguments[1];
      }
    }
  }
};

// Squadt protocol interface and utility pseudo-library
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <mcrl2/utilities/squadt_interface.h>

class squadt_interactor : public mcrl2::utilities::squadt::mcrl2_tool_interface {

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

void squadt_interactor::set_capabilities(tipi::tool::capabilities& c) const {
  c.add_input_configuration(lps_file_for_input, tipi::mime_type("lps", tipi::mime_type::application), tipi::tool::category::transformation);
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& c) {
  if (!c.output_exists(lps_file_for_output)) {
    c.add_output(lps_file_for_output, tipi::mime_type("lps", tipi::mime_type::application), c.get_output_name(".lps"));
  }
}

bool squadt_interactor::check_configuration(tipi::configuration const& c) const {
  bool result = true;

  result &= c.input_exists(lps_file_for_input);
  result &= c.output_exists(lps_file_for_output);

  return (result);
}

bool squadt_interactor::perform_task(tipi::configuration& c) {
  tool_configuration tc(c.get_input(lps_file_for_input).get_location(),
                        c.get_output(lps_file_for_output).get_location());
  
  return tc.execute();
}
#endif

int main(int argc, char** argv)
{
  MCRL2_ATERM_INIT(argc, argv)
  
  try {
#ifdef ENABLE_SQUADT_CONNECTIVITY
    if (mcrl2::utilities::squadt::interactor< squadt_interactor >::free_activation(argc, argv)) {
      return EXIT_SUCCESS;
    }
#endif

    return tool_configuration(argc, argv).execute();
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return EXIT_FAILURE;
}
