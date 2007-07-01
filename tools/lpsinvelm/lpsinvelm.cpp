// Author(s): Luc Engelen
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsinvelm.cpp
/// \brief Add your file description here.

#define  NAME      "lpsinvelm"
#define  VERSION   "0.0.1"
#define  AUTHOR    "Luc Engelen"

#include "invariant_eliminator.h"
#include "invariant_checker.h"
#include "getopt.h"
#include "libstruct.h"
#include "libparse.h"
#include "typecheck.h"
#include "dataimpl.h"
#include "prover/bdd_path_eliminator.h"
#include <mcrl2/lps/linear_process.h>
#include <mcrl2/lps/specification.h>
#include "print/messaging.h"
#include <string>
#include <fstream>

using namespace ::mcrl2::utilities;

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

      /// \brief Prints the version of the tool.
      void print_version();
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
      fprintf(stderr,
        "Usage: %s [OPTION]... [INFILE [OUTFILE]] {--invariant=INVARIANT}\n"
        "This tool checks whether the invariant (a boolean expression in mCRL2 format)\n"
        " as found in INVARIANT holds for the mCRL2 LPS as found in INFILE.\n"
        "If the invariant holds, the tool eliminates all summands of the LPS whose\n"
        "condition violates the invariant.\n"
        "It can also be used to simplify the conditions of the summands of\n"
        "the given LPS. The resulting LPS is written to the file named OUTFILE.\n"
        "If INFILE is not specified, the LPS is read from stdin. If OUTFILE is not\n"
        "specified, the resulting LPS is written to stdout.\n"
        "\n"
        "Mandatory arguments to long options are mandatory for short options too.\n"
        "  -i, --invariant=INVARIANT       Use the formula (a boolean expression in mCRL2\n"
        "                                  format) as found in INVARIANT as invariant.\n"
        "  -s, --summand=NUMBER            Eliminate or simplify the summand with number\n"
        "                                  NUMBER only.\n"
        "  -n, --no-check                  Do not check if the invariant holds before\n"
        "                                  eliminating unreachable summands.\n"
        "  -e, --no-elimination            Do not eliminate or simplify summands.\n"
        "  -l, --simplify-all              Simplify the conditions of all summands,\n"
        "                                  instead of just eliminating the summands\n"
        "                                  whose conditions in conjunction with the\n"
        "                                  invariant are contradictions.\n"
        "  -y, --all-violations            Do not terminate as soon as a single\n"
        "                                  violation of the invariant is found, but\n"
        "                                  report all violations instead.\n"
        "  -c, --counter-example           Display a valuation indicating why the\n"
        "                                  invariant could possibly be violated if it is\n"
        "                                  uncertain whether a summand violates the\n"
        "                                  invariant.\n"
        "  -p, --print-dot=PREFIX          Save a .dot file of the resulting BDD if it\n"
        "                                  is impossible to determine whether a summand\n"
        "                                  violates the invariant. PREFIX will be used\n"
        "                                  as prefix of the output files.\n"
        "  -h, --help                      Display this help and terminate.\n"
        "      --version                   Display version information and terminate.\n"
        "  -q, --quiet                     Do not display warning messages.\n"
        "  -v, --verbose                   Display concise intermediate messages.\n"
        "  -d, --debug                     Display detailed intermediate messages.\n"
        "  -r, --rewrite-strategy=STRATEGY Use the specified STRATEGY as rewrite\n"
        "                                  strategy:\n"
        "                                  - 'inner' for the innermost rewrite strategy\n"
        "                                  - 'innerc' for the compiled innermost rewrite\n"
        "                                    strategy\n"
        "                                  - 'jitty' for the jitty rewrite strategy\n"
        "                                  - 'jittyc' for the compiled jitty rewrite\n"
        "                                    strategy.\n"
        "                                  By default, the jitty rewrite strategy is\n"
        "                                  used.\n"
        "  -t, --time-limit=SECONDS        Spend at most the specified number of SECONDS\n"
        "                                  on proving a single formula.\n"
        "  -z --smt-solver=SOLVER          Use the specified SOLVER to remove\n"
        "                                  inconsistent paths from BDDs:\n"
        "                                  - 'ario' for the SMT solver Ario\n"
        "                                  - 'cvc-lite' for the SMT solver CVC Lite.\n"
#ifdef CVC_LITE_LIB
        "                                  - 'cvc-lite-fast' for the fast implementation\n"
        "                                    of the SMT solver CVC Lite.\n"
#endif
        "                                  By default, no path elimination is applied.\n"
        " -o, --induction                  Apply induction on lists.\n",
        f_tool_command
      );
    }

    // --------------------------------------------------------------------------------------------

    void LPS_Inv_Elm::print_more_info() {
      fprintf(stderr, "Try \'%s --help\' for more information.\n", f_tool_command);
    }

    // --------------------------------------------------------------------------------------------

    void LPS_Inv_Elm::print_version() {
      fprintf(stderr,"%s %s (revision %s)\n", NAME, VERSION, REVISION);
    }

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
      char* v_short_options = "i:s:nelycp:hqvdr:t:z:o";

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
        {"rewrite-strategy", required_argument, 0, 'r'},
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
            print_version();
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
            } else if (strcmp(optarg, "cvc-lite") == 0) {
              f_path_eliminator = true;
              f_solver_type = solver_type_cvc_lite;
#ifdef CVC_LITE_LIB
            } else if (strcmp(optarg, "cvc-lite-fast") == 0) {
              f_path_eliminator = true;
              f_solver_type = solver_type_cvc_lite_fast;
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
          if (v_number_of_remaining_arguments == 2) {
            f_output_file_name = strdup(a_argv[optind + 1]);
          }
        }
      }
      if (f_invariant_file_name == 0) {
        gsErrorMsg("%s: a file containing an invariant must be specified using the option --invariant=INVARIANT.\n", NAME);
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
      //parse the invariant formula from infilename
      std::ifstream instream(f_invariant_file_name, std::ifstream::in|std::ifstream::binary);
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

      //f_invariant = (ATermAppl) read_ATerm_from_file(f_invariant_file_name, "invariant");
      f_lps = (ATermAppl) read_ATerm_from_file(f_lps_file_name, "LPS");

      gsEnableConstructorFunctions();

      //if ((ATgetType(f_lps) != AT_APPL) || !gsIsSpecV1(f_lps)) {
      //  gsErrorMsg("The file '%s' does not contain an mCRL2 LPS.\n", f_lps_file_name);
      //  exit(1);
      //}

      lps::specification lps_specification;
      try
      {
        lps_specification.load(f_lps_file_name);
      }
      catch (std::runtime_error e)
      {
        gsErrorMsg("The file '%s' does not contain an mCRL2 LPS.\n", f_lps_file_name);
        exit(1);
      }


      //typecheck the invariant formula
      f_invariant = type_check_data_expr(f_invariant, gsMakeSortIdBool(), lps_specification, true);
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
        write_ATerm_to_file(f_output_file_name, f_lps, "resulting LPS");
      }
    }

// Main function ----------------------------------------------------------------------------------

  int main(int argc, char* argv[]) {
    ATerm v_bottom_of_stack;
    ATinit(argc, argv, &v_bottom_of_stack);

    LPS_Inv_Elm v_lps_inv_elm;

    v_lps_inv_elm.get_options(argc, argv);
    v_lps_inv_elm.read_input();
    if (v_lps_inv_elm.check_invariant()) {
      v_lps_inv_elm.simplify();
      v_lps_inv_elm.write_result();
    }
    return 0;
  }
