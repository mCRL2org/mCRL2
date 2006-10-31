// ======================================================================
//
// Copyright (c) 2006 TU/e
//
// ----------------------------------------------------------------------
//
// file          : lpedecluster 
// date          : 24-10-2006
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

//Aterms
#include <atermpp/aterm.h>
#include <atermpp/algorithm.h>

//LPE Framework
#include <lpe/function.h>
#include <lpe/lpe.h>
#include <lpe/specification.h>
#include <lpe/sort_utility.h>

//Enumerator
#include <libnextstate.h>
#include <enum_standard.h>

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

std::string input_file; ///< Name of the file to read input from
std::string output_file; ///< Name of the file to write output to (or stdout)
bool finite_only = false; ///< Only decluster finite sorts

#ifdef ENABLE_SQUADT_CONNECTIVITY
//Forward declaration because do_decluster() is called within squadt_interactor class
int do_decluster(const std::string input_file_name, const std::string output_file_name);

class squadt_interactor: public squadt_tool_interface
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

void squadt_interactor::set_capabilities(sip::tool::capabilities& capabilities) const
{
  // The tool has only one main input combination
  capabilities.add_input_combination(lpd_file_for_input, "Transformation", "lpe");
}

void squadt_interactor::user_interactive_configuration(sip::configuration& configuration)
{
  configuration.add_output(lpd_file_for_output, "lpe", configuration.get_output_name(".lpe"));
}

bool squadt_interactor::check_configuration(sip::configuration const& configuration) const
{
// Check if everything present (see lpe2lts)
  return (configuration.object_exists(lpd_file_for_input) &&
          configuration.object_exists(lpd_file_for_output)
         );
}

bool squadt_interactor::perform_task(sip::configuration& configuration)
{
  std::string in_file, out_file;
  in_file = configuration.get_object(lpd_file_for_input)->get_location();
  out_file = configuration.get_object(lpd_file_for_output)->get_location();

  return (do_decluster(in_file, out_file)==0);
}

#endif //ENABLE_SQUADT_CONNECTIVITY

/////////////////////////////////////////////////////////////////
// Helper functions
/////

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

///\ret variable v occurs in l.
template <typename data_type>
bool occurs_in(data_type l, lpe::data_variable v)
{
  return find_if(aterm_list(l), is_data_variable(v)) != aterm();
}


//TODO: Make sure type() is renamed to sort()
///\ret a list of all data_variables of sort s in vl
lpe::data_variable_list get_occurrences(const data_variable_list& vl, const lpe::sort& s)
{
  data_variable_list result;
  for (data_variable_list::iterator i = vl.begin(); i != vl.end(); ++i)
  {
    if (i->type() == s)
    {
      result = push_front(result, *i);
    }
  }
  result = reverse(result);
  return result;
}

///\ret the list of all data_variables in vl, which are unequal to v
//TODO: Check if this could be done with find_all_if
lpe::data_variable_list filter(const data_variable_list& vl, const data_variable& v)
{
  gsDebugMsg("filter:vl = %s, v = %s\n", vl.to_string().c_str(), v.to_string().c_str());
  data_variable_list result;
  for (data_variable_list::iterator i = vl.begin(); i != vl.end(); ++i)
  {
    if (!(*i == v))
    {
      result = push_front(result, *i);
    }
  }
  gsDebugMsg("filter:result = %s\n", result.to_string().c_str());
  return result;
}

///\ret the list of all date_variables in vl, that are not in rl
lpe::data_variable_list filter(const data_variable_list& vl, const data_variable_list& rl)
{
  data_variable_list result;
  for (data_variable_list::iterator i = vl.begin(); i != vl.end(); ++i)
  {
    if (!occurs_in(rl, *i))
    {
      result = push_front(result, *i);
    }
  }

  return result;
}

///\pre fl is a list of constructors
///\ret a list of declusterable sorts in sl
sort_list get_finite_sorts(const function_list& fl, const sort_list& sl)
{
  sort_list result;
  for(sort_list::iterator i = sl.begin(); i != sl.end(); ++i)
  {
    if (is_finite(fl, *i))
    {
      result = push_front(result, *i);
    }
  }
  reverse(result);
  return result;
}

///\ret a list of all variables of a sort that occurs in sl
data_variable_list get_variables(const data_variable_list& vl, const sort_list& sl)
{
  data_variable_list result;
  for (data_variable_list::iterator i = vl.begin(); i != vl.end(); ++i)
  {
    if (occurs_in(sl, i->type())) // TODO have type renamed to sort()
    {
      result = push_front(result, *i);
    }
  }
  result = reverse(result);
  return result;
}


////////////////////////////////////////////////////////////////
// Declustering
/////

///\pre specification is the specification belonging to summand
///\ret the declustered summand list of summand
lpe::summand_list decluster_summand(const lpe::specification& specification, const lpe::LPE_summand& summand)
{
  lpe::summand_list result;

  // Some use of internal format because we need it for the rewriter
  Rewriter* rewriter = createRewriter(gsMakeDataEqnSpec(specification.data().equations()));
  EnumeratorStandard enumerator = EnumeratorStandard(specification, rewriter);
  
  data_variable_list variables; // The variables we need to consider in declustering
  if (finite_only)
  {
    // Only consider finite variables
    variables = get_variables(summand.summation_variables(), get_finite_sorts(specification.data().constructors(), specification.data().sorts()));
  }
  else
  {
    variables = summand.summation_variables();
  }

  ATermList vars = (ATermList(variables));
  ATerm expr = ATerm(aterm_appl(summand.condition()));
  
  // Solutions
  EnumeratorSolutions* sols = enumerator.findSolutions(vars, expr, false, NULL);

  // sol is a solution in internal rewriter format
  ATermList sol;
  while (sols->next(&sol))
  {
    data_assignment_list substitutions; 
    aterm_list solution = aterm_list(sol); //convenience cast to atermpp library
    gsDebugMsg("solution: %s\n", solution.to_string().c_str());

    // Translate internal rewriter solution to lpe data_assignment_list
    for (aterm_list::iterator i = solution.begin(); i != solution.end(); ++i)
    {
      // lefthandside of substitution
      data_variable var = data_variable(aterm(ATgetArgument(ATerm(aterm_list(sol).front()),0)));

      // righthandside of substitution in internal rewriter format
      aterm arg = aterm(ATgetArgument(ATerm(aterm_list(sol).front()),1));

      // righthandside of substitution in lpe format
      data_expression res = data_expression(aterm_appl(rewriter->fromRewriteFormat(arg)));

      // Substitution to be performed
      data_assignment substitution = data_assignment(var, res);
      substitutions = push_front(substitutions, substitution);
    }

    gsDebugMsg("substitutions: %s\n", substitutions.to_string().c_str());

    LPE_summand s = LPE_summand(filter(summand.summation_variables(), variables),
                                summand.condition().substitute(assignment_list_substitution(substitutions)),
                                summand.is_delta(),
                                substitute(summand.actions(), assignment_list_substitution(substitutions)),
                                summand.time().substitute(assignment_list_substitution(substitutions)),
                                substitute(summand.assignments(), assignment_list_substitution(substitutions))
                                );

    result = push_front(result, s);
  }

  result = reverse(result);

  gsDebugMsg("orig summand: %s\n", summand.to_string().c_str());
  gsDebugMsg("result: %s\n", result.to_string().c_str());

  return result;
}

///Takes the summand list sl, declusters it,
///and returns the declustered summand list
lpe::summand_list decluster_summands(const lpe::specification& specification, const lpe::summand_list& sl)
{
  lpe::summand_list result;

  for (summand_list::iterator i = sl.begin(); i != sl.end(); ++i)
  {
    lpe::LPE_summand s = *i;
    result = result + decluster_summand(specification, s);
  }

  return result;
}

///Takes the specification in specification, declusters it,
///and returns the declustered specification.
lpe::specification decluster(const lpe::specification& specification)
{
  gsVerboseMsg("Declustering...\n");
  lpe::LPE lpe = specification.lpe();

  gsVerboseMsg("Input: %d summands.\n", lpe.summands().size());

  lpe::summand_list sl = decluster_summands(specification, lpe.summands());
  lpe = set_summands(lpe, sl);

  gsVerboseMsg("Output: %d summands.\n", lpe.summands().size());

  return set_lpe(specification, lpe);
}

///Reads a specification from input_file, 
///applies declustering to it and writes the result to output_file.
int do_decluster(const std::string input_file_name, const std::string output_file_name)
{
  lpe::specification lpe_specification;
  if (lpe_specification.load(input_file_name)) {
    // decluster lpe_specification and save the output to a binary file
    if (!decluster(lpe_specification).save(output_file_name, true)) 
    {
      // An error occurred when saving
      gsErrorMsg("Could not save to '%s'\n", output_file_name.c_str());
      return (1);
    }
  }
  else {
    gsErrorMsg("lpedecluster: Unable to load LPE from `%s'\n", input_file_name.c_str());
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
      ("debug,d",     "turn on the display of detailed intermediate messages")
      ("finite,f",    "only decluster finite sorts")
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
    cerr << "Decluster the LPE in INFILE and store the result to OUTFILE" << endl;

    cerr << endl;
    cerr << desc;

    exit (0);
  }
      
  if (vm.count("version")) {
    cerr << "lpedecluster " << VERSION << " (revision " << REVISION << ")" << endl;

    exit (0);
  }

  if (vm.count("debug")) {
    gsSetDebugMsg();
  }

  if (vm.count("verbose")) {
    gsSetVerboseMsg();
  }

  if (vm.count("finite")) {
    finite_only = true;
  }

  input_file = (0 < vm.count("INFILE")) ? vm["INFILE"].as< string >() : "-";
  output_file = (0 < vm.count("OUTFILE")) ? vm["OUTFILE"].as< string >() : "-";
}

int main(int ac, char** av) {
  ATerm bot;
  ATinit(ac, av, &bot);
  gsEnableConstructorFunctions();

#ifdef ENABLE_SQUADT_CONNECTIVITY
  squadt_interactor si;
  if (si.try_interaction(ac, av)) {
    return 0;
  }
#endif

  parse_command_line(ac,av);
  return do_decluster(input_file, output_file);
}
