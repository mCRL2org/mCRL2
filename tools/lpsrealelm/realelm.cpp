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
#include "real.h"
#include "realelm.h"

using namespace atermpp;
using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::data::data_expr;
using namespace mcrl2::lps;

// Implementation of integer exponentiation
static
unsigned long pow(unsigned long base, unsigned long exponent)
{
  if(exponent == 0)
  {
    return 1;
  }
  else if(exponent == 1)
  {
    return base;
  }
  else
  {
    div_t q = div(exponent, 2);
    if(q.rem == 0)
    {
      return pow(base << 1, q.quot);
    }
    else
    {
      //q.rem == 1
      return pow((base*base) << 1, q.quot);
    }
  }
}

// Three valued type used for fourrier-motzkin elimination
typedef enum { maybe_true, maybe_false, maybe_unknown } maybe;

// Retrieve the left hand side of a data expression
static inline
data_expression lhs(const data_expression e)
{
  return *(static_cast<const data_application&>(e).arguments().begin());
}

// Retrieve the right hand side of a data expression
static inline
data_expression rhs(const data_expression e)
{
  return *(++static_cast<const data_application&>(e).arguments().begin());
}

// Determine whether a data expression is a constant expression encoding a
// number
static inline
bool is_number(const data_expression e)
{
  return core::detail::gsIsDataExprC0(e) ||
         core::detail::gsIsDataExprCDub(e) ||
         core::detail::gsIsDataExprC1(e) ||
         core::detail::gsIsDataExprCNat(e) ||
         core::detail::gsIsDataExprCNeg(e) ||
         core::detail::gsIsDataExprCInt(e) ||
         core::detail::gsIsDataExprCReal(e) ||
         (is_data_application(e) && static_cast<const data_application&>(e).head() == rational());
}

// Determine wheter a number is negative
static inline
bool is_negative(const data_expression e)
{
  return is_negate(e) || gsIsDataExprCNeg(e) || (is_rational(e) && is_negative(lhs(e)));
}

// Determine whether a data expression is an inequality
static inline
bool is_inequality(const data_expression& e)
{
  return is_equal_to(e) || is_less(e) || is_less_equal(e) || is_greater(e) ||
         is_greater_equal(e);
}

// Determine whether one data expression is less or equal to an other data
// expression. This assumes for e1 and e2 that they are either:
// - negation
// - multiplication with a number as left hand side
// - data variable
static
bool less_or_equal(const data_expression& e1, const data_expression& e2)
{
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
  else
  {
    assert(is_data_variable(e1) && is_data_variable(e2));
    return (static_cast<const data_variable&>(e1).name() <= static_cast<const data_variable&>(e2).name());
  }
}

// Merge two sorted lists of data expressions
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

// Sort a data expression
// Assume data expression e is of the form c1 * x1 + ... + cn * xn, sort
// the expression in lexicographic order of variable names
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

// Assuming data expression e is of the form c1 * x1 + ... + cn * xn + d1 + ... + dn,
// split the data expression in a part contiaining c1 * x1 + ... + cn * xn, and
// a part containing d1 + ... + dn
static
std::pair<data_expression, data_expression> split_constants_and_variables(const data_expression& e)
{
  data_expression variable_part = real_zero();
  data_expression constant_part = real_zero();
  if(is_plus(e))
  {
    std::pair<data_expression, data_expression> left = split_constants_and_variables(lhs(e));
    std::pair<data_expression, data_expression> right = split_constants_and_variables(rhs(e));
    return std::make_pair(plus(left.first, right.first), plus(left.second, right.second));
  }
  else if (is_negate(e))
  {
    return std::make_pair(e, real_zero());
  }
  else if (is_minus(e))
  {
    std::pair<data_expression, data_expression> left = split_constants_and_variables(lhs(e));
    std::pair<data_expression, data_expression> right = split_constants_and_variables(rhs(e));
    return std::make_pair(plus(left.first, negate(right.first)), minus(left.second, right.second));
  }
  else if (is_multiplies(e))
  {
    return std::make_pair(e, real_zero());
  }
  else if (is_data_variable(e))
  {
    return std::make_pair(e, real_zero());
  }
  else
  {
    assert(is_number(e));
    return std::make_pair(real_zero(), e);
  }
}

// Returns a list of all real variables in l
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

// Returns a list of all nonreal variables in l
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

// Returns a list of all real expressions in l
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

// Returns a list of all nonreal expressions in l
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

// Returns a list of all real assignments in l
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

// Returns a list of all nonreal assignments in l
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

// Transform the inequality in e such that the left hand side only contains
// arguments of the form ci * xi, this is variables, possibly multiplied by a
// constant, and the right hand side contains a single constant.
static
data_expression normalize_inequality(const data_expression& e, rewriter& r)
{
  data_application d = static_cast<const data_application&>(e);
  std::pair<data_expression, data_expression> left = split_constants_and_variables(lhs(d));
  std::pair<data_expression, data_expression> right = split_constants_and_variables(rhs(d));

  data_expression new_left = plus(left.first, negate(right.first));
  data_expression new_right = r(minus(right.second, left.second));
  d = data_application(d.head(), make_list(new_left, new_right));
  
  data_expression res = r(d);

  return res;
}

// Normalize the inequalities in l, that is, make sure that for each of the
// expressions in l it holds that the left hand side is of the form
// c1*x1 + ... + cn*xn
// and the right hand side is a constant.
static
data_expression_list normalize_inequalities(const data_expression_list& l, rewriter& r)
{
  data_expression_list result;
  for(data_expression_list::iterator i = l.begin(); i != l.end() ; ++i)
  {
    data_expression inequality = r(*i);
    if(is_equal_to(inequality) || is_less(inequality) || is_less_equal(inequality))
    {
      inequality = normalize_inequality(inequality, r);
      if(!find_data_expression(result, inequality))
      {
        result = push_front(result,normalize_inequality(inequality, r));
      }
    }
    else
    {
      gsDebugMsg("Unexpected inequality %s occurred\n", pp(inequality).c_str());
    }
  }
  return result;
}

// Assuming data expression is a conjunct at the top level
// Split the expression into a list of real expressions and a list of non-real
// expressions
// Returns a pair (r, x) such that r is a list of real expressions, x is a list
// of non-real expressions
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

  if((is_equal_to(e) || is_less(e) || is_greater(e) || is_less_equal(e) || is_greater_equal(e)) &&
    (lhs(e).sort() == sort_expr::real() || rhs(e).sort() == sort_expr::real()))
  {
    real_conjuncts = push_front(real_conjuncts, e);
  }
  else
  {
    nonreal_conjuncts = push_front(nonreal_conjuncts, e);
  }
  return std::make_pair(real_conjuncts, nonreal_conjuncts);
}

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

// Transform all inequalities in the summands of the specification to be
// normalized
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

// Determine the inequalities ranging over real numbers in data expression e,
// and store them in inequalities
static
void determine_inequalities(const data_expression& e, data_expression_list& inequalities)
{
  if (is_and(e))
  {
    determine_inequalities(lhs(e), inequalities);
    determine_inequalities(rhs(e), inequalities);
  }
  else if (is_equal_to(e) || is_less(e) || is_less_equal(e))
  {
    inequalities = push_front(inequalities, e);
  }
  else
  {
    if(!is_true(e))
    {
      gsErrorMsg("Unexpected conjunct %s\n", pp(e).c_str());
    }
    assert(is_true(e));
  }
}

// post: variables contains the list of variables that have not been eliminated
//       by gauss elimination
static
data_expression_list gauss_elimination(data_expression_list inequalities, data_variable_list& variables, rewriter& r)
{
  gsDebugMsg("Trying to eliminate variables %s from system %s using gauss elimination\n", pp(variables).c_str(), pp(inequalities).c_str());

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
            right = divide(right, factor);
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
          right = divide(right, factor);
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

        for(atermpp::map<data_expression, data_expression>::const_iterator l = replacements.begin(); l != replacements.end(); ++l)
        {
          gsVerboseMsg("substitution %s := %s\n", pp(l->first).c_str(), pp(l->second).c_str());
        }

        data_expression_list new_inequalities;
        for(data_expression_list::const_iterator k = inequalities.begin(); k != inequalities.end(); ++k)
        {
          if(*k != *j)
          {
            new_inequalities = push_front(new_inequalities, r(data_expression_map_replace(*k, replacements)));
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

  gsDebugMsg("Gauss elimination eliminated variables %s, resulting in the system %s\n", pp(eliminated_variables).c_str(), pp(inequalities).c_str());

  return inequalities;
}

// Remove variable from the inequality, used for Fourrier-Motzkin elimination,
// eliminating variable
static
data_expression remove_variable(const data_variable& variable, const data_expression& inequality)
{
  data_expression left = lhs(inequality);
  data_expression new_left = real_zero();
  while(is_plus(left))
  {
    if(is_multiplies(lhs(left)))
    {
      data_expression factor = lhs(lhs(left));
      new_left = divide(plus(new_left, rhs(left)), factor);
      return data_application(static_cast<const data_application&>(inequality).head(), make_list(new_left, divide(rhs(inequality), factor)));
    }
    else if (find_data_variable(left, variable))
    {
      return data_application(static_cast<const data_application&>(inequality).head(), make_list(plus(new_left, rhs(left)), rhs(inequality)));
    }
    else
    {
      new_left = plus(new_left, lhs(left));
      left = rhs(left);
    }
  }

  if(is_negate(left) && is_plus(lhs(left)))
  {
    data_expression p = plus(negate(lhs(lhs(left))), negate(rhs(lhs(left))));
    return remove_variable(variable, data_application(static_cast<const data_application&>(inequality).head(), make_list(p, rhs(inequality))));
  }

  if (left == variable)
  {
    return data_application(static_cast<const data_application&>(inequality).head(), make_list(new_left, rhs(inequality)));
  }
  else if (left == negate(static_cast<const data_expression&>(variable)))
  {
    return data_application(static_cast<const data_application&>(inequality).head(), make_list(new_left, rhs(inequality)));
  }

  gsErrorMsg("cannot remove variable %s from %s\n", pp(variable).c_str(), pp(inequality).c_str());

  assert(false);
}

// Group inequalities by positive, 0 and negative occurrences of *i
static
void group_inequalities(const data_variable& v, const data_expression_list& inequalities,
                        data_expression_list& positive_occurrences,
                        data_expression_list& zero_occurrences,
                        data_expression_list& negative_occurrences) 
{
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
static
bool add_inequality_to_context(const data_expression& e, atermpp::map<std::pair<data_expression, data_expression>, data_variable>& context, rewriter& r, identifier_generator& variable_generator);

static
bool add_inequalities_to_context(const data_expression_list& l, atermpp::map<std::pair<data_expression, data_expression>, data_variable>& context, rewriter& r, identifier_generator& variable_generator)
{
  bool result = false;
  for(data_expression_list::const_iterator i = l.begin(); i != l.end(); ++i)
  {
    result = result || add_inequality_to_context(*i, context, r, variable_generator);
  }
  return result;
}

// Add variable for inequality to context
static
bool add_inequality_to_context(const data_expression& e, atermpp::map<std::pair<data_expression, data_expression>, data_variable>& context, rewriter& r, identifier_generator& variable_generator)
{
  assert(is_inequality(e));
  bool result = false;
  data_expression left = lhs(e);
  data_expression right = rhs(e);
  if(is_negative(right))
  {
    left = r(negate(left));
    right = r(negate(right));
  }
  if(context.find(std::make_pair(left, right)) == context.end())
  {
    data_variable xi(variable_generator("xi"), sort_identifier("Comp"));
    context[std::make_pair(left, right)] = xi;
    gsVerboseMsg("Introduced variable %s for < %s, %s >\n", pp(xi).c_str(), pp(left).c_str(), pp(right).c_str());
    result = true;
  }
  return result;
}

static
void fourrier_motzkin(data_expression_list& inequalities, data_variable_list variables, rewriter& r)
{
  inequalities = gauss_elimination(inequalities, variables, r);

  if(variables.size() != 0)
  {
    gsDebugMsg("Starting Fourier-Motzkin elimination on system produced by Gauss elimination, with variables %s\n", pp(variables).c_str());

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

      gsDebugMsg("equations with zero occurrance %s\n", pp(zero_variables).c_str());
      gsDebugMsg("equations with positive occurrance %s\n", pp(positive_variables).c_str());
      gsDebugMsg("equations with negative occurrance %s\n", pp(negative_variables).c_str());

      data_expression_list new_inequalities = zero_variables;

      // Variables are grouped, now construct new inequalities as follows:
      // Keep the zero occurrences
      // Combine each positive and negative equation as follows:
      // Given inequalities x1 + bi * x <= ci
      //                   -x1 + bj * x <= cj
      // This is equivalent to bj * x + bi * x <= ci + cj
      for(data_expression_list::iterator j = positive_variables.begin(); j != positive_variables.end(); ++j)
      {
        data_expression positive_inequality = normalize_inequality(remove_variable(*i, *j), r);
        if(is_inequality(positive_inequality))
        {
          assert(!is_greater(positive_inequality) && !is_greater_equal(positive_inequality));
          for(data_expression_list::iterator k = negative_variables.begin(); k != negative_variables.end(); ++k)
          {
            gsDebugMsg("combining %s and %s into new inequality\n", pp(*j).c_str(), pp(*k).c_str());
            data_expression negative_inequality = normalize_inequality(remove_variable(*i, *k), r);
            // Results may not be inequalities any more
            if(is_inequality(negative_inequality))
            {
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

              new_inequalities = push_front(new_inequalities, new_inequality);
            }
            else
            {
              gsDebugMsg("negative inequality was not an inequality, was %s\n", pp(negative_inequality).c_str());
            }
          }
        }
        else
        {
          gsDebugMsg("positive inequality was not an inequality, was %s\n", pp(positive_inequality).c_str());
        }
      }
      inequalities = new_inequalities;
    }

    inequalities = normalize_inequalities(inequalities, r);
  }
}

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

data_expression compute_condition_from_inequalities(data_expression_list inequalities, const data_variable_list& variables, rewriter& r, atermpp::map<std::pair<data_expression, data_expression>, data_variable>& context, identifier_generator& variable_generator)
{
  // Compute new condition
  fourrier_motzkin(inequalities, variables, r);
  gsVerboseMsg("condition inequalities: %s\n", pp(inequalities).c_str());
  data_expression condition = true_();
  for(data_expression_list::const_iterator j = inequalities.begin(); j != inequalities.end(); ++j)
  {
    data_expression left = lhs(*j);
    data_expression right = rhs(*j);
    if(is_negative(right))
    {
      left = r(negate(left));
      right = r(negate(right));
      data_expression variable = context[std::make_pair(left, right)];
      if(is_less(*j))
      {
        condition = and_(condition, is_larger(variable));
      }
      else if(is_less_equal(*j))
      {
        condition = and_(condition, or_(is_larger(variable), is_equal(variable)));
      }
      else
      {
        assert(is_equal_to(*j));
        condition = and_(condition, is_equal(variable));
      }
    }
    else
    {
      data_expression variable = context[std::make_pair(left, right)];
      if(is_less(*j))
      {
        condition = and_(condition, is_smaller(variable));
      }
      else if(is_less_equal(*j))
      {
        condition = and_(condition, or_(is_smaller(variable), is_equal(variable)));
      }
      else
      {
        assert(is_equal_to(*j));
        condition = and_(condition, is_equal(variable));
      }
    }
  }
  return condition;
}

data_expression_list data_expression_map_replace_list(const data_expression_list& inequalities, const atermpp::map<data_expression, data_expression>& replacements)
{
  data_expression_list result;
  for(data_expression_list::const_iterator i = inequalities.begin(); i != inequalities.end(); ++i)
  {
    result = push_front(result, data_expression_map_replace(*i, replacements));
  }
  return result;
}

static
data_expression_list simplify_cond(const data_expression_list& cond, const data_expression_list& context)
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

static
data_expression_list transform_real_to_cond(const data_expression_list& cond, atermpp::map<std::pair<data_expression, data_expression>, data_variable>& context, rewriter& r)
{
  data_expression_list result;
  for(data_expression_list::const_iterator i = cond.begin(); i != cond.end(); ++i)
  {
    data_expression left = lhs(*i);
    data_expression right = rhs(*i);
    if(is_negative(right))
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
  return result;
}

static
summand generate_summand(const summand& s, unsigned long i, data_expression_list cond, atermpp::map<std::pair<data_expression, data_expression>, data_variable>& context, const data_expression_list& nextstate_inequalities, rewriter& r)
{
  gsDebugMsg("generating new summand from summand %s, with cond %s and nextstate inequalities %s\n", pp(s).c_str(), pp(cond).c_str(), pp(nextstate_inequalities).c_str());

  std::pair<data_expression_list, data_expression_list> real_nonreal_condition = split_conjunct(s.condition());
  data_expression condition = and_(true_(), join_and(real_nonreal_condition.second.begin(), real_nonreal_condition.second.end()));
  cond = transform_real_to_cond(cond, context,r);
  condition = and_(condition, join_and(cond.begin(), cond.end()));

  gsDebugMsg("condition: %s\n", pp(condition).c_str());

  gsVerboseMsg("context: ");
  for(atermpp::map<std::pair<data_expression, data_expression>, data_variable>::iterator j = context.begin(); j != context.end(); ++j)
  {
    gsVerboseMsg("< %s, %s > %s, ", pp(j->first.first).c_str(), pp(j->first.second).c_str(), pp(j->second).c_str());
  }
  gsVerboseMsg("\n");

  data_assignment_list nextstate;
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
  assert(nextstate.size() == context.size());

  gsDebugMsg("nextstate: %s\n", pp(nextstate).c_str());

  summand result = summand(get_nonreal_variables(s.summation_variables()), r(condition), s.is_delta(), s.actions(), nextstate);

  gsVerboseMsg("Generated summand %s\n", pp(result).c_str());

  return result;
}

specification realelm(specification s)
{
  gsDebugMsg("Performing real time abstraction\n");
  s = set_data_specification(s, replace_real_implementation(s.data()));
  s = set_data_specification(s, add_comp_sort(s.data()));
  rewriter r(s.data());
  postfix_identifier_generator variable_generator("");
  variable_generator.add_to_context(s);
  s = normalize_specification(s, r, variable_generator);

  linear_process lps = s.process();

  atermpp::map<std::pair<data_expression, data_expression>, data_variable> context; // Contains introduced variables
  data_variable_list real_parameters = get_real_variables(lps.process_parameters());
  data_variable_list nonreal_parameters = get_nonreal_variables(lps.process_parameters());

  int iteration = 1;
  int max_iterations = 5;
  gsDebugMsg("Maximum number of iterations is %d\n", max_iterations);

  // Compute some context information for each summand.
  atermpp::map<summand, data_expression_list> summand_real_conditions;
  atermpp::map<summand, atermpp::map<data_expression, data_expression> > summand_real_nextstate_map;
  for(summand_list::const_iterator i = lps.summands().begin(); i != lps.summands().end(); ++i)
  {
    data_expression_list inequalities;
    determine_inequalities(i->condition(), inequalities);
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
  }

  bool changed = false;
  summand_list summands;
  do
  {
    changed = false;
    summands = summand_list();
    assert(summands.empty());
    gsDebugMsg("Iteration %d\n", ++iteration);

    for(summand_list::const_iterator i = lps.summands().begin(); i != lps.summands().end(); ++i)
    {
      unsigned long context_combinations = pow(3, context.size()); //Combinations to be considered

      for(unsigned long context_combination = 0; context_combination < context_combinations; ++context_combination)
      {
        data_expression_list context_inequalities = compute_inequalities(context_combination, context);

        gsDebugMsg("inequalites from context: %s\n", pp(context_inequalities).c_str());

        for(unsigned long nextstate_combination = 0; nextstate_combination < context_combinations; ++ nextstate_combination)
        {
          data_expression_list nextstate_inequalities = compute_inequalities(nextstate_combination, context);
          nextstate_inequalities = data_expression_map_replace_list(nextstate_inequalities, summand_real_nextstate_map[*i]);

          data_expression_list cond = nextstate_inequalities + summand_real_conditions[*i];
          cond = simplify_cond(cond, context_inequalities);
          cond = normalize_inequalities(cond, r);

          // Eliminate sum bound variables, resulting in inequalities over
          // process parameters of sort Real. Of this, we add the inequalities
          // that are not yet in the context.
          gsVerboseMsg("inequalities before fourier-motzkin: %s\n", pp(cond).c_str());
          fourrier_motzkin(cond, i->summation_variables(), r);
          gsVerboseMsg("inequalities after fourier-motzkin: %s\n", pp(cond).c_str());
          // cond contains the inequalities over the process parameters
          changed = changed || add_inequalities_to_context(cond, context, r, variable_generator);
          summand s = generate_summand(*i, nextstate_combination, cond, context, nextstate_inequalities, r);
          if(s.condition() != false_() && std::find(summands.begin(), summands.end(), s) == summands.end())
          {
            summands = push_front(summands, s);
          }
        }
      }
    }
  } while ((iteration <= max_iterations) && changed);

  gsVerboseMsg("generated the following variables in %d iterations:\n", iteration);
  for(atermpp::map<std::pair<data_expression, data_expression>, data_variable>::iterator i = context.begin(); i != context.end(); ++i)
  {
    gsVerboseMsg("< %s, %s > %s\n", pp(i->first.first).c_str(), pp(i->first.second).c_str(), pp(i->second).c_str());
  }

  summands = reverse(summands);
  gsVerboseMsg("Computed %d summands %s\n", summands.size(), pp(summands).c_str());

  data_variable_list process_parameters = reverse(nonreal_parameters);
  for(atermpp::map<std::pair<data_expression, data_expression>, data_variable>::const_iterator i = context.begin(); i != context.end(); ++i)
  {
    process_parameters = push_front(process_parameters, i->second);
  }
  process_parameters = reverse(process_parameters);

  lps = linear_process(lps.free_variables(), process_parameters, summands);
  s = set_lps(s, lps);

  return s;
}

