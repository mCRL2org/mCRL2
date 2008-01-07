// Author(s): Tom Haenen
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsactionrename.cpp
/// \brief Add your file description here.

#define NAME "lpsactionrename"

#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <climits>
#include <iostream>
#include <fstream>
#include <getopt.h>
#include <aterm2.h>
#include "mcrl2/core/struct.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/detail/parse.h"
#include "mcrl2/core/detail/typecheck.h"
#include "mcrl2/core/detail/data_implementation.h"
#include "mcrl2/core/detail/regfrmtrans.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/core_init.h"
#include "mcrl2/utilities/version_info.h"
#include "mcrl2/lps/rename.h"
#include "mcrl2/lps/sumelm.h"
#include "mcrl2/data/rewrite.h"
#include "mcrl2/data/sort_identifier.h"

using namespace mcrl2::utilities;
using namespace std;
using namespace lps;
using namespace data_expr;

//Type definitions
//----------------

//t_phase represents the phases at which the program should be able to stop
typedef enum { PH_NONE, PH_PARSE, PH_TYPE_CHECK, PH_DATA_IMPL, PH_MERGE} t_phase;

//t_tool_options represents the options of the tool 
typedef struct t_tool_options {
  bool pretty;
  bool norewrite;
  bool nosumelm;
  t_phase end_phase;
  string action_rename_filename;
  string infilename;
  string outfilename;
};

//Functions used by the main program
//----------------------------------

static t_tool_options parse_command_line(int argc, char **argv);
//Post: The command line options are parsed.
//      The program has aborted with a suitable error code, if:
//      - errors were encountered
//      - non-standard behaviour was requested (help or version)
//Ret:  the parsed command line options

static ATermAppl rename_actions(t_tool_options tool_options);
//Pre:  tool_options.action_rename_filename contains a action rename
//      specification
//      tool_options.infilename contains an LPS ("" indicates stdin)
//      tool_options.end_phase indicates at which phase conversion stops
//Ret:  if end_phase == PH_NONE, the new LPS generated from the action rename 
//      file and the old LPS
//      if end_phase != PH_NONE, the state formula after phase end_phase
//      NULL, if something went wrong

static void print_help(char *name);
static void print_more_info(char *name);

bool process(t_tool_options const& tool_options) {
  //process action rename specfication
  ATermAppl result = rename_actions(tool_options);
  if (result == NULL) {
    return false;
  }

  //store the result
  string outfilename = tool_options.outfilename;
  bool opt_pretty = tool_options.pretty;
  if (outfilename == "") {
    gsVerboseMsg("saving result to stdout...\n");
    PrintPart_CXX(cout, (ATerm) result, opt_pretty?ppDefault:ppInternal);
    cout << endl;
  } else {
    gsVerboseMsg("saving result to '%s'...\n", outfilename.c_str());
    ofstream outstream(outfilename.c_str(), ofstream::out|ofstream::binary);
    if (!outstream.is_open()) {
      gsErrorMsg("cannot open output file '%s'\n", outfilename.c_str());
      return false;
    }
    PrintPart_CXX(outstream, (ATerm) result, opt_pretty?ppDefault:ppInternal);
    outstream.close();
  }

  return true;
}

// SQuADT protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include "mcrl2/utilities/squadt_interface.h"

class squadt_interactor : public mcrl2::utilities::squadt::mcrl2_tool_interface {

  private:

    static const char*  lps_file_for_input;     ///< file containing an LPS
    static const char*  rename_file_for_input;  ///< file containing a rename specification
    static const char*  lps_file_for_output;    ///< file used to write the output to
    static const char*  option_no_rewrite;
    static const char*  option_no_sumelm;

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

const char* squadt_interactor::lps_file_for_input    = "lps_in";
const char* squadt_interactor::rename_file_for_input = "renamefile_in";
const char* squadt_interactor::lps_file_for_output   = "lps_out";
const char* squadt_interactor::option_no_rewrite     = "no_rewrites";
const char* squadt_interactor::option_no_sumelm      = "no_sumelm";

void squadt_interactor::set_capabilities(tipi::tool::capabilities& c) const {
  c.add_input_configuration(lps_file_for_input, tipi::mime_type("lps", tipi::mime_type::application), tipi::tool::category::transformation);
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& c) {
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::layout::elements;//

  if (!c.option_exists(option_no_rewrite)) {
    c.add_option(option_no_rewrite).set_argument_value< 0, tipi::datatype::boolean >(false);
  }
  if (!c.option_exists(option_no_sumelm)) {
    c.add_option(option_no_sumelm).set_argument_value< 0, tipi::datatype::boolean >(false);
  }

  /* Create display */
  tipi::layout::tool_display d;

  layout::vertical_box& m = d.create< vertical_box >().set_default_margins(margins(0,5,0,5));

  /* Create and add the top layout manager */
  text_field& rename_file_field = d.create< text_field >();
  checkbox&   rewrite            = d.create< checkbox >().set_status(!c.get_option_argument< bool >(option_no_rewrite));
  checkbox&   sumelm            = d.create< checkbox >().set_status(!c.get_option_argument< bool >(option_no_sumelm));
  button&     okay_button       = d.create< button >().set_label("OK");


  m.append(d.create< horizontal_box >().
        append(d.create< label >().set_text("Rename file name : ")).
        append(rename_file_field)).
        append(d.create< vertical_box >().set_default_alignment(layout::left).
            append(rewrite.set_label("Use rewriting")).
            append(sumelm.set_label("Apply sum elimination"))).
     append(d.create< label >().set_text(" ")).
     append(okay_button, layout::right);

  // Set default values if the configuration specifies them
  if (c.input_exists(rename_file_for_input)) {
    rename_file_field.set_text(c.get_input(rename_file_for_input).get_location());
  }

  send_display_layout(d.set_manager(m));

  /* Wait until the ok button was pressed */
  okay_button.await_change();
  
  c.add_input(rename_file_for_input, tipi::mime_type("mf", tipi::mime_type::text), rename_file_field.get_text());

  /* Add output file to the configuration */
  if (c.output_exists(lps_file_for_output)) {
    tipi::object& output_file = c.get_output(lps_file_for_output);

    output_file.set_location(c.get_output_name(".lps"));
  }
  else {
    c.add_output(lps_file_for_output, tipi::mime_type("lps", tipi::mime_type::application), c.get_output_name(".lps"));
  }

  c.get_option(option_no_rewrite).set_argument_value< 0, boolean >(!rewrite.get_status());
  c.get_option(option_no_sumelm).set_argument_value< 0, boolean >(!sumelm.get_status());

  send_clear_display();
}

bool squadt_interactor::check_configuration(tipi::configuration const& c) const {
  bool result = true;

  result &= c.input_exists(lps_file_for_input);
  result &= c.input_exists(rename_file_for_input);
  result &= c.output_exists(lps_file_for_output);

  return (result);
}

bool squadt_interactor::perform_task(tipi::configuration& c) {
  t_tool_options tool_options;

  tool_options.pretty           = false;
  tool_options.norewrite	= false;
  tool_options.nosumelm		= false;
  tool_options.end_phase        = PH_NONE;
  tool_options.action_rename_filename = c.get_input(rename_file_for_input).get_location();
  tool_options.infilename       = c.get_input(lps_file_for_input).get_location();
  tool_options.outfilename      = c.get_output(lps_file_for_output).get_location();

  bool result = process(tool_options);

  if (result) {
    send_clear_display();
  }

  return (result);
}
#endif //ENABLE_SQUADT_CONNECTIVITY

//Main program
//------------

int main(int argc, char **argv)
{
  MCRL2_CORE_LIBRARY_INIT(argv)


#ifdef ENABLE_SQUADT_CONNECTIVITY
  if (!mcrl2::utilities::squadt::interactor< squadt_interactor    >::free_activation(argc, argv)) {
#endif

  //parse command line
  t_tool_options tool_options = parse_command_line(argc, argv);

  //process action rename specfication
  if (!process(tool_options)) {
    return 1;
  }
#ifdef ENABLE_SQUADT_CONNECTIVITY
  }
#endif

  return 0;
}

static t_tool_options parse_command_line(int argc, char **argv)
{
  t_tool_options tool_options;
  //declarations for getopt
  t_phase opt_end_phase = PH_NONE;
  bool opt_pretty = false;
  bool opt_norewrite = false;
  bool opt_nosumelm = false;
  string action_rename_filename = "";
  #define SHORT_OPTIONS "f:p:ehqvdom"
  #define VERSION_OPTION CHAR_MAX + 1
  struct option long_options[] = {
    { "file",      required_argument,  NULL,  'f' },
    { "no-rewrite",no_argument,	       NULL,  'o' },
    { "no-sumelm", no_argument,        NULL,  'm' },
    { "end-phase", required_argument,  NULL,  'p' },
    { "external",  no_argument,        NULL,  'e' },
    { "help",      no_argument,        NULL,  'h' },
    { "version",   no_argument,        NULL,  VERSION_OPTION },
    { "quiet",     no_argument,        NULL,  'q' },
    { "verbose",   no_argument,        NULL,  'v' },
    { "debug",     no_argument,        NULL,  'd' },
    { 0, 0, 0, 0 }
  };
  int option;
  //parse options
  while ((option = getopt_long(argc, argv, SHORT_OPTIONS, long_options, NULL)) != -1) {
    switch (option) {
      case 'f': /* rename file */
        action_rename_filename = optarg;
        break;
      case 'o': /* no rewrites */
        opt_norewrite = true;
        break;
      case 'm': /* no sumelm */
        opt_nosumelm = true;
        break;
      case 'p': /* end-phase */
        if (strcmp(optarg, "pa") == 0) {
          opt_end_phase = PH_PARSE;
        } else if (strcmp(optarg, "tc") == 0) {
          opt_end_phase = PH_TYPE_CHECK;
        } else if (strcmp(optarg, "di") == 0) {
          opt_end_phase = PH_DATA_IMPL;
        } else if (strcmp(optarg, "me") == 0) {
          opt_end_phase = PH_MERGE;
        } else {
          gsErrorMsg("option -p has illegal argument '%s'\n", optarg);
          exit(1);
        }
        break;
      case 'e': /* pretty */
        opt_pretty = true;
        break;
      case 'h': /* help */
        print_help(argv[0]);
        exit(0);
      case VERSION_OPTION: /* version */
        print_version_information(NAME);
        exit(0);
      case 'q': /* quiet */
        gsSetQuietMsg();
        break;
      case 'v': /* verbose */
        gsSetVerboseMsg();
        break;
      case 'd': /* debug */
        gsSetDebugMsg();
        break;
      case '?':
      default:
        print_more_info(argv[0]);
        exit(1);
    }
  }
  //check for presence of -f
  if (action_rename_filename == "") {
    gsErrorMsg("option -f is not specified\n");
    exit(1);
  }
  //check for wrong number of arguments
  string infilename;
  string outfilename;
  int noargc; //non-option argument count
  noargc = argc - optind;
  if (noargc > 2) {
    fprintf(stderr, "%s: too many arguments\n", NAME);
    print_more_info(argv[0]);
    exit(1);
  } else {
    //noargc >= 0 && noargc <= 2
    if (noargc > 0) {
      infilename = argv[optind];
    }
    if (noargc == 2) {
      outfilename = argv[optind + 1];
      //check if input and output files are the same; disabled since it is not
      //problematic
      /*
      if (strcmp(infilename,outfilename) == 0) {
        gsErrorMsg("input and output files are the same\n");
        exit(1);
      }
      */      
    }
  }
  tool_options.end_phase    = opt_end_phase;
  tool_options.pretty       = opt_pretty;
  tool_options.norewrite    = opt_norewrite;
  tool_options.nosumelm     = opt_nosumelm;
  tool_options.action_rename_filename = action_rename_filename;
  tool_options.infilename   = infilename;
  tool_options.outfilename  = outfilename;
  return tool_options;
}

template <typename IdentifierGenerator>
void rename_renamerule_variables(lps::data_expression& rcond, lps::action& rleft, lps::action& rright, IdentifierGenerator& generator)
{
  std::vector<data_variable> src;  // contains the variables that need to be renamed
  std::vector<data_variable> dest; // contains the corresponding replacements

  std::set<data_variable> new_vars;
  std::set<data_variable> found_vars;
  for(lps::data_expression_list::iterator rleft_argument_i = rleft.arguments().begin();
                                          rleft_argument_i != rleft.arguments().end();
                                        ++rleft_argument_i){
    found_vars = find_variables(*rleft_argument_i);
    new_vars.insert(found_vars.begin(), found_vars.end());
  }

  for (std::set<data_variable>::iterator i = new_vars.begin(); i != new_vars.end(); ++i){
    identifier_string new_name = generator(i->name());
    if (new_name != i->name())
    {
      src.push_back(*i);
      dest.push_back(data_variable(new_name, i->sort()));
    }
  }

  rcond = atermpp::partial_replace(rcond, detail::make_data_variable_replacer(src, dest));
  rleft = atermpp::partial_replace(rleft, detail::make_data_variable_replacer(src, dest));
  rright = atermpp::partial_replace(rright, detail::make_data_variable_replacer(src, dest));
}

lps::specification rewrite_lps(lps::specification lps){
  lps::summand_list lps_summands = lps.process().summands();
  lps::summand_list new_summands;
  lps::summand new_summand;
  lps::action_list lps_actions;
  lps::action_list new_actions;
  lps::action new_action;
  lps::data_expression_list lps_arguments;
  lps::data_expression_list new_arguments;
  lps::data_expression new_expression;
  lps::data_assignment_list lps_assignments;
  lps::data_assignment_list new_assignments;
  lps::data_assignment new_assignment;

  Rewriter *rewr = createRewriter(lps.data());

  new_summands = lps::summand_list();
  for(lps::summand_list::iterator si = lps_summands.begin(); si != lps_summands.end(); ++si){
    //rewrite the arguments of the actions
    lps_actions = si->actions();
    new_actions = lps::action_list();
    for(lps::action_list::iterator ai = lps_actions.begin(); ai != lps_actions.end(); ++ai){
      lps_arguments = ai->arguments();
      new_arguments = lps::data_expression_list();
      for(lps::data_expression_list::iterator argi = lps_arguments.begin();
                                              argi != lps_arguments.end();
                                            ++argi){
        new_expression = rewr->rewrite(*argi);
        new_arguments = push_front(new_arguments, new_expression);
      }
      new_arguments = reverse(new_arguments);
      new_action = lps::action(ai->label(), new_arguments);
      new_actions = push_front(new_actions, new_action);
    }
    new_actions = reverse(new_actions);
    new_summand = set_actions(*si, new_actions);

    //rewrite the assignments
    lps_assignments = si->assignments();
    new_assignments = lps::data_assignment_list();
    for(lps::data_assignment_list::iterator di = lps_assignments.begin();
                                            di != lps_assignments.end();
                                          ++di){
      new_expression = rewr->rewrite(di->rhs());
      new_assignment = lps::data_assignment(di->lhs(), new_expression);
      new_assignments = push_front(new_assignments, new_assignment);
    }
    new_assignments = reverse(new_assignments);
    new_summand = set_assignments(new_summand, new_assignments);

    //rewrite the condition
    new_expression = rewr->rewrite(si->condition());
    new_summand = set_condition(new_summand, new_expression);

    new_summands = push_front(new_summands, new_summand);
  }   
  new_summands = reverse(new_summands);
  return set_lps(lps, set_summands(lps.process(), new_summands));
}

ATermAppl merge_declarations(ATermAppl action_rename, lps::specification lps_spec){
// merges action declarations in action_rename with the sections in the lps
// specification lps_spec and resolves variable name conflicts
  lps::specification result;
  lps::linear_process result_lp = lps_spec.process();
  //action rename already contains all data elements of lps_spec since typechecking
  lps::data_specification new_data = lps::data_specification(ATAgetArgument(action_rename, 0));

  gsVerboseMsg("  Merging action labels...\n");

  //merge action labes
  lps::action_label_list lps_actions = lps_spec.action_labels();
  lps::action_label_list new_actions = lps::action_label_list(ATLgetArgument(ATAgetArgument(action_rename, 1),0));
  lps_actions = reverse(lps_actions);
  for(lps::action_label_list::iterator i=new_actions.begin(); i!=new_actions.end(); i++){
    lps_actions = push_front(lps_actions, *i);
  }
  lps_actions = reverse(lps_actions);

  gsVerboseMsg("  Resolving naming conflicts...\n");

  // Resolve name clashes between the rename rule variables and lps_spec
  aterm_list rename_rules = ATLgetArgument(ATAgetArgument(action_rename, 2), 0);
  aterm_appl rename_rule = *rename_rules.begin();
  aterm_appl::iterator j = rename_rule.begin();
  lps::data_variable_list rule_vars = lps::data_variable_list(*j);

  std::set<identifier_string> used_names;
  used_names.insert(boost::make_transform_iterator(rule_vars.begin(), detail::data_variable_name()),
                    boost::make_transform_iterator(rule_vars.end()  , detail::data_variable_name())
                   );

  lps::postfix_identifier_generator generator("_S");
  generator.add_identifiers(used_names);

  result_lp = rename_free_variables(result_lp, generator);
  result_lp = rename_summation_variables(result_lp, generator);
  result = lps::specification(new_data, lps_actions, result_lp, lps_spec.initial_process());
  result = rename_process_parameters(result, generator);

  return result;
}

ATermAppl rename(ATermAppl action_rename,lps::specification lps_old_spec,lps::specification lps_new_spec, bool norewrite, bool nosumelm){
  aterm_list rename_rules = ATLgetArgument(ATAgetArgument(action_rename, 2), 0);
  aterm_appl rename_rule;
  lps::summand_list lps_old_summands = lps_old_spec.process().summands();
  lps::summand_list lps_new_summands;
  lps::summand_list lps_summands = lps::summand_list(); //for changes in lps_old_summands
  lps::summand lps_old_summand;
  lps::summand lps_new_summand;
  lps::action_list lps_old_actions;
  lps::action_list lps_new_actions = lps::action_list();;
  lps::action lps_old_action;
  lps::data_expression lps_new_condition = lps::data_expression();
  lps::data_expression rule_condition;

  lps::data_variable_list lps_new_sum_vars = lps::data_variable_list();
  lps::action rule_old_action;
  lps::action rule_new_action;
  
  std::set<identifier_string> s;
  lps::postfix_identifier_generator generator("_R");
  generator.add_identifiers(s);

  aterm_appl new_element;

  bool rename;
  bool to_tau=false;
  bool to_delta=false;

  //go through the rename rules of the rename file
  gsVerboseMsg("rename rules found: %i\n", rename_rules.size());
  for(aterm_list::iterator i = rename_rules.begin(); i != rename_rules.end(); ++i){
    rename_rule = *i;
    aterm_appl::iterator j =  rename_rule.begin();
    //skipping the data_variable_list j
    rule_condition = lps::data_expression(*++j);
    rule_old_action =  lps::action(*++j);
    new_element = *++j;

    if(is_action(new_element)){
      rule_new_action =  lps::action(new_element);
      to_tau = false;
      to_delta = false;
    }
    else{
      rule_new_action = lps::action();
      if(gsIsTau(new_element)){ to_tau = true; to_delta = false;}
      else if (gsIsDelta(new_element)){ to_tau = false; to_delta = true;}
    }

    //rename all previously used variables
    rename_renamerule_variables(rule_condition, rule_old_action, rule_new_action, generator);

    lps_summands = summand_list();
    //go through the summands of the old lps
    gsVerboseMsg("summands found: %i\n", lps_old_summands.size());
    for(lps::summand_list::iterator losi = lps_old_summands.begin(); losi != lps_old_summands.end(); ++losi){
      if(!is_false(losi->condition()))
      {
        lps_old_summand = *losi;
        lps_old_actions = lps_old_summand.actions();
        lps_new_sum_vars = lps_old_summand.summation_variables();
        lps_new_condition = lps_old_summand.condition();

        lps_new_actions = lps::action_list();
        rename = false;
        //go through the actions of the summand
        gsVerboseMsg("actions in summand found: %i\n", lps_old_actions.size());
        for(lps::action_list::iterator loai = lps_old_actions.begin(); loai != lps_old_actions.end(); ++loai){
          lps_old_action = *loai;

          if(equal_signatures(lps_old_action, rule_old_action)) {
            gsVerboseMsg("renaming action...\n");
            rename = true;

            //go through the arguments of the action
            gsVerboseMsg("  arguments...\n");
            gsVerboseMsg("  args in action found: %i\n", lps_old_action.arguments().size());
            lps::data_expression_list::iterator lps_old_argument_i = lps_old_action.arguments().begin();
            for(lps::data_expression_list::iterator rule_old_argument_i = rule_old_action.arguments().begin();
                                                    rule_old_argument_i != rule_old_action.arguments().end();
                                                  ++rule_old_argument_i){
              //add new variables to the summation list and to the condition
              std::set<data_variable> new_vars = find_variables(*rule_old_argument_i);
              for(std::set<data_variable>::iterator sdvi = new_vars.begin(); sdvi != new_vars.end(); sdvi++){
                lps_new_sum_vars = push_front(lps_new_sum_vars, *sdvi);
                lps_new_condition = and_(lps_new_condition, lps::data_expr::equal_to(*rule_old_argument_i, *lps_old_argument_i));
              }//end data var loop
  
              ++lps_old_argument_i;
            }//end argument loop

            //change the old condition and make the new condition
            gsVerboseMsg("  condition...\n");
            if(is_nil(rule_condition) || is_true(rule_condition)){
              lps_old_summand = set_condition(lps_old_summand, false_());
            }
	    else
            {
              lps_old_summand = set_condition(lps_old_summand, and_(lps_new_condition, lps::data_expr::not_(rule_condition)));
              lps_old_summand = set_summation_variables(lps_old_summand, lps_new_sum_vars);
              lps_new_condition = and_(lps_new_condition, rule_condition);
            }

            //add the new action
            gsVerboseMsg("  add new action...\n");
            if(!to_tau && !to_delta){
              lps_new_actions = push_front(lps_new_actions, rule_new_action);
            }
            //else {skip}
          }//end if(equal_signatures(...))
          else{
            lps_new_actions = push_front(lps_new_actions, lps_old_action);
          }
          gsVerboseMsg("action done\n");

        } //end of action list iterator
  
        lps_new_actions = reverse(lps_new_actions);

        if(rename){
          //create a summand for the new lps
          if(to_delta){
            gsVerboseMsg("creating delta action for new lps...\n");
            lps_new_summand = lps::summand(
	       lps_new_sum_vars,
               lps_new_condition,
	       true,
               lps::action_list(),
               lps_old_summand.time(),
               lps::data_assignment_list());
          }
          else{
            gsVerboseMsg("creating summand for new lps...\n");
            lps_new_summand = lps::summand(
	       lps_new_sum_vars,
               lps_new_condition,
               lps_old_summand.is_delta(),
	       lps_new_actions,
               lps_old_summand.time(),
               lps_old_summand.assignments());
          }
          lps_new_summands = push_front(lps_new_summands, lps_new_summand);
        }
        lps_summands = push_front(lps_summands, lps_old_summand);
      }
    } //end of summand list iterator
    lps_old_summands = reverse(lps_summands);
  }//end of rename rule iterator

  //copy all old summands to the new lps
  gsVerboseMsg("adding leftovers from old lps...\n");
  for(lps::summand_list::iterator losi = lps_old_summands.begin(); losi != lps_old_summands.end(); ++losi){
    if(!is_false(losi->condition()))
    {
      lps_new_summands = push_front(lps_new_summands, *losi);
    }
  }
  lps_new_summands = reverse(lps_new_summands);

  gsVerboseMsg("simplifying the result...\n");
  lps_new_spec = set_lps(lps_new_spec, set_summands(lps_new_spec.process(), lps_new_summands));
  if(!norewrite) lps_new_spec = rewrite_lps(lps_new_spec);
  if(!nosumelm)  lps_new_spec = lps::sumelm(lps_new_spec);
  if(!nosumelm && !norewrite) lps_new_spec = rewrite_lps(lps_new_spec);

  gsVerboseMsg("new lps complete\n");
  return lps_new_spec;
} //end of rename(...)

ATermAppl rename_actions(t_tool_options tool_options)
{
  string infilename = tool_options.infilename;
  string outfilename = tool_options.outfilename;
  string action_rename_filename = tool_options.action_rename_filename;
  t_phase end_phase = tool_options.end_phase;

  lps::specification lps_old_spec = lps::specification();
  lps::specification lps_new_spec;
  //ATermAppl rename_rules;

  //open infilename
  gsVerboseMsg("loading lps...\n");
  if (infilename == "") {
    try
    {
      lps_old_spec.load("-");
    }
    catch (std::runtime_error e)
    {
      gsErrorMsg("cannot open LPS from stdin\n");
      return NULL;
    }
  } else {
    try
    {
      lps_old_spec.load(infilename);
    }
    catch (std::runtime_error e)
    {
      gsErrorMsg("cannot open LPS from '%s'\n", infilename.c_str());
      return NULL;
    }
  }

  //parse the action rename file
  gsVerboseMsg("parsing action rename from '%s'...\n", action_rename_filename.c_str());
  ifstream formstream(action_rename_filename.c_str(), ifstream::in|ifstream::binary);
  if (!formstream.is_open()) {
    gsErrorMsg("cannot open formula file '%s'\n", action_rename_filename.c_str());
    return NULL;
  }
  ATermAppl action_rename_spec = parse_action_rename_spec(formstream);
  formstream.close();
  if (action_rename_spec == NULL) {
    gsErrorMsg("parsing failed\n");
    return NULL;
  }
  gsDebugMsg("parsing succeeded\n");
  if (end_phase == PH_PARSE) {
    return action_rename_spec;
  }

  //type check formula
  gsVerboseMsg("type checking...\n");
  action_rename_spec = type_check_action_rename_spec(action_rename_spec, lps_old_spec);
  if (action_rename_spec == NULL) {
    gsErrorMsg("type checking failed\n");
    return NULL;
  }
  if (end_phase == PH_TYPE_CHECK) {
    return action_rename_spec;
  }

  //implement standard data types and type constructors on the result
  gsVerboseMsg("implementing standard data types and type constructors...\n");
  action_rename_spec = implement_data_action_rename_spec(action_rename_spec, lps_old_spec);
  if (action_rename_spec == NULL) {
    gsErrorMsg("data implementation failed\n");
    return NULL;
  }
  if (end_phase == PH_DATA_IMPL) {
    return action_rename_spec;
  }
  
  //merge declarations from lps_newspec and action_rename
  gsVerboseMsg("merging declarations...\n");
  aterm_appl result = merge_declarations(action_rename_spec, lps_old_spec);
  if (result == NULL) {
    return NULL;
  }
  lps_old_spec = lps::specification(result);
  if (end_phase == PH_MERGE) {
    return lps_old_spec;
  }

  //create lps_newspec
  lps::linear_process lps_old_spec_lp = lps_old_spec.process();
  lps::linear_process lps_new_spec_lp;
  lps_new_spec_lp = lps::linear_process(
			lps_old_spec_lp.free_variables(),
			lps_old_spec_lp.process_parameters(),
			lps::summand_list());

  lps_new_spec = lps::specification(
			lps_old_spec.data(),
			lps_old_spec.action_labels(),
			lps_new_spec_lp,
			lps_old_spec.initial_process());

  //rename all assigned actions
  gsVerboseMsg("renaming actions...\n");
  result = rename(action_rename_spec, lps_old_spec, lps_new_spec, tool_options.norewrite, tool_options.nosumelm);
  if (result == NULL) {
    return NULL;
  }
  lps_new_spec = lps::specification(result);

  return lps_new_spec;
}


static void print_help(char *name)
{
  fprintf(stderr, //TODO: change
    "Usage: %s [OPTION]... -f FILE [INFILE [OUTFILE]]\n"
    "Apply the action rename specification in FILE to the LPS in INFILE and save it\n"
    "to OUTFILE.\n"
    "If OUTFILE is not present, stdout is used. If INFILE is not present, stdin is\n"
    "used.\n"
    "\n"
    "  -fRENAMEFILE, --file=RENAMEFILE\n"
    "                        use the rename rules from FILE\n"
    "  -o, --no-rewrite      do not rewrite data terms while renaming;\n"
    "                        useful when the rewrite system does not terminate\n"
    "  -m, --no-sumelm       do not apply sum elimination to the final result\n"
    "  -pPHASE, --end-phase=PHASE\n"
    "                        stop conversion after phase PHASE and output the\n"
    "                        result; PHASE can be 'pa' (parse), 'tc' (type check) or\n"
    "                        'di' (data implementation), 'me' (merge data types)\n"
    "  -e, --external        return the result in the external format\n"
    "  -h, --help            display this help message and terminate\n"
    "      --version         display version information and terminate\n"
    "  -q, --quiet           do not display warning messages\n"
    "  -v, --verbose         display concise intermediate messages\n"
    "  -d, --debug           display detailed intermediate messages\n",
    name
  );
}

void print_more_info(char *name)
{
  fprintf(stderr, "Use %s --help for options\n", name);
}
