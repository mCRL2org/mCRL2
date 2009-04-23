// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsinvelm.cpp
/// \brief Add your file description here.

#include "boost.hpp" // precompiled headers

#define NAME "lpsinvelm"
#define AUTHOR "Luc Engelen"

#include <string>
#include <fstream>

#include "mcrl2/core/parse.h"
#include "mcrl2/core/typecheck.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/data/detail/data_implementation.h"
#include "mcrl2/data/detail/data_reconstruct.h"
#include "mcrl2/data/detail/prover/bdd_path_eliminator.h"
#include "mcrl2/lps/linear_process.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/invariant_eliminator.h"
#include "mcrl2/lps/invariant_checker.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/command_line_messaging.h"
#include "mcrl2/utilities/command_line_rewriting.h"
#include "mcrl2/utilities/command_line_proving.h"

using namespace mcrl2::core;
using namespace mcrl2::data::detail;

  /// \mainpage lpsinvelm
  /// \section section_introduction Introduction
  /// This document provides information on the internals of the tool.
  /// \section section_additional_info Additional information
  /// More information about the tool and the classes used can be found in the corresponding man files.

// Class LPS_Inv_Elm --------------------------------------------------------------------------

  /// \brief The class LPS_Inv_Elm takes an invariant and an LPS, and simplifies this LPS using
  /// \brief the invariant.

  class LPS_Inv_Elm {
    private:
      /// \brief The name of the file containing the invariant.
      std::string f_invariant_file_name;

      /// \brief The name of the file containing the LPS.
      /// \brief If this string is 0, the input is read from stdin.
      std::string f_lps_file_name;

      /// \brief The name of the file the LPS is written to.
      /// \brief If this string is 0, the output is written to stdout.
      std::string f_output_file_name;

      /// \brief The number of the summand that is eliminated or simplified. If this number is 0,
      /// \brief all summands will be simplified or eliminated.
      size_t f_summand_number;

      /// \brief The flag indicating whether or not the invariance of the formula as found in
      /// \brief LPS_Inv_Elm::f_invariant_file_name is checked.
      bool f_no_check;

      /// \brief The flag indicating whether or not elimination or simplification is applied.
      bool f_no_elimination;

      /// \brief The flag indicating whether or not the conditions of all summands will be simplified.
      /// \brief If this flag is set to false, only the summands whose conditions violate the invariant
      /// \brief are eliminated.
      bool f_simplify_all;

      /// \brief The flag indicating whether or not all violations encountered while checking the invariant
      /// \brief are reported. If this flag is set to false, the checking stops as soon as a violation is
      /// \brief encountered.
      bool f_all_violations;

      /// \brief The flag indicating whether or not counter examples are printed each time a summand is
      /// \brief encountered whose condition in conjunction with the invariant is not a tautology or a
      /// \brief contradiction.
      bool f_counter_example;

      /// \brief The prefix of the files in dot format that are written each time a summand is
      /// \brief encountered whose condition in conjunction with the invariant is not a tautology or a
      /// \brief contradiction.
      std::string f_dot_file_name;

      /// \brief The flag indicating whether or not a path eliminator is used.
      bool f_path_eliminator;

      /// \brief The type of SMT solver used by the path eliminator.
      SMT_Solver_Type f_solver_type;

      /// \brief The rewrite strategy used by the rewriter.
      mcrl2::data::rewriter::strategy f_strategy;

      /// \brief The flag indicating whether or not induction should be applied.
      bool f_apply_induction;

      /// \brief The maximal number of seconds spent on proving the conjunction of the invariant
      /// \brief and a summands' condition
      size_t f_time_limit;

      /// \brief The LPS provided as input.
      ATermAppl f_lps;

      /// \brief The invariant provided as input.
      ATermAppl f_invariant;

    public:
      /// \brief Constructor setting all flags to their default values.
      LPS_Inv_Elm();

      /// \brief Parses command line options
      bool get_options(int argc, char* argv[]);

      /// \brief Reads an LPS and an invariant from the specified input sources.
      void read_input();

      /// \brief Checks whether or not the invariant holds, if
      /// \brief LPS_Inv_Elm::f_invariant_file_name differs from 0 and
      /// \brief LPS_Inv_Elm::f_no_check is set to false.
      bool check_invariant();

      /// \brief Simplifies or eliminates summands of the LPS, if the flag
      /// \brief LPS_Inv_Elm::f_no_elimination is set to false.
      void simplify();

      /// \brief Writes the resulting LPS to the preferred output.
      void write_result();
   };

  // Class LPS_Inv_Elm - Functions declared private --------------------------------------------


    // --------------------------------------------------------------------------------------------

  // Class LPS_Inv_Elm - Functions declared public ----------------------------------------------

    LPS_Inv_Elm::LPS_Inv_Elm() {
      f_summand_number = 0;
      f_no_check = false;
      f_no_elimination = false;
      f_simplify_all = false;
      f_all_violations = false;
      f_counter_example = false;
      f_strategy = mcrl2::data::rewriter::jitty;
      f_time_limit = 0;
      f_path_eliminator = false;
      f_solver_type = solver_type_ario;
      f_apply_induction = false;
    }

    // --------------------------------------------------------------------------------------------

    bool LPS_Inv_Elm::get_options(int argc, char* argv[]) {
      using namespace mcrl2::utilities;

      interface_description clinterface(argv[0], NAME, AUTHOR,
        "check invariants and use these to simplify or eliminate summands of an LPS",
        "[OPTION]... --invariant=INVFILE [INFILE [OUTFILE]]\n",
        "Checks whether the boolean formula (an mCRL2 data expression of sort Bool) in "
        "INVFILE is an invariant of the linear process specification (LPS) in INFILE. "
        "If this is the case, the tool eliminates all summands of the LPS whose condition "
        "violates the invariant, and writes the result to OUTFILE. "
        "If INFILE is present, stdin is used. If OUTFILE is not present, stdout is used.\n"
        "\n"
        "The tool can also be used to simplify the conditions of the summands of the given LPS.");

      clinterface.add_rewriting_options();
      clinterface.add_prover_options();

      clinterface.
        add_option("invariant", make_mandatory_argument("INVFILE"),
          "use the boolean formula (an mCRL2 data expression of sort Bool) in INVFILE as invariant", 'i').
        add_option("summand", make_mandatory_argument("NUM"),
          "eliminate or simplify the summand with number NUM only", 's').
        add_option("no-check",
          "do not check if the invariant holds before eliminating unreachable summands", 'n').
        add_option("no-elimination",
          "do not eliminate or simplify summands", 'e').
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

      command_line_parser parser(clinterface, argc, argv);

      if (parser.continue_execution()) {
        f_no_check        = 0 < parser.options.count("no-check");
        f_no_elimination  = 0 < parser.options.count("no-elimination");
        f_simplify_all    = 0 < parser.options.count("simplify-all");
        f_all_violations  = 0 < parser.options.count("all-violations");
        f_counter_example = 0 < parser.options.count("counter-example");
        f_apply_induction = 0 < parser.options.count("induction");

        if (parser.options.count("invariant")) {
          f_invariant_file_name = parser.option_argument_as< std::string >("invariant");
        }
        else {
          parser.error("a file containing an invariant must be specified using the option --invariant=INVFILE");
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
            gsVerboseMsg("Eliminating or simplifying summand number %u.\n", f_summand_number);
          }
        }
        if (parser.options.count("time-limit")) {
          f_time_limit = parser.option_argument_as< size_t >("time-limit");
        }

        f_strategy = parser.option_argument_as< mcrl2::data::rewriter::strategy >("rewriter");

        if (parser.options.count("smt-solver")) {
          f_path_eliminator = true;
          f_solver_type     = parser.option_argument_as< SMT_Solver_Type >("smt-solver");
        }

        if (2 < parser.arguments.size()) {
          parser.error("too many file arguments");
        }
        else {
          if (0 < parser.arguments.size()) {
            f_lps_file_name = parser.arguments[0];
          }
          if (1 < parser.arguments.size()) {
            f_output_file_name = parser.arguments[1];
          }
        }
      }

      return parser.continue_execution();
    }

    // --------------------------------------------------------------------------------------------

    /// Reads the invariant specified by LPS_Inv_Elm::f_invariant_file_name and saves it as
    /// LPS_Inv_Elm::f_invariant.
    /// Reads the LPS specified by LPS_Inv_Elm::f_lps_file_name and saves it as
    /// LPS_Inv_Elm::f_lps. If no input file name was specified, the LPS is read from stdin.

    void LPS_Inv_Elm::read_input() {

      //read the LPS
      mcrl2::lps::specification lps_specification;
      lps_specification.load(f_lps_file_name);

      // type checking and data implementation of data expressions use an lps
      // before data implementation
      ATermAppl f_reconstructed_spec = reconstruct_spec(specification_to_aterm(lps_specification));

      //parse the invariant formula from infilename
      std::ifstream instream(f_invariant_file_name.c_str());
      if (!instream.is_open()) {
        throw mcrl2::runtime_error("cannot open input file '" + f_invariant_file_name +"'");
      }
      gsVerboseMsg("parsing input file '%s'...\n", f_invariant_file_name.c_str());
      f_invariant = parse_data_expr(instream);
      instream.close();
      if(!f_invariant){
        exit(1);
      }

      //typecheck the invariant formula
      ATermList vars = ATLgetArgument(ATAgetArgument(f_reconstructed_spec, 2), 1);
      ATermTable var_table = ATtableCreate(63,50);
      for (; !ATisEmpty(vars); vars = ATgetNext(vars)) {
        ATermAppl var = ATAgetFirst(vars);
        ATtablePut(var_table, ATgetArgument(var, 0), ATgetArgument(var, 1));
      }
      f_invariant = type_check_data_expr(f_invariant, mcrl2::core::detail::gsMakeSortIdBool(), f_reconstructed_spec, var_table);
      ATtableDestroy(var_table);
      if(!f_invariant){
        gsErrorMsg("Typechecking of the invariant formula failed.\n");
        exit(1);
      }

      //data implement the invariant formula
      f_invariant = implement_data_expr(f_invariant,f_reconstructed_spec);
      if(!f_invariant){
        gsErrorMsg("Data implementation of the invariant formula failed.\n");
        exit(1);
      }

      f_lps = mcrl2::lps::specification(specification_to_aterm(f_reconstructed_spec));
    }

    // --------------------------------------------------------------------------------------------

    /// Checks if the formula in the file LPS_Inv_Elm::f_invariant_file_name is an invariant,
    /// if the flag LPS_Inv_Elm::f_no_check is set to false.
    /// \return true, if the invariant holds or is not checked.
    ///         false, if the invariant does not hold.

    bool LPS_Inv_Elm::check_invariant() {
      if (!f_no_check) {
        Invariant_Checker v_invariant_checker(
          f_lps, f_strategy, f_time_limit, f_path_eliminator, f_solver_type, f_apply_induction, f_counter_example, f_all_violations, f_dot_file_name.c_str()
        );

        return v_invariant_checker.check_invariant(f_invariant);
      } else {
        gsWarningMsg("The invariant is not checked; it may not hold for this LPS.\n");
        return true;
      }
    }

    // --------------------------------------------------------------------------------------------

    /// Removes the summands from the LPS LPS_Inv_Elm::f_lps whose conditions violate the invariant
    /// LPS_Inv_Elm::f_invariant, if the flag LPS_Inv_Elm::f_no_elimination is set to false.
    /// If the flag LPS_Inv_Elm::f_simplify_all is set
    /// to true, the condition of each summand is replaced by the BDD of the
    /// condition in conjunction with the invariant as well.

    void LPS_Inv_Elm::simplify() {
      if (!f_no_elimination) {
        Invariant_Eliminator v_invariant_eliminator(
          f_lps, f_strategy, f_time_limit, f_path_eliminator, f_solver_type, f_apply_induction, f_simplify_all
        );

        f_lps = v_invariant_eliminator.simplify(f_invariant, f_summand_number);
      }
    }

    // --------------------------------------------------------------------------------------------

    /// Writes the LPS LPS_Inv_Elm::f_lps to the file specified by
    /// LPS_Inv_Elm::f_output_file_name.

    void LPS_Inv_Elm::write_result() {
      if (!f_no_elimination) {
        mcrl2::lps::specification lps_specification(f_lps);
        lps_specification.save(f_output_file_name);
      }
    }

// Main function ----------------------------------------------------------------------------------

  int main(int argc, char* argv[])
  {
    MCRL2_ATERM_INIT(argc, argv)

    try {
      LPS_Inv_Elm v_lps_inv_elm;

      if (v_lps_inv_elm.get_options(argc, argv)) {
        v_lps_inv_elm.read_input();
        if (v_lps_inv_elm.check_invariant()) {
          v_lps_inv_elm.simplify();
          v_lps_inv_elm.write_result();
        }
      }
    }
    catch (std::exception& e) {
      std::cerr << e.what() << std::endl;
      return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
  }
