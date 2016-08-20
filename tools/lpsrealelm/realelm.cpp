// Author(s): Jan Friso Groote
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
#include <cstdlib>

#include "mcrl2/data/find.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/enumerator.h"

#include "mcrl2/lps/find.h"
#include "mcrl2/lps/print.h"

#include "realelm.h"

namespace mcrl2
{
namespace data
{

using namespace atermpp;
using namespace mcrl2::core;
using namespace mcrl2::lps;
using namespace mcrl2::log;
using namespace mcrl2::utilities;



static data_expression negate_inequality(const data_expression& e)
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

/// \brief Returns a list of all real variables in l
/// \param l a list of data variables
/// \return The list of all v in l such that v.sort() == real()
static inline
variable_list get_real_variables(const variable_list& l)
{
  variable_list r;
  for (variable_list::const_iterator i = l.begin(); i != l.end(); ++i)
  {
    if (i->sort() == sort_real::real_())
    {
      r.push_front(*i);
    }
  }
  return r;
}

/// \brief Returns a list of all nonreal variables in l
/// \param l a list of data variables
/// \return The list of all v in l such that v.sort() != real()
static inline
variable_list get_nonreal_variables(const variable_list& l)
{
  variable_list r;
  for (variable_list::const_iterator i = l.begin(); i != l.end(); ++i)
  {
    if (i->sort() != sort_real::real_())
    {
      r.push_front(*i);
    }
  }
  return r;
}

/* static data::function_symbol& negate_function_symbol(const sort_expression& s)
{
  static data::function_symbol f = data::function_symbol("negate",data::make_function_sort(s,s));
  assert(data::make_function_sort(s,s)==f.sort()); // Protect against using f for other sorts than sort comp.
  return f;
}*/ 

/// \brief Returns a list of all real assignments in l
/// \param l a list of data assignments
/// \return The list of all x := e in l such that x.sort() == e.sort() == real()
static inline
assignment_list get_real_assignments(const assignment_list& l)
{
  assignment_list r;
  for (assignment_list::const_iterator i = l.begin(); i != l.end(); ++i)
  {
    if (i->lhs().sort() == sort_real::real_())
    {
      r.push_front(*i);
    }
  }
  return r;
}

/// \brief Determine whether a data expression is an inequality
/// \param e A data expression
/// \return true iff e is a data application of ==, <, <=, > or >= to
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
/// \return The list of all x := e in l such that x.sort() == e.sort() != real()
static inline
assignment_list get_nonreal_assignments(const assignment_list& l)
{
  assignment_list r;
  for (assignment_list::const_iterator i = l.begin(); i != l.end(); ++i)
  {
    if (i->lhs().sort() != sort_real::real_())
    {
      r.push_front(*i);
    }
  }
  return r;
}

// Functions below should have been defined in the data library.
static const data_expression& condition_part(const data_expression& e)
{
  assert(is_if_application(e));
  const data::application& a = down_cast<const application>(e);
  data::application::const_iterator i = a.begin();
  return *i;
}

static const data_expression& then_part(const data_expression& e)
{
  assert(is_if_application(e));
  const data::application& a = down_cast<const application>(e);
  data::application::const_iterator i = a.begin();
  return *(++i);
}

static const data_expression& else_part(const data_expression& e)
{
  assert(is_if_application(e));
  const data::application& a = down_cast<const application>(e);
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
      real_conditions.push_back({ negate_inequality(e) });
      non_real_conditions.push_back(data_expression_list());
    }
    else
    {
      real_conditions.push_back({ e });
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
      non_real_conditions.push_back({ data_expression(sort_bool::not_(e)) });
      real_conditions.push_back(data_expression_list());
    }
    else
    {
      non_real_conditions.push_back({ e });
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
static void split_condition(
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

static size_t global_variable_counter=0;

/// \brief Find each expression of the form x<y, x<=y, x==y, x>=y x>y in t that contain variables occurring in real_parameters
///        and replace these by a boolean variable b. This variable is added to vars
static data_expression replace_linear_inequalities_with_reals_by_variables(
                  const data_expression& t,
                  data_expression& condition,
                  variable_list& vars,
                  const variable_list& real_parameters)
{
  if (is_function_symbol(t))
  {
    return t;
  }
  if (is_variable(t))
  {
    const variable v(t);
    if (std::find(real_parameters.begin(),real_parameters.end(),v)!=real_parameters.end()) // found
    {
      throw mcrl2::runtime_error(std::string("Variable ") + data::pp(v) + ":" + data::pp(v.sort()) + " occurs in an action and cannot be removed");
    }
    return t;
  }
  if (is_abstraction(t))
  {
    const abstraction ta(t);
    return abstraction(ta.binding_operator(),
                       ta.variables(),
                       replace_linear_inequalities_with_reals_by_variables(ta.body(),condition,vars,real_parameters));
  }
  if (is_where_clause(t))
  {
    const where_clause tw(t);
    const assignment_expression_list& l=tw.declarations();
    assignment_expression_vector new_l;
    for(assignment_expression_list::const_iterator i=l.begin(); i!=l.end(); ++i)
    {
      const assignment ass(*i);
      new_l.push_back(assignment(ass.lhs(),replace_linear_inequalities_with_reals_by_variables(ass.rhs(),condition,vars,real_parameters)));
    }

    return where_clause(replace_linear_inequalities_with_reals_by_variables(tw.body(),condition,vars,real_parameters),
                        assignment_expression_list(new_l.begin(),new_l.end()));
  }

  assert(is_application(t));
  const application ta(t);
  if (is_inequality(ta))
  {
    std::stringstream ss;
    ss << "v@@r" << global_variable_counter;
    variable v(ss.str(),sort_bool::bool_());
    global_variable_counter++;
    condition=sort_bool::and_(condition,equal_to(v,ta));
    vars.push_front(v);
    return v;
  }

  data_expression_vector new_args;
  for(application::const_iterator a=ta.begin(); a!=ta.end(); ++a)
  {
    new_args.push_back(replace_linear_inequalities_with_reals_by_variables(*a,condition,vars,real_parameters));
  }
  return application(replace_linear_inequalities_with_reals_by_variables(ta.head(),condition,vars,real_parameters),
                     new_args.begin(),new_args.end());

}

/// \brief Remove references to variables in real_parameters from actions,
///        if possible. In particular actions of the shape a(x<3).....
///        are replaced by summands of the shape x<3 -> a(true) .... + !(x<3) -> a(false) ....
/// \param s The specification s is changed in the sense that actions are removed.
/// \param real_parameters are used to determine what the real parameters are.
/// \detail This routine throws an exception if there is a real parameter in an
///         action that it fails to remove.

static void move_real_parameters_out_of_actions(stochastic_specification& s,
                                                const variable_list& real_parameters,
                                                const rewriter& r)
{
  global_variable_counter=0;
  const lps::stochastic_action_summand_vector action_smds = s.process().action_summands();
  lps::stochastic_action_summand_vector new_action_summands;
  enumerator_algorithm_with_iterator<> enumerator(r,s.data(),r);
  for (const lps::stochastic_action_summand& i: action_smds)
  {
     const process::action_list ma=i.multi_action().actions();
     variable_list replaced_variables;
     data_expression new_condition=sort_bool::true_();
     process::action_vector new_actions;
     for(process::action_list::const_iterator a=ma.begin(); a!=ma.end(); ++a)
     {
       const data_expression_list l=a->arguments();
       data_expression_vector resulting_data;
       for(data_expression_list::const_iterator j=l.begin(); j!=l.end(); ++j)
       {
         resulting_data.push_back(replace_linear_inequalities_with_reals_by_variables(*j,new_condition,replaced_variables,real_parameters));
       }
       new_actions.push_back(process::action(a->label(),data_expression_list(resulting_data.begin(),resulting_data.end())));
     }

     if (replaced_variables.empty())
     {
       new_action_summands.push_back(i);
     }
     else
     {
       mutable_indexed_substitution<> empty_sigma;
       std::deque<enumerator_list_element_with_substitution<> >
               enumerator_deque(1, enumerator_list_element_with_substitution<>(replaced_variables,sort_bool::true_()));
       for (enumerator_algorithm_with_iterator<>::iterator tl = enumerator.begin(empty_sigma, enumerator_deque); tl!= enumerator.end(); ++tl)
       {
         mutable_map_substitution<> sigma;
         tl->add_assignments(replaced_variables,sigma,r);

         process::action_vector new_replaced_actions;
         for(process::action_vector::const_iterator j=new_actions.begin(); j!=new_actions.end(); ++j)
         {
           data_expression_vector new_replaced_args;
           for(data_expression_list::const_iterator k=j->arguments().begin();k!=j->arguments().end(); ++k)
           {
             new_replaced_args.push_back(replace_free_variables(*k,sigma));
           }
           new_replaced_actions.push_back(process::action(j->label(),data_expression_list(new_replaced_args.begin(),new_replaced_args.end())));
         }
         const process::action_list new_action_list(new_replaced_actions.begin(),new_replaced_actions.end());
         new_action_summands.push_back(action_summand(
                                          i.summation_variables(),
                                          r(sort_bool::and_(data::replace_free_variables(new_condition,sigma),i.condition())),
                                          (i.has_time()?
                                             multi_action(new_action_list,i.multi_action().time()):
                                             multi_action(new_action_list)),
                                          i.assignments()));
       }
     }
  }
  s.process().action_summands()=new_action_summands;
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
  const stochastic_specification& s,
  const variable_list& real_parameters,
  const rewriter& r,
  std::vector < summand_information >& summand_info)
{
  const lps::stochastic_action_summand_vector action_smds = s.process().action_summands();
  for (const stochastic_action_summand& i: action_smds)
  {
    std::vector <data_expression_list> real_conditions; 
    std::vector <data_expression> non_real_conditions;
    split_condition(i.condition(),real_conditions,non_real_conditions);

    std::vector <data_expression>::const_iterator j_n=non_real_conditions.begin();
    for (std::vector <data_expression_list>::const_iterator j_r=real_conditions.begin();
         j_r!=real_conditions.end(); ++j_r, ++j_n)
    {
      const data_expression non_real_condition=*j_n;
      if (!sort_bool::is_false_function_symbol(non_real_condition))
      {
        std::vector < linear_inequality > inequalities;
        // Collect all real conditions from the condition from this summand and put them
        // into inequalities.
        for (data_expression_list::const_iterator k=j_r->begin(); k!=j_r->end(); k++)
        {
          inequalities.push_back(linear_inequality(*k,r));
        }

        // Determine all variables that occur in the sum operator, but not in the
        // next state. We can apply Fourier-Motzkin to eliminate these variables from
        // this sum operator and the condition.

        const std::set < variable> s1=data::find_all_variables(i.next_state(real_parameters));

        const variable_list original_real_sum_variables=get_real_variables(i.summation_variables());
        variable_list real_sum_variables;
        variable_list eliminatable_real_sum_variables;
        for (const variable& k: original_real_sum_variables)
        {
          if (s1.count(k)==0)
          {
            // The variable does not occur in the parameters. We can eliminate it using Fourier-Motzkin
            eliminatable_real_sum_variables.push_front(k);
          }
          else
          {
            real_sum_variables.push_front(k);
          }
        }

        std::vector < linear_inequality > new_inequalities;
        fourier_motzkin(inequalities,
                        eliminatable_real_sum_variables.begin(),
                        eliminatable_real_sum_variables.end(),
                        new_inequalities,
                        r);
        inequalities.clear();
        remove_redundant_inequalities(new_inequalities,inequalities,r);

        if (!((inequalities.size()>0) && (inequalities.front().is_false(r))))
        {
          // Add for all real parameters x of the process an inequality 0<=x
          // Also this is extremely confusing, and should not be done by lpsrealem.
          // for (variable_list::const_iterator k=real_parameters.begin(); k!=real_parameters.end(); k++)
          // {
          //   data_expression e=(atermpp::aterm_appl)*k;
          //   inequalities.push_back(linear_inequality(real_zero(),e,linear_inequality::less_eq,r));
          // }

          // Add for all real sum variables x of this summand an inequality 0<=x. CODE BELOW ADDS 0<x.
          // And it is not always reqruired, if this is not explicitly indicated. Certainly does not belong here.
          // If somebody wants to add this as a constraint, he should do this explicitly in the input code.
          /* for (variable_list::const_iterator k=real_sum_variables.begin(); k!=real_sum_variables.end(); k++)
          {
            const data_expression e=(atermpp::aterm_appl)*k;
            inequalities.push_back(linear_inequality(real_zero(),e,linear_inequality::less,r));
          } */

          // First check whether a similar summand with the same action, sum variables, and assignment already 
          // exists. 
          // exists. If so, merge the two.
          
          bool found=false;
          for(summand_information& s: summand_info)
          {
            if (s.get_summand().summation_variables()==i.summation_variables() &&
                s.get_multi_action()==i.multi_action() &&
                s.get_assignments()==i.assignments() &&
                s.get_distribution()==i.distribution() &&
                s.get_summand_real_conditions()==inequalities)
            { // A similar summand has been found. Extend the condition. 
              // Adding the condition could be optimised. Generally it is equal to existing summands.
              s.get_summand().condition()=sort_bool::or_(s.get_summand().condition(),non_real_condition);
              found=true;
            }               
          }

          if (!found)
          {
            // Construct replacements to contain the nextstate values for real variables in a map
            std::map<variable, data_expression> replacements;
            for (assignment_list::const_iterator j = i.assignments().begin(); j != i.assignments().end(); ++j)
            {
              if (j->lhs().sort() == sort_real::real_())
              {
                replacements[j->lhs()] = j->rhs();
              }
            }
            const summand_base t(i.summation_variables(),non_real_condition);
            const summand_information s(t,
                                        false, // This is not a delta summand.
                                        i.assignments(),
                                        i.distribution(),
                                        i.multi_action(),
                                        lps::deadlock(),  // default deadlock summand.
                                        real_sum_variables,
                                        get_nonreal_variables(t.summation_variables()),
                                        inequalities,
                                        replacements);
            summand_info.push_back(s);
          }
        }
      }
    }
  } // Finished dealing with action summands.



  const lps::deadlock_summand_vector& deadlock_smds = s.process().deadlock_summands();
  for (lps::deadlock_summand_vector::const_iterator i = deadlock_smds.begin(); i != deadlock_smds.end(); ++i)
  {
    std::vector <data_expression_list> real_conditions;
    std::vector <data_expression> non_real_conditions;
    split_condition(i->condition(),real_conditions,non_real_conditions);

    std::vector <data_expression>::const_iterator j_n=non_real_conditions.begin();
    for (std::vector <data_expression_list>::const_iterator
                 j_r=real_conditions.begin();
         j_r!=real_conditions.end(); ++j_r, ++j_n)
    {
      const data_expression c=*j_n;
      if (!sort_bool::is_false_function_symbol(c))
      {
        const summand_base t(i->summation_variables(),c);

        std::vector < linear_inequality > inequalities;
        // Collect all real conditions from the condition from this summand and put them
        // into inequalities.
        for (data_expression_list::const_iterator k=j_r->begin(); k!=j_r->end(); k++)
        {
          inequalities.push_back(linear_inequality(*k,r));
        }

        // We can apply Fourier-Motzkin to eliminate the real variables from
        // this sum operator and the condition.


        const variable_list eliminatable_real_sum_variables=get_real_variables(i->summation_variables());

        std::vector < linear_inequality > new_inequalities;
        fourier_motzkin(inequalities,
                        eliminatable_real_sum_variables.begin(),
                        eliminatable_real_sum_variables.end(),
                        new_inequalities,
                        r);
        inequalities.clear();
        remove_redundant_inequalities(new_inequalities,inequalities,r);

        if (!((inequalities.size()>0) && (inequalities.front().is_false(r))))
        {
          // Add for all real parameters x of the process an inequality 0<=x
          // Also this is extremely confusing, and should not be done by lpsrealem.
          // for (variable_list::const_iterator k=real_parameters.begin(); k!=real_parameters.end(); k++)
          // {
          //   data_expression e=(atermpp::aterm_appl)*k;
          //   inequalities.push_back(linear_inequality(real_zero(),e,linear_inequality::less_eq,r));
          // }

          // Add for all real sum variables x of this summand an inequality 0<=x. CODE BELOW ADDS 0<x.
          // And it is not always reqruired, if this is not explicitly indicated. Certainly does not belong here.
          // If somebody wants to add this as a constraint, he should do this explicitly in the input code.
          /* for (variable_list::const_iterator k=real_sum_variables.begin(); k!=real_sum_variables.end(); k++)
          {
            const data_expression e=(atermpp::aterm_appl)*k;
            inequalities.push_back(linear_inequality(real_zero(),e,linear_inequality::less,r));
          } */

          // Construct replacements to contain the nextstate values for real variables in a map

          const summand_information s(t,
                                      true, // This is a deadlock summand.
                                      assignment_list(),
                                      lps::stochastic_distribution(),
                                      lps::multi_action(),
                                      i->deadlock(),
                                      variable_list(), // All sum variables over reals have been eliminated.
                                      get_nonreal_variables(t.summation_variables()),
                                      inequalities,
                                      std::map<variable, data_expression>());
          summand_info.push_back(s);
        }
      }
    }
  } // Finished dealing with delta summands

}

/// \brief Add postponed inequalities to variable context
/// \param inequalities_to_add The pairs of inequalities to be added.
/// \param context A variable context
/// \param variable_generator A fresh variable generator
/// \post All inequalities in l are in the context
/// \return true iff a variable has been added to the context
static void add_postponed_inequalities_to_context(
  const std::vector < size_t >& inequalities_to_add_lhs_size,
  const std::vector < data_expression >& inequalities_to_add_lhs,
  const std::vector < data_expression >& inequalities_to_add_rhs,
  const std::vector < detail::comparison_t >& inequalities_to_add_comparison_operator,
  std::vector < summand_information >& summand_info,
  context_type& context,
  const rewriter& r,
  identifier_generator<>& variable_generator)
{
  assert(inequalities_to_add_lhs.size() ==inequalities_to_add_lhs_size.size() &&
         inequalities_to_add_lhs.size() ==inequalities_to_add_rhs.size() &&
         inequalities_to_add_lhs.size() ==inequalities_to_add_comparison_operator.size());

  // We add new next state arguments with increasing sizes of their lhs's.
  std::set <size_t> sorted_lhs_sizes(inequalities_to_add_lhs_size.begin(),inequalities_to_add_lhs_size.end());

  for (std::set <size_t>::const_iterator current_size=sorted_lhs_sizes.begin();
       current_size!=sorted_lhs_sizes.end(); ++current_size)
  {
    for (size_t i=0; i<inequalities_to_add_lhs.size(); ++i)
    {
      if (inequalities_to_add_lhs_size[i]== *current_size)
      {
        variable xi(variable_generator("xi"), sort_bool::bool_());
        context.emplace_back(xi,inequalities_to_add_lhs[i], inequalities_to_add_rhs[i], inequalities_to_add_comparison_operator[i]);
        mCRL2log(verbose) << "Introduced variable " <<  data::pp(xi) << " for <" << data::pp(inequalities_to_add_lhs[i]) <<
                    " "  << pp(inequalities_to_add_comparison_operator[i]) << " " <<  data::pp(inequalities_to_add_rhs[i]) << ">\n";

        for (std::vector < summand_information >::iterator j = summand_info.begin();
             j != summand_info.end(); ++j)
        {
          j->add_a_new_next_state_argument(context,r);
        }
      }
    }
  }
}

/// \brief Add inequalities to variable context
/// \param l A list of inequalities
/// \param context A variable context
/// \param variable_generator A fresh variable generator
/// \post All inequalities in l are in the context
/// \return true iff a variable has been added to the context
static void add_inequalities_to_context_postponed(
  std::vector < size_t >& inequalities_to_add_lhs_size,
  std::vector < data_expression >& inequalities_to_add_lhs,
  std::vector < data_expression >& inequalities_to_add_rhs,
  std::vector < detail::comparison_t >& inequalities_to_add_comparison_operator,
  std::vector < linear_inequality >& l,
  context_type& context,
  const rewriter& r)
{
  assert(inequalities_to_add_lhs.size()==inequalities_to_add_lhs_size.size() &&
         inequalities_to_add_lhs.size() ==inequalities_to_add_rhs.size() &&
         inequalities_to_add_lhs.size() == inequalities_to_add_comparison_operator.size() );
  for (std::vector < linear_inequality > ::iterator i = l.begin(); i != l.end();)
  {
    data_expression left;
    data_expression right;
    detail::comparison_t comparison_operator;
    i->typical_pair(left,right,comparison_operator,r);

    if (left!=real_zero())
    {
      bool pair_is_new=true;
      for (context_type::const_iterator c=context.begin() ; c!=context.end() && pair_is_new; ++c)
      {
        if (c->get_lowerbound()==left && c->get_upperbound()==right && c->comparison_operator()==comparison_operator)
        {
          pair_is_new=false;
          ++i;
        }
      }
      if (pair_is_new)
      {
        for (size_t j=0; j<inequalities_to_add_lhs.size(); ++j)
        {
          if (inequalities_to_add_lhs[j]==left && inequalities_to_add_rhs[j]==right && inequalities_to_add_comparison_operator[j]==comparison_operator)
          {
            pair_is_new=false;
            ++i;
          }
        }
        if (pair_is_new)
        {

          if (is_a_redundant_inequality(l,i,r))
          {
            i->swap(l.back());
            l.pop_back();
          }
          else
          {
            inequalities_to_add_lhs_size.push_back(i->lhs().size()); // store the number of variables at the lhs.
            inequalities_to_add_lhs.push_back(left);
            inequalities_to_add_rhs.push_back(right);
            inequalities_to_add_comparison_operator.push_back(comparison_operator);
            ++i;
          }
        }
      }
    }
    else
    {
      ++i;
    }
  }
}

/// \brief Generate a summand
/// \param s A summand
/// \param i A number, denoting the next state
/// \param cond A list of inequalities denoting the real part of the condition
/// \param context A variable context
/// \param r A rewriter
/// \return The summand corresponding to s with real part of condition cond, and
///      nextstate determined by i.
static void add_summand(summand_information& summand_info,
                        const data_expression& new_condition,
                        std::vector <linear_inequality>& nextstate_condition,
                        const context_type& complete_context,
                        const rewriter& r,
                        process::action_label_list& a,
                        identifier_generator<>& variable_generator,
                        const bool is_may_summand,
                        stochastic_action_summand_vector& action_summands,
                        deadlock_summand_vector& deadlock_summands)
{
  static std::vector < sort_expression_list > protect_against_garbage_collect;
  static std::map < std::pair < std::string, sort_expression_list >, std::string> action_label_map;
  // Used to recall which may actions labels have been
  // introduced, in order to re-use them.

  assignment_list nextstate = get_nonreal_assignments(summand_info.get_assignments());
  nextstate = reverse(nextstate);

  for (context_type::const_iterator c_complete = complete_context.begin();
       c_complete != complete_context.end(); ++c_complete)
  {
    data_expression substituted_lowerbound=
       data::replace_free_variables(c_complete->get_lowerbound(),summand_info.get_summand_real_nextstate_map());
    data_expression substituted_upperbound=
       data::replace_free_variables(c_complete->get_upperbound(),summand_info.get_summand_real_nextstate_map());
    linear_inequality e(substituted_lowerbound,substituted_upperbound,c_complete->comparison_operator(),r);
    data_expression t,u;
    detail::comparison_t comparison_operator;
    bool negate=e.typical_pair(t,u,comparison_operator,r);  
    bool success=false;
    /* First check whether the pair < t,u >
       already occurs in the context. In this case use the context variable */

    for (context_type::const_reverse_iterator c=complete_context.rbegin();
         ((c!=complete_context.rend()) && !success) ; ++c)
    {
      if (t==c->get_lowerbound() && u==c->get_upperbound())
      {  
        if (negate)
        {
          nextstate.push_front(assignment(c_complete->get_variable(),
                               application(sort_bool::not_(c->get_variable()))));
          success=true;
        }
        else if (comparison_operator==c->comparison_operator())
        {
          nextstate.push_front(assignment(c_complete->get_variable(),c->get_variable()));
          success=true;
        }
      }
    }
    /* if the variable is not represented by a context variable, find out what value it
       should get, based on the nextstate condition. By construction, there is exactly
       one possibility. The nextstate_condition is changed for this purpose. The changes
       are restored at the end. */
    if (!success)
    {
      nextstate_condition.push_back(e);
      if (!is_inconsistent(nextstate_condition,r))
      {
        nextstate.push_front(assignment(c_complete->get_variable(),sort_bool::true_()));
      }
      else
      {
        nextstate.push_front(assignment(c_complete->get_variable(),sort_bool::false_()));
      }
      nextstate_condition.pop_back();
    }
  }

  nextstate = reverse(nextstate);

  if (summand_info.is_delta_summand())
  {
    deadlock_summands.push_back(deadlock_summand(get_nonreal_variables(summand_info.get_summand().summation_variables()),
                                                 new_condition,
                                                 deadlock(summand_info.get_deadlock()).time()));
  }
  else
  { // Summand is not delta.
    process::action_list new_actions=summand_info.get_multi_action().actions();
    if (!summand_info.is_delta_summand() && is_may_summand)
    {
      process::action_list resulting_actions;
      for (process::action_list::const_iterator i=new_actions.begin(); i!=new_actions.end(); i++)
      {
        // put "_MAY" behind each action, and add its declaration to the action declarations.
        data_expression_list args=i->arguments();
        sort_expression_list sorts=i->label().sorts(); // get_sorts(args);
        std::map < std::pair< std::string, sort_expression_list >,
            std::string> ::iterator action_label_it=
              action_label_map.find(std::pair< std::string, sort_expression_list >
                                    (std::string(i->label().name()),sorts));
        if (action_label_it==action_label_map.end())
        {
          std::string may_action_label=variable_generator(std::string(i->label().name())+"_MAY");
          std::pair< std::string, sort_expression_list > p(std::string(i->label().name()),sorts);
          action_label_it=(action_label_map.insert(
                             std::pair< std::pair< std::string, sort_expression_list >,std::string>
                             (p,may_action_label))).first;
          a.push_front(process::action_label(may_action_label,sorts));
          protect_against_garbage_collect.push_back(sorts);
        }

        process::action_label may_action_label(action_label_it->second,sorts);
        resulting_actions.push_front(process::action(may_action_label,args));
      }
      new_actions=reverse(resulting_actions);
    }
    const lps::summand_base& s=summand_info.get_summand();
    action_summands.push_back(
                   stochastic_action_summand(get_nonreal_variables(s.summation_variables()),
                                             new_condition,
                                             multi_action(new_actions,summand_info.get_multi_action().time()),
                                             nextstate,
                                             summand_info.get_distribution()));
  }
}


static bool compare(const data_expression& t1, 
                    const data_expression& t2, 
                    const detail::comparison_t comp,
                    const rewriter& r)
{
 typedef mcrl2::data::detail::comparison_t ct;
  switch (comp)
  {
    case ct::less: return r(less(t1,t2))==sort_bool::true_();
    case ct::less_eq: return  r(less_equal(t1,t2))==sort_bool::true_();
    case ct::equal: return  r(equal_to(t1,t2))==sort_bool::true_();
  }
}

/// \brief Compute process initialisation given a variable context and a process
///        initialisation containing real values.
/// \param initialization A process initialisation containing real values
/// \param context A variable context
/// \param r A rewriter
/// \return A process initialisation in which all assignments to real variables
///      have been replaced with an initialization for each variable in context.
static
assignment_list determine_process_initialization(
  const assignment_list& initialization,
  context_type& context,
  const rewriter& r)
{
  assignment_list init = reverse(get_nonreal_assignments(initialization));
  assignment_list real_assignments = get_real_assignments(initialization);
  mutable_map_substitution< std::map<variable, data_expression> > replacements;
  for (assignment_list::const_iterator i = real_assignments.begin(); i != real_assignments.end(); ++i)
  {
    replacements[i->lhs()] = i->rhs();
  }

  for (context_type::const_iterator i = context.begin(); i != context.end(); ++i)
  {
    const data_expression left = replace_free_variables(i->get_lowerbound(), replacements);
    const data_expression right = replace_free_variables(i->get_upperbound(), replacements);
    const detail::comparison_t comparison=i->comparison_operator();

    assignment ass;
    if (compare(left,right,comparison,r))
    {
      ass = assignment(i->get_variable(), sort_bool::true_());
    }
    else 
    {
      ass = assignment(i->get_variable(), sort_bool::false_());
    }
    init.push_front(ass);
  }
  return reverse(init);
}



/// \brief Perform elimination of real variables on a specification in a maximum
///        number of iterations.
/// \param s A specification
/// \param max_iterations The maximal number of iterations the algorithm should
///        perform
/// \param strategy The rewrite strategy that should be used.
stochastic_specification realelm(stochastic_specification s, const size_t max_iterations, const rewrite_strategy strat)
{
  if (s.process().has_time())
  {
    throw  mcrl2::runtime_error("Input specification contains actions with time. Use lpsuntime first.");
  }

  rewriter r(s.data(),strat);
  set_identifier_generator variable_generator;
  variable_generator.add_identifiers(lps::find_identifiers((s)));
  stochastic_linear_process lps=s.process();
  const variable_list real_parameters = get_real_variables(lps.process_parameters());
  const variable_list nonreal_parameters = get_nonreal_variables(lps.process_parameters());
  std::vector < summand_information > summand_info;

  move_real_parameters_out_of_actions(s,real_parameters,r);
  normalize_specification(s, real_parameters, r, summand_info);

  context_type context; // Contains introduced variables

  std::vector < size_t > new_inequalities_sizes;
  std::vector < data_expression > new_inequalities_lhss;
  std::vector < data_expression > new_inequalities_rhss;
  std::vector < detail::comparison_t > new_comparison_operators;
  size_t iteration = 0;
  do
  {
    new_inequalities_sizes.clear();
    new_inequalities_lhss.clear();
    new_inequalities_rhss.clear();
    new_comparison_operators.clear();
    iteration++;
    mCRL2log(verbose) << "Iteration " <<  iteration << ", starting with " <<  context.size() << " context variables" << std::endl;

    for (summand_information& i: summand_info)
    {

      // First calculate the newly introduced variables xi for which the next_state value is not yet known
      // by only looking at variables that occur in the condition or in the effect.
      for (std::vector < std::vector < linear_inequality > >::iterator
           nextstate_combination = i.nextstate_context_combinations_begin();
           nextstate_combination != i.nextstate_context_combinations_end();
           ++nextstate_combination)
      {
        // Eliminate sum bound variables, resulting in inequalities over
        // process parameters of sort Real.

        variable_list sumvars= i.get_real_summation_variables();

        std::vector < linear_inequality > condition2;
        remove_redundant_inequalities(*nextstate_combination,condition2,r);
        *nextstate_combination=condition2;

        std::vector < linear_inequality > condition1;
        fourier_motzkin(condition2,
                        sumvars.begin(),
                        sumvars.end(),
                        condition1,
                        r);

        // First check which of these inequalities are equivalent to concrete values of xi variables.
        // Add these values for xi variables as a new condition. Remove these variables from the
        // context combinations to be considered for the xi variables.

        std::vector < linear_inequality > condition3;
        remove_redundant_inequalities(condition1,condition3,r);

        if (!condition3.empty() && !is_inconsistent(condition3,r))
        {
          mCRL2log(debug) << "Add new conditions " << pp_vector(condition3) << "\nin summand " << i.get_real_summation_variables() << "  " << i.get_multi_action() << "  " << i.get_assignments() << "\n" << 
                "Linear inequality " << pp_vector(*nextstate_combination) << "\n";; 
          // condition contains the inequalities over the process parameters
          add_inequalities_to_context_postponed(new_inequalities_sizes,
                                                new_inequalities_lhss,
                                                new_inequalities_rhss,
                                                new_comparison_operators,
                                                condition3, context, r);
        }
      }
    }
    add_postponed_inequalities_to_context(
      new_inequalities_sizes,
      new_inequalities_lhss,
      new_inequalities_rhss,
      new_comparison_operators,
      summand_info,
      context,
      r,
      variable_generator);
  }
  while ((iteration < max_iterations) && !new_inequalities_sizes.empty());

  mCRL2log(verbose) << "Generated the following variables in " <<  iteration << " iterations:" << std::endl;
  for (context_type::iterator i = context.begin(); i != context.end(); ++i)
  {
    mCRL2log(verbose) << "< " << data::pp(i->get_lowerbound()) << " " << pp(i->comparison_operator()) << " " << data::pp(i->get_upperbound())
                      << " > " << data::pp(i->get_variable()) << std::endl;
  }

  if (!new_inequalities_sizes.empty())
  {
    mCRL2log(verbose) << "A may-bisimilar lps is being generated, which is most likely not strongly bisimilar." << std::endl;
  }
  else
  {
    mCRL2log(verbose) << "A strongly bisimilar lps is being generated." << std::endl;
  }


  /* Generate the new summand list */
  lps::stochastic_action_summand_vector action_summands;
  lps::deadlock_summand_vector deadlock_summands;
  process::action_label_list new_act_declarations;
  for (std::vector < summand_information >::iterator i = summand_info.begin();
       i != summand_info.end(); ++i)
  {
    // Construct the real time condition for summand in terms of xi variables.

    for (std::vector < std::vector < linear_inequality > >::iterator
         nextstate_combination = i->nextstate_context_combinations_begin();
         nextstate_combination != i->nextstate_context_combinations_end();
         ++ nextstate_combination)
    {
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
      for (std::vector <linear_inequality>::const_iterator j=real_condition2.begin();
           j!=real_condition2.end(); ++j)
      {
        data_expression t;
        data_expression u;
        detail::comparison_t comparison_operator;
        bool negate=j->typical_pair(t,u,comparison_operator,r);
        bool found=false;
        for (context_type::iterator k = context.begin(); (k != context.end()) && !found ; ++k)
        {
          if (t==k->get_lowerbound() && u==k->get_upperbound() && k->comparison_operator()==comparison_operator)
          {
            if (negate)
            {
              new_condition=lazy::and_(new_condition,sort_bool::not_(k->get_variable()));
            }
            else 
            {
              new_condition=lazy::and_(new_condition,k->get_variable());
            }
            found=true;
          }
        }
        if (!found)
        {
          all_conditions_found=false;
        }
      }

      if (!all_conditions_found)
      {
        // add a may transition.
        add_summand(*i,
                    new_condition,
                    *nextstate_combination,
                    context,
                    r,
                    new_act_declarations,
                    variable_generator,
                    true,
                    action_summands,
                    deadlock_summands);
      }
      else
      {
        // add a must transition.
        add_summand(*i,
                    new_condition,
                    *nextstate_combination,
                    context,
                    r,
                    new_act_declarations,
                    variable_generator,
                    false,
                    action_summands,
                    deadlock_summands);
      }
    }
  }

  // Process parameters
  variable_list process_parameters = reverse(nonreal_parameters);
  for (context_type::const_iterator i = context.begin(); i != context.end(); ++i)
  {
    process_parameters.push_front(i->get_variable());
  }
  process_parameters = reverse(process_parameters);

  // New lps
  lps.process_parameters() = process_parameters;
  lps.action_summands() = action_summands;
  lps.deadlock_summands() = deadlock_summands;
  assignment_list initialization(determine_process_initialization(s.initial_process().assignments(), context, r));
  stochastic_process_initializer init(initialization,s.initial_process().distribution());

  return stochastic_specification(s.data(),
                                  s.action_labels()+new_act_declarations,
                                  s.global_variables(),
                                  lps,
                                  init);

}

} // namespace data
} // namespace mcrl2
