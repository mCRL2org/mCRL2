// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file fourier_motzkin.h
/// \brief Contains functions to apply Fourier-Motzkin on linear inequalities
///        and data expressions. 


#ifndef MCRL2_DATA_FOURIER_MOTZKIN_H
#define MCRL2_DATA_FOURIER_MOTZKIN_H

#include <algorithm>

#include "mcrl2/data/optimized_boolean_operators.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/linear_inequalities.h"
#include "mcrl2/data/detail/linear_inequalities_utilities.h"

namespace mcrl2
{

namespace data
{


template < class Data_variable_iterator >
inline void fourier_motzkin(const std::vector < linear_inequality >& inequalities_in,
                     Data_variable_iterator variables_begin,
                     Data_variable_iterator variables_end,
                     std::vector < linear_inequality >& resulting_inequalities,
                     const rewriter& r)
{
  assert(resulting_inequalities.empty());
  if (mCRL2logEnabled(log::debug2))
  {
    mCRL2log(log::debug2) << "Starting Fourier-Motzkin elimination on " + pp_vector(inequalities_in) + " on variables ";
    for (Data_variable_iterator i=variables_begin;
         i!=variables_end; ++i)
    {
      mCRL2log(log::debug2) << " " << pp(*i) ;
    }
    mCRL2log(log::debug2) << std::endl;
  }

  std::vector < linear_inequality > inequalities;
  std::vector < linear_inequality > equalities;
  std::set < variable > vars=
  gauss_elimination(inequalities_in,
                    equalities,      // Store all resulting equalities here.
                    inequalities,    // Store all resulting non equalities here.
                    variables_begin,
                    variables_end,
                    r);

  // At this stage, the variables that should be eliminated only occur in
  // inequalities. Group the inequalities into positive, 0, and negative
  // occurrences of each variable, and create a new system.
  for (std::set < variable >::const_iterator i = vars.begin(); i != vars.end(); ++i)
  {
    std::map < variable, size_t> nr_positive_occurrences;
    std::map < variable, size_t> nr_negative_occurrences;
    count_occurrences(inequalities,nr_positive_occurrences,nr_negative_occurrences,r);

    bool found=false;
    size_t best_choice=0;
    variable best_variable;
    for (std::set < variable >::const_iterator k = vars.begin(); k != vars.end(); ++k)
    {
      const size_t p=nr_positive_occurrences[*k];
      const size_t n=nr_negative_occurrences[*k];
      if ((p!=0) || (n!=0))
      {
        if (found)
        {
          if (n*p<best_choice)
          {
            best_choice=n*p;
            best_variable=*k;
          }
        }
        else
        {
          // found is false
          best_choice=n*p;
          best_variable=*k;
          found=true;
        }
      }
      if (found && (best_choice==0))
      {
        // Stop searching, we cannot find a better candidate.
        break;
      }
    }

    mCRL2log(log::debug2) << "Best variable " << pp(best_variable) << "\n";

    if (!found)
    {
      // There are no variables anymore that can be removed from inequalities
      break;
    }
    std::vector < linear_inequality > new_inequalities;
    std::vector < linear_inequality> inequalities_with_positive_variable;
    std::vector < linear_inequality> inequalities_with_negative_variable;   // Idem.

    for (const linear_inequality& e: inequalities)
    {
      const detail::lhs_t::const_iterator factor_it=e.lhs().find(best_variable); 
      if (factor_it==e.lhs().end()) // variable best_variable does not occur in inequality e.
      {
        new_inequalities.push_back(e);
      }
      else
      {
        data_expression f=factor_it->factor();
        if (is_positive(f,r))
        {
          inequalities_with_positive_variable.push_back(e);
        }
        else if (is_negative(f,r))
        {
          inequalities_with_negative_variable.push_back(e);
        }
        else
        {
          assert(0);
        }
      }
    }

    mCRL2log(log::debug2) << "Positive :" << pp_vector(inequalities_with_positive_variable) << "\n";
    mCRL2log(log::debug2) << "Negative :" << pp_vector(inequalities_with_negative_variable) << "\n";
    mCRL2log(log::debug2) << "Equalities :" << pp_vector(equalities) << "\n";
    mCRL2log(log::debug2) << "Rest :" << pp_vector(new_inequalities) << "\n";

    // Variables are grouped, now construct new inequalities as follows:
    // Keep the zero occurrences
    // Combine each positive and negative equation as follows with x the best variable:
    // Given inequalities N + bi * x <= ci
    //                    M - bj * x <= cj
    // This is equivalent to N/bi + M/bj <= ci/bi + cj/bj
    for (const linear_inequality& e1: inequalities_with_positive_variable)
    {
      for (const linear_inequality& e2: inequalities_with_negative_variable)
      {
        const detail::lhs_t::const_iterator e1_best_variable_it=e1.lhs().find(best_variable);
        const data_expression& e1_factor=e1_best_variable_it->factor();
        const data_expression& e1_reduced_rhs=real_divides(e1.rhs(),e1_factor);  
        const detail::lhs_t e1_reduced_lhs=detail::remove_variable_and_divide(e1.lhs(),best_variable,e1_factor,r);

        const detail::lhs_t::const_iterator e2_best_variable_it=e2.lhs().find(best_variable);
        const data_expression& e2_factor=e2_best_variable_it->factor();
        const data_expression& e2_reduced_rhs=real_divides(e2.rhs(),e2_factor);  
        const detail::lhs_t e2_reduced_lhs=detail::remove_variable_and_divide(e2.lhs(),best_variable,e2_factor,r);
        const linear_inequality new_inequality(subtract(e1_reduced_lhs,e2_reduced_lhs,r),
                                               r(real_minus(e1_reduced_rhs,e2_reduced_rhs)),
                                               (e1.comparison()==detail::less_eq) && (e2.comparison()==detail::less_eq)?
                                                   detail::less_eq:
                                                   detail::less,r);
        if (new_inequality.is_false(r))
        {
          resulting_inequalities.push_back(linear_inequality()); // This is a single contraditory inequality;
          return;
        }
        if (!new_inequality.is_true(r))
        {
          new_inequalities.push_back(new_inequality);
        }
      }
    }
    inequalities.swap(new_inequalities);
  }

  resulting_inequalities.swap(inequalities);
  // Add the equalities to the inequalities and return the result
  for (std::vector < linear_inequality > :: const_iterator i=equalities.begin();
       i!=equalities.end(); ++i)
  {
    assert(!i->is_false(r));
    if (!i->is_true(r))
    {
      resulting_inequalities.push_back(*i);
    }
  }
  mCRL2log(log::debug2) << "Fourier-Motzkin elimination yields " << pp_vector(resulting_inequalities) << std::endl;
}

/// \brief Eliminate variables from a data expression using Gauss elimination and
///        Fourier-Motzkin elimination.
/// \details Deliver a data_expression e_out and a set of variables vars_out such that 
///          exists vars_in.e_in is equivalent to exists vars_out.e_out. 
///          If the resulting list of inequalities is inconsistent, then [false] is
///          returned. 
/// \param e_in An input data_expression of sort Bool.
/// \param vars_in A container with variables. Supports iterating over these variables.
/// \param e_out The output data expression of sort Bool.
/// \param vars_out A list of variables to store resulting variables. Initially empty.
/// \param r A rewriter.
/// \post exists vars_out.e_out == exists vars_in.e_in.

inline void fourier_motzkin(const data_expression& e_in,
                            const variable_list& vars_in,
                            data_expression& e_out,
                            variable_list& vars_out,
                            const rewriter& r)
{
  assert(e_in.sort()==sort_bool::bool_());
  assert(vars_out.empty());

  // First check whether there are variables of sort real in vars_in. If not fourier motzkin does not make sense. 
  if (std::find_if(vars_in.begin(),vars_in.end(),[](variable v){ return v.sort()==sort_real::real_();})==vars_in.end())
  {
    vars_out=vars_in;
    e_out=e_in;
    return;
  }
  
  std::vector <data_expression_list> real_conditions; 
  std::vector <data_expression> non_real_conditions;
  detail::split_condition(e_in,real_conditions,non_real_conditions);

  // Determine all variables that occur in the sum operator, but not in the
  // next state. We can apply Fourier-Motzkin to eliminate these variables from
  // this sum operator and the condition.

  const std::set < variable> non_eliminatable_variables=data::find_all_variables(non_real_conditions);

  variable_list real_sum_variables;
  variable_list eliminatable_real_sum_variables;
  for (const variable& v: vars_in)
  {
    if (non_eliminatable_variables.count(v)==0)
    {
      // The variable does not occur in the parameters. We can eliminate it using Fourier-Motzkin
      eliminatable_real_sum_variables.push_front(v);
    }
    else
    {
      vars_out.push_front(v);
    }
  }
  if (vars_out.size()==vars_in.size())
  {
    // No variables can be eliminated. Stop here. 
    e_out=e_in;
    return;
  }

  // Now apply fourier-motzkin to each conjunct of linear inequalities. 
  std::vector <data_expression>::const_iterator j_n=non_real_conditions.begin();
  std::set< data_expression > result_disjunction_set;

  for (std::vector <data_expression_list>::const_iterator j_r=real_conditions.begin();
       j_r!=real_conditions.end(); ++j_r, ++j_n)
  {
    const data_expression non_real_condition=*j_n;
    if (!sort_bool::is_false_function_symbol(non_real_condition))
    {
      try
      {
        std::vector < linear_inequality > inequalities;
        // Collect all real conditions from the condition from this summand and put them
        // into inequalities.
        for (data_expression_list::const_iterator k=j_r->begin(); k!=j_r->end(); k++)
        {
          inequalities.push_back(linear_inequality(*k,r));
        }

        std::vector < linear_inequality > new_inequalities;
        fourier_motzkin(inequalities,
                        eliminatable_real_sum_variables.begin(),
                        eliminatable_real_sum_variables.end(),
                        new_inequalities,
                        r);
        inequalities.clear();
        remove_redundant_inequalities(new_inequalities,inequalities,r);
        // Save the result in the output expression. 
        data_expression partial_result=*j_n;

        for(const linear_inequality& l: inequalities)
        {
          partial_result=optimized_and(partial_result,l.transform_to_data_expression());       
        }
        result_disjunction_set.insert(partial_result);
      }
      catch (mcrl2::runtime_error &e)
      {
        // Something went wrong, most likely that the inequalities in the input were not linear.
        // Return the original expression.
        vars_out=vars_in;
        e_out=e_in;
      }
    }
  }
  
  e_out=lazy::join_or(result_disjunction_set.begin(),result_disjunction_set.end());
}


/// \brief A unary function that can be used in combination with
/// replace_data_expressions to eliminate real numbers from all
/// quantifiers in an expression.
/// It is adviced to first push the quantifiers inside and
/// apply the one point rule, since that reduces the time spent on
/// the Fourier-Motzkin procedure for large expression.
/// Apply this function innermost first if the expresion contains
/// nested quantifiers.
/// \author Thomas Neele
struct fourier_motzkin_sigma: public std::unary_function<data_expression, data_expression>
{

protected:
  rewriter rewr;

  const data_expression apply(const abstraction& d, bool negate) const
  {
    const variable_list variables = d.variables();
    const data_expression body = negate ? sort_bool::not_(d.body()) : d.body();

    variable_list new_variables;
    data_expression new_body;
    fourier_motzkin(body, variables, new_body, new_variables, rewr);

    const data_expression& result = negate ? 
        static_cast<data_expression>(forall(new_variables, sort_bool::not_(new_body))) :
        static_cast<data_expression>(exists(new_variables, new_body));
    return rewr(result);
  }

public:

  fourier_motzkin_sigma(rewriter rewr_)
  :  rewr(rewr_)
  {}

  const data_expression operator()(const data_expression& d) const
  {
    return is_forall(d) || is_exists(d) ? apply(static_cast<abstraction>(d), is_forall(d)) : d;
  }
};

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_FOURIER_MOTZKIN_H
