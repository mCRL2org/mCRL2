// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file simplifier.h


#ifndef MCRL2_PBESSYMBOLICBISIM_SIMPLIFIER_H
#define MCRL2_PBESSYMBOLICBISIM_SIMPLIFIER_H

#include "mcrl2/data/rewriter.h"

#include "simplifier_mode.h"

namespace mcrl2::data
{

class simplifier
{

public:
  virtual ~simplifier() = default;

  /**
   * \brief Creates a data_specification with rewrite rules that improve 'cannonicalness' of
   * expressions.
   * \detail A default data specification is expanded with a number of rewrite rules
   * for expressions over booleans and reals. Some rewrite rules help to simplify expressions.
   * Others try to rewrite linear inequalities to a normal form. There are also rewrite rules
   * to eliminate the [if] function symbol.
   */
  static data_specification norm_rules_spec();

protected:
  rewriter rewr;
  rewriter proving_rewr;
  std::map< data_expression, data_expression > cache;

  virtual data_expression simplify_expression(const data_expression& expr) = 0;

  data_expression apply_data_expression(const data_expression& expr, const mutable_indexed_substitution<>& sigma)
  {
    if(expr == sort_bool::true_() || expr == sort_bool::false_())
    {
      return expr;
    }
    // Rewrite the expression to some kind of normal form using BDDs
    data_expression rewritten = rewr(proving_rewr(expr,sigma));
    // Check the cache
    std::map< data_expression, data_expression >::const_iterator res = cache.find(rewritten);
    if(res != cache.end())
    {
      return res->second;
    }
    // Actually simplify the expression using the implementation in one of the subclasses
    data_expression simpl;
    try
    {
      simpl = simplify_expression(rewritten);
    }
    catch(const mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error("An exception occured while simplifying the expression " + pp(expr) +
        ".\nThe rewritten expression is " + pp(rewritten) + ".\nThe original exception is: " + e.what());
    }

    // Insert the result in the cache, both for the original expression and the expression in normal form
    cache.insert(std::make_pair(rewritten, simpl));

    return simpl;
  }

  lambda apply_lambda(const lambda& expr, const mutable_indexed_substitution<>& sigma)
  {
    return lambda(expr.variables(), apply_data_expression(expr.body(), sigma));
  }

public:
  simplifier(const rewriter& r, const rewriter& pr)
  : rewr(r)
  , proving_rewr(pr)
  {}

  data_expression apply(const data_expression& expr)
  {
    const mutable_indexed_substitution<> sigma;
    return apply(expr, sigma);
  }

  data_expression apply(const data_expression& expr, const mutable_indexed_substitution<>& sigma)
  {
    return is_lambda(expr) ? apply_lambda(atermpp::down_cast<lambda>(expr), sigma) : apply_data_expression(expr,sigma);
  }
};

simplifier* get_simplifier_instance(const simplifier_mode& mode, const rewriter& rewr, const rewriter& proving_rewr, const variable_list& process_parameters, const data_specification& dataspec,
  const std::map< variable, std::pair<data_expression, data_expression> >& lu_map = std::map< variable, std::pair<data_expression, data_expression> >());

} // namespace mcrl2
// namespace data

#endif // MCRL2_PBESSYMBOLICBISIM_SIMPLIFIER_H
