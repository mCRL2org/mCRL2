// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file simplify_dbm.h


#ifndef MCRL2_LPSSYMBOLICBISIM_SIMPLIFIER_FOURIER_MOTZKIN_H
#define MCRL2_LPSSYMBOLICBISIM_SIMPLIFIER_FOURIER_MOTZKIN_H

#include "mcrl2/data/detail/linear_inequalities_utilities.h"

#include "simplifier.h"
#include "simplifier_finite_domain.h"

namespace mcrl2
{
namespace data
{

class simplifier_fourier_motzkin : public simplifier
{
  typedef simplifier super;
  using super::rewr;
  using super::proving_rewr;

protected:
  simplifier_finite_domain simpl_discr;

  data_expression reduce_lineq(const data_expression_list& lineq)
  {
    std::vector< linear_inequality > linear_inequalities;
    for(const data_expression& e: lineq)
    {
      linear_inequalities.emplace_back(e, rewr);
    }
    std::vector< linear_inequality > resulting_inequalities;
    remove_redundant_inequalities(linear_inequalities, resulting_inequalities, rewr);
    if(!(resulting_inequalities.size() == 1 && resulting_inequalities[0].is_false(rewr)))
    {
      data_expression result = sort_bool::true_();
      for(const linear_inequality& li: resulting_inequalities)
      {
        result = lazy::and_(result, li.transform_to_data_expression());
      }
      return result;
    }
    else
    {
      return sort_bool::false_();
    }
  }

  data_expression simplify_expression(const data_expression& expr)
  {
    // Split the expression into two equally sized lists of
    // expressions over real numbers and expressions over other
    // data sorts.
    std::vector < data_expression_list > real_conditions;
    std::vector < data_expression > non_real_conditions;
    detail::split_condition(expr, real_conditions, non_real_conditions);
    std::map< data_expression, std::vector< data_expression_list >> discr_to_real;
    // We collect the real conditions per distinct non real condition
    for(uint32_t i = 0; i < real_conditions.size(); i++)
    {
      std::pair< std::map< data_expression, std::vector< data_expression_list >>::iterator, bool > res =
          discr_to_real.insert(std::make_pair(non_real_conditions[i], std::vector< data_expression_list >()));
      res.first->second.push_back(real_conditions[i]);
    }
    data_expression result = sort_bool::false_();
    for(std::map< data_expression, std::vector< data_expression_list >>::iterator it = discr_to_real.begin(); it != discr_to_real.end(); it++)
    {
      // Reduce each of the linear systems individually
      data_expression real_condition = sort_bool::false_();
      for(data_expression_list zone: it->second)
      {
        real_condition = lazy::or_(real_condition, reduce_lineq(zone));
      }

      result = lazy::or_(result, lazy::and_(simpl_discr.apply(it->first), real_condition));
    }
    return rewr(result);
  }

public:
  simplifier_fourier_motzkin(const rewriter& r, const rewriter& pr, const data_specification& dataspec_)
  : super(r, pr)
  , simpl_discr(r, pr, dataspec_)
  {}

};


} // namespace mcrl2
} // namespace data

#endif // MCRL2_LPSSYMBOLICBISIM_SIMPLIFIER_FOURIER_MOTZKIN_H
