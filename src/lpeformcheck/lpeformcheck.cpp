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

// Class LPE_Form_Check ---------------------------------------------------------------------------

  class LPE_Form_Check {
    private:
      char* f_tool_command;
      char* f_formulas_file_name;
      char* f_lpe_file_name;
      bool f_counter_example;
      bool f_witness;
      RewriteStrategy f_strategy;
      int f_time_limit;
      bool f_path_eliminator;
      SMT_Solver_Type f_solver_type;
      void print_help();
      void print_more_info();
      void print_version();
    public:
      LPE_Form_Check();
      ~LPE_Form_Check();
      void get_options(int a_argc, char* a_argv[]);
      void check_formulas();
  };

  // Class LPE_Form_Check - Functions declared private --------------------------------------------

    void LPE_Form_Check::print_help() {
      fprintf(stderr,
        "Usage: %s [OPTION]... [--formulas=FORMULAS] [--lpe=LPE]\n"
        "All formulas in the list of formulas in internal mCRL2 format as found in\n"
        "FORMULAS are checked using the data specification of the mCRL2 LPE as found in\n"
        "LPE. The tool will indicate whether each formula is a tautology or a\n"
        "contradiction. If the tool is unable to determine whether a formula is a\n"
        "tautology or a contradiction, it will indicate this fact.\n"
        "At least one of the arguments --formulas or --lpe is required. If only\n"
        "one is given, stdin is used as the other input.\n"
        "\n"
        "Mandatory arguments to long options are mandatory for short options too.\n"
        "  -f, --formulas=FORMULAS         Use the list of formulas in internal mCRL2\n"
        "                                  format as found in FORMULAS as input.\n"
        "  -l, --lpe=LPE                   Use the mCRL2 LPE as found in LPE as input.\n"
        "  -c, --counter-example           Give a valuation for which the current\n"
        "                                  formula does not hold, in case the current\n"
        "                                  formulas is neither a contradiction nor a\n"
        "                                  tautology.\n"
        "  -w, --witness                   Give a valuation for which the current\n"
        "                                  formula holds, in case the current formula\n"
        "                                  is neither a contradiction nor a tautology.\n"
        "  -h, --help                      Display this help and terminate.\n"
        "      --version                   Display version information and terminate.\n"
        "  -q, --quiet                     Do not display warning messages.\n"
        "  -v, --verbose                   Display concise intermediate messages.\n"
        "  -d, --debug                     Display detailed intermediate messages.\n"
        "  -r, --rewrite-strategy=STRATEGY Use the specified STRATEGY as rewrite\n"
        "                                  strategy:\n"
        "                                  - 'inner' for the innermost rewrite strategy\n"
        "                                  - 'innerc' for the compiled innermost\n"
        "                                    rewrite strategy\n"
        "                                  - 'jitty' for the jitty rewrite strategy\n"
        "                                    (default)\n"
        "                                  - 'jittyc' for the compiled jitty rewrite\n"
        "                                    strategy.\n"
        "  -t, --time-limit=SECONDS        Spend at most the specified number of\n"
        "                                  SECONDS on proving a single formula.\n"
        "  -z --smt-solver=SOLVER          Use the specified SOLVER to remove\n"
        "                                  inconsistent paths from BDDs:\n"
        "                                  - 'ario' for the SMT solver Ario\n"
        "                                  - 'cvc-lite' for the SMT solver CVC Lite.\n"
        "                                  By default, no path elimination is applied.\n",
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
      f_strategy = GS_REWR_JITTY;
      f_time_limit = 0;
      f_path_eliminator = false;
      f_solver_type = solver_type_ario;
    }

    // --------------------------------------------------------------------------------------------

    LPE_Form_Check::~LPE_Form_Check() {
      // Nothing to free.
    }

    // --------------------------------------------------------------------------------------------

    void LPE_Form_Check::get_options(int a_argc, char* a_argv[]) {
      char* v_short_options = "f:l:cwhqvdr:t:z:";

      struct option v_long_options[] = {
        {"formulas",         required_argument, 0, 'f'},
        {"lpe",              required_argument, 0, 'l'},
        {"counter-example",  no_argument,       0, 'c'},
        {"witness",          no_argument,       0, 'w'},
        {"help",             no_argument,       0, 'h'},
        {"version",          no_argument,       0, 0x1},
        {"quiet",            no_argument,       0, 'q'},
        {"verbose",          no_argument,       0, 'v'},
        {"debug",            no_argument,       0, 'd'},
        {"rewrite-strategy", required_argument, 0, 'r'},
        {"time-limit",       required_argument, 0, 't'},
        {"smt-solver",       required_argument, 0, 'z'},
        {0, 0, 0, 0}
      };

      f_tool_command = a_argv[0];

      int v_option = getopt_long(a_argc, a_argv, v_short_options, v_long_options, NULL);
      while (v_option != -1) {
        switch (v_option) {
          case 'f':
            f_formulas_file_name = strdup(optarg);
            break;
          case 'l':
            f_lpe_file_name = strdup(optarg);
            break;
          case 'c':
            f_counter_example = true;
            break;
          case 'w':
            f_witness = true;
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
            if (f_time_limit < 0) {
              f_time_limit = 0;
            }
            break;
          case 'z':
            if (strcmp(optarg, "ario") == 0) {
              f_path_eliminator = true;
              f_solver_type = solver_type_ario;
            } else if (strcmp(optarg, "cvc-lite") == 0) {
              f_path_eliminator = true;
              f_solver_type = solver_type_cvc_lite;
            } else {
              gsErrorMsg("option -z has illegal argument '%s'\n", optarg);
              exit(1);
            }
            break;
          default:
            print_more_info();
            exit(1);
        }
        v_option = getopt_long(a_argc, a_argv, v_short_options, v_long_options, NULL);
      }

      int v_number_of_remaining_arguments = a_argc - optind;
      if (v_number_of_remaining_arguments > 0) {
        gsErrorMsg("%s: too many arguments\n", NAME);
        print_more_info();
        exit(1);
      }
      if ((f_formulas_file_name == 0) and (f_lpe_file_name == 0)) {
        gsErrorMsg("%s: at least one of the options --formulas=FORMULAS or --lpe=LPE has to be used.\n", NAME);
        print_more_info();
        exit(1);
      }
    }

    // --------------------------------------------------------------------------------------------

    void LPE_Form_Check::check_formulas() {
      ATermList v_formulas = (ATermList) read_ATerm_from_file(f_formulas_file_name, "formulas");
      ATermAppl v_lpe = (ATermAppl) read_ATerm_from_file(f_lpe_file_name, "LPE");

      gsEnableConstructorFunctions();
      if ((ATgetType(v_lpe) != AT_APPL) || !gsIsSpecV1(v_lpe)) {
        gsErrorMsg("The file '%s' does not contain an mCRL2 LPE.\n", f_lpe_file_name);
        exit(1);
      } else {
        ATermAppl v_data_equations = ATAgetArgument(v_lpe, 3);
        Formula_Checker v_formula_checker(f_strategy, f_time_limit, f_path_eliminator, f_solver_type, v_data_equations, f_counter_example, f_witness);

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
