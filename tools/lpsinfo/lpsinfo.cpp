// ======================================================================
//
// Copyright (c) 2004 - 2007 TU/e
//
// ----------------------------------------------------------------------
//
// file          : lpsinfo 
// date          : 11-06-2007
// version       : 0.5.2
//
// author(s)     : Frank Stappers  <f.p.m.stappers@tue.nl>
//
// ======================================================================

// Squadt protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <mcrl2/utilities/squadt_interface.h>
#endif

//C++
#include <exception>
#include <cstdio>
#include <set>

//Boost
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

//mCRL2
#include <atermpp/aterm.h>
#include <mcrl2/lps/linear_process.h>
#include <mcrl2/lps/specification.h>
#include <print/messaging.h>
#include <mcrl2/utilities/aterm_ext.h>

//LPS framework
#include "mcrl2/lps/specification.h"

using namespace std;
using namespace atermpp;
using namespace ::mcrl2::utilities;

namespace po = boost::program_options;

#define VERSION "0.5.2"

/* Verbosity switch */
bool        verbose = false;

/* Name of the file to read input from (or standard input: "-") */
std::string file_name;

/* "is_tau_summand" taken from ../libraries/prover/source/confluence_checker.cpp */
bool is_tau_summand(ATermAppl a_summand) {
    ATermAppl v_multi_action_or_delta = ATAgetArgument(a_summand, 2);
    if (gsIsMultAct(v_multi_action_or_delta)) {
      return ATisEmpty(ATLgetArgument(v_multi_action_or_delta, 0));
    } else {
      return false;
    }
  }

int get_number_of_tau_summands(lps::linear_process lps) {
  int numOfTau = 0;
  for(lps::summand_list::iterator currentSummand = lps.summands().begin(); currentSummand != lps.summands().end(); currentSummand++){ 
	if ( is_tau_summand(*currentSummand)){
		numOfTau++;
	}
  }
  return numOfTau;
}

int get_number_of_used_actions(lps::linear_process lps){
  std::set<action_label > actionSet;
  for(lps::summand_list::iterator currentSummand = lps.summands().begin(); currentSummand != lps.summands().end(); currentSummand++){ 
	for(lps::action_list::iterator currentAction = currentSummand->actions().begin(); currentAction != currentSummand->actions().end(); currentAction++){
		actionSet.insert(currentAction->label());
	}
  }
  return actionSet.size();
}

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
    cerr << "Print basic information on the LPS in INFILE." << endl;
    cerr << endl;
    cerr << desc;

    exit (0);
  }
      
  if (vm.count("version")) {
    cerr << "lpsinfo " << VERSION << " (revision " << REVISION << ")" << endl;

    exit (0);
  }

  if (vm.count("debug")) {
    gsSetDebugMsg();
  }

  if (vm.count("verbose")) {
    gsSetVerboseMsg();
  }

  verbose  = 0 < vm.count("verbose");
  file_name = (0 < vm.count("INFILE")) ? vm["INFILE"].as< string >() : "-";
}
        
// SQuADT protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY

class squadt_interactor : public mcrl2::utilities::squadt::tool_interface {

  private:

    static const char*  lps_file_for_input;  ///< file containing an LPS that can be imported

  public:

    /** \brief configures tool capabilities */
    void set_capabilities(sip::tool::capabilities&) const;

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(sip::configuration&);

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(sip::configuration const&) const;

    /** \brief performs the task specified by a configuration */
    bool perform_task(sip::configuration&);
};

const char* squadt_interactor::lps_file_for_input  = "lps_in";

void squadt_interactor::set_capabilities(sip::tool::capabilities& c) const {
  c.add_input_combination(lps_file_for_input, sip::mime_type("lps", sip::mime_type::application), sip::tool::category::reporting);
}

void squadt_interactor::user_interactive_configuration(sip::configuration& c) {
}

bool squadt_interactor::check_configuration(sip::configuration const& c) const {
  bool result = true;

  result &= c.input_exists(lps_file_for_input);

  return (result);
}

bool squadt_interactor::perform_task(sip::configuration& c) {
  bool result = true;

  lps::specification lps_specification;

  if (lps_specification.load(c.get_input(lps_file_for_input).get_location())) {
    using namespace sip;
    using namespace sip::layout;
    using namespace sip::layout::elements;
   
    lps::linear_process lps = lps_specification.process();
  
    /* Create and add the top layout manager */
    layout::manager::aptr top(layout::horizontal_box::create());
  
    /* First column */
    layout::vertical_box* left_column = new layout::vertical_box();
  
    layout::vertical_box::alignment a = layout::left;
  
    left_column->add(new label("Input read from:"), a);
    left_column->add(new label("Summands (#):"), a);
    left_column->add(new label("Free variables (#):"), a);
    left_column->add(new label("Process parameters (#):"), a);
    left_column->add(new label("Action labels (#):"), a);
    left_column->add(new label("Used actions: (#):"), a);
    left_column->add(new label("Sorts (#):"), a);
    left_column->add(new label("\u03C4-Summands (#):"), a);
  
    /* Second column */
    layout::vertical_box* right_column = new layout::vertical_box();
  
    right_column->add(new label(file_name), a);
    right_column->add(new label(boost::lexical_cast < std::string > (lps.summands().size())), a);
    right_column->add(new label(boost::lexical_cast < std::string > ((lps_specification.initial_process().free_variables().size() + lps.free_variables().size()))), a);
    right_column->add(new label(boost::lexical_cast < std::string > (lps.process_parameters().size())), a);
    right_column->add(new label(boost::lexical_cast < std::string > (lps_specification.action_labels().size())), a);
    right_column->add(new label(boost::lexical_cast < std::string > (get_number_of_used_actions(lps))), a);
    right_column->add(new label(boost::lexical_cast < std::string > (lps_specification.data().sorts().size())), a);
    right_column->add(new label(boost::lexical_cast < std::string > (get_number_of_tau_summands(lps))), a);
  
    /* Attach columns*/
    top->add(left_column, margins(0,5,0,5));
    top->add(right_column, margins(0,5,0,20));
  
    send_display_layout(top);
  }
  else {
    send_error("Failure reading input from file: `" + file_name + "'\n");

    result = false;
  }

  return (result);
}
#endif

int main(int argc, char** argv) {
  ATerm bottom;

  ATinit(argc,argv,&bottom);

  gsEnableConstructorFunctions();

#ifdef ENABLE_SQUADT_CONNECTIVITY
  if (!mcrl2::utilities::squadt::interactor< squadt_interactor >::free_activation(argc, argv)) {
#endif
    parse_command_line(argc,argv);

    lps::specification lps_specification;
 
    if (lps_specification.load(file_name)) {
      lps::linear_process lps = lps_specification.process();
		 
      cout << "Input read from " << ((file_name == "-") ? "standard input" : file_name) << endl << endl;
     
      cout << "Number of summands                    : " << lps.summands().size() << endl;
      cout << "Number of free variables              : " << lps_specification.initial_process().free_variables().size() + lps.free_variables().size() << endl;
      cout << "Number of process parameters          : " << lps.process_parameters().size() << endl; 
      cout << "Number of action labels               : " << lps_specification.action_labels().size() << endl;
      cout << "Number of used versus declared actions: " << get_number_of_used_actions(lps) << "/"<< lps_specification.action_labels().size() << endl;
      //cout << "Number of used versus declared multi-actions: " << "" << endl;
      cout << "Number of used sorts                  : " << lps_specification.data().sorts().size() << endl;
      cout << "Number of \u03C4-summands                  : " << get_number_of_tau_summands(lps) << endl; 
    }
    else {
      std::cerr << "Error: Unable to load LPS from `" + file_name + "'\n";
    }
#ifdef ENABLE_SQUADT_CONNECTIVITY
  }
#endif

  return 0;
}
