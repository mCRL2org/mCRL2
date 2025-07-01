// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsinvelm.cpp
/// \brief Add your file description here.

#include "mcrl2/lps/invelm_algorithm.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/data/prover_tool.h"
#include "mcrl2/lps/io.h"
#include "mcrl2/lps/stochastic_specification.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::data::detail;
using namespace mcrl2::log;
using namespace mcrl2::lps;

using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

using mcrl2::data::tools::rewriter_tool;
using mcrl2::data::tools::prover_tool;

/// \mainpage lpsinvelm
/// \section section_introduction Introduction
/// This document provides information on the internals of the tool.
/// \section section_additional_info Additional information
/// More information about the tool and the classes used can be found in the corresponding man files.

/// \brief The class invelm_tool takes an invariant and an LPS, and simplifies this LPS using
/// \brief the invariant.
class lpsinvelm_tool : public prover_tool< rewriter_tool<input_output_tool> >
{
  private:
    /// \brief The name of the file containing the invariant.
    std::string m_invariant_file_name;

    /// \brief The flag indicating whether or not the invariance of the formula as found in
    /// \brief invelm_tool::m_invariant_file_name is checked.
    bool m_no_check;

    /// \brief The flag indicating whether or not elimination or simplification is applied.
    bool m_no_elimination;

    /// \brief The flag indicating whether or not the conditions of all summands will be simplified.
    /// \brief If this flag is set to false, only the summands whose conditions violate the invariant
    /// \brief are eliminated.
    bool m_simplify_all;

    /// \brief The flag indicating whether or not all violations encountered while checking the invariant
    /// \brief are reported. If this flag is set to false, the checking stops as soon as a violation is
    /// \brief encountered.
    bool m_all_violations;

    /// \brief The flag indicating whether or not counter examples are printed each time a summand is
    /// \brief encountered whose condition in conjunction with the invariant is not a tautology or a
    /// \brief contradiction.
    bool m_counter_example;

    /// \brief The prefix of the files in dot format that are written each time a summand is
    /// \brief encountered whose condition in conjunction with the invariant is not a tautology or a
    /// \brief contradiction.
    std::string m_dot_file_name;

    /// \brief The maximal number of seconds spent on proving the conjunction of the invariant
    /// \brief and a summands' condition
    int m_time_limit;

    /// \brief The flag indicating whether or not a path eliminator is used.
    bool m_path_eliminator;

    /// \brief The flag indicating whether or not induction should be applied.
    bool m_apply_induction;

    /// \brief The invariant provided as input.
    data_expression m_invariant;

    typedef prover_tool< rewriter_tool<input_output_tool> > super;

  protected:
    std::string synopsis() const
    {
      return "[OPTION]... --invfile=INVFILE [INFILE [OUTFILE]]\n";
    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);

      m_no_check        = 0 < parser.options.count("no-check");
      m_no_elimination  = 0 < parser.options.count("no-elimination");
      m_simplify_all    = 0 < parser.options.count("simplify-all")||m_no_elimination; // If no_elimation is true all summands must be simplified.
      m_all_violations  = 0 < parser.options.count("all-violations");
      m_counter_example = 0 < parser.options.count("counter-example");
      m_apply_induction = 0 < parser.options.count("induction");

      if (parser.options.count("invariant"))
      {
        m_invariant_file_name = parser.option_argument_as< std::string >("invariant");
      }

      if (parser.options.count("print-dot"))
      {
        m_dot_file_name = parser.option_argument_as< std::string >("print-dot");
      }
      if (parser.options.count("time-limit"))
      {
        m_time_limit = parser.option_argument_as< int >("time-limit");
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
      add_option("invariant", make_file_argument("INVFILE"),
                 "use the boolean formula (an mCRL2 data expression of sort Bool) in INVFILE as invariant", 'i').
      add_option("no-check",
                 "do not check if the invariant holds before applying the invariant", 'n').
      add_option("no-elimination",
                 "do not eliminate or simplify summands, but add the invariant to each condition. This implies setting the flag --simplify-all.", 'e').
      add_option("simplify-all",
                 "simplify the conditions of all summands, instead of just eliminating the summands "
                 "whose conditions in conjunction with the invariant are contradictions", 'l').
      add_option("all-violations",
                 "do not terminate as soon as a single violation of the invariant is found, but "
                 "report all violations instead", 'y').
      add_option("counter-example",
                 "display a valuation indicating why the invariant could possibly be violated "
                 "if it is uncertain whether a summand violates the invariant", 'c').
      add_option("print-dot", make_mandatory_argument("PREFIX"),
                 "save a .dot file of the resulting BDD if it is impossible to determine "
                 "whether a summand violates the invariant; PREFIX will be used as prefix "
                 "of the output files", 'p').
      add_option("time-limit", make_mandatory_argument("LIMIT"),
                 "spend at most LIMIT seconds on proving a single formula", 't').
      add_option("induction", "apply induction on lists", 'o');
    }

  public:
    /// \brief Constructor setting all flags to their default values.
    lpsinvelm_tool() : super(
        "lpsinvelm",
        "Luc Engelen",
        "check invariants and use these to simplify or eliminate summands of an LPS",
        "Checks whether the boolean formula (an mCRL2 data expression of sort Bool) provided "
        "as invariant is an invariant of the linear process specification (LPS) in INFILE. "
        "If this is the case, the tool eliminates all summands of the LPS whose condition "
        "violates the invariant, and writes the result to OUTFILE. "
        "If INFILE is present, stdin is used. If OUTFILE is not present, stdout is used.\n"
        "\n"
        "The tool can also be used to simplify the conditions of the summands of the given LPS."),
      m_no_check(false),
      m_no_elimination(false),
      m_simplify_all(false),
      m_all_violations(false),
      m_counter_example(false),
      m_time_limit(0),
      m_path_eliminator(false),
      m_apply_induction(false)
    {}

    bool run()
    {
      stochastic_specification spec;
      data::data_expression invariant;

      load_lps(spec, m_input_filename);

      if (!m_invariant_file_name.empty())
      {
        std::ifstream instream(m_invariant_file_name.c_str());

        if (!instream.is_open())
        {
          throw mcrl2::runtime_error("cannot open input file '" + m_invariant_file_name + "'");
        }

        mCRL2log(log::verbose) << "parsing input file '" <<  m_invariant_file_name << "'..." << std::endl;

        data::variable_list& parameters=spec.process().process_parameters();
        invariant = data::parse_data_expression(instream, parameters, spec.data());

        instream.close();
      }
      else
      {
        throw mcrl2::runtime_error("A file containing an invariant must be specified using the option --invariant=INVFILE.");
      }

      if (m_no_check)
      {
        mCRL2log(log::warning) << "The invariant is not checked; it may not hold for this LPS." << std::endl;
      }
      else
      {
        mcrl2::lps::detail::Invariant_Checker<stochastic_specification> v_invariant_checker(spec,
                                              m_rewrite_strategy,
                                              m_time_limit,
                                              m_path_eliminator,
                                              m_solver_type,
                                              m_apply_induction,
                                              m_counter_example,
                                              m_all_violations,
                                              m_dot_file_name);

        if (!v_invariant_checker.check_invariant(invariant))
        {
          return false; // The invariant was checked and found invalid.
        }
      }

      invelm_algorithm<stochastic_specification> algorithm(spec,
                                  rewrite_strategy(),
                                  m_time_limit,
                                  m_path_eliminator,
                                  m_solver_type,
                                  m_apply_induction,
                                  m_simplify_all);
      algorithm.run(invariant, !m_no_elimination);
      save_lps(spec, output_filename());
      return true;
    }
};

int main(int argc, char* argv[])
{
  return lpsinvelm_tool().execute(argc, argv);
}
