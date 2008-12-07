// Author(s): F.P.M. (Frank) Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./lpsinfo.cpp

#include "boost.hpp" // precompiled headers

#define NAME "lpsinfo"
#define AUTHOR "Frank Stappers"

// Squadt protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <mcrl2/utilities/mcrl2_squadt_interface.h>
#endif

//C++
#include <exception>
#include <cstdio>
#include <set>

//Boost
#include <boost/lexical_cast.hpp>

//mCRL2
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/lps/linear_process.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/command_line_messaging.h"

//LPS framework
#include "mcrl2/lps/specification.h"

using namespace std;
using namespace atermpp;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::lps;

/* Name of the file to read input from (or standard input if empty) */
std::string file_name;

/* "is_tau_summand" taken from ../libraries/prover/source/confluence_checker.cpp */
static inline bool is_tau_summand(ATermAppl a_summand) {
    ATermAppl v_multi_action_or_delta = ATAgetArgument(a_summand, 2);
    if (mcrl2::core::detail::gsIsMultAct(v_multi_action_or_delta)) {
      return ATisEmpty(ATLgetArgument(v_multi_action_or_delta, 0));
    } else {
      return false;
    }
  }

static inline int get_number_of_tau_summands(linear_process lps) {
  int numOfTau = 0;
  for(summand_list::iterator currentSummand = lps.summands().begin(); currentSummand != lps.summands().end(); ++currentSummand){ 
	if ( is_tau_summand(*currentSummand)){
		++numOfTau;
	}
  }
  return numOfTau;
}

static inline int get_number_of_used_actions(linear_process lps){
  std::set<action_label > actionSet;
  for(summand_list::iterator currentSummand = lps.summands().begin(); currentSummand != lps.summands().end(); ++currentSummand){ 
	for(action_list::iterator currentAction = currentSummand->actions().begin(); currentAction != currentSummand->actions().end(); ++currentAction){
		actionSet.insert(currentAction->label());
	}
  }
  return actionSet.size();
}

void parse_command_line(int ac, char** av) {
  interface_description clinterface(av[0], NAME, AUTHOR, "[OPTION]... [INFILE]\n",
                           "Print basic information on the linear process specification (LPS) in INFILE.");

  command_line_parser parser(clinterface, ac, av);

  if (0 < parser.arguments.size()) {
    file_name = parser.arguments[0];
  }
  if (1 < parser.arguments.size()) {
    parser.error("too many file arguments");
  }
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

  specification lps_specification;

  lps_specification.load(c.get_input(lps_file_for_input).location());

  linear_process lps = lps_specification.process();

  /* Create display */
  tipi::tool_display d;

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
                append(d.create< label >().set_text(c.get_input(lps_file_for_input).location())).
                append(d.create< label >().set_text(boost::lexical_cast< std::string > (lps.summands().size()))).
                append(d.create< label >().set_text(boost::lexical_cast< std::string > (get_number_of_tau_summands(lps)))).
                append(d.create< label >().set_text(boost::lexical_cast< std::string > ((lps_specification.initial_process().free_variables().size() + lps.free_variables().size())))).
                append(d.create< label >().set_text(boost::lexical_cast< std::string > (lps.process_parameters().size()))).
                append(d.create< label >().set_text(boost::lexical_cast< std::string > (lps_specification.action_labels().size()))).
                append(d.create< label >().set_text(boost::lexical_cast< std::string > (get_number_of_used_actions(lps)))).
                append(d.create< label >().set_text(boost::lexical_cast< std::string > (lps_specification.data().sorts().size()))));

  send_display_layout(d.manager(m));

  return true;
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
    parse_command_line(argc,argv);

    specification lps_specification;
 
    lps_specification.load(file_name);
    linear_process lps = lps_specification.process();
      	 
    cout << "Input read from " << (file_name.empty()?"stdin":("'" + file_name + "'")) << endl;
    cout << endl;   
    cout << "Number of summands                    : " << lps.summands().size() << endl;
    cout << "Number of tau-summands                : " << get_number_of_tau_summands(lps) << endl; 
    cout << "Number of free variables              : " << lps_specification.initial_process().free_variables().size() + lps.free_variables().size() << endl;
    cout << "Number of process parameters          : " << lps.process_parameters().size() << endl; 
    cout << "Number of action labels               : " << lps_specification.action_labels().size() << endl;
    cout << "Number of used versus declared actions: " << get_number_of_used_actions(lps) << "/"<< lps_specification.action_labels().size() << endl;
    //cout << "Number of used versus declared multi-actions: " << "" << endl;
    cout << "Number of used sorts                  : " << lps_specification.data().sorts().size() << endl;

    return EXIT_SUCCESS;
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return EXIT_FAILURE;
}
