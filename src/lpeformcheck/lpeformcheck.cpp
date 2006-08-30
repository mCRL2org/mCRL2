#define  NAME      "lpeformcheck"
#define  VERSION   "0.0.1"
#define  AUTHOR    "Luc Engelen"

#include "formula_checker.h"
#include "getopt.h"
#include "libprint_c.h"
#include "liblowlevel.h"
#include "libstruct.h"
#include "mcrl2_revision.h"
#include "bdd_path_eliminator.h"
#include <string>

  /// \mainpage lpeformcheck
  /// \section section_introduction Introduction
  /// This document provides information on the internals of the tool.
  /// \section section_additional_info Additional information
  /// More information about the tool and the classes used can be found in the corresponding man files.

// Class LPE_Form_Check ---------------------------------------------------------------------------

  /// \brief The class LPE_Form_Check uses an instance of the class Formula_Checker to check whether
  /// \brief or not the formulas in the list specified by LPE_Form_Check::f_formulas_file_name are
  /// \brief tautologies or contradictions.

  class LPE_Form_Check {
    private:
      /// \brief The command entered to invoke the tool lpeformcheck.
      char* f_tool_command;

      /// \brief The name of the file containing the list of formulas that is checked.
      char* f_formulas_file_name;

      /// \brief The name of the file containing the LPE.
      /// \brief If this string is 0, the input is read from stdin.
      char* f_lpe_file_name;

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

      /// \brief Prints the version of the tool.
      void print_version();
    public:
      /// \brief Constructor setting all flags to their default values.
      LPE_Form_Check();

      /// \brief Destructor with no particular functionality.
      ~LPE_Form_Check();

      /// \brief Uses the library getopt to determine which command line options are used.
      void get_options(int a_argc, char* a_argv[]);

      /// \brief Checks and indicates whether or not the formulas in the list specified by
      /// \brief LPE_Form_Check::f_formulas_file_name are tautologies or contradictions.
      void check_formulas();
  };

  // Class LPE_Form_Check - Functions declared private --------------------------------------------

    void LPE_Form_Check::print_help() {
      fprintf(stderr,
        "Usage: %s [OPTION]... [INFILE] {--formulas=FORMULAS}\n"
        "All formulas in the list of formulas in internal mCRL2 format as found in\n"
        "FORMULAS are checked using the data specification of the mCRL2 LPE as found in\n"
        "INFILE. The tool indicates whether each formula is a tautology or a\n"
        "contradiction. If the tool is unable to determine whether a formula is a\n"
        "tautology or a contradiction, it indicates this fact.\n"
        "If INFILE is not specified, the LPE is read from stdin.\n"
        "\n"
        "Mandatory arguments to long options are mandatory for short options too.\n"
        "  -f, --formulas=FORMULAS         Use the list of formulas in internal mCRL2\n"
        "                                  format as found in FORMULAS as input.\n"
        "  -c, --counter-example           Give a valuation for which the current\n"
        "                                  formula does not hold, in case the current\n"
        "                                  formula is neither a contradiction nor a\n"
        "                                  tautology.\n"
        "  -w, --witness                   Give a valuation for which the current\n"
        "                                  formula holds, in case the current formula\n"
        "                                  is neither a contradiction nor a tautology.\n"
        "  -p, --print-dot=PREFIX          Save a .dot file of the resulting BDD if it\n"
        "                                  is impossible to determine whether a formula\n"
        "                                  is a contradiction or a tautology. PREFIX\n"
        "                                  will be used as prefix of the output files.\n"
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

    void LPE_Form_Check::print_more_info() {
      fprintf(stderr, "Try \'%s --help\' for more information.\n", f_tool_command);
    }

    // --------------------------------------------------------------------------------------------

    void LPE_Form_Check::print_version() {
      fprintf(stderr,"%s %s (revision %d)\n", NAME, VERSION, REVISION);
    }

  // Class LPE_Form_Check - Functions declared public ---------------------------------------------

    LPE_Form_Check::LPE_Form_Check() {
      f_tool_command = 0;
      f_formulas_file_name = 0;
      f_lpe_file_name = 0;
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

    LPE_Form_Check::~LPE_Form_Check() {
      // Nothing to free.
    }

    // --------------------------------------------------------------------------------------------

    /// Sets the flags of the class according to the command line options passed.
    /// \param a_argc is the number of arguments passed on the command line
    /// \param a_argv is an array of all arguments passed on the command line

    void LPE_Form_Check::get_options(int a_argc, char* a_argv[]) {
      char* v_short_options = "f:cwp:hqvdr:t:z:o";

      struct option v_long_options[] = {
        {"formulas",         required_argument, 0, 'f'},
        {"counter-example",  no_argument,       0, 'c'},
        {"witness",          no_argument,       0, 'w'},
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

      f_tool_command = a_argv[0];

      int v_option = getopt_long(a_argc, a_argv, v_short_options, v_long_options, NULL);
      while (v_option != -1) {
        switch (v_option) {
          case 'f':
            f_formulas_file_name = strdup(optarg);
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
      if (v_number_of_remaining_arguments > 1) {
        gsErrorMsg("%s: too many arguments\n", NAME);
        print_more_info();
        exit(1);
      } else {
        if (v_number_of_remaining_arguments == 1) {
          f_lpe_file_name = strdup(a_argv[optind]);
        }
      }
      if (f_formulas_file_name == 0) {
        gsErrorMsg("%s: a file containing a list of formulas must be specified using the option --formulas=FORMULAS.\n", NAME);
        print_more_info();
        exit(1);
      }
    }

    // --------------------------------------------------------------------------------------------

    /// Reads the list of formulas specified by LPE_Form_Check::f_formulas_file_name and the LPE
    /// specified by LPE_Form_Check::f_file_name. The method determines and indicates whether or not the
    /// formulas in the list are tautologies or contradictions using the data equations of the LPE.

    void LPE_Form_Check::check_formulas() {
      ATermList v_formulas = (ATermList) read_ATerm_from_file(f_formulas_file_name, "formulas");
      ATermAppl v_lpe = (ATermAppl) read_ATerm_from_file(f_lpe_file_name, "LPE");

      gsEnableConstructorFunctions();
      if ((ATgetType(v_lpe) != AT_APPL) || !gsIsSpecV1(v_lpe)) {
        gsErrorMsg("The file '%s' does not contain an mCRL2 LPE.\n", f_lpe_file_name);
        exit(1);
      } else {
        Formula_Checker v_formula_checker(
          v_lpe, f_strategy, f_time_limit, f_path_eliminator, f_solver_type, f_apply_induction, f_counter_example, f_witness, f_dot_file_name
        );

        v_formula_checker.check_formulas(v_formulas);
      }
    }

// Main function ----------------------------------------------------------------------------------

  int main(int argc, char* argv[]) {
    ATerm v_bottom_of_stack;
    ATinit(argc, argv, &v_bottom_of_stack);

    LPE_Form_Check v_lpe_form_check;

    v_lpe_form_check.get_options(argc, argv);
    v_lpe_form_check.check_formulas();
    return 0;
  }
