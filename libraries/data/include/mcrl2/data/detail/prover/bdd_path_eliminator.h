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

#include <iostream> // For streaming operators
#include <algorithm>
#include <iterator>
#include <cstring>

#include "mcrl2/utilities/logger.h"
#include "mcrl2/core/detail/function_symbols.h"
#include "mcrl2/data/detail/prover/solver_type.h"
#include "mcrl2/data/detail/prover/bdd_simplifier.h"
#include "mcrl2/data/detail/prover/smt_lib_solver.h"
#include "mcrl2/data/detail/prover/bdd_manipulator.h"
#include "mcrl2/data/detail/prover/bdd_info.h"

namespace mcrl2
{
namespace data
{
namespace detail
{

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
class BDD_Path_Eliminator: public BDD_Simplifier
{

  private:

    /// \brief Pointer to an SMT solver used to determine whether or not a path is inconsistent.
    SMT_Solver* f_smt_solver;

    /// \brief Class that provides information about the structure of BDDs.
    BDD_Info f_bdd_info;

    /// \brief Class that can be used to manipulate BDDs.
    BDD_Manipulator f_bdd_manipulator;

    /// \brief Returns a list representing the conjunction of all guards in a_path and the guard a_guard.
    /// \param a_path A list of guards and negated guards, representing a path in a BDD.
    /// \param a_guard A guard or a negated guard.
    /// \param a_minimal A boolean value indicating whether or not minimal sets of possibly inconsistent guards are constructed.
    data_expression_list create_condition(
               data_expression_list a_path,
               const data_expression &a_guard,
               bool a_minimal)
    {
      if (!a_minimal)
      {
        a_path.push_front(a_guard);
        return a_path;
      }
      else
      {
        data_expression_list v_auxiliary_set;
        data_expression_list v_iterate_over_set;
        data_expression_list v_iterate_over_path;
        data_expression v_guard_from_set;
        data_expression v_guard_from_path;

        data_expression_list v_set = { a_guard };
        while (v_set != v_auxiliary_set)
        {
          v_auxiliary_set = v_set;
          v_iterate_over_set = v_set;
          while (!v_iterate_over_set.empty())
          {
            v_guard_from_set = v_iterate_over_set.front();
            v_iterate_over_set.pop_front();
            v_iterate_over_path = a_path;
            while (!v_iterate_over_path.empty())
            {
              v_guard_from_path = v_iterate_over_path.front();
              v_iterate_over_path.pop_front();
              if (variables_overlap(v_guard_from_set, v_guard_from_path))
              {
                v_set.push_front(v_guard_from_path);
                a_path = remove_one_element(a_path, v_guard_from_path);
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
    data_expression aux_simplify(
                        const data_expression &a_bdd,
                        const data_expression_list &a_path)
    {
      if (f_deadline != 0 && (f_deadline - time(nullptr)) < 0)
      {
        mCRL2log(log::debug) << "The time limit has passed." << std::endl;
        return a_bdd;
      }

      if (f_bdd_info.is_true(a_bdd) || f_bdd_info.is_false(a_bdd))
      {
        return a_bdd;
      }

      const data_expression v_guard = f_bdd_info.get_guard(a_bdd);
      const data_expression v_negated_guard = sort_bool::not_(v_guard);
      const data_expression_list v_true_condition = create_condition(a_path, v_guard, true);
      bool v_true_branch_enabled = f_smt_solver->is_satisfiable(v_true_condition);
      if (!v_true_branch_enabled)
      {
        data_expression_list v_false_path=a_path;
        v_false_path.push_front(v_negated_guard);
        return aux_simplify(f_bdd_info.get_false_branch(a_bdd), v_false_path);
      }
      else
      {
        data_expression_list v_false_condition = create_condition(a_path, v_negated_guard, true);
        bool v_false_branch_enabled = f_smt_solver->is_satisfiable(v_false_condition);
        if (!v_false_branch_enabled)
        {
          data_expression_list v_true_path = a_path;
          v_true_path.push_front(v_guard);
          return aux_simplify(f_bdd_info.get_true_branch(a_bdd), v_true_path);
        }
        else
        {
          data_expression_list v_true_path = a_path;
          v_true_path.push_front(v_guard);
          data_expression_list v_false_path = a_path;
          v_false_path.push_front(v_negated_guard);
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
    bool variables_overlap(
                    const data_expression &a_expression_1,
                    const data_expression &a_expression_2)
    {
      std::set < variable > set1=find_all_variables(a_expression_1);
      std::set < variable > set2=find_all_variables(a_expression_2);
      std::set < variable > intersection;
      std::set_intersection(set1.begin(),set1.end(),
                            set2.begin(),set2.end(),
                            std::insert_iterator<std::set < variable > >(intersection,intersection.begin()));
      return !intersection.empty();
    }

  public:

    /// \brief Constructor that initializes the field BDD_Path_Eliminator::f_smt_solver.
    /// \param a_solver_type A value of an enumerated type, representing an SMT solver.
    BDD_Path_Eliminator(smt_solver_type a_solver_type)
    {
#if !(defined(_MSC_VER) || defined(__MINGW32__) || defined(__CYGWIN__))
      if (a_solver_type == solver_type_cvc)
      {
        if (mcrl2::data::detail::prover::cvc_smt_solver::usable())
        {
          f_smt_solver = new mcrl2::data::detail::prover::cvc_smt_solver();

          return;
        }
      }
      else
      {
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
    virtual data_expression simplify(const data_expression& a_bdd)
    {
      return aux_simplify(a_bdd, data_expression_list());
    }
};

}
}
}

#endif
