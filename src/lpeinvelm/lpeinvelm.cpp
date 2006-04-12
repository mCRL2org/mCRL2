#define  NAME      "lpeinvelm"
#define  VERSION   "0.0.1"
#define  AUTHOR    "Luc Engelen"

#include "invarianteliminator.h"
#include "invariantchecker.h"
#include "getopt.h"
#include "libprint_c.h"
#include "liblowlevel.h"
#include "libstruct.h"
#include "mcrl2_revision.h"
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
      bool f_simplify_all;
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
        "Usage: %s [OPTION]... [--invariant=INVARIANT] [--lpe=LPE] [--output=OUTPUT]\n"
        "This tool uses the invariant as found in INVARIANT to eliminate unreachable\n"
        "summands of the mCRL2 LPE as found in LPE. The resulting LPE is written to the\n"
        "file named OUTPUT.\n"
        "At least one of the arguments --invariant=INVARIANT or --lpe=LPE is required.\n"
        "If only one is given, stdin is used as the other input.\n"
        "If --ouput=OUTPUT is not used, the resulting LPE is written to stdout.\n"
        "\n"
        "Mandatory arguments to long options are mandatory for short options too.\n"
        "  -i, --invariant=INVARIANT       Use the formula in internal mCRL2 format as\n"
        "                                  found in INVARIANT as invariant.\n"
        "  -l, --lpe=LPE                   Use the mCRL2 LPE as found in LPE as input.\n"
        "  -o, --output=OUPUT              write the resulting LPE to the file named\n"
        "                                  OUTPUT.\n"
        "  -s, --summand=NUMBER            Eliminate or simplify the summand with number\n"
        "                                  NUMBER only.\n"
        "  -n, --no-check                  Do not check if the invariant holds before\n"
        "                                  eliminating unreachable summands.\n"
        "  -a, --simplify-all              Simplify the conditions of all summands,\n"
        "                                  instead of just eliminating the summands\n"
        "                                  whose conditions are contradictions.\n"
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
      f_simplify_all = false;
      f_strategy = GS_REWR_JITTY;
      f_time_limit = 0;

    }

    // --------------------------------------------------------------------------------------------

    LPE_Inv_Elm::~LPE_Inv_Elm() {
      // Nothing to free.
    }

    // --------------------------------------------------------------------------------------------

    void LPE_Inv_Elm::get_options(int a_argc, char* a_argv[]) {
      char* v_short_options = "i:l:o:s:nahqvdr:t:";

      f_tool_command = a_argv[0];

      struct option v_long_options[] = {
        {"invariant",        required_argument, 0, 'i'},
        {"lpe",              required_argument, 0, 'l'},
        {"output",           required_argument, 0, 'o'},
        {"summand",          required_argument, 0, 's'},
        {"no-check",         no_argument,       0, 'n'},
        {"simplify-all",     no_argument,       0, 'a'},
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
          case 'o':
            f_output_file_name = strdup(optarg);
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
          case 'a':
            f_simplify_all = true;
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
        Invariant_Checker v_invariant_checker(f_strategy, f_time_limit, f_lpe, false, false);

        return v_invariant_checker.check_invariant(f_invariant);
      } else {
        gsWarningMsg("The invariant is not checked; it may not hold for this LPE.\n");
        return true;
      }
    }

    // --------------------------------------------------------------------------------------------

    void LPE_Inv_Elm::simplify() {
      Invariant_Eliminator v_invariant_eliminator(f_strategy, f_time_limit, f_lpe, f_simplify_all);

      f_lpe = v_invariant_eliminator.simplify(f_invariant, f_summand_number);
    }

    // --------------------------------------------------------------------------------------------

    void LPE_Inv_Elm::write_result() {
      write_ATerm_to_file(f_output_file_name, f_lpe, "resulting LPE");
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
