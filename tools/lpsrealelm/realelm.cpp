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
#include "mcrl2/data/standard_utility.h"

#include "realelm.h"
#include "linear_inequalities.h"

using namespace atermpp;
using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::data;
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
  if (is_equal_to_application(e))
  { return not_equal_to(application(e).left(),application(e).right());
  }
  if (is_not_equal_to_application(e))
  { return equal_to(application(e).left(),application(e).right());
  }
  else if (is_less_application(e))
  { return greater_equal(application(e).left(),application(e).right());
  }
  else if (is_less_equal_application(e))
  { return greater(application(e).left(),application(e).right());
  }
  else if (is_greater_application(e))
  { return less_equal(application(e).left(),application(e).right());
  }
  else if (is_greater_equal_application(e))
  { return less(application(e).left(),application(e).right());
  }
  else
  { throw mcrl2::runtime_error("Expression " + pp(e) + " is expected to be an inequality over sort Real");
  }
}

/// \brief Split constant and variable parts of a data expression
/// \param e A data expression of the form c1 * x1 + ... + cn * xn + d1 + ... +
///          dm, where ci and di are constants and xi are variables. Constants
///          and variables may occur mixed.
/// \ret The pair (c1 * x1 + ... + cn * xn, d1 + ... + dm)
/* static
std::pair<data_expression, data_expression> split_variables_and_constants(const data_expression& e)
{
  // gsDebugMsg("Splitting constants and variables in %P\n", (ATermAppl)e);
  std::pair<data_expression, data_expression> result;
  if(sort_real::is_plus_application(e))
  {
    std::pair<data_expression, data_expression> left = split_variables_and_constants(application(e).left());
    std::pair<data_expression, data_expression> right = split_variables_and_constants(application(e).right());
    result = std::make_pair(sort_real::plus(left.first, right.first), sort_real::plus(left.second, right.second));
  }
  else if (sort_real::is_minus_application(e))
  {
    std::pair<data_expression, data_expression> left = split_variables_and_constants(application(e).left());
    std::pair<data_expression, data_expression> right = split_variables_and_constants(application(e).right());
    result = std::make_pair(sort_real::plus(left.first, sort_real::negate(right.first)), sort_real::plus(left.second, sort_real::negate(right.second)));
  }
  else if (sort_real::is_negate_application(e))
  {
    data_expression argument = *static_cast<const data_application&>(e).arguments().begin();
    if(sort_real::is_plus_application(argument))
    {
      result = split_variables_and_constants(sort_real::plus(sort_real::is_negate_application(application(argument).left), sort_real::is_negate_application(application(argument).right())));
    }
    else if(sort_real::is_minus_application(argument))
    {
      result = split_variables_and_constants(sort_real::plus(sort_real::is_negate_application(application(argument).left), application(argument).right()));
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
  else if (mcrl2::data::sort_real::is_creal(e) && !is_number(*static_cast<const data_application&>(e).arguments().begin()))
  {
    result = std::make_pair(e, real_zero());
  }
  else if (is_multiplies(e) || is_variable(e))
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
} */

/// \brief Returns a list of all real variables in l
/// \param l a list of data variables
/// \ret The list of all v in l such that v.sort() == real()
static inline
variable_list get_real_variables(const variable_list& l)
{
  variable_list r;
  for(variable_list::const_iterator i = l.begin(); i != l.end(); ++i)
  {
    if(i->sort() == sort_real::real_())
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
variable_list get_nonreal_variables(const variable_list& l)
{
  variable_list r;
  for(variable_list::const_iterator i = l.begin(); i != l.end(); ++i)
  {
    if(i->sort() != sort_real::real_())
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
    if(i->sort() == sort_real::real_())
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
    if(i->sort() != sort_real::real_())
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
assignment_list get_real_assignments(const assignment_list& l)
{
  assignment_list r;
  for(assignment_list::const_iterator i = l.begin(); i != l.end(); ++i)
  {
    if(i->lhs().sort() == sort_real::real_())
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
  return is_equal_to_application(e) || is_less_application(e) ||
         is_less_equal_application(e) || is_greater_application(e) ||
         is_greater_equal_application(e);
}



/// \brief Returns a list of all nonreal assignments in l
/// \param l a list of data assignments
/// \ret The list of all x := e in l such that x.sort() == e.sort() != real()
static inline
assignment_list get_nonreal_assignments(const assignment_list& l)
{
  assignment_list r;
  for(assignment_list::const_iterator i = l.begin(); i != l.end(); ++i)
  {
    if(i->lhs().sort() != sort_real::real_())
    {
      r = push_front(r, *i);
    }
  }
  return r;
}

// Functions below should have been defined in the data library.
static data_expression condition_part(const data_expression e)
{
  assert(is_if_application(e));
  return application(e).arguments()[0];
}

static data_expression then_part(const data_expression e)
{
  assert(is_if_application(e));
  return application(e).arguments()[1];
}

static data_expression else_part(const data_expression e)
{
  assert(is_if_application(e));
  return application(e).arguments()[2];
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

  if ((!negate && sort_bool::is_and_application(e))  || (negate && sort_bool::is_or_application(e)))
  {
    atermpp::vector < data_expression_list >
                 real_conditions_aux1, non_real_conditions_aux1;
    split_condition(application(e).left(),real_conditions_aux1,non_real_conditions_aux1,negate);
    atermpp::vector < data_expression_list >
                 real_conditions_aux2, non_real_conditions_aux2;
    split_condition(application(e).right(),real_conditions_aux2,non_real_conditions_aux2,negate);
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
  else if ((!negate && sort_bool::is_or_application(e))  || (negate && sort_bool::is_and_application(e)))
  {
    split_condition(application(e).left(),real_conditions,non_real_conditions,negate);
    atermpp::vector < data_expression_list >
                 real_conditions_aux, non_real_conditions_aux;
    split_condition(application(e).right(),real_conditions_aux,non_real_conditions_aux,negate);
    for (atermpp::vector < data_expression_list >::const_iterator
                       i_r=real_conditions_aux.begin(), i_n=non_real_conditions_aux.begin() ;
                       i_r!=real_conditions_aux.end(); ++i_r, ++i_n)
    { real_conditions.push_back(*i_r);
      non_real_conditions.push_back(*i_n);
    }
  }
  else if (is_if_application(e))
  { split_condition(sort_bool::or_(sort_bool::and_(condition_part(e),then_part(e)),
                        sort_bool::and_(sort_bool::not_(condition_part(e)),else_part(e))),
                        real_conditions,non_real_conditions,negate);
  }
  else if (sort_bool::is_not_application(e))
  { split_condition(application(e).arguments()[0],real_conditions,non_real_conditions,!negate);
  }
  else if(is_inequality(e) && (application(e).left().sort() == sort_real::real_() || application(e).right().sort() == sort_real::real_()))
  { std::set < variable > vars=find_all_variables(e);
    for(std::set < variable >::const_iterator i=vars.begin(); i!=vars.end(); ++i)
    { if (i->sort()!=sort_real::real_())
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
    std::set < variable > vars=find_all_variables(e);
    for(std::set < variable >::const_iterator i=vars.begin(); i!=vars.end(); ++i)
    { if (i->sort()==sort_real::real_())
      { throw  mcrl2::runtime_error("Expression " + pp(e) + " contains variable " +                                          pp(*i) + " of sort Real.");
      }
    }
    if (negate)
    { non_real_conditions.push_back(push_front(data_expression_list(),
                                               data_expression(sort_bool::not_(e))));
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
                    const variable_list real_parameters,
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
      const data_expression c=r(lazy::join_and(j_n->begin(), j_n->end()));
      if (!sort_bool::is_false_function_symbol(c))
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
        const std::set < variable> s1=find_all_variables(i->next_state(real_parameters));
        // for(std::set < variable>::const_iterator k=s1.begin(); k!=s1.end(); ++k)
        // { std::cerr << "VAR " << pp(*k) << "\n";
//
//
        // }

        const variable_list original_real_sum_variables=get_real_variables(i->summation_variables());
        variable_list real_sum_variables;
        variable_list eliminatable_real_sum_variables;
        for( variable_list::const_iterator k=original_real_sum_variables.begin();
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

        if ((inequalities.size()>0) && (inequalities.front().is_false(r)))
        { //  std::cerr << "INCONSISTENT \n";
        }
        else
        {
          // Add for all real parameters x of the process an inequality 0<=x
          for(variable_list::const_iterator k=real_parameters.begin(); k!=real_parameters.end(); k++)
          { data_expression e=(atermpp::aterm_appl)*k;
            inequalities.push_back(linear_inequality(real_zero(),e,linear_inequality::less_eq,r));
          }

          // Add for all real sum variables x of this summand an inequality 0<=x
          for(variable_list::const_iterator k=real_sum_variables.begin(); k!=real_sum_variables.end(); k++)
          { const data_expression e=(atermpp::aterm_appl)*k;
            inequalities.push_back(linear_inequality(real_zero(),e,linear_inequality::less,r));
          }

          // Construct replacements to contain the nextstate values for real variables in a map
          atermpp::map<data_expression, data_expression> replacements;
          for(assignment_list::const_iterator j = i->assignments().begin(); j != i->assignments().end(); ++j)
          {
            if(j->lhs().sort() == sort_real::real_())
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
  if (sort_bool::is_and_application(e))
  {
    determine_real_inequalities(application(e).left(), inequalities,r);
    determine_real_inequalities(application(e).right(), inequalities,r);
  }
  else if (is_inequality(e) && (application(e).right().sort() == sort_real::real_()))
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
                identifier_generator& variable_generator,
                const comp_struct &c
                )
{ assert(inequalities_to_add.size() % 2==0);
  for(atermpp::vector < data_expression > ::const_iterator i=inequalities_to_add.begin();
             i!=inequalities_to_add.end(); i++)
  { // std::cerr << "XXXX " << pp(*i) << "\n";
  }

  for(atermpp::vector < data_expression > ::const_iterator i=inequalities_to_add.begin();
                        i!=inequalities_to_add.end(); i=i+2)
  {
    variable xi(variable_generator("xi"), c.sort());
    context.push_back(real_representing_variable(xi,*i, *(i+1)));
    if (core::gsVerbose)
    { std::cerr << "Introduced variable " <<  pp(xi) << " for <" << pp(*i) << "," <<  pp(*(i+1)) << ">\n";
    }

    for(std::vector < summand_information >::iterator j = summand_info.begin();
                       j != summand_info.end(); ++j)
    { j->add_a_new_next_state_argument(context,r,c);
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
                std::vector < linear_inequality > &l,
                context_type& context,
                const rewriter& r)
{ assert(inequalities_to_add.size() % 2==0);
  // std::cerr << "Inequalities to add: " << pp_vector(inequalities_to_add) << "\n";
  for(vector < linear_inequality > ::iterator i = l.begin(); i != l.end(); )
  {
    data_expression left;
    data_expression right;
    i->typical_pair(left,right,r);

    if (left!=real_zero())
    { bool pair_is_new(true);
      for(context_type::const_iterator c=context.begin() ; c!=context.end() && pair_is_new; ++c)
      { if ((c->get_lowerbound()==left) && (c->get_upperbound()==right))
        { pair_is_new=false;
          ++i;
        }
      }
      if (pair_is_new)
      { for(atermpp::vector < data_expression >::const_iterator j=inequalities_to_add.begin() ;
                   j!=inequalities_to_add.end() && pair_is_new ; j=j+2)
        { if ((*j==left) && (*(j+1)==right))
          { pair_is_new=false;
            ++i;
          }
        }
        if (pair_is_new)
        { if (is_a_redundant_inequality(l,i,r))
          { i->swap(l.back());
            l.pop_back();
          }
          else
          { std::cerr << "Reserved to be added <" << pp(left) << "," << pp(right) << "\n";
            inequalities_to_add.push_back(left);
            inequalities_to_add.push_back(right);
            ++i;
          }
        }
      }
    }
    else
    { ++i;
    }
  }
}

/// \brief Remove a variable from an inequality
/// \param variable A variable
/// \param inequality An inequality over real numbers
/// \pre inequality is an inequality
/// \ret The inequality from which variable has been removed
/* static
data_expression remove_variable(const variable& variable, const data_expression& inequality)
{
  assert(is_inequality(inequality));

  // gsDebugMsg("Removing variable %P from inequality %P\n", (ATermAppl)variable, (ATermAppl)inequality);

  data_expression left = application(inequality).left;
  data_expression new_left = real_zero();
  while(sort_real::is_plus_application(left))
  {
    // gsDebugMsg("left = %P is a plus expression\n", (ATermAppl)left);
    if(is_multiplies(application(left).right()))
    {
      data_expression factor = application(application(left()).right).left();
      new_left = sort_real::divides(sort_real::plus(new_left, application(left).left()), factor);
      return data_application(static_cast<const data_application&>(inequality).head(), make_list(new_left, sort_real::divides(application(inequality).right(), factor)));
    }
    else if (application(left).right() == variable || application(left).right() == sort_real::is_negate_application(static_cast<const data_expression&>(variable)))
    {
      return data_application(static_cast<const data_application&>(inequality).head(), make_list(sort_real::plus(new_left, application(left).left()), application(inequality).right()));
    }
    else
    {
      new_left = sort_real::plus(new_left, application(left).right());
      left = application(left).left();
    }
  }

  // gsDebugMsg("left = %P\n", (ATermAppl)left);

  if(is_negate_application(left))
  {
    data_expression argument = *static_cast<const data_application&>(left).arguments().begin();
    if(sort_real::is_plus_application(argument))
    {
      data_expression p = sort_real::plus(sort_real::is_negate_application(application(argument).left), sort_real::is_negate_application(application(argument).right()));
      return remove_variable(variable, data_application(static_cast<const data_application&>(inequality).head(), make_list(p, application(inequality).right())));
    }
  }
  if (left == variable || left == sort_real::is_negate_application(static_cast<const data_expression&>(variable)))
  {
    return data_application(static_cast<const data_application&>(inequality).head(), make_list(new_left, application(inequality).right()));
  }

  gsErrorMsg("cannot remove variable %P from %P\n", (ATermAppl)variable, (ATermAppl)inequality);

  assert(false);
  return data_expression(); // Never reached, silence gcc 4.1.2
} */

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
                         const rewriter& r,
                         action_label_list &a,
                         identifier_generator& variable_generator,
                         const bool is_may_summand=false)
{ // std::cerr << "SUMMNAD " << pp(summand_info.get_summand()) << "\nCOND " << pp(new_condition) << "\n";
  static atermpp::vector < sort_expression_list > protect_against_garbage_collect;
  static std::map < std::pair < std::string, sort_expression_list >, std::string> action_label_map;
                                         // Used to recall which may actions labels have been
                                         // introduced, in order to re-use them.
  const summand s=summand_info.get_summand();
  // std::pair<data_expression_list, data_expression_list> real_nonreal_condition = s.condition();
  // data_expression condition = and_(true_(), join_and(real_nonreal_condition.second.begin(), real_nonreal_condition.second.end()));

  assignment_list nextstate = get_nonreal_assignments(s.assignments());
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
      nextstate=push_front(nextstate,assignment(c_complete->get_variable(),*j));
    }
    else
    { // We have no preset value for the j'th variable in the next state. So, use the one from extra_zeta_value.
      nextstate=push_front(nextstate,assignment(c_complete->get_variable(),*extra_zeta_value));
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
      sort_expression_list sorts=i->label().sorts(); // get_sorts(args);
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
assignment_list determine_process_initialization(
                          const assignment_list& initialization,
                          context_type& context,
                          const rewriter& r,
                          const comp_struct &c)
{
  assignment_list init = reverse(get_nonreal_assignments(initialization));
  assignment_list real_assignments = get_real_assignments(initialization);
  atermpp::map<data_expression, data_expression> replacements;
  for(assignment_list::const_iterator i = real_assignments.begin(); i != real_assignments.end(); ++i)
  {
    replacements[i->lhs()] = i->rhs();
  }

  for(context_type::const_iterator i = context.begin(); i != context.end(); ++i)
  {
    data_expression left = realelm_data_expression_map_replace(i->get_lowerbound(), replacements);
    data_expression right = realelm_data_expression_map_replace(i->get_upperbound(), replacements);
    assignment ass;
    if(r(less(left, right)) == sort_bool::true_())
    {
      ass = assignment(i->get_variable(), c.smaller());
    }
    else if(r(equal_to(left, right)) == sort_bool::true_())
    {
      ass = assignment(i->get_variable(), c.equal());
    }
    else
    {
      assert(r(greater(left, right)) == sort_bool::true_());
      ass = assignment(i->get_variable(), c.larger());
    }
    init = push_front(init, ass);
  }
  return reverse(init);
}


/// \brief Perform elimination of real variables on a specification in a maximum
///        number of iterations.
/// \param s A specification
/// \param max_iterations The maximal number of iterations the algorithm should
///        perform
/// \param strategy The rewrite strategy that should be used.
specification realelm(specification s, int max_iterations, const rewriter &r)
{
  if (s.process().has_time())
  { throw  mcrl2::runtime_error("Input specification contains actions with time tags. Use lpsuntime first.");
  }
  // First prepare the rewriter and normalize the specification.
  comp_struct c;
  data_specification ds=s.data();
  ds.add_sort(alias(c.sort(),c));
  s.data() = ds;
  postfix_identifier_generator variable_generator("");
  variable_generator.add_to_context(specification_to_aterm(s));
  linear_process lps=s.process();
  const variable_list real_parameters = get_real_variables(lps.process_parameters());
  const variable_list nonreal_parameters = get_nonreal_variables(lps.process_parameters());
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
      std::cerr << "SUMMAND_IN " << pp(i->get_summand()) << "\n" ;

      // First calculate the newly introduced variables xi for which the next_state value is not yet known.
      // get , by only looking at variables that
      // occur in the condition or in the effect.
      for(std::vector < std::vector < linear_inequality > >::const_iterator
                nextstate_combination = i->nextstate_context_combinations_begin();
                nextstate_combination != i->nextstate_context_combinations_end();
                        ++ nextstate_combination) // ,++ nextstate_value)
      {
        // zeta[x := g(x)]
        // vector < linear_inequality > zeta_condition=*nextstate_combination;

        // original condition of the summand && zeta[x := g(x)]
        // vector < linear_inequality >  condition = *nextstate_combination;
        // condition.insert(condition.end(),
        //                 i->get_summand_real_conditions_begin(),
        //                 i->get_summand_real_conditions_end());


        // Eliminate sum bound variables, resulting in inequalities over
        // process parameters of sort Real.

        std::vector < linear_inequality > condition1;
        variable_list sumvars= i->get_real_summation_variables();

        // std::cerr << "SUMVARS " << pp(sumvars) << "\n" ;
        // std::cerr << "CONDITION IN" << pp_vector(*nextstate_combination) << "\n" ;

        fourier_motzkin(*nextstate_combination,
                        sumvars.begin(),
                        sumvars.end(),
                        condition1,
                        r);
        // condition.clear();
        // Line below is the bottleneck in the second iteration for the railwaycrossing example.
        // vector < linear_inequality >  condition2;
        // remove_redundant_inequalities(condition1,condition2,r);
        // std::cerr << "CONDITION OUT" << pp_vector(condition) << "\n" ;

        // First check which of these inequalities are equivalent to concrete values of xi variables.
        // Add these values for xi variables as a new condition. Remove these variables from the
        // context combinations to be considered for the xi variables.


        // if (condition.empty() || !condition.front().is_false(r))
        if (!is_inconsistent(condition1,r))
        {
          // condition contains the inequalities over the process parameters
          add_inequalities_to_context_postponed(new_inequalities,condition1, context, r);
        }
      }
    }
    add_postponed_inequalities_to_context(
                new_inequalities,
                summand_info,
                context,
                r,
                variable_generator,
                c);

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
    std::cerr << "SUMMAND_IN__ " << pp(i->get_summand()) << "\n";

    // Construct the real time condition for summand in terms of xi variables.

    atermpp::vector < data_expression_list >::const_iterator
                         nextstate_value=i->nextstate_value_combinations_begin();

    for(std::vector < std::vector < linear_inequality > >::const_iterator
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
            { new_condition=lazy::and_(new_condition,c.is_equal(k->get_variable()));
            }
            else if ((j->lhs_begin()!=j->lhs_end()) && (is_positive(j->lhs_begin()->second,r)))
            { // The inequality has *j has shape t<u or t<=u
              if (j->comparison()==linear_inequality::less)
              { new_condition=lazy::and_(new_condition,c.is_smaller(k->get_variable()));
              }
              else
              { assert(j->comparison()==linear_inequality::less_eq);
                new_condition=lazy::and_(new_condition,sort_bool::not_(c.is_larger(k->get_variable())));
              }
            }
            else
            { // The inequality *j has shape t>=u or t>u
              if (j->comparison()==linear_inequality::less)
              { new_condition=lazy::and_(new_condition,c.is_larger(k->get_variable()));
              }
              else
              { assert(j->comparison()==linear_inequality::less_eq);
                new_condition=lazy::and_(new_condition,sort_bool::not_(c.is_smaller(k->get_variable())));
              }
            }
          }
        }
        if (!found)
        { all_conditions_found=false;
        }
      }

      if (!all_conditions_found)
      // if (!new_inequalities.empty())
      { // add a may transition.
        summand s = generate_summand(*i,
                                     new_condition,
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


  summands = reverse(summands);

  // Process parameters
  variable_list process_parameters = reverse(nonreal_parameters);
  for(context_type::const_iterator i = context.begin(); i != context.end(); ++i)
  {
    process_parameters = push_front(process_parameters, i->get_variable());
  }
  process_parameters = reverse(process_parameters);

  // New lps
  lps.process_parameters() = process_parameters;
  lps.set_summands(summands);

  // New process initializer
  assignment_list initialization(determine_process_initialization(s.initial_process().assignments(), context, r,c));
  process_initializer init(s.initial_process().free_variables(), initialization);

  return specification(s.data(),
                       s.action_labels()+new_act_declarations,
                       lps,
                       init);

}

