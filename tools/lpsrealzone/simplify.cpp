// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file simplify.cpp
/// \brief


#include "simplify.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/data/enumerator.h"

namespace mcrl2
{
namespace data
{

  /// \brief Returns a list of all real variables in l
  /// \param l a list of data variables
  /// \return The list of all v in l such that v.sort() == real()
  inline
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
  inline
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

  data_expression negate_inequality(const data_expression& e)
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
  inline
  bool is_inequality(data_expression e)
  {
    return is_equal_to_application(e) || is_less_application(e) ||
           is_less_equal_application(e) || is_greater_application(e) ||
           is_greater_equal_application(e);
  }

  // Functions below should have been defined in the data library.
  const data_expression& condition_part(const data_expression& e)
  {
    assert(is_if_application(e));
    const data::application& a = down_cast<application>(e);
    data::application::const_iterator i = a.begin();
    return *i;
  }

  const data_expression& then_part(const data_expression& e)
  {
    assert(is_if_application(e));
    const data::application& a = down_cast<application>(e);
    data::application::const_iterator i = a.begin();
    return *(++i);
  }

  const data_expression& else_part(const data_expression& e)
  {
    assert(is_if_application(e));
    const data::application& a = down_cast<application>(e);
    data::application::const_iterator i = a.begin();
    return *(++(++i));
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
  void split_condition_aux(
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
  void split_condition(
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

  void normalize_specification(
          const stochastic_specification& s,
          const variable_list& real_parameters,
          const rewriter& r,
          std::vector < summand_information1 >& summand_info)
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
            for(summand_information1& s: summand_info)
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
              const summand_information1 s(t,
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

            const summand_information1 s(t,
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


  mcrl2::lps::stochastic_specification simplify(mcrl2::lps::stochastic_specification s,
                                             const rewrite_strategy strat)
  {
    rewriter r(s.data(),strat);
    std::vector < summand_information1 > summand_info;

    const variable_list real_parameters = get_real_variables(s.process().process_parameters());
    move_real_parameters_out_of_actions(s, real_parameters, r);
    normalize_specification(s, get_real_variables(s.process().process_parameters()), r, summand_info);
    stochastic_action_summand_vector action_summands;
    deadlock_summand_vector deadlock_summands;
    for(std::vector<summand_information1>::iterator i = summand_info.begin(); i != summand_info.end(); i++) {
      if (i->is_delta_summand())
      {
        deadlock_summands.push_back(i->get_represented_deadlock_summand());
      }
      else
      {
        action_summands.push_back(i->get_represented_action_summand());
      }
    }
    stochastic_linear_process lps(s.process().process_parameters(),
                                  deadlock_summands,
                                  action_summands);
    lps::stochastic_specification spec1(
        s.data(),
        s.action_labels(),
        s.global_variables(),
        lps,
        s.initial_process());
    return spec1;
  }

} // namespace data
} // namespace mcrl2
