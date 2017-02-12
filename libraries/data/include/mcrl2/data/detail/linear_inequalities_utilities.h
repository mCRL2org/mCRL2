// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file linear_inequalities_utilities.h
/// \brief Contains utility functions for linear inequalities. 


#ifndef MCRL2_LPSREALELM_DETAIL_LINEAR_INEQUALITY_UTILITIES_H
#define MCRL2_LPSREALELM_DETAIL_LINEAR_INEQUALITY_UTILITIES_H


#include "mcrl2/data/linear_inequalities.h" 

namespace mcrl2
{

namespace data
{

namespace detail 
{

inline data_expression negate_inequality(const data_expression& e)
{
  if (is_equal_to_application(e))
  {
    return not_equal_to(data::binary_left(atermpp::down_cast<application>(e)),data::binary_right(atermpp::down_cast<application>(e)));
  }
  if (is_not_equal_to_application(e))
  {
    return equal_to(data::binary_left(atermpp::down_cast<application>(e)),data::binary_right(atermpp::down_cast<application>(e)));
  }
  else if (is_less_application(e))
  {
    return greater_equal(data::binary_left(atermpp::down_cast<application>(e)),data::binary_right(atermpp::down_cast<application>(e)));
  }
  else if (is_less_equal_application(e))
  {
    return data::greater(data::binary_left(atermpp::down_cast<application>(e)),data::binary_right(atermpp::down_cast<application>(e)));
  }
  else if (is_greater_application(e))
  {
    return less_equal(data::binary_left(atermpp::down_cast<application>(e)),data::binary_right(atermpp::down_cast<application>(e)));
  }
  else if (is_greater_equal_application(e))
  {
    return data::less(data::binary_left(atermpp::down_cast<application>(e)),data::binary_right(atermpp::down_cast<application>(e)));
  }
  else
  {
    throw mcrl2::runtime_error("Expression " + data::pp(e) + " is expected to be an inequality over sort Real");
  }
}


/// \brief Determine whether a data expression is an inequality
/// \param e A data expression
/// \return true iff e is a data application of ==, <, <=, > or >= to
///      two arguments.

inline bool is_inequality(const data_expression& e)
{
  return is_equal_to_application(e) || is_less_application(e) ||
         is_less_equal_application(e) || is_greater_application(e) ||
         is_greater_equal_application(e);
}

// Functions below should have been defined in the data library.
inline const data_expression& condition_part(const data_expression& e)
{
  assert(is_if_application(e));
  const data::application& a = atermpp::down_cast<const application>(e);
  data::application::const_iterator i = a.begin();
  return *i;
}

inline const data_expression& then_part(const data_expression& e)
{
  assert(is_if_application(e));
  const data::application& a = atermpp::down_cast<const application>(e);
  data::application::const_iterator i = a.begin();
  return *(++i);
}

inline const data_expression& else_part(const data_expression& e)
{
  assert(is_if_application(e));
  const data::application& a = atermpp::down_cast<const application>(e);
  data::application::const_iterator i = a.begin();
  return *(++(++i));
}



/// \brief Splits a condition in expressions ranging over reals and the others
/// \details Conceptually, the condition is first transformed to disjunctive
///          normal form. For each disjunct, there will be an entry in both
///          resulting std::vectors, where the real conditions are in "real_conditions",
///          and the others in non_real_conditions. If there are conjuncts with
///          both real and non-real variables an exception is thrown. If negate
///          is true the result will be negated.
/// \param e A data expression of sort bool.
/// \param real_condition Those parts of e with only variables over sort Real.
/// \param non_real_condition Those parts of e with only variables not of sort Real.
/// \param negate A boolean variable that indicates whether the result must be negated.
/// \pre The parameter e must be of sort Bool.

static void split_condition_aux(
  const data_expression& e,
  std::vector < data_expression_list >& real_conditions,
  std::vector < data_expression_list >& non_real_conditions,
  const bool negate=false)
{
  assert(real_conditions.empty());
  assert(non_real_conditions.empty());

  if ((!negate && sort_bool::is_and_application(e))  || (negate && sort_bool::is_or_application(e)))
  {
    std::vector < data_expression_list > real_conditions_aux1, non_real_conditions_aux1;
    split_condition_aux(data::binary_left(atermpp::down_cast<application>(e)),real_conditions_aux1,non_real_conditions_aux1,negate);
    std::vector < data_expression_list > real_conditions_aux2, non_real_conditions_aux2;
    split_condition_aux(data::binary_right(atermpp::down_cast<application>(e)),real_conditions_aux2,non_real_conditions_aux2,negate);

    for (std::vector < data_expression_list >::const_iterator
         i1r=real_conditions_aux1.begin(), i1n=non_real_conditions_aux1.begin() ;
         i1r!=real_conditions_aux1.end(); ++i1r, ++i1n)
    {
      for (std::vector < data_expression_list >::const_iterator
           i2r=real_conditions_aux2.begin(), i2n=non_real_conditions_aux2.begin() ;
           i2r!=real_conditions_aux2.end(); ++i2r, ++i2n)
      {
        real_conditions.push_back(*i1r + *i2r);
        non_real_conditions.push_back(*i1n + *i2n);
      }
    }
  }
  else if ((!negate && sort_bool::is_or_application(e))  || (negate && sort_bool::is_and_application(e)))
  {
    split_condition_aux(data::binary_left(atermpp::down_cast<application>(e)),real_conditions,non_real_conditions,negate);
    std::vector < data_expression_list >
    real_conditions_aux, non_real_conditions_aux;
    split_condition_aux(data::binary_right(atermpp::down_cast<application>(e)),real_conditions_aux,non_real_conditions_aux,negate);
    for (std::vector < data_expression_list >::const_iterator
         i_r=real_conditions_aux.begin(), i_n=non_real_conditions_aux.begin() ;
         i_r!=real_conditions_aux.end(); ++i_r, ++i_n)
    {
      real_conditions.push_back(*i_r);
      non_real_conditions.push_back(*i_n);
    }
  }
  else if (is_if_application(e))
  {
    split_condition_aux(sort_bool::or_(sort_bool::and_(condition_part(e),then_part(e)),
                                   sort_bool::and_(sort_bool::not_(condition_part(e)),else_part(e))),
                    real_conditions,non_real_conditions,negate);
  }
  else if (sort_bool::is_not_application(e))
  {
    split_condition_aux(unary_operand(atermpp::down_cast<application>(e)),real_conditions,non_real_conditions,!negate);
  }
  else if (is_inequality(e) && (data::binary_left(atermpp::down_cast<application>(e)).sort() == sort_real::real_() || data::binary_right(atermpp::down_cast<application>(e)).sort() == sort_real::real_()))
  {
    std::set < variable > vars=data::find_all_variables(e);
    for (std::set < variable >::const_iterator i=vars.begin(); i!=vars.end(); ++i)
    {
      if (i->sort()!=sort_real::real_())
      {
        throw  mcrl2::runtime_error("Expression " + data::pp(e) + " contains variable " +
                                    data::pp(*i) + " not of sort Real.");
      }
    }
    if (negate)
    {
      real_conditions.push_back(data_expression_list({ negate_inequality(e) }));
      non_real_conditions.push_back(data_expression_list());
    }
    else
    {
      real_conditions.push_back(data_expression_list({ e }));
      non_real_conditions.push_back(data_expression_list());
    }
  }
  else
  {
    // e is assumed to be a non_real expression.
    std::set < variable > vars=data::find_all_variables(e);
    for (std::set < variable >::const_iterator i=vars.begin(); i!=vars.end(); ++i)
    {
      if (i->sort()==sort_real::real_())
      {
        throw  mcrl2::runtime_error("Expression " + data::pp(e) + " contains variable " +
                                    data::pp(*i) + " of sort Real.");
      }
    }
    if (negate)
    {
      non_real_conditions.push_back(data_expression_list({ data_expression(sort_bool::not_(e)) }));
      real_conditions.push_back(data_expression_list());
    }
    else
    {
      non_real_conditions.push_back(data_expression_list({ e }));
      real_conditions.push_back(data_expression_list());
    }
  }
}

/// \brief This function first splits the given condition e into real conditions and
///        non real conditions. 
/// \detail This function first uses split_condition_aux to split the condition e. Then
//          it merges equal real conditions by merging the non-real conditions. No further
//          calculations take place with the non-real conditions, but if the non-real conditions
//          lead to unnecessary copying, this may lead to a huge overhead in removing the 
//          real conditions.
inline void split_condition(
  const data_expression& e,
  std::vector < data_expression_list >& real_conditions,
  std::vector < data_expression >& non_real_conditions)
{
  std::vector < data_expression_list > aux_real_conditions;
  std::vector < data_expression_list > aux_non_real_conditions;

  split_condition_aux(e,aux_real_conditions, aux_non_real_conditions);
  assert(aux_non_real_conditions.size()==aux_real_conditions.size() && aux_non_real_conditions.size()>0);
  

  for(std::vector < data_expression_list >::const_iterator i=aux_real_conditions.begin(), j=aux_non_real_conditions.begin();
              i!=aux_real_conditions.end(); ++i, ++j)
  {
    bool found=false;
    std::vector < data_expression >::iterator j_search=non_real_conditions.begin();
    for(std::vector < data_expression_list >::const_iterator i_search=real_conditions.begin();
           i_search!=real_conditions.end(); ++i_search, ++j_search)
    {
      assert(j_search!=non_real_conditions.end());
      if (*i==*i_search)
      {
        *j_search=lazy::or_(*j_search,lazy::join_and(j->begin(), j->end()));
        found=true;
        break;
      }
    }
    if (!found)
    {
      real_conditions.push_back(*i);
      non_real_conditions.push_back(lazy::join_and(j->begin(), j->end()));
    }
  }
  assert(non_real_conditions.size()==real_conditions.size() && non_real_conditions.size()>0);
}

} // end namespace detail


} // namespace data

} // namespace mcrl2

#endif // MCRL2_LPSREALELM_DETAIL_LINEAR_INEQUALITY_UTILITIES_H
