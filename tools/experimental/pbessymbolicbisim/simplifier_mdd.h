// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file simplifier_mdd.h


#ifndef MCRL2_PBESSYMBOLICBISIM_SIMPLIFIER_MDD_H
#define MCRL2_PBESSYMBOLICBISIM_SIMPLIFIER_MDD_H

#include "mcrl2/data/enumerator.h"

#include "simplifier.h"

namespace mcrl2::data
{

/**
 * \brief Simplifier that tries to minimize expressions over finite domains using
 * multi-value decision diagrams.
 */
class simplifier_mdd: public simplifier
{
  using super = simplifier;
  using super::rewr;
  using super::proving_rewr;

protected:
  data_specification dataspec;
  std::map<sort_expression, data_expression_vector> enumeration_cache;

  /**
   * \brief Enumerate all the possible values for 'sort' and return them in a vector
   */
  data_expression_vector enumerate_sort(const sort_expression& sort)
  {
    const std::map<sort_expression, data_expression_vector>::const_iterator res = enumeration_cache.find(sort);
    if(res != enumeration_cache.end())
    {
      return res->second;
    }
    const data_expression_vector& valid_exprs = enumerate_expressions(sort, dataspec, rewr);
    enumeration_cache.insert(std::make_pair(sort, valid_exprs));
    return valid_exprs;
  }

  data_expression make_mdd(const data_expression& expr)
  {
    const std::set<variable> free_vars = find_free_variables(expr);
    const auto smallest_var_it = std::find_if(free_vars.begin(), free_vars.end(), [this](const variable& v){ return dataspec.is_certainly_finite(v.sort()); });
    if(free_vars.empty() || smallest_var_it == free_vars.end())
    {
      return expr;
    }

    std::map< data_expression, std::list<data_expression> > remaining_map;
    //TODO improve this to also deal with natural numbers
    const variable& smallest_var = *smallest_var_it;
    // Gather possible values for smallest_var and the resulting expressions
    // if we substitute each of those values for smallest_var.
    // A reverse mapping (from resulting expression to value for smallest_var)
    // is maintained in 'remaining_map'.
    for(const data_expression& d: enumerate_sort(smallest_var.sort()))
    {
      data::mutable_indexed_substitution<> sigma;
      sigma[smallest_var] = d;
      data_expression remaining = make_mdd(rewr(expr, sigma));
      remaining_map[remaining].push_back(d);
    }

    // From 'remaining_map', we build the MDD.
    data_expression result = sort_bool::false_();
    for(const std::pair<data_expression, std::list<data_expression>> mdd_edge: remaining_map)
    {
      data_expression minimal_edge;
      if(mdd_edge.second.size() <= enumerate_sort(smallest_var.sort()).size()/2)
      {
        // Use positive formulation
        minimal_edge = sort_bool::false_();
        for(const data_expression& d: mdd_edge.second)
        {
          minimal_edge = lazy::or_(minimal_edge, equal_to(d, smallest_var));
        }
      }
      else
      {
        // Use negative formulation
        minimal_edge = sort_bool::true_();
        auto pos_it = mdd_edge.second.cbegin();
        for(const data_expression& d: enumerate_sort(smallest_var.sort()))
        {
          if(pos_it == mdd_edge.second.cend() || d != *pos_it)
          {
            minimal_edge = lazy::and_(minimal_edge, not_equal_to(d, smallest_var));
          }
          else if(pos_it != mdd_edge.second.cend())
          {
            ++pos_it;
          }
        }
      }

      result = lazy::or_(result, lazy::and_(minimal_edge, mdd_edge.first));
    }

    return result;
  }

  data_expression simplify_expression(const data_expression& expr) override { return rewr(make_mdd(expr)); }

public:
  simplifier_mdd(const rewriter& r, const rewriter& pr, const data_specification& dataspec_)
  : super(r, pr)
  , dataspec(dataspec_)
  {}
  ~simplifier_mdd() override = default;
};


} // namespace mcrl2
// namespace data

#endif // MCRL2_PBESSYMBOLICBISIM_SIMPLIFIER_MDD_H
