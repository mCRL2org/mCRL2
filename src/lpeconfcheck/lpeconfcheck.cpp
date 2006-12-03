#define  NAME      "lpeconfcheck"
#define  VERSION   "0.0.1"
#define  AUTHOR    "Luc Engelen"

#include "confluence_checker.h"
#include "invariant_checker.h"
#include "getopt.h"
#include "libprint_c.h"
#include "libstruct.h"
#include <string>

  /// \mainpage lpeconfcheck
  /// \section section_introduction Introduction
  /// This document provides information on the internals of the tool.
  /// \section section_additional_info Additional information
  /// More information about the tool and the classes used can be found in the corresponding man files.

// Class LPE_Conf_Check ---------------------------------------------------------------------------

  /// \brief The class LPE_Conf_Check uses an instance of the class Confluence_Checker to check which
  /// \brief tau-summands of an LPE are confluent. The tau-actions of all confluent tau-summands can be
  /// \brief renamed to ctau, depending on the flag LPE_Conf_Check::f_no_marking.

  class LPE_Conf_Check {
    private:
      /// \brief The command entered to invoke the tool lpeconfcheck.
      char* f_tool_command;

      /// \brief The name of a file containing an invariant that is used to check confluence.
      /// \brief If this string is 0, the constant true is used as invariant.
      char* f_invariant_file_name;

      /// \brief The name of the file containing the LPE.
      /// \brief If this string is 0, the input is read from stdin.
      char* f_input_file_name;

      /// \brief The name of the file the LPE is written to.
      /// \brief If this string is 0, the output is written to stdout.
      char* f_output_file_name;

      /// \brief The number of the summand that is checked for confluence.
      /// \brief If this number is 0, all summands are checked.
      int f_summand_number;

      /// \brief The flag indicating if the invariance of resulting expressions should be checked in case a confluence
      /// \brief condition is neither a tautology nor a contradiction.
      bool f_generate_invariants;

      /// \brief The flag indicating whether or not the invariance of the formula as found in the file
      /// \brief LPE_Conf_Check::f_invariant_file_name is checked.
      bool f_no_check;

      /// \brief The flag indicating whether or not the tau-actions of the confluent summands should be renamed to ctau.
      bool f_no_marking;

      /// \brief The flag indicating whether or not the confluence of a tau-summand regarding all other summands is checked.
      bool f_check_all;

      /// \brief The flag indicating whether or not counter examples are printed each time a condition is encountered
      /// \brief that is neither a contradiction nor a tautology.
      bool f_counter_example;

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

      /// \brief The LPE provided as input.
      ATermAppl f_lpe;

      /// \brief The invariant provided as input.
      /// \brief If no invariant was provided, the constant true is used as invariant.
      ATermAppl f_invariant;

      /// \brief Prints the help message.
      void print_help();

      /// \brief Prints a message indicating how to display the help message.
      void print_more_info();

      /// \brief Prints the version of the tool.
      void print_version();

    public:
      /// \brief Constructor setting all flags to their default values.
      LPE_Conf_Check();

      /// \brief Destructor with no particular functionality.
      ~LPE_Conf_Check();

      /// \brief Uses the library getopt to determine which command line options are used.
      void get_options(int a_argc, char* a_argv[]);

      /// \brief Reads an LPE and an invariant from the specified input sources.
      void read_input();

      /// \brief Checks whether or not the invariant holds, if
      /// \brief LPE_Conf_Check::f_invariant_file_name differs from 0 and
      /// \brief LPE_Conf_Check::f_no_check is set to false.
      bool check_invariant();

      /// \brief Checks the confluence of the LPE.
      void check_confluence_and_mark();

      /// \brief Writes the resulting LPE to the preferred output.
      void write_result();
  };

  // Class LPE_Conf_Check - Functions declared private --------------------------------------------

    void LPE_Conf_Check::print_help() {
      fprintf(stderr,
        "Usage: %s [OPTION]... [INFILE [OUTFILE]]\n"
        "This tool checks which tau-summands of the mCRL2 LPE as found in INFILE are\n"
        "confluent and marks them by renaming them to ctau. The resulting LPE is\n"
        "written to the file named OUTFILE.\n"
        "If INFILE is not specified, the LPE is read from stdin. If OUTFILE is not\n"
        "specified, the resulting LPE is written to stdout.\n"
        "\n"
        "Mandatory arguments to long options are mandatory for short options too.\n"
        "  -i, --invariant=INVARIANT       Use the formula in internal mCRL2 format as\n"
        "                                  found in INVARIANT as invariant.\n"
        "  -g. --generate-invariants       Try to prove that the reduced confluence\n"
        "                                  condition is an invariant of the LPE, in case\n"
        "                                  the confluence condition is not a tautology.\n"
        "  -s, --summand=NUMBER            Check the summand with number NUMBER only.\n"
        "  -n, --no-check                  Do not check if the invariant holds before\n"
        "                                  checking for confluence.\n"
        "  -m, --no-marking                Do not mark the confluent tau-summands. Since\n"
        "                                  there are no changes made to the LPE, nothing\n"
        "                                  is written to OUTFILE.\n"
        "  -a, --check-all                 Check the confluence of tau-summands\n"
        "                                  regarding all other summands, instead of\n"
        "                                  continuing with the next tau-summand as soon\n"
        "                                  as a summand is encountered that is not\n"
        "                                  confluent with the current tau-summand.\n"
        "  -c, --counter-example           Give a valuation for which the confluence\n"
        "                                  condition does not hold, in case the\n"
        "                                  encountered condition is neither a\n"
        "                                  contradiction nor a tautolgy.\n"
        "  -p, --print-dot=PREFIX          Save a .dot file of the resulting BDD in case\n"
        "                                  two summands cannot be proven confluent.\n"
        "                                  PREFIX will be used as prefix of the output\n"
        "                                  files.\n"
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
        "  -o, --induction                 Apply induction on lists.\n",
        f_tool_command
      );
    }

    // --------------------------------------------------------------------------------------------

    void LPE_Conf_Check::print_more_info() {
      fprintf(stderr, "Try \'%s --help\' for more information.\n", f_tool_command);
    }

    // --------------------------------------------------------------------------------------------

    void LPE_Conf_Check::print_version() {
      fprintf(stderr,"%s %s (revision %s)\n", NAME, VERSION, REVISION);
    }

  // Class LPE_Conf_Check - Functions declared public ---------------------------------------------

    LPE_Conf_Check::LPE_Conf_Check() {
      f_tool_command = 0;
      f_invariant_file_name = 0;
      f_input_file_name = 0;
      f_output_file_name = 0;
      f_summand_number = 0;
      f_generate_invariants = false;
      f_no_check = false;
      f_no_marking = false;
      f_check_all = false;
      f_counter_example = false;
      f_dot_file_name = 0;
      f_strategy = GS_REWR_JITTY;
      f_time_limit = 0;
      f_path_eliminator = false;
      f_solver_type = solver_type_ario;
      f_apply_induction = false;
    }

    // --------------------------------------------------------------------------------------------

    LPE_Conf_Check::~LPE_Conf_Check() {
      free(f_invariant_file_name);
      f_invariant_file_name = 0;
      free(f_input_file_name);
      f_input_file_name = 0;
      free(f_output_file_name);
      f_output_file_name = 0;
      free(f_dot_file_name);
      f_dot_file_name = 0;
    }

    // --------------------------------------------------------------------------------------------

    /// Sets the flags of the class according to the command line options passed.
    /// \param a_argc is the number of arguments passed on the command line
    /// \param a_argv is an array of all arguments passed on the command line

    void LPE_Conf_Check::get_options(int a_argc, char* a_argv[]) {
      char* v_short_options = "i:gs:nmacp:hqvdr:t:z:o";

      f_tool_command = a_argv[0];

      struct option v_long_options[] = {
        {"invariant",        required_argument, 0, 'i'},
        {"negation",         no_argument,       0, 'g'},
        {"summand",          required_argument, 0, 's'},
        {"no-check",         no_argument,       0, 'n'},
        {"no-marking",       no_argument,       0, 'm'},
        {"check-all",        no_argument,       0, 'a'},
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
          case 'g':
            f_generate_invariants = true;
            break;
          case 's':
            sscanf(optarg, "%d", &f_summand_number);
            if (f_summand_number < 1) {
              gsErrorMsg("The summand number must be greater than or equal to 1.\n");
              exit(1);
            } else {
              gsVerboseMsg("Checking confluence of summand number %d.\n", f_summand_number);
            }
            break;
          case 'n':
            f_no_check = true;
            break;
          case 'm':
            f_no_marking = true;
            break;
          case 'a':
            f_check_all = true;
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
          f_input_file_name = strdup(a_argv[optind]);
          if (v_number_of_remaining_arguments == 2) {
            f_output_file_name = strdup(a_argv[optind + 1]);
          }
        }
      }
    }

    // --------------------------------------------------------------------------------------------

    /// Reads the invariant specified by LPE_Conf_Check::f_invariant_file_name and saves it as
    /// LPE_Conf_Check::f_invariant. If no file name was specified, the constant true is used as
    /// invariant.
    /// Reads the LPE specified by LPE_Conf_Check::f_input_file_name and saves it as
    /// LPE_Conf_Check::f_lpe. If no input file name was specified, the LPE is read from stdin.

    void LPE_Conf_Check::read_input() {
      if (f_invariant_file_name != 0) {
        f_invariant = (ATermAppl) read_ATerm_from_file(f_invariant_file_name, "invariant");
      } else {
        gsEnableConstructorFunctions();
        f_invariant = gsMakeOpIdTrue();
      }
      f_lpe = (ATermAppl) read_ATerm_from_file(f_input_file_name, "LPE");

      gsEnableConstructorFunctions();
      if ((ATgetType(f_lpe) != AT_APPL) || !gsIsSpecV1(f_lpe)) {
        gsErrorMsg("The file '%s' does not contain an mCRL2 LPE.\n", f_input_file_name);
        exit(1);
      }
    }

    // --------------------------------------------------------------------------------------------

    /// Checks if the formula in the file LPE_Conf_Check::f_invariant_file_name is an invariant,
    /// if the flag LPE_Conf_Check::f_no_check is set to false and
    /// LPE_Conf_Check::f_invariant_file_name differs from 0.
    /// \return true, if the invariant holds or no invariant is specified.
    ///         false, if the invariant does not hold.

    bool LPE_Conf_Check::check_invariant() {
      bool result = true;

      if (!f_no_check && f_invariant_file_name != 0) {
        Invariant_Checker v_invariant_checker(
          f_lpe, f_strategy, f_time_limit, f_path_eliminator, f_solver_type, false, false, f_dot_file_name
        );

        result = v_invariant_checker.check_invariant(f_invariant);
      } else {
        if (f_invariant_file_name != 0) {
          gsWarningMsg("The invariant is not checked; it may not hold for this LPE.\n");
        }
      }

      return result;
    }

    // --------------------------------------------------------------------------------------------

    /// Checks which tau-summands of the LPE LPE_Conf_Check::f_lpe are confluent. If the flag
    /// LPE_Conf_Check::f_no_marking is set to false, the tau-actions of the confluent tau-summands
    /// are renamed to ctau.

    void LPE_Conf_Check::check_confluence_and_mark() {
      Confluence_Checker v_confluence_checker(
        f_lpe, f_strategy, f_time_limit, f_path_eliminator, f_solver_type, f_apply_induction, f_no_marking, f_check_all, f_counter_example,
        f_generate_invariants, f_dot_file_name
      );

      f_lpe = v_confluence_checker.check_confluence_and_mark(f_invariant, f_summand_number);
    }

    // --------------------------------------------------------------------------------------------

    /// Writes the LPE with the tau-actions of all confluent tau-summands renamed to ctau to the file
    /// specified by LPE_Conf_Check::f_output_file_name, if the flag LPE_Conf_Check::f_no_marking
    /// is set to false.

    void LPE_Conf_Check::write_result() {
      if (!f_no_marking) {
        write_ATerm_to_file(f_output_file_name, f_lpe, "resulting LPE");
      }
    }

// Main function ----------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
  ATerm v_bottom_of_stack;
  ATinit(argc, argv, &v_bottom_of_stack);

  LPE_Conf_Check v_lpe_conf_check;

  v_lpe_conf_check.get_options(argc, argv);
  v_lpe_conf_check.read_input();
  if (v_lpe_conf_check.check_invariant()) {
    v_lpe_conf_check.check_confluence_and_mark();
    v_lpe_conf_check.write_result();
  }
  return 0;
}
