// Author(s): Luc Engelen
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsconfcheck.cpp
/// \brief Add your file description here.

#define NAME "lpsconfcheck"
#define AUTHOR "Luc Engelen"

#include "mcrl2/confluence_checker.h"
#include "mcrl2/invariant_checker.h"
#include "getopt.h"
#include "mcrl2/core/detail/parse.h"
#include "mcrl2/core/detail/typecheck.h"
#include "mcrl2/core/detail/data_implementation.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/struct.h"
#include "mcrl2/utilities/aterm_ext.h"
#include "mcrl2/utilities/version_info.h"
#include <string>
#include <fstream>

using namespace ::mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::lps;

  /// \mainpage lpsconfcheck
  /// \section section_introduction Introduction
  /// This document provides information on the internals of the tool.
  /// \section section_additional_info Additional information
  /// More information about the tool and the classes used can be found in the corresponding man files.

// Class LPS_Conf_Check ---------------------------------------------------------------------------

  /// \brief The class LPS_Conf_Check uses an instance of the class Confluence_Checker to check which
  /// \brief tau-summands of an LPS are confluent. The tau-actions of all confluent tau-summands can be
  /// \brief renamed to ctau, depending on the flag LPS_Conf_Check::f_no_marking.

  class LPS_Conf_Check {
    private:
      /// \brief The command entered to invoke the tool lpsconfcheck.
      char* f_tool_command;

      /// \brief The name of a file containing an invariant that is used to check confluence.
      /// \brief If this string is 0, the constant true is used as invariant.
      std::string f_invariant_file_name;

      /// \brief The name of the file containing the LPS.
      /// \brief If this string is 0, the input is read from stdin.
      std::string f_input_file_name;

      /// \brief The name of the file the LPS is written to.
      /// \brief If this string is 0, the output is written to stdout.
      std::string f_output_file_name;

      /// \brief The number of the summand that is checked for confluence.
      /// \brief If this number is 0, all summands are checked.
      int f_summand_number;

      /// \brief The flag indicating if the invariance of resulting expressions should be checked in case a confluence
      /// \brief condition is neither a tautology nor a contradiction.
      bool f_generate_invariants;

      /// \brief The flag indicating whether or not the invariance of the formula as found in the file
      /// \brief LPS_Conf_Check::f_invariant_file_name is checked.
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
      std::string f_dot_file_name;

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

      /// \brief The LPS provided as input.
      ATermAppl f_lps;

      /// \brief The invariant provided as input.
      /// \brief If no invariant was provided, the constant true is used as invariant.
      ATermAppl f_invariant;

      /// \brief Prints the help message.
      void print_help();

      /// \brief Prints a message indicating how to display the help message.
      void print_more_info();

    public:
      /// \brief Constructor setting all flags to their default values.
      LPS_Conf_Check();

      /// \brief Destructor with no particular functionality.
      ~LPS_Conf_Check();

      /// \brief Uses the library getopt to determine which command line options are used.
      void get_options(int a_argc, char* a_argv[]);

      /// \brief Reads an LPS and an invariant from the specified input sources.
      void read_input();

      /// \brief Checks whether or not the invariant holds, if
      /// \brief LPS_Conf_Check::f_invariant_file_name differs from 0 and
      /// \brief LPS_Conf_Check::f_no_check is set to false.
      bool check_invariant();

      /// \brief Checks the confluence of the LPS.
      void check_confluence_and_mark();

      /// \brief Writes the resulting LPS to the preferred output.
      void write_result();
  };

// Squadt protocol interface and utility pseudo-library
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include "mcrl2/utilities/squadt_interface.h"

class squadt_interactor : public mcrl2::utilities::squadt::mcrl2_tool_interface, public LPS_Conf_Check {

  private:

    static const char* lps_file_for_input; ///< file containing an LTS that can be imported using the LTS library
    static const char* invariant_file_for_input; ///< file containing an LTS that can be imported using the LTS library
    static const char* lps_file_for_output;  ///< file containing an LTS that can be imported using the LTS library

    static const char* option_generate_invariants;
    static const char* option_check_invariant;
    static const char* option_mark_tau;
    static const char* option_check_combinations;
    static const char* option_counter_example;
    static const char* option_induction_on_lists;
    static const char* option_invariant;
    static const char* option_time_limit;
    static const char* option_rewrite_strategy;
    static const char* option_smt_solver;

  private:

    boost::shared_ptr < tipi::datatype::enumeration > smt_solver_enumeration;

  public:

    /** \brief constructor */
    squadt_interactor();

    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities&) const;

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration&);

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const&) const;

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration&);
};

const char* squadt_interactor::lps_file_for_input       = "lps_in";
const char* squadt_interactor::invariant_file_for_input = "invariant_in";
const char* squadt_interactor::lps_file_for_output      = "lps_out";

const char* squadt_interactor::option_generate_invariants = "generate_invariants";
const char* squadt_interactor::option_check_invariant     = "check_invariant";
const char* squadt_interactor::option_mark_tau            = "mark_tau";
const char* squadt_interactor::option_check_combinations  = "check_combinations";
const char* squadt_interactor::option_counter_example     = "counter_example";
const char* squadt_interactor::option_induction_on_lists  = "induction_on_lists";
const char* squadt_interactor::option_invariant           = "invariant";
const char* squadt_interactor::option_time_limit          = "time_limit";
const char* squadt_interactor::option_rewrite_strategy    = "rewrite_strategy";
const char* squadt_interactor::option_smt_solver          = "smt_solver";

squadt_interactor::squadt_interactor() {
  smt_solver_enumeration.reset(new tipi::datatype::enumeration("none"));

  *smt_solver_enumeration % "ario" % "cvc";
}

void squadt_interactor::set_capabilities(tipi::tool::capabilities& c) const {
  c.add_input_configuration(lps_file_for_input, tipi::mime_type("mcrl2", tipi::mime_type::text),
                                                            tipi::tool::category::transformation);
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& c) {
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::datatype;
  using namespace tipi::layout::elements;

  std::string infilename = c.get_input(lps_file_for_input).get_location();

  // Set defaults for options
  if (!c.option_exists(option_generate_invariants)) {
    c.add_option(option_generate_invariants).set_argument_value< 0, tipi::datatype::boolean >(false);
  }
  if (!c.option_exists(option_check_invariant)) {
    c.add_option(option_check_invariant).set_argument_value< 0, tipi::datatype::boolean >(
        c.option_exists(option_generate_invariants) || c.option_exists(option_invariant));
  }
  if (!c.option_exists(option_mark_tau)) {
    c.add_option(option_mark_tau).set_argument_value< 0, tipi::datatype::boolean >(true);
  }
  if (!c.option_exists(option_check_combinations)) {
    c.add_option(option_check_combinations).set_argument_value< 0, tipi::datatype::boolean >(false);
  }
  if (!c.option_exists(option_counter_example)) {
    c.add_option(option_counter_example).set_argument_value< 0, tipi::datatype::boolean >(false);
  }
  if (!c.option_exists(option_induction_on_lists)) {
    c.add_option(option_induction_on_lists).set_argument_value< 0, tipi::datatype::boolean >(false);
  }

  /* Create display */
  tipi::layout::tool_display d;

  // Helper for linearisation method selection
  mcrl2::utilities::squadt::radio_button_helper < SMT_Solver_Type > solver_selector(d);

  // Helper for strategy selection
  mcrl2::utilities::squadt::radio_button_helper < RewriteStrategy > strategy_selector(d);

  layout::vertical_box& m = d.create< vertical_box >().set_default_margins(margins(0,5,0,5));

  m.append(d.create< horizontal_box >().
        append(d.create< label >().set_text("SMT prover: ")).
        append(solver_selector.associate(solver_type_cvc_fast, "none")).
        append(solver_selector.associate(solver_type_ario, "ario")).
        append(solver_selector.associate(solver_type_cvc, "CVC3", true)));

  m.append(d.create< horizontal_box >().
                append(d.create< label >().set_text("Rewrite strategy")).
                append(strategy_selector.associate(GS_REWR_INNER, "inner")).
                append(strategy_selector.associate(GS_REWR_INNERC, "innerc")).
                append(strategy_selector.associate(GS_REWR_JITTY, "jitty")).
                append(strategy_selector.associate(GS_REWR_JITTYC, "jittyc")));

  checkbox&   generate_invariants = d.create< checkbox >().set_status(c.get_option_argument< bool >(option_generate_invariants));
  checkbox&   check_invariant     = d.create< checkbox >().set_status(c.get_option_argument< bool >(option_check_invariant));
  checkbox&   mark_tau            = d.create< checkbox >().set_status(c.get_option_argument< bool >(option_mark_tau));
  checkbox&   check_combinations  = d.create< checkbox >().set_status(c.get_option_argument< bool >(option_check_combinations));
  checkbox&   counter_example     = d.create< checkbox >().set_status(c.get_option_argument< bool >(option_counter_example));
  checkbox&   induction_on_lists  = d.create< checkbox >().set_status(c.get_option_argument< bool >(option_induction_on_lists));
  text_field& invariant           = d.create< text_field >().set_text("");
  text_field& time_limit          = d.create< text_field >().set_text("0");

  // two columns to select the linearisation options of the tool
  m.append(d.create< label >().set_text(" ")).
    append(d.create< horizontal_box >().
        append(d.create< vertical_box >().set_default_alignment(layout::right).
            append(generate_invariants.set_label("Generate invariants")).
            append(check_invariant.set_label("Check invariant")).
            append(mark_tau.set_label("Mark confluent tau's")).
            append(check_combinations.set_label("Check all combinations of summands for confluence")).
            append(counter_example.set_label("Produce counter examples")).
            append(induction_on_lists.set_label("Add delta summands"))).
            append(d.create< text_field >().set_text("Time limit for proving a single formula")).
        append(d.create< vertical_box >().set_default_alignment(layout::left).
            append(d.create< checkbox >(), layout::hidden).
            append(invariant).
            append(d.create< checkbox >(), layout::hidden).
            append(d.create< checkbox >(), layout::hidden).
            append(d.create< checkbox >(), layout::hidden).
            append(d.create< checkbox >(), layout::hidden).
            append(time_limit)));

  // Set default values for options if the configuration specifies them
  if (c.option_exists(option_rewrite_strategy)) {
    strategy_selector.set_selection(static_cast < RewriteStrategy > (
        c.get_option_argument< size_t >(option_rewrite_strategy, 0)));
  }
  if (c.option_exists(option_invariant)) {
    invariant.set_text(c.get_option_argument< std::string >(option_invariant));;
  }
  if (c.option_exists(option_time_limit)) {
    time_limit.set_text(c.get_option_argument< std::string >(option_time_limit));
  }

  // Add okay button
  button& okay_button = d.create< button >().set_label("OK");

  m.append(d.create< label >().set_text(" ")).
    append(okay_button, layout::right);

  send_display_layout(d.set_manager(m));

  /* Wait for the OK button to be pressed */
  okay_button.await_change();

  // Update configuration
  using tipi::datatype::boolean;

  c.get_option(option_generate_invariants).set_argument_value< 0, boolean >(generate_invariants.get_status());
  c.get_option(option_check_invariant).set_argument_value< 0, boolean >(check_invariant.get_status());
  c.get_option(option_mark_tau).set_argument_value< 0, boolean >(mark_tau.get_status());
  c.get_option(option_check_combinations).set_argument_value< 0, boolean >(check_combinations.get_status());
  c.get_option(option_counter_example).set_argument_value< 0, boolean >(counter_example.get_status());
  c.get_option(option_induction_on_lists).set_argument_value< 0, boolean >(induction_on_lists.get_status());

  if (!c.output_exists(lps_file_for_output) && !c.get_option_argument< bool >(option_mark_tau)) {
    c.add_output(lps_file_for_output, tipi::mime_type("lps", tipi::mime_type::application), c.get_output_name(".lps"));
  }

  using mcrl2::utilities::squadt::rewrite_strategy_enumeration;

  if (invariant.get_text().empty()) {
    c.remove_option(option_invariant);
  }
  else {
    c.add_option(option_invariant).set_argument_value< 0, tipi::datatype::string >(invariant.get_text());
  }
  if (time_limit.get_text().empty()) {
    c.remove_option(option_time_limit);
  }
  else {
    c.add_option(option_time_limit).set_argument_value< 0, tipi::datatype::natural >(time_limit.get_text());
  }

  c.get_option(option_rewrite_strategy).replace_argument(0, rewrite_strategy_enumeration, strategy_selector.get_selection());
  c.get_option(option_smt_solver).replace_argument(0, smt_solver_enumeration, solver_selector.get_selection());

  send_clear_display();
}

bool squadt_interactor::check_configuration(tipi::configuration const& c) const {
  bool result = true;

  result |= c.input_exists(lps_file_for_input);

  return (result);
}

bool squadt_interactor::perform_task(tipi::configuration& c) {
  using namespace boost;
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::datatype;
  using namespace tipi::layout::elements;

  bool result = true;

  t_lin_options task_options;
  
  // Extract configuration
  extract_task_options(c, task_options);

  /* Create display */
  tipi::layout::tool_display d;

  label& message = d.create< label >();
 
  d.set_manager(d.create< vertical_box >().
                        append(message.set_text("Linearisation in progress"), layout::left));

  send_display_layout(d);

  // Perform linearisation
  ATermAppl linearisation_result = linearise_file(task_options);

  if (linearisation_result == 0) {
    message.set_text("Linearisation in failed");

    result = false;
  }
  else if (task_options.opt_check_only) {
    message.set_text(str(format("%s contains a well-formed mCRL2 specification.") % task_options.infilename));
  }
  else {
    //store the result
    FILE *outstream = fopen(task_options.outfilename.c_str(), "wb");

    if (outstream != 0) {
      ATwriteToSAFFile((ATerm) linearisation_result, outstream);

      fclose(outstream);
    }
    else {
      send_error(str(format("cannot open output file '%s'\n") % task_options.outfilename));

      result = false;
    }

    if (result) {
      message.set_text("Linearisation finished");
    }
  }

  send_display_layout(d);

  return (result);
}

#endif

  // Class LPS_Conf_Check - Functions declared private --------------------------------------------

    void LPS_Conf_Check::print_help() {
      fprintf(stdout,
        "Usage: %s [OPTION]... [INFILE [OUTFILE]]\n"
        "Checks which tau-summands of the mCRL2 LPS in INFILE are confluent, marks them\n"
        "by renaming them to ctau, and write the result to OUTFILE. If INFILE is not\n"
        "present stdin is used. If OUTFILE is not present, stdout is used.\n"
        "\n"
        "Options:\n"
        "  -iINVFILE, --invariant=INVFILE  use the formula (a boolean expression in mCRL2\n"
        "                                  format) in INVFILE as invariant\n"
        "  -g. --generate-invariants       try to prove that the reduced confluence\n"
        "                                  condition is an invariant of the LPS, in case\n"
        "                                  the confluence condition is not a tautology\n"
        "  -sNUM, --summand=NUM            check the summand with number NUM only\n"
        "  -n, --no-check                  do not check if the invariant holds before\n"
        "                                  checking for confluence\n"
        "  -m, --no-marking                do not mark the confluent tau-summands; since\n"
        "                                  there are no changes made to the LPS, nothing\n"
        "                                  is written to OUTFILE\n"
        "  -a, --check-all                 check the confluence of tau-summands regarding\n"
        "                                  all other summands, instead of continuing with\n"
        "                                  the next tau-summand as soon as a summand is\n"
        "                                  encountered that is not confluent with the\n"
        "                                  current tau-summand\n"
        "  -c, --counter-example           display a valuation for which the confluence\n"
        "                                  condition does not hold, in case the\n"
        "                                  encountered condition is neither a\n"
        "                                  contradiction nor a tautolgy\n"
        "  -pPREFIX, --print-dot=PREFIX    save a .dot file of the resulting BDD in case\n"
        "                                  two summands cannot be proven confluent;\n"
        "                                  PREFIX will be used as prefix of the output\n"
        "                                  files\n"
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
        "  -zSOLVER --smt-solver=SOLVER    use SOLVER to remove inconsistent paths from\n"
        "                                  the internally used BDDs:\n"
#ifdef CVC_LITE_LIB
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

    void LPS_Conf_Check::print_more_info() {
      fprintf(stderr, "Try \'%s --help\' for more information.\n", f_tool_command);
    }

    // --------------------------------------------------------------------------------------------

  // Class LPS_Conf_Check - Functions declared public ---------------------------------------------

    LPS_Conf_Check::LPS_Conf_Check() : f_input_file_name("-"), f_output_file_name("-") {
      f_dot_file_name = "";
      f_tool_command = 0;
      f_summand_number = 0;
      f_generate_invariants = false;
      f_no_check = false;
      f_no_marking = false;
      f_check_all = false;
      f_counter_example = false;
      f_strategy = GS_REWR_JITTY;
      f_time_limit = 0;
      f_path_eliminator = false;
      f_solver_type = solver_type_ario;
      f_apply_induction = false;
    }

    // --------------------------------------------------------------------------------------------

    LPS_Conf_Check::~LPS_Conf_Check() {
    }

    // --------------------------------------------------------------------------------------------

    /// Sets the flags of the class according to the command line options passed.
    /// \param a_argc is the number of arguments passed on the command line
    /// \param a_argv is an array of all arguments passed on the command line

    void LPS_Conf_Check::get_options(int a_argc, char* a_argv[]) {
      const char* v_short_options = "i:gs:nmacp:hqvdr:t:z:o";

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
            f_invariant_file_name = std::string(optarg);
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
            f_dot_file_name = std::string(optarg);
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
              gsErrorMsg("the time-limit must be greater than or equal to one.\n");
              exit(1);
            }
            break;
          case 'z':
            if (strcmp(optarg, "ario") == 0) {
              f_path_eliminator = true;
              f_solver_type = solver_type_ario;
            } else if (strcmp(optarg, "cvc") == 0) {
              f_path_eliminator = true;
              f_solver_type = solver_type_cvc;
#ifdef HAVE_CVC
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
          f_input_file_name = std::string(a_argv[optind]);
          if (v_number_of_remaining_arguments == 2) {
            f_output_file_name = std::string(a_argv[optind + 1]);
          }
        }
      }
    }

    // --------------------------------------------------------------------------------------------

    /// Reads the invariant specified by LPS_Conf_Check::f_invariant_file_name and saves it as
    /// LPS_Conf_Check::f_invariant. If no file name was specified, the constant true is used as
    /// invariant.
    /// Reads the LPS specified by LPS_Conf_Check::f_input_file_name and saves it as
    /// LPS_Conf_Check::f_lps. If no input file name was specified, the LPS is read from stdin.

    void LPS_Conf_Check::read_input() {
      
      if (!f_invariant_file_name.empty()) {
        std::ifstream instream(f_invariant_file_name.c_str());
      
        //XXX commented out because it always raised exceptions, rendering the program useless
        //instream.exceptions(std::ifstream::eofbit|std::ifstream::failbit|std::ifstream::badbit);

        if (!instream.is_open()) {
          throw std::runtime_error("cannot open input file '" + f_invariant_file_name + "'");
        }

        gsVerboseMsg("parsing input file '%s'...\n", f_invariant_file_name.c_str());

        if (!(f_invariant = parse_data_expr(instream))) {
          throw std::runtime_error("parsing failed!");
        }

        instream.close();
      }
      else {
        f_invariant = gsMakeOpIdTrue();
      }

      specification lps_specification;
      
      lps_specification.load(f_input_file_name);

      if (!lps_specification.is_well_typed()) {
        throw std::runtime_error("Invalid mCRL2 LPS read from " + f_input_file_name);
      }
      // temporary measure, until the invariant and confluence checkers use the lps framework
      f_lps = (ATermAppl) lps_specification;

      //typecheck the invariant formula
      ATermList vars = lps_specification.process().process_parameters();
      ATermTable var_table = ATtableCreate(63,50);
      for (; !ATisEmpty(vars); vars = ATgetNext(vars)) {
        ATermAppl var = ATAgetFirst(vars);
        ATtablePut(var_table, ATgetArgument(var, 0), ATgetArgument(var, 1));
      }
      f_invariant = type_check_data_expr(f_invariant, gsMakeSortIdBool(), lps_specification, var_table);
      ATtableDestroy(var_table);
      if (!f_invariant) {
        throw std::runtime_error("Typechecking of the invariant formula failed.\n");
      }
     
      //data implement the invariant formula
      f_invariant = implement_data_data_expr(f_invariant, lps_specification);
      if (!f_invariant) {
        throw std::runtime_error("Data implementation of the invariant formula failed.\n");
      }
    }

    // --------------------------------------------------------------------------------------------

    /// Checks if the formula in the file LPS_Conf_Check::f_invariant_file_name is an invariant,
    /// if the flag LPS_Conf_Check::f_no_check is set to false and
    /// LPS_Conf_Check::f_invariant_file_name differs from 0.
    /// \return true, if the invariant holds or no invariant is specified.
    ///         false, if the invariant does not hold.

    bool LPS_Conf_Check::check_invariant() {
      if (!f_invariant_file_name.empty()) {
        if (!f_no_check) {
          Invariant_Checker v_invariant_checker(
            f_lps, f_strategy, f_time_limit, f_path_eliminator, f_solver_type, false, false, false, (f_dot_file_name == "")?0:const_cast < char* > (f_dot_file_name.c_str()));

          return v_invariant_checker.check_invariant(f_invariant);
        }
        else {
          gsWarningMsg("The invariant is not checked; it may not hold for this LPS.\n");
        }
      }

      return true;
    }

    // --------------------------------------------------------------------------------------------

    /// Checks which tau-summands of the LPS LPS_Conf_Check::f_lps are confluent. If the flag
    /// LPS_Conf_Check::f_no_marking is set to false, the tau-actions of the confluent tau-summands
    /// are renamed to ctau.

    void LPS_Conf_Check::check_confluence_and_mark() {
      Confluence_Checker v_confluence_checker(
        f_lps, f_strategy, f_time_limit, f_path_eliminator, f_solver_type, f_apply_induction, f_no_marking, f_check_all, f_counter_example,
        f_generate_invariants, (f_dot_file_name == "")?0:const_cast < char* > (f_dot_file_name.c_str()));

      f_lps = v_confluence_checker.check_confluence_and_mark(f_invariant, f_summand_number);
    }

    // --------------------------------------------------------------------------------------------

    /// Writes the LPS with the tau-actions of all confluent tau-summands renamed to ctau to the file
    /// specified by LPS_Conf_Check::f_output_file_name, if the flag LPS_Conf_Check::f_no_marking
    /// is set to false.

    void LPS_Conf_Check::write_result() {
      if (!f_no_marking) {
        ATwriteToNamedSAFFile((ATerm) f_lps, const_cast < char* > (f_output_file_name.c_str()));
      }
    }

// Main function ----------------------------------------------------------------------------------

int main(int argc, char* argv[]) {

  MCRL2_ATERM_INIT(argc, argv)

  try {
    LPS_Conf_Check v_lps_conf_check;

    v_lps_conf_check.get_options(argc, argv);
    v_lps_conf_check.read_input();

    if (v_lps_conf_check.check_invariant()) {
      v_lps_conf_check.check_confluence_and_mark();
      v_lps_conf_check.write_result();
    }
  }
  catch (std::exception& e) {
    gsErrorMsg("Fatal: %s\n", e.what());

    return 1;
  }

  return 0;
}
