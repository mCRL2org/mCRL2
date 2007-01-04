// ======================================================================
//
// Copyright (c) 2006 TU/e
//
// ----------------------------------------------------------------------
//
// file          : lpedecluster 
// date          : 22-12-2006
// version       : 0.5
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
#include <utility/squadt_utility.h>
#endif

using namespace std;
using namespace atermpp;
using namespace lpe;

namespace po = boost::program_options;

#define VERSION "0.5"

///////////////////////////////////////////////////////////////////////////////
/// \brief structure that holds all options available for the tool.
///
typedef struct
{
  std::string input_file; ///< Name of the file to read input from
  std::string output_file; ///< Name of the file to write output to (or stdout)
  bool finite_only; ///< Only decluster finite sorts
  RewriteStrategy strategy; ///< Rewrite strategy to use, default inner
}tool_options;

#ifdef ENABLE_SQUADT_CONNECTIVITY
//Forward declaration because do_decluster() is called within squadt_interactor class
int do_decluster(const tool_options& options);

class squadt_interactor: public squadt_tool_interface
{
  private:
    enum input_files {
      lpd_file_for_input, ///< file containing an lpd that can be imported
      lpd_file_for_output ///< file used to write output to
    };

    enum further_options {
      option_finite_only,
      option_rewrite_strategy
    };

  private:
    boost::shared_ptr < sip::datatype::enumeration > rewrite_strategy_enumeration;
    
  public:
    
    /** \brief constructor */
    squadt_interactor();
    
    /** \brief configures tool capabilities */
    void set_capabilities(sip::tool::capabilities&) const;

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(sip::configuration&);

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(sip::configuration const&) const;

    /** \brief performs the task specified by a configuration */
    bool perform_task(sip::configuration&);
};

squadt_interactor::squadt_interactor() {
  rewrite_strategy_enumeration = sip::datatype::enumeration::create("inner");
  *rewrite_strategy_enumeration % "innerc" % "jitty" % "jittyc";
}

void squadt_interactor::set_capabilities(sip::tool::capabilities& capabilities) const
{
  // The tool has only one main input combination
  capabilities.add_input_combination(lpd_file_for_input, sip::mime_type("lpe"), sip::tool::category::transformation);
}

void squadt_interactor::user_interactive_configuration(sip::configuration& configuration)
{
  using namespace sip;
  using namespace sip::layout;
  using namespace sip::datatype;
  using namespace sip::layout::elements;

  layout::manager::aptr top = layout::vertical_box::create();
  layout::manager* current_box = new horizontal_box();

  squadt_utility::radio_button_helper < RewriteStrategy >
                                        strategy_selector(current_box, GS_REWR_INNER, "Inner");
  strategy_selector.associate(current_box, GS_REWR_INNERC, "Innerc");
  strategy_selector.associate(current_box, GS_REWR_JITTY,  "Jitty");
  strategy_selector.associate(current_box, GS_REWR_JITTYC, "Jittyc");
  
  top->add(new label("Rewrite strategy"));
  top->add(current_box);

  current_box = new horizontal_box();
  checkbox* finite_only = new checkbox("Only decluster variables of finite sorts");
  current_box->add(finite_only, layout::left);
  top->add(new label(" "));
  top->add(current_box);
  
  button* okay_button = new button("OK");
  top->add(new label(" "));
  top->add(okay_button, layout::right);

  send_display_layout(top);

  okay_button->await_change();
  
  configuration.add_output(lpd_file_for_output, sip::mime_type("lpe"), configuration.get_output_name(".lpe"));
  if (finite_only->get_status())
  {
    configuration.add_option(option_finite_only);
  }
  
  configuration.add_option(option_rewrite_strategy).append_argument(rewrite_strategy_enumeration, strategy_selector.get_selection());

}

bool squadt_interactor::check_configuration(sip::configuration const& configuration) const
{
  bool result = true;
  result |= configuration.object_exists(lpd_file_for_input);
  result |= configuration.object_exists(lpd_file_for_output);
  result |= configuration.object_exists(option_rewrite_strategy);

  return result;
}

bool squadt_interactor::perform_task(sip::configuration& configuration)
{
  using namespace sip;
  using namespace sip::layout;
  using namespace sip::datatype;
  using namespace sip::layout::elements;

  bool result = true;
  
  tool_options options;
  options.input_file = configuration.get_object(lpd_file_for_input)->get_location();
  options.output_file = configuration.get_object(lpd_file_for_output)->get_location();
  options.finite_only = configuration.object_exists(option_finite_only);
  options.strategy = static_cast < RewriteStrategy > (boost::any_cast < size_t > (configuration.get_option_value(option_rewrite_strategy)));

  layout::manager::aptr top(layout::vertical_box::create());
  
  top->add(new label("Declustering in progress"), layout::left);
  send_display_layout(top);

  //Perform declustering
  top = layout::vertical_box::create();
  int decluster_result = do_decluster(options);
  if (decluster_result == 0) {
    top->add(new label("Declustering succeeded"));
    result = true;
  }
  else
  {
    top->add(new label("Declustering failed"));
  }

  send_display_layout(top);

  return result;
}

#endif //ENABLE_SQUADT_CONNECTIVITY

/////////////////////////////////////////////////////////////////
// Helper functions
/////

///Used to assist in occurs_in function.
struct compare_data_variable
{
  aterm v;

  compare_data_variable(data_variable v_)
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
  return find_if(l, compare_data_variable(v)) != aterm();
}


///\ret a list of all data_variables of sort s in vl
lpe::data_variable_list get_occurrences(const data_variable_list& vl, const lpe::sort& s)
{
  data_variable_list result;
  for (data_variable_list::iterator i = vl.begin(); i != vl.end(); ++i)
  {
    if (i->sort() == s)
    {
      result = push_front(result, *i);
    }
  }
  result = reverse(result);
  return result;
}

///\ret the list of all data_variables in vl, which are unequal to v
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
    if (occurs_in(sl, i->sort()))
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
///\post the declustered version of summand has been appended to result
///\ret none
void decluster_summand(const lpe::specification& specification, const lpe::LPE_summand& summand, lpe::summand_list& result, EnumeratorStandard& enumerator, bool finite_only)
{
  int nr_summands = 0; // Counter for the nummer of new summands, used for verbose output

  gsVerboseMsg("initialization...");

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

  // List of variables with the declustered variables removed (can be done upfront, which is more efficient,
  // because we only need to calculate it once.
  data_variable_list new_vars = filter(summand.summation_variables(), variables);

  ATermList vars = ATermList(variables);

  ATerm expr = enumerator.getRewriter()->toRewriteFormat(aterm_appl(summand.condition()));

  // Solutions
  EnumeratorSolutions* sols = enumerator.findSolutions(vars, expr, false, NULL);

  gsVerboseMsg("processing...");
  // sol is a solution in internal rewriter format
  ATermList sol;
  while (sols->next(&sol))
  {
    data_assignment_list substitutions; 

    // Convenience cast, so that the iterator, and the modifications from the atermpp library can be used
    aterm_list solution = aterm_list(sol);

    // Translate internal rewriter solution to lpe data_assignment_list
    for (aterm_list::iterator i = solution.begin(); i != solution.end(); ++i)
    {
      // lefthandside of substitution
      data_variable var = data_variable(ATgetArgument(ATerm(*i), 0));

      // righthandside of substitution in internal rewriter format
      ATerm arg = ATgetArgument(ATerm(*i),1);

      // righthandside of substitution in lpe format
      data_expression res = data_expression(aterm_appl(enumerator.getRewriter()->fromRewriteFormat(arg)));

      // Substitution to be performed
      data_assignment substitution = data_assignment(var, res);
      substitutions = push_front(substitutions, substitution);
    }
    gsDebugMsg("substitutions: %s\n", substitutions.to_string().c_str());

    LPE_summand s = LPE_summand(new_vars,
                                summand.condition().substitute(assignment_list_substitution(substitutions)),
                                summand.is_delta(),
                                summand.actions().substitute(assignment_list_substitution(substitutions)),
                                summand.time().substitute(assignment_list_substitution(substitutions)),
                                summand.assignments().substitute(assignment_list_substitution(substitutions))
                                );
    
    result = push_front(result, s);
    ++nr_summands;
  }

  gsVerboseMsg("done...\n");
  gsVerboseMsg("Replaced with %d summands\n", nr_summands);
}

///Takes the summand list sl, declusters it,
///and returns the declustered summand list
lpe::summand_list decluster_summands(const lpe::specification& specification,
                                     const lpe::summand_list& sl,
                                     EnumeratorStandard& enumerator, 
                                     const tool_options& options)
{
  lpe::summand_list result;

  // decluster_summand(..) is called only in this function, therefore, it is safe to count the summands here for verbose output.
  lpe::summand_list summands = reverse(sl); // This is not absolutely necessary, but it helps in comparing input and output of the decluster algorithm (that is, the relative order is preserved (because decluster_summand plainly appends to result)
  int j = 1;
  for (summand_list::iterator i = summands.begin(); i != summands.end(); ++i, ++j)
  {
    gsVerboseMsg("Summand %d\n", j);
    lpe::LPE_summand s = *i;
    decluster_summand(specification, s, result, enumerator, options.finite_only);
  }

  return result;
}

///Takes the specification in specification, declusters it,
///and returns the declustered specification.
lpe::specification decluster(const lpe::specification& specification, const tool_options& options)
{
  gsVerboseMsg("Declustering...\n");
  gsDebugMsg("Using rewrite strategy %d\n", options.strategy);
  lpe::LPE lpe = specification.lpe();

  gsVerboseMsg("Input: %d summands.\n", lpe.summands().size());

  // Some use of internal format because we need it for the rewriter
  Rewriter* rewriter = createRewriter(specification.data(), options.strategy);
  EnumeratorStandard enumerator = EnumeratorStandard(specification, rewriter);

  lpe::summand_list sl = decluster_summands(specification, lpe.summands(), enumerator, options);
  lpe = set_summands(lpe, sl);

  gsVerboseMsg("Output: %d summands.\n", lpe.summands().size());

  return set_lpe(specification, lpe);
}

///Reads a specification from input_file, 
///applies declustering to it and writes the result to output_file.
int do_decluster(const tool_options& options)
{
  lpe::specification lpe_specification;
  
  if (lpe_specification.load(options.input_file)) {
    // decluster lpe_specification and save the output to a binary file
    if (!decluster(lpe_specification, options).save(options.output_file, true)) 
    {
      // An error occurred when saving
      gsErrorMsg("Could not save to '%s'\n", options.output_file.c_str());
      return (1);
    }
  }
  else {
    gsErrorMsg("lpedecluster: Unable to load LPE from `%s'\n", options.input_file.c_str());
    return (1);
  }

  return 0;
}

///Parses command line and sets settings from command line switches
void parse_command_line(int ac, char** av, tool_options& t_options) {
  po::options_description desc;
  std::string rewriter;

  desc.add_options()
      ("help,h",      "display this help")
      ("verbose,v",   "turn on the display of short intermediate messages")
      ("debug,d",     "turn on the display of detailed intermediate messages")
      ("finite,f",    "only decluster finite sorts")
      ("rewriter,R",   po::value<std::string>(&rewriter)->default_value("inner"), "use rewriter arg (default 'inner');"
                      "available rewriters are inner, jitty, innerc and jittyc")
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
  
  if (0 < vm.count("help")) {
    cerr << "Usage: "<< av[0] << " [OPTION]... [INFILE] [OUTFILE]" << endl;
    cerr << "Decluster the LPE in INFILE and store the result to OUTFILE" << endl;

    cerr << endl;
    cerr << desc;

    exit (0);
  }
      
  if (0 < vm.count("version")) {
    cerr << "lpedecluster " << VERSION << " (revision " << REVISION << ")" << endl;

    exit (0);
  }

  if (0 < vm.count("debug")) {
    gsSetDebugMsg();
  }

  if (0 < vm.count("verbose")) {
    gsSetVerboseMsg();
  }

  t_options.finite_only = (0 < vm.count("finite"));

  if (0 < vm.count("rewriter")) {
    cerr << "rewrite strategy: " << rewriter << endl;
    if      (rewriter == "inner")  { t_options.strategy = GS_REWR_INNER; }
    else if (rewriter == "innerc") { t_options.strategy = GS_REWR_INNERC; }
    else if (rewriter == "jitty")  { t_options.strategy = GS_REWR_JITTY; }
    else if (rewriter == "jittyc") { t_options.strategy = GS_REWR_JITTYC; }
    else { 
      cerr << rewriter << " is not a valid rewriter strategy" << endl;
      exit(EXIT_FAILURE);
    }
  }

  t_options.input_file = (0 < vm.count("INFILE")) ? vm["INFILE"].as< string >() : "-";
  t_options.output_file = (0 < vm.count("OUTFILE")) ? vm["OUTFILE"].as< string >() : "-";
}

int main(int ac, char** av) {
  ATerm bot;
  ATinit(ac, av, &bot);
  tool_options options;
  gsEnableConstructorFunctions();

#ifdef ENABLE_SQUADT_CONNECTIVITY
  squadt_interactor si;
  if (si.try_interaction(ac, av)) {
    return 0;
  }
#endif

  parse_command_line(ac,av, options);
  return do_decluster(options);
}
