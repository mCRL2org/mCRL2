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

//C++
#include <exception>
#include <cstdio>

//Boost
#include <boost/program_options.hpp>

//Aterm
#include <mcrl2/atermpp/aterm.h>

//LPS framework
#include <mcrl2/lps/specification.h>
#include <mcrl2/lps/linear_process.h>
#include "mcrl2/core/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"
#include "mcrl2/utilities/version_info.h"
#include <mcrl2/data/data.h>
#include <mcrl2/data/utility.h>

using namespace std;
using namespace atermpp;
using namespace lps;
using namespace lps::data_expr;
using namespace mcrl2::utilities;

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

bool has_time(lps::linear_process& lps)
{
  for (lps::summand_list::iterator i = lps.summands().begin(); i != lps.summands().end(); ++i)
  {
    if(i->has_time()) return true;
  }
  return false;
}

///\ret specification, in which all delta summands have been removed, and replaced with a single true->delta
lps::specification remove_deltas(const lps::specification& specification) {
  lps::specification result;
  lps::summand_list summands;
  for (summand_list::iterator i = specification.process().summands().begin(); i != specification.process().summands().end(); ++i)
  {
    if (!(i->is_delta()))
    {
      summands = push_front(summands, *i);
    }
  }

  lps::summand delta_summand = summand(data_variable_list(),
                                       true_(),
                                       true,
                                       action_list(),
                                       data_assignment_list()
                                      );

  summands = push_front(summands, delta_summand);
  summands = atermpp::reverse(summands);

  result = set_lps(specification, set_summands(specification.process(), summands));

  return result;
}

///Returns an LPS specification in which the timed arguments have been rewritten
lps::specification untime(const lps::specification& specification) {
  lps::specification untime_specification; // Updated specification
  lps::linear_process lps = specification.process(); // Original lps
  lps::linear_process untime_lps; // Updated lps
  lps::summand_list untime_summand_list; // Updated summand list
  lps::data_variable_list untime_process_parameters; // Updated process parameters
  lps::data_variable last_action_time; // Extra parameter to display the last action time
  lps::data_assignment_list untime_initial_assignments; // Updated initial assignments

  gsVerboseMsg("Untiming %d summands\n", lps.summands().size());
  
  //If an lps has got no time at the initialization, return the original lps with all present delta's removed, and replaced with one true->delta.
  if (!has_time(lps))
  {
    gsVerboseMsg("LPS has no time, only removing deltas, and replacing with one true->delta summand\n");
    return remove_deltas(specification);
  }

  // Create extra parameter last_action_time and add it to the list of process parameters,
  // last_action_time is used later on in the code
  last_action_time = fresh_variable(aterm_appl(specification), lps::sort_expr::real(), "last_action_time");
  untime_process_parameters = push_back(lps.process_parameters(), last_action_time);
      
  // Transpose the original summand list, and see if there are summands with time
  // If a summand has time, remove it, create new conditions for time, and add it to the new summand list (untime_summand_list)
  // If a summand does not contain time, first introduce time, and then untime it.
  int j = 0; //Counter only used for verbose output (keep track of the summand number
  for (lps::summand_list::iterator i = lps.summands().begin(); i != lps.summands().end(); ++i,++j)
  { 
    gsVerboseMsg("Untiming summand %d\n", j);

    // Declarations within scope of for-loop
    lps::data_variable_list untime_summation_variables; //Updated set of summation variables
    lps::data_expression untime_condition; //Updated condition
    lps::data_assignment_list untime_assignments; //Updated assignments (or next state)
    lps::summand untime_summand; //Updated summand

    // Only untime summands that are not delta summands; all delta summands are removed, and replaced by one true->delta summand
    if (!(i->is_delta())){

      if (i->has_time()) 
      { 
	// The summand is already timed, therefor there is no need to add an extra summation variable for time
	untime_summation_variables = i->summation_variables();

	// Extend the original condition with an additional argument t.i(d,e.i)>last_action_time
	untime_condition = and_(i->condition(), lps::data_expr::greater(i->time(),data_expression(last_action_time)));

	// Extend original assignments to include t.i(d,e.i)
	untime_assignments = push_back(i->assignments(),data_assignment(last_action_time,i->time()));
      }
      else
      {

	// Add a new summation variable (this is allowed because according to an axiom the following equality holds):
	// c -> a . X == sum t:Real . c -> a@t . X
	lps::data_variable time_var = fresh_variable(aterm_appl(specification), lps::sort_expr::real(), "time_var");
	untime_summation_variables = push_back(i->summation_variables(), time_var);

	// Extend the original condition with an additional argument
	untime_condition = and_(i->condition(), lps::data_expr::greater(time_var, data_expression(last_action_time)));


	// Extend original assignments to include t.i(d,e.i)
	untime_assignments = push_back(i->assignments(),
                                      data_assignment(last_action_time, time_var)
                                     );
      } // i->has_time()

      // Create a new summand with the changed parameters
      untime_summand = lps::summand(untime_summation_variables,
					  untime_condition,
					  i->is_delta(),
					  i->actions(),
					  untime_assignments
					  );

      // Add the new summand to the list
      untime_summand_list = push_front(untime_summand_list, untime_summand);

    } // !(i->is_delta())

  }

  // Add delta summand
  lps::summand untime_summand;
  untime_summand = lps::summand(data_variable_list(),
                                data_expression(true_()),
                                true,
                                action_list(),
                                data_assignment_list()
                               );

  untime_summand_list = push_front(untime_summand_list, untime_summand);
  
  // Reverse summand list, because it is the wrong way round now.
  untime_summand_list = atermpp::reverse(untime_summand_list);
      
  // Create new LPS, this equals lps, except for the new summand list and the additional process parameter.
  untime_lps = lps::linear_process(lps.free_variables(), untime_process_parameters, untime_summand_list);

  // Create new initial_variables and initial_state in order to correctly initialize.
  untime_initial_assignments = push_back(specification.initial_process().assignments(), data_assignment(last_action_time, real(0)));

  // Create new specification, this equals original specification, except for the new LPS.
  untime_specification = lps::specification(specification.data(), 
			                    specification.action_labels(),
					    untime_lps, //new LPS
					    process_initializer(specification.initial_process().free_variables(),
                                            untime_initial_assignments)
					   );

  return untime_specification;
}

int do_untime(const tool_options& options)
{
  lps::specification lps_specification;

  try
  {
    lps_specification.load(options.input_file);

    // Untime lps_specification and save the output to a binary file
    if (!untime(lps_specification).save(options.output_file, true)) 
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
    cerr << "Usage: "<< av[0] << " [OPTION]... [INFILE] [OUTFILE]" << endl;
    cerr << "Untime the LPS in INFILE and store the result to OUTFILE" << endl;
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
