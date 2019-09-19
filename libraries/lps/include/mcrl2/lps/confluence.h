// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/confluence.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_CONFLUENCE_H
#define MCRL2_LPS_CONFLUENCE_H

#include <memory>
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/consistency.h"
#include "mcrl2/data/join.h"
#include "mcrl2/data/rewriters/one_point_rule_rewriter.h"
#include "mcrl2/data/rewriters/quantifiers_inside_rewriter.h"
#include "mcrl2/data/standard.h" // equal_to
#include "mcrl2/data/substitution_utility.h"
#include "mcrl2/lps/stochastic_specification.h"
#include "mcrl2/smt/solver.h"

namespace mcrl2 {

namespace lps {

namespace detail {

inline
data::data_expression make_and(const data::data_expression& x1, const data::data_expression& x2, const data::data_expression& x3)
{
  return and_(x1, data::and_(x2, x3));
}

inline
data::data_expression make_and(const data::data_expression& x1, const data::data_expression& x2, const data::data_expression& x3, const data::data_expression& x4)
{
  return and_(data::and_(x1, x2), data::and_(x3, x4));
}

inline
data::data_expression equal_to(const data::data_expression_list& x, const data::data_expression_list& y)
{
  std::vector<data::data_expression> conjuncts;
  auto xi = x.begin();
  auto yi = y.begin();
  for (; xi != x.end(); ++xi, ++yi)
  {
    conjuncts.push_back(data::equal_to(*xi, *yi));
  }
  return data::join_and(conjuncts.begin(), conjuncts.end());
}

} // namespace detail

// TODO: reuse this code
template <typename Summand>
const stochastic_distribution& summand_distribution(const Summand& /* summand */)
{
  static stochastic_distribution empty_distribution;
  return empty_distribution;
}

// TODO: reuse this code
template <>
const stochastic_distribution& summand_distribution(const lps::stochastic_action_summand& summand)
{
  return summand.distribution();
}

inline
std::set<data::variable> used_read_variables(const action_summand& summand)
{
  std::set<data::variable> result;
  data::find_all_variables(summand.condition(), std::inserter(result, result.end()));
  data::find_all_variables(summand.multi_action(), std::inserter(result, result.end()));
  for (const auto& a : summand.assignments())
  {
    if (a.lhs() != a.rhs())
    {
      data::find_all_variables(a.rhs(), std::inserter(result, result.end()));
    }
  }
  return result;
}

inline
std::set<data::variable> changed_variables(const action_summand& summand)
{
  std::set<data::variable> result;
  for (const auto& a: summand.assignments())
  {
    result.insert(a.lhs());
  }
  return result;
}

// This is an alternative version of data::make_assignment_list that removes trivial assignments
inline
data::assignment_list make_assignment_list(const data::variable_list& variables, const data::data_expression_list& expressions)
{
  assert(variables.size() == expressions.size());
  std::vector<data::assignment> result;
  auto vi = variables.begin();
  auto ei = expressions.begin();
  for (; vi != variables.end(); ++vi, ++ei)
  {
    if (*vi != *ei)
    {
      result.emplace_back(*vi, *ei);
    }
  }
  return data::assignment_list(result.begin(), result.end());
}

struct confluence_summand
{
  data::variable_list variables;
  data::data_expression condition;
  lps::multi_action multi_action;
  stochastic_distribution distribution;
  data::data_expression_list next_state;
  std::set<data::variable> changed;
  std::set<data::variable> used;

  template <typename ActionSummand>
  confluence_summand(const ActionSummand& summand, const data::variable_list& process_parameters)
    : variables(summand.summation_variables()),
      condition(summand.condition()),
      multi_action(summand.multi_action().actions(), summand.multi_action().time()),
      distribution(summand_distribution(summand)),
      next_state(summand.next_state(process_parameters)),
      changed(changed_variables(summand)),
      used(used_read_variables(summand))
  {}

  bool is_tau() const
  {
    return multi_action.actions().empty();
  }
};

inline
std::string print_confluence_summand(const confluence_summand& summand, const data::variable_list& process_parameters)
{
  std::ostringstream out;
  out << lps::stochastic_action_summand(
    summand.variables,
    summand.condition,
    summand.multi_action,
    make_assignment_list(process_parameters, summand.next_state),
    summand.distribution
  );
  return out.str();
}

/// \brief Indicates whether or not two summands are disjoint.
inline
bool disjoint(const confluence_summand& summand1, const confluence_summand& summand2)
{
  using utilities::detail::has_empty_intersection;
  return has_empty_intersection(summand1.used, summand2.changed)
         && has_empty_intersection(summand2.used, summand1.changed)
         && has_empty_intersection(summand1.changed, summand2.changed);
}

/// \brief Creates an identifier for the ctau action
inline process::action_label make_ctau_act_id()
{
  static atermpp::aterm_appl ctau_act_id = atermpp::aterm_appl(core::detail::function_symbol_ActId(), atermpp::aterm_appl(atermpp::function_symbol("ctau", 0)), atermpp::aterm_list());
  return process::action_label(ctau_act_id);
}

/// \brief Creates the ctau action
inline process::action make_ctau_action()
{
  static atermpp::aterm_appl ctau_action = atermpp::aterm_appl(core::detail::function_symbol_Action(), make_ctau_act_id(), atermpp::aterm_list());
  return process::action(ctau_action);
}

template <typename Specification>
bool has_ctau_action(const Specification& lpsspec)
{
  using utilities::detail::contains;
  return contains(lpsspec.action_labels(), make_ctau_act_id());
}

/// \brief Function object that computes the condition for square confluence
/// \param summand_i A tau summand
/// \param summand_j An arbitrary action summand
struct square_confluence_condition
{
  const data::variable_list& process_parameters;
  data::mutable_indexed_substitution<>& sigma;

  square_confluence_condition(const data::variable_list& process_parameters_, data::mutable_indexed_substitution<>& sigma_)
   : process_parameters(process_parameters_), sigma(sigma_)
  {}

  data::data_expression operator()(const confluence_summand& summand_i, const confluence_summand& summand_j) const
  {
    assert(summand_i.is_tau());
    const auto& d = process_parameters;

    const auto& ci = summand_i.condition;
    const auto& gi = summand_i.next_state;

    const auto& cj = summand_j.condition;
    const auto& gj = summand_j.next_state;

    data::add_assignments(sigma, d, gi);
    data::data_expression cj_gi = data::replace_variables_capture_avoiding(cj, sigma);
    data::data_expression_list gj_gi = data::replace_variables_capture_avoiding(gj, sigma);

    data::add_assignments(sigma, d, gj);
    data::data_expression ci_gj = data::replace_variables_capture_avoiding(ci, sigma);
    data::data_expression_list gi_gj = data::replace_variables_capture_avoiding(gi, sigma);

    if (summand_j.is_tau())
    {
      data::remove_assignments(sigma, d);
      return imp(data::and_(ci, cj), or_(detail::equal_to(gi, gj), detail::make_and(ci_gj, cj_gi, detail::equal_to(gj_gi, gi_gj))));
    }
    else
    {
      const auto& fj = summand_j.multi_action.arguments();
      data::data_expression_list fj_gi = data::replace_variables_capture_avoiding(fj, sigma);
      data::remove_assignments(sigma, d);
      return imp(data::and_(ci, cj), detail::make_and(ci_gj, cj_gi, detail::equal_to(fj, fj_gi), detail::equal_to(gj_gi, gi_gj)));
    }
  }
};

/// \brief Function object that computes the condition for triangular confluence
/// \param summand_i A tau summand
/// \param summand_j An arbitrary action summand
struct triangular_confluence_condition
{
  const data::variable_list& process_parameters;
  data::mutable_indexed_substitution<>& sigma;

  triangular_confluence_condition(const data::variable_list& process_parameters_, data::mutable_indexed_substitution<>& sigma_)
    : process_parameters(process_parameters_), sigma(sigma_)
  {}

  inline
  data::data_expression operator()(const confluence_summand& summand_i, const confluence_summand& summand_j) const
  {
    assert(summand_i.is_tau());
    const auto& d = process_parameters;

    const auto& ci = summand_i.condition;
    const auto& gi = summand_i.next_state;

    const auto& cj = summand_j.condition;
    const auto& gj = summand_j.next_state;

    data::add_assignments(sigma, d, gi);
    data::data_expression cj_gi = data::replace_variables_capture_avoiding(cj, sigma);
    data::data_expression_list gj_gi = data::replace_variables_capture_avoiding(gj, sigma);

    if (summand_j.is_tau())
    {
      data::remove_assignments(sigma, d);
      return imp(and_(ci, cj), and_(cj_gi, detail::equal_to(gj_gi, gj)));
    }
    else
    {
      const auto& fj = summand_j.multi_action.arguments();
      data::data_expression_list fj_gi = data::replace_variables_capture_avoiding(fj, sigma);
      data::remove_assignments(sigma, d);
      return imp(and_(ci, cj), detail::make_and(cj_gi, detail::equal_to(fj, fj_gi), detail::equal_to(gj_gi, gj)));
    }
  }
};

/// \brief Function object that computes the condition for triangular confluence
/// \param summand_i A tau summand
/// \param summand_j An arbitrary action summand
struct trivial_confluence_condition
{
  data::mutable_indexed_substitution<>& sigma;

  explicit trivial_confluence_condition(data::mutable_indexed_substitution<>& sigma_)
    : sigma(sigma_)
  {}

  inline
  data::data_expression operator()(const confluence_summand& summand_i, const confluence_summand& summand_j) const
  {
    assert(summand_i.is_tau());
    data::data_expression aj_is_tau = summand_j.is_tau() ? data::true_() : data::false_();

    const auto& ci = summand_i.condition;
    const auto& gi = summand_i.next_state;

    const auto& cj = summand_j.condition;
    const auto& gj = summand_j.next_state;

    return imp(and_(ci, cj), and_(aj_is_tau, detail::equal_to(gi, gj)));
  }
};

class confluence_checker
{
  protected:
    std::vector<confluence_summand> m_summands;
    data::variable_list m_process_parameters;
    data::rewriter m_rewr;
    mutable data::mutable_indexed_substitution<> m_sigma;
    std::unique_ptr<smt::smt_solver> m_solver;

    // check if x is a tautology using the data rewriter
    bool is_tautology_rewriter(data::data_expression x) const
    {
      data::one_point_rule_rewriter R_one_point;
      data::quantifiers_inside_rewriter R_quantifiers_inside;
      x = R_one_point(R_quantifiers_inside(x));
      std::set<data::variable> freevars = data::find_free_variables(x);
      x = data::make_forall(data::variable_list(freevars.begin(), freevars.end()), x);
      x = m_rewr(x);
      return is_true(x);
    }

    // check if x is a tautology using an smt solver
    bool is_tautology_smt(const data::data_expression& x) const
    {
      std::set<data::variable> freevars = data::find_free_variables(x);
      // determine if the negation is satisfiable
      bool result = m_solver->solve(data::variable_list(freevars.begin(), freevars.end()), data::sort_bool::not_(x));
      return !result;
    }

    // check if x is a tautology
    bool is_tautology(const data::data_expression& x) const
    {
      return m_solver ? is_tautology_smt(x) : is_tautology_rewriter(x);
    }

    // Returns whether tau_summand is confluent. If not, the second value returned is the index of the summand
    // for which a violation was detected.
    template <typename ConfluenceCondition>
    std::pair<bool, std::size_t> is_confluent(const confluence_summand& tau_summand, ConfluenceCondition confluence_condition, bool check_disjointness) const
    {
      for (std::size_t i = 0; i < m_summands.size(); i++)
      {
        const confluence_summand& summand =  m_summands[i];

        if (check_disjointness && disjoint(tau_summand, summand))
        {
          mCRL2log(log::info) << ':';
          continue;
        }

        data::data_expression condition = confluence_condition(tau_summand, summand);
        mCRL2log(log::debug) << "\nsummand = " << print_confluence_summand(summand, m_process_parameters);
        mCRL2log(log::debug) << "\ntau summand = " << print_confluence_summand(tau_summand, m_process_parameters);
        mCRL2log(log::debug) << "\nconfluence condition = " << condition << std::endl;
        mCRL2log(log::debug) << "\nrewritten confluence condition = " << m_rewr(condition) << std::endl;
        if (!is_tautology(condition))
        {
          return { false, i };
        }
        mCRL2log(log::info) << '+';
      }
      return { true, 0 };
    }

  public:
    std::pair<bool, std::size_t> is_square_confluent(const confluence_summand& tau_summand) const
    {
      bool check_disjointness = true;
      return is_confluent(tau_summand, square_confluence_condition(m_process_parameters, m_sigma), check_disjointness);
    }

    std::pair<bool, std::size_t> is_triangular_confluent(const confluence_summand& tau_summand) const
    {
      bool check_disjointness = false;
      return is_confluent(tau_summand, triangular_confluence_condition(m_process_parameters, m_sigma), check_disjointness);
    }

    std::pair<bool, std::size_t> is_trivial_confluent(const confluence_summand& tau_summand) const
    {
      bool check_disjointness = false;
      return is_confluent(tau_summand, trivial_confluence_condition(m_sigma), check_disjointness);
    }

    template <typename Specification>
    std::vector<std::size_t> compute_confluent_summands(const Specification& lpsspec, char confluence_type)
    {
      std::vector<std::size_t> result;

      m_summands.clear();
      m_process_parameters = lpsspec.process().process_parameters();
      m_rewr = data::rewriter(lpsspec.data());

      for (const auto& summand: lpsspec.process().action_summands())
      {
        m_summands.emplace_back(summand, m_process_parameters);
      }

      std::size_t n = m_summands.size();
      std::size_t tau_summand_count = 0;
      for (std::size_t i = 0; i < n; i++)
      {
        const auto& summand = m_summands[i];
        if (!summand.is_tau())
        {
          continue;
        }
        tau_summand_count++;
        mCRL2log(log::info) << "summand " << (i + 1) << " of " << n << " (condition = " << confluence_type << "): ";
        bool confluent;
        std::size_t summand_index;
        switch (confluence_type)
        {
          case 'C': std::tie(confluent, summand_index) = is_square_confluent(summand); break;
          case 'T': std::tie(confluent, summand_index) = is_triangular_confluent(summand); break;
          case 'Z': std::tie(confluent, summand_index) = is_trivial_confluent(summand); break;
          default: throw mcrl2::runtime_error("Unknown confluence type " + std::to_string(confluence_type));
        }
        if (confluent)
        {
          result.push_back(i);
          mCRL2log(log::info) << "Confluent with all summands";
        }
        else
        {
          mCRL2log(log::info) << "Not confluent with summand " << (summand_index + 1);
        }
        mCRL2log(log::info) << std::endl;
      }
      mCRL2log(log::info) << result.size() << " of " << tau_summand_count << " tau summands were found to be confluent";
      return result;
    }

    /// \brief Applies confluent reduction to an LPS
    template <typename Specification>
    void run(Specification& lpsspec, char confluence_type, bool use_smt_solver = false)
    {
      if (use_smt_solver)
      {
        m_solver = std::unique_ptr<smt::smt_solver>(new smt::smt_solver(lpsspec.data()));
      }

      multi_action ctau{make_ctau_action()};
      std::vector<std::size_t> I = compute_confluent_summands(lpsspec, confluence_type);
      auto& summands = lpsspec.process().action_summands();
      for (std::size_t i: I)
      {
        summands[i].multi_action() = ctau;
      }
    }
};

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_CONFLUENCE_H
