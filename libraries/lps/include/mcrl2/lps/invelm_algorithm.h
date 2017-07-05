// Author(s): Luc Engelen, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/invelm_algorithm.h
/// \brief Interface to class invariant_eliminator

#ifndef MCRL2_LPS_INVELM_ALGORITHM_H
#define MCRL2_LPS_INVELM_ALGORITHM_H

#include "mcrl2/data/detail/bdd_prover.h"
#include "mcrl2/data/detail/prover/solver_type.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/lps/detail/lps_algorithm.h"
#include "mcrl2/lps/invariant_checker.h"

namespace mcrl2
{

namespace lps
{

/// The class invariant_eliminator is initialized with an LPS using the constructor
/// invariant_eliminator::invariant_eliminator. After initialization, the method invariant_eliminator::simplify can be
/// called any number of times to simplify or eliminate the summands of this LPS using an invariant. A new instance of the
/// class invariant_eliminator has to be created for each new LPS that has to be processed.
///
/// The class invariant_eliminator uses an instance of the class BDD_Prover to simplify or eliminate summands of an mCRL2
/// LPS. The constructor invariant_eliminator::invariant_eliminator initializes the BDD based prover with the parameters
/// a_rewrite_strategy, a_time_limit, a_path_eliminator, a_solver_type, and the data specification of the LPS passed as
/// parameter a_lps. The parameter a_rewrite_strategy specifies which rewrite strategy is used by the prover's rewriter.
/// It can be set to either GS_REWR_JITTY or GS_REWR_JITTYC. The parameter a_time_limit
/// specifies the maximum amount of time in seconds to be spent by the prover on proving a single expression. If
/// a_time_limit is set to 0, no time limit will be enforced. The parameter a_path_eliminator specifies whether or not
/// path elimination is applied. When path elimination is applied, the prover uses an SMT solver to remove inconsistent
/// paths from BDDs. The parameter a_solver_type specifies which SMT solver is used for path elimination. Either the SMT
/// solver ario (http://www.eecs.umich.edu/~ario/) or cvc-lite (http://www.cs.nyu.edu/acsys/cvcl/) can be used. To use one
/// of these solvers, the directory containing the corresponding executable must be in the path. If the parameter
/// a_path_eliminator is set to false, the parameter a_solver_type is ignored. The parameter a_apply_induction indicates
/// whether or not induction on list will be applied.
///
/// The parameter a_dot_file_name specifies whether a file in dot format of the resulting BDD is saved each time the
/// prover cannot determine whether an expression is a contradiction or a tautology. If the parameter is set to 0, no .dot
/// files are saved. If a string is passed as parameter a_dot_file_name, this string will be used as the prefix of the
/// filenames. An instance of the class BDD2Dot is used to save these files in dot format.
///
/// If the parameter a_simplify_all is set to false, the summands whose conditions in conjunction with the invariant prove
/// to be a contradiction are eliminated. If parameter a_simplify_all is set to true, in addition to removing those
/// summands, the conditions of all other summands of the LPS are simplified when calling method
/// invariant_eliminator::simplify.
///
/// The method invariant_eliminator::simplify returns the LPS passed as parameter a_lps of the constructor after
/// simplification or elimination using the invariant passed as parameter a_invariant. If a_summand_number differs from 0,
/// only the summand with the number passed as parameter a_summand_number will be eliminated or simplified. One instance
/// of the class invariant_eliminator can check any number of invariants on the same LPS. For each new LPS to be checked,
/// a new instance of the class has to be created.
///
/// Given an LPS,
///
///    P(d: D) = ...
///            + sum ei: Ei. ci(d, ei) -> ai(fi(d, ei)) . P(gi(d, ei))
///            + ...;
///
/// an instance of the class invariant_eliminator will generate a formula of the form
///
///    inv(d) /\ ci(d, ei)
///
/// for each of the summands or for the chosen summand only, where inv() is the invariant passed as parameter a_invariant.
/// If such a formula is a contradiction according to the prover, the corresponding summand will be eliminated. If the
/// parameter a_simplify_all is set, the condition of each summand is replaced by the BDD obtained from the prover after
/// proving that summand's formula.

template <class Specification>
class invelm_algorithm: public detail::lps_algorithm<Specification>
{
  typedef typename detail::lps_algorithm<Specification> super;
  typedef typename Specification::process_type process_type;
  typedef typename process_type::action_summand_type action_summand_type;
  using super::m_spec;

  private:
    data::detail::BDD_Prover f_bdd_prover;
    const specification f_lps;
    bool f_simplify_all;

    /// \brief Adds an invariant to the condition of the summand s, and optionally applies the prover to it
    /// \param s The summand that needs to be simplified
    /// \param invariant A data expression
    /// \param apply_prover If true, the prover is applied to the condition
    void simplify_summand(summand_base& s, const data::data_expression& invariant, bool apply_prover)
    {
      data::data_expression new_condition = data::lazy::and_(invariant, s.condition());

      if (apply_prover)
      {
        f_bdd_prover.set_formula(new_condition);
        new_condition = f_bdd_prover.get_bdd();
      }
      if (f_simplify_all || data::sort_bool::is_false_function_symbol(new_condition))
      {
        s.condition() = new_condition;
      }
    }

    template <typename SummandSequence>
    void simplify_summands(SummandSequence& summands, const data::data_expression& invariant, bool apply_prover)
    {
      for (summand_base& s: summands)
      {
        simplify_summand(s, invariant, apply_prover);
      }
    }

  public:
    /// precondition: the argument passed as parameter a_lps is a valid mCRL2 LPS
    /// precondition: the argument passed as parameter a_time_limit is greater than or equal to 0. If the argument is equal
    /// to 0, no time limit will be enforced
    invelm_algorithm(
      Specification& a_lps,
      const data::rewriter::strategy a_rewrite_strategy = data::jitty,
      const int a_time_limit = 0,
      const bool a_path_eliminator = false,
      const data::detail::smt_solver_type a_solver_type = data::detail::solver_type_cvc,
      const bool a_apply_induction = false,
      const bool a_simplify_all = false
    )
      : detail::lps_algorithm<Specification>(a_lps),
        f_bdd_prover(a_lps.data(), data::used_data_equation_selector(a_lps.data()),a_rewrite_strategy, a_time_limit, a_path_eliminator, a_solver_type, a_apply_induction),
        f_simplify_all(a_simplify_all)
    {}

    void run(const data::data_expression& invariant, bool apply_prover)
    {
      simplify_summands(m_spec.process().action_summands(), invariant, apply_prover);
      simplify_summands(m_spec.process().deadlock_summands(), invariant, apply_prover);
      super::remove_trivial_summands();
    }
};

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_INVELM_ALGORITHM_H
