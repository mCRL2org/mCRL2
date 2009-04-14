// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/prover.h

#ifndef PROVER_H
#define PROVER_H

#include "aterm2.h"
#include "mcrl2/data/rewrite.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/utilities/manipulator.h"
#include "mcrl2/utilities/info.h"
#include "mcrl2/utilities/utilities.h"
#include "time.h"

  /// \brief An enumerated type respresenting the answers "yes", "no" and "undefined".
  /// The term "formula" in the following text denotes arbitrary expressions of sort Bool in the mCRL2 format.
  ///
  /// The Prover class is a base class for provers. Provers take formulas as input and indicate if these formulas are
  /// tautologies or contradictions. Since provers are not necessarily complete, a prover can be unable to determine if a
  /// formula is a tautology or a contradiction for some formulas. If this is the case, the prover will indicate this fact.
  /// A prover uses a rewriter to rewrite parts of the formulas it manipulates. The constructor Prover::Prover initializes
  /// the prover's rewriter with the data equations in internal mCRL2 format passed as parameter a_equations and the rewrite
  /// strategy passed as parameter a_strategy. To limit the number of seconds spent on proving a single formula, a time
  /// limit can be set. This time limit can be set initially by the constructor Prover::Prover and can be changed afterwards
  /// using the method Prover::set_time_limit. If the time limit is set to 0, no time limit will be enforced.
  ///
  /// Once a prover is created, the formula to be proven can be set using the method Prover::set_formula. The method
  /// Prover::set_formula takes a propositional formula in internal mCRL2 format as parameter a_formula.
  ///
  /// The methods Prover::is_tautology and Prover::is_contradiction can then indicate whether or not this formula is a
  /// tautology or a contradiction. These methods will return answer_yes, answer_no or answer_undefined.
  ///
  /// If a formula is neither a tautology nor a contradiction according to the prover, a so called witness or counter
  /// example can be returned by the methods Prover::get_witness and Prover::get_counter_example. A witness is a valuation
  /// for which the formula holds, a counter example is a valuation for which it does not hold.

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
      mcrl2::data::data_specification a_data_spec,
      RewriteStrategy a_rewrite_strategy = GS_REWR_JITTY,
      int a_time_limit = 0
    );

    /// \brief Destroys Prover::f_manipulator, Prover::f_info and Prover::f_rewriter.
    virtual ~Prover();

    /// \brief Sets Prover::f_formula to a_formula.
    /// precondition: the argument passed as parameter a_formula is an expression of sort Bool in internal mCRL2 format
    void set_formula(ATermAppl a_formula);

    /// \brief Sets Prover::f_time_limit to the value a_time_limit.
    /// precondition: the argument passed as parameter a_time_limit is greater than or equal to 0. If the argument is equal
    /// to 0, no time limit will be enforced
    /// precondition: the argument passed as parameter a_equations is a specification of data equations in internal mCRL2
    /// format
    void set_time_limit(int a_time_limit);

    /// \brief Indicates whether or not the formula Prover::f_formula is a tautology.
    /// precondition: the method Prover::set_formula has been called
    virtual Answer is_tautology() = 0;

    /// \brief Indicates whether or not the formula Prover::f_formula is a contradiction.
    /// precondition: the method Prover::set_formula has been called
    virtual Answer is_contradiction() = 0;

    /// \brief Returns a witness corresponding to a situation for which the formula Prover::f_formula holds.
    /// precondition: the method Prover::set_formula has been called
    virtual ATermAppl get_witness() = 0;

    /// \brief Returns a counterexample corresponding to a situation for which the formula Prover::f_formula does not hold.
    /// precondition: the method Prover::set_formula has been called
    virtual ATermAppl get_counter_example() = 0;

    /// \brief Returns the rewriter used by this prover (i.e. it returns Prover::f_rewriter).
    Rewriter *get_rewriter();
};

#endif
