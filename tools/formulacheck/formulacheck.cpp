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

#include "mcrl2/formula_checker.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/prover/bdd_path_eliminator.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/detail/aterm_io.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/core/typecheck.h"
#include "mcrl2/core/data_implementation.h"
#include "mcrl2/core/data_reconstruct.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/command_line_rewriting.h"
#include "mcrl2/utilities/command_line_proving.h"
#include "mcrl2/utilities/command_line_messaging.h"

using namespace mcrl2::utilities;
using namespace mcrl2::core;

  /// \mainpage formcheck
  /// \section section_introduction Introduction
  /// This document provides information on the internals of the tool.
  /// \section section_additional_info Additional information
  /// More information about the tool and the classes used can be found in the corresponding tool manual page.

// Class Form_Check ---------------------------------------------------------------------------

  /// \brief The class Form_Check uses an instance of the class Formula_Checker to check whether
  /// \brief or not the formula specified by Form_Check::f_formula_file_name is a tautology or a contradiction.

  class Form_Check {
    private:
      /// \brief The name of the file containing the formula that is checked.
      std::string f_formula_file_name;

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

      /// \brief The rewrite strategy used by the rewriter.
      RewriteStrategy f_strategy;

      /// \brief The maximal number of seconds spent on proving a single confluence condition.
      size_t f_time_limit;

      /// \brief The flag indicating whether or not a path eliminator is used.
      bool f_path_eliminator;

      /// \brief The type of SMT solver used by the path eliminator.
      SMT_Solver_Type f_solver_type;

      /// \brief The flag indicating whether or not induction should be applied.
      bool f_apply_induction;

      /// \brief Load a data specification from file.
      /// \param[in] infilename a string representing the input file
      /// \return if infilename is nonempty and if it contains a valid
      ///  LPS or PBES, the data specification of this LPS or PBES is
      ///  returned;
      ///  if infilename is empty, a minimal data specification is returned
      mcrl2::data::data_specification load_specification(const std::string &infilename);

    public:
      /// \brief Constructor setting all flags to their default values.
      Form_Check();

      /// \brief Parses command line options
      bool get_options(int a_argc, char* a_argv[]);

      /// \brief Checks and indicates whether or not the formula specified by
      /// \brief Form_Check::f_formula_file_name is a tautology or a contradiction.
      void check_formula();
  };

  // Class Form_Check - Functions declared private --------------------------------------------

  // Class Form_Check - Functions declared public ---------------------------------------------

    Form_Check::Form_Check() {
      f_counter_example = false;
      f_witness = false;
      f_strategy = GS_REWR_JITTY;
      f_time_limit = 0;
      f_path_eliminator = false;
      f_solver_type = solver_type_ario;
      f_apply_induction = false;
    }

    // --------------------------------------------------------------------------------------------

    /// Sets the flags of the class according to the command line options passed.
    /// \param argc is the number of arguments passed on the command line
    /// \param argv is an array of all arguments passed on the command line

    bool Form_Check::get_options(int argc, char* argv[]) {
      interface_description clinterface(argv[0], NAME, AUTHOR,
        "check a boolean formula",
        "[OPTION]... [INFILE]\n",
        "Checks whether the boolean formula (an mCRL2 data expression of sort Bool) in "
        "INFILE holds. If INFILE is not present, stdin is used.");

      clinterface.add_rewriting_options();
      clinterface.add_prover_options();

      clinterface.
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

      command_line_parser parser(clinterface, argc, argv);

      if (parser.continue_execution()) {
        f_counter_example = 0 < parser.options.count("counter-example");
        f_apply_induction = 0 < parser.options.count("induction");
        f_witness         = 0 < parser.options.count("witness");

        if (parser.options.count("print-dot")) {
          f_dot_file_name = parser.option_argument_as< std::string >("print-dot");
        }
        if (parser.options.count("time-limit")) {
          f_time_limit = parser.option_argument_as< size_t >("time-limit");
        }

        f_strategy = parser.option_argument_as< RewriteStrategy >("rewriter");

        if (parser.options.count("smt-solver")) {
          f_path_eliminator = true;
          f_solver_type     = parser.option_argument_as< SMT_Solver_Type >("smt-solver");
        }

        if (parser.options.count("spec")) {
          f_spec_file_name = parser.option_argument_as< std::string >("spec");
        }

        if (0 < parser.arguments.size()) {
          f_formula_file_name = parser.arguments[0];
        }
        if (1 < parser.arguments.size()) {
          parser.error("too many file arguments");
        }
      }

      return parser.continue_execution();
    }

    // --------------------------------------------------------------------------------------------

    mcrl2::data::data_specification Form_Check::load_specification(const std::string &infilename)
    {
      ATermAppl raw_specification;
      if (infilename.empty()) {
        //use empty data specification
        raw_specification = implement_data_data_spec(mcrl2::core::detail::gsMakeEmptyDataSpec());
      } else {
        //load data specification from file infilename
        gsVerboseMsg("reading LPS or PBES from '%s'\n", infilename.c_str());
        raw_specification = (ATermAppl) mcrl2::core::detail::load_aterm(infilename);
        if (!mcrl2::core::detail::gsIsLinProcSpec(raw_specification) && !mcrl2::core::detail::gsIsPBES(raw_specification)) {
          throw mcrl2::runtime_error("'" + infilename + "' does not contain an LPS or PBES");
        }
        raw_specification = ATAgetArgument(raw_specification, 0);
      }
      mcrl2::data::data_specification spec(raw_specification);
      return spec;
    }

    // --------------------------------------------------------------------------------------------

    /// Reads the list of formulas specified by Form_Check::f_formulas_file_name and the LPS
    /// specified by Form_Check::f_file_name. The method determines and indicates whether or not the
    /// formulas in the list are tautologies or contradictions using the data equations of the LPS.

    void Form_Check::check_formula() {

      //Load data specification
      mcrl2::data::data_specification spec = load_specification(f_spec_file_name);

      // typechecking and data implementation use a specification before data
      // implementation.
      ATermAppl v_reconstructed_spec = reconstruct_spec(spec);

      ATermAppl f_formula;
      //parse formula
      if (f_formula_file_name.empty()) {
        //parse formula from stdin
        gsVerboseMsg("parsing formula file stdin...\n");
        f_formula = parse_data_expr(std::cin);
      } else {
        //parse formula from f_formula_file_name
        gsVerboseMsg("parsing formula file '%s'...\n", f_formula_file_name.c_str());
        std::ifstream instream(f_formula_file_name.c_str());
        if (!instream.is_open()) {
          throw mcrl2::runtime_error("cannot open formula file '" + f_formula_file_name + "'");
        }
        f_formula = parse_data_expr(instream);
        instream.close();
      }
      if(!f_formula){
        throw mcrl2::runtime_error("parsing formula from " + (f_formula_file_name.empty()?"stdin":"'" + f_formula_file_name + "'") + "failed");
      }
      //typecheck the formula
      f_formula = type_check_data_expr(f_formula, mcrl2::core::detail::gsMakeSortIdBool(), v_reconstructed_spec);
      if(!f_formula){
        throw mcrl2::runtime_error("type checking formula from '" + (f_formula_file_name.empty()?"stdin":"'" + f_formula_file_name + "'") + "' failed");
      }
      //implement data in the formula
      f_formula = implement_data_data_expr(f_formula,v_reconstructed_spec);
      if(!f_formula){
        throw mcrl2::runtime_error("implementation of data types in the formula from '" + (f_formula_file_name.empty()?"stdin":"'" + f_formula_file_name + "'") + "' failed");
      }

      //update spec with the contents of v_reconstructed_spec
      spec = mcrl2::data::data_specification(v_reconstructed_spec);

      //check formula
      Formula_Checker v_formula_checker(
        spec, f_strategy, f_time_limit, f_path_eliminator, f_solver_type, f_apply_induction, f_counter_example, f_witness, f_dot_file_name.c_str());
      v_formula_checker.check_formulas(ATmakeList1((ATerm) f_formula));
    }

// Main function ----------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
  MCRL2_ATERM_INIT(argc, argv)

  try {
    Form_Check v_form_check;

    if (v_form_check.get_options(argc, argv)) {
      v_form_check.check_formula();
    }
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
