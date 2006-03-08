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

using namespace std;
using namespace atermpp;

namespace po = boost::program_options;

#define VERSION "0.5"

/* Verbosity switch */
bool        verbose;

/* Name of the file to read input from (or standard input: "-") */
std::string filename;

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
  filename = (0 < vm.count("INFILE")) ? vm["INFILE"].as< string >() : "-";
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
  cp.add_input_combination("Reporter", "lpe");

  /* On purpose we do not catch exceptions */
  if (tc.activate(ac,av)) {
    /* Wait for configuration */
    tc.await_message(sip::send_configuration);

    /* Send the controller the signal that we're ready to rumble (no further configuration necessary) */
    tc.send_accept_configuration();

    /* Wait for start message */
    tc.await(send_signal_start);
  }
  else {
    parse_command_line(ac,av);
  }
#else
  parse_command_line(ac,av);
#endif

  lpe::specification lpe_specification;

  if (lpe_specification.load(filename)) {
    lpe::LPE lpe = lpe_specification.lpe();
    
    cout << "Output read from " << ((filename == "-") ? "standard input" : filename) << endl << endl;

    cout << "Number of summands          : " << lpe.summands().size() << endl;
    cout << "Number of free variables    : " << lpe_specification.initial_free_variables().size() + lpe.free_variables().size() << endl;
    cout << "Number of process parameters: " << lpe.process_parameters().size() << endl; 
    cout << "Number of actions           : " << lpe.actions().size() << endl;
  }

  return 0;
}
