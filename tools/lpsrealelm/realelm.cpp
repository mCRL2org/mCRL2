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

#include "mcrl2/utilities/logger.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/replace.h"

#include "mcrl2/lps/find.h"

#include "realelm.h"
#include "linear_inequalities.h"

using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::lps;
using namespace mcrl2::log;


static data_expression negate_inequality(const data_expression e)
{
  if (is_equal_to_application(e))
  {
    return not_equal_to(application(e).left(),application(e).right());
  }
  if (is_not_equal_to_application(e))
  {
    return equal_to(application(e).left(),application(e).right());
  }
  else if (is_less_application(e))
  {
    return greater_equal(application(e).left(),application(e).right());
  }
  else if (is_less_equal_application(e))
  {
    return greater(application(e).left(),application(e).right());
  }
  else if (is_greater_application(e))
  {
    return less_equal(application(e).left(),application(e).right());
  }
  else if (is_greater_equal_application(e))
  {
    return less(application(e).left(),application(e).right());
  }
  else
  {
    throw mcrl2::runtime_error("Expression " + data::pp(e) + " is expected to be an inequality over sort Real");
  }
}

/// \brief Returns a list of all real variables in l
/// \param l a list of data variables
/// \ret The list of all v in l such that v.sort() == real()
static inline
variable_list get_real_variables(const variable_list& l)
{
  variable_list r;
  for (variable_list::const_iterator i = l.begin(); i != l.end(); ++i)
  {
    if (i->sort() == sort_real::real_())
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
  for (variable_list::const_iterator i = l.begin(); i != l.end(); ++i)
  {
    if (i->sort() != sort_real::real_())
    {
      r = push_front(r, *i);
    }
  }
  return r;
}

static data::function_symbol& negate_function_symbol(const sort_expression s)
{
  static data::function_symbol f = data::function_symbol("negate",data::make_function_sort(s,s));
  assert(data::make_function_sort(s,s)==f.sort()); // Protect against using f for other sorts than sort comp.
  return f;
}

/// \brief Returns a list of all real assignments in l
/// \param l a list of data assignments
/// \ret The list of all x := e in l such that x.sort() == e.sort() == real()
static inline
assignment_list get_real_assignments(const assignment_list& l)
{
  assignment_list r;
  for (assignment_list::const_iterator i = l.begin(); i != l.end(); ++i)
  {
    if (i->lhs().sort() == sort_real::real_())
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
  for (assignment_list::const_iterator i = l.begin(); i != l.end(); ++i)
  {
    if (i->lhs().sort() != sort_real::real_())
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
  return *boost::next(application(e).arguments().begin(), 0);
}

static data_expression then_part(const data_expression e)
{
  assert(is_if_application(e));
  return *boost::next(application(e).arguments().begin(), 1);
}

static data_expression else_part(const data_expression e)
{
  assert(is_if_application(e));
  return *boost::next(application(e).arguments().begin(), 2);
}


/// \brief Splits a condition in expressions ranging over reals and the others
/// \details Conceptually, the condition is first transformed to conjunctive
///          normalform. For each conjunct, there will be an entry in both
///          resulting std::vectors, where the real conditions are in "real_conditions",
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
  std::vector < data_expression_list > &real_conditions,
  std::vector < data_expression_list > &non_real_conditions,
  const bool negate=false)
{
  // mCRL2log(debug) << "Split condition " << data::pp(e) << "\n";
  real_conditions.clear();
  non_real_conditions.clear();

  if ((!negate && sort_bool::is_and_application(e))  || (negate && sort_bool::is_or_application(e)))
  {
    std::vector < data_expression_list >
    real_conditions_aux1, non_real_conditions_aux1;
    split_condition(application(e).left(),real_conditions_aux1,non_real_conditions_aux1,negate);
    std::vector < data_expression_list >
    real_conditions_aux2, non_real_conditions_aux2;
    split_condition(application(e).right(),real_conditions_aux2,non_real_conditions_aux2,negate);
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
    split_condition(application(e).left(),real_conditions,non_real_conditions,negate);
    std::vector < data_expression_list >
    real_conditions_aux, non_real_conditions_aux;
    split_condition(application(e).right(),real_conditions_aux,non_real_conditions_aux,negate);
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
    split_condition(sort_bool::or_(sort_bool::and_(condition_part(e),then_part(e)),
                                   sort_bool::and_(sort_bool::not_(condition_part(e)),else_part(e))),
                    real_conditions,non_real_conditions,negate);
  }
  else if (sort_bool::is_not_application(e))
  {
    split_condition(*application(e).arguments().begin(),real_conditions,non_real_conditions,!negate);
  }
  else if (is_inequality(e) && (application(e).left().sort() == sort_real::real_() || application(e).right().sort() == sort_real::real_()))
  {
    std::set < variable > vars=data::find_variables(e);
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
      real_conditions.push_back(push_front(data_expression_list(),negate_inequality(e)));
      non_real_conditions.push_back(data_expression_list());
    }
    else
    {
      real_conditions.push_back(push_front(data_expression_list(),e));
      non_real_conditions.push_back(data_expression_list());
    }
  }
  else
  {
    // e is assumed to be a non_real expression.
    std::set < variable > vars=data::find_variables(e);
    for (std::set < variable >::const_iterator i=vars.begin(); i!=vars.end(); ++i)
    {
      if (i->sort()==sort_real::real_())
      {
        throw  mcrl2::runtime_error("Expression " + data::pp(e) + " contains variable " +                                          data::pp(*i) + " of sort Real.");
      }
    }
    if (negate)
    {
      non_real_conditions.push_back(push_front(data_expression_list(),
                                    data_expression(sort_bool::not_(e))));
      real_conditions.push_back(data_expression_list());
    }
    else
    {
      non_real_conditions.push_back(push_front(data_expression_list(),e));
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
  lps::deprecated::summand_list smds = lps::deprecated::linear_process_summands(s.process());
  // summand_list sl;
  for (lps::deprecated::summand_list::const_iterator i = smds.begin(); i != smds.end(); ++i)
  {
    std::vector <data_expression_list> real_conditions, non_real_conditions;
    // mCRL2log(debug) << "SUMMANDNORM: " << lps::pp(*i) << "\n";
    // mCRL2log(debug) << "Condition in: " << data::pp(i->condition()) << "\n";
    split_condition(i->condition(),real_conditions,non_real_conditions);

    for (std::vector <data_expression_list>::const_iterator
         j_r=real_conditions.begin(), j_n=non_real_conditions.begin();
         j_r!=real_conditions.end(); ++j_r, ++j_n)
    {
      lps::deprecated::summand t(*i);
      const data_expression c=r(lazy::join_and(j_n->begin(), j_n->end()));
      if (!sort_bool::is_false_function_symbol(c))
      {
        t=set_condition(t,c);

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

        // mCRL2log(debug) << "REALPARS " << data::pp(i->next_state(real_parameters)) << "\n";
        const std::set < variable> s1=data::find_variables(i->next_state(real_parameters));
        // for(std::set < variable>::const_iterator k=s1.begin(); k!=s1.end(); ++k)
        // { mCRL2log(debug) << "VAR " << data::pp(*k) << "\n";
//
//
        // }

        const variable_list original_real_sum_variables=get_real_variables(i->summation_variables());
        variable_list real_sum_variables;
        variable_list eliminatable_real_sum_variables;
        for (variable_list::const_iterator k=original_real_sum_variables.begin();
             k!=original_real_sum_variables.end(); ++k)
        {

          // mCRL2log(debug) << "Treat " << data::pp(*k) << "\n";
          if (s1.count(*k)==0)
          {
            // The variable does not occur in the parameters. We can eliminate it using Fourier-Motzkin
            eliminatable_real_sum_variables=push_front(eliminatable_real_sum_variables,*k);
          }
          else
          {
            real_sum_variables=push_front(real_sum_variables,*k);
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

        if ((inequalities.size()>0) && (inequalities.front().is_false(r)))
        {
          //  mCRL2log(debug) << "INCONSISTENT \n";
        }
        else
        {
          // Add for all real parameters x of the process an inequality 0<=x
          for (variable_list::const_iterator k=real_parameters.begin(); k!=real_parameters.end(); k++)
          {
            data_expression e=(atermpp::aterm_appl)*k;
            inequalities.push_back(linear_inequality(real_zero(),e,linear_inequality::less_eq,r));
          }

          // Add for all real sum variables x of this summand an inequality 0<=x
          for (variable_list::const_iterator k=real_sum_variables.begin(); k!=real_sum_variables.end(); k++)
          {
            const data_expression e=(atermpp::aterm_appl)*k;
            inequalities.push_back(linear_inequality(real_zero(),e,linear_inequality::less,r));
          }

          // Construct replacements to contain the nextstate values for real variables in a map
          std::map<variable, data_expression> replacements;
          for (assignment_list::const_iterator j = i->assignments().begin(); j != i->assignments().end(); ++j)
          {
            if (j->lhs().sort() == sort_real::real_())
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

/// \brief Add postponed inequalities to variable context
/// \param inequalities_to_add The pairs of inequalities to be added.
/// \param context A variable context
/// \param variable_generator A fresh variable generator
/// \post All inequalities in l are in the context
/// \ret true iff a variable has been added to the context
static void add_postponed_inequalities_to_context(
  const std::vector < size_t > &inequalities_to_add_lhs_size,
  const std::vector < data_expression > &inequalities_to_add_lhs,
  const std::vector < data_expression > &inequalities_to_add_rhs,
  std::vector < summand_information > &summand_info,
  context_type& context,
  const rewriter& r,
  identifier_generator<>& variable_generator,
  const comp_struct& c
)
{
  assert(inequalities_to_add_lhs.size() ==inequalities_to_add_lhs_size.size() &&
         inequalities_to_add_lhs.size() ==inequalities_to_add_rhs.size());

  // We add new next state arguments with increasing sizes of their lhs's.
  std::set <size_t> sorted_lhs_sizes(inequalities_to_add_lhs_size.begin(),inequalities_to_add_lhs_size.end());

  for (std::set <size_t>::const_iterator current_size=sorted_lhs_sizes.begin();
       current_size!=sorted_lhs_sizes.end(); ++current_size)
  {
    for (size_t i=0; i<inequalities_to_add_lhs.size(); ++i)
    {
      if (inequalities_to_add_lhs_size[i]== *current_size)
      {
        variable xi(variable_generator("xi"), c.sort());
        context.push_back(real_representing_variable(xi,inequalities_to_add_lhs[i], inequalities_to_add_rhs[i]));
        mCRL2log(verbose) << "Introduced variable " <<  data::pp(xi) << " for <" << data::pp(inequalities_to_add_lhs[i]) <<
                    "," <<  data::pp(inequalities_to_add_rhs[i]) << ">\n";

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
/// \ret true iff a variable has been added to the context
static void add_inequalities_to_context_postponed(
  std::vector < size_t > &inequalities_to_add_lhs_size,
  std::vector < data_expression > &inequalities_to_add_lhs,
  std::vector < data_expression > &inequalities_to_add_rhs,
  std::vector < linear_inequality > &l,
  context_type& context,
  const rewriter& r)
{
  assert(inequalities_to_add_lhs.size()==inequalities_to_add_lhs_size.size() &&
         inequalities_to_add_lhs.size() ==inequalities_to_add_rhs.size());
  // mCRL2log(debug) << "Inequalities to add: " << pp_vector(inequalities_to_add) << "\n";
  for (std::vector < linear_inequality > ::iterator i = l.begin(); i != l.end();)
  {
    data_expression left;
    data_expression right;
    i->typical_pair(left,right,r);

    if (left!=real_zero())
    {
      bool pair_is_new(true);
      for (context_type::const_iterator c=context.begin() ; c!=context.end() && pair_is_new; ++c)
      {
        if ((c->get_lowerbound()==left) && (c->get_upperbound()==right))
        {
          pair_is_new=false;
          ++i;
        }
      }
      if (pair_is_new)
      {
        for (size_t j=0; j<inequalities_to_add_lhs.size(); ++j)
        {
          if ((inequalities_to_add_lhs[j]==left) && (inequalities_to_add_rhs[j]==right))
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
            // mCRL2log(debug) << "Reserved to be added <" << data::pp(left) << "," << data::pp(right) << "\n";
            inequalities_to_add_lhs_size.push_back(i->lhs().size()); // store the number of variables at the lhs.
            inequalities_to_add_lhs.push_back(left);
            inequalities_to_add_rhs.push_back(right);
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
/// \ret The summand corresponding to s with real part of condition cond, and
///      nextstate determined by i.
static
lps::deprecated::summand generate_summand(summand_information& summand_info,
                         const data_expression& new_condition,
                         std::vector <linear_inequality> &nextstate_condition,
                         const context_type& complete_context,
                         const rewriter& r,
                         action_label_list& a,
                         identifier_generator<>& variable_generator,
                         const comp_struct& cs,
                         const bool is_may_summand=false)
{
  // mCRL2log(debug) << "SUMMAND " << lps::pp(summand_info.get_summand()) << "\nCOND " << data::pp(new_condition) << "\n";
  static std::vector < sort_expression_list > protect_against_garbage_collect;
  static std::map < std::pair < std::string, sort_expression_list >, std::string> action_label_map;
  // Used to recall which may actions labels have been
  // introduced, in order to re-use them.
  const lps::deprecated::summand s=summand_info.get_summand();

  assignment_list nextstate = get_nonreal_assignments(s.assignments());
  nextstate = reverse(nextstate);

  for (context_type::const_iterator c_complete = complete_context.begin();
       c_complete != complete_context.end(); ++c_complete)
  {
    data_expression substituted_lowerbound=
       data::replace_free_variables(c_complete->get_lowerbound(),summand_info.get_summand_real_nextstate_map());
    data_expression substituted_upperbound=
       data::replace_free_variables(c_complete->get_upperbound(),summand_info.get_summand_real_nextstate_map());
    // mCRL2log(debug) << "Lower Upper " << data::pp(substituted_lowerbound) << "  " << data::pp(substituted_upperbound) << "\n";
    linear_inequality e(substituted_lowerbound,substituted_upperbound,linear_inequality::less,r);
    // mCRL2log(debug) << "INequality: " << string(e) << "\n";
    data_expression t,u;
    const bool negate=e.typical_pair(t,u,r);  // XXXXXXXXX       This can negate inequality, causing trouble.
    // mCRL2log(debug) << "Typical pair " << data::pp(t) << "  " << data::pp(u) << "   " << negate << "\n";

    bool success(false);
    /* First check whether the pair < t,u >
       already occurs in the context. In this case use the context variable */

    for (context_type::const_reverse_iterator c=complete_context.rbegin();
         ((c!=complete_context.rend()) && !success) ; ++c)
    {
      if ((t==c->get_lowerbound()) && (u==c->get_upperbound()))
      {
        if (negate)
        {
          nextstate=push_front(nextstate,assignment(c_complete->get_variable(),
                               make_application(negate_function_symbol(cs.sort()),c->get_variable())));
        }
        else
        {
          nextstate=push_front(nextstate,assignment(c_complete->get_variable(),c->get_variable()));
        }
        success=true;
      }
    }
    /* if the variable is not represented by a context variable, find out what value it
       should get, based on the nextstate condition. By construction, there is exactly
       one possibility. The nextstate_condition is changed for this purpose. The changes
       are restored at the end. */
    if (!success)
    {
      nextstate_condition.push_back(linear_inequality(
                                      substituted_lowerbound,
                                      substituted_upperbound,
                                      linear_inequality::less_eq,r));
      // mCRL2log(debug) << "Nextstate_condition " << pp_vector(nextstate_condition) << "\n";
      if (is_inconsistent(nextstate_condition,r))
      {
        // mCRL2log(debug) << "ASSIGNMENT: " << c_complete->get_variable() << " := " << data_expression(cs.larger()) << "\n";
        nextstate=push_front(nextstate,assignment(c_complete->get_variable(),data_expression(cs.larger())));
      }
      else
      {
        nextstate_condition[nextstate_condition.size()-1]=linear_inequality(substituted_upperbound,
            substituted_lowerbound,linear_inequality::less_eq,r);
        if (is_inconsistent(nextstate_condition,r))
        {
          nextstate=push_front(nextstate,assignment(c_complete->get_variable(),data_expression(cs.smaller())));
        }
        else
        {
          nextstate=push_front(nextstate,assignment(c_complete->get_variable(),data_expression(cs.equal())));
        }
      }
      nextstate_condition.pop_back();
    }
  }

  nextstate = reverse(nextstate);

  action_list new_actions;
  if ((!s.is_delta()) && is_may_summand)
  {
    new_actions=s.actions();
    action_list resulting_actions;
    for (action_list::const_iterator i=new_actions.begin();
         i!=new_actions.end(); i++)
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
        a=push_front(a,action_label(may_action_label,sorts));
        protect_against_garbage_collect.push_back(sorts);
      }

      action_label may_action_label(action_label_it->second,sorts);
      resulting_actions=push_front(resulting_actions,action(may_action_label,args));
    }
    new_actions=reverse(resulting_actions);
  }

  lps::deprecated::summand result = lps::deprecated::summand(get_nonreal_variables(s.summation_variables()),
                           new_condition, s.is_delta(), new_actions, nextstate);

  return result;
}

/// \brief Compute process initialisation given a variable context and a process
///        initialisation containing real values.
/// \param initialization A process initialisation containing real values
/// \param context A variable context
/// \param r A rewriter
/// \ret A process initialisation in which all assignments to real variables
///      have been replaced with an initialization for each variable in context.
static
assignment_list determine_process_initialization(
  const assignment_list& initialization,
  context_type& context,
  const rewriter& r,
  const comp_struct& c)
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
    /* data_expression left = realelm_data_expression_map_replace(i->get_lowerbound(), replacements);
    data_expression right = realelm_data_expression_map_replace(i->get_upperbound(), replacements); */
    const data_expression left = replace_free_variables(i->get_lowerbound(), replacements);
    const data_expression right = replace_free_variables(i->get_upperbound(), replacements);
    assignment ass;
    if (r(less(left, right)) == sort_bool::true_())
    {
      ass = assignment(i->get_variable(), c.smaller());
    }
    else if (r(equal_to(left, right)) == sort_bool::true_())
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
specification realelm(specification s, int max_iterations, const rewriter& r)
{
  if (s.process().has_time())
  {
    throw  mcrl2::runtime_error("Input specification contains actions with time. Use lpsuntime first.");
  }
  // First add a constructor with elements smaller, larger and equal to the specification,
  // including a mapping negate that reverses smaller into larger and vice versa.
  comp_struct c;
  data_specification ds=s.data();
  ds.add_alias(alias(c.basic_sort_name(),c));
  ds.add_mapping(negate_function_symbol(c.sort()));
  ds.add_equation(data_equation(  // negate(larger)=smaller;
                    std::vector <variable>(),
                    sort_bool::true_(),
                    make_application(negate_function_symbol(c.sort()),c.larger()),
                    c.smaller()));
  ds.add_equation(data_equation(  // negate(smaller)=larger;
                    std::vector <variable>(),
                    sort_bool::true_(),
                    make_application(negate_function_symbol(c.sort()),c.smaller()),
                    c.larger()));
  ds.add_equation(data_equation(  // negate(equal)=equal;
                    std::vector <variable>(),
                    sort_bool::true_(),
                    make_application(negate_function_symbol(c.sort()),c.equal()),
                    c.equal()));
  variable v("x",c.sort());
  std::vector <variable> vars;
  vars.push_back(v);
  ds.add_equation(data_equation(  // negate(negate(x))=x;
                    vars,
                    sort_bool::true_(),
                    make_application(negate_function_symbol(c.sort()),make_application(negate_function_symbol(c.sort()),v)),
                    v));

  s.data() = ds;
  set_identifier_generator variable_generator;
  variable_generator.add_identifiers(lps::find_identifiers((s)));
  linear_process lps=s.process();
  const variable_list real_parameters = get_real_variables(lps.process_parameters());
  const variable_list nonreal_parameters = get_nonreal_variables(lps.process_parameters());
  std::vector < summand_information > summand_info;
  // mCRL2log(debug) << "Normalize specification in\n";
  normalize_specification(s, real_parameters, r, summand_info);
  // mCRL2log(debug) << "Normalize specification out\n";

  context_type context; // Contains introduced variables

  std::vector < size_t > new_inequalities_sizes;
  std::vector < data_expression > new_inequalities_lhss;
  std::vector < data_expression > new_inequalities_rhss;
  int iteration = 0;
  do
  {
    new_inequalities_sizes.clear();
    new_inequalities_lhss.clear();
    new_inequalities_rhss.clear();
    iteration++;
    mCRL2log(verbose) << "Iteration " <<  iteration << ", starting with " <<  context.size() << " context variables" << std::endl;

    for (std::vector < summand_information >::iterator i = summand_info.begin();
         i != summand_info.end(); ++i)
    {
      // mCRL2log(debug) << "SUMMAND_IN " << lps::pp(i->get_summand()) << "\n" ;

      // First calculate the newly introduced variables xi for which the next_state value is not yet known.
      // get , by only looking at variables that
      // occur in the condition or in the effect.
      for (std::vector < std::vector < linear_inequality > >::iterator
           nextstate_combination = i->nextstate_context_combinations_begin();
           nextstate_combination != i->nextstate_context_combinations_end();
           ++ nextstate_combination)
      {
        // Eliminate sum bound variables, resulting in inequalities over
        // process parameters of sort Real.

        variable_list sumvars= i->get_real_summation_variables();

        // mCRL2log(debug) << "SUMVARS " << data::pp(sumvars) << "\n" ;
        std::vector < linear_inequality > condition2;
        remove_redundant_inequalities(*nextstate_combination,condition2,r);
        // mCRL2log(debug) << "CONDITION IN" << pp_vector(*nextstate_combination) << "\n" ;
        // mCRL2log(debug) << "REMOVING REDUNDANT INEQUALITIES: FROM1 " << nextstate_combination->size() << " TO " << condition2.size() << "\n";
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
        // mCRL2log(debug) << "CONDITION OUT" << pp_vector(condition3) << "\n" ;

        // mCRL2log(debug) << "REMOVING REDUNDANT INEQUALITIES: FROM2 " << condition1.size() << " TO " << condition3.size() << "\n";
        if (!is_inconsistent(condition3,r))
        {
          // condition contains the inequalities over the process parameters
          add_inequalities_to_context_postponed(new_inequalities_sizes,
                                                new_inequalities_lhss,
                                                new_inequalities_rhss,
                                                condition3, context, r);
        }
      }
    }
    add_postponed_inequalities_to_context(
      new_inequalities_sizes,
      new_inequalities_lhss,
      new_inequalities_rhss,
      summand_info,
      context,
      r,
      variable_generator,
      c);

  }
  while ((iteration < max_iterations) && !new_inequalities_sizes.empty());

  mCRL2log(verbose) << "Generated the following variables in " <<  iteration << " iterations:" << std::endl;
  for (context_type::iterator i = context.begin(); i != context.end(); ++i)
  {
    mCRL2log(verbose) << "< " << data::pp(i->get_lowerbound()) << ", " << data::pp(i->get_upperbound())
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
  // std::vector < data_expression_list > nextstate_context_combinations;
  lps::deprecated::summand_list summands;
  action_label_list new_act_declarations;
  for (std::vector < summand_information >::iterator i = summand_info.begin();
       i != summand_info.end(); ++i)
  {
    // mCRL2log(debug) << "SUMMAND_IN__ " << lps::pp(i->get_summand()) << "\n";

    // Construct the real time condition for summand in terms of xi variables.

    /* std::vector < data_expression_list >::const_iterator
                         nextstate_value=i->nextstate_value_combinations_begin(); */

    for (std::vector < std::vector < linear_inequality > >::iterator
         nextstate_combination = i->nextstate_context_combinations_begin();
         nextstate_combination != i->nextstate_context_combinations_end();
         ++ nextstate_combination)
    {
      // mCRL2log(debug) << "Nextstate cond: " << pp_vector(*nextstate_combination) << "\n";

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
      // mCRL2log(debug) << "Normalised nextstate cond: " << pp_vector(real_condition2) << "\n";
      for (std::vector <linear_inequality>::const_iterator j=real_condition2.begin();
           j!=real_condition2.end(); ++j)
      {
        // mCRL2log(debug) << "condition " << string(*j) << "\n";
        data_expression t;
        data_expression u;
        j->typical_pair(t,u,r);
        bool found=false;
        for (context_type::iterator k = context.begin(); (k != context.end()) && !found ; ++k)
        {
          if ((t==k->get_lowerbound()) && (u==k->get_upperbound()))
          {
            found=true;
            if (j->comparison()==linear_inequality::equal)
            {
              new_condition=lazy::and_(new_condition,c.is_equal(k->get_variable()));
            }
            else if ((j->lhs_begin()!=j->lhs_end()) && (is_positive(j->lhs_begin()->second,r)))
            {
              // The inequality has *j has shape t<u or t<=u
              if (j->comparison()==linear_inequality::less)
              {
                new_condition=lazy::and_(new_condition,c.is_smaller(k->get_variable()));
              }
              else
              {
                assert(j->comparison()==linear_inequality::less_eq);
                new_condition=lazy::and_(new_condition,sort_bool::not_(c.is_larger(k->get_variable())));
              }
            }
            else
            {
              // The inequality *j has shape t>=u or t>u
              if (j->comparison()==linear_inequality::less)
              {
                new_condition=lazy::and_(new_condition,c.is_larger(k->get_variable()));
              }
              else
              {
                assert(j->comparison()==linear_inequality::less_eq);
                new_condition=lazy::and_(new_condition,sort_bool::not_(c.is_smaller(k->get_variable())));
              }
            }
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
        lps::deprecated::summand s = generate_summand(*i,
                                     new_condition,
                                     *nextstate_combination,
                                     context,
                                     r,
                                     new_act_declarations,
                                     variable_generator,
                                     c,
                                     true);
        // mCRL2log(debug) << "MAY SUMMAND_OUT: " << lps::pp(s) << "\n";
        summands = push_front(summands, s);
      }
      else
      {
        // add a must transition.
        lps::deprecated::summand s = generate_summand(*i,
                                     new_condition,
                                     *nextstate_combination,
                                     context,
                                     r,
                                     new_act_declarations,
                                     variable_generator,
                                     c,
                                     false);
        // mCRL2log(debug) << "MUST SUMMAND_OUT: " << lps::pp(s) << "\n";
        summands = push_front(summands, s);
      }
    }
  }


  summands = reverse(summands);

  // Process parameters
  variable_list process_parameters = reverse(nonreal_parameters);
  for (context_type::const_iterator i = context.begin(); i != context.end(); ++i)
  {
    process_parameters = push_front(process_parameters, i->get_variable());
  }
  process_parameters = reverse(process_parameters);

  // New lps
  lps.process_parameters() = process_parameters;
  lps::deprecated::set_linear_process_summands(lps, summands);
  assignment_list initialization(determine_process_initialization(s.initial_process().assignments(), context, r,c));
  process_initializer init(initialization);

  return specification(s.data(),
                       s.action_labels()+new_act_declarations,
                       s.global_variables(),
                       lps,
                       init);

}

