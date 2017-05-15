// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file enumerate_block_union.h

#ifndef MCRL2_LPSSYMBOLICBISIM_ENUMERATE_BLOCK_UNION_H
#define MCRL2_LPSSYMBOLICBISIM_ENUMERATE_BLOCK_UNION_H

namespace mcrl2
{
namespace data
{

struct enumerate_filter_print
{
  data_expression last_linear_inequality;
  rewriter rewr;

  explicit enumerate_filter_print(const data_expression& lli, const rewriter& rewr_)
    : last_linear_inequality(lli)
    , rewr(rewr_)
  {}

  bool is_consistent_conjunct(data_expression conjunct)
  {
    std::vector< linear_inequality > lis;
    while(sort_bool::is_and_application(conjunct))
    {
      data_expression li_expr = sort_bool::right(conjunct);
      bool invert = false;
      if(sort_bool::is_not_application(li_expr))
      {
        li_expr = sort_bool::arg(li_expr);
        invert = true;
      }
      linear_inequality li = invert ? linear_inequality(li_expr, rewr).invert(rewr) : linear_inequality(li_expr, rewr);
      lis.push_back(li);
      conjunct = sort_bool::left(conjunct);
    }
    data_expression li_expr = conjunct;
    bool invert = false;
    if(sort_bool::is_not_application(li_expr))
    {
      li_expr = sort_bool::arg(li_expr);
      invert = true;
    }
    linear_inequality li = invert ? linear_inequality(li_expr, rewr).invert(rewr) : linear_inequality(li_expr, rewr);
    lis.push_back(li);
    return !is_inconsistent(lis, rewr, true);
  }

  bool operator()(const data_expression& d)
  {
    data_expression conjunct = rewr(d);
    if(sort_bool::is_false_function_symbol(conjunct))
    {
      return false;
    }
    else if(sort_bool::is_and_application(conjunct) &&
      (sort_bool::right(conjunct) == last_linear_inequality || sort_bool::right(conjunct) == sort_bool::not_(last_linear_inequality)))
    {
      // We assume now all variables have been filled in so we can
      // just walk over a conjuction to gather all the linear
      // inequalities
      return is_consistent_conjunct(conjunct);
    }
    else if(sort_bool::is_and_application(conjunct) && sort_bool::is_and_application(sort_bool::right(conjunct)) &&
      (   sort_bool::right(sort_bool::right(conjunct)) == last_linear_inequality 
       || sort_bool::right(sort_bool::right(conjunct)) == sort_bool::not_(last_linear_inequality)))
    {
      return is_consistent_conjunct(sort_bool::right(conjunct));
    }
    return true;
  }
};


} // namespace data
} // namespace mcrl2


#endif // MCRL2_LPSSYMBOLICBISIM_ENUMERATE_BLOCK_UNION_H