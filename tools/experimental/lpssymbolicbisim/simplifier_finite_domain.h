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
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1,vb2), 
      sort_bool::or_(sort_bool::and_(sort_bool::not_(vb1),vb2), sort_bool::and_(vb1, vb2)),
      vb2));

    nested_rewr = rewriter(ad_hoc_data);
  }

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

  data_expression simplify_nested(const data_expression& expr)
  {
    data_expression result = nested_rewr(expr);
    data_expression shorter_result = undo_nesting_rewr(result);
    for(const data_expression& d: find_data_expressions(shorter_result))
    {
      if(sort_bool::is_or_application(d) && find_free_variables(d).size() > 1)
      {
        // std::cout << "Simplify nested rewrote \n\t\t" << expr << "\n\tto\n\t\t" << result << std::endl;
        return result;
      }
    }
    // std::cout << "Simplify nested rewrote \n\t\t" << expr << "\n\tto (short version)\n\t\t" << shorter_result << std::endl;
    return shorter_result;
  }

  data_expression simplify_with_enumeration(const data_expression& expr)
  {
    data_expression result = expr;
    std::set< variable > free_vars = find_free_variables(result);
    if(sort_bool::is_or_application(result) && free_vars.size() > 1)
    {
      return sort_bool::or_(simplify_with_enumeration(sort_bool::left(result)), simplify_with_enumeration(sort_bool::right(result)));
    }
    // std::cout << "variables\t ";
    for(variable v: free_vars)
    {
      // std::cout << "   " << v.name() << ": " << v.sort() << " ";
      if(dataspec.is_certainly_finite(v.sort()))
      {
        // std::cout << "Simplifying discrete condition " << result << " for variable " << v << std::endl;
        data_expression_vector valid_exprs = enumerate_expressions(v.sort(), dataspec, rewr);
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
        result = lazy::and_(next_result, negative_size < positive_size ? negative_values_of_v : positive_values_of_v);
        // std::cout << "\t result " << result << std::endl;
      }
    }
    // std::cout << std::endl;
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