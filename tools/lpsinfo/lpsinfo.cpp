//  Copyright 2007 F.P.M. (Frank) Stappers. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./lpsinfo.cpp

#define NAME "lpsinfo"

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
#include <mcrl2/atermpp/aterm.h>
#include <mcrl2/lps/linear_process.h>
#include <mcrl2/lps/specification.h>
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/core_init.h"
#include <mcrl2/utilities/aterm_ext.h>
#include "mcrl2/utilities/version_info.h"

//LPS framework
#include "mcrl2/lps/specification.h"

using namespace std;
using namespace atermpp;
using namespace ::mcrl2::utilities;

namespace po = boost::program_options;

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
    print_version_information(NAME);
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

class squadt_interactor : public mcrl2::utilities::squadt::mcrl2_tool_interface {

  private:

    static const char*  lps_file_for_input;  ///< file containing an LPS that can be imported

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

void squadt_interactor::set_capabilities(tipi::tool::capabilities& c) const {
  c.add_input_configuration(lps_file_for_input, tipi::mime_type("lps", tipi::mime_type::application), tipi::tool::category::reporting);
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& c) {
}

bool squadt_interactor::check_configuration(tipi::configuration const& c) const {
  bool result = true;

  result &= c.input_exists(lps_file_for_input);

  return (result);
}

bool squadt_interactor::perform_task(tipi::configuration& c) {
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::layout::elements;
 
  lps::specification lps_specification;

  lps_specification.load(c.get_input(lps_file_for_input).get_location());

  lps::linear_process lps = lps_specification.process();

  /* Create display */
  tipi::layout::tool_display d;

  layout::horizontal_box& m = d.create< horizontal_box >().set_default_margins(margins(0, 5, 0, 5));

  /* First column */
  m.append(d.create< vertical_box >().set_default_alignment(layout::left).
                append(d.create< label >().set_text("Input read from:")).
                append(d.create< label >().set_text("Summands (#):")).
                append(d.create< label >().set_text("Tau-summands (#):")).
                append(d.create< label >().set_text("Free variables (#):")).
                append(d.create< label >().set_text("Process parameters (#):")).
                append(d.create< label >().set_text("Action labels (#):")).
                append(d.create< label >().set_text("Used actions: (#):")).
                append(d.create< label >().set_text("Sorts (#):")));

  /* Second column */
  m.append(d.create< vertical_box >().set_default_alignment(layout::left).
                append(d.create< label >().set_text(c.get_input(lps_file_for_input).get_location())).
                append(d.create< label >().set_text(boost::lexical_cast< std::string > (lps.summands().size()))).
                append(d.create< label >().set_text(boost::lexical_cast< std::string > (get_number_of_tau_summands(lps)))).
                append(d.create< label >().set_text(boost::lexical_cast< std::string > ((lps_specification.initial_process().free_variables().size() + lps.free_variables().size())))).
                append(d.create< label >().set_text(boost::lexical_cast< std::string > (lps.process_parameters().size()))).
                append(d.create< label >().set_text(boost::lexical_cast< std::string > (lps_specification.action_labels().size()))).
                append(d.create< label >().set_text(boost::lexical_cast< std::string > (get_number_of_used_actions(lps)))).
                append(d.create< label >().set_text(boost::lexical_cast< std::string > (lps_specification.data().sorts().size()))));

  send_display_layout(d.set_manager(m));

  return true;
}
#endif

int main(int argc, char** argv) {
  MCRL2_CORE_LIBRARY_INIT()

#ifdef ENABLE_SQUADT_CONNECTIVITY
  if (!mcrl2::utilities::squadt::interactor< squadt_interactor >::free_activation(argc, argv)) {
#endif
    parse_command_line(argc,argv);

    lps::specification lps_specification;
 
    try
    {
      lps_specification.load(file_name);
      lps::linear_process lps = lps_specification.process();
		 
      cout << "Input read from " << ((file_name == "-") ? "standard input" : file_name) << endl << endl;
     
      cout << "Number of summands                    : " << lps.summands().size() << endl;
      cout << "Number of tau-summands                : " << get_number_of_tau_summands(lps) << endl; 
      cout << "Number of free variables              : " << lps_specification.initial_process().free_variables().size() + lps.free_variables().size() << endl;
      cout << "Number of process parameters          : " << lps.process_parameters().size() << endl; 
      cout << "Number of action labels               : " << lps_specification.action_labels().size() << endl;
      cout << "Number of used versus declared actions: " << get_number_of_used_actions(lps) << "/"<< lps_specification.action_labels().size() << endl;
      //cout << "Number of used versus declared multi-actions: " << "" << endl;
      cout << "Number of used sorts                  : " << lps_specification.data().sorts().size() << endl;
    }
    catch (std::runtime_error e)
    {
      std::cerr << "Error: Unable to load LPS from `" + file_name + "'\n";
    }
#ifdef ENABLE_SQUADT_CONNECTIVITY
  }
#endif

  return 0;
}
