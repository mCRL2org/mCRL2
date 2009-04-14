// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/bdd_prover.h
/// \brief EQ-BDD based prover for mCRL2 boolean data expressions

#ifndef BDD_PROVER_H
#define BDD_PROVER_H

#include "aterm2.h"
#include "mcrl2/data/rewrite.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/prover.h"
#include "mcrl2/data/prover/bdd_simplifier.h"
#include "mcrl2/data/prover/bdd_path_eliminator.h"
#include "mcrl2/data/detail/prover/induction.h"
#include "mcrl2/utilities/utilities.h"

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
    ATermAppl f_data_spec;

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
    void build_bdd();

    /// \brief Creates the EQ-BDD corresponding to the formula a_formula.
    ATerm bdd_down(ATerm a_formula);

    /// \brief Creates the EQ-BDD corresponding to the formula a_formula.
    ATerm bdd_down(ATerm a_formula, std::string& a_indent);

    /// \brief Removes all inconsistent paths from the BDD BDD_Prover::f_bdd.
    void eliminate_paths();

    /// \brief Updates the values of Prover::f_tautology and Prover::f_contradiction.
    void update_answers();

    /// \brief Returns the smallest guard in the formula a_formula.
    ATerm smallest(ATerm a_formula);

    /// \brief Returns branch of the BDD a_bdd, depending on the polarity a_polarity.
    ATermAppl get_branch(ATermAppl a_bdd, bool a_polarity);
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
    BDD_Prover(
      mcrl2::data::data_specification data_spec,
      RewriteStrategy a_rewrite_strategy = GS_REWR_JITTY,
      int a_time_limit = 0,
      bool a_path_eliminator = false,
      SMT_Solver_Type a_solver_type = solver_type_ario,
      bool a_apply_induction = false
    );

    /// \brief Destructor that destroys the BDD simplifier BDD_Prover::f_bdd_simplifier.
    virtual ~BDD_Prover();

    /// \brief Indicates whether or not the formula Prover::f_formula is a tautology.
    virtual Answer is_tautology();

    /// \brief Indicates whether or not the formula Prover::f_formula is a contradiction.
    virtual Answer is_contradiction();

    /// \brief Returns the BDD BDD_Prover::f_bdd.
    virtual ATermAppl get_bdd();

    /// \brief Returns all the guards on a path in the BDD that leads to a leaf labelled "true", if such a leaf exists.
    virtual ATermAppl get_witness();

    /// \brief Returns all the guards on a path in the BDD that leads to a leaf labelled "false", if such a leaf exists.
    virtual ATermAppl get_counter_example();
};

#endif
