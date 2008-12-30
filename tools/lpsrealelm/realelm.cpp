// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file realelm.cpp
/// \brief 

#include <algorithm>
#include <stdlib.h>

#include "mcrl2/atermpp/map.h"
#include "mcrl2/atermpp/set_operations.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/postfix_identifier_generator.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/lps/linear_process.h"

#include "comp.h"
#include "realelm.h"

using namespace atermpp;
using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::data::data_expr;
using namespace mcrl2::lps;

static inline
data_specification add_ad_hoc_real_equations(const data_specification& specification)
{
  ATermAppl nil = gsMakeNil();
  ATermAppl zero = gsMakeDataExprC0();
  ATermAppl one = gsMakeDataExprC1();
  ATermAppl two = gsMakeDataExprCDub(gsMakeDataExprFalse(), one);
  ATermAppl real_zero = gsMakeDataExprCReal(gsMakeDataExprCInt(zero), one);
  ATermAppl real_one = gsMakeDataExprCReal(gsMakeDataExprCInt(gsMakeDataExprCNat(one)), one);
  ATermAppl real_two = gsMakeDataExprCReal(gsMakeDataExprCInt(gsMakeDataExprCNat(two)), one);
  ATermAppl p = gsMakeDataVarId(gsString2ATermAppl("p"), gsMakeSortExprPos());
  ATermAppl r = gsMakeDataVarId(gsString2ATermAppl("r"), gsMakeSortExprReal());
  ATermAppl s = gsMakeDataVarId(gsString2ATermAppl("s"), gsMakeSortExprReal());
  ATermAppl t = gsMakeDataVarId(gsString2ATermAppl("t"), gsMakeSortExprReal());
  ATermList rl = ATmakeList1((ATerm) r);
  ATermList prl = ATmakeList2((ATerm) p, (ATerm) r);
  ATermList rsl = ATmakeList2((ATerm) r, (ATerm) s);
  ATermList rstl = ATmakeList3((ATerm) r, (ATerm) s, (ATerm) t);

  ATermList result = ATmakeList(16,
    // r+0/p=r
    (ATerm) gsMakeDataEqn(prl, nil, gsMakeDataExprAdd(r, gsMakeDataExprCReal(gsMakeDataExprCInt(zero), p)), r),
    // 0/p+r=r
    (ATerm) gsMakeDataEqn(prl, nil, gsMakeDataExprAdd(gsMakeDataExprCReal(gsMakeDataExprCInt(zero), p), r), r),
    // r+-r=0
    (ATerm) gsMakeDataEqn(rl, nil, gsMakeDataExprAdd(r, gsMakeDataExprNeg(r)), real_zero),
    // -r+r=0
    (ATerm) gsMakeDataEqn(rl, nil, gsMakeDataExprAdd(gsMakeDataExprNeg(r), r), real_zero),
    // r+r=2*r
    (ATerm) gsMakeDataEqn(rl, nil, gsMakeDataExprAdd(r, r), gsMakeDataExprMult(real_two, r)),
    // -r+-r=-2*r
    (ATerm) gsMakeDataEqn(rl, nil, gsMakeDataExprAdd(gsMakeDataExprNeg(r), gsMakeDataExprNeg(r)), gsMakeDataExprMult(gsMakeDataExprNeg(real_two), r)),
    // s.r+t.r=(s+t).r
    (ATerm) gsMakeDataEqn(rstl, nil, gsMakeDataExprAdd(gsMakeDataExprMult(s, r), gsMakeDataExprMult(t, r)), gsMakeDataExprMult(gsMakeDataExprAdd(s, t), r)),
    // (r+s)+-s=r
    (ATerm) gsMakeDataEqn(rsl, nil, gsMakeDataExprAdd(gsMakeDataExprAdd(r,s), gsMakeDataExprNeg(s)), r),
    // (r+-s)+s=r
    (ATerm) gsMakeDataEqn(rsl, nil, gsMakeDataExprAdd(gsMakeDataExprAdd(r,gsMakeDataExprNeg(s)), s), r),
    // r-0/p=r
    (ATerm) gsMakeDataEqn(prl, nil, gsMakeDataExprSubt(r, gsMakeDataExprCReal(gsMakeDataExprCInt(zero), p)), r),
    // 0/p-r=-r
    (ATerm) gsMakeDataEqn(prl, nil, gsMakeDataExprSubt(gsMakeDataExprCReal(gsMakeDataExprCInt(zero), p), r), gsMakeDataExprNeg(r)),
    // r.(1/1)=r
    (ATerm) gsMakeDataEqn(prl, nil, gsMakeDataExprMult(r, real_one), r),
    // (1/1).r=r
    (ATerm) gsMakeDataEqn(prl, nil, gsMakeDataExprMult(real_one, r), r),
    // --r=r
    (ATerm) gsMakeDataEqn(prl, nil, gsMakeDataExprNeg(gsMakeDataExprNeg(r)), r),
    // -(r+s)=-r+-s
    (ATerm) gsMakeDataEqn(rsl, nil, gsMakeDataExprNeg(gsMakeDataExprAdd(r,s)), gsMakeDataExprAdd(gsMakeDataExprNeg(r), gsMakeDataExprNeg(s))),
    // -(r-s)=-r+s
    (ATerm) gsMakeDataEqn(rsl, nil, gsMakeDataExprNeg(gsMakeDataExprSubt(r,s)), gsMakeDataExprAdd(gsMakeDataExprNeg(r), s))
  );

  return set_equations(specification, specification.equations() + data_equation_list(result));
}

// Custom replace functions
// Needed as the replace functions of the data library do not
// recurse into data expressions
template <typename ReplaceFunction>
struct realelm_replace_data_expressions_helper
{
  const ReplaceFunction& r_;

  realelm_replace_data_expressions_helper(const ReplaceFunction& r)
    : r_(r)
  {}

  std::pair<atermpp::aterm_appl, bool> operator()(atermpp::aterm_appl t) const
  {
    if (is_sort_expression(t))
    {
      return std::pair<atermpp::aterm_appl, bool>(t, false); // do not continue the recursion
    }
    else if (is_data_expression(t))
    {
      data_expression new_t = r_(t);
      if(t == new_t)
      {
        return std::pair<atermpp::aterm_appl, bool>(t, true); // continue the recursion
      }
      else
      {
        return std::pair<atermpp::aterm_appl, bool>(new_t, false); // do not continue the recursion
      }
    }
    else
    {
      return std::pair<atermpp::aterm_appl, bool>(t, true); // continue the recursion
    }
  }
};

/// \cond INTERNAL_DOCS
template <typename MapContainer>
struct realelm_map_replace_helper
{
  const MapContainer& replacements_;

  /// \brief Constructor.
  ///
  realelm_map_replace_helper(const MapContainer& replacements)
    : replacements_(replacements)
  {}

  /// \brief Returns s if a substitution of the form t := s is present in the replacement map,
  /// otherwise t.
  ///
  data_expression operator()(const data_expression& t) const
  {
    typename MapContainer::const_iterator i = replacements_.find(t);
    if (i == replacements_.end())
    {
      return atermpp::aterm_appl(t);
    }
    else
    {
      return i->second;
    }
  }
};

template <typename Term, typename ReplaceFunction>
Term realelm_replace_data_expressions(Term t, ReplaceFunction r)
{
  return atermpp::partial_replace(t, realelm_replace_data_expressions_helper<ReplaceFunction>(r));
}

template <typename Term, typename MapContainer>
Term realelm_data_expression_map_replace(Term t, const MapContainer& replacements)
{
  return realelm_replace_data_expressions(t, realelm_map_replace_helper<MapContainer>(replacements));
}
// End of replace substitute

/// \brief Integer exponentiation
/// \param base Base of the exponentiation
/// \param exponent Exponent of the exponentiation
/// \ret base^exponent
static
unsigned long pow(unsigned long base, unsigned long exponent)
{
  if(exponent == 0)
  {
    return 1;
  }
  else
  {
    div_t q = div(exponent, 2);
    if(q.rem == 0)
    {
      return pow(base * base, q.quot);
    }
    else
    {
      assert(q.rem == 1);
      return base * pow(base, exponent - 1);
    }
  }
}

/// \brief Retrieve the left hand side of a data expression
/// \param e A data expression
/// \pre e is a data application d(x,y) with two arguments
/// \ret x
static inline
data_expression lhs(const data_expression e)
{
  assert(is_data_application(e));
  data_expression_list arguments = static_cast<const data_application&>(e).arguments();
  assert(arguments.size() == 2);
  return *(arguments.begin());
}

/// \brief Retrieve the right hand side of a data expression
/// \param e A data expression
/// \pre e is a data application d(x,y) with two arguments
/// \ret y
static inline
data_expression rhs(const data_expression e)
{
  assert(is_data_application(e));
  data_expression_list arguments = static_cast<const data_application&>(e).arguments();
  assert(arguments.size() == 2);
  return *(++arguments.begin());
}

/// \brief returns the number zero of sort Real
/// \ret the data_expression representing zero
static inline data_expression real_zero()
{
  return gsMakeDataExprReal_int(0);
}

/// \brief Determine whether a data expression is a constant expression
///        encoding a number
/// \param e A data expression
/// \ret true iff e is a numeric constant
static inline
bool is_number(const data_expression e)
{
  return core::detail::gsIsDataExprC0(e) ||
         core::detail::gsIsDataExprCDub(e) ||
         core::detail::gsIsDataExprC1(e) ||
         core::detail::gsIsDataExprCNat(e) ||
         core::detail::gsIsDataExprCNeg(e) ||
         core::detail::gsIsDataExprCInt(e) ||
         core::detail::gsIsDataExprCReal(e);
}

/// \brief Determine wheter a number is negative
/// \param e A data expression
/// \ret true iff e is -e' or @rational(-e',e'')
static inline
bool is_negative(const data_expression e)
{
  return is_negate(e) || gsIsDataExprCNeg(e) || (gsIsDataExprCReal(e) && is_negative(lhs(e)));
}

/// \brief Determine whether a data expression is an inequality
/// \param e A data expression
/// \ret true iff e is a data application of ==, <, <=, > or >= to
///      two arguments.
static inline
bool is_inequality(const data_expression& e)
{
  return is_equal_to(e) || is_less(e) || is_less_equal(e) || is_greater(e) ||
         is_greater_equal(e);
}


/// \brief Determine whether the variables in two data expressions are lexicographically ordered.
/// \param e1 A data expression containing a single variable
/// \param e2 A data expression also containing at most a single
/// \pre e1 and e2 are either a negation, multiplication with a number as left hand
///      side, a data variable or a constant
/// \ret true iff e1 <= e2, where <= is lexicographic order on the variable name. An expression
/// without a variable is considered the smallest.
static
bool less_or_equal(const data_expression& e1, const data_expression& e2)
{
  gsDebugMsg("less or equal %P, %P\n", (ATermAppl)e1, (ATermAppl)e2);
  if(is_negate(e1))
  {
    return less_or_equal(*(static_cast<const data_application&>(e1).arguments().begin()), e2);
  }
  else if(is_negate(e2))
  {
    return less_or_equal(e1, *(static_cast<const data_application&>(e2).arguments().begin()));
  }
  else if (is_multiplies(e1))
  {
    return less_or_equal(rhs(e1), e2);
  }
  else if (is_multiplies(e2))
  {
    return less_or_equal(e1, rhs(e2));
  }
  else if (gsIsDataExprCReal(e1))
  {
    return less_or_equal(lhs(e1), e2);
  }
  else if (gsIsDataExprCReal(e2))
  {
    return less_or_equal(e1, lhs(e2));
  }
  else if (gsIsDataExprCInt(e1))
  {
    return true;
  }
  else if (gsIsDataExprCInt(e2) )
  {
    return false;
  }

  gsDebugMsg("e1 = %s, e2 = %s\n", (ATermAppl)e1, (ATermAppl)e2);
  assert(is_data_variable(e1) && is_data_variable(e2));
  return (static_cast<const data_variable&>(e1).name() <= static_cast<const data_variable&>(e2).name());
}

/// \brief Merge two sorted lists of data expressions
/// \param l1 A list of data expressions
/// \param l2 A list of data expressions
/// \pre l1 and l2 are sorted
/// \ret The sorted list of data expressions consisting of all elements in l1
///      and l2
static
data_expression_list merge(const data_expression_list& l1, const data_expression_list& l2)
{
  data_expression_list r;
  data_expression_list::iterator i = l1.begin();
  data_expression_list::iterator j = l2.begin();
  while(i != l1.end() && j != l2.end())
  {
    if(less_or_equal(*i, *j))
    {
      r = push_front(r, *i++);
    }
    else
    {
      r = push_front(r, *j++);
    }
  }
  while(i != l1.end())
  {
    r = push_front(r, *i++);
  }
  while(j != l2.end())
  {
    r = push_front(r, *j++);
  }
  r = reverse(r);
  return r;
}

/// \brief Sort a data expression
/// \param e A data expression of the form c1 * x1 + ... + cn * xn
/// \ret The list of data expressions ci * xi, such that it is sorted
///      in lexicographic order of the names of xi
static inline
data_expression_list sort(const data_expression& e)
{
  data_expression_list r;
  if (is_plus(e))
  {
    r = merge(sort(lhs(e)), sort(rhs(e)));
  }
  else
  {
    r = push_front(r, e);
  }
  return r;
}

/// \brief Order the variables in an inequality, using a lexicographic order.
/// \param inequality An inequality
/// \param r A rewriter
/// \ret A version of the inequality in which the variables occur in
///      lexicographically sorted order.
static inline
data_expression order_inequality(const data_expression& inequality, rewriter& r)
{
  assert(is_inequality(inequality));
  data_expression left = lhs(inequality);
  if (!is_number(left) && !is_data_variable(left))
  {
    data_expression_list sorted = sort(left);
    left = real_zero();
    for(data_expression_list::iterator j = sorted.begin(); j != sorted.end(); ++j)
    {
      left = plus(left, *j);
    }
  }
  return data_application(static_cast<const data_application&>(inequality).head(), make_list(r(left), r(rhs(inequality))));
}

/// \brief Split constant and variable parts of a data expression
/// \param e A data expression of the form c1 * x1 + ... + cn * xn + d1 + ... +
///          dm, where ci and di are constants and xi are variables. Constants
///          and variables may occur mixed.
/// \ret The pair (c1 * x1 + ... + cn * xn, d1 + ... + dm)
static
std::pair<data_expression, data_expression> split_variables_and_constants(const data_expression& e)
{
  gsDebugMsg("Splitting constants and variables in %P\n", (ATermAppl)e);
  std::pair<data_expression, data_expression> result;
  if(is_plus(e))
  {
    std::pair<data_expression, data_expression> left = split_variables_and_constants(lhs(e));
    std::pair<data_expression, data_expression> right = split_variables_and_constants(rhs(e));
    result = std::make_pair(plus(left.first, right.first), plus(left.second, right.second));
  }
  else if (is_minus(e))
  {
    std::pair<data_expression, data_expression> left = split_variables_and_constants(lhs(e));
    std::pair<data_expression, data_expression> right = split_variables_and_constants(rhs(e));
    result = std::make_pair(plus(left.first, negate(right.first)), plus(left.second, negate(right.second)));
  }
  else if (is_negate(e))
  {
    data_expression argument = *static_cast<const data_application&>(e).arguments().begin();
    if(is_plus(argument))
    {
      result = split_variables_and_constants(plus(negate(lhs(argument)), negate(rhs(argument))));
    }
    else if(is_minus(argument))
    {
      result = split_variables_and_constants(plus(negate(lhs(argument)), rhs(argument)));
    }
    else if(is_number(argument))
    {
      result = std::make_pair(real_zero(), e);
    }
    else
    {
      result = std::make_pair(e, real_zero());
    }
  }
  else if (gsIsDataExprCReal(e) && !is_number(*static_cast<const data_application&>(e).arguments().begin()))
  {
    result = std::make_pair(e, real_zero());
  }
  else if (is_multiplies(e) || is_data_variable(e))
  {
    result = std::make_pair(e, real_zero());
  }
  else
  {
    gsDebugMsg("e: %P (%T)\n", (ATermAppl)e, (ATermAppl)e);
    assert(is_number(e));
    result = std::make_pair(real_zero(), e);
  }
  gsDebugMsg("split version: left = %P, right = %P\n", (ATermAppl)result.first, (ATermAppl)result.second);
  return result;
}

/// \brief Returns a list of all real variables in l
/// \param l a list of data variables
/// \ret The list of all v in l such that v.sort() == real()
static inline
data_variable_list get_real_variables(const data_variable_list& l)
{
  data_variable_list r;
  for(data_variable_list::const_iterator i = l.begin(); i != l.end(); ++i)
  {
    if(i->sort() == sort_expr::real())
    {
      r = push_front(r, *i);
    }
  }
  return r;
}

/// \brief Returns a list of all nonreal variables in l
/// \param l a list of data variables
/// \ret The list of all v in l such that v.sort() != real()
static inline
data_variable_list get_nonreal_variables(const data_variable_list& l)
{
  data_variable_list r;
  for(data_variable_list::const_iterator i = l.begin(); i != l.end(); ++i)
  {
    if(i->sort() != sort_expr::real())
    {
      r = push_front(r, *i);
    }
  }
  return r;
}

/// \brief Returns a list of all real expressions in l
/// \param l a list of data expressions
/// \ret The list of all e in l such that e.sort() == real()
static inline
data_expression_list get_real_expressions(const data_expression_list& l)
{
  data_expression_list r;
  for(data_expression_list::const_iterator i = l.begin(); i != l.end(); ++i)
  {
    if(i->sort() == sort_expr::real())
    {
      r = push_front(r, *i);
    }
  }
  return r;
}

/// \brief Returns a list of all nonreal expressions in l
/// \param l a list of data expressions
/// \ret The list of all e in l such that e.sort() != real()
static inline
data_expression_list get_nonreal_expressions(const data_expression_list& l)
{
  data_expression_list r;
  for(data_expression_list::const_iterator i = l.begin(); i != l.end(); ++i)
  {
    if(i->sort() != sort_expr::real())
    {
      r = push_front(r, *i);
    }
  }
  return r;
}

/// \brief Returns a list of all real assignments in l
/// \param l a list of data assignments
/// \ret The list of all x := e in l such that x.sort() == e.sort() == real()
static inline
data_assignment_list get_real_assignments(const data_assignment_list& l)
{
  data_assignment_list r;
  for(data_assignment_list::const_iterator i = l.begin(); i != l.end(); ++i)
  {
    if(i->lhs().sort() == sort_expr::real())
    {
      r = push_front(r, *i);
    }
  }
  return r;
}

/// \brief Returns a list of all nonreal assignments in l
/// \param l a list of data assignments
/// \ret The list of all x := e in l such that x.sort() == e.sort() != real()
static inline
data_assignment_list get_nonreal_assignments(const data_assignment_list& l)
{
  data_assignment_list r;
  for(data_assignment_list::const_iterator i = l.begin(); i != l.end(); ++i)
  {
    if(i->lhs().sort() != sort_expr::real())
    {
      r = push_front(r, *i);
    }
  }
  return r;
}

/// \brief Normalize an equality.
/// \details Transform the inequality in e such that the left hand side only
///        contains multiples of variables and the right hand side contains a
///        single constant. 
/// \param e A data expression
/// \param r A rewriter
/// \pre e is an inequality
/// \ret An inequality e' equivalent to e, such that the left hand side is of
///      the form c1 * e1 + ... + cn * en === d, with === one of ==, <, <=, >,
///      >=. In case there are no variables left, the left hand side is the 
///      expression 0.
static inline
data_expression normalize_inequality(const data_expression& e, rewriter& r)
{

  assert(is_inequality(e));
  gsDebugMsg("Normalizing inequality %P\n", (ATermAppl)e);

  data_application d = static_cast<const data_application&>(e);
  std::pair<data_expression, data_expression> left = split_variables_and_constants(r(lhs(d)));
  std::pair<data_expression, data_expression> right = split_variables_and_constants(r(rhs(d)));
  // Variables are moved to the left, constants to the right of each pair.

  gsDebugMsg("left.first = %P, left.second = %P, right.first = %P, right.second = %P\n", (ATermAppl)left.first, (ATermAppl)left.second, (ATermAppl)right.first, (ATermAppl)right.second);

  data_expression new_left = left.first;
  while(is_plus(right.first))
  {
    new_left = plus(new_left, negate(rhs(right.first)));
    right.first = lhs(right.first);
  }
  new_left = r(plus(new_left, negate(right.first)));

  data_expression new_right = right.second;
  while(is_plus(left.second))
  {
    new_right = plus(new_right, negate(rhs(left.second)));
    left.second = lhs(left.second);
  }
  new_right = r(plus(new_right, negate(left.second)));

  data_expression result = data_application(d.head(), make_list(new_left, new_right));

  gsDebugMsg("Normalization result %P\n", (ATermAppl)result);

  result = order_inequality(result, r);
  gsDebugMsg("Ordered version of result %P\n", (ATermAppl)result);

  return result;
}

/// \brief Normalize the inequalities in l. 
/// \details See normalize_inequality for more details. The result
///          can be an empty list, if the l was equivalent to true.
///          In case the input was inconsistent, the result can be 
///          [false]
/// \param l A list of data expressions
/// \param r A rewriter
/// \ret The list of data expressions l' equivalent to l, but with all
///      inequalities normalized.
static
data_expression_list normalize_inequalities(const data_expression_list& l, rewriter& r)
{
  data_expression_list result;
  for(data_expression_list::iterator i = l.begin(); i != l.end() ; ++i)
  {
    data_expression inequality = r(*i);
    if (is_inequality(inequality))
    {
      inequality = r(normalize_inequality(inequality, r));
      if (inequality==true_())
      { /* do nothing */
      }
      else if (inequality==false_())
      {
        return push_front(data_expression_list(),false_());
      }
      else if (!find_data_expression(result, inequality))
      {
        result = push_front(result,normalize_inequality(inequality, r));
      }
    }
    else
    {
      result = push_front(result, inequality);
    }
  }
  return result;
}

/// \brief Splits a conjunct in a conjunct ranging over inequalities of reals
///        and a conjunct ranging over other expressions.
/// \param e A data expression
/// \pre e is a conjunct
/// \ret Pair (r,x) such that r is a list of expressions over real numbers and
///      x is a list of expressions not over real numbers.
static
std::pair<data_expression_list, data_expression_list> split_conjunct(data_expression e)
{
  data_expression_list real_conjuncts;
  data_expression_list nonreal_conjuncts;

  while(is_and(e))
  {
    std::pair<data_expression_list, data_expression_list> c = split_conjunct(lhs(e));
    real_conjuncts = real_conjuncts + c.first;
    nonreal_conjuncts = nonreal_conjuncts + c.second;
    e = rhs(e);
  }

  if(is_inequality(e) && (lhs(e).sort() == sort_expr::real() || rhs(e).sort() == sort_expr::real()))
  {
    real_conjuncts = push_front(real_conjuncts, e);
  }
  else
  {
    nonreal_conjuncts = push_front(nonreal_conjuncts, e);
  }
  return std::make_pair(real_conjuncts, nonreal_conjuncts);
}

/// \brief Removes constant real numbers from the nextstate vectors of all
///        summands by introducing auxiliary sum variables
/// \param s A data specification
/// \param variable_generator A variable generator
/// \ret A specification equivalent to s in which no real constants occur in
///      nextstate vectors.
static
specification remove_real_constants_from_nextstate(specification s, identifier_generator& variable_generator)
{
  linear_process lps = s.process();
  summand_list sl;
  for(summand_list::const_iterator i = lps.summands().begin(); i != lps.summands().end(); ++i)
  {
    data_assignment_list nextstate;
    data_expression condition = i->condition();
    data_variable_list sum_variables = i->summation_variables();
    for(data_assignment_list::const_iterator j = i->assignments().begin(); j != i->assignments().end(); ++j)
    {
      if(j->rhs().sort() == sort_expr::real() && is_number(j->rhs()))
      {
        data_expression right = j->rhs();
        data_variable var(variable_generator("y"), sort_expr::real());
        condition = and_(condition, equal_to(var, right));
        nextstate = push_front(nextstate, data_assignment(j->lhs(), var));
        sum_variables = push_front(sum_variables, var);
      }
      else
      {
        nextstate = push_front(nextstate, *j);
      }
    }
    nextstate = reverse(nextstate);
    summand s(sum_variables, condition, i->is_delta(), i->actions(), nextstate);
    sl = push_front(sl, s);
  }
  sl = reverse(sl);
  lps = set_summands(lps, sl);
  s = set_lps(s, lps);
  return s;

}

/// \brief Normalize all inequalities in the summands of the specification
/// \details All real constants occurring in the nextstate vectors of all
///          summands are replaced by a summation variable (see
///          remove_real_constants_from_nextstate). Furthermore, the parts of the
///          conditions ranging over real numbers and the other parts of the conditions
///          are separated and the part ranging over real numbers is normalized
///          (see normalize_inequalities for details).
/// \param s A data specification
/// \param r A rewriter
/// \param variable_generator A variable generator
/// \ret A specification equivalent to s, but of which all inequalities have
///      been normalized.
static
specification normalize_specification(specification s, rewriter& r, identifier_generator& variable_generator)
{
  s = remove_real_constants_from_nextstate(s, variable_generator);
  linear_process lps = s.process();
  summand_list sl;
  for(summand_list::const_iterator i = lps.summands().begin(); i != lps.summands().end(); ++i)
  {
    std::pair<data_expression_list, data_expression_list> conjuncts = split_conjunct(i->condition());

    data_expression non_real = join_and(conjuncts.second.begin(), conjuncts.second.end());
    data_expression_list real_inequalities = normalize_inequalities(conjuncts.first, r);
    data_expression real = join_and(real_inequalities.begin(), real_inequalities.end());
    data_expression condition = and_(non_real, real);

    summand t = set_condition(*i, condition);
    sl = push_front(sl, set_condition(*i, condition));
  }
  sl = reverse(sl);
  lps = set_summands(lps, sl);

  s = set_lps(s, lps);
  return s;
}

/// \brief Determine the inequalities ranging over real numbers in a data expression.
/// \param e A data expression
/// \param inequalities A list of inequalities
/// \post inequalities contains all inequalities ranging over real numbers in e.
static
void determine_real_inequalities(const data_expression& e, data_expression_list& inequalities)
{
  if (is_and(e))
  {
    determine_real_inequalities(lhs(e), inequalities);
    determine_real_inequalities(rhs(e), inequalities);
  }
  else if ((is_equal_to(e) || is_less(e) || is_less_equal(e)) && rhs(e).sort() == sort_expr::real())
  {
    inequalities = push_front(inequalities, e);
  }
}

/// \brief Try to eliminate variables from a system of inequalities using Gauss
///        elimination.
/// \param inequalities A list of inequalities over real numbers
/// \param variables A list of variables to be eliminated
/// \param r A rewriter.
/// \post variables contains the list of variables that have not been eliminated
/// \ret The system of inequalities after Gauss eliminatation.
static
data_expression_list gauss_elimination(data_expression_list inequalities, data_variable_list& variables, rewriter& r)
{
  gsDebugMsg("Trying to eliminate variables %P from system %P using gauss elimination\n", (ATermList)variables, (ATermList)inequalities);

  // First find out whether there are variables that occur in an equality, so
  // that we can perform gauss elimination.
  data_variable_list eliminated_variables;
  for(data_variable_list::const_iterator i = variables.begin(); i != variables.end(); ++i)
  {
    for(data_expression_list::const_iterator j = inequalities.begin(); j != inequalities.end(); ++j)
    {
      if(is_equal_to(*j) && find_data_variable(*j, *i))
      {
        // There is an equality in which the variable occurs in the left hand
        // side, perform gauss elimination, and break the loop.
        data_expression left = lhs(*j);
        data_expression right = rhs(*j);
        while(is_plus(left))
        {
          if(find_data_variable(lhs(left), *i))
          {
            right = minus(right, rhs(left));
            left = lhs(left);
          }
          else
          {
            right = minus(right, lhs(left));
            left = rhs(left);
          }
          if(is_multiplies(left))
          {
            // Divide out the factor in the right hand side
            data_expression factor = lhs(left);
            left = rhs(left);
            right = gsMakeDataExprDivide(right, factor);
          }
          if(is_negate(left))
          {
            left = r(negate(left));
            right = negate(right);
          }
        }

        if(is_multiplies(left))
        {
          // Divide out the factor in the right hand side
          data_expression factor = lhs(left);
          left = rhs(left);
          right = gsMakeDataExprDivide(right, factor);
        }
        if(is_negate(left))
        {
          left = r(negate(left));
          right = r(negate(right));
        }

        // left := right is the desired substitution, carry out in the other
        // (in)equalities
        atermpp::map<data_expression, data_expression> replacements;
        replacements[left] = right;

        data_expression_list new_inequalities;
        for(data_expression_list::const_iterator k = inequalities.begin(); k != inequalities.end(); ++k)
        {
          if(*k != *j)
          {
            new_inequalities = push_front(new_inequalities, r(realelm_data_expression_map_replace(*k, replacements)));
          }
        }
        inequalities = new_inequalities;
        eliminated_variables = push_front(eliminated_variables, *i);
        break;
      }
    }
  }

  // Remove the variables that we have eliminated from the list of variables
  variables = term_list_difference(variables, eliminated_variables);

  gsDebugMsg("Gauss elimination eliminated variables %P, resulting in the system %P\n", 
                        (ATermList)eliminated_variables, (ATermList)inequalities);

  return inequalities;
}

/// \brief Remove a variable from an inequality
/// \param variable A variable
/// \param inequality An inequality over real numbers
/// \pre inequality is an inequality
/// \ret The inequality from which variable has been removed
static
data_expression remove_variable(const data_variable& variable, const data_expression& inequality)
{
  assert(is_inequality(inequality));

  gsDebugMsg("Removing variable %P from inequality %P\n", (ATermAppl)variable, (ATermAppl)inequality);

  data_expression left = lhs(inequality);
  data_expression new_left = real_zero();
  while(is_plus(left))
  {
    gsDebugMsg("left = %P is a plus expression\n", (ATermAppl)left);
    if(is_multiplies(rhs(left)))
    {
      data_expression factor = lhs(rhs(left));
      new_left = gsMakeDataExprDivide(plus(new_left, lhs(left)), factor);
      return data_application(static_cast<const data_application&>(inequality).head(), make_list(new_left, gsMakeDataExprDivide(rhs(inequality), factor)));
    }
    else if (rhs(left) == variable || rhs(left) == negate(static_cast<const data_expression&>(variable)))
    {
      return data_application(static_cast<const data_application&>(inequality).head(), make_list(plus(new_left, lhs(left)), rhs(inequality)));
    }
    else
    {
      new_left = plus(new_left, rhs(left));
      left = lhs(left);
    }
  }

  gsDebugMsg("left = %P\n", (ATermAppl)left);

  if(is_negate(left))
  {
    data_expression argument = *static_cast<const data_application&>(left).arguments().begin();
    if(is_plus(argument))
    {
      data_expression p = plus(negate(lhs(argument)), negate(rhs(argument)));
      return remove_variable(variable, data_application(static_cast<const data_application&>(inequality).head(), make_list(p, rhs(inequality))));
    }
  }
  if (left == variable || left == negate(static_cast<const data_expression&>(variable)))
  {
    return data_application(static_cast<const data_application&>(inequality).head(), make_list(new_left, rhs(inequality)));
  }

  gsErrorMsg("cannot remove variable %P from %P\n", (ATermAppl)variable, (ATermAppl)inequality);

  assert(false);
}

/// \brief Group inequalities by positive, 0 and negative occurrences of a
///        variable.
/// \param v A variable
/// \param inequalities A list of inequalities
/// \param positive_occurrences an empty list
/// \param zero_occurrences an empty list
/// \param negative_occurrences and empty list
/// \post positive_occurrences contains all inequalities in which c * v occurs,
///       zero_occurrences contains all inequalities in which v does not occur,
///       negative_occurrences contains all inequalities in which -c * v occurs.
static
void group_inequalities(const data_variable& v, const data_expression_list& inequalities,
                        data_expression_list& positive_occurrences,
                        data_expression_list& zero_occurrences,
                        data_expression_list& negative_occurrences)
{
  assert(positive_occurrences.empty());
  assert(zero_occurrences.empty());
  assert(negative_occurrences.empty());
  for(data_expression_list::iterator j = inequalities.begin(); j != inequalities.end(); ++j)
  {
    if(find_data_variable(*j, v))
    {
      if(find_data_expression(*j, negate(static_cast<const data_expression&>(v))))
      {
        negative_occurrences = push_front(negative_occurrences, *j);
      }
      else
      {
        positive_occurrences = push_front(positive_occurrences, *j);
      }
    }
    else
    {
      zero_occurrences = push_front(zero_occurrences, *j);
    }
  }
}

// prototype
static inline
bool add_inequality_to_context(const data_expression& e, atermpp::map<std::pair<data_expression, data_expression>, data_variable>& context, rewriter& r, identifier_generator& variable_generator);

/// \brief Add inequalities to variable context
/// \param l A list of inequalities
/// \param context A variable context
/// \param variable_generator A fresh variable generator
/// \post All inequalities in l are in the context
/// \ret true iff a variable has been added to the context
static inline
bool add_inequalities_to_context(
                data_expression_list l, 
                atermpp::map<std::pair<data_expression, 
                data_expression>, 
                data_variable>& context, 
                rewriter& r, 
                identifier_generator& variable_generator)
{
  bool result = false;
  for(data_expression_list::const_iterator i = l.begin(); i != l.end(); ++i)
  {
    result = result || add_inequality_to_context(*i, context, r, variable_generator);
  }
  return result;
}

/// \brief Add inequality to variable context
/// \param e An inequality
/// \param context A variable context
/// \param variable_generator A fresh variable generator
/// \post e is in the context
/// \ret true iff there was no variable for e in the context
bool add_inequality_to_context(
                const data_expression& e, 
                atermpp::map<std::pair<data_expression, data_expression>, 
                data_variable>& context, 
                rewriter& r, 
                identifier_generator& variable_generator)
{
  if(!is_inequality(e))
  {
    return false;
  }
  assert(is_inequality(e));
  bool result = false;
  data_expression left = lhs(e);
  data_expression right = rhs(e);
  if(is_negative(right) || (right == real_zero() && is_negate(left)))
  {
    left = r(negate(left));
    right = r(negate(right));
  }
  if(context.find(std::make_pair(left, right)) == context.end())
  {
    data_variable xi(variable_generator("xi"), sort_identifier("Comp"));
    context[std::make_pair(left, right)] = xi;
    gsVerboseMsg("Introduced variable %P for < %P, %P >\n", (ATermAppl)xi, (ATermAppl)left, (ATermAppl)right);
    result = true;
  }
  return result;
}

/// \brief Eliminate variables from inequalities using Gauss elimination and
///        Fourrier-Motzkin elimination.
/// \param inequalities A list of inequalities
/// \param variables The list of variables to be eliminated
/// \param r A rewriter
/// \post All variables in variables have been eliminated, inequalities contains
///       the resulting system of inequalities.
static
void fourier_motzkin(data_expression_list& inequalities, data_variable_list variables, rewriter& r)
{
  inequalities = gauss_elimination(inequalities, variables, r);

  if(variables.size() != 0)
  {
    gsDebugMsg("Starting Fourier-Motzkin elimination on system produced by Gauss elimination, with variables %P\n", (ATermList)variables);

    // At this stage, the variables that should be eliminated only occur in
    // inequalities. Group the inequalities into positive, 0, and negative
    // occurrences of each variable, and create a new system.
    for(data_variable_list::iterator i = variables.begin(); i != variables.end(); ++i)
    {
      data_expression_list positive_variables;
      data_expression_list zero_variables;
      data_expression_list negative_variables;

      group_inequalities(*i, inequalities, positive_variables, zero_variables, negative_variables);

      positive_variables = normalize_inequalities(positive_variables, r);
      zero_variables = normalize_inequalities(zero_variables, r);
      negative_variables = normalize_inequalities(negative_variables, r);

      gsDebugMsg("equations with zero occurrence %P\n", (ATermList)zero_variables);
      gsDebugMsg("equations with positive occurrence %P\n", (ATermList)positive_variables);
      gsDebugMsg("equations with negative occurrence %P\n", (ATermList)negative_variables);

      data_expression_list new_inequalities = zero_variables;

      // Variables are grouped, now construct new inequalities as follows:
      // Keep the zero occurrences
      // Combine each positive and negative equation as follows:
      // Given inequalities x1 + bi * x <= ci
      //                   -x1 + bj * x <= cj
      // This is equivalent to bj * x + bi * x <= ci + cj
      for(data_expression_list::iterator j = positive_variables.begin(); j != positive_variables.end(); ++j)
      {
        data_expression positive_inequality = remove_variable(*i, *j);
        gsDebugMsg("positive inequality: %P\n", (ATermAppl)positive_inequality);
        positive_inequality = normalize_inequality(positive_inequality,r);
        gsDebugMsg("positive inequality after normalization: %P\n", (ATermAppl)positive_inequality);
        assert(is_inequality(positive_inequality));
        assert(!is_greater(positive_inequality) && !is_greater_equal(positive_inequality));
        for(data_expression_list::iterator k = negative_variables.begin(); k != negative_variables.end(); ++k)
        {
          gsDebugMsg("combining %P and %P into new inequality\n", (ATermAppl)*j, (ATermAppl)*k);
          data_expression negative_inequality = remove_variable(*i, *k);
          gsDebugMsg("negative inequality: %P\n", (ATermAppl)negative_inequality);
          negative_inequality = normalize_inequality(negative_inequality, r);
          gsDebugMsg("negative inequality after normalization: %P\n", (ATermAppl)negative_inequality);
          // Results may not be inequalities any more
          assert(is_inequality(negative_inequality));
          assert(!is_greater(negative_inequality) && !is_greater_equal(negative_inequality));
          data_expression new_inequality;
          if(is_less(positive_inequality) && is_less(negative_inequality))
          {
            new_inequality = less(plus(lhs(positive_inequality), lhs(negative_inequality)), plus(rhs(positive_inequality), rhs(negative_inequality)));
          }
          else
          {
            new_inequality = less_equal(plus(lhs(positive_inequality), lhs(negative_inequality)), plus(rhs(positive_inequality), rhs(negative_inequality)));
          }
          new_inequality = normalize_inequality(new_inequality, r);
          if(lhs(new_inequality) != real_zero())
          {
            new_inequalities = push_front(new_inequalities, new_inequality);
          }
        }
      }
      inequalities = new_inequalities;
    }
  }
}

/// \brief Compute a concrete inequality given a pair of data expressions and a
///        number
/// \param i a number
/// \param e A pair <x,y> of data expressions
/// \pre 0 <= i <= 2
/// \ret Data expression for the following inequality:
///      x < y if i = 0
///      x = y if i = 1
///      -x < -y if i = 2
static inline
data_expression compute_inequality(unsigned long i, const std::pair<data_expression, data_expression>& e)
{
  if(i == 0)
  {
    return less(e.first, e.second);
  }
  else if(i == 1)
  {
    return equal_to(e.first, e.second);
  }
  else
  {
    assert(i == 2);
    return less(negate(e.first), negate(e.second));
  }
}

/// \brief Compute a list of concrete inequalities given a variable context
/// \param i a number
/// \param context A variable context
/// \ret The list of concrete inequalities corresponding to combination i
static inline
data_expression_list compute_inequalities(unsigned long i, const atermpp::map<std::pair<data_expression, data_expression>, data_variable>& context)
{
  data_expression_list result;
  for(atermpp::map<std::pair<data_expression, data_expression>, data_variable>::const_iterator j = context.begin(); j != context.end(); ++j)
  {
    div_t q = div(i, 3);
    i = q.quot;
    result = push_front(result, compute_inequality(q.rem, j->first));
  }
  return result;
}

/// \brief Apply replacements to a list of inequalities
/// \param inequalities A list of data expressions
/// \param replacements A map of replacements
/// \ret inequalities to which the substitutions in replacements have been
///      applied
data_expression_list data_expression_map_replace_list(const data_expression_list& inequalities, const atermpp::map<data_expression, data_expression>& replacements)
{
  data_expression_list result;
  for(data_expression_list::const_iterator i = inequalities.begin(); i != inequalities.end(); ++i)
  {
    result = push_front(result, realelm_data_expression_map_replace(*i, replacements));
  }
  return result;
}

/// \brief Simplify condition given a context
/// \param cond A list of inequalities
/// \param context A list of inequalities
/// \ret A simplified version of inequalities, assuming context holds.
static
data_expression_list simplify_condition(const data_expression_list& cond, const data_expression_list& context)
{
  data_expression_list result;
  for(data_expression_list::const_iterator i = cond.begin(); i != cond.end(); ++i)
  {
    // Only unique elements in cond
    if(std::find(result.begin(), result.end(), *i) == result.end())
    {
      result = push_front(result, *i);
    }
  }
  return result;
}

/// \brief Determine whether a list of data expressions is inconsistent
/// \param cond A list of inequalities
/// \param r A rewriter
/// \ret true if the system of inequalities can be determined to be
///      inconsistent, false otherwise.
static
bool is_inconsistent(const data_expression_list& cond, rewriter& r)
{
  // Check if any condition rewrites to false.
  if(std::find(cond.begin(), cond.end(), false_()) != cond.end())
  {
    return true;
  }
  // for(data_expression_list::const_iterator j = cond.begin(); j != cond.end(); ++j)
  // { if (false_()==r(*j))  
  //  return true;
  // }
  for(data_expression_list::const_iterator i = cond.begin(); i != cond.end(); ++i)
  {
    for(data_expression_list::const_iterator j = cond.begin(); j != cond.end(); ++j)
    {
      gsDebugMsg("*i = %s, *j = %s\n", (ATermAppl)*i, (ATermAppl)*j);
      if(is_inequality(*i) && is_inequality(*j) && *i != *j && !is_less_equal(*i) && !is_less_equal(*j))
      {
        if(lhs(*i) == lhs(*j))
        {
          if(rhs(*i) == rhs(*j))
          {
            return true;
          }
          else if((is_less(*i) && is_equal_to(*j) && r(less(rhs(*j), rhs(*i))) == false_()) ||
             (is_equal_to(*i) && is_less(*j) && r(less(rhs(*i), rhs(*j))) == false_()))
          {
            return true;
          }
          else if (is_equal_to(*i) && is_equal_to(*j) && rhs(*i) != rhs(*j))
          {
            return true;
          }
        }
        else
        {
          if(rhs(*i) == real_zero() && !is_negate(lhs(*i)) && is_negate(rhs(*j)) &&
             lhs(*i) == r(negate(lhs(*j))))
          {
            if(r(greater(rhs(*j), rhs(*i))) == false_())
            {
              return true;
            }
          }
          else if (rhs(*j) == real_zero() && !is_negate(lhs(*j)) && is_negate(lhs(*i)) &&
            lhs(*j) == r(negate(lhs(*i))))
          {
            if(r(greater(rhs(*i), rhs(*j))) == false_())
            {
              return true;
            }
          }
        }
        gsDebugMsg("Cannot conclude anything\n");
      }
    }
  }
  return false;
}

/// \brief Transform a list of inequalities over Reals to expressions over Cond,
///        given a context.
/// \param cond A list of inequalities
/// \param context A variable context
/// \param r A rewriter
/// \ret The list of inequalities corresponding to cond, but expressed in
///      expressions over sort Cond.
static
data_expression_list transform_real_to_cond(const data_expression_list& cond, atermpp::map<std::pair<data_expression, data_expression>, data_variable>& context, rewriter& r)
{
  data_expression_list result;
  for(data_expression_list::const_iterator i = cond.begin(); i != cond.end(); ++i)
  {
    if(is_inequality(*i))
    {
      data_expression left = lhs(*i);
      data_expression right = rhs(*i);
      if(is_negative(right) || (right == real_zero() && is_negate(left)))
      {
        left = r(negate(left));
        right = r(negate(right));
      }
      atermpp::map<std::pair<data_expression, data_expression>, data_variable>::iterator j;
      j = context.find(std::make_pair(left, right));
      if(j == context.end())
      {
        result = push_front(result, *i);
      }
      else
      {
        if(is_negative(rhs(*i)))
        {
          if(is_less(*i))
          {
            result = push_front(result, static_cast<const data_expression&>(is_larger(j->second)));
          }
          else if(is_less_equal(*i))
          {
            result = push_front(result, or_(static_cast<const data_expression&>(is_larger(j->second)), static_cast<const data_expression&>(is_equal(j->second))));
          }
          else if(is_equal_to(*i))
          {
            result = push_front(result, static_cast<const data_expression&>(is_equal(j->second)));
          }
          else
          {
            assert(false);
          }
        }
        else
        {
          if(is_less(*i))
          {
            result = push_front(result,static_cast<const data_expression&>(is_smaller(j->second)));
          }
          else if(is_less_equal(*i))
          {
            result = push_front(result, or_(static_cast<const data_expression&>(is_smaller(j->second)), static_cast<const data_expression&>(is_equal(j->second))));
          }
          else if(is_equal_to(*i))
          {
            result = push_front(result, static_cast<const data_expression&>(is_equal(j->second)));
          }
          else
          {
            assert(false);
          }
        }
      }
    }
  }
  return result;
}

/// \brief Generate a summand
/// \param s A summand
/// \param i A number, denoting the next state
/// \param cond A list of inequalities denoting the real part of the condition
/// \param context A variable context
/// \param r A rewriter
/// \ret The summand corresponding to s with real part of condition cond, and
///      nextstate determined by i.
static
summand generate_summand(const summand& s, unsigned long i, data_expression_list cond, atermpp::map<std::pair<data_expression, data_expression>, data_variable>& context, rewriter& r)
{
  gsDebugMsg("generating new summand from summand %P, with cond %P, i = %ld\n", (ATermAppl)s, (ATermList)cond, i);

  std::pair<data_expression_list, data_expression_list> real_nonreal_condition = split_conjunct(s.condition());
  data_expression condition = and_(true_(), join_and(real_nonreal_condition.second.begin(), real_nonreal_condition.second.end()));
  cond = transform_real_to_cond(cond, context,r);
  condition = and_(condition, join_and(cond.begin(), cond.end()));

  gsDebugMsg("condition: %P\n", (ATermAppl)condition);

  gsDebugMsg("context: ");
  for(atermpp::map<std::pair<data_expression, data_expression>, data_variable>::iterator j = context.begin(); j != context.end(); ++j)
  {
    gsDebugMsg("< %P, %P > %P, ", (ATermAppl)j->first.first, (ATermAppl)j->first.second, (ATermAppl)j->second);
  }
  gsDebugMsg("\n");

  data_assignment_list nextstate = get_nonreal_assignments(s.assignments());
  nextstate = reverse(nextstate);
  for(atermpp::map<std::pair<data_expression, data_expression>, data_variable>::const_iterator j = context.begin(); j != context.end(); ++j)
  {
    div_t q = div(i, 3);
    i = q.quot;
    if(q.rem == 0)
    {
      nextstate = push_front(nextstate, data_assignment(j->second, data_expression(smaller())));
    }
    else if(q.rem == 1)
    {
      nextstate = push_front(nextstate, data_assignment(j->second, data_expression(equal())));
    }
    else
    {
      assert(q.rem == 2);
      nextstate = push_front(nextstate, data_assignment(j->second, data_expression(larger())));
    }
  }
  nextstate = reverse(nextstate);

  gsDebugMsg("nextstate: %P\n", (ATermList)nextstate);

  summand result = summand(get_nonreal_variables(s.summation_variables()), r(condition), s.is_delta(), s.actions(), nextstate);

  gsDebugMsg("Generated summand %P\n", (ATermAppl)result);

  return result;
}

/// \brief Compute process initialisation given a variable context and a process
///        initialisation containing real values.
/// \param initialization A process initialisation containing real values
/// \param context A variable context
/// \param r A rewriter
/// \ret A process initialisation in which all assignments to real variables
///      have been replaced with an initialization for each variable in context.
data_assignment_list determine_process_initialization(const data_assignment_list& initialization, atermpp::map<std::pair<data_expression, data_expression>, data_variable>& context, rewriter& r)
{
  data_assignment_list init = get_nonreal_assignments(initialization);
  data_assignment_list real_assignments = get_real_assignments(initialization);
  atermpp::map<data_expression, data_expression> replacements;
  for(data_assignment_list::const_iterator i = real_assignments.begin(); i != real_assignments.end(); ++i)
  {
    replacements[i->lhs()] = i->rhs();
  }

  for(atermpp::map<std::pair<data_expression, data_expression>, data_variable>::const_iterator i = context.begin(); i != context.end(); ++i)
  {
    data_expression left = realelm_data_expression_map_replace(i->first.first, replacements);
    data_expression right = realelm_data_expression_map_replace(i->first.second, replacements);
    data_assignment assignment;
    if(r(less(left, right)) == true_())
    {
      assignment = data_assignment(i->second, smaller());
    }
    else if(r(equal_to(left, right)) == true_())
    {
      assignment = data_assignment(i->second, equal());
    }
    else
    {
      assert(r(greater(left, right)) == true_());
      assignment = data_assignment(i->second, larger());
    }
    init = push_front(init, assignment);
  }
  return reverse(init);
}

/// \brief Perform elimination of real variables on a specification in a maximum
///        number of iterations.
/// \param s A specification
/// \param max_iterations The maximal number of iterations the algorithm should
///        perform
/// \param strategy The rewrite strategy that should be used.
specification realelm(specification s, int max_iterations, RewriteStrategy strategy)
{
  gsDebugMsg("Performing real time abstraction with a maximum of %d iterations\n", max_iterations);
  s = set_data_specification(s, add_comp_sort(s.data()));
  rewriter r = rewriter(add_ad_hoc_real_equations(s.data()), (rewriter::strategy)strategy);
  postfix_identifier_generator variable_generator("");
  variable_generator.add_to_context(s);
  s = normalize_specification(s, r, variable_generator);

  linear_process lps = s.process();

  atermpp::map<std::pair<data_expression, data_expression>, data_variable> context; // Contains introduced variables
  data_variable_list real_parameters = get_real_variables(lps.process_parameters());
  data_variable_list nonreal_parameters = get_nonreal_variables(lps.process_parameters());


  // Compute some context information for each summand.
  atermpp::map<summand, data_expression_list> summand_real_conditions;
  atermpp::map<summand, atermpp::map<data_expression, data_expression> > summand_real_nextstate_map;
  for(summand_list::const_iterator i = lps.summands().begin(); i != lps.summands().end(); ++i)
  {
    data_expression_list inequalities;
    determine_real_inequalities(i->condition(), inequalities);
    summand_real_conditions[*i] = inequalities;
    // Replacements is the nextstate vector in a map
    atermpp::map<data_expression, data_expression> replacements;
    for(data_assignment_list::const_iterator j = i->assignments().begin(); j != i->assignments().end(); ++j)
    {
      if(j->lhs().sort() == sort_expr::real())
      {
        replacements[j->lhs()] = j->rhs();
      }
    }
    summand_real_nextstate_map[*i] = replacements;
    gsDebugMsg("replacements for summand %P:\n", (ATermAppl)*i);
    for(atermpp::map<data_expression, data_expression>::const_iterator j = replacements.begin(); j != replacements.end(); ++j)
    {
      gsDebugMsg("%P := %P, ", (ATermAppl)j->first, (ATermAppl)j->second);
    }
    gsDebugMsg("\n");
  }

  bool changed = false;
  int iteration = 0;
  summand_list summands;
  do
  {
    changed = false;
    summands = summand_list();
    assert(summands.empty());
    gsVerboseMsg("Iteration %d, starting with %d context variables\n", iteration++, context.size());

    for(summand_list::const_iterator i = lps.summands().begin(); i != lps.summands().end(); ++i)
    {
      unsigned long context_combinations = pow(3, context.size()); //Combinations to be considered
      gsDebugMsg("Considering %ld combinations for context\n", context_combinations);

      for(unsigned long context_combination = 0; context_combination < context_combinations; ++context_combination)
      {
        // xi == xi'
        data_expression_list context_inequalities = compute_inequalities(context_combination, context);

        gsDebugMsg("inequalites from context: %P\n", (ATermList)context_inequalities);

        for(unsigned long nextstate_combination = 0; nextstate_combination < context_combinations; ++ nextstate_combination)
        {
          gsDebugMsg("context = %ld, nextstate = %ld\n", context_combination, nextstate_combination);
          // zeta
          data_expression_list condition = compute_inequalities(nextstate_combination, context);
          gsDebugMsg("inequalities for condition before substitution: %P\n", (ATermList)condition);
          // zeta[x := g(x)]
          condition = data_expression_map_replace_list(condition, summand_real_nextstate_map[*i]);

          gsDebugMsg("inequalities for condition: %P\n", (ATermList)condition);

          // original condition of the summand && zeta[x := g(x)]
          condition = condition + summand_real_conditions[*i];
          gsDebugMsg("condition: %P\n", (ATermList)condition);
          // simplify this condition in the context of xi'
          condition = simplify_condition(condition, context_inequalities);
          gsDebugMsg("cond after simplification: %P\n", (ATermList)condition);
          condition = normalize_inequalities(condition, r);

          // Eliminate sum bound variables, resulting in inequalities over
          // process parameters of sort Real. Of this, we add the inequalities
          // that are not yet in the context.
          gsVerboseMsg("inequalities before fourier-motzkin: %P\n", (ATermList)condition);
          fourier_motzkin(condition, i->summation_variables(), r);
          if(!is_inconsistent(condition, r))
          {
            condition = normalize_inequalities(condition, r);
            if(!is_inconsistent(condition, r))
            {
              gsVerboseMsg("inequalities after fourier-motzkin: %P\n", (ATermList)condition);
              // condition contains the inequalities over the process parameters
              changed = changed || add_inequalities_to_context(condition, context, r, variable_generator);
              if(!changed)
              {
                summand s = generate_summand(*i, nextstate_combination, condition, context, r);
                if(s.condition() != false_() && std::find(summands.begin(), summands.end(), s) == summands.end())
                {
                  summands = push_front(summands, s);
                }
              }
            }
            else
            {
              gsDebugMsg("System inconsistent after normalization\n");
            }
          }
          else
          {
            gsDebugMsg("System is inconsistent\n");
          }
        }
      }
    }
  } while ((iteration < max_iterations) && changed);

  if(changed)
  {
    gsVerboseMsg("No exact solution computed\n");
  }

  gsVerboseMsg("generated the following variables in %d iterations:\n", iteration);
  for(atermpp::map<std::pair<data_expression, data_expression>, data_variable>::iterator i = context.begin(); i != context.end(); ++i)
  {
    gsVerboseMsg("< %P, %P > %P\n", (ATermAppl)i->first.first, (ATermAppl)i->first.second, (ATermAppl)i->second);
  }

  summands = reverse(summands);
  gsVerboseMsg("Computed %d summands %P\n", summands.size(), (ATermList)summands);

  // Process parameters
  data_variable_list process_parameters = reverse(nonreal_parameters);
  for(atermpp::map<std::pair<data_expression, data_expression>, data_variable>::const_iterator i = context.begin(); i != context.end(); ++i)
  {
    process_parameters = push_front(process_parameters, i->second);
  }
  process_parameters = reverse(process_parameters);

  // New lps
  lps = linear_process(lps.free_variables(), process_parameters, summands);
  s = set_lps(s, lps);

  // New process initializer
  data_assignment_list initialization(determine_process_initialization(s.initial_process().assignments(), context, r));
  process_initializer init(s.initial_process().free_variables(), initialization);
  s = set_initial_process(s, init);

  return s;
}

