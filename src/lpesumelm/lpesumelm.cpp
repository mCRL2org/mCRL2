// ======================================================================
//
// Copyright (c) 2006 TU/e
//
// ----------------------------------------------------------------------
//
// file          : lpesumelm 
// date          : 22-09-2006
// version       : 0.1
//
// author(s)     : Jeroen Keiren <j.j.a.keiren@student.tue.nl>
//
// ======================================================================

#include "squadt_lpesumelm.h"

#include "mcrl2_revision.h"

//C++
#include <exception>
#include <cstdio>

//Boost
#include <boost/program_options.hpp>
#include <boost/filesystem/convenience.hpp>

//Lowlevel library for gsErrorMsg
#include <libprint_c.h>

//For find operations on aterms
#include "atermpp/algorithm.h"

using namespace std;
using namespace atermpp;
using namespace lpe;

namespace po = boost::program_options;

#define VERSION "0.1"

std::string input_file; // Name of the file to read input from
std::string output_file; // Name of the file to write output to (or stdout)

#ifdef ENABLE_SQUADT_CONNECTIVITY
const unsigned int lpd_file_for_input = 0;
const unsigned int lpd_file_for_output = 1;

void squadt_lpesumelm::set_capabilities()
{
  // Get tool capabilities in order to modify settings
  sip::tool::capabilities& cp = tc.get_tool_capabilities();

  // The tool has only one main input combination
  cp.add_input_combination(lpd_file_for_input, "Transformation", "lpe");
}

void squadt_lpesumelm::initialise()
{
}

void squadt_lpesumelm::configure(sip::configuration& configuration)
{
  configuration.add_output(lpd_file_for_output, "lpe", configuration.get_output_name(".lpe"));
}

bool squadt_lpesumelm::check_configuration(sip::configuration& configuration)
{
// Check if everything present (see lpe2lts)
  return (configuration.object_exists(lpd_file_for_input) &&
          configuration.object_exists(lpd_file_for_output)
         );
}

void squadt_lpesumelm::execute(sip::configuration& configuration)
{
  input_file = configuration.get_object(lpd_file_for_input)->get_location();
  output_file = configuration.get_object(lpd_file_for_output)->get_location();
  do_sumelm();
}

void squadt_lpesumelm::finalise()
{
}
#endif

////////////////////////////////////////////////////////////////
// Auxiliary functions used to support untime operation
// Some of the might be interesting to get into the LPE library

struct is_data_variable
{
  aterm v;

  is_data_variable(data_variable v_)
    : v(aterm_appl(v_))
  {}
  
  bool operator()(aterm t) const
  {
    return v == t;
  }
};

///Returns whether data_variable v occurs in term d.
//TODO: See if I can make a template for the occurs_in function
bool occurs_in(data_expression d, data_variable v)
{
  return find_if(aterm_appl(d), is_data_variable(v)) != aterm();
}

bool occurs_in(action_list d, data_variable v)
{
  return find_if(aterm_appl(d), is_data_variable(v)) != aterm();
}

bool occurs_in(data_assignment_list d, data_variable v)
{
  return find_if(aterm_appl(d), is_data_variable(v)) != aterm();
}


///Apply a simple form of sum elimination in the case a summation
///variable does not occur within the summand at all
LPE_summand apply_no_occurrence_sumelm(const lpe::LPE_summand& summand)
{
  LPE_summand new_summand;
  // New summation variable list, all variables in this list occur in other terms in the summand.
  data_variable_list new_summation_variables = data_variable_list();

  for(data_variable_list::iterator i = summand.summation_variables().begin(); i != summand.summation_variables().end(); ++i)
  { 
    data_variable v = *i;
    
    //Check whether variable occurs in other terms of summand
    //If variable occurs leave the variable, so add variable to new list
    if (occurs_in(summand.condition(), v) || occurs_in(summand.actions(), v)
        || occurs_in(summand.time(), v) || occurs_in(summand.assignments(), v))
    {
      new_summation_variables = push_front(new_summation_variables, v);
    }
    //else remove the variable, i.e. do not add it to the new list (skip)
  }

  new_summation_variables = reverse(new_summation_variables);

  new_summand = LPE_summand(new_summation_variables,
                            summand.condition(),
                            summand.is_delta(),
                            summand.actions(),
                            summand.time(),
                            summand.assignments());

  return new_summand;
}

///Take a specification and apply sum elimination to its summands
lpe::specification no_occurrence_sumelm(const lpe::specification& specification)
{
  lpe::specification new_specification;
  lpe::LPE new_lpe;
//  data_variable_list new_summand_list = data_variable_list();
  summand_list new_summand_list = data_variable_list();

  lpe::LPE lpe = specification.lpe();

  // Traverse the summand list, and apply sum elimination to its summands,
  // whilst constructing a new summand list in the process.
  for(lpe::summand_list::iterator i = lpe.summands().begin(); i != lpe.summands().end(); ++i)
  {
    LPE_summand s = apply_no_occurrence_sumelm(*i);
    new_summand_list = push_front(new_summand_list, s);
  }

  new_summand_list = reverse(new_summand_list);

  new_lpe = LPE(lpe.free_variables(),
                lpe.process_parameters(),
                new_summand_list,
                lpe.actions());

  new_specification = lpe::specification(specification.sorts(),
                                         specification.constructors(),
                                         specification.mappings(),
                                         specification.equations(),
                                         specification.actions(),
                                         new_lpe,
                                         specification.initial_free_variables(),
                                         specification.initial_variables(),
                                         specification.initial_state());

  return new_specification;
}

///Apply sum elimination on the summand consisting of
///summation_variables, condition, actions, time and assignments.
///This checks the following:
///X(..) = sum d. d=e -> a(..) . X(..)
///and returns X(..) = e -> a(..) . X(..)
static void apply_sumelm(lpe::specification& specification)
{
  
}


///Returns an LPE specification in which the timed arguments have been rewritten
lpe::specification squadt_lpesumelm::sumelm(const lpe::specification& specification) 
{
  lpe::specification new_specification;
  new_specification = no_occurrence_sumelm(specification);  

  return new_specification;
}

int squadt_lpesumelm::do_sumelm()
{
  lpe::specification lpe_specification;

  if (lpe_specification.load(input_file)) {
    // Untime lpe_specification and save the output to a binary file
    if (!sumelm(lpe_specification).save(output_file, true)) 
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
    cerr << "Apply sum elimination on the LPE in INFILE and store the result to OUTFILE" << endl;
    cerr << endl;
    cerr << desc;

    exit (0);
  }
      
  if (vm.count("version")) {
    cerr << "lpesumelm " << VERSION << " (revision " << REVISION << ")" << endl;

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

  squadt_lpesumelm sl;
  if (sl.run(ac, av)) {
    return 0;
  }

  parse_command_line(ac,av);
  return sl.do_sumelm();
}
