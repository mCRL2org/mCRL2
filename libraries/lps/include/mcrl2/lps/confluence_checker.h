// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/confluence_checker.h
/// \brief Add your file description here.

// Interface to class Confluence_Checker
// file: confluence_checker.h

#ifndef CONFLUENCE_CHECKER_H
#define CONFLUENCE_CHECKER_H

#include <string>
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/detail/prover/solver_type.h"
#include "mcrl2/data/detail/bdd_prover.h"
#include "mcrl2/lps/disjointness_checker.h"
#include "mcrl2/lps/invariant_checker.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/data/detail/prover/bdd2dot.h"
#include "mcrl2/lps/linear_process.h"

/** \brief A class that takes a linear process specification and checks all tau-summands of that LPS for confluence.
    \brief The tau actions of all confluent tau-summands are renamed to ctau.

     Given an LPS,

       P(d: D) = ...
               + sum ei: Ei. ci(d, ei) -> ai(fi(d, ei)) . P(gi(d, ei))
               + ...
               + sum ej: Ej. cj(d, ej) -> tau . P(gj(d, ej))
               + ...;


    tau-summand j is confluent with summand i if the following condition holds for all d: D, for all ei: Ei and for all
     ej: Ej:

       (inv(d) /\ ci(d, ei) /\ cj(d, ej))
       =>
       (
        ci(gj(d, ej), ei) /\
        cj(gi(d, ei), ej) /\
        fi(d, ei) == fi(gj(d, ej), ei) /\
        gi(gj(d, ej), ei) == gj(gi(d, ei), ej)
       )

    where inv() is the invariant specified using the parameter a_invariant of the function
    Confluence_Checker::check_confluence_and_mark. In case ai is also a tau-action, the formula above can be weakened to
    the following:

       (inv(d) /\ ci(d, ei) /\ cj(d, ej))
       =>
       (
        gi(d, ei) == gj(d, ej) \/
        (
         ci(gj(d, ej), ei) /\
         cj(gi(d, ei), ej) /\
         gi(gj(d, ej), ei) == gj(gi(d, ei), ej)
        )
       )

    The class Confluence_Checker can determine whether two summands are confluent in three ways and will indicate which
    of the methods was used while proving confluence. The three ways of determining confluence are as follows:

         If summand number 1 has been proven confluent with summand number 2, summand number 2 is obviously confluent
    with summand number 1. This method of checking confluence is called checking confluence by symmetry. If two summands
    are confluent by symmetry, the class Confluence_Checker indicates this by printing a dot ('.').

         Another way of checking the confluence of two summands is determining whether the two summands are
    syntactically disjoint. Two summands are syntactically disjoint if the following holds:
    - The set of variables used by one summand is disjoint from the set of variables changed by the other summand and
      vice versa.
    - The set of variables changed by one summand is disjoint from the set of variables changed by the other summand.
    If two summands are confluent because of syntactic disjointness, the class Confluence_Checker indicates this by
    printing a colon (':').

         The most time consuming way of checking the confluence of two summands is generating the confluence condition
    and then checking if this condition is a tautology using a prover for expressions of sort Bool. If two summands are
    proven confluent using the prover, the class Confluence_Checker indicates this by printing a plus sign ('+'). If the
    parameter a_generate_invariants is set to true, the class Confluence_Checker will try to prove that the reduced
    confluence condition is an invariant of the LPS, in case the confluence condition is not a tautology. If the reduced
    confluence condition is indeed an invariant, the two summands are proven confluent. The class Confluence_Checker
    indicates this by printing an 'i'.

    The class Confluence_Checker uses an instance of the class BDD_Prover, an instance of the class Disjointness_Checker
    and an instance of the class Invariant_Checker to determine which tau-summands of an mCRL2 LPS are confluent.
    Confluent tau-summands will be marked by renaming their tau-actions to ctau. The constructor
    Confluence_Checker::Confluence_Checker initializes the BDD based prover with the parameters a_rewrite_strategy,
    a_time_limit, a_path_eliminator, a_solver_type, a_apply_induction and a_lps. The parameter a_rewrite_strategy
    specifies which rewrite strategy is used by the prover's rewriter. It can be set to either
    GS_REWR_JITTY or GS_REWR_JITTYC. The parameter a_time_limit specifies the maximum amount of time in
    seconds to be spent by the prover on proving a single expression. If a_time_limit is set to 0, no time limit will be
    enforced. The parameter a_path_eliminator specifies whether or not path elimination is applied. When path
    elimination is applied, the prover uses an SMT solver to remove inconsistent paths from BDDs. The parameter
    a_solver_type specifies which SMT solver is used for path elimination. Either the SMT solver ario
    (http://www.eecs.umich.edu/~ario/) or cvc-lite (http://www.cs.nyu.edu/acsys/cvcl/) can be used. To use one of these
    solvers, the directory containing the corresponding executable must be in the path. If the parameter
    a_path_eliminator is set to false, the parameter a_solver_type is ignored. The parameter a_apply_induction indicates
    whether or not induction on list will be applied.

    The parameter a_dot_file_name specifies whether a file in dot format of the resulting BDD is saved each time the
    prover cannot determine whether an expression of sort Bool is a contradiction or a tautology. If the parameter is
    set to 0, no .dot files are saved. If a string is passed as parameter a_dot_file_name, this string will be used as
    the prefix of the filenames. An instance of the class BDD2Dot is used to save these files in dot format.

    If the parameter a_counter_example is set to true, a so called counter example is printed to stderr each time the
    prover indicates that two summands are not confluent. A counter example is a valuation for which the confluence
    condition to be proven does not hold.

    If the parameter a_check_all is set to true, the confluence of the tau-summands regarding all other summands will be
    checked. If the parameter is set to false, Confluence_Checker continues with the next tau-summand as soon as a
    summand is encountered that is not confluent with the current tau-summand.

    If the parameter a_generate_invariants is set, an invariant checker is used to check if the reduced confluence
    condition is an invariant of the LPS passed as parameter a_lps. If the reduced confluence condition is an invariant,
    the two summands are confluent.

    The function Confluence_Checker::check_confluence_and_mark returns an LPS with all tau-actions of confluent
    tau-summands renamed to ctau, unless the parameter a_no_marking is set to true. In case the parameter a_no_marking
    was set to true, the confluent tau-summands will not be marked, only the results of the confluence checking will be
    displayed.

    If there already is an action named ctau present in the LPS passed as parameter a_lps, an error will be reported. */


namespace mcrl2
{
namespace lps
{
namespace detail
{

/**
 * \brief Creates an identifier for the for the ctau action
 **/
inline action_label make_ctau_act_id()
{
  static atermpp::aterm_appl ctau_act_id = mcrl2::core::detail::gsMakeActId(atermpp::aterm_appl(atermpp::function_symbol("ctau", 0, true)), atermpp::aterm_list());

  assert(&*ctau_act_id);

  return action_label(ctau_act_id);
}

/**
 * \brief Creates the ctau action
 **/
inline action make_ctau_action()
{
  static atermpp::aterm_appl ctau_action = mcrl2::core::detail::gsMakeAction(make_ctau_act_id(), atermpp::aterm_list());

  assert(&*ctau_action);

  return action(ctau_action);
}


class Confluence_Checker
{
  private:
    /// \brief Class that can check if two summands are disjoint.
    Disjointness_Checker f_disjointness_checker;

    /// \brief Class that checks if an invariant holds for an LPS.
    Invariant_Checker f_invariant_checker;

    /// \brief BDD based prover.
    mcrl2::data::detail::BDD_Prover f_bdd_prover;

    /// \brief Class that prints BDDs in dot format.
    mcrl2::data::detail::BDD2Dot f_bdd2dot;

    /// \brief A linear process specification.
    const mcrl2::lps::specification& f_lps;

    /// \brief Flag indicating whether or not the tau actions of confluent tau summands are renamed to ctau.
    bool f_no_marking;

    /// \brief Flag indicating whether or not the process of checking the confluence of a summand stops when
    /// \brief a summand is encountered that is not confluent with the tau summand at hand.
    bool f_check_all;

    /// \brief Flag indicating whether or not counter examples are printed.
    bool f_counter_example;

    /// \brief The prefix for the names of the files written in dot format.
    std::string f_dot_file_name;

    /// \brief Flag indicating whether or not invariants are generated and checked each time a
    /// \brief summand is encountered that is not confluent with the tau summand at hand.
    bool f_generate_invariants;

    /// \brief The number of summands of the current LPS.
    size_t f_number_of_summands;

    /// \brief An integer array, storing intermediate results per summand.
    std::vector <size_t> f_intermediate;

    /// \brief Writes a dot file of the BDD created when checking the confluence of summands a_summand_number_1 and a_summand_number_2.
    void save_dot_file(size_t a_summand_number_1, size_t a_summand_number_2);

    /// \brief Outputs a path in the BDD corresponding to the condition at hand that leads to a node labelled false.
    void print_counter_example();

    /// \brief Checks the confluence of summand a_summand_1 and a_summand_2
    bool check_summands(
      const data::data_expression a_invariant,
      const action_summand a_summand_1,
      const size_t a_summand_number_1,
      const action_summand a_summand_2,
      const size_t a_summand_number_2);

    /// \brief Checks the confluence of summand a_summand concerning all other tau-summands.
    action_summand check_confluence_and_mark_summand(
      const data::data_expression a_invariant,
      const action_summand a_summand,
      const size_t a_summand_number,
      bool& a_is_marked);

  public:
    /// \brief Constructor that initializes Confluence_Checker::f_lps, Confluence_Checker::f_bdd_prover,
    /// \brief Confluence_Checker::f_generate_invariants and Confluence_Checker::f_dot_file_name.
    /// precondition: the argument passed as parameter a_lps is a valid mCRL2 LPS
    /// precondition: the argument passed as parameter a_time_limit is greater than or equal to 0. If the argument is equal
    /// to 0, no time limit will be enforced
    Confluence_Checker
    (
      mcrl2::lps::specification const& a_lps,
      mcrl2::data::rewriter::strategy a_rewrite_strategy = mcrl2::data::jitty,
      int a_time_limit = 0,
      bool a_path_eliminator = false,
      mcrl2::data::detail::smt_solver_type a_solver_type = mcrl2::data::detail::solver_type_cvc,
      bool a_apply_induction = false,
      bool a_no_marking = false,
      bool a_check_all = false,
      bool a_counter_example = false,
      bool a_generate_invariants = false,
      std::string const& a_dot_file_name = std::string()
    );

    /// \brief Destructor that frees the memory used by Confluence_Checker::f_dot_file_name.
    ~Confluence_Checker();

    /// \brief Check the confluence of the LPS Confluence_Checker::f_lps.
    /// precondition: the argument passed as parameter a_invariant is an expression of sort Bool in internal mCRL2 format
    /// precondition: the argument passed as parameter a_summand_number corresponds with a summand of the LPS for which
    /// confluence must be checked (lowest summand has number 1). If this number is 0 confluence for all summands is checked.
    specification check_confluence_and_mark(const data::data_expression a_invariant, const size_t a_summand_number);
};

} // namespace detail
} // namespace lps
} // namespace mcrl2
#endif
