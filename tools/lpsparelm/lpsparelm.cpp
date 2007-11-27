//  Copyright 2007 F.P.M. (Frank) Stappers. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./lpsparelm.cpp

#include <exception>

//Boost
#include <boost/program_options.hpp>

#include <mcrl2/lps/specification.h>
#include <mcrl2/lps/parelm.h>

struct tool_configuration {
  std::string input_file;  ///< Name of the file to read input from
  std::string output_file; ///< Name of the file to write output to (or stdout)

  tool_configuration() {
  }

  tool_configuration(std::string in, std::string out) : input_file(in), output_file(out) {
  }

  bool execute() {
    lps::specification lps_specification;
    
    try {
      lps_specification.load(input_file);
     
      lps::specification r = lps::parelm(lps_specification);

      r.save(output_file);
    }
    catch (std::exception e) {
      std::cerr << e.what() << std::endl;

      return false;
    }

    return true;;
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

tool_configuration parse_command_line(int ac, char** av, tool_configuration& options) {
  namespace po = boost::program_options;

  po::options_description description;

  description.add_options()
    ("help,h",      "display this help")
    ("verbose,v",   "turn on the display of short intermediate messages")
    ("debug,d",     "turn on the display of detailed intermediate messages")
    ("version",     "display version information")
  ;
  po::options_description hidden("Hidden options");
  hidden.add_options()
      ("INFILE", po::value< string >(), "input file")
      ("OUTFILE", po::value< string >(), "output file");

  po::options_description cmdline_options;
  cmdline_options.add(description).add(hidden);
        
  po::options_description visible("Allowed options");
  visible.add(description);
        
  po::positional_options_description p;
  p.add("INFILE", 1).add("OUTFILE", 1);

  po::variables_map vm;
  po::store(po::command_line_parser(ac, av).
    options(cmdline_options).positional(p).run(), vm);
  po::notify(vm);
     
  if (vm.count("help")) {
    std::cerr << "Usage: "<< av[0] << " [OPTION]... [INFILE [OUTFILE]] \n"
              << "Removes unused parameters from the LPS read from standard input or INFILE." << std::endl
              << "By default the result is written to standard output, and otherwise to OUTFILE." << std::endl
              << std::endl << description;

    exit (0);
  }
        
  if (vm.count("version")) {
    std::cerr << "July 2007" << " (revision " << REVISION << ")" << std::endl;

    exit (0);
  }

  return tool_configuration((0 < vm.count("INFILE")) ? vm["INFILE"].as< string >() : "-",
                            (0 < vm.count("OUTFILE")) ? vm["OUTFILE"].as< string >() : "-");
}

int main(int argc, char** argv) {
  ATerm     bottom;

  ATinit(argc,argv,&bottom);

  gsEnableConstructorFunctions();
  
#ifdef ENABLE_SQUADT_CONNECTIVITY
  if (mcrl2::utilities::squadt::interactor< squadt_interactor >::free_activation(argc, argv)) {
    return (0);
  }
#endif

  tool_configuration c(parse_command_line(argc,argv,c));

  return c.execute();
}
