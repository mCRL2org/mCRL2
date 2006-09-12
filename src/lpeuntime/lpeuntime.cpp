// ======================================================================
//
// Copyright (c) 2006 TU/e
//
// ----------------------------------------------------------------------
//
// file          : lpeuntime 
// date          : 12-09-2006
// version       : 0.2
//
// author(s)     : Jeroen Keiren <j.j.a.keiren@student.tue.nl>
//
// Based on the framework in lpeinfo by Frank Stappers
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

//Lowlevel library for gsErrorMsg
#include <libprint_c.h>

using namespace std;
using namespace atermpp;
using namespace lpe;

namespace po = boost::program_options;

#define VERSION "0.2"

std::string input_file; // Name of the file to read input from
std::string output_file; // Name of the file to write output to (or stdout)

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
    cerr << "Usage: "<< av[0] << " [OPTION]... [INFILE] [OUTFILE]" << endl;
    cerr << "Untime the LPE in INFILE and store the result to OUTFILE" << endl;
    cerr << endl;
    cerr << desc;

    exit (0);
  }
      
  if (vm.count("version")) {
    cerr << "lpeuntime " << VERSION << " (revision " << REVISION << ")" << endl;

    exit (0);
  }

  if (vm.count("debug")) {
    gsSetDebugMsg();
  }

  if (vm.count("verbose")) {
    gsSetVerboseMsg();
  }

  input_file = (0 < vm.count("INFILE")) ? vm["INFILE"].as< string >() : "-";
  output_file = (0 < vm.count("OUTFILE")) ? vm["OUTFILE"].as< string >() : "-";
}

///Returns an LPE specification in which the timed arguments have been rewritten
lpe::specification untime(const lpe::specification specification) {
  // TODO: Strip use of gs functions as much as possible; everything that's possible through these
  // should also be available through the LPE library!
  // NOTE: The gs functions will be made available in the LPE library by Wieger.
  lpe::specification untime_specification; // Updated specification
  lpe::LPE lpe; // Original lpe
  lpe::LPE untime_lpe; // Updated lpe
  lpe::summand_list untime_summand_list; // Updated summand list
  lpe::data_variable_list untime_process_parameters; // Updated process parameters
  lpe::data_variable last_action_time; // Extra parameter to display the last action time
  lpe::data_variable_list untime_initial_variables; // Updated initial variables
  lpe::data_expression_list untime_initial_state; // Updated initial state
  // Note: initial variables and initial state together form initial assignment
 
  // init
  untime_summand_list = lpe::summand_list();
  lpe = specification.lpe();

  // Create extra parameter last_action_time and add it to the list of process parameters,
  // last_action_time is used later on in the code
  last_action_time = data_variable("last_action_time", lpe::sort("Real"));
  untime_process_parameters = push_back(lpe.process_parameters(), last_action_time);
      
  // Transpose the original summand list, and see if there are summands with time
  // If a summand has time, remove it, create new conditions for time, and add it to the new summand list (untime_summand_list)
  // If a summand does not contain time, first introduce time, and then untime it.
  for (lpe::summand_list::iterator i = lpe.summands().begin(); i != lpe.summands().end(); ++i)
  { 
    // Declarations within scope of for-loop
    lpe::data_variable_list untime_summation_variables; //Updated set of summation variables
    lpe::data_expression untime_condition; //Updated condition
    lpe::data_assignment_list untime_assignments; //Updated assignments (or next state)
    lpe::LPE_summand untime_summand; //Updated summand

    if (i->has_time()) 
    { 
      // The summand is already timed, therefor there is no need to add an extra summation variable for time
      untime_summation_variables = i->summation_variables();   

      // Extend the original condition with an additional argument t.i(d,e.i)>last_action_time
      untime_condition = gsMakeDataExprAnd(i->condition(), 
                                           gsMakeDataExprGT(i->time(), 
                                                            last_action_time.to_expr()
                                                           )
                                           );

      // Extend original assignments to include t.i(d,e.i)
      untime_assignments = push_back(i->assignments(),data_assignment(last_action_time,i->time()));
    }
    else
    {
          
      // Add a new summation variable (this is allowed because according to an axiom the following equality holds):
      // c -> a . X == sum t:Real . c -> a@t . X
      lpe::data_variable time_var;
      time_var = data_variable("t", lpe::sort("Real")); // TODO: See if we can auto-name the variable (in order to prevent name collisions)
      untime_summation_variables = push_back(i->summation_variables(), time_var);
 
      // Extend the original condition with an additional argument
      untime_condition = gsMakeDataExprAnd(i->condition(),
                                           gsMakeDataExprGT(time_var,
                                                            last_action_time.to_expr()
                                                           )
                                           );


      // Extend original assignments to include t.i(d,e.i)
      untime_assignments = push_back(i->assignments(),
                                    data_assignment(last_action_time, time_var.to_expr())
                                   );
    } // i->has_time()

    // Create a new summand with the changed parameters
    untime_summand = lpe::LPE_summand(untime_summation_variables,
					untime_condition,
					i->is_delta(),
					i->actions(),
					gsMakeNil(), // new time
					untime_assignments
					);

    // Add the new summand to the list
    untime_summand_list = push_front(untime_summand_list, untime_summand);

  }
 
  // Revert summand list, because it is the wrong way round now.
  untime_summand_list = reverse(untime_summand_list);
      
  // Create new LPE, this equals lpe, except for the new summand list and the additional process parameter.
  untime_lpe = lpe::LPE(lpe.free_variables(), untime_process_parameters, untime_summand_list, lpe.actions());


  // Create new initial_variables and initial_state in order to correctly initialize.
  // NOTE: The initial assignments are calculated at creation 
  // time by "zipping" the initial_variables and the initial_state
  untime_initial_variables = push_back(specification.initial_variables(), last_action_time);
  untime_initial_state = push_back(specification.initial_state(), data_expression(gsMakeDataExprInt2Real(gsMakeDataExprInt("0"))));

  // Create new specification, this equals original specification, except for the new LPE.
  untime_specification = lpe::specification(specification.sorts(), 
						specification.constructors(),
						specification.mappings(),
						specification.equations(),
						specification.actions(),
						untime_lpe, //new LPE
						specification.initial_free_variables(),
						untime_initial_variables, //new initial variables
						untime_initial_state // new initial state
						);

  return untime_specification;
}
        
int main(int ac, char** av) {
  ATerm bot;
  ATinit(0,0,&bot);
  gsEnableConstructorFunctions();

#ifdef ENABLE_SQUADT_CONNECTIVITY
/*
  sip::tool::communicator tc;

  // Constants for identifiers of options and objects
  const unsigned int lpd_file_for_input = 0;

  // Get tool capabilities in order to modify settings
  sip::tool::capabilities& cp = tc.get_tool_capabilities();

  // The tool has only one main input combination it takes an LPD and then behaves as a reporter
  cp.add_input_combination(lpd_file_for_input, "Reporting", "lpe");

  // On purpose we do not catch exceptions 
  if (tc.activate(ac,av)) {
    bool valid = false;

    // Initialise utility pseudo-library 
    squadt_utility::initialise(tc);

    // Static configuration cycle
    while (!valid) {
      // Wait for configuration data to be send (either a previous configuration, or only an input combination)
      sip::configuration::sptr configuration = tc.await_configuration();

      // Validate configuration specification, should contain a file name of an LPD that is to be read as input
      valid  = configuration.get() != 0;
      valid &= configuration->object_exists(lpd_file_for_input);

      if (valid) {
        // An object with the correct id exists, assume the URI is relative (i.e. a file name in the local file system)
        input_file = configuration->get_object(lpd_file_for_input)->get_location();
      }
      else {
        tc.send_status_report(sip::report::error, "Invalid input combination!");
      }
    }

    // Send the controller the signal that we're ready to rumble (no further configuration necessary)
    tc.send_accept_configuration();

    // Wait for start message
    tc.await_message(sip::message_signal_start);
  }
  else {
*/
#endif
    parse_command_line(ac,av);
#ifdef ENABLE_SQUADT_CONNECTIVITY
/*
  }
*/
#endif

  lpe::specification lpe_specification;

  if (lpe_specification.load(input_file)) {

// TODO: Change the following to follow lpeuntime output instead of lpeinfo    
#ifdef ENABLE_SQUADT_CONNECTIVITY
/*
    if (tc.is_active()) {
      using namespace sip;
      using namespace sip::layout;
      using namespace sip::layout::elements;

      layout::tool_display::sptr display(new layout::tool_display);

      // Create and add the top layout manager 
      layout::manager::aptr layout_manager = layout::horizontal_box::create();

      // First column 
      layout::vertical_box* left_column = new layout::vertical_box();

      layout::vertical_box::alignment a = layout::left;

      left_column->add(new label("Input read from:"), a);
      left_column->add(new label("Summands (#):"), a);
      left_column->add(new label("Free variables (#):"), a);
      left_column->add(new label("Process parameters (#):"), a);
      left_column->add(new label("Actions (#):"), a);

      // Second column
      layout::vertical_box* right_column = new layout::vertical_box();

      boost::format c("%u");

      right_column->add(new label(boost::filesystem::path(input_file).leaf()), a);
      right_column->add(new label(boost::str(c % lpe.summands().size())), a);
      right_column->add(new label(boost::str(c % (lpe_specification.initial_free_variables().size() + lpe.free_variables().size()))), a);
      right_column->add(new label(boost::str(c % lpe.process_parameters().size())), a);
      right_column->add(new label(boost::str(c % lpe.actions().size())), a);

      // Attach columns
      layout_manager->add(left_column, margins(0,5,0,5));
      layout_manager->add(right_column, margins(0,5,0,20));

      display->set_top_manager(layout_manager);

      tc.send_display_layout(display);

      // Termination sequence
      tc.send_signal_done();

      tc.await_message(sip::message_request_termination);
    }
    else {
*/
#endif

      // Untime lpe_specification and save the output to a binary file
      if (!untime(lpe_specification).save(output_file, true)) 
      {
        // An error occurred when saving
        gsErrorMsg("Could not save to '%s'\n", output_file.c_str());
      }

#ifdef ENABLE_SQUADT_CONNECTIVITY
/*
    }
*/
#endif
  }
  else {
    std::string error("Unable to load LPE from `" + input_file + "'\n");

#ifdef ENABLE_SQUADT_CONNECTIVITY
/*
    if (tc.is_active()) {
      // Something went wrong, send an error report
      tc.send_status_report(sip::report::error, error);

      tc.send_signal_done();

      tc.await_message(sip::message_request_termination);
    }
    else {
      std::cerr << "Error: " + error;
    }
*/
#else
    std::cerr << "Error: " + error;
#endif

    return (1);
  }

  return 0;
}
