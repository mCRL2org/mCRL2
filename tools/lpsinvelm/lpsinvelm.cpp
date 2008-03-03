// Author(s): Luc Engelen
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsinvelm.cpp
/// \brief Add your file description here.

#define NAME "lpsinvelm"
#define AUTHOR "Luc Engelen"

#include "mcrl2/invariant_eliminator.h"
#include "mcrl2/invariant_checker.h"
#include "getopt.h"
#include "mcrl2/core/struct.h"
#include "mcrl2/core/detail/parse.h"
#include "mcrl2/core/detail/typecheck.h"
#include "mcrl2/core/detail/data_implementation.h"
#include "mcrl2/data/prover/bdd_path_eliminator.h"
#include "mcrl2/lps/linear_process.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/utilities/version_info.h"
#include <string>
#include <fstream>
#include "mcrl2/utilities/aterm_ext.h"

using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2;

  /// \mainpage lpsinvelm
  /// \section section_introduction Introduction
  /// This document provides information on the internals of the tool.
  /// \section section_additional_info Additional information
  /// More information about the tool and the classes used can be found in the corresponding man files.

// Class LPS_Inv_Elm --------------------------------------------------------------------------

  /// \brief The class LPS_Inv_Elm takes an invariant and an LPS, and simplifies this LPS using
  /// \brief the invariant.

  class LPS_Inv_Elm {
    private:
      /// \brief The command entered to invoke the tool lpsinvelm.
      char* f_tool_command;

      /// \brief The name of the file containing the invariant.
      char* f_invariant_file_name;

      /// \brief The name of the file containing the LPS.
      /// \brief If this string is 0, the input is read from stdin.
      char* f_lps_file_name;

      /// \brief The name of the file the LPS is written to.
      /// \brief If this string is 0, the output is written to stdout.
      char* f_output_file_name;

      /// \brief The number of the summand that is eliminated or simplified. If this number is 0,
      /// \brief all summands will be simplified or eliminated.
      int f_summand_number;

      /// \brief The flag indicating whether or not the invariance of the formula as found in
      /// \brief LPS_Inv_Elm::f_invariant_file_name is checked.
      bool f_no_check;

      /// \brief The flag indicating whether or not elimination or simplification is applied.
      bool f_no_elimination;

      /// \brief The flag indicating whether or not the conditions of all summands will be simplified.
      /// \brief If this flag is set to false, only the summands whose conditions violate the invariant
      /// \brief are eliminated.
      bool f_simplify_all;

      /// \brief The flag indicating whether or not all violations encountered while checking the invariant
      /// \brief are reported. If this flag is set to false, the checking stops as soon as a violation is
      /// \brief encountered.
      bool f_all_violations;

      /// \brief The flag indicating whether or not counter examples are printed each time a summand is
      /// \brief encountered whose condition in conjunction with the invariant is not a tautology or a
      /// \brief contradiction.
      bool f_counter_example;

      /// \brief The prefix of the files in dot format that are written each time a summand is
      /// \brief encountered whose condition in conjunction with the invariant is not a tautology or a
      /// \brief contradiction.
      char* f_dot_file_name;

      /// \brief The flag indicating whether or not a path eliminator is used.
      bool f_path_eliminator;

      /// \brief The type of SMT solver used by the path eliminator.
      SMT_Solver_Type f_solver_type;

      /// \brief The rewrite strategy used by the rewriter.
      RewriteStrategy f_strategy;

      /// \brief The flag indicating whether or not induction should be applied.
      bool f_apply_induction;

      /// \brief The maximal number of seconds spent on proving the conjunction of the invariant
      /// \brief and a summands' condition
      int f_time_limit;

      /// \brief The LPS provided as input.
      ATermAppl f_lps;

      /// \brief The invariant provided as input.
      ATermAppl f_invariant;

      /// \brief Prints the help message.
      void print_help();

      /// \brief Prints a message indicating how to display the help message.
      void print_more_info();

    public:
      /// \brief Constructor setting all flags to their default values.
      LPS_Inv_Elm();

      /// \brief Destructor with no particular functionality.
      ~LPS_Inv_Elm();

      /// \brief Uses the library getopt to determine which command line options are used.
      void get_options(int argc, char* argv[]);

      /// \brief Reads an LPS and an invariant from the specified input sources.
      void read_input();

      /// \brief Checks whether or not the invariant holds, if
      /// \brief LPS_Inv_Elm::f_invariant_file_name differs from 0 and
      /// \brief LPS_Inv_Elm::f_no_check is set to false.
      bool check_invariant();

      /// \brief Simplifies or eliminates summands of the LPS, if the flag
      /// \brief LPS_Inv_Elm::f_no_elimination is set to false.
      void simplify();

      /// \brief Writes the resulting LPS to the preferred output.
      void write_result();
   };

  // Class LPS_Inv_Elm - Functions declared private --------------------------------------------

    void LPS_Inv_Elm::print_help() {
      fprintf(stdout,
        "Usage: %s [OPTION]... -iINVFILE [INFILE [OUTFILE]]\n"
        "Checks whether the boolean formula (an mCRL2 data expression of sort Bool) in\n"
        "INVFILE is an invariant of the linear process specification (LPS) in INFILE.\n"
        "If this is the case, the tool eliminates all summands of the LPS whose condition\n"
        "violates the invariant, and writes the result to OUTFILE.\n"
        "If INFILE is present, stdin is used. If OUTFILE is not present, stdout is used.\n"
        "\n"
        "The tool can also be used to simplify the conditions of the summands of the\n"
        "given LPS.\n"
        "\n"
        "Options:\n"
        "  -iINVFILE, --invariant=INVFILE  use the invariant in INVFILE as input\n"
        "  -sNUM, --summand=NUM            eliminate or simplify the summand with number\n"
        "                                  NUM only\n"
        "  -n, --no-check                  do not check if the invariant holds before\n"
        "                                  eliminating unreachable summands\n"
        "  -e, --no-elimination            do not eliminate or simplify summands\n"
        "  -l, --simplify-all              simplify the conditions of all summands,\n"
        "                                  instead of just eliminating the summands\n"
        "                                  whose conditions in conjunction with the\n"
        "                                  invariant are contradictions\n"
        "  -y, --all-violations            do not terminate as soon as a single\n"
        "                                  violation of the invariant is found, but\n"
        "                                  report all violations instead\n"
        "  -c, --counter-example           display a valuation indicating why the\n"
        "                                  invariant could possibly be violated if it is\n"
        "                                  uncertain whether a summand violates the\n"
        "                                  invariant\n"
        "  -pPREFIX, --print-dot=PREFIX    save a .dot file of the resulting BDD if it\n"
        "                                  is impossible to determine whether a summand\n"
        "                                  violates the invariant; PREFIX will be used\n"
        "                                  as prefix of the output files\n"
        "  -h, --help                      display this help and terminate\n"
        "      --version                   display version information and terminate\n"
        "  -q, --quiet                     do not display warning messages\n"
        "  -v, --verbose                   display concise intermediate messages\n"
        "  -d, --debug                     display detailed intermediate messages\n"
        "  -rNAME, --rewriter=NAME         use rewrite strategy NAME:\n"
        "                                  'inner' for the innermost rewriter,\n"
        "                                  'innerc' for the compiled innermost rewriter,\n"
        "                                  'jitty' for the jitty rewriter (default), or\n"
        "                                  'jittyc' for the compiled jitty rewriter\n"
        "  -tLIMIT, --time-limit=LIMIT     spend at most LIMIT seconds on proving a\n"
        "                                  single formula\n"
        "  -zSOLVER, --smt-solver=SOLVER   use SOLVER to remove inconsistent paths from\n"
        "                                  the internally used BDDs:\n"
#ifdef HAVE_CVC
        "                                  'ario' for the SMT solver Ario,\n"
        "                                  'cvc' for the SMT solver CVC3, or\n"
        "                                  'cvc-fast' for the fast implementation of the\n"
        "                                  SMT solver CVC3;\n"
#else
        "                                  'ario' for the SMT solver Ario, or\n"
        "                                  'cvc' for the SMT solver CVC3;\n"
#endif
        "                                  by default, no path elimination is applied\n"
        "  -o, --induction                 apply induction on lists\n"
        "\n"
        "Report bugs at <http://www.mcrl2.org/issuetracker>.\n"
        , f_tool_command
      );
    }

    // --------------------------------------------------------------------------------------------

    void LPS_Inv_Elm::print_more_info() {
      fprintf(stderr, "Try \'%s --help\' for more information.\n", f_tool_command);
    }

    // --------------------------------------------------------------------------------------------

  // Class LPS_Inv_Elm - Functions declared public ----------------------------------------------

    LPS_Inv_Elm::LPS_Inv_Elm() {
      f_tool_command = 0;
      f_invariant_file_name = 0;
      f_lps_file_name = 0;
      f_output_file_name = 0;
      f_summand_number = 0;
      f_no_check = false;
      f_no_elimination = false;
      f_simplify_all = false;
      f_all_violations = false;
      f_counter_example = false;
      f_dot_file_name = 0;
      f_strategy = GS_REWR_JITTY;
      f_time_limit = 0;
      f_path_eliminator = false;
      f_solver_type = solver_type_ario;
      f_apply_induction = false;
    }

    // --------------------------------------------------------------------------------------------

    LPS_Inv_Elm::~LPS_Inv_Elm() {
      free(f_invariant_file_name);
      f_invariant_file_name = 0;
      free(f_lps_file_name);
      f_lps_file_name = 0;
      free(f_output_file_name);
      f_output_file_name = 0;
      free(f_dot_file_name);
      f_dot_file_name = 0;
    }

    // --------------------------------------------------------------------------------------------

    /// Sets the flags of the class according to the command line options passed.
    /// \param a_argc is the number of arguments passed on the command line
    /// \param a_argv is an array of all arguments passed on the command line

    void LPS_Inv_Elm::get_options(int a_argc, char* a_argv[]) {
      const char* v_short_options = "i:s:nelycp:hqvdr:t:z:o";

      f_tool_command = a_argv[0];

      struct option v_long_options[] = {
        {"invariant",        required_argument, 0, 'i'},
        {"summand",          required_argument, 0, 's'},
        {"no-check",         no_argument,       0, 'n'},
        {"no-elimination",   no_argument,       0, 'e'},
        {"simplify-all",     no_argument,       0, 'l'},
        {"all-violation",    no_argument,       0, 'y'},
        {"counter-example",  no_argument,       0, 'c'},
        {"print-dot",        required_argument, 0, 'p'},
        {"help",             no_argument,       0, 'h'},
        {"version",          no_argument,       0, 0x1},
        {"quiet",            no_argument,       0, 'q'},
        {"verbose",          no_argument,       0, 'v'},
        {"debug",            no_argument,       0, 'd'},
        {"rewriter",         required_argument, 0, 'r'},
        {"time-limit",       required_argument, 0, 't'},
        {"smt-solver",       required_argument, 0, 'z'},
        {"induction",        no_argument,       0, 'o'},
        {0, 0, 0, 0}
      };

      int v_option = getopt_long(a_argc, a_argv, v_short_options, v_long_options, NULL);
      while (v_option != -1) {
        switch (v_option) {
          case 'i':
            f_invariant_file_name = strdup(optarg);
            break;
          case 's':
            sscanf(optarg, "%d", &f_summand_number);
            if (f_summand_number < 1) {
              gsErrorMsg("The summand number must be greater than or equal to 1.\n");
              exit(1);
            } else {
              gsVerboseMsg("Eliminating or simplifying summand number %d.\n", f_summand_number);
            }
            break;
          case 'n':
            f_no_check = true;
            break;
          case 'e':
            f_no_elimination = true;
            break;
          case 'l':
            f_simplify_all = true;
            break;
          case 'y':
            f_all_violations = true;
            break;
          case 'c':
            f_counter_example = true;
            break;
          case 'p':
            f_dot_file_name = strdup(optarg);
            break;
          case 'h':
            print_help();
            exit(0);
          case 0x1:
            print_version_information(NAME, AUTHOR);
            exit(0);
          case 'q':
            gsSetQuietMsg();
            break;
          case 'v':
            gsSetVerboseMsg();
            break;
          case 'd':
            gsSetDebugMsg();
            break;
          case 'r':
            if (strcmp(optarg, "jitty") == 0) {
              f_strategy = GS_REWR_JITTY;
            } else if (strcmp(optarg, "inner") == 0) {
              f_strategy = GS_REWR_INNER;
            } else if (strcmp(optarg, "jittyc") == 0) {
              f_strategy = GS_REWR_JITTYC;
            } else if (strcmp(optarg, "innerc") == 0) {
              f_strategy = GS_REWR_INNERC;
            } else {
              gsErrorMsg("option -s has illegal argument '%s'\n", optarg);
              exit(1);
            }
            break;
          case 't':
            sscanf(optarg, "%d", &f_time_limit);
            if (f_time_limit <= 0) {
              gsErrorMsg("The time-limit must be greater than or equal to one.\n");
              exit(0);
            }
            break;
          case 'z':
            if (strcmp(optarg, "ario") == 0) {
              f_path_eliminator = true;
              f_solver_type = solver_type_ario;
            } else if (strcmp(optarg, "cvc") == 0) {
              f_path_eliminator = true;
              f_solver_type = solver_type_cvc;
#ifdef CVC_LITE_LIB
            } else if (strcmp(optarg, "cvc-fast") == 0) {
              f_path_eliminator = true;
              f_solver_type = solver_type_cvc_fast;
#endif
            } else {
              gsErrorMsg("option -z has illegal argument '%s'\n", optarg);
              exit(1);
            }
            break;
          case 'o':
            f_apply_induction = true;
            break;
          default:
            print_more_info();
            exit(1);
        }
        v_option = getopt_long(a_argc, a_argv, v_short_options, v_long_options, NULL);
      }

      int v_number_of_remaining_arguments = a_argc - optind;
      if (v_number_of_remaining_arguments > 2) {
        gsErrorMsg("%s: too many arguments\n", NAME);
        print_more_info();
        exit(1);
      } else {
        if (v_number_of_remaining_arguments > 0) {
          f_lps_file_name = strdup(a_argv[optind]);
        }
        if (v_number_of_remaining_arguments == 2) {
          f_output_file_name = strdup(a_argv[optind + 1]);
        }
      }
      if (f_invariant_file_name == 0) {
        gsErrorMsg("%s: a file containing an invariant must be specified using the option --invariant=INVFILE\n", NAME);
        print_more_info();
        exit(1);
      }
    }

    // --------------------------------------------------------------------------------------------

    /// Reads the invariant specified by LPS_Inv_Elm::f_invariant_file_name and saves it as
    /// LPS_Inv_Elm::f_invariant.
    /// Reads the LPS specified by LPS_Inv_Elm::f_lps_file_name and saves it as
    /// LPS_Inv_Elm::f_lps. If no input file name was specified, the LPS is read from stdin.

    void LPS_Inv_Elm::read_input() {

      //read the LPS
      lps::specification lps_specification;
      try {
        lps_specification.load((f_lps_file_name == 0)?"-":f_lps_file_name);
      } catch (std::exception) {
        gsErrorMsg("could not read LPS from '%s'\n", (f_lps_file_name == 0)?"stdin":f_lps_file_name);
        exit(1);
      }

      if (!lps_specification.is_well_typed()) {
        gsErrorMsg("Invalid mCRL2 LPS read from %s.\n", (f_lps_file_name == 0)?"stdin":f_lps_file_name);
        exit(1);
      }

      // temporary measure until the invariant and confluence checkers use the lps framework
      f_lps = (ATermAppl) lps_specification;

      //parse the invariant formula from infilename
      std::ifstream instream(f_invariant_file_name);
      if (!instream.is_open()) {
        gsErrorMsg("cannot open input file '%s'\n", f_invariant_file_name);
        exit(1);
      }
      gsVerboseMsg("parsing input file '%s'...\n", f_invariant_file_name);
      f_invariant = parse_data_expr(instream);
      instream.close();
      if(!f_invariant){
        exit(1);
      }
      
      //typecheck the invariant formula
      ATermList vars = lps_specification.process().process_parameters();
      ATermTable var_table = ATtableCreate(63,50);
      for (; !ATisEmpty(vars); vars = ATgetNext(vars)) {
        ATermAppl var = ATAgetFirst(vars);
        ATtablePut(var_table, ATgetArgument(var, 0), ATgetArgument(var, 1));
      } 
      f_invariant = type_check_data_expr(f_invariant, gsMakeSortIdBool(), lps_specification, var_table);
      ATtableDestroy(var_table);
      if(!f_invariant){
        gsErrorMsg("Typechecking of the invariant formula failed.\n");
        exit(1);
      }
     
      //data implement the invariant formula
      f_invariant = implement_data_data_expr(f_invariant,lps_specification);
      if(!f_invariant){
        gsErrorMsg("Data implementation of the invariant formula failed.\n");
        exit(1);
      }
    }

    // --------------------------------------------------------------------------------------------

    /// Checks if the formula in the file LPS_Inv_Elm::f_invariant_file_name is an invariant,
    /// if the flag LPS_Inv_Elm::f_no_check is set to false.
    /// \return true, if the invariant holds or is not checked.
    ///         false, if the invariant does not hold.

    bool LPS_Inv_Elm::check_invariant() {
      if (!f_no_check) {
        Invariant_Checker v_invariant_checker(
          f_lps, f_strategy, f_time_limit, f_path_eliminator, f_solver_type, f_apply_induction, f_counter_example, f_all_violations, f_dot_file_name
        );

        return v_invariant_checker.check_invariant(f_invariant);
      } else {
        gsWarningMsg("The invariant is not checked; it may not hold for this LPS.\n");
        return true;
      }
    }

    // --------------------------------------------------------------------------------------------

    /// Removes the summands from the LPS LPS_Inv_Elm::f_lps whose conditions violate the invariant
    /// LPS_Inv_Elm::f_invariant, if the flag LPS_Inv_Elm::f_no_elimination is set to false.
    /// If the flag LPS_Inv_Elm::f_simplify_all is set
    /// to true, the condition of each summand is replaced by the BDD of the
    /// condition in conjunction with the invariant as well.

    void LPS_Inv_Elm::simplify() {
      if (!f_no_elimination) {
        Invariant_Eliminator v_invariant_eliminator(
          f_lps, f_strategy, f_time_limit, f_path_eliminator, f_solver_type, f_apply_induction, f_simplify_all
        );

        f_lps = v_invariant_eliminator.simplify(f_invariant, f_summand_number);
      }
    }

    // --------------------------------------------------------------------------------------------

    /// Writes the LPS LPS_Inv_Elm::f_lps to the file specified by
    /// LPS_Inv_Elm::f_output_file_name.

    void LPS_Inv_Elm::write_result() {
      if (!f_no_elimination) {
        ATwriteToNamedSAFFile((ATerm) f_lps, (f_output_file_name == 0)?"-":f_output_file_name);
      }
    }

// Main function ----------------------------------------------------------------------------------

  int main(int argc, char* argv[])
  {
    MCRL2_ATERM_INIT(argc, argv)

    LPS_Inv_Elm v_lps_inv_elm;

    v_lps_inv_elm.get_options(argc, argv);
    v_lps_inv_elm.read_input();
    if (v_lps_inv_elm.check_invariant()) {
      v_lps_inv_elm.simplify();
      v_lps_inv_elm.write_result();
    }
    return 0;
  }
