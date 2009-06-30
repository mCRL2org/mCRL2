// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsconfcheck.cpp

#include "boost.hpp" // precompiled headers

#include <string>
#include <fstream>

#include "mcrl2/data/parser.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/confluence_checker.h"
#include "mcrl2/lps/invariant_checker.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/prover_tool.h"
#include "mcrl2/utilities/squadt_tool.h"

using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::lps;
using namespace mcrl2::data::detail;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

/// \mainpage lpsconfcheck
/// \section section_introduction Introduction
/// This document provides information on the internals of the tool.
/// \section section_additional_info Additional information
/// More information about the tool and the classes used can be found in the corresponding man files.

/// \brief The class confcheck_tool uses an instance of the class Confluence_Checker to check which
/// \brief tau-summands of an LPS are confluent. The tau-actions of all confluent tau-summands can be
/// \brief renamed to ctau, depending on the flag m_no_marking.

class confcheck_tool : public squadt_tool< prover_tool< rewriter_tool<input_output_tool> > >
{
  private:
    /// \brief The name of a file containing an invariant that is used to check confluence.
    /// \brief If this string is 0, the constant true is used as invariant.
    std::string m_invariant_file_name;

    /// \brief The number of the summand that is checked for confluence.
    /// \brief If this number is 0, all summands are checked.
    size_t m_summand_number;

    /// \brief The flag indicating if the invariance of resulting expressions should be checked in case a confluence
    /// \brief condition is neither a tautology nor a contradiction.
    bool m_generate_invariants;

    /// \brief The flag indicating whether or not the invariance of the formula as found in the file
    /// \brief m_invariant_file_name is checked.
    bool m_no_check;

    /// \brief The flag indicating whether or not the tau-actions of the confluent summands should be renamed to ctau.
    bool m_no_marking;

    /// \brief The flag indicating whether or not the confluence of a tau-summand regarding all other summands is checked.
    bool m_check_all;

    /// \brief The flag indicating whether or not counter examples are printed each time a condition is encountered
    /// \brief that is neither a contradiction nor a tautology.
    bool m_counter_example;

    /// \brief The prefix of the files in dot format that are written each time a condition is encountered that is neither
    /// \brief a contradiction nor a tautology. If the string is empty, no files are written.
    std::string m_dot_file_name;

    /// \brief The maximal number of seconds spent on proving a single confluence condition.
    size_t m_time_limit;

    /// \brief The flag indicating whether or not a path eliminator is used.
    bool m_path_eliminator;

    /// \brief The flag indicating whether or not induction should be applied.
    bool m_apply_induction;

    /// \brief The invariant provided as input.
    /// \brief If no invariant was provided, the constant true is used as invariant.
    data_expression m_invariant;

    typedef squadt_tool< prover_tool< rewriter_tool<input_output_tool> > > super;

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);

      m_no_check            = 0 < parser.options.count("no-check");
      m_no_marking          = 0 < parser.options.count("no-marking");
      m_generate_invariants = 0 < parser.options.count("generate-invariants");
      m_check_all           = 0 < parser.options.count("check-all");
      m_counter_example     = 0 < parser.options.count("counter-example");
      m_apply_induction     = 0 < parser.options.count("induction");

      if (parser.options.count("invariant"))
      {
        m_invariant_file_name = parser.option_argument_as< std::string >("invariant");
      }

      if (parser.options.count("print-dot"))
      {
        m_dot_file_name = parser.option_argument_as< std::string >("print-dot");
      }
      if (parser.options.count("summand"))
      {
        m_summand_number = parser.option_argument_as< size_t >("summand");

        if (m_summand_number < 1)
        {
          parser.error("The summand number must be greater than or equal to 1.\n");
        }
        else
        {
          gsVerboseMsg("Checking confluence of summand number %u.\n", m_summand_number);
        }
      }
      if (parser.options.count("time-limit"))
      {
        m_time_limit = parser.option_argument_as< size_t >("time-limit");
      }

      if (parser.options.count("smt-solver"))
      {
        m_path_eliminator = true;
      }
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.
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
    }

  public:
    /// \brief Constructor setting all flags to their default values.
    confcheck_tool() : super(
        "lpsconfcheck",
        "Luc Engelen",
        "mark confluent tau-summands of an LPS",
        "Checks which tau-summands of the mCRL2 LPS in INFILE are confluent, marks them by "
        "renaming them to ctau, and write the result to OUTFILE. If INFILE is not present "
        "stdin is used. If OUTFILE is not present, stdout is used."),
    m_summand_number(0),
    m_generate_invariants(false),
    m_no_check(false),
    m_no_marking(false),
    m_check_all(false),
    m_counter_example(false),
    m_dot_file_name(""),
    m_time_limit(0),
    m_path_eliminator(false),
    m_apply_induction(false),
    m_invariant(mcrl2::data::sort_bool::true_())
  {}

  bool run()
  {
    lps::specification specification;

    specification.load(m_input_filename);

    if (!m_invariant_file_name.empty())
    {
      std::ifstream instream(m_invariant_file_name.c_str());

      if (!instream.is_open())
      {
        throw mcrl2::runtime_error("cannot open input file '" + m_invariant_file_name + "'");
      }

      gsVerboseMsg("parsing input file '%s'...\n", m_invariant_file_name.c_str());

      m_invariant = parse_data_expression(instream, specification.data());

      instream.close();
    }

    if (check_invariant(specification))
    {
      Confluence_Checker v_confluence_checker(
        specification, rewrite_strategy(), m_time_limit, m_path_eliminator, solver_type(), m_apply_induction, m_no_marking, m_check_all, m_counter_example,
        m_generate_invariants, m_dot_file_name);

      specification = lps::specification(v_confluence_checker.check_confluence_and_mark(m_invariant, m_summand_number));

      if (!m_no_marking)
      {
        specification.save(m_output_filename);
      }
    }

    return true;
  }

  /// \brief Checks whether or not the invariant holds, if
  /// Checks if the formula in the file m_invariant_file_name is an invariant,
  /// if the flag m_no_check is set to false and
  /// m_invariant_file_name differs from 0.
  /// \return true, if the invariant holds or no invariant is specified.
  ///         false, if the invariant does not hold.
  bool check_invariant(lps::specification const& specification) const
  {
    if (!m_invariant_file_name.empty()) {
      if (!m_no_check) {
        Invariant_Checker v_invariant_checker(specification, rewrite_strategy(), m_time_limit, m_path_eliminator, solver_type(), false, false, false, m_dot_file_name);

        return v_invariant_checker.check_invariant(m_invariant);
      }
      else {
        gsWarningMsg("The invariant is not checked; it may not hold for this LPS.\n");
      }
    }

    return true;
  }

#ifdef ENABLE_SQUADT_CONNECTIVITY
#define invariant_file_for_input = "invariant_in";

#define option_generate_invariants = "generate_invariants";
#define option_check_invariant     = "check_invariant";
#define option_mark_tau            = "mark_tau";
#define option_check_combinations  = "check_combinations";
#define option_counter_example     = "counter_example";
#define option_induction_on_lists  = "induction_on_lists";
#define option_invariant           = "invariant";
#define option_time_limit          = "time_limit";
#define option_rewrite_strategy    = "rewrite_strategy";

  void set_capabilities(tipi::tool::capabilities& c) const
  {
    c.add_input_configuration("main-input", tipi::mime_type("mcrl2", tipi::mime_type::text),
                                                            tipi::tool::category::transformation);
  }


  void user_interactive_configuration(tipi::configuration& c)
  {
    using namespace tipi;
    using namespace tipi::layout;
    using namespace tipi::datatype;
    using namespace tipi::layout::elements;

    synchronise_with_configuration(configuration);

    std::string infilename = c.get_input("main-input").location();

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

    layout::vertical_box& m = d.create< vertical_box >().set_default_margins(margins(0,5,0,5));

    add_solver_option(d, m);
    add_rewrite_option(d, m);

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

    send_display_layout(d.manager(m));

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

    if (!c.output_exists("main-output") && !c.get_option_argument< bool >(option_mark_tau)) {
      c.add_output("main-output", tipi::mime_type("lps", tipi::mime_type::application), c.get_output_name(".lps"));
    }

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

    // let squadt_tool update configuration for rewriter and input/output files
    update_configuration(configuration);

    send_clear_display();
  }

  bool check_configuration(tipi::configuration const& c) const
  {
    return c.input_exists("main-input");
  }

  bool perform_task(tipi::configuration& c)
  {
    using namespace boost;
    using namespace tipi;
    using namespace tipi::layout;
    using namespace tipi::datatype;
    using namespace tipi::layout::elements;

    // Let squadt_tool update configuration for rewriter and add output file configuration
    synchronise_with_configuration(configuration);

    // TODO finish
    run();

    send_display_layout(d);

    return true;
  }
#endif
};

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)
  return confcheck_tool().execute(argc, argv);
}
