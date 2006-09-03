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
        
#ifdef ENABLE_SQUADT_CONNECTIVITY
/* Constants for identifiers of options and objects */
const unsigned int lpd_file_for_input = 0;

bool try_to_accept_configuration(sip::tool::communicator& tc) {
  sip::configuration& configuration = tc.get_configuration();

  if (configuration.object_exists(lpd_file_for_input)) {
    /* The input object is present */
    sip::object::sptr input_object = configuration.get_object(lpd_file_for_input);

    if (!boost::filesystem::exists(boost::filesystem::path(input_object->get_location()))) {
      tc.send_status_report(sip::report::error, std::string("Invalid configuration: input object does not exist"));

      return (false);
    }
  }

  tc.send_accept_configuration();

  return (true);
}
#endif

int main(int ac, char** av) {
  ATerm bot;
  ATinit(0,0,&bot);
  gsEnableConstructorFunctions();

#ifdef ENABLE_SQUADT_CONNECTIVITY
  sip::tool::communicator tc;

  /* Get tool capabilities in order to modify settings */
  sip::tool::capabilities& cp = tc.get_tool_capabilities();

  /* The tool has only one main input combination it takes an LPD and then behaves as a reporter */
  cp.add_input_combination(lpd_file_for_input, "Reporting", "lpe");

  /* On purpose we do not catch exceptions */
  if (tc.activate(ac,av)) {
    bool valid_configuration_present = false;
    bool termination_requested       = false;

    /* Initialise utility pseudo-library */
    squadt_utility::initialise(tc);

    while (!termination_requested) {
      sip::message_ptr m = tc.await_message(sip::message_any);

      assert(m.get() != 0);

      switch (m->get_type()) {
        case sip::message_offer_configuration:

          /* Insert configuration in tool communicator object */
          valid_configuration_present = try_to_accept_configuration(tc);

          break;
        case sip::message_signal_start:
          if (valid_configuration_present) {
            lpe::specification lpe_specification;

            if (lpe_specification.load(tc.get_configuration().get_object(lpd_file_for_input)->get_location())) {
              using namespace sip;
              using namespace sip::layout;
              using namespace sip::layout::elements;
             
              lpe::LPE lpe = lpe_specification.lpe();
    
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
            
              /* Signal that the job is finished */
              tc.send_signal_done();
            }
            else {
              tc.send_status_report(sip::report::error, "Failure reading input from file: `" + file_name + "'\n");
            }
          }
          else {
            tc.send_status_report(sip::report::error, "Invalid configuration cannot proceed.");
          }
          break;
        case sip::message_request_termination:

          termination_requested = true;

          tc.send_signal_termination();

          break;
        default:
          /* Messages with a type that do not need to be handled */
          break;
      }
    }
  }
  else {
#endif
    parse_command_line(ac,av);

    lpe::specification lpe_specification;
 
    if (lpe_specification.load(file_name)) {
      lpe::LPE lpe = lpe_specification.lpe();
    
      cout << "Input read from " << ((file_name == "-") ? "standard input" : file_name) << endl << endl;
     
      cout << "Number of summands          : " << lpe.summands().size() << endl;
      cout << "Number of free variables    : " << lpe_specification.initial_free_variables().size() + lpe.free_variables().size() << endl;
      cout << "Number of process parameters: " << lpe.process_parameters().size() << endl; 
      cout << "Number of actions           : " << lpe.actions().size() << endl;
    }
    else {
      std::cerr << "Error: Unable to load LPE from `" + file_name + "'\n";
    }
#ifdef ENABLE_SQUADT_CONNECTIVITY
  }
#endif

  return 0;
}
