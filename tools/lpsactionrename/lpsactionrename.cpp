// Author(s): Tom Haenen, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsactionrename.cpp
/// \brief The tool lpsactionrename renames actions in an LPS

#include "boost.hpp" // precompiled headers

#define NAME "lpsactionrename"
#define AUTHOR "Jan Friso Groote and Tom Haenen"

#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <aterm2.h>
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/core/typecheck.h"
#include "mcrl2/core/data_implementation.h"
#include "mcrl2/core/data_reconstruct.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/lps/rename.h"
#include "mcrl2/lps/sumelm.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/rewrite.h"
#include "mcrl2/data/sort_identifier.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/command_line_messaging.h"
#include "mcrl2/utilities/command_line_rewriting.h"

using namespace atermpp;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::data::data_expr;
using namespace mcrl2::data;
using namespace mcrl2::lps;
using namespace std;

//Type definitions
//----------------

//t_phase represents the phases at which the program should be able to stop
typedef enum { PH_NONE, PH_PARSE, PH_TYPE_CHECK, PH_DATA_IMPL} t_phase;

//t_tool_options represents the options of the tool 
struct t_tool_options {
  bool            pretty;
  bool            no_rewrite;
  bool            no_sumelm;
  t_phase         end_phase;
  string          action_rename_filename;
  string          infilename;
  string          outfilename;
  RewriteStrategy rewrite_strategy;

  t_tool_options() : pretty(false), no_rewrite(false), no_sumelm(false),
    end_phase(PH_NONE), rewrite_strategy(GS_REWR_JITTY) {
  }
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

int process(t_tool_options const& tool_options) 
{
  //process action rename specfication
  ATermAppl result = rename_actions(tool_options);
  if (result == NULL) {
    return EXIT_FAILURE;
  }

  //store the result
  string outfilename = tool_options.outfilename;
  if (outfilename.empty()) {
    gsVerboseMsg("saving result to stdout...\n");
  } else {
    gsVerboseMsg("saving result to '%s'...\n", outfilename.c_str());
  }
  if (tool_options.end_phase == PH_NONE && !tool_options.pretty) {
    specification lps_spec(result);
    lps_spec.save(outfilename);
  } else {
    if (outfilename.empty()) {
      PrintPart_CXX(cout, (ATerm) result, (tool_options.pretty)?ppDefault:ppInternal);
      cout << endl;
    } else {
      ofstream outstream(outfilename.c_str(), ofstream::out|ofstream::binary);
      if (!outstream.is_open()) { 
        throw mcrl2::runtime_error("could not open output file '" + outfilename + "' for writing");
      }
      PrintPart_CXX(outstream, (ATerm) result, (tool_options.pretty)?ppDefault:ppInternal);
      outstream.close();
    }
  }
  return EXIT_SUCCESS;
}

// SQuADT protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include "mcrl2/utilities/mcrl2_squadt_interface.h"

class squadt_interactor : public mcrl2::utilities::squadt::mcrl2_tool_interface {

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

const char* lps_file_for_input    = "lps_in";
const char* rename_file           = "renamefile_in";
const char* lps_file_for_output   = "lps_out";
const char* option_no_rewrite     = "no_rewrites";
const char* option_no_sumelm      = "no_sumelm";

void squadt_interactor::set_capabilities(tipi::tool::capabilities& c) const {
  c.add_input_configuration(lps_file_for_input, tipi::mime_type("lps", tipi::mime_type::application), tipi::tool::category::transformation);
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& c) {
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::layout::elements;

  if (!c.option_exists(option_no_rewrite)) {
    c.add_option(option_no_rewrite).set_argument_value< 0 >(false);
  }
  if (!c.option_exists(option_no_sumelm)) {
    c.add_option(option_no_sumelm).set_argument_value< 0 >(false);
  }

  /* Create display */
  tipi::tool_display d;

  layout::vertical_box& m = d.create< vertical_box >().set_default_margins(margins(0,5,0,5));

  /* Create and add the top layout manager */
  text_field& rename_file_field = d.create< text_field >();
  checkbox&   rewrite           = d.create< checkbox >().set_status(!c.get_option_argument< bool >(option_no_rewrite));
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
  if (c.input_exists(rename_file)) {
    rename_file_field.set_text(c.get_input(rename_file).location());
  }

  send_display_layout(d.manager(m));

  /* Wait until the ok button was pressed */
  okay_button.await_change();

  c.add_input(rename_file, tipi::mime_type("mf", tipi::mime_type::text), rename_file_field.get_text());

  /* Add output file to the configuration */
  if (c.output_exists(lps_file_for_output)) {
    tipi::configuration::object& output_file = c.get_output(lps_file_for_output);

    output_file.location(c.get_output_name(".lps"));
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
  result &= c.input_exists(rename_file);
  result &= c.output_exists(lps_file_for_output);

  return (result);
}

bool squadt_interactor::perform_task(tipi::configuration& c) {
  t_tool_options tool_options;

  tool_options.action_rename_filename = c.get_input(rename_file).location();
  tool_options.infilename             = c.get_input(lps_file_for_input).location();
  tool_options.outfilename            = c.get_output(lps_file_for_output).location();
  tool_options.no_sumelm              = c.get_option_argument< bool >(option_no_sumelm);
  tool_options.no_rewrite             = c.get_option_argument< bool >(option_no_rewrite);

  bool result = process(tool_options) == 0;

  if (result) {
    send_clear_display();
  }

  return (result);
}
#endif //ENABLE_SQUADT_CONNECTIVITY

static t_tool_options parse_command_line(int argc, char **argv)
{
  interface_description clinterface(argv[0], NAME, AUTHOR, "[OPTION]... --file=RENAMEFILE [INFILE [OUTFILE]]\n",
    "Apply the action rename specification in FILE to the LPS in INFILE and save it to OUTFILE. "
    "If OUTFILE is not present, stdout is used. If INFILE is not present, stdin is used.");

  clinterface.add_rewriting_options();

  clinterface.add_option("file", make_mandatory_argument("RENAMEFILE"),
     "use the rename rules from RENAMEFILE", 'f');
  clinterface.add_option("no-rewrite",
     "do not rewrite data terms while renaming; useful when the rewrite system "
     "does not terminate", 'o');
  clinterface.add_option("no-sumelm",
     "do not apply sum elimination to the final result", 'm');
  clinterface.add_option("end-phase", make_mandatory_argument("PHASE"),
      "stop conversion and output the action rename specification after phase PHASE: "
      "'pa' (parsing), "
      "'tc' (type checking), or "
      "'di' (data implementation)"
    , 'p');
  clinterface.add_option("pretty",
      "return a pretty printed version of the output", 'P');

  command_line_parser parser(clinterface, argc, argv);

  t_tool_options tool_options;

  tool_options.no_rewrite = 0 < parser.options.count("no-rewrite"); 
  tool_options.no_sumelm  = 0 < parser.options.count("no-sumelm"); 
  tool_options.pretty     = 0 < parser.options.count("pretty"); 

  if (parser.options.count("end-phase")) {
    std::string phase = parser.option_argument("end-phase");

    if (std::strncmp(phase.c_str(), "pa", 3) == 0) {
      tool_options.end_phase = PH_PARSE;
    } else if (std::strncmp(phase.c_str(), "tc", 3) == 0) {
      tool_options.end_phase = PH_TYPE_CHECK;
    } else if (std::strncmp(phase.c_str(), "di", 3) == 0) {
      tool_options.end_phase = PH_DATA_IMPL;
    } else {
      parser.error("option -p has illegal argument '" + phase + "'");
    }
  }

  tool_options.rewrite_strategy = parser.option_argument_as< RewriteStrategy >("rewriter");

  if (parser.options.count("file")) {
    tool_options.action_rename_filename = parser.option_argument("file");
  }
  else {
    parser.error("option -f is not specified");
  }

  if (2 < parser.arguments.size()) {
    parser.error("too many file arguments");
  }
  else {
    if (0 < parser.arguments.size()) {
      tool_options.infilename = parser.arguments[0];
    }
    if (1 < parser.arguments.size()) {
      tool_options.outfilename = parser.arguments[1];
    }
  }

  return tool_options;
}

//Main program
//------------

int main(int argc, char **argv)
{
  MCRL2_ATERM_INIT(argc, argv)

  try 
  {
#ifdef ENABLE_SQUADT_CONNECTIVITY
    if (mcrl2::utilities::squadt::interactor< squadt_interactor >::free_activation(argc, argv)) 
    { return EXIT_SUCCESS;
    }
#endif

    return process(parse_command_line(argc, argv));
  }
  catch (std::exception& e) 
  { 
    std::cerr << e.what() << std::endl;
  }

  return EXIT_FAILURE;
}

template <typename IdentifierGenerator>
void rename_renamerule_variables(data_expression& rcond, action& rleft, action& rright, IdentifierGenerator& generator)
{
  std::vector<data_variable> src;  // contains the variables that need to be renamed
  std::vector<data_variable> dest; // contains the corresponding replacements

  std::set<data_variable> new_vars;
  std::set<data_variable> found_vars;
  for(data_expression_list::iterator rleft_argument_i = rleft.arguments().begin();
                                          rleft_argument_i != rleft.arguments().end();
                                        ++rleft_argument_i){
    found_vars = find_all_data_variables(*rleft_argument_i);
    new_vars.insert(found_vars.begin(), found_vars.end());
  }

  for (std::set<data_variable>::iterator i = new_vars.begin(); i != new_vars.end(); ++i)
  { identifier_string new_name = generator(i->name());
    if (new_name != i->name())
    {
      src.push_back(*i);
      dest.push_back(data_variable(new_name, i->sort()));
    }
  }

  rcond = atermpp::partial_replace(rcond, mcrl2::lps::detail::make_data_variable_replacer(src, dest));
  rleft = atermpp::partial_replace(rleft, mcrl2::lps::detail::make_data_variable_replacer(src, dest));
  rright = atermpp::partial_replace(rright, mcrl2::lps::detail::make_data_variable_replacer(src, dest));
}

specification rewrite_lps(specification lps, RewriteStrategy rewrite_strategy)
{
  summand_list lps_summands = lps.process().summands();
  summand_list new_summands;
  summand new_summand;
  action_list lps_actions;
  action_list new_actions;
  action new_action;
  data_expression_list lps_arguments;
  data_expression_list new_arguments;
  data_expression new_expression;
  data_assignment_list lps_assignments;
  data_assignment_list new_assignments;
  data_assignment new_assignment;

  Rewriter *rewr = createRewriter(lps.data(), rewrite_strategy);

  new_summands = summand_list();
  for(summand_list::iterator si = lps_summands.begin(); si != lps_summands.end(); ++si)
  {
    //rewrite the arguments of the actions
    lps_actions = si->actions();
    new_actions = action_list();
    for(action_list::iterator ai = lps_actions.begin(); ai != lps_actions.end(); ++ai){
      lps_arguments = ai->arguments();
      new_arguments = data_expression_list();
      for(data_expression_list::iterator argi = lps_arguments.begin();
                                              argi != lps_arguments.end();
                                            ++argi){
        new_expression = rewr->rewrite(*argi);
        new_arguments = push_front(new_arguments, new_expression);
      }
      new_arguments = reverse(new_arguments);
      new_action = action(ai->label(), new_arguments);
      new_actions = push_front(new_actions, new_action);
    }
    new_actions = reverse(new_actions);
    new_summand = set_actions(*si, new_actions);

    //rewrite the assignments
    lps_assignments = si->assignments();
    new_assignments = data_assignment_list();
    for(data_assignment_list::iterator di = lps_assignments.begin();
                                            di != lps_assignments.end();
                                          ++di){
      new_expression = rewr->rewrite(di->rhs());
      new_assignment = data_assignment(di->lhs(), new_expression);
      new_assignments = push_front(new_assignments, new_assignment);
    }
    new_assignments = reverse(new_assignments);
    new_summand = set_assignments(new_summand, new_assignments);

    //rewrite the condition
    new_expression = rewr->rewrite(si->condition());

    if (!is_false(new_expression))
    { new_summand = set_condition(new_summand, new_expression);
      new_summands = push_front(new_summands, new_summand);
    }
  }   
  return set_lps(lps, set_summands(lps.process(), new_summands));
}

/****************  Main rename routine   ****************/

ATermAppl rename(
            ATermAppl action_rename,
            specification lps_old_spec,
            RewriteStrategy rewrite_strategy,
            bool norewrite, 
            bool nosumelm)
{
  // In the line below the reverse is necessary, because the parser puts the
  // rename rules in the reversed order compared to the order they appear in the text.
  aterm_list rename_rules = ATreverse(ATLgetArgument(ATAgetArgument(action_rename, 2), 0));
  summand_list lps_old_summands = lps_old_spec.process().summands();
  summand_list lps_summands = summand_list(); //for changes in lps_old_summands
  action_list lps_new_actions = action_list();;

  postfix_identifier_generator generator("");
  generator.add_to_context(lps_old_spec);

  bool to_tau=false;
  bool to_delta=false;

  //go through the rename rules of the rename file
  gsVerboseMsg("rename rules found: %i\n", rename_rules.size());
  for(aterm_list::iterator i = rename_rules.begin(); i != rename_rules.end(); ++i)
  {
    summand_list lps_new_summands;
    aterm_appl rename_rule = *i;
    
    aterm_appl::iterator j =  rename_rule.begin();
    //skipping the data_variable_list j
    data_expression rule_condition = data_expression(*++j);     
    action rule_old_action =  action(*++j);      
    action rule_new_action;
    aterm_appl new_element = *++j;

    if(is_action(new_element))
    {
      rule_new_action =  action(new_element);
      to_tau = false;
      to_delta = false;
    }
    else
    {
      rule_new_action = action();
      if(mcrl2::core::detail::gsIsTau(new_element)){ to_tau = true; to_delta = false;}
      else if (mcrl2::core::detail::gsIsDelta(new_element)){ to_tau = false; to_delta = true;}
    }

    // Check here that the arguments of the rule_old_action only consist
    // of uniquely occurring variables or closed terms. Furthermore, check that the variables
    // in rule_new_action and in rule_condition are a subset of those in
    // rule_old_action. This check ought to be done in the static checking
    // part of the renaming rules, but as yet it has nog been done. Ultimately
    // this check should be moved there.

    // first check that the arguments of rule_old_action are variables or closed 
    // terms.

    for(data_expression_list::iterator
                       rule_old_argument_i = rule_old_action.arguments().begin();
                       rule_old_argument_i != rule_old_action.arguments().end();
                       rule_old_argument_i++)
    { if ((!is_data_variable(*rule_old_argument_i)) &&
          (!(find_all_data_variables(*rule_old_argument_i).empty())))
      { std::cerr << "The arguments of the lhs " << pp(rule_old_action) << 
                          " are not variables or closed expressions\n";
        exit(1);
      }
    }
  
    // Check whether the variables in rhs are included in the lefthandside.
    std::set < data_variable > variables_in_old_rule = find_all_data_variables(rule_old_action);
    std::set < data_variable > variables_in_new_rule = find_all_data_variables(rule_new_action);

    if (!includes(variables_in_old_rule.begin(),variables_in_old_rule.end(),
                  variables_in_new_rule.begin(),variables_in_new_rule.end()))
    { std::cerr << "There are variables occurring in rhs " << pp(rule_new_action) << 
                   " of a rename rule not occurring in lhs " << pp(rule_old_action) << "\n";
      exit(1);
    }

    // Check whether the variables in condition are included in the lefthandside.
    std::set < data_variable > variables_in_condition = find_all_data_variables(rule_condition);
    if (!includes(variables_in_old_rule.begin(),variables_in_old_rule.end(),
                  variables_in_condition.begin(),variables_in_condition.end()))
    { std::cerr << "There are variables occurring in the condition " << pp(rule_condition) << 
                   " of a rename rule not occurring in lhs " << pp(rule_old_action) << "\n";
      exit(1);
    }

    // check for double occurrences of variables in the lhs. Note that variables_in_old_rule
    // is empty at the end.
    for(data_expression_list::iterator i=rule_old_action.arguments().begin() ;
                     i!=rule_old_action.arguments().end() ; i++)
    { if (is_data_variable(*i))
      { if (variables_in_old_rule.find(*i)==variables_in_old_rule.end())
        { std::cerr << "Variable " << pp(*i) << " occurs more than once in lhs " << 
                       pp(rule_old_action) << " of an action rename rule\n";
          exit(1);
        }
        else
        { variables_in_old_rule.erase(*i);
        }
      }
    }
    assert(variables_in_old_rule.empty());
 

    lps_summands = summand_list();
    //go through the summands of the old lps
    gsVerboseMsg("summands found: %i\n", lps_old_summands.size());
    for(summand_list::iterator losi = lps_old_summands.begin(); 
                                    losi != lps_old_summands.end(); ++losi)
    {
      summand lps_old_summand = *losi;
      action_list lps_old_actions = lps_old_summand.actions();

      /* For each individual action in the multi-action, for which the 
         rename rule applies, two new summands must be made, namely one
         where the rule does not match with the parameters of the action,
         and one where it actually does. This means that for a multiaction
         with k summands 2^k new summands can result. */

      atermpp::vector < data_variable_list >  
                           lps_new_sum_vars(1,lps_old_summand.summation_variables());
      atermpp::vector < data_expression > 
                         lps_new_condition(1,lps_old_summand.condition());
      atermpp::vector < std::pair <bool, action_list > >
                           lps_new_actions(1,std::make_pair(lps_old_summand.is_delta(),action_list()));
      
      gsVerboseMsg("actions in summand found: %i\n", lps_old_actions.size());
      for(action_list::iterator loai = lps_old_actions.begin(); 
                loai != lps_old_actions.end(); loai++)
      { 
        action lps_old_action = *loai;

        if (equal_signatures(lps_old_action, rule_old_action)) 
        {
          gsVerboseMsg("renaming action %P\n",(ATermAppl)rule_old_action);

          //rename all previously used variables
          data_expression renamed_rule_condition=rule_condition;
          action renamed_rule_old_action=rule_old_action;
          action renamed_rule_new_action=rule_new_action;
          rename_renamerule_variables(renamed_rule_condition, renamed_rule_old_action, renamed_rule_new_action, generator);

          if (is_nil(renamed_rule_condition))
          { renamed_rule_condition=true_();
          }

          //go through the arguments of the action
          data_expression_list::iterator 
                    lps_old_argument_i = lps_old_action.arguments().begin();
          data_expression new_equalities_condition=true_();
          for(data_expression_list::iterator 
                       rule_old_argument_i = renamed_rule_old_action.arguments().begin();
                       rule_old_argument_i != renamed_rule_old_action.arguments().end();
                       rule_old_argument_i++)
          { if (is_data_variable(*rule_old_argument_i))
            { 
              new_equalities_condition=optimized::and_(new_equalities_condition,
                               data_expr::equal_to(*rule_old_argument_i, *lps_old_argument_i));
            }
            else 
            { assert((find_all_data_variables(*rule_old_argument_i).empty())); // the argument must be closed, 
                                                                               // which is checked above.
              renamed_rule_condition=
                        optimized::and_(renamed_rule_condition,
                             data_expr::equal_to(*rule_old_argument_i, *lps_old_argument_i));
            }
            lps_old_argument_i++;
          }

          /* insert the new equality condition in all the newly generated summands */
          for (atermpp::vector < data_expression > :: iterator i=lps_new_condition.begin() ;
                       i!=lps_new_condition.end() ; i++ )
          { *i=optimized::and_(*i,new_equalities_condition);
          }

          /* insert the new sum variables in all the newly generated summands */
          std::set<data_variable> new_vars = find_all_data_variables(renamed_rule_old_action);
          for(std::set<data_variable>::iterator sdvi = new_vars.begin(); 
                         sdvi != new_vars.end(); sdvi++)
          { 
            for ( atermpp::vector < data_variable_list > :: iterator i=lps_new_sum_vars.begin() ;
                        i!=lps_new_sum_vars.end() ; i++ )
            { *i = push_front(*i, *sdvi);
            }
          }

          if (is_true(renamed_rule_condition))
          { 
            if (to_delta)
            { for(atermpp::vector < std::pair <bool, action_list > > :: iterator 
                      i=lps_new_actions.begin() ;
                      i!=lps_new_actions.end() ; i++ )
              { *i=std::make_pair(true,action_list()); /* the action becomes delta */
              }
            }
            else if (!to_tau)
            { for(atermpp::vector < std::pair <bool, action_list > > :: iterator i=lps_new_actions.begin() ;
                        i!=lps_new_actions.end() ; i++ )
              { if (!((*i).first)) // the action is not delta
                { *i=std::make_pair(false,push_front((*i).second,renamed_rule_new_action));
                }
              }
            }
          }
          else if (is_false(renamed_rule_condition))
          {
            for(atermpp::vector < std::pair <bool, action_list > > :: iterator i=lps_new_actions.begin() ;
                        i!=lps_new_actions.end() ; i++ )
            { if (!((*i).first)) // The action does not equal delta.
              { *i=std::make_pair(false,push_front((*i).second,lps_old_action));
              }
            }

          }
          else
          { /* Duplicate summands, one where the renaming is applied, and one where it is not
               applied. */

            atermpp::vector < std::pair <bool, action_list > > lps_new_actions_temp(lps_new_actions);

            if (!to_tau) // if the new element is tau, we do not insert it in the multi-action.
            { for(atermpp::vector < std::pair <bool, action_list > > :: iterator 
                        i=lps_new_actions.begin() ;
                        i!=lps_new_actions.end() ; i++ )
              { if (to_delta) 
                { *i=std::make_pair(true,action_list());
                }
                else 
                { *i=std::make_pair(false,push_front(i->second,renamed_rule_new_action));
                }
              }
            }

            for(atermpp::vector < std::pair <bool, action_list > > :: iterator      
                        i=lps_new_actions_temp.begin() ;
                        i!=lps_new_actions_temp.end() ; i++ )
            { if (!(i->first)) // The element is not equal to delta 
              { *i=std::make_pair(false,push_front(i->second,lps_old_action));
              }
            }

            lps_new_actions.insert(lps_new_actions.end(),
                                   lps_new_actions_temp.begin(),
                                   lps_new_actions_temp.end()); 
              

            /* lps_new_condition_temp will contain the conditions in conjunction with
               the negated new_condition. It will be concatenated to lps_new_condition,
               in which the terms will be conjoined with the non-negated new_condition */

            atermpp::vector < data_expression > lps_new_condition_temp(lps_new_condition);

            for (atermpp::vector < data_expression > :: iterator i=lps_new_condition.begin() ;
                         i!=lps_new_condition.end() ; i++ )
            { *i=optimized::and_(*i,renamed_rule_condition);
            }

            for (atermpp::vector < data_expression > :: iterator i=lps_new_condition_temp.begin() ;
                         i!=lps_new_condition_temp.end() ; i++ )
            { *i=optimized::and_(*i,not_(renamed_rule_condition));
            }

            lps_new_condition.insert(lps_new_condition.end(),
                                     lps_new_condition_temp.begin(),
                                     lps_new_condition_temp.end());
            
            lps_new_sum_vars.insert(lps_new_sum_vars.end(),lps_new_sum_vars.begin(),lps_new_sum_vars.end());
          }

        }//end if(equal_signatures(...))
        else
        { for ( atermpp::vector < std::pair <bool, action_list > > :: iterator i=lps_new_actions.begin() ;
                i!=lps_new_actions.end() ; i++ )
          { *i = std::make_pair((*i).first,push_front((*i).second, lps_old_action));
          }
        }
        gsVerboseMsg("action done\n");

      } //end of action list iterator

      /* Add the summands to lps_new_summands */

      atermpp::vector < std::pair <bool, action_list > > :: iterator i_act=lps_new_actions.begin();
      atermpp::vector < data_variable_list > :: iterator i_sumvars=lps_new_sum_vars.begin();
      for( atermpp::vector < data_expression > :: iterator i_cond=lps_new_condition.begin() ;
           i_cond!=lps_new_condition.end() ; i_cond++)
      { 
        //create a summand for the new lps
        summand lps_new_summand = summand(
                                           *i_sumvars,
                                           *i_cond,
                                           (*i_act).first,
                                           reverse((*i_act).second), 
                                           lps_old_summand.time(),
                                           lps_old_summand.assignments());
        lps_new_summands = push_front(lps_new_summands, lps_new_summand);
        i_act++;
        i_sumvars++;
      }
    } // end of summand list iterator
    lps_old_summands = lps_new_summands;
  } //end of rename rule iterator

  gsVerboseMsg("simplifying the result...\n");

  specification lps_new_spec = specification(
                                          lps_old_spec.data(),
                                          lps_old_spec.action_labels(),
                                          linear_process(
                                                      lps_old_spec.process().free_variables(),
                                                      lps_old_spec.process().process_parameters(),
                                                      lps_old_summands), // These are the renamed sumands.
                                          lps_old_spec.initial_process());

  if(!norewrite) lps_new_spec = rewrite_lps(lps_new_spec, rewrite_strategy);
  if(!nosumelm)  lps_new_spec = sumelm(lps_new_spec);
  if(!nosumelm && !norewrite) lps_new_spec = rewrite_lps(lps_new_spec, rewrite_strategy);

  gsVerboseMsg("new lps complete\n");
  return lps_new_spec;
} //end of rename(...)

ATermAppl rename_actions(t_tool_options tool_options)
{
  string outfilename = tool_options.outfilename;
  string action_rename_filename = tool_options.action_rename_filename;
  t_phase end_phase = tool_options.end_phase;

  //open infilename
  gsVerboseMsg("loading lps...\n");
  specification lps_old_spec;
  lps_old_spec.load(tool_options.infilename);

  //parse the action rename file
  gsVerboseMsg("parsing action rename from '%s'...\n", action_rename_filename.c_str());
  ifstream formstream(action_rename_filename.c_str());
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

  //type check formula against a reconstructed lps specification
  ATermAppl reconstructed_lps_old_spec = reconstruct_spec(lps_old_spec);

  gsVerboseMsg("type checking...\n");
  action_rename_spec = type_check_action_rename_spec(action_rename_spec, reconstructed_lps_old_spec);
  if (action_rename_spec == NULL) {
    gsErrorMsg("type checking failed\n");
    return NULL;
  }
  if (end_phase == PH_TYPE_CHECK) {
    return action_rename_spec;
  }

  //implement standard data types and type constructors on the result
  gsVerboseMsg("implementing standard data types and type constructors...\n");
  action_rename_spec = implement_data_action_rename_spec(action_rename_spec, reconstructed_lps_old_spec);
  if (action_rename_spec == NULL) {
    gsErrorMsg("data implementation failed\n");
    return NULL;
  }
  if (end_phase == PH_DATA_IMPL) {
    return action_rename_spec;
  }
  lps_old_spec = specification(reconstructed_lps_old_spec);

  //rename all assigned actions
  gsVerboseMsg("renaming actions...\n");
  return rename(action_rename_spec, lps_old_spec, tool_options.rewrite_strategy, tool_options.no_rewrite, tool_options.no_sumelm);
}
