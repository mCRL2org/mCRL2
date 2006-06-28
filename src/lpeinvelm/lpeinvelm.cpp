#define  NAME      "lpeinvelm"
#define  VERSION   "0.0.1"
#define  AUTHOR    "Luc Engelen"

#include "invariant_eliminator.h"
#include "invariant_checker.h"
#include "getopt.h"
#include "libprint_c.h"
#include "liblowlevel.h"
#include "libstruct.h"
#include "mcrl2_revision.h"
#include "bdd_path_eliminator.h"
#include <string>

// Class LPE_Inv_Elm --------------------------------------------------------------------------

  class LPE_Inv_Elm {
    private:
      char* f_tool_command;
      char* f_invariant_file_name;
      char* f_lpe_file_name;
      char* f_output_file_name;
      int f_summand_number;
      bool f_no_check;
      bool f_no_elimination;
      bool f_simplify_all;
      bool f_all_violations;
      bool f_counter_example;
      char* f_dot_file_name;
      bool f_path_eliminator;
      SMT_Solver_Type f_solver_type;
      RewriteStrategy f_strategy;
      int f_time_limit;
      ATermAppl f_lpe;
      ATermAppl f_invariant;
      void print_help();
      void print_more_info();
      void print_version();
    public:
      LPE_Inv_Elm();
      ~LPE_Inv_Elm();
      void get_options(int argc, char* argv[]);
      void read_input();
      bool check_invariant();
      void simplify();
      void write_result();
   };

  // Class LPE_Inv_Elm - Functions declared private --------------------------------------------

    void LPE_Inv_Elm::print_help() {
      fprintf(stderr,
        "Usage: %s [OPTION]... [INFILE [OUTFILE]] {--invariant=INVARIANT}\n"
        "This tool checks whether the invariant in internal mCRL2 format as found in\n"
        "INVARIANT holds for the mCRL2 LPE as found in INFILE. If the invariant holds,\n"
        "the tool eliminates all summands of the LPE whose condition violates the\n"
        "invariant. It can also be used to simplify the conditions of the summands of\n"
        "the given LPE. The resulting LPE is written to the file named OUTFILE.\n"
        "If INFILE is not specified, the LPE is read from stdin. If OUTFILE is not\n"
        "specified, the resulting LPE is written to stdout.\n"
        "\n"
        "Mandatory arguments to long options are mandatory for short options too.\n"
        "  -i, --invariant=INVARIANT       Use the formula in internal mCRL2 format as\n"
        "                                  found in INVARIANT as invariant.\n"
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
        "                                  By default, no path elimination is applied.\n",
        f_tool_command
      );
    }

    // --------------------------------------------------------------------------------------------

    void LPE_Inv_Elm::print_more_info() {
      fprintf(stderr, "Try \'%s --help\' for more information.\n", f_tool_command);
    }

    // --------------------------------------------------------------------------------------------

    void LPE_Inv_Elm::print_version() {
      fprintf(stderr,"%s %s (revision %d)\n", NAME, VERSION, REVISION);
    }

  // Class LPE_Inv_Elm - Functions declared public ----------------------------------------------

    LPE_Inv_Elm::LPE_Inv_Elm() {
      f_tool_command = 0;
      f_invariant_file_name = 0;
      f_lpe_file_name = 0;
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
    }

    // --------------------------------------------------------------------------------------------

    LPE_Inv_Elm::~LPE_Inv_Elm() {
      // Nothing to free.
    }

    // --------------------------------------------------------------------------------------------

    void LPE_Inv_Elm::get_options(int a_argc, char* a_argv[]) {
      char* v_short_options = "i:s:nelycp:hqvdr:t:z:";

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
      if (v_number_of_remaining_arguments > 2) {
        gsErrorMsg("%s: too many arguments\n", NAME);
        print_more_info();
        exit(1);
      } else {
        if (v_number_of_remaining_arguments > 0) {
          f_lpe_file_name = strdup(a_argv[optind]);
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

    void LPE_Inv_Elm::read_input() {
      f_invariant = (ATermAppl) read_ATerm_from_file(f_invariant_file_name, "invariant");
      f_lpe = (ATermAppl) read_ATerm_from_file(f_lpe_file_name, "LPE");

      gsEnableConstructorFunctions();
      if ((ATgetType(f_lpe) != AT_APPL) || !gsIsSpecV1(f_lpe)) {
        gsErrorMsg("The file '%s' does not contain an mCRL2 LPE.\n", f_lpe_file_name);
        exit(1);
      }
    }

    // --------------------------------------------------------------------------------------------

    bool LPE_Inv_Elm::check_invariant() {
      if (!f_no_check) {
        Invariant_Checker v_invariant_checker(
          f_lpe, f_strategy, f_time_limit, f_path_eliminator, f_solver_type, f_counter_example, f_all_violations, f_dot_file_name
        );

        return v_invariant_checker.check_invariant(f_invariant);
      } else {
        gsWarningMsg("The invariant is not checked; it may not hold for this LPE.\n");
        return true;
      }
    }

    // --------------------------------------------------------------------------------------------

    void LPE_Inv_Elm::simplify() {
      if (!f_no_elimination) {
        Invariant_Eliminator v_invariant_eliminator(f_lpe, f_strategy, f_time_limit, f_path_eliminator, f_solver_type, f_simplify_all);

        f_lpe = v_invariant_eliminator.simplify(f_invariant, f_summand_number);
      }
    }

    // --------------------------------------------------------------------------------------------

    void LPE_Inv_Elm::write_result() {
      if (!f_no_elimination) {
        write_ATerm_to_file(f_output_file_name, f_lpe, "resulting LPE");
      }
    }

// Main function ----------------------------------------------------------------------------------

  int main(int argc, char* argv[]) {
    ATerm v_bottom_of_stack;
    ATinit(argc, argv, &v_bottom_of_stack);

    LPE_Inv_Elm v_lpe_inv_elm;

    v_lpe_inv_elm.get_options(argc, argv);
    v_lpe_inv_elm.read_input();
    if (v_lpe_inv_elm.check_invariant()) {
      v_lpe_inv_elm.simplify();
      v_lpe_inv_elm.write_result();
    }
    return 0;
  }
