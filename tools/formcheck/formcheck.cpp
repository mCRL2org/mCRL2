// Author(s): Luc Engelen
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file formcheck.cpp
/// \brief Add your file description here.

#define NAME "formcheck"
#define AUTHOR "Luc Engelen"

#include <string>
#include <fstream>

#include "mcrl2/formula_checker.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/data/prover/bdd_path_eliminator.h"
#include "mcrl2/core/struct.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/detail/parse.h"
#include "mcrl2/core/detail/typecheck.h"
#include "mcrl2/core/detail/data_implementation.h"
#include "mcrl2/core/detail/data_reconstruct.h"
#include "mcrl2/utilities/aterm_ext.h"
#include "mcrl2/utilities/command_line_interface.h" // after messaging.h, rewrite.h and bdd_path_eliminator.h

using namespace ::mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2;

  /// \mainpage formcheck
  /// \section section_introduction Introduction
  /// This document provides information on the internals of the tool.
  /// \section section_additional_info Additional information
  /// More information about the tool and the classes used can be found in the corresponding tool manual page.

// Class LPS_Form_Check ---------------------------------------------------------------------------

  /// \brief The class LPS_Form_Check uses an instance of the class Formula_Checker to check whether
  /// \brief or not the formula specified by LPS_Form_Check::f_formula_file_name is a tautology or a contradiction.

  class LPS_Form_Check {
    private:
      /// \brief The name of the file containing the formula that is checked.
      std::string f_formula_file_name;

      /// \brief The name of the file containing the LPS.
      /// \brief If this string is 0, the input is read from stdin.
      std::string f_lps_file_name;

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

    public:
      /// \brief Constructor setting all flags to their default values.
      LPS_Form_Check();

      /// \brief Parses command line options
      void get_options(int a_argc, char* a_argv[]);

      /// \brief Checks and indicates whether or not the formula specified by
      /// \brief LPS_Form_Check::f_formula_file_name is a tautology or a contradiction.
      void check_formula();
  };

  // Class LPS_Form_Check - Functions declared private --------------------------------------------

  // Class LPS_Form_Check - Functions declared public ---------------------------------------------

    LPS_Form_Check::LPS_Form_Check() {
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

    void LPS_Form_Check::get_options(int argc, char* argv[]) {
      interface_description clinterface(argv[0], NAME, AUTHOR, "[OPTION]... --formula=FORMFILE [INFILE]\n"
        "Checks whether the boolean formula (an mCRL2 data expression of sort Bool) in "
        "FORMFILE holds for the data specification of the linear process specification "
        "(LPS) in INFILE. If INFILE is not present, stdin is used.");

      clinterface.add_rewriting_options();
      clinterface.add_prover_options();

      clinterface.
        add_option("formula", make_mandatory_argument("FORMFILE"), 
          "use the formula in FORMFILE as input", 'f').
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

      f_counter_example = 0 < parser.options.count("counter-example");
      f_apply_induction = 0 < parser.options.count("induction");
      f_witness         = 0 < parser.options.count("witness");

      if (parser.options.count("formula")) {
        f_formula_file_name = parser.option_argument_as< std::string >("formula");
      }
      else {
        parser.error("a formula file must be specified using the option --formula=FORMFILE.");
      }
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

      if (0 < parser.arguments.size()) {
        f_lps_file_name = parser.arguments[0];
      }
      if (1 < parser.arguments.size()) {
        parser.error("too many file arguments");
      }
    }

    // --------------------------------------------------------------------------------------------

    /// Reads the list of formulas specified by LPS_Form_Check::f_formulas_file_name and the LPS
    /// specified by LPS_Form_Check::f_file_name. The method determines and indicates whether or not the
    /// formulas in the list are tautologies or contradictions using the data equations of the LPS.

    void LPS_Form_Check::check_formula() {

      //Load LPS
      lps::specification lps_specification;
      lps_specification.load((f_lps_file_name.empty())?"-":f_lps_file_name.c_str());
      if (!lps_specification.is_well_typed()) {
        throw std::runtime_error("invalid mCRL2 LPS read from " + std::string((f_lps_file_name.empty())?"stdin":f_lps_file_name.c_str()));
      }

      // typechecking and data implementation use a specification before data
      // implementation.
      ATermAppl v_reconstructed_lps = reconstruct_spec(lps_specification);

      gsVerboseMsg("parsing formula file '%s'...\n", f_formula_file_name.c_str());
      //open the formula from f_formula_file_name
      std::ifstream instream(f_formula_file_name.c_str());
      if (!instream.is_open()) {
        throw std::runtime_error("cannot open formula file '" + f_formula_file_name + "'");
      }
      //parse the formula
      ATermAppl f_formula = parse_data_expr(instream);
      instream.close();
      if(!f_formula){
        throw std::runtime_error("cannot parse formula from '" + f_formula_file_name + "'");
      }
      //typecheck the formula
      f_formula = type_check_data_expr(f_formula, gsMakeSortIdBool(), v_reconstructed_lps);
      if(!f_formula){
        throw std::runtime_error("type checking formula from '" + f_formula_file_name + "' failed");
      }
      //implement data in the formula
      f_formula = implement_data_data_expr(f_formula,v_reconstructed_lps);
      if(!f_formula){
        throw std::runtime_error("implementation of data types in the formula from '" + f_formula_file_name + "' failed");
      }

      //check formula
      Formula_Checker v_formula_checker(
        ATAgetArgument(v_reconstructed_lps,0), f_strategy, f_time_limit, f_path_eliminator, f_solver_type, f_apply_induction, f_counter_example, f_witness, f_dot_file_name.c_str());
      v_formula_checker.check_formulas(ATmakeList1((ATerm) f_formula));
    }

// Main function ----------------------------------------------------------------------------------

  int main(int argc, char* argv[])
  {
    MCRL2_ATERM_INIT(argc, argv)

    try {
      LPS_Form_Check v_lps_form_check;

      v_lps_form_check.get_options(argc, argv);
      v_lps_form_check.check_formula();

      return EXIT_SUCCESS;
    }
    catch (std::exception& e) {
      std::cerr << e.what() << std::endl;
    }

    return EXIT_FAILURE;
  }
