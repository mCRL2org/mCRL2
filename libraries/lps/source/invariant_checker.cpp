// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file source/invariant_checker.cpp
/// \brief Add your file description here.

#include "boost.hpp" // precompiled headers

#include <sstream>
#include <cstring>

#include "mcrl2/core/messaging.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/lps/invariant_checker.h"
#include "mcrl2/new_data/detail/bdd_prover.h"
#include "mcrl2/exception.h"

using namespace mcrl2::new_data;
using namespace mcrl2::new_data::detail;
using namespace mcrl2::core;

// Class Invariant_Checker ------------------------------------------------------------------------
  // Class Invariant_Checker - Functions declared private -----------------------------------------

    void Invariant_Checker::print_counter_example() {
      if (f_counter_example) {
        ATermAppl v_counter_example;

        v_counter_example = f_bdd_prover.get_counter_example();
        if (v_counter_example == 0) {
          throw mcrl2::runtime_error(
           "Cannot print counter example. This is probably caused by an abrupt stop of the\n"
           "conversion from expression to EQ-BDD. This typically occurs when a time limit is set.");
        } else {
          gsMessage("  Counter example: %P\n", v_counter_example);
        }
      }
    }

    // --------------------------------------------------------------------------------------------

    void Invariant_Checker::save_dot_file(int a_summand_number) {
      if (f_dot_file_name != 0) {
        std::ostringstream v_file_name(f_dot_file_name);

        if (a_summand_number == -1) {
          v_file_name << "-init.dot";
        } else {
          v_file_name << "-" << a_summand_number << ".dot";
        }
        f_bdd2dot.output_bdd(f_bdd_prover.get_bdd(), v_file_name.str().c_str());
      }
    }

    // --------------------------------------------------------------------------------------------

    bool Invariant_Checker::check_init(ATermAppl a_invariant) {
      ATermList v_assignments = ATLgetArgument(f_init, 1);
      ATermAppl v_assignment;
      ATermAppl v_variable;
      ATermAppl v_expression;
      ATermList v_substitutions = ATmakeList0();
      ATermAppl v_substitution;

      while (!ATisEmpty(v_assignments)) {
        v_assignment = ATAgetFirst(v_assignments);
        v_variable = ATAgetArgument(v_assignment, 0);
        v_expression = ATAgetArgument(v_assignment, 1);
        v_substitution = gsMakeSubst_Appl(v_variable, v_expression);
        v_substitutions = ATinsert(v_substitutions, (ATerm) v_substitution);
        v_assignments = ATgetNext(v_assignments);
      }
      a_invariant = gsSubstValues_Appl(v_substitutions, a_invariant, true);
      f_bdd_prover.set_formula(a_invariant);
      if (f_bdd_prover.is_tautology() == answer_yes) {
        return true;
      } else {
        if (f_bdd_prover.is_contradiction() != answer_yes) {
          print_counter_example();
          save_dot_file(-1);
        }
        return false;
      }
    }

    // --------------------------------------------------------------------------------------------

    bool Invariant_Checker::check_summand(ATermAppl a_invariant, ATermAppl a_summand, int a_summand_number) {
      ATermAppl v_condition = ATAgetArgument(a_summand, 1);
      ATermList v_assignments = ATLgetArgument(a_summand, 4);
      ATermAppl v_assignment;
      ATermAppl v_variable;
      ATermAppl v_expression;
      ATermList v_substitutions = ATmakeList0();
      ATermAppl v_substitution;

      while (!ATisEmpty(v_assignments)) {
        v_assignment = ATAgetFirst(v_assignments);
        v_variable = ATAgetArgument(v_assignment, 0);
        v_expression = ATAgetArgument(v_assignment, 1);
        v_substitution = gsMakeSubst_Appl(v_variable, v_expression);
        v_substitutions = ATinsert(v_substitutions, (ATerm) v_substitution);
        v_assignments = ATgetNext(v_assignments);
      }

      ATermAppl v_subst_invariant = gsSubstValues_Appl(v_substitutions, a_invariant, true);
      ATermAppl v_formula = mcrl2::core::detail::gsMakeDataExprAnd(a_invariant, v_condition);

      v_formula = mcrl2::core::detail::gsMakeDataExprImp(v_formula, v_subst_invariant);
      f_bdd_prover.set_formula(v_formula);
      if (f_bdd_prover.is_tautology() == answer_yes) {
        gsVerboseMsg("The invariant holds for summand %d.\n", a_summand_number);
        return true;
      } else {
        gsMessage("The invariant does not hold for summand %d.\n", a_summand_number);
        if (f_bdd_prover.is_contradiction() != answer_yes) {
          print_counter_example();
          save_dot_file(a_summand_number);
        }
        return false;
      }
    }

    // --------------------------------------------------------------------------------------------

    bool Invariant_Checker::check_summands(ATermAppl a_invariant) {
      ATermList v_summands = f_summands;
      ATermAppl v_summand;
      bool v_result = true;
      int v_summand_number = 1;

      while (!ATisEmpty(v_summands) && (f_all_violations || v_result) ) {
        v_summand = ATAgetFirst(v_summands);
        v_result = check_summand(a_invariant, v_summand, v_summand_number) && v_result;
        v_summands = ATgetNext(v_summands);
        v_summand_number++;
      }
      return v_result;
    }

  // Class Invariant_Checker - Functions declared public --------------------------------------------

    Invariant_Checker::Invariant_Checker(
      ATermAppl a_lps, mcrl2::new_data::rewriter::strategy a_rewrite_strategy, int a_time_limit, bool a_path_eliminator, SMT_Solver_Type a_solver_type,
      bool a_apply_induction, bool a_counter_example, bool a_all_violations, char const* a_dot_file_name
    ):
      f_bdd_prover(ATAgetArgument(a_lps,0), a_rewrite_strategy, a_time_limit, a_path_eliminator, a_solver_type, a_apply_induction)
    {
      f_init = ATAgetArgument(a_lps, 3);
      f_summands = ATLgetArgument(ATAgetArgument(a_lps, 2), 2);
      f_counter_example = a_counter_example;
      f_all_violations = a_all_violations;
      if (a_dot_file_name == 0) {
        f_dot_file_name = 0;
      } else {
        f_dot_file_name = strdup(a_dot_file_name);
      }
    }

    // --------------------------------------------------------------------------------------------

    Invariant_Checker::~Invariant_Checker() {
      // Nothing to free.
    }

    // --------------------------------------------------------------------------------------------

    bool Invariant_Checker::check_invariant(ATermAppl a_invariant) {
      bool v_result = true;

      if (check_init(a_invariant)) {
        gsVerboseMsg("The invariant holds for the initial state.\n");
      } else {
        gsMessage("The invariant does not hold for the initial state.\n");
        v_result = false;
      }
      if ((f_all_violations || v_result)) {
        if (check_summands(a_invariant)) {
          gsVerboseMsg("The invariant holds for all summands.\n");
        } else {
          gsMessage("The invariant does not hold for all summands.\n");
          v_result = false;
        }
      }
      if (v_result) {
        gsMessage("The invariant holds for this LPS.\n");
      } else {
        gsMessage("The invariant does not hold for this LPS.\n");
      }

      return v_result;
    }
