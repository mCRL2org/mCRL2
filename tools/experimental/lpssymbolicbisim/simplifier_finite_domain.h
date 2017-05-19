// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file simplify_dbm.h


#ifndef MCRL2_LPSSYMBOLICBISIM_SIMPLIFIER_FINITE_DOMAIN_H
#define MCRL2_LPSSYMBOLICBISIM_SIMPLIFIER_FINITE_DOMAIN_H

#include "simplifier.h"

namespace mcrl2
{
namespace data
{

class simplifier_finite_domain: public simplifier
{
  typedef simplifier super;
  using super::rewr;
  using super::proving_rewr;

protected:
  data_specification dataspec;
  rewriter nested_rewr;
  rewriter undo_nesting_rewr;

  /**
   * \brief Initializes a rewriter that removes all nested applications of '||'
   * \details This rewriter can be used to rewrite boolean expressions to
   * disjunctive normal form
   */
  void make_nested_rewr()
  {
    data_specification ad_hoc_data = dataspec;
    variable vb1("b1", sort_bool::bool_());
    variable vb2("b2", sort_bool::bool_());
    variable vb3("b3", sort_bool::bool_());

    //  a && (b || c) = a && b || a && c
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1,vb2,vb3), sort_bool::and_(vb1, sort_bool::or_(vb2, vb3)), 
      sort_bool::or_(sort_bool::and_(vb1,vb2), sort_bool::and_(vb1,vb3))));
    //  a || (b || c) = (a || b) || c;
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1,vb2,vb3), sort_bool::or_(vb1, sort_bool::or_(vb2,vb3)),
      sort_bool::or_(sort_bool::or_(vb1,vb2), vb3)));
    //  a && b || !a && b = b
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1,vb2), 
      sort_bool::or_(sort_bool::and_(vb1,vb2), sort_bool::and_(sort_bool::not_(vb1), vb2)),
      vb2));
    //  !a && b || a && b = b
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1,vb2), 
      sort_bool::or_(sort_bool::and_(sort_bool::not_(vb1),vb2), sort_bool::and_(vb1, vb2)),
      vb2));

    nested_rewr = rewriter(ad_hoc_data);
  }

  /**
   * \brief Initializes a rewriter that removes redundancy by nesting '&&'
   */
  void make_undo_nesting_rewr()
  {
    data_specification ad_hoc_data = dataspec;
    variable vb1("b1", sort_bool::bool_());
    variable vb2("b2", sort_bool::bool_());
    variable vb3("b3", sort_bool::bool_());

    //  a && b || a && c = a && (b || c)
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1,vb2,vb3), 
      sort_bool::or_(sort_bool::and_(vb1,vb2), sort_bool::and_(vb1,vb3)),
      sort_bool::and_(vb1, sort_bool::or_(vb2, vb3))));
    //  (a && b) && c = a && (b && c);
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1,vb2,vb3), sort_bool::and_(sort_bool::and_(vb1,vb2), vb3),
      sort_bool::and_(vb1, sort_bool::and_(vb2,vb3))));

    undo_nesting_rewr = rewriter(ad_hoc_data);
  }

  /**
   * \brief This function tries to simplify the expression
   * by translating to/from DNF/CNF
   * \details The result is an expression in (almost) DNF.
   * The only allowed exception is that terms in a conjuction may be
   * a disjuction over one free variable.
   * Example: (s == 1 || s == 2) && t == 0 || s == 3
   */
  data_expression simplify_nested(const data_expression& expr)
  {
    data_expression result = nested_rewr(expr);
    data_expression shorter_result = undo_nesting_rewr(result);
    for(const data_expression& d: find_data_expressions(shorter_result))
    {
      if(sort_bool::is_or_application(d) && find_free_variables(d).size() > 1)
      {
        // Found a disjunction over more that one free variable, so we have
        // to return the expression in real DNF.
        return result;
      }
    }
    // No nested disjuction contains more than one free variable, so we
    // can return the shortened version
    return shorter_result;
  }

  /**
   * \brief Try to simplify subexpressions with free variables
   * with a finite domain
   * \details This recursive function takes an expression in (almost) DNF,
   * as returned by simplify_nested, an tries to simplify each
   * conjunctive clause by enumerating possible values for free variables
   * with a finite domain.
   * Example: consider an the sort Enum4 = struct e1 | e2 | e3 | e4;
   * The expression (e != e1) && (e != e2) && (e != e3) can be simplified
   * to e == e4.
   */
  data_expression simplify_with_enumeration(const data_expression& expr)
  {
    data_expression result = expr;
    std::set< variable > free_vars = find_free_variables(result);
    if(sort_bool::is_or_application(result) && free_vars.size() > 1)
    {
      // Found a disjuction, so recurse
      return sort_bool::or_(simplify_with_enumeration(sort_bool::left(result)), simplify_with_enumeration(sort_bool::right(result)));
    }
    // Now we deal with the base case of the recursion: a conjunction
    for(variable v: free_vars)
    {
      if(dataspec.is_certainly_finite(v.sort()))
      {
        data_expression_vector valid_exprs = enumerate_expressions(v.sort(), dataspec, rewr);
        // Accumulate the values for which v occurs positively and negatively
        data_expression positive_values_of_v = sort_bool::false_();
        data_expression negative_values_of_v = sort_bool::true_();
        data_expression next_result = result;
        int positive_size = 0;
        int negative_size = 0;
        for(data_expression d: valid_exprs)
        {
          data::mutable_indexed_substitution<> sigma;
          sigma[v] = d;
          data_expression expr_without_v = rewr(result, sigma);
          if(expr_without_v != sort_bool::false_())
          {
            next_result = expr_without_v;
            positive_values_of_v = lazy::or_(positive_values_of_v, equal_to(v, d));
            positive_size++;
          }
          else
          {
            negative_values_of_v = lazy::and_(negative_values_of_v, not_equal_to(v, d));
            negative_size++;
          }
        }
        // Update the expression with the smallest amount of occurences of v
        result = lazy::and_(next_result, negative_size < positive_size ? negative_values_of_v : positive_values_of_v);
      }
    }
    return result;
  }

  data_expression simplify_expression(const data_expression& expr)
  {
    return rewr(proving_rewr(simplify_with_enumeration(simplify_nested(expr))));
  }

public:
  simplifier_finite_domain(rewriter r, rewriter pr, data_specification dataspec_)
  : super(r, pr)
  , dataspec(dataspec_)
  {
    make_nested_rewr();
    make_undo_nesting_rewr();
  }

};


} // namespace mcrl2
} // namespace data

#endif // MCRL2_LPSSYMBOLICBISIM_SIMPLIFIER_FINITE_DOMAIN_H