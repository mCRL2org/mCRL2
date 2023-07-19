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
#include "mcrl2/data/real_utilities.h"
#include "mcrl2/pres/builder.h" 

namespace mcrl2 {

namespace pres_system {

namespace detail {


double evaluate(const pres_expression& p, const std::unordered_map<core::identifier_string, double>& solution)
{
  if (is_propositional_variable_instantiation(p))
  {
    const propositional_variable_instantiation& pv = atermpp::down_cast<propositional_variable_instantiation>(p);
    return solution.at(pv.name());
  }
  else if (is_plus(p))
  {
    const plus& pp = atermpp::down_cast<plus>(p);
    {
      return evaluate(pp.left(), solution) + evaluate(pp.right(), solution);
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
  else if (data::is_data_expression(p))
  {
    const data::data_expression& pp = atermpp::down_cast<data::data_expression>(p);
    if (data::sort_real::real_() == pp.sort())
    {
      return data::sort_real::value(pp);
    }
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
    return data::sort_real::value(pp.left()) * evaluate(pp.right(), solution);
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
std::cerr << " ERROR " << error << "\n";     
std::cerr << "Current solution " << detail::evaluate(m_input_pres.initial_state(),m_new_solution) << "\n";
std::cerr << "Next solution0 " << m_new_solution[m_equations[0].variable().name()] << "\n";
std::cerr << "Next solution1 " << m_new_solution[m_equations[1].variable().name()] << "\n";
std::cerr << "Next solution2 " << m_new_solution[m_equations[2].variable().name()] << "\n";
      return error<0.0000001;
    }

    void calculate_new_solution(std::size_t base_equation_index, std::size_t to)
    {
      for(std::size_t j=base_equation_index ; j<to; ++j)
      {
        m_previous_solution[m_equations[j].variable().name()]= m_new_solution[m_equations[j].variable().name()];
      }
      for(std::size_t j=base_equation_index ; j<to; ++j)
      { 
        m_new_solution[m_equations[j].variable().name()] = detail::evaluate(m_equations[j].formula(), m_previous_solution);
// std::cerr << "INTERMEDIATE SOLUTION " << m_equations[j].variable().name() << " := " << m_new_solution[m_equations[j].variable().name()] << "\n";
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
          m_new_solution[m_equations[i].variable().name()] = (m_equations[i].symbol().is_mu()?
                                                                    -1*std::numeric_limits<double>::infinity():
                                                                    std::numeric_limits<double>::infinity());
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
      if (m_options.remove_unused_rewrite_rules)
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
                              data::used_data_equation_selector(presspec.data(), used_functions, presspec.global_variables()),
                              m_options.rewrite_strategy);
      }
      else
      {
        return data::rewriter(presspec.data(), m_options.rewrite_strategy);
      }
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
      mCRL2log(log::info) << "Solution " << solution << "\n";
      return solution;
    }
};

/* /// \brief An algorithm that takes a res, i.e. a pres with propositional variables without parameters
///        and solves it by Gauss elimination.

class ressolve_by_gauss_elimination_algorithm
{
  protected:
    pressolve_options m_options;
    pres m_input_pres;
    enumerate_quantifiers_rewriter m_R;   // The rewriter.

    data::rewriter construct_rewriter(const pres& presspec)
    {
      if (m_options.remove_unused_rewrite_rules)
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
                              data::used_data_equation_selector(presspec.data(), used_functions, presspec.global_variables()),
                              m_options.rewrite_strategy);
      }
      else
      {
        return data::rewriter(presspec.data(), m_options.rewrite_strategy);
      }
    }

  public:
    ressolve_by_gauss_elimination_algorithm(
      const pressolve_options& options,
      const pres& input_pres
    ) 
     : m_options(options),
       m_datar(construct_rewriter(input_pres)),
       m_input_pres(input_pres),
       m_R(m_datar,input_pres.data())
    {}

    const pres_expression run()
    {
      std::vector<pres_equation> res_equations(m_input_pres.equations().begin(), m_input_pres.equations().end());
      assert(res_equations.size()>0);
      // Take care that the first equation has the initial variable at the lhs.
      if (res_equations.front().variable().name()!=m_input_pres.initial_state().name())
      {
        core::identifier_string new_name("initial_variable$$"); // Name should not be seen externally.
        res_equations.insert(res_equations.begin(),
                             pres_equation(res_equations.front().symbol(),
                                           propositional_variable(new_name, data::variable_list()),
                                           m_input_pres.initial_state()));
         m_input_pres.initial_state() = propositional_variable_instantiation(new_name, data::data_expression_list());
      }

      atermpp::indexed_set<core::identifier_string> variable_names;
      pres_expression result;
      res_conjunctive_disjunctive_normal_form_builder conjunctive_normal_form_builder(true);
      res_conjunctive_disjunctive_normal_form_builder disjunctive_normal_form_builder(false);
      for(pres_equation& e: res_equations)
      {
        variable_names.insert(e.variable().name());
      }

      for(std::vector<pres_equation>::reverse_iterator equation_it=res_equations.rbegin(); equation_it!=res_equations.rend(); equation_it++)
      {
        mCRL2log(log::debug) << "Solving    " << equation_it->symbol() << " " << equation_it->variable() << " = " << equation_it->formula() << "\n";
        if (equation_it->symbol().is_mu())
        {
// std::cerr << "IN1 " << equation_it->formula() << "\n";
// std::cerr << "IN2 " << m_R(equation_it->formula()) << "\n";
          conjunctive_normal_form_builder.apply(result, m_R(equation_it->formula()));
// std::cerr << "IN3 " << result << "\n";
          result=detail::group_sums_conjuncts_disjuncts(result, m_datar);
// std::cerr << "OUT " << result << "\n";
        }
        else
        {
// std::cerr << "INX1 " << equation_it->formula() << "\n";
// std::cerr << "INX2 " << m_R(equation_it->formula()) << "\n";
          disjunctive_normal_form_builder.apply(result, m_R(equation_it->formula()));
// std::cerr << "INX3 " << result << "\n";
          result=detail::group_sums_conjuncts_disjuncts(result, m_datar);
// std::cerr << "OUTX " << result << "\n";
        }
        mCRL2log(log::debug) << "Norm. Form " << equation_it->symbol() << " " << equation_it->variable() << " = " << result << "\n";  

        pres_expression solution = detail::solve_single_equation(equation_it->symbol(),
                                                                 equation_it->variable(),
                                                                 result,
                                                                 m_datar);
        equation_it->formula() = solution;
        mCRL2log(log::debug) << "Solution   " << equation_it->symbol() << " " << equation_it->variable() << " = " << equation_it->formula() << "\n";

        substitute_pres_equation_builder substitute_pres_equation(equation_it->variable(), solution);
       
        for(std::vector<pres_equation>::iterator substitution_equation_it=res_equations.begin();
                                                 substitution_equation_it!=equation_it.base(); 
                                                 substitution_equation_it++)
        {
std::cerr << ".";
          substitute_pres_equation.apply(result, substitution_equation_it->formula());
          if (substitution_equation_it->formula() != result)
          {
// std::cerr << "ZZZZZIN  " << equation_it->symbol() << "   " << substitution_equation_it->formula() << "     " << result << "\n";
            if (equation_it->symbol().is_mu())
            {
              conjunctive_normal_form_builder.apply(substitution_equation_it->formula(), result);
            }
            else
            {
              disjunctive_normal_form_builder.apply(substitution_equation_it->formula(), result);
            }
// std::cerr << "ZZZZZMID " << substitution_equation_it->formula() << "\n";
            substitution_equation_it->formula() = detail::group_sums_conjuncts_disjuncts(substitution_equation_it->formula(), m_datar);
// std::cerr << "ZZZZZOUT " << substitution_equation_it->formula() << "\n";
          }
        }
        mCRL2log(log::debug) << "Substituted the solution backwards.\n";
      }
      return m_R(res_equations.front().formula());
    } 
}; */

} // namespace pres_system

} // namespace mcrl2

#endif // MCRL2_PRES_RESSOLVE_NUMERICAL_H
