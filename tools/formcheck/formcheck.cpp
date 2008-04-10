// Author(s): Luc Engelen
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file formcheck.cpp
/// \brief Add your file description here.

#define NAME "formcheck"
#define AUTHOR "Luc Engelen"

#include "mcrl2/formula_checker.h"
#include "getopt.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/data/prover/bdd_path_eliminator.h"
#include "mcrl2/core/struct.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/detail/parse.h"
#include "mcrl2/core/detail/typecheck.h"
#include "mcrl2/core/detail/data_implementation.h"
#include "mcrl2/core/detail/data_reconstruct.h"
#include "mcrl2/utilities/aterm_ext.h"
#include "mcrl2/utilities/version_info.h"
#include <string>
#include <fstream>

using namespace ::mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2;

  /// \mainpage formcheck
  /// \section section_introduction Introduction
  /// This document provides information on the internals of the tool.
  /// \section section_additional_info Additional information
  /// More information about the tool and the classes used can be found in the corresponding tool manual page.

// Class LPS_Form_Check ---------------------------------------------------------------------------

  /// \brief The class LPS_Form_Check uses an instance of the class Formula_Checker to check whether
  /// \brief or not the formula specified by LPS_Form_Check::f_formula_file_name is a tautology or a contradiction.

  class LPS_Form_Check {
    private:
      /// \brief The command entered to invoke the tool formcheck.
      char* f_tool_command;

      /// \brief The name of the file containing the formula that is checked.
      char* f_formula_file_name;

      /// \brief The name of the file containing the LPS.
      /// \brief If this string is 0, the input is read from stdin.
      char* f_lps_file_name;

      /// \brief The flag indicating whether or not counter examples are printed each time a formula is encountered
      /// \brief that is neither a contradiction nor a tautology.
      bool f_counter_example;

      /// \brief The flag indicating whether or not witnesses are printed each time a formula is encountered
      /// \brief that is neither a contradiction nor a tautology.
      bool f_witness;

      /// \brief The prefix of the files in dot format that are written each time a condition is encountered that is neither
      /// \brief a contradiction nor a tautology. If the string is 0, no files are written.
      char* f_dot_file_name;

      /// \brief The rewrite strategy used by the rewriter.
      RewriteStrategy f_strategy;

      /// \brief The maximal number of seconds spent on proving a single confluence condition.
      int f_time_limit;

      /// \brief The flag indicating whether or not a path eliminator is used.
      bool f_path_eliminator;

      /// \brief The type of SMT solver used by the path eliminator.
      SMT_Solver_Type f_solver_type;

      /// \brief The flag indicating whether or not induction should be applied.
      bool f_apply_induction;

      /// \brief Prints the help message.
      void print_help();

      /// \brief Prints a message indicating how to display the help message.
      void print_more_info();

    public:
      /// \brief Constructor setting all flags to their default values.
      LPS_Form_Check();

      /// \brief Destructor with no particular functionality.
      ~LPS_Form_Check();

      /// \brief Uses the library getopt to determine which command line options are used.
      void get_options(int a_argc, char* a_argv[]);

      /// \brief Checks and indicates whether or not the formula specified by
      /// \brief LPS_Form_Check::f_formula_file_name is a tautology or a contradiction.
      void check_formula();
  };

  // Class LPS_Form_Check - Functions declared private --------------------------------------------

    void LPS_Form_Check::print_help() {
      fprintf(stdout,
        "Usage: %s [OPTION]... -fFORMFILE [INFILE]\n"
        "Checks whether the boolean formula (an mCRL2 data expression of sort Bool) in\n"
        "FORMFILE holds for the data specification of the linear process specification\n"
        "(LPS) in INFILE. If INFILE is not present, stdin is used.\n"
        "\n"
        "Options:\n"
        "  -fFORMFILE, --formula=FORMFILE  use the formula in FORMFILE as input\n"
        "  -c, --counter-example           display a valuation for which the formula does\n"
        "                                  not hold, in case it is neither a\n"
        "                                  contradiction nor a tautology\n"
        "  -w, --witness                   display a valuation for which the formula\n"
        "                                  holds, in case it is neither a contradiction\n"
        "                                  nor a tautology\n"
        "  -pPREFIX, --print-dot=PREFIX    save a .dot file of the resulting BDD if it\n"
        "                                  is impossible to determine whether the formula\n"
        "                                  is a contradiction or a tautology; PREFIX\n"
        "                                  will be used as prefix of the output files\n"
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
        "  -tLIMIT, --time-limit=LIMIT     spend at most LIMIT seconds on proving the\n"
        "                                  formula\n"
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

    void LPS_Form_Check::print_more_info() {
      fprintf(stderr, "Try `%s --help' for more information.\n", f_tool_command);
    }

    // --------------------------------------------------------------------------------------------

  // Class LPS_Form_Check - Functions declared public ---------------------------------------------

    LPS_Form_Check::LPS_Form_Check() {
      f_tool_command = 0;
      f_formula_file_name = 0;
      f_lps_file_name = 0;
      f_counter_example = false;
      f_witness = false;
      f_dot_file_name = 0;
      f_strategy = GS_REWR_JITTY;
      f_time_limit = 0;
      f_path_eliminator = false;
      f_solver_type = solver_type_ario;
      f_apply_induction = false;
    }

    // --------------------------------------------------------------------------------------------

    LPS_Form_Check::~LPS_Form_Check() {
      free(f_formula_file_name);
      f_formula_file_name = 0;
      free(f_lps_file_name);
      f_lps_file_name = 0;
      free(f_dot_file_name);
      f_dot_file_name = 0;
    }

    // --------------------------------------------------------------------------------------------

    /// Sets the flags of the class according to the command line options passed.
    /// \param a_argc is the number of arguments passed on the command line
    /// \param a_argv is an array of all arguments passed on the command line

    void LPS_Form_Check::get_options(int a_argc, char* a_argv[]) {
      const char* v_short_options = "f:cwp:hqvdr:t:z:o";

      struct option v_long_options[] = {
        {"formula",          required_argument, 0, 'f'},
        {"counter-example",  no_argument,       0, 'c'},
        {"witness",          no_argument,       0, 'w'},
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

      f_tool_command = a_argv[0];

      int v_option = getopt_long(a_argc, a_argv, v_short_options, v_long_options, NULL);
      while (v_option != -1) {
        switch (v_option) {
          case 'f':
            f_formula_file_name = strdup(optarg);
            break;
          case 'c':
            f_counter_example = true;
            break;
          case 'w':
            f_witness = true;
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
              throw std::runtime_error("option -r has illegal argument '" + std::string(optarg) + "'");
            }
            break;
          case 't':
            sscanf(optarg, "%d", &f_time_limit);
            if (f_time_limit <= 0) {
              throw std::runtime_error("the time-limit must be greater than or equal to one");
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
              throw std::runtime_error("option -z has illegal argument '" + std::string(optarg) + "'");
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
      if (v_number_of_remaining_arguments > 1) {
        fprintf(stderr, "%s: too many arguments\n", NAME);
        print_more_info();
        exit(1);
      } else {
        if (v_number_of_remaining_arguments == 1) {
          f_lps_file_name = strdup(a_argv[optind]);
        }
      }
      if (f_formula_file_name == 0) {
        fprintf(stderr, "%s: a formula file must be specified using the option --formula=FORMFILE.\n", NAME);
        print_more_info();
        exit(1);
      }
    }

    // --------------------------------------------------------------------------------------------

    /// Reads the list of formulas specified by LPS_Form_Check::f_formulas_file_name and the LPS
    /// specified by LPS_Form_Check::f_file_name. The method determines and indicates whether or not the
    /// formulas in the list are tautologies or contradictions using the data equations of the LPS.

    void LPS_Form_Check::check_formula() {

      //Load LPS
      lps::specification lps_specification;
      lps_specification.load((f_lps_file_name == 0)?"-":f_lps_file_name);
      if (!lps_specification.is_well_typed()) {
        throw std::runtime_error("invalid mCRL2 LPS read from " + std::string((f_lps_file_name == 0)?"stdin":f_lps_file_name));
      }

      // typechecking and data implementation use a specification before data
      // implementation.
      ATermAppl v_reconstructed_lps = reconstruct_spec(lps_specification);

      gsVerboseMsg("parsing formula file '%s'...\n", f_formula_file_name);
      //open the formula from f_formula_file_name
      std::ifstream instream(f_formula_file_name);
      if (!instream.is_open()) {
        throw std::runtime_error("cannot open formula file '" + std::string(f_formula_file_name) + "'");
      }
      //parse the formula
      ATermAppl f_formula = parse_data_expr(instream);
      instream.close();
      if(!f_formula){
        throw std::runtime_error("cannot parse formula from '" + std::string(f_formula_file_name) + "'");
      }
      //typecheck the formula
      f_formula = type_check_data_expr(f_formula, gsMakeSortIdBool(), v_reconstructed_lps);
      if(!f_formula){
        throw std::runtime_error("type checking formula from '" + std::string(f_formula_file_name) + "' failed");
      }
      //implement data in the formula
      f_formula = implement_data_data_expr(f_formula,v_reconstructed_lps);
      if(!f_formula){
        throw std::runtime_error("implementation of data types in the formula from '" + std::string(f_formula_file_name) + "' failed");
      }

      //check formula
      Formula_Checker v_formula_checker(
        ATAgetArgument(v_reconstructed_lps,0), f_strategy, f_time_limit, f_path_eliminator, f_solver_type, f_apply_induction, f_counter_example, f_witness, f_dot_file_name);
      v_formula_checker.check_formulas(ATmakeList1((ATerm) f_formula));
    }

// Main function ----------------------------------------------------------------------------------

  int main(int argc, char* argv[])
  {
    MCRL2_ATERM_INIT(argc, argv)

    try {
      LPS_Form_Check v_lps_form_check;

      v_lps_form_check.get_options(argc, argv);
      v_lps_form_check.check_formula();
    }
    catch (std::exception& e) {
      gsErrorMsg("%s\n", e.what());
      exit(1);
    }

    return 0;
  }
