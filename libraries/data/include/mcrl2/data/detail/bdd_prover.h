// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/bdd_prover.h
/// \brief EQ-BDD based prover for mCRL2 boolean data expressions

#ifndef BDD_PROVER_H
#define BDD_PROVER_H

#include "aterm2.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/detail/prover.h"
#include "mcrl2/data/detail/prover/bdd_simplifier.h"
#include "mcrl2/data/detail/prover/bdd_path_eliminator.h"
#include "mcrl2/data/detail/prover/induction.h"
#include "mcrl2/data/detail/prover/utilities.h"

namespace mcrl2 {
  namespace data {
    namespace detail {

  /** \brief A prover that uses EQ-BDDs.
   *
   * \detail
   * A class based on the Prover class that takes an expression of sort
   * Bool in internal mCRL2 format and creates the corresponding EQ-BDD.
   * Using this EQ-BDD, the class can determine if the original
   * formula is a tautology or a contradiction. The term "formula" in
   * the following text denotes arbitrary expressions of sort Bool in
   * the mCRL2 format.
   *
   * A prover uses a rewriter to rewrite parts of the formulas it
   * manipulates. The constructor BDD_Prover::BDD_Prover initializes the
   * prover's rewriter with the data equations in internal mCRL2 format
   * contained in the LPS passed as parameter a_lps and the rewrite
   * strategy passed as parameter a_rewrite_strategy. The parameter
   * a_rewrite_strategy can be set to either GS_REWR_INNER,
   * GS_REWR_INNERC, GS_REWR_JITTY or GS_REWR_JITTYC. To limit the
   * number of seconds spent on proving a single formula, a time limit
   * can be set. If the time limit is set to 0, no time limit will be
   * enforced. The parameter a_apply_induction indicates whether or
   * induction on lists is applied. The constructor
   * BDD_Prover::BDD_Prover has two additional parameters,
   * a_path_eliminator and a_solver_type. The parameter
   * a_path_eliminator can be used to enable the use of an instance of
   * the class BDD_Path_Eliminator. Instances of this class use an SMT
   * solver to eliminate inconsistent paths from BDDs. The parameter
   * a_solver_type can be used to indicate which SMT solver should be
   * used for this task. Either the SMT solver ario
   * (http://www.eecs.umich.edu/~ario/) or cvc-lite
   * (http://www.cs.nyu.edu/acsys/cvcl/) can be used. To use one of
   * these solvers, the directory containing the corresponding
   * executable must be in the path. If the parameter a_path_eliminator
   * is set to false, the parameter a_solver_type is ignored and no
   * instance of the class BDD_Path_Eliminator is initialized.
   *
   * The formula to be handled is set using the method
   * Prover::set_formula inherited from the class Prover. An entity of
   * the class BDD_Prover uses binary decision diagrams to determine if
   * a given formula is a tautology or a contradiction. The resulting
   * BDD can be retreived using the method BDD_Prover::get_bdd.
   *
   * The methods BDD_Prover::is_tautology and
   * BDD_Prover::is_contradiction indicate whether or not a formula is a
   * tautology or a contradiction. These methods will return answer_yes,
   * answer_no or answer_undefined. If a formula is neither a tautology
   * nor a contradiction according to the prover, a so called witness or
   * counter example can be returned by the methods
   * BDD_Prover::get_witness and BDD_Prover::get_counter_example. A
   * witness is a valuation for which the formula holds, a counter
   * example is a valuation for which it does not hold.
  */

class BDD_Prover: public Prover {
  private:

    /// \brief Flag indicating whether or not the result of the comparison between the first two arguments
    /// \brief weighs stronger than the result of the comparison between the second pair of arguments of an
    /// \brief equation, when determining the order of expressions.
    bool f_reverse;

    /// \brief Flag indicating whether or not the arguments of equality functions are taken into account
    /// \brief when determining the order of expressions.
    bool f_full;

    /// \brief A flag indicating whether or not induction on lists is applied.
    bool f_apply_induction;

    /// \brief A data specification.
    const data_specification &f_data_spec;

    /// \brief A hashtable that maps formulas to BDDs.
    /// \brief If the BDD of a formula is unknown, it maps this formula to 0.
    ATermTable f_formula_to_bdd;

    /// \brief A hashtable that maps formulas to the smallest guard occuring in those formulas.
    /// \brief If the smallest guard of a formula is unknown, it maps this formula to 0.
    ATermTable f_smallest;

    /// \brief Class that provides information about the structure of BDDs.
    BDD_Info f_bdd_info;

    /// \brief Class that simplifies a BDD.
    BDD_Simplifier* f_bdd_simplifier;

    /// \brief Class that creates all statements needed to prove a given property using induction.
    Induction f_induction;

    /// \brief Constructs the EQ-BDD corresponding to the formula Prover::f_formula.
    void build_bdd()
    {
      f_formula_to_bdd = ATtableCreate(60000, 25);
      f_smallest = ATtableCreate(2000, 50);
      f_deadline = time(0) + f_time_limit;

      ATerm v_previous_1 = 0;
      ATerm v_previous_2 = 0;

      core::gsDebugMsg("Formula: %P\n", f_formula);

      f_internal_bdd = f_rewriter->toRewriteFormat(f_formula);
      f_internal_bdd = f_rewriter->rewriteInternal(f_internal_bdd);
      f_internal_bdd = f_manipulator->orient(f_internal_bdd);

      core::gsDebugMsg("Formula rewritten and oriented: %P\n", f_rewriter->fromRewriteFormat(f_internal_bdd));

      while (v_previous_1 != f_internal_bdd && v_previous_2 != f_internal_bdd) {
        v_previous_2 = v_previous_1;
        v_previous_1 = f_internal_bdd;
        f_internal_bdd = bdd_down(f_internal_bdd);
        core::gsDebugMsg("End of iteration.\n");
        core::gsDebugMsg("Intermediate BDD: %P\n", f_rewriter->fromRewriteFormat(f_internal_bdd));
      }

      f_bdd = f_rewriter->fromRewriteFormat(f_internal_bdd);
      core::gsDebugMsg("Resulting BDD: %P\n", f_bdd);

      ATtableDestroy(f_formula_to_bdd);
      ATtableDestroy(f_smallest);
    }

    /// \brief Creates the EQ-BDD corresponding to the formula a_formula.
    ATerm bdd_down(ATerm a_formula)
    {
      std::string indent;

      return bdd_down(f_internal_bdd, indent);
    }

    /// \brief Creates the EQ-BDD corresponding to the formula a_formula.
    ATerm bdd_down(ATerm a_formula, std::string& a_indent){
      a_indent.append("  ");

      if (f_time_limit != 0 && (f_deadline - time(0)) <= 0) {
        core::gsDebugMsg("The time limit has passed.\n");
        return a_formula;
      }

      if (f_info->is_true(a_formula)) {
        return a_formula;
      }
      if (f_info->is_false(a_formula)) {
        return a_formula;
      }

      ATerm v_bdd = ATtableGet(f_formula_to_bdd, a_formula);
      if (v_bdd) {
        return v_bdd;
      }

      ATerm v_guard = smallest(a_formula);
      if (!v_guard) {
        return a_formula;
      } else {
        core::gsDebugMsg("%sSmallest guard: %P\n", a_indent.c_str(), f_rewriter->fromRewriteFormat(v_guard));
      }

      ATerm v_term1, v_term2;

      v_term1 = f_manipulator->set_true(a_formula, v_guard);
      v_term1 = f_rewriter->rewriteInternal(v_term1);
      v_term1 = f_manipulator->orient(v_term1);
      core::gsDebugMsg("%sTrue-branch after rewriting and orienting: %P\n", a_indent.c_str(), f_rewriter->fromRewriteFormat(v_term1));
      v_term1 = bdd_down(v_term1, a_indent);
      core::gsDebugMsg("%sBDD of the true-branch: %P\n", a_indent.c_str(), f_rewriter->fromRewriteFormat(v_term1));

      v_term2 = f_manipulator->set_false(a_formula, v_guard);
      v_term2 = f_rewriter->rewriteInternal(v_term2);
      v_term2 = f_manipulator->orient(v_term2);
      core::gsDebugMsg("%sFalse-branch after rewriting and orienting: %P\n", a_indent.c_str(), f_rewriter->fromRewriteFormat(v_term2));
      v_term2 = bdd_down(v_term2, a_indent);
      core::gsDebugMsg("%sBDD of the false-branch: %P\n", a_indent.c_str(), f_rewriter->fromRewriteFormat(v_term2));

      v_bdd = f_manipulator->make_reduced_if_then_else(v_guard, v_term1, v_term2);
      ATtablePut(f_formula_to_bdd, a_formula, v_bdd);

      a_indent.erase(a_indent.size() - 2);

      return v_bdd;
    }

    /// \brief Removes all inconsistent paths from the BDD BDD_Prover::f_bdd.
    void eliminate_paths()
    {
      time_t v_new_time_limit;

      v_new_time_limit = f_deadline - time(0);
      if (v_new_time_limit > 0 || f_time_limit == 0) {
        core::gsDebugMsg("Simplifying the BDD:\n");
        f_bdd_simplifier->set_time_limit((std::max)(v_new_time_limit, time(0)));
        f_bdd = f_bdd_simplifier->simplify(f_bdd);
        core::gsDebugMsg("Resulting BDD: %P\n", f_bdd);
      }
    }

    /// \brief Updates the values of Prover::f_tautology and Prover::f_contradiction.
    void update_answers()
    {
      if (!f_processed) {
        build_bdd();
        eliminate_paths();
        ATermAppl v_original_formula = f_formula;
        ATermAppl v_original_bdd = f_bdd;
        if (f_apply_induction && !(f_bdd_info.is_true(f_bdd) || f_bdd_info.is_false(f_bdd))) {
          f_induction.initialize(v_original_formula);
          while (f_induction.can_apply_induction() && !f_bdd_info.is_true(f_bdd)) {
            core::gsDebugMsg("Applying induction.\n");
            f_formula = f_induction.apply_induction();
            build_bdd();
            eliminate_paths();
          }
          if (f_bdd_info.is_true(f_bdd)) {
            f_tautology = answer_yes;
            f_contradiction = answer_no;
          } else {
            v_original_formula = sort_bool::not_(data_expression(v_original_formula));
            f_bdd = v_original_bdd;
            f_induction.initialize(v_original_formula);
            while (f_induction.can_apply_induction() && !f_bdd_info.is_true(f_bdd)) {
              core::gsDebugMsg("Applying induction on the negated formula.\n");
              f_formula = f_induction.apply_induction();
              build_bdd();
              eliminate_paths();
            }
            if (f_bdd_info.is_true(f_bdd)) {
              f_bdd = sort_bool::false_();
              f_tautology = answer_no;
              f_contradiction = answer_yes;
            } else {
              f_bdd = v_original_bdd;
              f_tautology = answer_undefined;
              f_contradiction = answer_undefined;
            }
          }
        } else {
          if (f_bdd_info.is_true(f_bdd)) {
            f_tautology = answer_yes;
            f_contradiction = answer_no;
          } else if (f_bdd_info.is_false(f_bdd)) {
            f_tautology = answer_no;
            f_contradiction = answer_yes;
          } else {
            f_tautology = answer_undefined;
            f_contradiction = answer_undefined;
          }
        }
        f_processed = true;
      }
    };

    /// \brief Returns the smallest guard in the formula a_formula.
    ATerm smallest(ATerm a_formula)
    {
      if (f_info->is_variable(a_formula)) {
        if (f_info->has_type_bool(a_formula)) {
          return a_formula;
        } else {
          return 0;
        }
      }
      if (f_info->is_true(a_formula) || f_info->is_false(a_formula)) {
        return 0;
      }

      ATerm v_result;

      v_result = ATtableGet(f_smallest, a_formula);
      if (v_result) {
        return v_result;
      }

      int i;
      int v_length;
      ATerm v_small;

      v_length = f_info->get_number_of_arguments(a_formula);

      for (i = 0; i < v_length; i++) {
        v_small = smallest(f_info->get_argument(a_formula, i));
        if (v_small) {
          if (v_result) {
            if (f_info->lpo1(v_result, v_small)) {
              v_result = v_small;
            }
          } else {
            v_result = v_small;
          }
        }
      }
      if (!v_result && f_info->has_type_bool(a_formula)) {
        v_result = a_formula;
      }
      if (v_result) {
        ATtablePut(f_smallest, a_formula, v_result);
      }

      return v_result;
    }

    /// \brief Returns branch of the BDD a_bdd, depending on the polarity a_polarity.
    ATermAppl get_branch(ATermAppl a_bdd, bool a_polarity){
      ATermAppl v_result;

      if (f_bdd_info.is_if_then_else(a_bdd)) {
        ATermAppl v_guard = f_bdd_info.get_guard(a_bdd);
        ATermAppl v_true_branch = f_bdd_info.get_true_branch(a_bdd);
        ATermAppl v_false_branch = f_bdd_info.get_false_branch(a_bdd);
        ATermAppl v_branch = get_branch(v_true_branch, a_polarity);
        if (v_branch == 0) {
          v_branch = get_branch(v_false_branch, a_polarity);
          if (v_branch == 0) {
            v_result = 0;
          } else {
            data_expression v_term = sort_bool::not_(data_expression(v_guard));
            v_result = lazy::and_(data_expression(v_branch), v_term);
          }
        } else {
          v_result = lazy::and_(data_expression(v_branch), data_expression(v_guard));
        }
      } else {
        if ((f_bdd_info.is_true(a_bdd) && a_polarity) || (f_bdd_info.is_false(a_bdd) && !a_polarity)) {
          v_result = sort_bool::true_();
        } else {
          v_result = 0;
        }
      }
      return v_result;
    }

  protected:

    /// \brief A binary decision diagram in the internal representation of the rewriter.
    ATerm f_internal_bdd;

    /// \brief A binary decision diagram in the internal representation of mCRL2.
    ATermAppl f_bdd;
  public:

    /// \brief Constructor that initializes the attributes BDD_Prover::f_data_spec, Prover::f_time_limit and
    /// \brief BDD_Prover::f_bdd_simplifier.
    /// precondition: the argument passed as parameter a_time_limit is greater than or equal to 0. If the argument is equal
    /// to 0, no time limit will be enforced
    /// precondition: the argument passed as parameter a_lps is an LPS
    /* BDD_Prover(
      const data_specification &data_spec,
      mcrl2::data::rewriter::strategy a_rewrite_strategy = mcrl2::data::rewriter::jitty,
      int a_time_limit = 0,
      bool a_path_eliminator = false,
      SMT_Solver_Type a_solver_type = solver_type_ario,
      bool a_apply_induction = false
    ); */

    BDD_Prover(
      mcrl2::data::data_specification const& data_spec,
      mcrl2::data::rewriter::strategy a_rewrite_strategy = mcrl2::data::rewriter::jitty,
      int a_time_limit = 0,
      bool a_path_eliminator = false,
      SMT_Solver_Type a_solver_type = solver_type_ario,
      bool a_apply_induction = false)
      : Prover(data_spec, a_rewrite_strategy, a_time_limit),
        f_data_spec(data_spec),
        f_induction(data_spec)
    {
      f_reverse = true;
      f_full = true;
      f_apply_induction = a_apply_induction;
      f_info->set_reverse(f_reverse);
      f_info->set_full(f_full);
      core::gsDebugMsg(
        "Flags:\n"
        "  Reverse: %s,\n"
        "  Full: %s,\n",
        bool_to_char_string(f_reverse),
        bool_to_char_string(f_full)
      );
      if (a_path_eliminator) {
        f_bdd_simplifier = new BDD_Path_Eliminator(a_solver_type);
      } else {
        f_bdd_simplifier = new BDD_Simplifier();
      }
    }

    /// \brief Destructor that destroys the BDD simplifier BDD_Prover::f_bdd_simplifier.
    virtual ~BDD_Prover()
    {
      delete f_bdd_simplifier;
      f_bdd_simplifier = 0;
    }

    /// \brief Indicates whether or not the formula Prover::f_formula is a tautology.
    virtual Answer is_tautology()
    {
      update_answers();
      return f_tautology;
    }

    /// \brief Indicates whether or not the formula Prover::f_formula is a contradiction.
    virtual Answer is_contradiction()
    {
      update_answers();
      return f_contradiction;
    }

    /// \brief Returns the BDD BDD_Prover::f_bdd.
    virtual ATermAppl get_bdd()
    {
      update_answers();
      return f_bdd;
    }

    /// \brief Returns all the guards on a path in the BDD that leads to a leaf labelled "true", if such a leaf exists.
    virtual ATermAppl get_witness()
    {
      ATermAppl v_result;

      update_answers();
      if (!(is_contradiction() == answer_yes) && !(is_tautology() == answer_yes)) {
        core::gsDebugMsg("The formula appears to be satisfiable.\n");
        v_result = get_branch(f_bdd, true);
      } else {
        core::gsDebugMsg("The formula is a contradiction or a tautology.\n");
        v_result = 0;
      }
      return v_result;
    }

    /// \brief Returns all the guards on a path in the BDD that leads to a leaf labelled "false", if such a leaf exists.
    virtual ATermAppl get_counter_example(){
      ATermAppl v_result;

      update_answers();
      if (!(is_contradiction() == answer_yes) && !(is_tautology() == answer_yes)) {
        core::gsDebugMsg("The formula appears to be satisfiable.\n");
        v_result = get_branch(f_bdd, false);
      } else {
        core::gsDebugMsg("The formula is a contradiction or a tautology.\n");
        v_result = 0;
      }
      return v_result;
    }

};
    }
  }
}

#endif
