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
#include "mcrl2/data/detail/prover/bdd_simplifier.h"
#include "mcrl2/data/detail/prover/smt_lib_solver.h"
#include "mcrl2/data/detail/prover/smt_solver_cvc_fast.h"
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
    /// \brief Returns a list representing the conjunction of all guards in a_path and the guard a_guard.
    ATermList create_condition(ATermList a_path, ATermAppl a_guard, bool a_minimal);

    /// \brief Simplifies the BDD a_bdd using path a_path. Paths whose guards in conjunction with the guards in
    /// \brief a_path are inconsistent are removed.
    ATermAppl aux_simplify(ATermAppl a_bdd, ATermList a_path);

    /// \brief Returns true if the expression a_expression_1 has variables in common with expression a_expression_2.
    bool variables_overlap(ATermAppl a_expression_1, ATermAppl a_expression_2);

    /// \brief Pointer to an SMT solver used to determine whether or not a path is inconsistent.
    SMT_Solver* f_smt_solver;

    /// \brief Class that provides information about the structure of BDDs.
    BDD_Info f_bdd_info;

    /// \brief Class that can be used to manipulate BDDs.
    BDD_Manipulator f_bdd_manipulator;

    /// \brief Class that provides information about expressions.
    Expression_Info f_expression_info;
  public:
    /// \brief Constructor that initializes the field BDD_Path_Eliminator::f_smt_solver.
    BDD_Path_Eliminator(SMT_Solver_Type a_solver_type);

    /// \brief Returns a BDD without inconsistent paths, equivalent to a_bdd.
    /// precondition: The argument passed as parameter a_bdd is a data expression in internal mCRL2 format with the
    /// following restrictions: It either represents the constant true or the constant false, or it is an if-then-else
    /// expression with an expression of sort Bool  as guard, and a then-branch and an else-branch that again follow
    /// these restrictions
    virtual ATermAppl simplify(ATermAppl a_bdd);
};

    }
  }
}

#endif
