// ======================================================================
//
// Copyright (c) 2006 TU/e
//
// ----------------------------------------------------------------------
//
// file          : lpesumelm 
// date          : 03-10-2006
// version       : 0.21
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

#define VERSION "0.21"

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
/*
      if (!existsort(sort))
      { return 0;
      }
*/
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
int is_equality(data_expression t)
{
  return gsIsDataExprEquality(ATermAppl(t));
}

///pre: true
///ret: 1 if t is a conjunction, 0 otherwise
int is_and(data_expression t)
{
  return gsIsDataExprAnd(ATermAppl(t));
}

///pre: true
///ret: 1 if t as a data_variable, 0 otherwise
int is_var(data_expression t)
{
  return gsIsDataVarId(ATermAppl(t));
}

///pre: is_var(t)
///ret: The data_variable embedded in t
data_variable get_var(data_expression t)
{
  assert(is_var(t));
  return data_variable(ATgetArgument(ATermAppl(t),0));
}

///pre: is_and(t) || is_equality(t)
///ret: lefthandside of t
data_expression lhs(data_expression t)
{
  assert(is_and(t) || is_equality(t));
  return data_expression(ATAgetArgument(ATAgetArgument(t, 0), 1));  
}

///pre: is_and(t) || is_equality(t)
///ret: righthandside of t
data_expression rhs(data_expression t)
{
  assert(is_and(t) || is_equality(t));
  return data_expression(ATAgetArgument(t, 1));
}

///pre: is_equality(t); t is of form a == b
///ret: b == a
data_expression swap(data_expression t)
{
  assert(is_equality(t));
  return data_expression(gsMakeDataExprEq(ATermAppl(rhs(t)), ATermAppl(lhs(t))));
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

//Recursive function for sum elimination on a summand, uses dirty tricks
//that we want to shield from the caller of the sumelm functions
//it is therefor called by the apply_eq_sumelm function,
//and should only be called by that one.
//Initially (that is, on first call): *condition==working_condition
//
// working_condition is a term that is at first call equal to *condition
// in the process, each time working_condition is a conjunct it is split up
// in a lefthandside (lhs) and a righthandside (rhs). lhs and rhs are then
// used as a parameter in a recursive call of this function.
// once the working_condition is small enough, and it is an equality we check if
// one of the sides is a variable that occurs in the summation_variables, and substitute
// that with the other side of the equality in the summand, and remove it from the
// summation variables.
///!!!INTERNAL USE ONLY!!!
///ret: working condition of the respective subset of the LPE, with transformations
///     necesary because of substitution applied to it.
///     return value of the first call of this function resembles the new condition.
data_expression recursive_apply_eq_sumelm(data_variable_list* summation_variables,
                               data_expression* condition,
                               action_list* actions,
                               data_expression* time,
                               data_assignment_list* assignments,
                               data_expression working_condition)
{
  data_expression result; // Return value, consisting of working_condition with transformation needed because of elimination
  if (is_and(ATermAppl(working_condition)))
  {
    //Recursively apply sum elimination on lhs and rhs
    //Note that recursive application provides for progress because lhs and rhs split the working condition.
    data_expression a,b; // Temporarily store results from recursive calls to make result
    a = recursive_apply_eq_sumelm(summation_variables,
                              condition,
                              actions,
                              time,
                              assignments,
                              lhs(working_condition));
    b = recursive_apply_eq_sumelm(summation_variables,
                              condition,
                              actions,
                              time,
                              assignments,
                              rhs(working_condition));

    result = eliminate_unit_and(gsMakeDataExprAnd(a, b));
  }

  else if (is_equality(ATermAppl(working_condition)))
  { 
    //See if lhs or rhs occurs in summation_variables, and if so,
    //remove the variable and apply substitution

    //Preserve the original working condition because we might need that later.
    data_expression equality = working_condition;

    //If rhs is a variable then swap, so that the rest of the code can be used
    if (is_var(rhs(equality)))
    {
      equality = swap(equality);
    }
    
    if (is_var(lhs(equality)))
    {
      if (occurs_in(*summation_variables, get_var(lhs(equality))))
      {
        //Substitute all occurrences of lhs for rhs in the lpe
        
        data_assignment a = data_assignment(get_var(lhs(equality)), rhs(equality));
        data_assignment_list al = data_assignment_list();
        al = push_front(al, a);

        //TODO: Try to update summation variables here as well;
        //condition is updated through the return value of this function!
        *actions = substitute(*actions, assignment_list_substitution(al));
        *time = time->substitute(a);
        *assignments = substitute(*assignments, assignment_list_substitution(al));
        
        result = gsMakeDataExprTrue(); // Substitution applied, equality condition can be removed
      }
      else //!occurs_in(..)
      {
        result = working_condition; // No substitution, keep old condition
      }
    }
    else //!is_var(lhs(equality))
    {
      result = working_condition; // No substitution, keep old condition
    }
  }

  else
  {
    gsDebugMsg("A non-handled condition type occurred!\n");
    // The condition is not a conjunct or an equality, we can't easily eliminate.
    result = working_condition; // No substitution, keep old condition
  }

  return result;
}

///Apply sum elimination on the summand consisting of
///summation_variables, condition, actions, time and assignments.
///This checks the following:
///X(..) = sum d. d=e -> a(..) . X(..)
///and returns X(..) = e -> a(..) . X(..)
lpe::LPE_summand apply_eq_sumelm(lpe::LPE_summand& summand)
{
  LPE_summand new_summand;
  data_variable_list new_summation_variables = summand.summation_variables();
  data_expression new_condition = summand.condition();
  action_list new_actions = summand.actions();
  data_expression new_time = summand.time();
  data_assignment_list new_assignments = summand.assignments();

  // Apply elimination
  new_condition = recursive_apply_eq_sumelm(&new_summation_variables,
                                            &new_condition,
                                            &new_actions,
                                            &new_time,
                                            &new_assignments,
                                            new_condition);
 
  new_summand = LPE_summand(new_summation_variables,
                            new_condition,
                            summand.is_delta(),
                            new_actions,
                            new_time,
                            new_assignments);
 
  //Take the summand with substitution, and remove the summation variables that are now not needed
  //TODO: move the no_occurrence_sumelm into the recursive function above, that would make the separation more perfect.
  return apply_no_occurrence_sumelm(new_summand);
  //return new_summand;
}


///Take an lpe specification, apply equality sum elimination to it,
///and return an lpe specification
lpe::specification eq_sumelm(const lpe::specification& specification)
{
  lpe::specification new_specification;
  lpe::LPE new_lpe;
  summand_list new_summand_list = data_variable_list();

  lpe::LPE lpe = specification.lpe();

  // Traverse the summand list, and apply sum elimination to its summands,
  // whilst constructing a new summand list in the process.
  for(lpe::summand_list::iterator i = lpe.summands().begin(); i != lpe.summands().end(); ++i)
  {
    LPE_summand s = *i;
    new_summand_list = push_front(new_summand_list, apply_eq_sumelm(s));
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
    gsDebugMsg("loaded spec\n");
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
