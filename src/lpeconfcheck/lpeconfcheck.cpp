#define  NAME      "lpeconfcheck"
#define  VERSION   "0.0.1"
#define  AUTHOR    "Luc Engelen"

#include "confluencechecker.h"
#include "invariantchecker.h"
#include "getopt.h"
#include "libprint_c.h"
#include "libstruct.h"
#include "mcrl2_revision.h"
#include <string>

// Class LPE_Conf_Check ---------------------------------------------------------------------------

  class LPE_Conf_Check {
    private:
      char* f_tool_command;
      char* f_invariant_file_name;
      char* f_input_file_name;
      char* f_output_file_name;
      int f_summand_number;
      bool f_no_check;
      bool f_no_marking;
      bool f_check_all;
      bool f_counter_example;
      RewriteStrategy f_strategy;
      int f_time_limit;
      ATermAppl f_lpe;
      ATermAppl f_invariant;
      void print_help();
      void print_more_info();
      void print_version();
    public:
      LPE_Conf_Check();
      ~LPE_Conf_Check();
      void get_options(int a_argc, char* a_argv[]);
      void read_input();
      bool check_invariant();
      void check_confluence_and_mark();
      void write_result();
  };

  // Class LPE_Conf_Check - Functions declared private --------------------------------------------

    void LPE_Conf_Check::print_help() {
      fprintf(stderr,
        "Usage: %s [OPTION]... [INFILE [OUTFILE]]\n"
        "This tool checks which tau-summands of the mCRL2 LPE as found in INFILE are\n"
        "confluent with relation to all other summands and marks these. The resulting\n"
        "LPE is written to the file named OUTFILE.\n"
        "If INFILE is not specified, the LPE is read from stdin. If OUTFILE is not\n"
        "specified, the resulting LPE is written to stdout.\n"
        "\n"
        "Mandatory arguments to long options are mandatory for short options too.\n"
        "  -i, --invariant=INVARIANT       Use the formula in internal mCRL2 format as\n"
        "                                  found in INVARIANT as invariant.\n"
        "  -s, --summand=NUMBER            Check the summand with number NUMBER only.\n"
        "  -n, --no-check                  Do not check if the invariant holds before\n"
        "                                  checking for confluence.\n"
        "  -m, --no-marking                Do not mark the confluent tau-summands.\n"
        "                                  Since there are no changes made to the LPE,\n"
        "                                  nothing is written to OUTFILE.\n"
        "  -a, --check-all                 Do not stop checking summands on detection\n"
        "                                  of the first non-confluence.\n"
        "  -c, --counter-example           Give a valuation for which the confluence\n"
        "                                  condition encountered last does not hold.\n"
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
        "                                  - 'jittyc' for the compiled jitty rewrite\n"
        "                                    strategy.\n"
        "  -t, --time-limit=SECONDS        Spend at most the specified number of\n"
        "                                  SECONDS on proving a single formula.\n",
        f_tool_command
      );
    }

    // --------------------------------------------------------------------------------------------

    void LPE_Conf_Check::print_more_info() {
      fprintf(stderr, "Try \'%s --help\' for more information.\n", f_tool_command);
    }

    // --------------------------------------------------------------------------------------------

    void LPE_Conf_Check::print_version() {
      fprintf(stderr,"%s %s (revision %d)\n", NAME, VERSION, REVISION);
    }

  // Class LPE_Conf_Check - Functions declared public ---------------------------------------------

    LPE_Conf_Check::LPE_Conf_Check() {
      f_tool_command = 0;
      f_invariant_file_name = 0;
      f_input_file_name = 0;
      f_output_file_name = 0;
      f_summand_number = 0;
      f_no_check = false;
      f_no_marking = false;
      f_check_all = false;
      f_counter_example = false;
      f_strategy = GS_REWR_JITTY;
      f_time_limit = 0;
    }

    // --------------------------------------------------------------------------------------------

    LPE_Conf_Check::~LPE_Conf_Check() {
      // Nothing to free.
    }

    // --------------------------------------------------------------------------------------------

    void LPE_Conf_Check::get_options(int a_argc, char* a_argv[]) {
      char* v_short_options = "i:s:nmachqvdr:t:";

      f_tool_command = a_argv[0];

      struct option v_long_options[] = {
        {"invariant",        required_argument, 0, 'i'},
        {"summand",          required_argument, 0, 's'},
        {"no-check",         no_argument,       0, 'n'},
        {"no-marking",       no_argument,       0, 'm'},
        {"check-all",        no_argument,       0, 'a'},
        {"counter-example",  no_argument,       0, 'c'},
        {"help",             no_argument,       0, 'h'},
        {"version",          no_argument,       0, 0x1},
        {"quiet",            no_argument,       0, 'q'},
        {"verbose",          no_argument,       0, 'v'},
        {"debug",            no_argument,       0, 'd'},
        {"rewrite-strategy", required_argument, 0, 'r'},
        {"time-limit",       required_argument, 0, 't'},
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
          case 'm':
            f_no_marking = true;
            break;
          case 'a':
            f_check_all = true;
            break;
          case 'c':
            f_counter_example = true;
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
              gsErrorMsg("The time-limit must be greater than or equal to one.\n");
              exit(0);
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
          f_input_file_name = strdup(a_argv[optind]);
          if (v_number_of_remaining_arguments == 2) {
            f_output_file_name = strdup(a_argv[optind + 1]);
          }
        }
      }
    }

    // --------------------------------------------------------------------------------------------

    void LPE_Conf_Check::read_input() {
      if (f_invariant_file_name != 0) {
        f_invariant = (ATermAppl) read_ATerm_from_file(f_invariant_file_name, "invariant");
      } else {
        f_invariant = 0;
      }
      f_lpe = (ATermAppl) read_ATerm_from_file(f_input_file_name, "LPE");

      gsEnableConstructorFunctions();
      if ((ATgetType(f_lpe) != AT_APPL) || !gsIsSpecV1(f_lpe)) {
        gsErrorMsg("The file '%s' does not contain an mCRL2 LPE.\n", f_input_file_name);
        exit(1);
      }
    }

    // --------------------------------------------------------------------------------------------

    bool LPE_Conf_Check::check_invariant() {
      if (!f_no_check && f_invariant_file_name != 0) {
        Invariant_Checker v_invariant_checker(f_strategy, f_time_limit, f_lpe, false, false);

        return v_invariant_checker.check_invariant(f_invariant);
      } else {
        if (f_invariant_file_name != 0) {
          gsWarningMsg("The invariant is not checked; it may not hold for this LPE.\n");
        }
        return true;
      }
    }

    // --------------------------------------------------------------------------------------------

    void LPE_Conf_Check::check_confluence_and_mark() {
      Confluence_Checker v_confluence_checker(f_strategy, f_time_limit, f_lpe, f_no_marking, f_check_all, f_counter_example);

      f_lpe = v_confluence_checker.check_confluence_and_mark(f_invariant, f_summand_number);
    }

    // --------------------------------------------------------------------------------------------

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
