// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpssymbolicbisim.cpp


#ifndef MCRL2_LPSSYMBOLICBISIM_SIMPLIFY_LPS_H
#define MCRL2_LPSSYMBOLICBISIM_SIMPLIFY_LPS_H

#include "mcrl2/data/join.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/linear_inequalities.h"
#include "mcrl2/lps/stochastic_specification.h"
#include "mcrl2/lps/summand.h"

namespace mcrl2
{
namespace data
{

using namespace mcrl2::log;


struct simplify_real_inequalities: public std::unary_function<data_expression, data_expression>
{

  rewriter rewr;

  simplify_real_inequalities(rewriter rewr_)
  :  rewr(rewr_)
  {}

  const data_expression operator()(const data_expression& x) const
  {
    if ((is_equal_to_application(x) || 
      is_not_equal_to_application(x) || 
      is_less_application(x) || 
      is_less_equal_application(x) || 
      is_greater_application(x) || 
      is_greater_equal_application(x)))

    {
      if(atermpp::down_cast<application>(x)[0].sort() == sort_real::real_() && 
        atermpp::down_cast<application>(x)[1].sort() == sort_real::real_())
      {
        data_expression result(linear_inequality(x,rewr).transform_to_data_expression());
        return result;
      }
    }
    return x;
  }
};

struct
set_simplifier
{
  data::data_expression operator()(const data::data_expression& x) const
  {
    if (data::sort_bool::is_not_application(x))
    {
      return (*this)(unary_operand1(x));
    }
    else if (data::sort_bool::is_and_application(x))
    {
      std::set<data::data_expression> args = data::split_and(x);
      std::vector<data::data_expression> result;
      for (const data::data_expression& arg: args)
      {
        result.push_back((*this)(arg));
      }
      return data::join_and(result.begin(), result.end());
    }
    else if (data::sort_bool::is_or_application(x))
    {
      std::set<data::data_expression> args = data::split_or(x);
      std::vector<data::data_expression> result;
      for (const data::data_expression& arg: args)
      {
        result.push_back((*this)(arg));
      }
      return data::join_or(result.begin(), result.end());
    }
    return x;
  }
};

void simplify_one_by_one(mcrl2::lps::stochastic_specification& spec,
                                                const rewrite_strategy strat)
{
  rewriter r(spec.data(), strat);
  set_simplifier set_simplify;
  //iterate through conditions and simplify with a builder and fourier_motzkin
  for(lps::summand_base& s: spec.process().action_summands())
  {
    s.condition() = r(replace_data_expressions(s.condition(), simplify_real_inequalities(r), true));
    s.condition() = set_simplify(s.condition());
  }
  for(lps::summand_base& s: spec.process().deadlock_summands())
  {
    s.condition() = replace_data_expressions(s.condition(), simplify_real_inequalities(r), true);
    s.condition() = set_simplify(s.condition());
  }
}

} // namespace data
} // namespace mcrl2

#endif // MCRL2_LPSSYMBOLICBISIM_SIMPLIFY_LPS_H