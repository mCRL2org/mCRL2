// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/ressolve_numerical.h
/// \brief This contains a gauss-elimination like algorithm to solve a res

#ifndef MCRL2_PRES_RESSOLVE_H
#define MCRL2_PRES_RESSOLVE_H

#include "limits"
#include <cmath>
#include <unordered_map>
#include "mcrl2/data/real_utilities.h"
#include "mcrl2/pres/builder.h" 
#include "mcrl2/pres/pressolve_options.h"
#include "mcrl2/pres/rewriters/enumerate_quantifiers_rewriter.h"



namespace mcrl2::pres_system {

namespace detail {

inline double evaluate(const pres_expression& p, const std::unordered_map<core::identifier_string, double>& solution)
{
// std::cerr << "Evaluate: " << p << "\n";
using value_cache_type = std::unordered_map<data::data_expression, double>;
static value_cache_type value_cache;
if (is_propositional_variable_instantiation(p))
{
  const propositional_variable_instantiation& pv = atermpp::down_cast<propositional_variable_instantiation>(p);
  return solution.at(pv.name());
  }
  else if (is_plus(p))
  {
    // Take care that inf + -inf and -inf + inf yield inf. 
    // Floating points arithmetic gives nan, which is incorrect. 
    const plus& pp = atermpp::down_cast<plus>(p);
    {
      double left=evaluate(pp.left(), solution);
      if (std::isinf(left))
      {
        return left;
      }
      double right=evaluate(pp.right(), solution);
      if (std::isinf(right))
      {
        return right;
      }
      return left+right;
    }
  }
  else if (is_true(p))
  {
    return std::numeric_limits<double>::infinity();
  }
  else if (is_false(p))
  {
    return -std::numeric_limits<double>::infinity();
  }
  else if (is_and(p))
  {
    const and_& pp = atermpp::down_cast<and_>(p);
    return std::min(evaluate(pp.left(), solution), evaluate(pp.right(), solution));
  }
  else if (is_or(p))
  {
    const or_& pp = atermpp::down_cast<or_>(p);
    return std::max(evaluate(pp.left(), solution), evaluate(pp.right(), solution));
  }
  else if (is_const_multiply(p))
  {
    const const_multiply& pp = atermpp::down_cast<const_multiply>(p);
    const data::data_expression& pleft = pp.left();
    const value_cache_type::const_iterator i=value_cache.find(pleft);
    double r;
    if (i==value_cache.end())
    {
      r=data::sort_real::value<double>(pleft);
      value_cache[pleft]=r;
    }
    else 
    {
      r=i->second;
    }
    if (r==0.0)
    {
      return r;
    }
    return r * evaluate(pp.right(), solution);
  }
  else if (data::is_data_expression(p))
  {
    const data::data_expression& pp = atermpp::down_cast<data::data_expression>(p);
    const value_cache_type::const_iterator i=value_cache.find(pp);
    if (i==value_cache.end())
    {
      if (data::sort_real::real_() == pp.sort())
      {
        double r=data::sort_real::value<double>(pp);
        value_cache[pp]=r;
        return r;
      }
      throw mcrl2::runtime_error("Unexpected expression in evaluate: " + data::pp(pp) + ".");
    }
    return i->second;
  }
  throw runtime_error("Unknown term format in evaluate " + pp(p) + ".");
}

} // namespace detail

class ressolve_by_numerical_iteration
{
  protected:
    const pressolve_options m_options;
    const pres& m_input_pres;
    data::rewriter m_datar;    // data_rewriter
    enumerate_quantifiers_rewriter m_R;   // The rewriter.
    
    std::vector<pres_equation> m_equations;
    std::unordered_map<core::identifier_string, double> m_new_solution, m_previous_solution;

    bool stable_solution_found(std::size_t from, std::size_t to)
    {
      double error=0;
      for(std::size_t i=from; i!=to; ++i)
      {
        error = std::max(error,std::abs(m_new_solution[m_equations[i].variable().name()]-m_previous_solution[m_equations[i].variable().name()]));
      }
      mCRL2log(log::debug) << "Current solution: " << std::setprecision(m_options.precision) << detail::evaluate(m_input_pres.initial_state(),m_new_solution) << "   " 
                           << " Difference with previous iteration: " << error << "\n";     
// std::cerr << "Next solution0 " << m_new_solution[m_equations[0].variable().name()] << "\n";
// std::cerr << "Next solution1 " << m_new_solution[m_equations[1].variable().name()] << "\n";
// std::cerr << "Next solution2 " << m_new_solution[m_equations[2].variable().name()] << "\n";
      return error<=pow(0.1,m_options.precision);
    }

    void calculate_new_solution(std::size_t base_equation_index, std::size_t to)
    {
      for(std::size_t j=base_equation_index ; j<to; ++j)
      {
        m_previous_solution[m_equations[j].variable().name()]= m_new_solution[m_equations[j].variable().name()];
      }
      for(std::size_t j=base_equation_index ; j<to; ++j)
      { 
        m_new_solution[m_equations[j].variable().name()] = detail::evaluate(m_equations[j].formula(), m_new_solution);
      }
    }

    void apply_numerical_recursive_algorithm(std::size_t base_equation_index)
    {
      if (base_equation_index<m_equations.size())
      {
// std::cerr << "RECURSIVE ALGORITHM " << base_equation_index << "---------------------------\n";
        std::size_t i=base_equation_index;
        for( ; i<m_equations.size() && m_equations[i].symbol()==m_equations[base_equation_index].symbol() ; ++i)
        {
// std::cerr << "SET " << m_equations[i].variable().name() << "\n";
          const double sol = (m_equations[i].symbol().is_mu()?
                                             -1*std::numeric_limits<double>::infinity():
                                             std::numeric_limits<double>::infinity());
          m_new_solution[m_equations[i].variable().name()] = sol;
//          m_previous_solution[m_equations[i].variable().name()] = sol;
                                                                  
// std::cerr << "INITIAL SOLUTION " << m_equations[i].variable().name() << " := " << m_new_solution[m_equations[i].variable().name()] << "\n";
        }

        apply_numerical_recursive_algorithm(i);
        calculate_new_solution(base_equation_index, i);
 
        if (stable_solution_found(base_equation_index, i))
        {
          return;
        }
        do
        {
          do
          {
            calculate_new_solution(base_equation_index, i);
          } while (!stable_solution_found(base_equation_index, i));
// std::cerr << "HIJERERE\n";
          apply_numerical_recursive_algorithm(i);
          calculate_new_solution(base_equation_index, i);
        } while (!stable_solution_found(base_equation_index, i));
// std::cerr << "RECURSIVE ALGORITHM TERMINATE " << base_equation_index << "---------------------------\n";
      }
    };

    data::rewriter construct_rewriter(const pres& presspec)
    {
      std::set<data::function_symbol> used_functions = pres_system::find_function_symbols(presspec);
      used_functions.insert(data::less(data::sort_real::real_()));
      used_functions.insert(data::sort_real::divides(data::sort_real::real_(),data::sort_real::real_()));
      used_functions.insert(data::sort_real::times(data::sort_real::real_(),data::sort_real::real_()));
      used_functions.insert(data::sort_real::plus(data::sort_real::real_(),data::sort_real::real_()));
      used_functions.insert(data::sort_real::minus(data::sort_real::real_(),data::sort_real::real_()));
      used_functions.insert(data::sort_real::minimum(data::sort_real::real_(),data::sort_real::real_()));
      used_functions.insert(data::sort_real::maximum(data::sort_real::real_(),data::sort_real::real_()));
      return data::rewriter(presspec.data(),
                            data::used_data_equation_selector(presspec.data(), used_functions, presspec.global_variables(), !m_options.remove_unused_rewrite_rules),
                            m_options.rewrite_strategy);
    } 


  public:
    ressolve_by_numerical_iteration(
      const pressolve_options& options,
      const pres& input_pres
    ) 
     : m_options(options),
       m_input_pres(input_pres),
       m_datar(construct_rewriter(input_pres)),
       m_R(m_datar,input_pres.data())
    {}

    double run()
    {
      for(const pres_equation& eq: m_input_pres.equations())
      {
        m_equations.emplace_back(eq.symbol(), eq.variable(), m_R(eq.formula()));
      }
      apply_numerical_recursive_algorithm(0);

      double solution = detail::evaluate(m_input_pres.initial_state(),m_new_solution);
      // mCRL2log(log::info) << "Solution " << solution << "\n";
      return solution;
    }
};

} // namespace mcrl2::pres_system



#endif // MCRL2_PRES_RESSOLVE_NUMERICAL_H
