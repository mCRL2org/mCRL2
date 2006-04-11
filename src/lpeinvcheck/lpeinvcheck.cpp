#define  NAME      "lpeinvcheck"
#define  VERSION   "0.0.1"
#define  AUTHOR    "Luc Engelen"

#include "getopt.h"
#include "invariantchecker.h"
#include "libprint_c.h"
#include "libstruct.h"
#include <string>

// Class LPE_Inv_Check --------------------------------------------------------------------------

  class LPE_Inv_Check {
    private:
      char* f_tool_command;
      char* f_invariant_file_name;
      char* f_lpe_file_name;
      bool f_all;
      bool f_counter_example;
      RewriteStrategy f_strategy;
      int f_time_limit;
      void print_help();
      void print_more_info();
      void print_version();
    public:
      LPE_Inv_Check();
      ~LPE_Inv_Check();
      void get_options(int argc, char* argv[]);
      void check_invariant();
   };

  // Class LPE_Inv_Check - Functions declared private --------------------------------------------

    void LPE_Inv_Check::print_help() {
      fprintf(stderr,
        "Usage: %s [OPTION]... [--invariant=INVARIANT] [--lpe=LPE]\n"
        "This tool checks whether the formula in INVARIANT is an invariant of the mCRL2\n"
        "LPE as found in LPE.\n"
        "At least one of the arguments --invariant=INVARIANT or --lpe=LPE is required.\n"
        "If only one is given, stdin is used as the other input.\n"
        "\n"
        "Mandatory arguments to long options are mandatory for short options too.\n"
        "  -i, --invariant=INVARIANT       Use the formula in internal mCRL2 format as\n"
        "                                  found in INVARIANT as invariant.\n"
        "  -l, --lpe=LPE                   Use the mCRL2 LPE as found in LPE as input.\n"
        "  -a, --all                       Do not terminate as soon as a violation of\n"
        "                                  the invariant is found, but report all\n"
        "                                  violations instead.\n"
        "  -c, --counter-example           Give a valuation for which the invariant is\n"
        "                                  violated.\n"
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

    void LPE_Inv_Check::print_more_info() {
      fprintf(stderr, "Try \'%s --help\' for more information.\n", f_tool_command);
    }

    // --------------------------------------------------------------------------------------------

    void LPE_Inv_Check::print_version() {
      fprintf(stderr,"%s %s (revision %d)\n", NAME, VERSION, REVISION);
    }

  // Class LPE_Inv_Check - Functions declared public ----------------------------------------------

    LPE_Inv_Check::LPE_Inv_Check() {
      f_tool_command = 0;
      f_invariant_file_name = 0;
      f_lpe_file_name = 0;
      f_all = false;
      f_counter_example = false;
      f_strategy = GS_REWR_JITTY;
      f_time_limit = 0;

    }

    // --------------------------------------------------------------------------------------------

    LPE_Inv_Check::~LPE_Inv_Check() {
      // Nothing to free.
    }

    void LPE_Inv_Check::get_options(int a_argc, char* a_argv[]) {
      char* v_short_options = "i:l:achqvdr:t:";

      f_tool_command = a_argv[0];

      struct option v_long_options[] = {
        {"invariant",        required_argument, 0, 'i'},
        {"lpe",              required_argument, 0, 'l'},
        {"all",              no_argument,       0, 'a'},
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
          case 'l':
            f_lpe_file_name = strdup(optarg);
            break;
          case 'a':
            f_all = true;
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
              f_time_limit = 0;
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
      if ((f_invariant_file_name == 0) and (f_lpe_file_name == 0)) {
        gsErrorMsg(
          "%s: at least one of the options --invariant=INVARIANT or "
          "--lpe=LPE has to be used.\n",
          NAME
        );
        print_more_info();
        exit(1);
      }
    }

    // --------------------------------------------------------------------------------------------

    void LPE_Inv_Check::check_invariant() {
      ATermAppl v_invariant = (ATermAppl) read_ATerm_from_file(f_invariant_file_name, "invariant");
      ATermAppl v_lpe = (ATermAppl) read_ATerm_from_file(f_lpe_file_name, "LPE");

      gsEnableConstructorFunctions();
      if ((ATgetType(v_lpe) != AT_APPL) || !gsIsSpecV1(v_lpe)) {
        gsErrorMsg("The file '%s' does not contain an mCRL2 LPE.\n", f_lpe_file_name);
        exit(1);
      } else {
        Invariant_Checker v_invariant_checker(f_strategy, f_time_limit, v_lpe, f_counter_example, f_all);

        v_invariant_checker.check_invariant(v_invariant);
      }
    }

// Main function ----------------------------------------------------------------------------------

  int main(int argc, char* argv[]) {
    ATerm v_bottom_of_stack;
    ATinit(argc, argv, &v_bottom_of_stack);

    LPE_Inv_Check v_lpe_inv_check;

    v_lpe_inv_check.get_options(argc, argv);
    v_lpe_inv_check.check_invariant();

    return 0;
  }
