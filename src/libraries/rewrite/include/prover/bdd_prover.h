// Interface to class BDD_Prover
// file: bdd_prover.h

#ifndef BDD_PROVER_H
#define BDD_PROVER_H

#include "aterm2.h"
#include "librewrite.h"
#include "prover/prover.h"
#include "prover/bdd_simplifier.h"
#include "prover/bdd_path_eliminator.h"
#include "prover/induction.h"
#include "auxiliary/utilities.h"

  /// \brief A class based on the Prover class that takes an expression of sort Bool in internal mCRL2 format
  /// \brief and creates the corresponding EQ-BDD. Using this EQ-BDD, the class can determine if the original
  /// \brief formula is a tautology or a contradiction.

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

    /// \brief Class that produces a number of spaces corresponding to an indentation level.
    Indent f_indent;

    /// \brief Constructs the EQ-BDD corresponding to the formula Prover::f_formula.
    void build_bdd();

    /// \brief Creates the EQ-BDD corresponding to the formula a_formula.
    ATerm bdd_down(ATerm a_formula);

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
    BDD_Prover(
      ATermAppl a_data_spec,
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
