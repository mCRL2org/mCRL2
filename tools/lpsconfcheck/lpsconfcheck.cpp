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

#include "mcrl2/data/parse.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/confluence_checker.h"
#include "mcrl2/lps/invariant_checker.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/prover_tool.h"

using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::data::detail;
using namespace mcrl2::lps;
using namespace mcrl2::lps::detail;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::log;

/// \mainpage lpsconfcheck
/// \section section_introduction Introduction
/// This document provides information on the internals of the tool.
/// \section section_additional_info Additional information
/// More information about the tool and the classes used can be found in the corresponding man files.

/// \brief The class confcheck_tool uses an instance of the class Confluence_Checker to check which
/// \brief tau-summands of an LPS are confluent. The tau-actions of all confluent tau-summands can be
/// \brief renamed to ctau, depending on the flag m_no_marking.

class lpsconfcheck_tool : public prover_tool< rewriter_tool<input_output_tool> >
{
  protected:

    typedef prover_tool< rewriter_tool<input_output_tool> > super;

    /// \brief The name of a file containing an invariant that is used to check confluence.
    /// \brief If this string is 0, the constant true is used as invariant.
    std::string m_invariant_filename;

    /// \brief The number of the summand that is checked for confluence.
    /// \brief If this number is 0, all summands are checked.
    size_t m_summand_number;

    /// \brief The flag indicating if the invariance of resulting expressions should be checked in case a confluence
    /// \brief condition is neither a tautology nor a contradiction.
    bool m_generate_invariants;

    /// \brief The flag indicating whether or not the invariance of the formula as found in the file
    /// \brief m_invariant_filename is checked.
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
    int m_time_limit;

    /// \brief The flag indicating whether or not a path eliminator is used.
    bool m_path_eliminator;

    /// \brief The flag indicating whether or not induction should be applied.
    bool m_apply_induction;

    /// \brief The invariant provided as input.
    /// \brief If no invariant was provided, the constant true is used as invariant.
    data_expression m_invariant;

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
        m_invariant_filename = parser.option_argument_as< std::string >("invariant");
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
          throw parser.error("The summand number must be greater than or equal to 1.\n");
        }
        else
        {
          mCRL2log(verbose) << "Checking confluence of summand number " <<  m_summand_number << "." << std::endl;
        }
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
    lpsconfcheck_tool() : super(
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

      mCRL2log(verbose) << "lpsconfcheck parameters:" << std::endl;
      mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  output file:        " << m_output_filename << std::endl;
      mCRL2log(verbose) << "  data rewriter:      " << m_rewrite_strategy << std::endl;

      lps::specification specification;

      specification.load(m_input_filename);

      if (!m_invariant_filename.empty())
      {
        std::ifstream instream(m_invariant_filename.c_str());

        if (!instream.is_open())
        {
          throw mcrl2::runtime_error("cannot open input file '" + m_invariant_filename + "'");
        }

        mCRL2log(verbose) << "parsing input file '" <<  m_invariant_filename << "'..." << std::endl;

        m_invariant = parse_data_expression(instream, specification.data());

        instream.close();
      }

      if (check_invariant(specification))
      {
        Confluence_Checker v_confluence_checker(
          specification, rewrite_strategy(),
          m_time_limit, m_path_eliminator, solver_type(),
          m_apply_induction, m_no_marking, m_check_all,
          m_counter_example, m_generate_invariants, m_dot_file_name);

        specification = lps::specification(v_confluence_checker.check_confluence_and_mark(m_invariant, m_summand_number));

        if (!m_no_marking)
        {
          specification.save(m_output_filename);
        }
      }

      return true;
    }

    /// \brief Checks whether or not the invariant holds, if
    /// Checks if the formula in the file m_invariant_filename is an invariant,
    /// if the flag m_no_check is set to false and
    /// m_invariant_filename differs from 0.
    /// \return true, if the invariant holds or no invariant is specified.
    ///         false, if the invariant does not hold.
    bool check_invariant(lps::specification const& specification) const
    {
      if (!m_invariant_filename.empty())
      {
        if (!m_no_check)
        {
          Invariant_Checker v_invariant_checker(specification, rewrite_strategy(), m_time_limit, m_path_eliminator, solver_type(), false, false, false, m_dot_file_name);

          return v_invariant_checker.check_invariant(m_invariant);
        }
        else
        {
          mCRL2log(warning) << "The invariant is not checked; it may not hold for this LPS." << std::endl;
        }
      }

      return true;
    }

};

int main(int argc, char* argv[])
{
  return lpsconfcheck_tool().execute(argc, argv);
}
