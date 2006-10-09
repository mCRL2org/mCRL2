// ======================================================================
//
// Copyright (c) 2006 TU/e
//
// ----------------------------------------------------------------------
//
// file          : lpesumelm 
// date          : 03-10-2006
// version       : 0.23
//
// author(s)     : Jeroen Keiren <j.j.a.keiren@student.tue.nl>
//
// ======================================================================

#include "mcrl2_revision.h"

//C++
#include <exception>
#include <cstdio>

//Boost
#include <boost/program_options.hpp>
#include <boost/filesystem/convenience.hpp>

//Lowlevel library for gsErrorMsg
#include <libprint_c.h>

//Aterms
#include <atermpp/aterm.h>
#include <atermpp/algorithm.h>

//LPE Framework
#include <lpe/lpe.h>
#include <lpe/specification.h>

//Squadt connectivity
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <sip/tool.h>
#include <squadt_utility.h>
#endif

using namespace std;
using namespace atermpp;
using namespace lpe;

namespace po = boost::program_options;

//TODO: Add configuration parameter for substitution sumelimination (enable/disable)
//TODO: Get rid of gsMakeDataExpr...() functions

#define VERSION "0.23"

std::string input_file; ///< Name of the file to read input from
std::string output_file; ///< Name of the file to write output to (or stdout)

#ifdef ENABLE_SQUADT_CONNECTIVITY
//Forward declaration because do_sumelm() is called within squadt_lpesumelm class
int do_sumelm(const std::string input_file_name, const std::string output_file_name);

class squadt_lpesumelm: public squadt_tool_interface
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

void squadt_lpesumelm::set_capabilities(sip::tool::capabilities& capabilities) const
{
  // The tool has only one main input combination
  capabilities.add_input_combination(lpd_file_for_input, "Transformation", "lpe");
}

void squadt_lpesumelm::user_interactive_configuration(sip::configuration& configuration)
{
  configuration.add_output(lpd_file_for_output, "lpe", configuration.get_output_name(".lpe"));
}

bool squadt_lpesumelm::check_configuration(sip::configuration const& configuration) const
{
// Check if everything present (see lpe2lts)
  return (configuration.object_exists(lpd_file_for_input) &&
          configuration.object_exists(lpd_file_for_output)
         );
}

bool squadt_lpesumelm::perform_task(sip::configuration& configuration)
{
  std::string in_file, out_file;
  in_file = configuration.get_object(lpd_file_for_input)->get_location();
  out_file = configuration.get_object(lpd_file_for_output)->get_location();

  return (do_sumelm(in_file, out_file)==0);
}

#endif //ENABLE_SQUADT_CONNECTIVITY

////////////////////////////////////////////////////////////////
// Auxiliary functions used to support untime operation
// Some of these might be interesting to get into the LPE library

///Used to assist in occurs_in function.
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

///pre: true
///ret: data_variable v occurs in d.
template <typename data_type>
bool occurs_in(data_type d, data_variable v)
{
  return find_if(aterm_appl(d), is_data_variable(v)) != aterm();
}

///pre: true
///ret: 1 if t is a DataExprEquality, 0 otherwise
static int gsIsDataExprEquality(ATermAppl t)
{
  if (gsIsDataAppl(t))
  { ATermAppl t1=ATAgetArgument(t,0);
    if (gsIsDataAppl(t1))
    { ATermAppl f=ATAgetArgument(t1,0);
      if (!gsIsOpId(f))
      { // This is not a functionsymbol
        return 0;
      }
      ATermAppl functionsort=ATAgetArgument(f,1);
      if (!gsIsSortArrow(functionsort))
      { return 0;
      }
      ATermAppl sort=ATAgetArgument(functionsort,0);
      if (ATAgetArgument(t1,0)==gsMakeOpIdEq(sort))
      {
        return 1;
      };
    }
  }
  return 0;
}

///pre: true
///ret: 1 if t is a DataExprAnd, 0 otherwise
static int gsIsDataExprAnd(ATermAppl t)
{
  if (gsIsDataAppl(t))
  { ATermAppl t1=ATAgetArgument(t,0);
    if (gsIsDataAppl(t1))
    { if (ATAgetArgument(t1,0)==gsMakeOpIdAnd())
      {
       return 1;
      };
    }
  }
  return 0;
}

///pre: true
///ret: 1 if t is an equality, 0 otherwise
inline
int is_equality(data_expression t)
{
  return gsIsDataExprEquality(ATermAppl(t));
}

///pre: true
///ret: 1 if t is a conjunction, 0 otherwise
inline
int is_and(data_expression t)
{
  return gsIsDataExprAnd(ATermAppl(t));
}

///pre: true
///ret: 1 if t as a data_variable, 0 otherwise
inline
int is_var(data_expression t)
{
  return gsIsDataVarId(ATermAppl(t));
}

///pre: is_var(t)
///ret: The data_variable embedded in t
inline
data_variable get_var(data_expression t)
{
  assert(is_var(t));
  return data_variable(ATgetArgument(ATermAppl(t),0));
}

///pre: is_and(t) || is_equality(t)
///ret: lefthandside of t
inline
data_expression lhs(data_expression t)
{
  assert(is_and(t) || is_equality(t));
  return data_expression(ATAgetArgument(ATAgetArgument(t, 0), 1));  
}

///pre: is_and(t) || is_equality(t)
///ret: righthandside of t
inline
data_expression rhs(data_expression t)
{
  assert(is_and(t) || is_equality(t));
  return data_expression(ATAgetArgument(t, 1));
}

///pre: is_equality(t); t is of form a == b
///ret: b == a
inline
data_expression swap_equality(data_expression t)
{
  assert(is_equality(t));
  return data_expression(gsMakeDataExprEq(ATermAppl(rhs(t)), ATermAppl(lhs(t))));
//  return equal_to(rhs(t), lhs(t));
}

///pre: is_and(t)
///ret: if t is of the form b && true or of the form true && b return b,
///     if t is of the form a && b return t,
data_expression eliminate_unit_and(data_expression t)
{
  assert(is_and(t));

  data_expression result;

  if (lhs(t).is_true())
  {
    result = rhs(t);
  }
  else if (rhs(t).is_true())
  {
    result = lhs(t);
  }
  else
  {
    result = t;
  }

  return result;
}

////////////////////////////////////////////////////////////
// Functions for sumelm operations
//

///Apply a simple form of sum elimination in the case a summation
///variable does not occur within the summand at all
LPE_summand apply_no_occurrence_sumelm(const lpe::LPE_summand& summand)
{
  LPE_summand new_summand;
  // New summation variable list, all variables in this list occur in other terms in the summand.
  data_variable_list new_summation_variables;

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

  new_summand = set_summation_variables(summand, new_summation_variables);

  return new_summand;
}

///Take a specification and apply sum elimination to its summands
lpe::specification no_occurrence_sumelm(const lpe::specification& specification)
{
  lpe::LPE lpe = specification.lpe();
  lpe::specification new_specification;
  summand_list new_summand_list = lpe.summands();

  // Traverse the summand list, and apply sum elimination to its summands,
  // whilst constructing a new summand list in the process.
  new_summand_list = apply(new_summand_list, apply_no_occurrence_sumelm);

  new_specification = set_lpe(specification, set_summands(lpe, new_summand_list));

  return new_specification;
}

//Recursively apply sum elimination on a summand. Note that summand is
//passed as a pointer argument because we apply substitution to the
//parameters of the substitution at the deepest level of recursion.
//working_condition is a parameter that we use to split up the problem,
//at the first call of this function working_condition == summand->condition()
//should hold
//
//Note that filtering the summation variables should (for now) be done in the calling
//function, by applying no_occurrence_sumelm on the result, because that is a little
//more efficient.
//The new condition is built up on the return path of the recursion, so
//the last exit of the recursion is the new condition of the summand.
//
//!!!INTERNAL USE ONLY!!!
data_expression recursive_apply_eq_sumelm(LPE_summand* summand,
                                          data_expression working_condition)
{
  // In all cases not explicitly handled we return the original working_condition
  data_expression result = working_condition;

  if (is_and(working_condition))
  {
    //Recursively apply sum elimination on lhs and rhs
    //Note that recursive application provides for progress because lhs and rhs split the working condition.
    data_expression a,b;
    a = recursive_apply_eq_sumelm(summand, lhs(working_condition));
    b = recursive_apply_eq_sumelm(summand, rhs(working_condition));
    result = eliminate_unit_and(gsMakeDataExprAnd(a,b));
  }
  
  else if (is_equality(working_condition))
  {
    //Check if rhs is a variable, if so, swap lhs and rhs, so that the following code
    //is always the same.
    if (is_var(rhs(working_condition)))
    {
      working_condition = swap_equality(working_condition);
    }

    //If lhs is a variable, check if it occurs in the summation variables, if so
    //apply substitution lhs := rhs in actions, time and assignments.
    //substitution in condition is accounted for on returnpath of recursion,
    //substitution in summation_variables is done in calling function.
    if (is_var(lhs(working_condition)))
    {
      if (occurs_in(summand->summation_variables(), get_var(lhs(working_condition))))
      {
        data_assignment substitution = data_assignment(get_var(lhs(working_condition)), rhs(working_condition));

        *summand = LPE_summand(summand->summation_variables(),
                               summand->condition(),
                               summand->is_delta(),
                               substitute(summand->actions(), substitution),
                               summand->time().substitute(substitution),
                               substitute(summand->assignments(), substitution)
		      );

        result = gsMakeDataExprTrue();
      }
    }

  }
  
  return result;
}

///Apply sum elimination on the summand consisting of
///summation_variables, condition, actions, time and assignments.
///This checks the following:
///X(..) = sum d. d=e -> a(..) . X(..)
///and returns X(..) = e -> a(..) . X(..)
lpe::LPE_summand apply_eq_sumelm(const lpe::LPE_summand& summand)
{
  LPE_summand new_summand = summand;

  //Apply elimination and store result
  data_expression new_condition = recursive_apply_eq_sumelm(&new_summand, new_summand.condition());

  //Incorporate the new condition in the summand
  new_summand = set_condition(new_summand, new_condition);
 
  //Take the summand with substitution, and remove the summation variables that are now not needed
  //TODO: move the no_occurrence_sumelm into the recursive function above, that would make the separation more perfect.
  return apply_no_occurrence_sumelm(new_summand);
}


///Take an lpe specification, apply equality sum elimination to it,
///and return an lpe specification
lpe::specification eq_sumelm(const lpe::specification& specification)
{
  lpe::LPE lpe = specification.lpe();
  lpe::specification new_specification;
  summand_list new_summand_list = lpe.summands();

  // Apply sum elimination on each of the summands in the summand list.
  new_summand_list = apply(new_summand_list, apply_eq_sumelm);

  new_specification = set_lpe(specification, set_summands(lpe, new_summand_list));

  return new_specification;

}

///Returns an LPE specification in which the timed arguments have been rewritten
lpe::specification sumelm(const lpe::specification& specification) 
{
  lpe::specification new_specification = specification;
  new_specification = eq_sumelm(new_specification); // new_specification used for future concerns, possibly disabling eq_sumelm
  new_specification = no_occurrence_sumelm(new_specification);

  return new_specification;
}

///Reads a specification from input_file, 
///applies sum elimination to it and writes the result to output_file.
int do_sumelm(const std::string input_file_name, const std::string output_file_name)
{
  lpe::specification lpe_specification;
  if (lpe_specification.load(input_file_name)) {
    // Untime lpe_specification and save the output to a binary file
    if (!sumelm(lpe_specification).save(output_file_name, true)) 
    {
      // An error occurred when saving
      gsErrorMsg("Could not save to '%s'\n", output_file_name.c_str());
      return (1);
    }
  }
  else {
    gsErrorMsg("lpesumelm: Unable to load LPE from `%s'\n", input_file_name.c_str());
    return (1);
  }

  return 0;
}

///Parses command line and sets settings from command line switches
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

#ifdef ENABLE_SQUADT_CONNECTIVITY
  squadt_lpesumelm sl;
  if (sl.try_interaction(ac, av)) {
    return 0;
  }
#endif

  parse_command_line(ac,av);
  return do_sumelm(input_file, output_file);
}
