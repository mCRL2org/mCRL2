// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/prover/bdd_path_eliminator.h
/// \brief BDD inconsistent path elimination using external SMT solvers

#ifndef BDD_PATH_ELIMINATOR_H
#define BDD_PATH_ELIMINATOR_H

#include "aterm2.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/data/detail/prover/bdd_simplifier.h"
#include "mcrl2/data/detail/prover/smt_lib_solver.h"
#include "mcrl2/data/detail/prover/bdd_manipulator.h"
#include "mcrl2/data/detail/prover/bdd_info.h"

namespace mcrl2 {
  namespace data {
    namespace detail {

  /// \brief The enumaration type SMT_Solver_Type enumerates all available SMT solvers.
enum SMT_Solver_Type {
  solver_type_ario,
  solver_type_cvc,
  solver_type_cvc_fast
};

  /** \brief Base class for eliminating inconsistent paths from BDDs.
   *
   * \detail
   * The class BDD_Path_Eliminator is a base class for classes that
   * eliminate inconsistent paths from BDDs. The class
   * BDD_Path_Eliminator inherits from the class BDD_Simplifier. It uses
   * an SMT solver to eliminate inconsistent paths from BDDs. The
   * parameter a_solver_type of the constructor
   * BDD_Path_Eliminator::BDD_Path_Eliminator is used to indicate which
   * SMT solver should be used for this task. A path in a BDD is
   * constructed by selecting a set of guards as follows: starting at
   * the root node, one of the two edges at each guard is followed until
   * a leaf is reached. Each time the true-edge is chosen, the guard is
   * added to the set. Each time the false-edge is chosen, the negation
   * of the guard is added to the set. If the conjunction of all
   * elements in this set is a contradiction, the path
   * is inconsistent.

   * The method BDD_Path_Eliminator::simplify receives a BDD as
   * parameter a_bdd and returns the equivalent BDD from which all
   * inconsistent paths have been removed.
  */
class BDD_Path_Eliminator: public BDD_Simplifier {

  private:

    /// \brief Pointer to an SMT solver used to determine whether or not a path is inconsistent.
    SMT_Solver* f_smt_solver;

    /// \brief Class that provides information about the structure of BDDs.
    BDD_Info f_bdd_info;

    /// \brief Class that can be used to manipulate BDDs.
    BDD_Manipulator f_bdd_manipulator;

    /// \brief Class that provides information about expressions.
    Expression_Info f_expression_info;

    /// \brief Returns a list representing the conjunction of all guards in a_path and the guard a_guard.
    /// \param a_path A list of guards and negated guards, representing a path in a BDD.
    /// \param a_guard A guard or a negated guard.
    /// \param a_minimal A boolean value indicating whether or not minimal sets of possibly inconsistent guards are constructed.
    ATermList create_condition(ATermList a_path, ATermAppl a_guard, bool a_minimal) {
      if (!a_minimal) {
        return ATinsert(a_path, (ATerm) a_guard);
      } else {
        ATermList v_set;
        ATermList v_auxiliary_set;
        ATermList v_iterate_over_set;
        ATermList v_iterate_over_path;
        ATermAppl v_guard_from_set;
        ATermAppl v_guard_from_path;

        v_set = ATmakeList1((ATerm) a_guard);
        v_auxiliary_set = ATmakeList0();
        while (v_set != v_auxiliary_set) {
          v_auxiliary_set = v_set;
          v_iterate_over_set = v_set;
          while (!ATisEmpty(v_iterate_over_set)) {
            v_guard_from_set = core::ATAgetFirst(v_iterate_over_set);
            v_iterate_over_set = ATgetNext(v_iterate_over_set);
            v_iterate_over_path = a_path;
            while (!ATisEmpty(v_iterate_over_path)) {
              v_guard_from_path = core::ATAgetFirst(v_iterate_over_path);
              v_iterate_over_path = ATgetNext(v_iterate_over_path);
              if (variables_overlap(v_guard_from_set, v_guard_from_path)) {
                v_set = ATinsert(v_set, (ATerm) v_guard_from_path);
                a_path = ATremoveElement(a_path, (ATerm) v_guard_from_path);
              }
            }
          }
        }
        return v_set;
      }
    }

    /// \brief Simplifies the BDD a_bdd using path a_path. Paths whose guards in conjunction with the guards in
    /// \brief a_path are inconsistent are removed.
    /// \param a_bdd A binary decision diagram.
    /// \param a_path A list of guards and negated guards, representing a path in a BDD.
    ATermAppl aux_simplify(ATermAppl a_bdd, ATermList a_path)
    {
      if (f_deadline != 0 && (f_deadline - time(0)) < 0) {
        core::gsDebugMsg("The time limit has passed.\n");
        return a_bdd;
      }

      if (f_bdd_info.is_true(a_bdd) || f_bdd_info.is_false(a_bdd)) {
        return a_bdd;
      }

      ATermAppl v_guard = f_bdd_info.get_guard(a_bdd);
      ATermAppl v_negated_guard = sort_bool::not_(data_expression(v_guard));
      ATermList v_true_condition = create_condition(a_path, v_guard, true);
      bool v_true_branch_enabled = f_smt_solver->is_satisfiable(v_true_condition);
      if (!v_true_branch_enabled) {
        ATermList v_false_path = ATinsert(a_path, (ATerm) v_negated_guard);
        return aux_simplify(f_bdd_info.get_false_branch(a_bdd), v_false_path);
      } else {
        ATermList v_false_condition = create_condition(a_path, v_negated_guard, true);
        bool v_false_branch_enabled = f_smt_solver->is_satisfiable(v_false_condition);
        if (!v_false_branch_enabled) {
          ATermList v_true_path = ATinsert(a_path, (ATerm) v_guard);
          return aux_simplify(f_bdd_info.get_true_branch(a_bdd), v_true_path);
        } else {
          ATermList v_true_path = ATinsert(a_path, (ATerm) v_guard);
          ATermList v_false_path = ATinsert(a_path, (ATerm) v_negated_guard);
          return f_bdd_manipulator.make_reduced_if_then_else(
            v_guard,
            aux_simplify(f_bdd_info.get_true_branch(a_bdd), v_true_path),
            aux_simplify(f_bdd_info.get_false_branch(a_bdd), v_false_path)
          );
        }
      }
    }

    /// \brief Returns true if the expression a_expression_1 has variables in common with expression a_expression_2.
    /// \param a_expression_1 An arbitrary expression.
    /// \param a_expression_2 An arbitrary expression.
    bool variables_overlap(ATermAppl a_expression_1, ATermAppl a_expression_2)
    {
      if (core::detail::gsIsOpId(a_expression_1)) {
        return false;
      } else if (core::detail::gsIsDataVarId(a_expression_1)) {
        return core::gsOccurs((ATerm) a_expression_1, (ATerm) a_expression_2);
      } else {
        assert (data::is_application(a_expression_1));
        data::application a = data::application(data::data_expression(a_expression_1));
        for (data_expression_list::const_iterator i = a.arguments().begin(); i != a.arguments().end(); ++i)
        {
          if(variables_overlap(*i, a_expression_2))
          {
            return true;
          }
        }
        return false;
      }
    }

  public:

    /// \brief Constructor that initializes the field BDD_Path_Eliminator::f_smt_solver.
    /// \param a_solver_type A value of an enumerated type, representing an SMT solver.
    BDD_Path_Eliminator(SMT_Solver_Type a_solver_type)
    {
#if !(defined(_MSC_VER) || defined(__MINGW32__) || defined(__CYGWIN__))
      if (a_solver_type == solver_type_ario) {
        if (mcrl2::data::detail::prover::ario_smt_solver::usable()) {
          f_smt_solver = new mcrl2::data::detail::prover::ario_smt_solver();

          return;
        }
      } else if (a_solver_type == solver_type_cvc) {
        if (mcrl2::data::detail::prover::cvc_smt_solver::usable()) {
          f_smt_solver = new mcrl2::data::detail::prover::cvc_smt_solver();

          return;
        }
      } else if (a_solver_type == solver_type_cvc_fast) {
#ifdef HAVE_CVC
        f_smt_solver = new SMT_Solver_CVC_Fast();
#else
        throw mcrl2::runtime_error("The fast implementation of CVC Lite is not available.");
#endif
      } else {
        throw mcrl2::runtime_error("An unknown SMT solver type was passed as argument.");
      }
#else
      throw mcrl2::runtime_error("No SMT solvers available on this platform.");
#endif // _MSC_VER
    }

    /// \brief Returns a BDD without inconsistent paths, equivalent to a_bdd.
    /// precondition: The argument passed as parameter a_bdd is a data expression in internal mCRL2 format with the
    /// following restrictions: It either represents the constant true or the constant false, or it is an if-then-else
    /// expression with an expression of sort Bool  as guard, and a then-branch and an else-branch that again follow
    /// these restrictions
    /// \param a_bdd A binary decision diagram.
    virtual ATermAppl simplify(ATermAppl a_bdd)
    {
      return aux_simplify(a_bdd, ATmakeList0());
    }
};

    }
  }
}

#endif
