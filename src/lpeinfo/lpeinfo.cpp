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

// Squadt protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <squadt_utility.h>
#endif

#include "mcrl2_revision.h"

//C++
#include <exception>
#include <cstdio>

//Boost
#include <boost/program_options.hpp>
#include <boost/filesystem/convenience.hpp>

//mCRL2
#include <atermpp/aterm.h>
#include <lpe/lpe.h>
#include <lpe/specification.h>

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
        
int main(int ac, char** av) {
  ATerm bot;
  ATinit(0,0,&bot);
  gsEnableConstructorFunctions();

#ifdef ENABLE_SQUADT_CONNECTIVITY
  sip::tool::communicator tc;

  /* Constants for identifiers of options and objects */
  const unsigned int lpd_file_for_input = 0;

  /* Get tool capabilities in order to modify settings */
  sip::tool::capabilities& cp = tc.get_tool_capabilities();

  /* The tool has only one main input combination it takes an LPE and then behaves as a reporter */
  cp.add_input_combination(lpd_file_for_input, "Reporting", "lpe");

  /* On purpose we do not catch exceptions */
  if (tc.activate(ac,av)) {
    bool valid = false;

    /* Initialise utility pseudo-library */
    squadt_utility::initialise(tc);

    /* Static configuration cycle */
    while (!valid) {
      /* Wait for configuration data to be send (either a previous configuration, or only an input combination) */
      sip::configuration::sptr configuration = tc.await_configuration();

      /* Validate configuration specification, should contain a file name of an LPD that is to be read as input */
      valid  = configuration.get() != 0;
      valid &= configuration->object_exists(lpd_file_for_input);

      if (valid) {
        /* An object with the correct id exists, assume the URI is relative (i.e. a file name in the local file system) */
        file_name = configuration->get_object(lpd_file_for_input)->get_location();
      }
      else {
        tc.send_status_report(sip::report::error, "Invalid input combination!");
      }
    }

    /* Send the controller the signal that we're ready to rumble (no further configuration necessary) */
    tc.send_accept_configuration();

    /* Wait for start message */
    tc.await_message(sip::message_signal_start);
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
    
#ifdef ENABLE_SQUADT_CONNECTIVITY
    if (tc.is_active()) {
      using namespace sip;
      using namespace sip::layout;
      using namespace sip::layout::elements;

      layout::tool_display::sptr display(new layout::tool_display);

      /* Create and add the top layout manager */
      layout::manager::aptr layout_manager = layout::horizontal_box::create();

      /* First column */
      layout::vertical_box* left_column = new layout::vertical_box();

      layout::vertical_box::alignment a = layout::left;

      left_column->add(new label("Input read from:"), a);
      left_column->add(new label("Summands (#):"), a);
      left_column->add(new label("Free variables (#):"), a);
      left_column->add(new label("Process parameters (#):"), a);
      left_column->add(new label("Actions (#):"), a);

      /* Second column */
      layout::vertical_box* right_column = new layout::vertical_box();

      boost::format c("%u");

      right_column->add(new label(boost::filesystem::path(file_name).leaf()), a);
      right_column->add(new label(boost::str(c % lpe.summands().size())), a);
      right_column->add(new label(boost::str(c % (lpe_specification.initial_free_variables().size() + lpe.free_variables().size()))), a);
      right_column->add(new label(boost::str(c % lpe.process_parameters().size())), a);
      right_column->add(new label(boost::str(c % lpe.actions().size())), a);

      /* Attach columns*/
      layout_manager->add(left_column, margins(0,5,0,5));
      layout_manager->add(right_column, margins(0,5,0,20));

      display->set_top_manager(layout_manager);

      tc.send_display_layout(display);
    }
    else {
#endif
      cout << "Input read from " << ((file_name == "-") ? "standard input" : file_name) << endl << endl;
     
      cout << "Number of summands          : " << lpe.summands().size() << endl;
      cout << "Number of free variables    : " << lpe_specification.initial_free_variables().size() + lpe.free_variables().size() << endl;
      cout << "Number of process parameters: " << lpe.process_parameters().size() << endl; 
      cout << "Number of actions           : " << lpe.actions().size() << endl;
#ifdef ENABLE_SQUADT_CONNECTIVITY
    }
#endif
  }
  else {
    std::string error("Unable to load LPE from `" + file_name + "'\n");

#ifdef ENABLE_SQUADT_CONNECTIVITY
    if (tc.is_active()) {
      /* Something went wrong, send an error report */
      tc.send_status_report(sip::report::error, error);

      tc.send_signal_done();

      tc.await_message(sip::message_request_termination);
    }
    else {
      std::cerr << "Error: " + error;
    }
#else
    std::cerr << "Error: " + error;
#endif

    return (1);
  }

#ifdef ENABLE_SQUADT_CONNECTIVITY
  tc.send_signal_done();

  tc.await_message(sip::message_request_termination);
#endif

  return 0;
}
