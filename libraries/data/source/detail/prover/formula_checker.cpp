// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "boost.hpp" // precompiled headers

#include <sstream>

#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/exception.h"
#include "mcrl2/data/detail/prover/formula_checker.h"

using namespace mcrl2::core;

namespace mcrl2 {
  namespace data {
    namespace detail {

// Class Formula_Checker --------------------------------------------------------------------------
  // Class Formula_Checker - Functions declared private -------------------------------------------

    void Formula_Checker::print_counter_example() {
      if (f_counter_example) {
        ATermAppl v_counter_example;

        v_counter_example = f_bdd_prover.get_counter_example();
        if (v_counter_example == 0) {
          throw mcrl2::runtime_error(
            "Cannot print counter example. This is probably caused by an abrupt stop of the\n"
            "conversion from expression to EQ-BDD. This typically occurs when a time limit is set.\n"
          );
        } else {
          gsMessage("  Counter-example: %P\n", v_counter_example);
        }
      }
    }

    // ----------------------------------------------------------------------------------------------

    void Formula_Checker::print_witness() {
      if (f_witness) {
        ATermAppl v_witness;

        v_witness = f_bdd_prover.get_witness();
        if (v_witness == 0) {
          throw mcrl2::runtime_error(
            "Cannot print witness. This is probably caused by an abrupt stop of the\n"
            "conversion from expression to EQ-BDD. This typically occurs when a time limit is set.\n"
          );
        } else {
          gsMessage("  Witness: %P\n", v_witness);
        }
      }
    }

    // ----------------------------------------------------------------------------------------------

    void Formula_Checker::save_dot_file(int a_formula_number) {

      if (f_dot_file_name != 0) {
        std::ostringstream  v_file_name(f_dot_file_name);

        v_file_name << "-" << a_formula_number << ".dot";
        f_bdd2dot.output_bdd(f_bdd_prover.get_bdd(), v_file_name.str().c_str());
      }
    }

  // Class Formula_Checker - Functions declared public --------------------------------------------

    Formula_Checker::Formula_Checker(
      mcrl2::data::data_specification a_data_spec, mcrl2::data::rewriter::strategy a_rewrite_strategy, int a_time_limit, bool a_path_eliminator, SMT_Solver_Type a_solver_type,
      bool a_apply_induction, bool a_counter_example, bool a_witness, char const* a_dot_file_name
    ):
      f_bdd_prover(a_data_spec, a_rewrite_strategy, a_time_limit, a_path_eliminator, a_solver_type, a_apply_induction)
    {
      f_counter_example = a_counter_example;
      f_witness = a_witness;
      if (a_dot_file_name == 0) {
        f_dot_file_name = 0;
      } else {
        f_dot_file_name = strdup(a_dot_file_name);
      }
    }

    // --------------------------------------------------------------------------------------------

    Formula_Checker::~Formula_Checker() {
      // Nothing to free.
    }

    // --------------------------------------------------------------------------------------------

    void Formula_Checker::check_formulas(ATermList a_formulas) {
      ATermAppl v_formula;
      int v_formula_number = 1;

      while (!ATisEmpty(a_formulas)) {
        v_formula = ATAgetFirst(a_formulas);
        gsMessage("'%P': ", v_formula);
        f_bdd_prover.set_formula(v_formula);
        Answer v_is_tautology = f_bdd_prover.is_tautology();
        Answer v_is_contradiction = f_bdd_prover.is_contradiction();
        if (v_is_tautology == answer_yes) {
          gsMessage("Tautology\n");
        } else if (v_is_contradiction == answer_yes) {
          gsMessage("Contradiction\n");
        } else {
          gsMessage("Undeterminable\n");
          print_counter_example();
          print_witness();
          save_dot_file(v_formula_number);
        }
        a_formulas = ATgetNext(a_formulas);
        v_formula_number++;
      }
    }

    } //namespace detail
  } //namespace data
} //namespace mcrl2

