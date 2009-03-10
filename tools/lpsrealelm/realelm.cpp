// Author(s): Jan Friso Groote and Jeroen Keiren
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

#include "mcrl2/atermpp/set_operations.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/postfix_identifier_generator.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/sort_utility.h"

#include "realelm.h"
#include "linear_inequalities.h"

using namespace atermpp;
using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::data::data_expr;
using namespace mcrl2::lps;

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


static data_expression negate_inequality(const data_expression e)
{
  if (is_equal_to(e))
  { return not_equal_to(lhs_(e),rhs_(e));
  }
  if (is_not_equal_to(e))
  { return equal_to(lhs_(e),rhs_(e));
  }
  else if (is_less(e))
  { return greater_equal(lhs_(e),rhs_(e));
  }
  else if (is_less_equal(e)) 
  { return greater(lhs_(e),rhs_(e));
  }
  else if (is_greater(e))
  { return less_equal(lhs_(e),rhs_(e));
  }
  else if (is_greater_equal(e))
  { return less(lhs_(e),rhs_(e));
  }
  else 
  { throw mcrl2::runtime_error("Expression " + pp(e) + " is expected to be an inequality over sort Real");
  }
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
  // gsDebugMsg("less or equal %P, %P\n", (ATermAppl)e1, (ATermAppl)e2);
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
    return less_or_equal(rhs_(e1), e2);
  }
  else if (is_multiplies(e2))
  {
    return less_or_equal(e1, rhs_(e2));
  }
  else if (gsIsDataExprCReal(e1))
  {
    return less_or_equal(lhs_(e1), e2);
  }
  else if (gsIsDataExprCReal(e2))
  {
    return less_or_equal(e1, lhs_(e2));
  }
  else if (gsIsDataExprCInt(e1))
  {
    return true;
  }
  else if (gsIsDataExprCInt(e2) )
  {
    return false;
  }

  // gsDebugMsg("e1 = %s, e2 = %s\n", (ATermAppl)e1, (ATermAppl)e2);
  assert(is_data_variable(e1) && is_data_variable(e2));
  return (static_cast<const data_variable&>(e1).name() <= static_cast<const data_variable&>(e2).name());
}

/// \brief Merge two sorted lists of data expressions
/// \param l1 A list of data expressions
/// \param l2 A list of data expressions
/// \pre l1 and l2 are sorted
/// \ret The sorted list of data expressions consisting of all elements in l1
///      and l2
/* static
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
} */

/// \brief Sort a data expression
/// \param e A data expression of the form c1 * x1 + ... + cn * xn
/// \ret The list of data expressions ci * xi, such that it is sorted
///      in lexicographic order of the names of xi
/* static inline
data_expression_list sort(const data_expression& e)
{
  data_expression_list r;
  if (is_plus(e))
  {
    r = merge(sort(lhs_(e)), sort(rhs_(e)));
  }
  else
  {
    r = push_front(r, e);
  }
  return r;
} */

/// \brief Order the variables in the lhs_ of an inequality, using a lexicographic order.
/// \param inequality An inequality
/// \param r A rewriter
/// \param negated If initially true, it allows to negate the data expression. If it results
//         in true, the data expression has been negated.
/// \ret A version of the inequality in which the variables occur in
///      lexicographically sorted order.
/* static inline
data_expression order_lhs_inequality(const data_expression left, const rewriter& r,bool &negated)
{
  data_expression_list sorted = sort(left);
  if (sorted.empty())
  { return real_zero();
  }
  if (negated)
  { negated=is_negate(sorted.front());
  }

  data_expression result = real_zero();

  for(data_expression_list::iterator j = sorted.begin(); j != sorted.end(); ++j)
  {
    result = plus(result, *j);
  }
  if (negated)
  { result=negate(result);
  }
  return r(result);
} */

/// \brief Split constant and variable parts of a data expression
/// \param e A data expression of the form c1 * x1 + ... + cn * xn + d1 + ... +
///          dm, where ci and di are constants and xi are variables. Constants
///          and variables may occur mixed.
/// \ret The pair (c1 * x1 + ... + cn * xn, d1 + ... + dm)
static
std::pair<data_expression, data_expression> split_variables_and_constants(const data_expression& e)
{
  // gsDebugMsg("Splitting constants and variables in %P\n", (ATermAppl)e);
  std::pair<data_expression, data_expression> result;
  if(is_plus(e))
  {
    std::pair<data_expression, data_expression> left = split_variables_and_constants(lhs_(e));
    std::pair<data_expression, data_expression> right = split_variables_and_constants(rhs_(e));
    result = std::make_pair(plus(left.first, right.first), plus(left.second, right.second));
  }
  else if (is_minus(e))
  {
    std::pair<data_expression, data_expression> left = split_variables_and_constants(lhs_(e));
    std::pair<data_expression, data_expression> right = split_variables_and_constants(rhs_(e));
    result = std::make_pair(plus(left.first, negate(right.first)), plus(left.second, negate(right.second)));
  }
  else if (is_negate(e))
  {
    data_expression argument = *static_cast<const data_application&>(e).arguments().begin();
    if(is_plus(argument))
    {
      result = split_variables_and_constants(plus(negate(lhs_(argument)), negate(rhs_(argument))));
    }
    else if(is_minus(argument))
    {
      result = split_variables_and_constants(plus(negate(lhs_(argument)), rhs_(argument)));
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
    assert(is_number(e));
    result = std::make_pair(real_zero(), e);
  }
  // gsDebugMsg("split version: left = %P, right = %P\n", (ATermAppl)result.first, (ATermAppl)result.second);
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

// Functions below should have been defined in the data library.
static data_expression condition_part(const data_expression e)
{
  assert(is_data_application(e));
  assert(gsIsDataExprIf(e));
  data_expression_list arguments = static_cast<const data_application&>(e).arguments();
  assert(arguments.size() == 3); 
  return *(arguments.begin());
}

static data_expression then_part(const data_expression e)
{
  assert(is_data_application(e));
  assert(gsIsDataExprIf(e));
  data_expression_list arguments = static_cast<const data_application&>(e).arguments();
  assert(arguments.size() == 3); 
  return *(++arguments.begin());
}

static data_expression else_part(const data_expression e)
{
  assert(is_data_application(e));
  assert(gsIsDataExprIf(e));
  data_expression_list arguments = static_cast<const data_application&>(e).arguments();
  assert(arguments.size() == 3); // Allow application to be applied on unary functions!
  return *(++(++arguments.begin()));
}


/// \brief Splits a condition in expressions ranging over reals and the others
/// \details Conceptually, the condition is first transformed to conjunctive
///          normalform. For each conjunct, there will be an entry in both
///          resulting vectors, where the real conditions are in "real_conditions",
///          and the others in non_real_conditions. If there are conjuncts with
///          both real and non-real variables an exception is thrown. If negate
///          is true the result will be negated.
/// \param e A data expression of sort bool.
/// \param real_condition Those parts of e with only variables over sort Real.
/// \param non_real_condition Those parts of e with only variables not of sort Real.
/// \param negate A boolean variable that indicates whether the result must be negated.
/// \pre The parameter e must be of sort Bool.

static void split_condition(
                const data_expression e,
                atermpp::vector < data_expression_list > &real_conditions,
                atermpp::vector < data_expression_list > &non_real_conditions,
                const bool negate=false)
{ // std::cerr << "Split condition " << pp(e) << "\n";
  real_conditions.clear();
  non_real_conditions.clear();

  if ((!negate && is_and(e))  || (negate && is_or(e)))
  {
    atermpp::vector < data_expression_list > 
                 real_conditions_aux1, non_real_conditions_aux1;
    split_condition(lhs_(e),real_conditions_aux1,non_real_conditions_aux1,negate);
    atermpp::vector < data_expression_list > 
                 real_conditions_aux2, non_real_conditions_aux2;
    split_condition(rhs_(e),real_conditions_aux2,non_real_conditions_aux2,negate);
    for (atermpp::vector < data_expression_list >::const_iterator 
                       i1r=real_conditions_aux1.begin(), i1n=non_real_conditions_aux1.begin() ;
                       i1r!=real_conditions_aux1.end(); ++i1r, ++i1n) 
    { for (atermpp::vector < data_expression_list >::const_iterator 
                         i2r=real_conditions_aux2.begin(), i2n=non_real_conditions_aux2.begin() ;
                         i2r!=real_conditions_aux2.end(); ++i2r, ++i2n) 
      { real_conditions.push_back(*i1r + *i2r);
        non_real_conditions.push_back(*i1n + *i2n);
      }
    }
  }
  else if ((!negate && is_or(e))  || (negate && is_and(e)))
  {
    split_condition(lhs_(e),real_conditions,non_real_conditions,negate);
    atermpp::vector < data_expression_list > 
                 real_conditions_aux, non_real_conditions_aux;
    split_condition(rhs_(e),real_conditions_aux,non_real_conditions_aux,negate);
    for (atermpp::vector < data_expression_list >::const_iterator 
                       i_r=real_conditions_aux.begin(), i_n=non_real_conditions_aux.begin() ;
                       i_r!=real_conditions_aux.end(); ++i_r, ++i_n) 
    { real_conditions.push_back(*i_r);
      non_real_conditions.push_back(*i_n);
    }
  }
  else if (gsIsDataExprIf(e))
  { split_condition(or_(and_(condition_part(e),then_part(e)),
                        and_(not_(condition_part(e)),else_part(e))),
                        real_conditions,non_real_conditions,negate);
  }
  else if (is_not(e))
  { split_condition(lhs_(e),real_conditions,non_real_conditions,!negate);
  }
  else if(is_inequality(e) && (lhs_(e).sort() == sort_expr::real() || rhs_(e).sort() == sort_expr::real()))
  { std::set < data_variable > vars=find_all_data_variables(e);
    for(std::set < data_variable >::const_iterator i=vars.begin(); i!=vars.end(); ++i)
    { if (i->sort()!=sort_expr::real())
      { throw  mcrl2::runtime_error("Expression " + pp(e) + " contains variable " + 
                                         pp(*i) + " not of sort Real.");
      }
    }
    if (negate)
    { real_conditions.push_back(push_front(data_expression_list(),negate_inequality(e)));
      non_real_conditions.push_back(data_expression_list());
    }
    else 
    { real_conditions.push_back(push_front(data_expression_list(),e));
      non_real_conditions.push_back(data_expression_list());
    }
  }
  else
  { // e is assumed to be a non_real expression.
    std::set < data_variable > vars=find_all_data_variables(e);
    for(std::set < data_variable >::const_iterator i=vars.begin(); i!=vars.end(); ++i)
    { if (i->sort()==sort_expr::real())
      { throw  mcrl2::runtime_error("Expression " + pp(e) + " contains variable " +                                          pp(*i) + " of sort Real.");
      }
    }
    if (negate)
    { non_real_conditions.push_back(push_front(data_expression_list(),not_(e)));
      real_conditions.push_back(data_expression_list());
    }
    else 
    { non_real_conditions.push_back(push_front(data_expression_list(),e));
      real_conditions.push_back(data_expression_list());
    }
  }
  assert(non_real_conditions.size()==real_conditions.size());
}

/// \brief Normalize all inequalities in the summands of the specification
/// \details The parts of the
///          conditions ranging over real numbers and the other parts of the conditions
///          are separated and the part ranging over real numbers is normalized. Result is
///          stored in summand info.
///          
///          Fourier motzkin is applied to each summand to remove real variables in sums,
///          that do not occur in the next state. This simplifies conditions.
/// \param s A data specification
/// \param real_parameters The parameters of sort real of the process in the data specification
/// \param r A rewriter
/// \param summand_info Normalized summand information is stored conveniently in summand info.

static void normalize_specification(
                    const specification s, 
                    const data_variable_list real_parameters,
                    const rewriter& r, 
                    std::vector < summand_information > &summand_info)
{
  summand_list smds = s.process().summands();
  // summand_list sl;
  for(summand_list::const_iterator i = smds.begin(); i != smds.end(); ++i)
  {
    atermpp::vector <data_expression_list> real_conditions, non_real_conditions;
    // std::cerr << "Condition in: " << pp(i->condition()) << "\n";
    split_condition(i->condition(),real_conditions,non_real_conditions);

    for(atermpp::vector <data_expression_list>::const_iterator 
                   j_r=real_conditions.begin(), j_n=non_real_conditions.begin();
                   j_r!=real_conditions.end(); ++j_r, ++j_n)
    {
      summand t(*i);
      const data_expression c=r(join_and(j_n->begin(), j_n->end()));
      if (!is_false(c))
      { t=set_condition(t,c);

        vector < linear_inequality > inequalities;
        // Collect all real conditions from the condition from this summand and put them
        // into inequalities.
        for(data_expression_list::const_iterator k=j_r->begin(); k!=j_r->end(); k++)
        { inequalities.push_back(linear_inequality(*k,r));
        }

        // Determine all variables that occur in the sum operator, but not in the
        // next state. We can apply Fourier-Motzkin to eliminate these variables from
        // this sum operator and the condition.

        // std::cerr << "REALPARS " << pp(i->next_state(real_parameters)) << "\n";
        const std::set < data_variable> s1=find_all_data_variables(i->next_state(real_parameters));
        // for(std::set < data_variable>::const_iterator k=s1.begin(); k!=s1.end(); ++k)
        // { std::cerr << "VAR " << pp(*k) << "\n";
// 
// 
        // }

        const data_variable_list original_real_sum_variables=get_real_variables(i->summation_variables());
        data_variable_list real_sum_variables;
        data_variable_list eliminatable_real_sum_variables;
        for( data_variable_list::const_iterator k=original_real_sum_variables.begin();
                          k!=original_real_sum_variables.end(); ++k)
        { 

          // std::cerr << "Treat " << pp(*k) << "\n";
          if (s1.count(*k)==0)
          { // The variable does not occur in the parameters. We can eliminate it using Fourier-Motzkin
            eliminatable_real_sum_variables=push_front(eliminatable_real_sum_variables,*k);
          }
          else 
          { real_sum_variables=push_front(real_sum_variables,*k);
          }
        }

        vector < linear_inequality > new_inequalities;
        fourier_motzkin(inequalities,
                        eliminatable_real_sum_variables.begin(),
                        eliminatable_real_sum_variables.end(),
                        new_inequalities,
                        r);
        inequalities.clear();
        remove_redundant_inequalities(new_inequalities,inequalities,r);

        if ((inequalities.size()>0) && (inequalities.front().is_false()))
        { //  std::cerr << "INCONSISTENT \n";
        }
        else
        { 
          // Add for all real parameters x of the process an inequality 0<=x
          for(data_variable_list::const_iterator k=real_parameters.begin(); k!=real_parameters.end(); k++)
          { data_expression e=(atermpp::aterm_appl)*k;
            inequalities.push_back(linear_inequality(real_zero(),e,linear_inequality::less_eq,r));
          }

          // Add for all real sum variables x of this summand an inequality 0<=x
          for(data_variable_list::const_iterator k=real_sum_variables.begin(); k!=real_sum_variables.end(); k++)
          { const data_expression e=(atermpp::aterm_appl)*k;
            inequalities.push_back(linear_inequality(real_zero(),e,linear_inequality::less,r));
          }  

          // Construct replacements to contain the nextstate values for real variables in a map
          atermpp::map<data_expression, data_expression> replacements;
          for(data_assignment_list::const_iterator j = i->assignments().begin(); j != i->assignments().end(); ++j)
          {
            if(j->lhs().sort() == sort_expr::real())
            {
              replacements[j->lhs()] = j->rhs();
            }
          }
          const summand_information s(t,
                                      real_sum_variables,
                                      get_nonreal_variables(t.summation_variables()),
                                      inequalities,
                                      replacements);
          summand_info.push_back(s);
        }
      }
    }
  }
  // sl = reverse(sl);
  // lps = set_summands(lps, sl);

  // s = set_lps(s, lps);
  //return s;
}

/// \brief Determine the inequalities ranging over real numbers in a data expression.
/// \param e A data expression
/// \param inequalities A list of inequalities
/// \post inequalities contains all inequalities ranging over real numbers in e.
static
void determine_real_inequalities(
              const data_expression& e,
              vector < linear_inequality > &inequalities,
              const rewriter &r)
{
  // std::cerr << "Real inequalities in" << pp(e) << "\n";
  if (is_and(e))
  {
    determine_real_inequalities(lhs_(e), inequalities,r);
    determine_real_inequalities(rhs_(e), inequalities,r);
  }
  else if (is_inequality(e) && (rhs_(e).sort() == sort_expr::real()))
  {
    inequalities.push_back(linear_inequality(e,r));
  }
  // std::cerr << "Real inequalities out" << pp_vector(inequalities) << "\n";
  //else Do nothing, as it is not an expression on reals
}

/// \brief Add postponed inequalities to variable context
/// \param inequalities_to_add The pairs of inequalities to be added.
/// \param context A variable context
/// \param variable_generator A fresh variable generator
/// \post All inequalities in l are in the context
/// \ret true iff a variable has been added to the context
static void add_postponed_inequalities_to_context(
                const atermpp::vector < data_expression > &inequalities_to_add,
                std::vector < summand_information > &summand_info,
                context_type& context,
                const rewriter& r,
                identifier_generator& variable_generator)
{ assert(inequalities_to_add.size() % 2==0);
  for(atermpp::vector < data_expression > ::const_iterator i=inequalities_to_add.begin();
                        i!=inequalities_to_add.end(); i=i+2)
  {
    data_variable xi(variable_generator("xi"), sort_identifier("Comp"));
    context.push_back(real_representing_variable(xi,*i, *(i+1)));
    if (core::gsVerbose)
    { gsVerboseMsg("Introduced variable %s for < %s, %s >\n", pp(xi).c_str(), pp(*i).c_str(), pp(*(i+1)).c_str());
    }

    for(std::vector < summand_information >::iterator j = summand_info.begin();
                       j != summand_info.end(); ++j)
    { j->add_a_new_next_state_argument(context,r);
    }

  }
}

/// \brief Add inequalities to variable context
/// \param l A list of inequalities
/// \param context A variable context
/// \param variable_generator A fresh variable generator
/// \post All inequalities in l are in the context
/// \ret true iff a variable has been added to the context
static void add_inequalities_to_context_postponed(
                atermpp::vector < data_expression > &inequalities_to_add,
                const  vector < linear_inequality > l,
                context_type& context,
                const rewriter& r)
{ assert(inequalities_to_add.size() % 2==0);
  for(vector < linear_inequality > ::const_iterator i = l.begin(); i != l.end(); ++i)
  {
    data_expression left;
    data_expression right;
    i->typical_pair(left,right,r);

    if (left!=real_zero())
    { bool pair_is_new(true);
      for(context_type::const_iterator c=context.begin() ; c!=context.end() && pair_is_new; ++c)
      { if ((c->get_lowerbound()==left) && (c->get_upperbound()==right))
        { pair_is_new=false;
        }
      }
      if (pair_is_new)
      { for(atermpp::vector < data_expression >::const_iterator j=inequalities_to_add.begin() ;
                   j!=inequalities_to_add.end() && pair_is_new ; j=j+2)
        { if ((*j==left) && (*(j+1)==right))
          { pair_is_new=false;
          }
        }
        if(pair_is_new)
        {
          inequalities_to_add.push_back(left);
          inequalities_to_add.push_back(right);
        }
      }
    }
  }
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

  // gsDebugMsg("Removing variable %P from inequality %P\n", (ATermAppl)variable, (ATermAppl)inequality);

  data_expression left = lhs_(inequality);
  data_expression new_left = real_zero();
  while(is_plus(left))
  {
    // gsDebugMsg("left = %P is a plus expression\n", (ATermAppl)left);
    if(is_multiplies(rhs_(left)))
    {
      data_expression factor = lhs_(rhs_(left));
      new_left = gsMakeDataExprDivide(plus(new_left, lhs_(left)), factor);
      return data_application(static_cast<const data_application&>(inequality).head(), make_list(new_left, gsMakeDataExprDivide(rhs_(inequality), factor)));
    }
    else if (rhs_(left) == variable || rhs_(left) == negate(static_cast<const data_expression&>(variable)))
    {
      return data_application(static_cast<const data_application&>(inequality).head(), make_list(plus(new_left, lhs_(left)), rhs_(inequality)));
    }
    else
    {
      new_left = plus(new_left, rhs_(left));
      left = lhs_(left);
    }
  }

  // gsDebugMsg("left = %P\n", (ATermAppl)left);

  if(is_negate(left))
  {
    data_expression argument = *static_cast<const data_application&>(left).arguments().begin();
    if(is_plus(argument))
    {
      data_expression p = plus(negate(lhs_(argument)), negate(rhs_(argument)));
      return remove_variable(variable, data_application(static_cast<const data_application&>(inequality).head(), make_list(p, rhs_(inequality))));
    }
  }
  if (left == variable || left == negate(static_cast<const data_expression&>(variable)))
  {
    return data_application(static_cast<const data_application&>(inequality).head(), make_list(new_left, rhs_(inequality)));
  }

  gsErrorMsg("cannot remove variable %P from %P\n", (ATermAppl)variable, (ATermAppl)inequality);

  assert(false);
  return data_expression(); // Never reached, silence gcc 4.1.2
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

/// \brief Generate a summand
/// \param s A summand
/// \param i A number, denoting the next state
/// \param cond A list of inequalities denoting the real part of the condition
/// \param context A variable context
/// \param r A rewriter
/// \ret The summand corresponding to s with real part of condition cond, and
///      nextstate determined by i.
static
summand generate_summand(const summand_information &summand_info,
                         const data_expression &new_condition,
                         const data_expression_list &extra_zeta_values,
                         context_type& complete_context,
                         rewriter& r,
                         action_label_list &a,
                         identifier_generator& variable_generator,
                         const bool is_may_summand=false)
{ static atermpp::vector < sort_expression_list > protect_against_garbage_collect;
  static std::map < std::pair < std::string, sort_expression_list >, std::string> action_label_map;
                                         // Used to recall which may actions labels have been
                                         // introduced, in order to re-use them.
  const summand s=summand_info.get_summand();
  // std::pair<data_expression_list, data_expression_list> real_nonreal_condition = s.condition();
  // data_expression condition = and_(true_(), join_and(real_nonreal_condition.second.begin(), real_nonreal_condition.second.end()));

  data_assignment_list nextstate = get_nonreal_assignments(s.assignments());
  nextstate = reverse(nextstate);
  context_type::const_iterator c_complete = complete_context.begin();
  data_expression_list extra_zeta_values_reverse=reverse(extra_zeta_values);
  data_expression_list::iterator extra_zeta_value=extra_zeta_values_reverse.begin();

  for(atermpp::vector < data_expression >::const_iterator 
                j = summand_info.get_new_values_for_xi_variables_begin();
                j != summand_info.get_new_values_for_xi_variables_end(); ++j,++c_complete)
  {
    if ((*j)!=data_expression())
    { // We have a preset value for the j'th variable in the next state
      nextstate=push_front(nextstate,data_assignment(c_complete->get_variable(),*j));
    }
    else
    { // We have no preset value for the j'th variable in the next state. So, use the one from extra_zeta_value.
      nextstate=push_front(nextstate,data_assignment(c_complete->get_variable(),*extra_zeta_value));
      ++extra_zeta_value;
    }
  }
  assert(extra_zeta_value==extra_zeta_values_reverse.end());

  nextstate = reverse(nextstate);

  action_list new_actions=s.actions();
  if ((!s.is_delta()) && is_may_summand)
  { action_list resulting_actions;
    for(action_list::const_iterator i=new_actions.begin();
                 i!=new_actions.end(); i++)
    { // put "_MAY" behind each action, and add its declaration to the action declarations.
      data_expression_list args=i->arguments();
      sort_expression_list sorts=get_sorts(args);
      std::map < std::pair< std::string, sort_expression_list >,
                 std::string> ::iterator action_label_it=
                     action_label_map.find(std::pair< std::string, sort_expression_list >
                                         (std::string(i->label().name()),sorts));
      if (action_label_it==action_label_map.end())
      { std::string may_action_label=variable_generator(std::string(i->label().name())+"_MAY");
        std::pair< std::string, sort_expression_list > p(std::string(i->label().name()),sorts);
        action_label_it=(action_label_map.insert(
                    std::pair< std::pair< std::string, sort_expression_list >,std::string>
                      ( p,may_action_label))).first;
        a=push_front(a,action_label(may_action_label,sorts));
        protect_against_garbage_collect.push_back(sorts);
      }

      action_label may_action_label(action_label_it->second,sorts);
      resulting_actions=push_front(resulting_actions,action(may_action_label,args));
    }
    new_actions=reverse(resulting_actions);
  }

  summand result = summand(get_nonreal_variables(s.summation_variables()),
                           new_condition, s.is_delta(), new_actions, nextstate);

  // gsDebugMsg("Generated summand %P\n", (ATermAppl)result);

  return result;
}

/// \brief Compute process initialisation given a variable context and a process
///        initialisation containing real values.
/// \param initialization A process initialisation containing real values
/// \param context A variable context
/// \param r A rewriter
/// \ret A process initialisation in which all assignments to real variables
///      have been replaced with an initialization for each variable in context.
data_assignment_list determine_process_initialization(
                          const data_assignment_list& initialization,
                          context_type& context,
                          rewriter& r)
{
  data_assignment_list init = get_nonreal_assignments(initialization);
  data_assignment_list real_assignments = get_real_assignments(initialization);
  atermpp::map<data_expression, data_expression> replacements;
  for(data_assignment_list::const_iterator i = real_assignments.begin(); i != real_assignments.end(); ++i)
  {
    replacements[i->lhs()] = i->rhs();
  }

  for(context_type::const_iterator i = context.begin(); i != context.end(); ++i)
  {
    data_expression left = realelm_data_expression_map_replace(i->get_lowerbound(), replacements);
    data_expression right = realelm_data_expression_map_replace(i->get_upperbound(), replacements);
    data_assignment assignment;
    if(r(less(left, right)) == true_())
    {
      assignment = data_assignment(i->get_variable(), smaller());
    }
    else if(r(equal_to(left, right)) == true_())
    {
      assignment = data_assignment(i->get_variable(), equal());
    }
    else
    {
      assert(r(greater(left, right)) == true_());
      assignment = data_assignment(i->get_variable(), larger());
    }
    init = push_front(init, assignment);
  }
  return reverse(init);
}


/* static void generate_xi_conditions_rec(
              const context_type::const_reverse_iterator context_begin,
              const context_type::const_reverse_iterator context_end,
              vector < linear_inequality > &condition_list,
              const data_expression_list comp_value_list,
              vector < data_expression_list > &resulting_comp_values,
              const rewriter &r)
{ if (context_begin==context_end)
  { // TODO: filter the comp value list to only contain
    // those conditions on xi variables that match with
    // conditions in the condition list.

    // OLD CODE: resulting_conditions.push_back(condition_list);
    // std::cerr << "Generate " << pp(comp_value_list) << "   " << pp_vector(condition_list) << "\n";
    resulting_comp_values.push_back(comp_value_list);
    return;
  }

  unsigned int old_size=condition_list.size();
  condition_list.push_back(linear_inequality(
                                    context_begin->get_lowerbound(),
                                    context_begin->get_upperbound(),
                                    linear_inequality::equal,
                                    r));
  // std::cerr << "condition list " << pp_vector(condition_list) << "\n";

  vector < linear_inequality > new_condition_list;
  remove_redundant_inequalities(condition_list,new_condition_list,r);
  // std::cerr << "new condition list " << pp_vector(new_condition_list) << "\n";
  data_expression_list new_comp_value_list=
                                push_front(comp_value_list,
                                    data_expression(is_equal(context_begin->get_variable())));

  // if (!is_inconsistent(new_condition_list,r))
  if (new_condition_list.empty() || !new_condition_list.front().is_false())
  { generate_xi_conditions_rec(context_begin+1,
                                      context_end,
                                      new_condition_list,
                                      new_comp_value_list,
                                      resulting_comp_values,
                                      r);
  }
  condition_list[old_size]=linear_inequality(
                                    context_begin->get_lowerbound(),
                                    context_begin->get_upperbound(),
                                    linear_inequality::less,
                                    r);
  new_condition_list.clear();
  remove_redundant_inequalities(condition_list,new_condition_list,r);

  new_comp_value_list= push_front(comp_value_list,
                                 data_expression(is_smaller(context_begin->get_variable())));

  // if (!is_inconsistent(new_condition_list,r))
  if (new_condition_list.empty() || !new_condition_list.front().is_false())
  { generate_xi_conditions_rec(context_begin+1,
                                      context_end,
                                      new_condition_list,
                                      new_comp_value_list,
                                      resulting_comp_values,
                                      r);
  }

  condition_list[old_size]=linear_inequality(
                                    context_begin->get_upperbound(),
                                    context_begin->get_lowerbound(),
                                    linear_inequality::less,
                                    r);
  new_condition_list.clear();
  remove_redundant_inequalities(condition_list,new_condition_list,r);

  new_comp_value_list=push_front(comp_value_list,
                                 data_expression(is_larger(context_begin->get_variable())));

  // if (!is_inconsistent(new_condition_list,r))
  if (new_condition_list.empty() || !new_condition_list.front().is_false())
  { generate_xi_conditions_rec(context_begin+1,
                                      context_end,
                                      new_condition_list,
                                      new_comp_value_list,
                                      resulting_comp_values,
                                      r);
  }
  condition_list.pop_back();
} 

/// \brief generate all combinations of contexts that are not inconsistent with each other
///
/// \param context The context variables from which the combinations must be generated.
/// \param resulting_comp_values A vector with lists of values for the zeta/xi variables.
///                              Depending on the parameter value_no_condition each list
///                              contains values smaller, equal or greater corresponding to
///                              the corresponding list in the result, or it contains
///                              conditions is_smaller(xi), is_equal(xi) and is_greater(xi).
/// \ret A vector containing consistent lists of conditions corresponding to context.
static atermpp::vector < data_expression_list >
       generate_xi_conditions(
                   const context_type &context,
                   const vector < linear_inequality > &context_conditions,
                   const rewriter &r)
{ atermpp::vector < data_expression_list > resulting_comp_values;
  vector < linear_inequality > new_context_conditions;
  remove_redundant_inequalities(context_conditions,new_context_conditions,r);
  // std::cerr << "context conditions " << pp_vector(context_conditions) << std::endl;
  // std::cerr << "new context conditions " << pp_vector(new_context_conditions) << std::endl;
//  linear_inequality front = new_constext_conditions.front();
  // if (!is_inconsistent(new_context_conditions,r))
  if (new_context_conditions.empty() || !new_context_conditions.front().is_false())
  { generate_xi_conditions_rec(
                   context.rbegin(),
                   context.rend(),
                   new_context_conditions,
                   data_expression_list(),
                   resulting_comp_values,
                   r);
  }
  return resulting_comp_values;
} */

// Check whether variables in the first two arguments coincide with those in the last two
/* static bool are_data_variables_shared(
                 const data_expression d1,
                 const data_expression d2,
                 const vector < linear_inequality > &l)
{
  std::set < data_variable> s1=find_all_data_variables(d1);
  std::set < data_variable> s2=find_all_data_variables(d2);

  // Check whether the variables in d1 and d2 occur in e.
  atermpp::set < data_variable> s3;
  for(vector < linear_inequality >::const_iterator i=l.begin();
               i!=l.end(); ++i)
  { i->add_variables(s3);
  }

  for(std::set < data_variable>::iterator i=s3.begin();
              i!=s3.end(); i++)
  { if ((s1.count(*i)>0) || (s2.count(*i)>0))
    { // found
      return true;
    }
  }

  // So, the variables in d1 and d2 do not occur in e and l.
  return false;
} */

/// \brief Perform elimination of real variables on a specification in a maximum
///        number of iterations.
/// \param s A specification
/// \param max_iterations The maximal number of iterations the algorithm should
///        perform
/// \param strategy The rewrite strategy that should be used.
specification realelm(specification s, int max_iterations, RewriteStrategy strategy)
{
  if (s.process().has_time())
  { throw  mcrl2::runtime_error("Input specification contains actions with time tags. Use lpsuntime first.");
  }
  // First prepare the rewriter and normalize the specification.
  s = set_data_specification(s, add_comp_sort(s.data()));
  rewriter r = rewriter(s.data(), (rewriter::strategy)strategy);
  postfix_identifier_generator variable_generator("");
  variable_generator.add_to_context(s);
  linear_process lps=s.process();
  const data_variable_list real_parameters = get_real_variables(lps.process_parameters());
  const data_variable_list nonreal_parameters = get_nonreal_variables(lps.process_parameters());
  std::vector < summand_information > summand_info;
  normalize_specification(s, real_parameters, r, summand_info);

  context_type context; // Contains introduced variables

  atermpp::vector < data_expression > new_inequalities; // New inequalities are stored in two consecutive positions;
                                                        // I.e., for t<u, t is at position i, and u at position i+1.
  int iteration = 0;
  do
  {
    new_inequalities.clear();
    iteration++;
    gsVerboseMsg("Iteration %d, starting with %d context variables\n", iteration, context.size());

    for(std::vector < summand_information >::const_iterator i = summand_info.begin();
                       i != summand_info.end(); ++i)
    {
      // std::cerr << "SUMMAND_IN " << pp(i->get_summand()) << "\n" ;
                  
      // First calculate the newly introduced variables xi for which the next_state value is not yet known.
      // get , by only looking at variables that
      // occur in the condition or in the effect.
      context_type nextstate_context_for_this_summand;
      for(vector < vector < linear_inequality > >::const_iterator
                nextstate_combination = i->nextstate_context_combinations_begin();
                nextstate_combination != i->nextstate_context_combinations_end();
                        ++ nextstate_combination) // ,++ nextstate_value)
      {
        // zeta[x := g(x)]
        vector < linear_inequality > zeta_condition=*nextstate_combination;

        // original condition of the summand && zeta[x := g(x)]
        vector < linear_inequality >  condition = *nextstate_combination;
        // condition.insert(condition.end(),
        //                 i->get_summand_real_conditions_begin(),
        //                 i->get_summand_real_conditions_end());


        // Eliminate sum bound variables, resulting in inequalities over
        // process parameters of sort Real.

        vector < linear_inequality > condition1;
        data_variable_list sumvars= i->get_real_summation_variables(); 
 
        // std::cerr << "SUMVARS " << pp(sumvars) << "\n" ;
        // std::cerr << "CONDITION IN" << pp_vector(condition) << "\n" ;
        
        fourier_motzkin(condition, 
                        sumvars.begin(),
                        sumvars.end(),
                        condition1,
                        r);
        condition.clear();
        remove_redundant_inequalities(condition1,condition,r);
        // std::cerr << "CONDITION OUT" << pp_vector(condition) << "\n" ;

        // First check which of these inequalities are equivalent to concrete values of xi variables.
        // Add these values for xi variables as a new condition. Remove these variables from the
        // context combinations to be considered for the xi variables.


        if (condition.empty() || !condition.front().is_false())
        {
          // condition contains the inequalities over the process parameters
          add_inequalities_to_context_postponed(new_inequalities,condition, context, r);
        }
      }
    }
    add_postponed_inequalities_to_context(
                new_inequalities,
                summand_info,
                context,
                r,
                variable_generator);

  } while ((iteration < max_iterations) && !new_inequalities.empty());

  gsVerboseMsg("Generated the following variables in %d iterations:\n", iteration);
  for(context_type::iterator i = context.begin(); i != context.end(); ++i)
  { gsVerboseMsg("< %P, %P > %P\n", (ATermAppl)i->get_lowerbound(),
                   (ATermAppl)i->get_upperbound(), (ATermAppl)i->get_variable());
  }

  if (!new_inequalities.empty())
  { gsVerboseMsg("A may-bisimilar lps is being generated, which is most likely not strongly bisimilar.\n");
  }
  else
  { gsVerboseMsg("A strongly bisimilar lps is being generated.\n");
  }


  /* Generate the new summand list */
  // atermpp::vector < data_expression_list > nextstate_context_combinations;
  summand_list summands;
  action_label_list new_act_declarations;
  for(std::vector < summand_information >::const_iterator i = summand_info.begin();
                       i != summand_info.end(); ++i)
  {
    // std::cerr << "SUMMAND_IN " << pp(i->get_summand()) << "\n";
    
    // Construct the real time condition for summand in terms of xi variables.

    context_type nextstate_context_for_this_summand;
    atermpp::vector < data_expression_list >::const_iterator
                         nextstate_value=i->nextstate_value_combinations_begin();

    for(vector < vector < linear_inequality > >::const_iterator
              nextstate_combination = i->nextstate_context_combinations_begin();
              nextstate_combination != i->nextstate_context_combinations_end();
                      ++ nextstate_combination, ++nextstate_value)
    {
      // std::cerr << "Nextstate cond: " << pp_vector(*nextstate_combination) << "\n";

      data_expression new_condition=i->get_summand().condition();
      std::vector < linear_inequality > real_condition1;

      fourier_motzkin(*nextstate_combination,
                      i->get_real_summation_variables().begin(),
                      i->get_real_summation_variables().end(),
                      real_condition1,
                      r);
      std::vector < linear_inequality > real_condition2;
      remove_redundant_inequalities(real_condition1,real_condition2,r);
      
      bool all_conditions_found=true;
      // std::cerr << "Nextstate cond: " << pp_vector(real_condition2) << "\n";
      for(std::vector <linear_inequality>::const_iterator j=real_condition2.begin();
                 j!=real_condition2.end(); ++j)
      { 
        // std::cerr << "condition " << string(*j) << "\n";
        data_expression t;
        data_expression u;
        j->typical_pair(t,u,r);
        bool found=false;
        for(context_type::iterator k = context.begin(); (k != context.end()) && !found ; ++k)
        { 
          if ((t==k->get_lowerbound()) && (u==k->get_upperbound()))
          { found=true;
            if (j->comparison()==linear_inequality::equal)
            { new_condition=optimized::and_(new_condition,is_equal(k->get_variable()));
            }
            else if ((j->lhs_begin()!=j->lhs_end()) && (is_positive(j->lhs_begin()->second)))
            { // The inequality has *j has shape t<u or t<=u
              if (j->comparison()==linear_inequality::less)
              { new_condition=optimized::and_(new_condition,is_smaller(k->get_variable()));
              }
              else 
              { assert(j->comparison()==linear_inequality::less_eq);
                new_condition=optimized::and_(new_condition,not_(is_larger(k->get_variable())));
              }
            }
            else
            { // The inequality *j has shape t>=u or t>u 
              if (j->comparison()==linear_inequality::less)
              { new_condition=optimized::and_(new_condition,is_larger(k->get_variable()));
              }
              else 
              { assert(j->comparison()==linear_inequality::less_eq);
                new_condition=optimized::and_(new_condition,not_(is_smaller(k->get_variable())));
              }
            }
          }
        }
        // std::cerr << "FOUND " << pp(new_condition) << "\n";
        if (!found)
        { all_conditions_found=false;
        }
      }

      // original condition of the summand && zeta[x := g(x)]
      /* vector < linear_inequality >  condition = *nextstate_combination;
         condition.insert(condition.end(),
                         i->get_summand_real_conditions_begin(),
                         i->get_summand_real_conditions_end());

      // Eliminate sum bound variables, resulting in inequalities over
      // process parameters of sort Real.

      vector < linear_inequality > condition1;
      data_variable_list sumvars= i->get_summand().summation_variables();
      fourier_motzkin(condition,
                      sumvars.begin(),
                      sumvars.end(),
                      condition1,
                      r);
      condition.clear();
      remove_redundant_inequalities(condition1,condition,r); */

      // First check which of these inequalities are equivalent to concrete values of xi variables.
      // Add these values for xi variables as a new condition. Remove these variables from the
      // context combinations to be considered for the xi variables.

      /* if (condition.empty() || !condition.front().is_false()) // is consistent... */
      {
        /* context_type xi_context_for_this_summand;
        data_expression_list xi_condition;

        // Filter the xi_context_for_this_summand by removing variables for which lower and upperbound
        // do not share variables with the expressions in condition.
        context_type filtered_xi_context_for_this_summand;
        for(context_type::iterator c=context.begin();
                                     c!=context.end(); ++c)
        { if (are_data_variables_shared(c->get_lowerbound(),c->get_upperbound(),condition))
          { filtered_xi_context_for_this_summand.push_back(*c);
          }
        }

        // std::cerr << "condition " << pp_vector(condition) << "\n";
        atermpp::vector < data_expression_list > xi_context_conditions =
                    generate_xi_conditions(filtered_xi_context_for_this_summand,
                    condition,
                    r);

        // std::cerr << "Xi combinations: " << xi_context_conditions.size() << "\n";
        for(atermpp::vector < data_expression_list >::iterator
                  xi_context_condition = xi_context_conditions.begin();
                  xi_context_condition != xi_context_conditions.end(); 
                  ++xi_context_condition) // , ++xi_context_condition) */
        {
          /* // std::cerr << "Xi context condition " << pp(*xi_context_condition) << "\n";
          atermpp::vector < data_expression > new_inequalities; */


          if (!all_conditions_found)
          // if (!new_inequalities.empty())
          { // add a may transition.
            summand s = generate_summand(*i,
                                         new_condition,
                                         // and_(join_and(xi_condition.begin(), xi_condition.end()),
                                         //     join_and(xi_context_condition->begin(),
                                         //            xi_context_condition->end())),
                                         *nextstate_value,
                                         context,
                                         r,
                                         new_act_declarations,
                                         variable_generator,
                                         true);
            // std::cerr << "MAY SUMMAND_OUT: " << pp(s) << "\n";
            summands = push_front(summands, s);
          }
          else
          { // add a must transition.
            summand s = generate_summand(*i,
                                      new_condition,
                                      // and_(join_and(xi_condition.begin(), xi_condition.end()),
                                      //       join_and(xi_context_condition->begin(),
                                      //               xi_context_condition->end())),
                                       *nextstate_value,
                                       context,
                                       r,
                                       new_act_declarations,
                                       variable_generator,
                                       false);
            // std::cerr << "MUST SUMMAND_OUT: " << pp(s) << "\n";
            summands = push_front(summands, s);
          }
        }
      }
    }
  }
  summands = reverse(summands);

  // Process parameters
  data_variable_list process_parameters = reverse(nonreal_parameters);
  for(context_type::const_iterator i = context.begin(); i != context.end(); ++i)
  {
    process_parameters = push_front(process_parameters, i->get_variable());
  }
  process_parameters = reverse(process_parameters);

  // New lps
  lps = linear_process(lps.free_variables(), process_parameters, summands);
  // s = set_lps(s, lps);

  // New process initializer
  data_assignment_list initialization(determine_process_initialization(s.initial_process().assignments(), context, r));
  process_initializer init(s.initial_process().free_variables(), initialization);
  // s = set_initial_process(s, init);

  return specification(s.data(),
                       s.action_labels()+new_act_declarations,
                       lps,
                       init);

}

