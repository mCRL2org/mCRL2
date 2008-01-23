// Author(s): Jeroen Keiren
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsuntime.cpp
/// \brief Add your file description here.

// ======================================================================
//
// Copyright (c) 2006 TU/e
//
// ----------------------------------------------------------------------
//
// file          : lpsuntime 
// date          : 07-11-2006
// version       : 0.31
//
// author(s)     : Jeroen Keiren <j.j.a.keiren@student.tue.nl>
//
// ======================================================================
//

#define NAME "lpsuntime"
#define AUTHOR "Jeroen Keiren"

//C++
#include <exception>
#include <cstdio>

//Boost
#include <boost/program_options.hpp>

//Aterms
#include <mcrl2/atermpp/aterm.h>

#include <mcrl2/utilities/version_info.h>
#include <mcrl2/core/messaging.h>
#include <mcrl2/utilities/aterm_ext.h>

//LPS framework
#include <mcrl2/lps/untime.h>


using namespace std;
using namespace ::mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2;


namespace po = boost::program_options;

typedef struct
{
  std::string input_file; // Name of the file to read input from
  std::string output_file; // Name of the file to write output to (or stdout)
}tool_options;

//Squadt connectivity
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <mcrl2/utilities/squadt_interface.h>

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
  options.input_file = configuration.get_input(lps_file_for_input).get_location();
  options.output_file = configuration.get_output(lps_file_for_output).get_location();
  return (do_untime(options)==0);
}

#endif //ENABLE_SQUADT_CONNECTIVITY

int do_untime(const tool_options& options)
{
  lps::specification lps_specification;

  try
  {
    lps_specification.load(options.input_file);

    // Untime lps_specification and save the output to a binary file
    if (!lps::untime(lps_specification).save(options.output_file, true)) 
    {
      // An error occurred when saving
      gsErrorMsg("could not save to '%s'\n", options.output_file.c_str());
      return (1);
    }
  }
  catch (std::runtime_error e)
  {
    gsErrorMsg("unable to load LPS from `%s'\n", options.input_file.c_str());
    return (1);
  }

  return 0;
}

void parse_command_line(int ac, char** av, tool_options& t_options) {
  po::options_description desc;

  desc.add_options()
      ("help,h",      "display this help")
      ("verbose,v",   "turn on the display of short intermediate messages")
      ("debug,d",    "turn on the display of detailed intermediate messages")
      ("version",     "display version information")
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
    cout << "Remove time from the linear process specification (LPS) in INFILE and write the" << endl;
    cout << "result to OUTFILE. If INFILE is not present, stdin is used. If OUTFILE is not" << endl;
    cout << "present, stdout is used." << endl;
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

  parse_command_line(argc, argv, options);
  return do_untime(options);
}
