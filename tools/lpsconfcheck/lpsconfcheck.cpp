// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsconfcheck.cpp
/// \brief Add your file description here.

#include "boost.hpp" // precompiled headers

#define NAME "lpsconfcheck"
#define AUTHOR "Luc Engelen"

#include <string>
#include <fstream>

#include "mcrl2/core/parse.h"
#include "mcrl2/core/typecheck.h"
#include "mcrl2/new_data/detail/data_implementation.h"
#include "mcrl2/new_data/detail/data_reconstruct.h"
#include "mcrl2/new_data/rewriter.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/confluence_checker.h"
#include "mcrl2/lps/invariant_checker.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/command_line_messaging.h"
#include "mcrl2/utilities/command_line_rewriting.h"
#include "mcrl2/utilities/command_line_proving.h"

using namespace mcrl2::core;
using namespace mcrl2::lps;
using namespace mcrl2::new_data::detail;

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
      size_t f_summand_number;

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
      /// \brief a contradiction nor a tautology. If the string is empty, no files are written.
      std::string f_dot_file_name;

      /// \brief The rewrite strategy used by the rewriter.
      mcrl2::new_data::rewriter::strategy f_strategy;

      /// \brief The maximal number of seconds spent on proving a single confluence condition.
      size_t f_time_limit;

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

    public:
      /// \brief Constructor setting all flags to their default values.
      LPS_Conf_Check();

      /// \brief Parses command line options
      bool get_options(int argc, char* argv[]);

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

const char* lps_file_for_input       = "lps_in";
const char* invariant_file_for_input = "invariant_in";
const char* lps_file_for_output      = "lps_out";

const char* option_generate_invariants = "generate_invariants";
const char* option_check_invariant     = "check_invariant";
const char* option_mark_tau            = "mark_tau";
const char* option_check_combinations  = "check_combinations";
const char* option_counter_example     = "counter_example";
const char* option_induction_on_lists  = "induction_on_lists";
const char* option_invariant           = "invariant";
const char* option_time_limit          = "time_limit";
const char* option_rewrite_strategy    = "rewrite_strategy";
const char* option_smt_solver          = "smt_solver";

squadt_interactor::squadt_interactor() {
  tipi::datatype::enumeration< SMT_Solver_Type > smt_solver_enumeration;

  smt_solver_enumeration.
    add(solver_type_cvc_fast, "none")
    add(solver_type_ario, "ario")
    add(solver_type_cvc, "cvc");
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
  tipi::tool_display d;

  // Helper for linearisation method selection
  mcrl2::utilities::squadt::radio_button_helper < SMT_Solver_Type > solver_selector(d);

  // Helper for strategy selection
  mcrl2::utilities::squadt::radio_button_helper < mcrl2::new_data::rewriter::strategy > strategy_selector(d);

  layout::vertical_box& m = d.create< vertical_box >().set_default_margins(margins(0,5,0,5));

  m.append(d.create< horizontal_box >().
        append(d.create< label >().set_text("SMT prover: ")).
        append(solver_selector.associate(solver_type_cvc_fast, "none")).
        append(solver_selector.associate(solver_type_ario, "ario")).
        append(solver_selector.associate(solver_type_cvc, "CVC3", true)));

  m.append(d.create< horizontal_box >().
                append(d.create< label >().set_text("Rewrite strategy")).
                append(strategy_selector.associate(GS_REWR_INNER, "inner")).
#ifdef MCRL2_INNERC_AVAILABLE
                append(strategy_selector.associate(GS_REWR_INNERC, "innerc")).
#endif
#ifdef MCRL2_JITTYC_AVAILABLE
                append(strategy_selector.associate(GS_REWR_JITTY, "jitty")).
                append(strategy_selector.associate(GS_REWR_JITTYC, "jittyc")));
#else
                append(strategy_selector.associate(GS_REWR_JITTY, "jitty")));
#endif

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
    strategy_selector.set_selection(c.get_option_argument< mcrl2::new_data::rewriter::strategy >(option_rewrite_strategy, 0));
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

  c.get_option(option_rewrite_strategy).set_argument_value< 0 >(strategy_selector.get_selection());
  c.get_option(option_smt_solver).set_argument_value< 0 >(solver_selector.get_selection());

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
  tipi::tool_display d;

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

  // Class LPS_Conf_Check - Functions declared public ---------------------------------------------

    LPS_Conf_Check::LPS_Conf_Check() :
      f_invariant_file_name(""),
      f_input_file_name(""),
      f_output_file_name(""),
      f_summand_number(0),
      f_generate_invariants(false),
      f_no_check(false),
      f_no_marking(false),
      f_check_all(false),
      f_counter_example(false),
      f_dot_file_name(""),
      f_strategy(mcrl2::new_data::rewriter::jitty),
      f_time_limit(0),
      f_path_eliminator(false),
      f_solver_type(solver_type_ario),
      f_apply_induction(false),
      f_lps(NULL),
      f_invariant(NULL)
    { }

    // --------------------------------------------------------------------------------------------

    /// Sets the flags of the class according to the command line options passed.
    /// \param argc is the number of arguments passed on the command line
    /// \param argv is an array of all arguments passed on the command line

    bool LPS_Conf_Check::get_options(int argc, char* argv[]) {
      using namespace mcrl2::utilities;

      interface_description clinterface(argv[0], NAME, AUTHOR,
        "mark confluent tau-summands of an LPS",
        "[OPTION]... [INFILE [OUTFILE]]\n",
        "Checks which tau-summands of the mCRL2 LPS in INFILE are confluent, marks them by "
        "renaming them to ctau, and write the result to OUTFILE. If INFILE is not present "
        "stdin is used. If OUTFILE is not present, stdout is used.");

      clinterface.add_rewriting_options();
      clinterface.add_prover_options();

      clinterface.
        add_option("invariant", make_mandatory_argument("INVFILE"),
          "use the boolean formula (an mCRL2 data expression of sort Bool) in INVFILE as invariant", 'i').
        add_option("summand", make_mandatory_argument("NUM"),
          "eliminate or simplify the summand with number NUM only", 's').
        add_option("check-all",
          "check the confluence of tau-summands regarding all other summands, instead of "
          "continuing with the next tau-summand as soon as a summand is encountered that "
          "is not confluent with the current tau-summand", 'a').
        add_option("generate-invariants",
          "try to prove that the reduced confluence condition is an invariant of the LPS, "
          "in case the confluence condition is not a tautology", 'g').
        add_option("no-check",
          "do not check if the invariant holds before checking for for confluence", 'n').
        add_option("no-marking",
          "do not mark the confluent tau-summands; since there are no changes made to the LPS, "
          "nothing is written to OUTFILE", 'm').
        add_option("counter-example",
          "display a valuation for which the confluence condition does not hold, in case the "
          "encountered condition is neither a contradiction nor a tautolgy", 'c').
        add_option("print-dot", make_mandatory_argument("PREFIX"),
          "save a .dot file of the resulting BDD in case two summands cannot be proven "
          "confluent; PREFIX will be used as prefix of the output files", 'p').
        add_option("time-limit", make_mandatory_argument("LIMIT"),
          "spend at most LIMIT seconds on proving a single formula", 't').
        add_option("induction", "apply induction on lists", 'o');

      command_line_parser parser(clinterface, argc, argv);

      if (parser.continue_execution()) {
        f_no_check            = 0 < parser.options.count("no-check");
        f_no_marking          = 0 < parser.options.count("no-marking");
        f_generate_invariants = 0 < parser.options.count("generate-invariants");
        f_check_all           = 0 < parser.options.count("check-all");
        f_counter_example     = 0 < parser.options.count("counter-example");
        f_apply_induction     = 0 < parser.options.count("induction");

        if (parser.options.count("invariant")) {
          f_invariant_file_name = parser.option_argument_as< std::string >("invariant");
        }

        if (parser.options.count("print-dot")) {
          f_dot_file_name = parser.option_argument_as< std::string >("print-dot");
        }
        if (parser.options.count("summand")) {
          f_summand_number = parser.option_argument_as< size_t >("summand");

          if (f_summand_number < 1) {
            parser.error("The summand number must be greater than or equal to 1.\n");
          }
          else {
            gsVerboseMsg("Checking confluence of summand number %u.\n", f_summand_number);
          }
        }
        if (parser.options.count("time-limit")) {
          f_time_limit = parser.option_argument_as< size_t >("time-limit");
        }

        f_strategy = parser.option_argument_as< mcrl2::new_data::rewriter::strategy >("rewriter");

        if (parser.options.count("smt-solver")) {
          f_path_eliminator = true;
          f_solver_type     = parser.option_argument_as< SMT_Solver_Type >("smt-solver");
        }

        if (2 < parser.arguments.size()) {
          parser.error("too many file arguments");
        }
        else {
          if (0 < parser.arguments.size()) {
            f_input_file_name = parser.arguments[0];
          }
          if (1 < parser.arguments.size()) {
            f_output_file_name = parser.arguments[1];
          }
        }
      }

      return parser.continue_execution();
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
          throw mcrl2::runtime_error("cannot open input file '" + f_invariant_file_name + "'");
        }

        gsVerboseMsg("parsing input file '%s'...\n", f_invariant_file_name.c_str());

        if (!(f_invariant = parse_data_expr(instream))) {
          instream.close();
          throw mcrl2::runtime_error("parsing failed");
        }

        instream.close();
      }
      else {
        f_invariant = mcrl2::core::detail::gsMakeOpIdTrue();
      }

      specification lps_specification;

      lps_specification.load(f_input_file_name);

      // temporary measure, until the invariant and confluence checkers use the lps framework
      f_lps = (ATermAppl) lps_specification;
        // type checking and data implementation of data expressions use an lps
        // before data implementation
      ATermAppl f_reconstructed_lps = reconstruct_spec(f_lps);

      //typecheck the invariant formula
      ATermAppl process = ATAgetArgument(f_reconstructed_lps, 2);
      assert(mcrl2::core::detail::gsIsLinearProcess(process));
      ATermList vars = ATLgetArgument(process, 1);
      ATermTable var_table = ATtableCreate(63,50);
      for (; !ATisEmpty(vars); vars = ATgetNext(vars)) {
        ATermAppl var = ATAgetFirst(vars);
        ATtablePut(var_table, ATgetArgument(var, 0), ATgetArgument(var, 1));
      }
      f_invariant = type_check_data_expr(f_invariant, mcrl2::core::detail::gsMakeSortIdBool(), f_reconstructed_lps, var_table);
      ATtableDestroy(var_table);
      if (!f_invariant) {
        throw mcrl2::runtime_error("Typechecking of the invariant formula failed.\n");
      }

      //data implement the invariant formula
      f_invariant = implement_data_expr(f_invariant, f_reconstructed_lps);
      if (!f_invariant) {
        throw mcrl2::runtime_error("Data implementation of the invariant formula failed.\n");
      }

      f_lps = specification(f_reconstructed_lps);

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
            f_lps, f_strategy, f_time_limit, f_path_eliminator, f_solver_type, false, false, false, (f_dot_file_name.empty())?0:const_cast < char* > (f_dot_file_name.c_str()));

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
        f_generate_invariants, (f_dot_file_name.empty())?0:const_cast < char* > (f_dot_file_name.c_str()));

      f_lps = v_confluence_checker.check_confluence_and_mark(f_invariant, f_summand_number);
    }

    // --------------------------------------------------------------------------------------------

    /// Writes the LPS with the tau-actions of all confluent tau-summands renamed to ctau to the file
    /// specified by LPS_Conf_Check::f_output_file_name, if the flag LPS_Conf_Check::f_no_marking
    /// is set to false.

    void LPS_Conf_Check::write_result() {
      if (!f_no_marking) {
        specification lps_specification(f_lps);
        lps_specification.save(f_output_file_name);
      }
    }

// Main function ----------------------------------------------------------------------------------

int main(int argc, char* argv[]) {

  MCRL2_ATERM_INIT(argc, argv)

  try {
    LPS_Conf_Check v_lps_conf_check;

    if (v_lps_conf_check.get_options(argc, argv)) {
      v_lps_conf_check.read_input();

      if (v_lps_conf_check.check_invariant()) {
        v_lps_conf_check.check_confluence_and_mark();
        v_lps_conf_check.write_result();
      }
    }
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
