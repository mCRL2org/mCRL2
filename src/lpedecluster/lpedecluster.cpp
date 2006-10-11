// ======================================================================
//
// Copyright (c) 2006 TU/e
//
// ----------------------------------------------------------------------
//
// file          : lpedecluster 
// date          : 10-10-2006
// version       : 0.1
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

//Lowlevel library for gsErrorMsg
#include <libprint_c.h>

//Aterms
#include <atermpp/aterm.h>
#include <atermpp/algorithm.h>

//LPE Framework
#include <lpe/function.h>
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

#define VERSION "0.1"

std::string input_file; ///< Name of the file to read input from
std::string output_file; ///< Name of the file to write output to (or stdout)

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
/*
struct is_sort
{
  aterm s;

  is_sort(lpe::sort s_)
    : s(aterm_appl(s_))
  {}

  bool operator()(aterm t) const
  {
    return s == t;
  }
};

///\ret true if there exists a data_variable of sort s in sl
bool occurs_in(data_variable_list sl, lpe::sort s)
{
  return find_if(aterm_list(sl), is_sort(s)) != aterm();
}
*/

//TODO: Make sure type() is renamed to sort()
///\ret a list of all data_variables of sort s in vl
lpe::data_variable_list get_occurrences(data_variable_list vl, lpe::sort s)
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
lpe::data_variable_list filter(data_variable_list vl, data_variable v)
{
  gsDebugMsg("filter:vl = %s, v = %s\n", vl.to_string().c_str(), v.to_string().c_str());
  data_variable_list result;
  for (data_variable_list::iterator i = vl.begin(); i != vl.end(); i++)
  {
    if (!(*i == v))
    {
      result = push_front(result, *i);
    }
  }
  gsDebugMsg("filter:result = %s\n", result.to_string().c_str());
  return result;
}

///\ret the domain of function f
lpe::sort result_sort(function f)
{
  return f.result_type();
}

///\ret the list of all functions f of sort s in fl
function_list get_constructors(function_list fl, lpe::sort s)
{
  function_list result;
  for(function_list::iterator i = fl.begin(); i != fl.end(); ++i)
  {
    if (result_sort(*i) == s)
    {
      result = push_front(result, *i);
    }
  }
  reverse(result);
  return result;
}

///\ret true if f has 1 or more arguments, false otherwise
bool has_arguments(function f, lpe::sort s)
{
  return !gsIsSortId(aterm_appl(f.argument(1)));
}

///\ret sort s is finite
bool is_finite(function_list fl, lpe::sort s)
{
  function_list cl = get_constructors(fl, s);
  bool result = true;

  //Make sure that false is returned when there are no constructor, needed for some internal formats
  if (cl.size() == 0)
  {
    result = false;
  }

  for (function_list::iterator i = cl.begin(); i != cl.end(); ++i)
  {
    result = result && !(has_arguments(*i, s));
  }

  return result;
}

///\ret sort s can be declustered
//Note: call is_finite, because in the future we might want to do some tricks on infinite sorts
bool is_declusterable(function_list fl, lpe::sort s)
{
  return is_finite(fl, s);
}

///\ret a list of declusterable sorts in sl
sort_list get_declusterable(function_list fl, sort_list sl)
{
  sort_list result;
  for(sort_list::iterator i = sl.begin(); i != sl.end(); ++i)
  {
    if (is_declusterable(fl, *i))
    {
      result = push_front(result, *i);
    }
  }
  reverse(result);
  return result;
}

////////////////////////////////////////////////////////////////
// Declustering
/////

//prototype
lpe::summand_list decluster_summand_list(const lpe::summand_list& sl, sort_list declusterable_sorts, function_list constructors);

///Decluster a summand
lpe::summand_list decluster_summand(const lpe::LPE_summand& summand, sort_list declusterable_sorts, function_list constructors)
{
  summand_list result;

  if (declusterable_sorts.empty())
  {
    result = push_front(result, summand); //Point of exit for recursion
  }

  else
  {
    //Take the first argument of sort_list, look if it occurs in summand.summation_variables,
    //if so, decluster to the first argument, create a new list, apply declustering on this list
    lpe::sort working_sort = declusterable_sorts.front(); //The sort we are declustering
    summand_list temp_result; //Temporary result, recursion is performed on this

    //A list of summation variables in summand.summation_variables which have sort working_sort
    data_variable_list occurrence_list = get_occurrences(summand.summation_variables(), working_sort);

    if (!occurrence_list.empty())
    {
      function_list constructor_list = get_constructors(constructors, working_sort); //Constructors for working_sort

      data_variable occurrence = occurrence_list.front(); //The occurrence we are declustering
      gsDebugMsg("declustering occurrence %s with relation to sort %s\n", occurrence.to_string().c_str(), working_sort.to_string().c_str());
      for (function_list::iterator i = constructor_list.begin(); i != constructor_list.end(); ++i)
      {
        //for every constructor of sort s in cl create a new summand
        //which is equal to the original summand, with the summation variable
        //removed, and the dummy of the summation variable instantiated
        //with the constructor
        data_assignment substitution = data_assignment(occurrence, data_expression(i->argument(0)));
        LPE_summand s = LPE_summand(filter(summand.summation_variables(), occurrence),
                                summand.condition().substitute(substitution),
                                summand.is_delta(),
                                substitute(summand.actions(), substitution),
                                summand.time().substitute(substitution),
                                substitute(summand.assignments(), substitution)
                               );
      
        temp_result = push_front(temp_result, s);
      }

      if (occurrence_list.size() == 1)
      {
        result = decluster_summand_list(temp_result, pop_front(declusterable_sorts), constructors);
      }
      else
      {
        //occurrence_list.size() > 1
        result = decluster_summand_list(temp_result, declusterable_sorts, constructors);
      }
    }

    else
    {
      //No summation variable of sort s occurs in the summand, no need to change
      //create a singleton summand_list with the unchanged summand, needed for recursion.
      temp_result = push_front(temp_result, summand);
      result = decluster_summand_list(temp_result, pop_front(declusterable_sorts), constructors);
    }
  }

  return result;
}

///Decluster a summand_list
lpe::summand_list decluster_summand_list(const lpe::summand_list& sl, sort_list declusterable_sorts, function_list constructors)
{
  summand_list result;
  for (summand_list::iterator i = sl.begin(); i != sl.end(); ++i)
  {
    result = result + decluster_summand(*i, declusterable_sorts, constructors);
  }
  return result;
}

///Decluster specification
lpe::specification decluster(const lpe::specification& specification)
{
  gsVerboseMsg("Declustering...\n");
  lpe::LPE lpe = specification.lpe();

  sort_list declusterable_sorts = get_declusterable(specification.data().constructors(), specification.data().sorts());

  gsVerboseMsg("Input: %d summands.\n", lpe.summands().size());
  lpe = set_summands(lpe, decluster_summand_list(lpe.summands(), declusterable_sorts, specification.data().constructors())); 
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

  input_file = (0 < vm.count("INFILE")) ? vm["INFILE"].as< string >() : "-";
  output_file = (0 < vm.count("OUTFILE")) ? vm["OUTFILE"].as< string >() : "-";
}

int main(int ac, char** av) {
  ATerm bot;
  ATinit(0,0,&bot);
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
