// Author(s): Tom Haenen
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsactionrename.cpp
/// \brief Add your file description here.

#define NAME "lpsactionrename"
#define VERSION "0.0"

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
#include "libstruct.h"
#include "libprint.h"
#include "libprint_c.h"
#include "libparse.h"
#include "typecheck.h"
#include "mcrl2/dataimpl.h"
#include "regfrmtrans.h"
#include "mcrl2/lps/specification.h"
#include "print/messaging.h"

using namespace mcrl2::utilities;
using namespace std;
using namespace lps;
using namespace data_expr;

//Type definitions
//----------------

//t_phase represents the phases at which the program should be able to stop
typedef enum { PH_NONE, PH_PARSE, PH_TYPE_CHECK, PH_DATA_IMPL, PH_MERGE, PH_RENAME} t_phase;

//t_tool_options represents the options of the tool 
typedef struct {
  bool pretty;
  t_phase end_phase;
  string action_rename_filename;
  string infilename;
  string outfilename;
} t_tool_options;

//Functions used by the main program
//----------------------------------

static t_tool_options parse_command_line(int argc, char **argv);
//Post: The command line options are parsed.
//      The program has aborted with a suitable error code, if:
//      - errors were encountered
//      - non-standard behaviour was requested (help or version)
//Ret:  the parsed command line options

static ATermAppl rename_lps_actions(t_tool_options tool_options);
//Pre:  tool_options.action_rename_filename contains a action rename
//      specification
//      tool_options.infilename contains an LPS ("" indicates stdin)
//      tool_options.end_phase indicates at which phase conversion stops
//Ret:  if end_phase == PH_NONE, the new LPS generated from the action rename 
//      file and the old LPS
//      if end_phase != PH_NONE, the state formula after phase end_phase
//      NULL, if something went wrong

static void print_help(char *name);
static void print_version(void);
static void print_more_info(char *name);

//Main program
//------------

int main(int argc, char **argv)
{
  //parse command line
  t_tool_options tool_options = parse_command_line(argc, argv);

  //initialise ATerm library
  ATerm stackbot;
  ATinit(argc,argv,&stackbot);
  gsEnableConstructorFunctions();

  //process action rename specfication
  ATermAppl result = rename_lps_actions(tool_options);
  if (result == NULL) {
    return 1;
  }

  //store the result
  string outfilename = tool_options.outfilename;
  bool opt_pretty = tool_options.pretty;
  if (outfilename == "") {
    gsVerboseMsg("saving result to stdout...\n");
    PrintPart_CXX(cout, (ATerm) result, opt_pretty?ppDefault:ppInternal);//TODO: change +Q? why not load/save
    cout << endl;
  } else {
    gsVerboseMsg("saving result to '%s'...\n", outfilename.c_str());
    ofstream outstream(outfilename.c_str(), ofstream::out|ofstream::binary);
    if (!outstream.is_open()) {
      gsErrorMsg("cannot open output file '%s'\n", outfilename.c_str());
      return 1;
    }
    PrintPart_CXX(outstream, (ATerm) result, opt_pretty?ppDefault:ppInternal);//TODO: change
    outstream.close();
  }
  return 0;
}

static t_tool_options parse_command_line(int argc, char **argv)
{
  t_tool_options tool_options;
  //declarations for getopt
  t_phase opt_end_phase = PH_NONE;
  bool opt_pretty = false;
  string action_rename_filename = "";
  #define SHORT_OPTIONS "f:p:ehqvd"
  #define VERSION_OPTION CHAR_MAX + 1
  struct option long_options[] = {
    { "file",      required_argument,  NULL,  'f' },
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
      case 'f': /* formula */
        action_rename_filename = optarg;
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
        } else if (strcmp(optarg, "re") == 0) {
          opt_end_phase = PH_RENAME;
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
        print_version();
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
  tool_options.action_rename_filename = action_rename_filename;
  tool_options.infilename   = infilename;
  tool_options.outfilename  = outfilename;
  return tool_options;
}

ATermAppl merge_decls(ATermAppl action_rename, ATermAppl lps_newspec){
// merges the declarations in the data section and the actions declared in the action rename file action_rename
// with the sections in the lps specification lps_newspec
  return lps_newspec;
}

bool compare_action(ATermAppl action1, ATermAppl action2){ //move this function to action class
//  string name = action1.label().name();
//  if (name.compare(action2.label().name())!=0) return false;
//
//  lps::data_expression_list::iterator arg1 = action1.arguments().begin();
//  lps::data_expression_list::iterator arg2 = action2.arguments().begin();
//  if (arg1->size() != arg2->size()) return false;
//  for(; arg1 != ; ++arg1){
//    if(arg1->sort() != arg2->sort()) return false;
//    ++arg2;
//  }
  return true;
}


ATermAppl rename(ATermAppl action_rename,lps::specification lps_old_spec,lps::specification lps_new_spec){
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
  lps::action lps_new_action;
  lps::data_expression lps_old_condition;
  lps::data_expression lps_new_condition = lps::data_expression();
  lps::data_expression rule_condition;

  lps::data_variable_list rule_vars;
  lps::data_variable_list lps_new_sum_vars = lps::data_variable_list();
  lps::action rule_old_action;
  lps::action rule_new_action;

  lps::data_expression_list new_arguments;

  string delta = "delta";
  string tau = "tau";

  bool rename;

  //go through the rename rules of the rename file
  gsVerboseMsg("rename rules found: %i\n", rename_rules.size());
  for(aterm_list::iterator i = rename_rules.begin(); i != rename_rules.end(); ++i){
    rename_rule = *i;
    aterm_appl::iterator j =  rename_rule.begin();
    rule_vars = lps::data_variable_list(*j++);
    cerr<<"rule_condition = data_expression("<< *j<<");\n";
    rule_condition = lps::data_expression(*j++);
    rule_old_action =  lps::action(*j++);
    rule_new_action =  lps::action(*j);
    //TODO: check if no variables in the rename file are equal to the process or free variables; if so change names of rename vars.

    lps_summands = summand_list();
    //go through the summands of the old lps
    gsVerboseMsg("summands found: %i\n", lps_old_summands.size());
    for(lps::summand_list::iterator losi = lps_old_summands.begin(); losi != lps_old_summands.end(); ++losi){
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

        for(lps::data_expression_list::iterator rule_old_argument_i = rule_old_action.arguments().begin();
                                                rule_old_argument_i != rule_old_action.arguments().end();
                                              ++rule_old_argument_i){
          
        }

        if(equal_signatures(lps_old_action, rule_old_action)) {
          gsVerboseMsg("renaming action...\n");
          rename = true;
          //change the old condition and make the new condition
          gsVerboseMsg("  condition...\n");
          if(is_nil(rule_condition) || is_true(rule_condition)){
            lps_old_summand = set_condition(lps_old_summand, false_());
          }
	  else
          {
            lps_new_condition = and_(lps_new_condition, rule_condition);
            lps_old_summand = set_condition(lps_old_summand, and_(lps_old_summand.condition(), lps::data_expr::not_(lps_new_condition)));
          }

          //go through the arguments of the action
          gsVerboseMsg("  arguments...\n");
          gsVerboseMsg("  args in action found: %i\n", lps_old_action.arguments().size());
          lps::data_expression_list::iterator lps_old_argument_i = lps_old_action.arguments().begin();
          for(lps::data_expression_list::iterator rule_old_argument_i = rule_old_action.arguments().begin();
                                                  rule_old_argument_i != rule_old_action.arguments().end();
                                                ++rule_old_argument_i){
            //add new variables to the summation list and to the condition
            std::set<data_variable> new_vars = find_variables(*rule_old_argument_i);
            gsVerboseMsg("  vars in arg found: %i\n", new_vars.size());
            for(std::set<data_variable>::iterator sdvi = new_vars.begin(); sdvi != new_vars.end(); sdvi++){
              lps_new_sum_vars = push_front(lps_new_sum_vars, *sdvi);
            }
            lps_new_condition = and_(lps_new_condition, lps::data_expr::equal_to(*rule_old_argument_i, *lps_old_argument_i));

            ++lps_old_argument_i;
          }

          //add the new action
          gsVerboseMsg("   new action...\n");
          if((tau.compare(rule_new_action.label().name())!=0)||(lps_old_actions.size()!=1)){//gives empty action list when lps: a|a, rename: a =>tau
            lps_new_actions = push_front(lps_new_actions, rule_new_action);
          }
          //else {skip}
        }
        else{
          lps_new_actions = push_front(lps_new_actions, lps_old_action);
        }
        gsVerboseMsg("action done\n");

      } //end of action list iterator

      if((tau.compare(rule_new_action.label().name())!=0) && (lps_new_actions.size() != 0)){
        lps_new_actions = reverse(lps_new_actions);
      }
      else{
        lps_new_actions = push_back(lps_new_actions, lps::action(lps::action_label(tau, lps::sort_list()), lps::data_expression_list()));
      }

      if(rename){
        //create a summand for the new lps
        if(delta.compare(rule_new_action.label().name()) == 0){
          gsVerboseMsg("creating delta action for new lps...\n");
          lps_new_actions = push_back(lps::action_list(), rule_new_action);
          lps_new_summand = lps::summand(
	     lps_new_sum_vars,
             lps_new_condition,
	     true,
  	     lps_new_actions,
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
    } //end of summand list iterator
    lps_old_summands = reverse(lps_summands);
  }//end of rename rule iterator

  //copy all old summands to the new lps
  gsVerboseMsg("adding left overs from old lps...\n");
  for(lps::summand_list::iterator losi = lps_old_summands.begin(); losi != lps_old_summands.end(); ++losi){
    if(!is_false(losi->condition()))
    {
      lps_new_summands = push_front(lps_new_summands, *losi);
    }
  }
  lps_new_summands = reverse(lps_new_summands);

  lps_new_spec = set_lps(lps_new_spec, set_summands(lps_new_spec.process(), lps_new_summands));
  return lps_new_spec;
} //end of rename(...)

ATermAppl rename_lps_actions(t_tool_options tool_options)
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
  if (lps_old_spec == NULL) {
    gsErrorMsg("data implementation failed\n");
    return NULL;
  }
  if (end_phase == PH_DATA_IMPL) {
    return lps_old_spec;
  }
  
  //merge declarations from lps_newspec and action_rename
  gsVerboseMsg("merging declarations...\n");
  //lpsnewspec = merge_declarations(action_rename, lps_newspec);
  if (lps_new_spec == NULL) {
    return NULL;
  }
  if (end_phase == PH_MERGE) {
    return lps_old_spec;
  }

  //rename all assigned actions
  gsVerboseMsg("renaming actions...\n");
  ATermAppl result = rename(action_rename_spec, lps_old_spec, lps_new_spec);
  lps_new_spec = lps::specification(result);
  if (lps_old_spec == NULL) {
    return NULL;
  }
  if (end_phase == PH_RENAME) {
    return lps_new_spec;
  }
 
  //type check the new LPS
  gsVerboseMsg("type checking the new LPS...\n");
  if (!lps_new_spec.is_well_typed()) {
    gsVerboseMsg("The newly formed LPS is not well typed!\n");
    return NULL;
  }
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
    "  -fFILE, --file=FILE   use the state formula from FILE\n"
    "  -pPHASE, --end-phase=PHASE\n"
    "                        stop conversion after phase PHASE and output the\n"
    "                        result; PHASE can be 'pa' (parse), 'tc' (type check) or\n"
    "                        'di' (data implementation), 'me' (merge), 're' (rename)\n"
    "  -e, --external        return the result in the external format\n"
    "  -h, --help            display this help message and terminate\n"
    "      --version         display version information and terminate\n"
    "  -q, --quiet           do not display warning messages\n"
    "  -v, --verbose         display concise intermediate messages\n"
    "  -d, --debug           display detailed intermediate messages\n",
    name
  );
}

void print_version(void)
{
  fprintf(stderr,"%s %s (revision %s)\n", NAME, VERSION, REVISION);
}

void print_more_info(char *name)
{
  fprintf(stderr, "Use %s --help for options\n", name);
}
