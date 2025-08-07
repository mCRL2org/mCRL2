// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/stochastic_state.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_STOCHASTIC_STATE_H
#define MCRL2_LPS_STOCHASTIC_STATE_H

#include "mcrl2/data/print.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/real_utilities.h"
#include "mcrl2/lps/state.h"

namespace mcrl2::lps
{

/* inline
const data::data_expression& real_zero()
{
  static data::data_expression result = data::sort_real::creal(data::sort_int::cint(data::sort_nat::c0()), data::sort_pos::c1());
  return result;
}

inline
const data::data_expression& real_one()
{
  static data::data_expression result = data::sort_real::creal(data::sort_int::cint(data::sort_nat::cnat(data::sort_pos::c1())), data::sort_pos::c1());
  return result;
} */

// invariant: the elements of states must be unique
// invariant: the elements of probabilities must be >= 0
// invariant: the elements of probabilities must sum up to 1
// invariant: |probabilities| = |states|
struct stochastic_state
{
  // TODO: use a more efficient representation
  std::vector<data::data_expression> probabilities;
  std::vector<state> states;

  stochastic_state() = default;

  explicit stochastic_state(const state& s)
    : probabilities{data::sort_real::real_one()}, states{s}
  {}

  void push_back(const data::data_expression& probability, const state& s)
  {
    probabilities.push_back(probability);
    states.push_back(s);
  }

  void clear()
  {
    probabilities.clear();
    states.clear();
  }

  std::size_t size() const
  {
    assert(states.size()==probabilities.size());
    return states.size();
  }
};

inline
std::string print_probability(const data::data_expression& x)
{
  const auto& x_ = atermpp::down_cast<data::application>(x);
  return data::pp(x_[0]) + '/' + data::pp(x_[1]);
}

inline
void check_probability(const data::data_expression& x, const data::rewriter& rewr)
{
  const auto& x_ = atermpp::down_cast<data::application>(x);
  if (x_.head().size() != 3)
  {
    throw mcrl2::runtime_error("The probability " + data::pp(x) + " is not a proper rational number.");
  }
  if (x_.head() != data::sort_real::creal())
  {
    throw mcrl2::runtime_error("Probability is not a closed expression with a proper enumerator and denominator: " + data::pp(x) + ".");
  }
  if (rewr(data::greater_equal(x, data::sort_real::real_zero())) != data::sort_bool::true_())
  {
    throw mcrl2::runtime_error("Probability is smaller than zero: " + data::pp(x) + ".");
  }
  if (rewr(data::greater_equal(data::sort_real::real_one(), x)) != data::sort_bool::true_())
  {
    throw mcrl2::runtime_error("Probability is greater than one: " + data::pp(x) + ".");
  }
}

inline
void check_stochastic_state(const stochastic_state& s, const data::rewriter& rewr)
{
  data::data_expression probability = data::sort_real::real_zero();
  for (const data::data_expression& prob: s.probabilities)
  {
    check_probability(prob, rewr);
    probability = data::sort_real::plus(probability, prob);
  }
  probability = rewr(probability);
  if (probability != data::sort_real::real_one())
  {
    std::vector<std::string> v;
    for (const data::data_expression& prob: s.probabilities)
    {
      v.push_back(print_probability(prob));
    }
    throw mcrl2::runtime_error("The probabilities " + core::detail::print_list(v) + " do not add up to one, but to " + pp(probability) + "." );
  }
}

} // namespace mcrl2::lps

#endif // MCRL2_LPS_STOCHASTIC_STATE_H
