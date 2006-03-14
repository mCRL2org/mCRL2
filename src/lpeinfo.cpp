// ======================================================================
//
// Copyright (c) 2004, 2005 TU/e
//
// ----------------------------------------------------------------------
//
// file          : lpeinfo 
// date          : 18-11-2005
// version       : 0.5.1
//
// author(s)     : Frank Stappers  <f.p.m.stappers@student.tue.nl>
//
// ======================================================================

//C++
#include <exception>
#include <cstdio>

//Boost
#include <boost/program_options.hpp>

//mCRL2
#include <atermpp/aterm.h>
#include <lpe/lpe.h>
#include <lpe/specification.h>

// Squadt protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <sip/tool.h>
#endif

/* Macros for identifiers of options and objects */
#define LPD_FOR_INPUT 0

using namespace std;
using namespace atermpp;

namespace po = boost::program_options;

#define VERSION "0.5"

/* Verbosity switch */
bool        verbose = false;

/* Name of the file to read input from (or standard input: "-") */
std::string file_name;

void parse_command_line(int ac, char** av) {
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
  ;
      
  po::options_description cmdline_options;
  cmdline_options.add(desc).add(hidden);
      
  po::options_description visible("Allowed options");
  visible.add(desc);
      
  po::positional_options_description p;
  p.add("INFILE", -1);
      
  po::variables_map vm;
  po::store(po::command_line_parser(ac, av).
    options(cmdline_options).positional(p).run(), vm);
  po::notify(vm);
      
  if (vm.count("help")) {
    cerr << "Usage: "<< av[0] << " [OPTION]... [INFILE]" << endl;
    cerr << "Print basic information on the LPE in INFILE." << endl;
    cerr << endl;
    cerr << desc;

    exit (0);
  }
      
  if (vm.count("version")) {
    cerr << "lpeinfo " << VERSION << " (revision " << REVISION << ")" << endl;

    exit (0);
  }

  verbose  = 0 < vm.count("verbose");
  file_name = (0 < vm.count("INFILE")) ? vm["INFILE"].as< string >() : "-";
}
        
int main(int ac, char** av)
{
  ATerm bot;
  ATinit(0,0,&bot);
  gsEnableConstructorFunctions();

#ifdef ENABLE_SQUADT_CONNECTIVITY
  sip::tool_communicator tc;

  /* Get tool capabilities in order to modify settings */
  sip::tool_capabilities& cp = tc.get_tool_capabilities();

  /* The tool has only one main input combination it takes an LPE and then behaves as a reporter */
  cp.add_input_combination(LPD_FOR_INPUT, "Reporter", "lpe");

  /* On purpose we do not catch exceptions */
  if (tc.activate(ac,av)) {
    bool valid = false;

    /* Static configuration cycle */
    while (valid == false) {
      valid = true;

      /* Wait for configuration data to be send (either a previous configuration, or only an input combination) */
      tc.await_message(sip::send_configuration);

      /* Validate configuration specification, should contain a file name of an LPD that is to be read as input */
      sip::configuration& configuration = tc.get_tool_configuration();

      valid &= configuration.object_exists(LPD_FOR_INPUT);

      if (valid) {
        /* An object with the correct id exists, assume the URI is relative (i.e. a file name in the local file system) */
        file_name = configuration.get_object(LPD_FOR_INPUT)->get_location();
      }
    }

    /* Send the controller the signal that we're ready to rumble (no further configuration necessary) */
    tc.send_accept_configuration();

    /* Wait for start message */
    tc.await_message(sip::send_signal_start);
  }
  else {
    parse_command_line(ac,av);
  }
#else
  parse_command_line(ac,av);
#endif

  lpe::specification lpe_specification;

  if (lpe_specification.load(file_name)) {
    lpe::LPE lpe = lpe_specification.lpe();
    
    cout << "Output read from " << ((file_name == "-") ? "standard input" : file_name) << endl << endl;

    cout << "Number of summands          : " << lpe.summands().size() << endl;
    cout << "Number of free variables    : " << lpe_specification.initial_free_variables().size() + lpe.free_variables().size() << endl;
    cout << "Number of process parameters: " << lpe.process_parameters().size() << endl; 
    cout << "Number of actions           : " << lpe.actions().size() << endl;
  }
  else {
    std::string error("Error: unable to load LPE from `" + file_name + "'\n");

#ifdef ENABLE_SQUADT_CONNECTIVITY
    if (tc.is_active()) {
      /* Something went wrong, send an error report */
      sip::report report;

      report.set_error(error);

      tc.send_report(report);
    }
    else {
      std::cerr << error;
    }
#else
    std::cerr << error
#endif

    return (1);
  }

  return 0;
}
