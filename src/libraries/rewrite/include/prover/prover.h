// Interface to class Prover
// file: prover.h

#ifndef PROVER_H
#define PROVER_H

#include "aterm2.h"
#include "librewrite.h"
#include "lpe/data_specification.h"
#include "auxiliary/manipulator.h"
#include "auxiliary/info.h"
#include "auxiliary/utilities.h"
#include "time.h"

  /// \brief An enumerated type respresenting the answers "yes", "no" and "undefined".

enum Answer {
  answer_yes,
  answer_no,
  answer_undefined
};

  /// \brief A base class for provers. Provers take an expression of sort Bool in internal mCRL2 format and
  /// \brief can indicate whether or not this expression is a tautology or a contradiction.

class Prover {
  protected:
    /// \brief An expression of sort Bool in the internal format of mCRL2.
    ATermAppl f_formula;

    /// \brief A rewriter that rewrites expressions given a set of rewrite rules.
    Rewriter* f_rewriter;

    /// \brief A class that can be used to manipulate expressions in the internal format of the rewriter.
    ATerm_Manipulator* f_manipulator;

    /// \brief A class that provides information about expressions in the internal format of the rewriter.
    ATerm_Info* f_info;

    /// \brief A flag that indicates whether or not the formala Prover::f_formula has been processed.
    bool f_processed;

    /// \brief A flag that indicates whether or not the formala Prover::f_formula is a tautology.
    Answer f_tautology;

    /// \brief A flag that indicates whether or not the formala Prover::f_formula is a contradiction.
    Answer f_contradiction;

    /// \brief An integer representing the maximal amount of seconds to be spent on processing a formula.
    int f_time_limit;

    /// \brief A timestamp representing the moment when the maximal amount of seconds has been spent on processing the current formula.
    time_t f_deadline;
  public:
    /// \brief Constructor that initializes Prover::f_rewriter and Prover::f_time_limit.
    Prover(
      lpe::data_specification a_data_spec,
      RewriteStrategy a_rewrite_strategy = GS_REWR_JITTY,
      int a_time_limit = 0
    );

    /// \brief Destroys Prover::f_manipulator, Prover::f_info and Prover::f_rewriter.
    virtual ~Prover();

    /// \brief Sets Prover::f_formula to a_formula.
    void set_formula(ATermAppl a_formula);

    /// \brief Sets Prover::f_time_limit to the value a_time_limit.
    void set_time_limit(int a_time_limit);

    /// \brief Indicates whether or not the formula Prover::f_formula is a tautology.
    virtual Answer is_tautology() = 0;

    /// \brief Indicates whether or not the formula Prover::f_formula is a contradiction.
    virtual Answer is_contradiction() = 0;

    /// \brief Returns a witness corresponding to a situation for which the formula Prover::f_formula holds.
    virtual ATermAppl get_witness() = 0;

    /// \brief Returns a counterexample corresponding to a situation for which the formula Prover::f_formula does not hold.
    virtual ATermAppl get_counter_example() = 0;

    /// \brief Returns the rewriter used by this prover (i.e. it returns Prover::f_rewriter).
    Rewriter *get_rewriter();
};

#endif
