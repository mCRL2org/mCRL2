// ======================================================================
//
// Copyright (c) 2006 TU/e
//
// ----------------------------------------------------------------------
//
// file          : lpeuntime 
// date          : 07-11-2006
// version       : 0.3
//
// author(s)     : Jeroen Keiren <j.j.a.keiren@student.tue.nl>
//
// ======================================================================

//C++
#include <exception>
#include <cstdio>

//Boost
#include <boost/program_options.hpp>

//Lowlevel library for gsErrorMsg
#include <libprint_c.h>

//Aterm
#include <atermpp/aterm.h>

//LPE framework
#include <lpe/lpe.h>
#include <lpe/specification.h>
#include <lpe/utility.h>

//Squadt connectivity
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <sip/tool.h>
#include <squadt_utility.h>
#endif


using namespace std;
using namespace atermpp;
using namespace lpe;

namespace po = boost::program_options;

#define VERSION "0.3"

std::string input_file; // Name of the file to read input from
std::string output_file; // Name of the file to write output to (or stdout)

#ifdef ENABLE_SQUADT_CONNECTIVITY
//Forward declaration needed for use within squadt_lpeuntime class
int do_untime();
class squadt_lpeuntime: public squadt_tool_interface
{
  private:
    enum input_files {
      lpd_file_for_input, ///< file containing an lpd that can be imported
      lpd_file_for_output ///< file used to write output to
    };

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

void squadt_lpeuntime::set_capabilities(sip::tool::capabilities& capabilities) const
{
  // The tool has only one main input combination
  capabilities.add_input_combination(lpd_file_for_input, sip::mime_type("lpe"), sip::tool::category::transformation);
}

void squadt_lpeuntime::user_interactive_configuration(sip::configuration& configuration)
{
  configuration.add_output(lpd_file_for_output, sip::mime_type("lpe"), configuration.get_output_name(".lpe"));
}

bool squadt_lpeuntime::check_configuration(sip::configuration const& configuration) const
{
// Check if everything present
  return (configuration.object_exists(lpd_file_for_input) &&
          configuration.object_exists(lpd_file_for_output)
         );
}

bool squadt_lpeuntime::perform_task(sip::configuration& configuration)
{
  input_file = configuration.get_object(lpd_file_for_input)->get_location();
  output_file = configuration.get_object(lpd_file_for_output)->get_location();
  return (do_untime()==0);
}

#endif //ENABLE_SQUADT_CONNECTIVITY

bool has_time(lpe::LPE& lpe)
{
  bool result = true;
  for (lpe::summand_list::iterator i = lpe.summands().begin(); i != lpe.summands().end(); ++i)
  {
    result = result && i->has_time();
  }
  return result;
}

///\ret specification, in which all delta summands have been removed, and replaced with a single true->delta
lpe::specification remove_deltas(const lpe::specification& specification) {
  lpe::specification result;
  lpe::summand_list summands;
  for (summand_list::iterator i = specification.lpe().summands().begin(); i != specification.lpe().summands().end(); ++i)
  {
    if (!(i->is_delta()))
    {
      summands = push_front(summands, *i);
    }
  }

  lpe::LPE_summand delta_summand = LPE_summand(data_variable_list(),
                                               data_expression(gsMakeDataExprTrue()),
                                               true,
                                               action_list(),
                                               gsMakeNil(),
                                               data_assignment_list()
                                              );

  summands = push_front(summands, delta_summand);
  summands = atermpp::reverse(summands);

  result = set_lpe(specification, set_summands(specification.lpe(), summands));

  return result;
}

///Returns an LPE specification in which the timed arguments have been rewritten
lpe::specification untime(const lpe::specification& specification) {
  // TODO: Strip use of gs functions as much as possible; everything that's possible through these
  // should also be available through the LPE library!
  // NOTE: The gs functions will be made available in the LPE library by Wieger.

  lpe::specification untime_specification; // Updated specification
  lpe::LPE lpe = specification.lpe(); // Original lpe
  lpe::LPE untime_lpe; // Updated lpe
  lpe::summand_list untime_summand_list; // Updated summand list
  lpe::data_variable_list untime_process_parameters; // Updated process parameters
  lpe::data_variable last_action_time; // Extra parameter to display the last action time
  lpe::data_variable_list untime_initial_variables; // Updated initial variables
  lpe::data_expression_list untime_initial_state; // Updated initial state
  // Note: initial variables and initial state together form initial assignment

  //If an lpe has got no time at the initialization, return the original lpe with all present delta's removed, and replaced with one true->delta.
  if (!has_time(lpe))
  {
    return remove_deltas(specification);
  }

  // Create extra parameter last_action_time and add it to the list of process parameters,
  // last_action_time is used later on in the code
  last_action_time = fresh_variable("last_action_time", aterm_appl(specification));
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

    if (!(i->is_delta())){

      if (i->has_time()) 
      { 
	// The summand is already timed, therefor there is no need to add an extra summation variable for time
	untime_summation_variables = i->summation_variables();   

	// Extend the original condition with an additional argument t.i(d,e.i)>last_action_time
	untime_condition = gsMakeDataExprAnd(i->condition(), 
                                             lpe::greater(i->time(),data_expression(last_action_time))
                                             );

	// Extend original assignments to include t.i(d,e.i)
	untime_assignments = push_back(i->assignments(),data_assignment(last_action_time,i->time()));
      }
      else
      {

	// Add a new summation variable (this is allowed because according to an axiom the following equality holds):
	// c -> a . X == sum t:Real . c -> a@t . X
	lpe::data_variable time_var = fresh_variable("time_var", aterm_appl(specification));
	untime_summation_variables = push_back(i->summation_variables(), time_var);

	// Extend the original condition with an additional argument
	untime_condition = gsMakeDataExprAnd(i->condition(),
                                             lpe::greater(time_var, data_expression(last_action_time))
                                             );


	// Extend original assignments to include t.i(d,e.i)
	untime_assignments = push_back(i->assignments(),
                                      data_assignment(last_action_time, time_var)
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

    } // !(i->is_delta())

  }

  // Add delta summand
  lpe::LPE_summand untime_summand;
  untime_summand = lpe::LPE_summand(data_variable_list(),
                                    data_expression(gsMakeDataExprTrue()),
                                    true,
                                    action_list(),
                                    gsMakeNil(),
                                    data_assignment_list()
                                   );

  untime_summand_list = push_front(untime_summand_list, untime_summand);
  
  // Reverse summand list, because it is the wrong way round now.
  untime_summand_list = atermpp::reverse(untime_summand_list);
      
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

int do_untime()
{
  lpe::specification lpe_specification;

  if (lpe_specification.load(input_file)) {
    // Untime lpe_specification and save the output to a binary file
    if (!untime(lpe_specification).save(output_file, true)) 
    {
      // An error occurred when saving
      gsErrorMsg("Could not save to '%s'\n", output_file.c_str());
      return (1);
    }
  }
  else {
    gsErrorMsg("lpeuntime: Unable to load LPE from `%s'\n", input_file.c_str());
    return (1);
  }

  return 0;
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

int main(int ac, char** av) {
  ATerm bot;
  ATinit(0,0,&bot);
  gsEnableConstructorFunctions();

#ifdef ENABLE_SQUADT_CONNECTIVITY
  squadt_lpeuntime sl;
  if (sl.try_interaction(ac, av)) {
    return 0;
  }
#endif

  parse_command_line(ac,av);
  return do_untime();
}
