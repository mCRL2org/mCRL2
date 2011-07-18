// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file formcheck.cpp
/// \brief Add your file description here.

#define NAME "formulacheck"
#define AUTHOR "Luc Engelen"

#include <string>
#include <fstream>

#include "mcrl2/data/parse.h"
#include "mcrl2/data/detail/prover/bdd_path_eliminator.h"
#include "mcrl2/data/detail/prover/formula_checker.h"
#include "mcrl2/data/find.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/prover_tool.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::lps;
using namespace mcrl2::data::detail;
using namespace mcrl2::log;

using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

/// \mainpage formcheck
/// \section section_introduction Introduction
/// This document provides information on the internals of the tool.
/// \section section_additional_info Additional information
/// More information about the tool and the classes used can be found in the corresponding tool manual page.
/// \brief The class Form_Check uses an instance of the class Formula_Checker to check whether
/// \brief or not the formula specified by m_input_filename is a tautology or a contradiction.
class formulacheck_tool : public prover_tool< rewriter_tool<input_tool> >
{
  private:
    /// \brief The name of the file containing the data specification.
    /// \brief If this string is 0, a minimal data specification is used.
    std::string f_spec_file_name;

    /// \brief The flag indicating whether or not counter examples are printed each time a formula is encountered
    /// \brief that is neither a contradiction nor a tautology.
    bool f_counter_example;

    /// \brief The flag indicating whether or not witnesses are printed each time a formula is encountered
    /// \brief that is neither a contradiction nor a tautology.
    bool f_witness;

    /// \brief The prefix of the files in dot format that are written each time a condition is encountered that is neither
    /// \brief a contradiction nor a tautology. If the string is 0, no files are written.
    std::string f_dot_file_name;

    /// \brief The maximal number of seconds spent on proving a single confluence condition.
    int f_time_limit;

    /// \brief The flag indicating whether or not a path eliminator is used.
    bool f_path_eliminator;

    /// \brief The flag indicating whether or not induction should be applied.
    bool f_apply_induction;

    typedef prover_tool< rewriter_tool<input_tool> > super;

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);

      f_counter_example = 0 < parser.options.count("counter-example");
      f_apply_induction = 0 < parser.options.count("induction");
      f_witness         = 0 < parser.options.count("witness");

      if (parser.options.count("spec"))
      {
        f_spec_file_name = parser.option_argument_as< std::string >("spec");
      }
      if (parser.options.count("print-dot"))
      {
        f_dot_file_name = parser.option_argument_as< std::string >("print-dot");
      }
      if (parser.options.count("time-limit"))
      {
        f_time_limit = parser.option_argument_as< int >("time-limit");
      }
      if (parser.options.count("smt-solver"))
      {
        f_path_eliminator = true;
      }
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);

      desc.
      add_option("spec", make_mandatory_argument("SPECFILE"),
                 "check the formula against the data types from the LPS or PBES in SPECFILE", 's').
      add_option("counter-example",
                 "display a valuation for which the formula does not hold, in case it is neither a "
                 "contradiction nor a tautology", 'c').
      add_option("witness",
                 "display a valuation for which the formula holds, in case it is neither a "
                 "contradiction nor a tautology", 'w').
      add_option("print-dot", make_mandatory_argument("PREFIX"),
                 "save a .dot file of the resulting BDD if it is impossible to determine whether "
                 "the formula is a contradiction or a tautology; PREFIX will be used as prefix of "
                 "the output files", 'p').
      add_option("time-limit", make_mandatory_argument("LIMIT"),
                 "spend at most LIMIT seconds on proving a single formula", 't').
      add_option("induction", "apply induction on lists", 'o');
    }

    /// \brief Load formula from the file mentioned in infilename, if infilename
    ///        is empty, use std::cin.
    /// \param infilename The name of the input file
    /// \return The formula stored in infilename
    data_expression load_formula(const std::string& infilename, data_specification& specification)
    {
      if (infilename.empty())
      {
        return parse_data_expression(std::cin, specification);
      }
      else
      {
        std::ifstream instream(infilename.c_str());
        if (!instream.is_open())
        {
          throw mcrl2::runtime_error("cannot open input file '" + m_input_filename + "'");
        }

        data_expression formula = parse_data_expression(instream, specification);
        instream.close();

        return formula;
      }
    }

    /// \brief Load a data specification from file.
    /// \param[in] infilename a string representing the input file
    /// \return if infilename is nonempty and if it contains a valid
    ///  LPS, PBES or data specification, the data specification of this LPS or PBES is
    ///  returned;
    ///  if infilename is empty, a minimal data specification is returned
    data_specification load_specification(const std::string& infilename)
    {
      if (!infilename.empty())
      {
        //load data specification from file infilename
        mCRL2log(verbose) << "reading LPS or PBES from '" <<  infilename << "'" << std::endl;
        try
        {
          lps::specification s;
          s.load(infilename);
          return s.data();
        }
        catch (mcrl2::runtime_error&)
          {}

        try
        {
          mcrl2::pbes_system::pbes<> p;
          p.load(infilename);
          return p.data();
        }
        catch (mcrl2::runtime_error&)
          {}

        throw mcrl2::runtime_error("'" + infilename + "' does not contain an LPS, PBES or data specification");
      }
      return data_specification();
    }

  public:
    /// \brief Constructor setting all flags to their default values.
    formulacheck_tool() : super(
        "formulacheck",
        "Luc Engelen",
        "check a boolean formula",
        "Checks whether the boolean formula (an mCRL2 data expression of sort Bool) in "
        "INFILE holds. If INFILE is not present, stdin is used."),
      f_counter_example(false),
      f_witness(false),
      f_time_limit(0),
      f_path_eliminator(false),
      f_apply_induction(false)
    {}

    /// \brief Checks and indicates whether or not the formula specified by
    /// \brief m_input_filename is a tautology or a contradiction.
    /// Reads the list of formulas specified by m_input_filename and the LPS
    /// specified by f_specification_filename. The method determines and indicates whether or not the
    /// formulas in the list are tautologies or contradictions using the data equations of the LPS.
    bool run()
    {
      data_specification specification = load_specification(f_spec_file_name);
      data_expression formula = load_formula(m_input_filename, specification);

      if (formula == data_expression())
      {
        throw mcrl2::runtime_error("no formula in input");
      }

      std::set<data::sort_expression> s;
      data::find_sort_expressions(formula, std::inserter(s, s.end()));
      specification.add_context_sorts(s);

      //check formula
      Formula_Checker v_formula_checker(
        specification, rewrite_strategy(), f_time_limit, f_path_eliminator, solver_type(), f_apply_induction, f_counter_example, f_witness, f_dot_file_name.c_str());
      v_formula_checker.check_formulas(make_list(formula));

      return true;
    }
};

int main(int argc, char* argv[])
{
  MCRL2_ATERM_INIT(argc, argv)
  return formulacheck_tool().execute(argc, argv);
}
