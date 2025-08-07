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

#ifndef MCRL2_PRES_RESSOLVE_DIRECTED_H
#define MCRL2_PRES_RESSOLVE_DIRECTED_H

#include "limits"
#include <cmath>
#include <unordered_map>
#include "mcrl2/data/real_utilities.h"
#include "mcrl2/pres/builder.h" 
#include "mcrl2/pres/pressolve_options.h"
#include "mcrl2/pres/rewriters/enumerate_quantifiers_rewriter.h"



namespace mcrl2::pres_system {

namespace detail {

// We introduce a user defined class for res's in which real constants are
// stored as double's and variables are stored by their index. 
//
// The values of true and false are represented by the real constants -infinity and infinity.
enum internal_res_expression_type
{
  propositional_variable,
  real_constant,
  plus,
  and_,
  or_,
  const_multiply
};

class internal_res_expression
{
  public:
    virtual internal_res_expression_type type() const =0;
    virtual std::size_t index() const
    {
      throw mcrl2::runtime_error("This object has no index.");
    }

    virtual double value() const
    {
      throw mcrl2::runtime_error("This object has no value.");
    }

    virtual const internal_res_expression* left() const
    {
      throw mcrl2::runtime_error("This object has no left argument.");
    }

    virtual const internal_res_expression* right() const
    {
      throw mcrl2::runtime_error("This object has no right argument.");
    }

    virtual ~internal_res_expression() = default;

};

class internal_propositional_variable: public virtual internal_res_expression
{
  const std::size_t m_index;
  public:
    internal_propositional_variable(std::size_t index)
      : m_index(index)
    {}

    internal_res_expression_type type() const override
    {
      return internal_res_expression_type::propositional_variable;
    }

    std::size_t index() const override
    {
      return m_index;
    }

    ~internal_propositional_variable() override = default;
};

class internal_real_constant: public virtual internal_res_expression
{
  const double m_value;

  public:
    // constructor
    internal_real_constant(const double value)
      : m_value(value)
    {}


    internal_res_expression_type type() const override
    {
      return internal_res_expression_type::real_constant;
    }

    double value() const override
    {
      return m_value;
    }

    ~internal_real_constant() override = default;
};

class internal_plus: public virtual internal_res_expression
{
  const internal_res_expression* m_left_argument;
  const internal_res_expression* m_right_argument;
   
  public:
    // Constructor.
    internal_plus(const internal_res_expression& left_argument, const internal_res_expression& right_argument)
      : m_left_argument(&left_argument),
        m_right_argument(&right_argument)
    {}

    internal_res_expression_type type() const override
    {
      return internal_res_expression_type::plus;
    }

    const internal_res_expression* left() const override
    {
      return m_left_argument;
    }

    const internal_res_expression* right() const override
    {
      return m_right_argument;
    }

    ~internal_plus() override = default;
};

class internal_and: public virtual internal_res_expression
{
  const internal_res_expression* m_left_argument;
  const internal_res_expression* m_right_argument;
   
  public:
    // Constructor.
    internal_and(const internal_res_expression& left_argument, const internal_res_expression& right_argument)
      : m_left_argument(&left_argument),
        m_right_argument(&right_argument)
    {}

    internal_res_expression_type type() const override
    {
      return internal_res_expression_type::and_;
    }

    const internal_res_expression* left() const override
    {
      return m_left_argument;
    }

    const internal_res_expression* right() const override
    {
      return m_right_argument;
    }

    ~internal_and() override = default;
};

class internal_or: public virtual internal_res_expression
{
  const internal_res_expression* m_left_argument;
  const internal_res_expression* m_right_argument;
   
  public:
    // Constructor.
    internal_or(const internal_res_expression& left_argument, const internal_res_expression& right_argument)
      : m_left_argument(&left_argument),
        m_right_argument(&right_argument)
    {}

    internal_res_expression_type type() const override
    {
      return internal_res_expression_type::or_;
    }

    const internal_res_expression* left() const override
    {
      return m_left_argument;
    }

    const internal_res_expression* right() const override
    {
      return m_right_argument;
    }

    ~internal_or() override = default;
};

class internal_const_multiply: public virtual internal_res_expression
{
  const double m_constant;
  const internal_res_expression* m_right_argument;
   
  public:
    // Constructor.
    internal_const_multiply(const double constant, const internal_res_expression& right_argument)
      : m_constant(constant),
        m_right_argument(&right_argument)
    {}

    internal_res_expression_type type() const override
    {
      return internal_res_expression_type::const_multiply;
    }

    double value() const override
    {
      return m_constant;
    }

    const internal_res_expression* right() const override
    {
      return m_right_argument;
    }

    ~internal_const_multiply() override = default;
};

// End definition of internal_res_expression.

inline double evaluate_directed(const internal_res_expression* p, const std::vector<double>& solution)
{
  switch (p->type())
  {
    case internal_res_expression_type::propositional_variable:
    {
      return solution[p->index()];
    }
    case internal_res_expression_type::real_constant:
    {
      return p->value();
    }
    case internal_res_expression_type::plus:
    {
      // Take care that inf + -inf and -inf + inf yield inf. 
      // Floating points arithmetic gives nan, which is incorrect. 
      double left=evaluate_directed(p->left(), solution);
      if (std::isinf(left))
      {
        return left;
      }
      double right=evaluate_directed(p->right(), solution);
      if (std::isinf(right))
      {
        return right;
      }
      return left+right;
    }
    case internal_res_expression_type::and_:
    {
      return std::min(evaluate_directed(p->left(), solution), evaluate_directed(p->right(), solution));
    }
    case internal_res_expression_type::or_:
    {
      return std::max(evaluate_directed(p->left(), solution), evaluate_directed(p->right(), solution));
    }
    case internal_res_expression_type::const_multiply:
    {
      return p->value() * evaluate_directed(p->right(), solution);
    }
  }
  assert(0);
  return 0;
}

} // namespace detail

class ressolve_by_numerical_iteration_directed
{
  protected:
    const pressolve_options m_options;
    const pres& m_input_pres;
    // data::rewriter m_datar;    // data_rewriter
    // enumerate_quantifiers_rewriter m_R;   // The rewriter.
    
    std::vector<const detail::internal_res_expression*> m_equations;
    std::vector<std::size_t> m_rank;
    // The data structure below indicates for a variable index in which equations indicated it occurs. 
    std::vector<std::set<std::size_t>> variable_occurrences;

    std::deque<detail::internal_propositional_variable> propositional_variables;
    std::deque<detail::internal_real_constant> real_constants;
    std::deque<detail::internal_plus> plus_terms;
    std::deque<detail::internal_and> and_terms;
    std::deque<detail::internal_or> or_terms;
    std::deque<detail::internal_const_multiply> const_multiply_terms;

    using pres_expressions_hash_map = std::unordered_map<pres_expression, detail::internal_res_expression*>;
    pres_expressions_hash_map translated_pres_expressions;
    
    std::vector<double> m_new_solution, m_previous_solution;

    bool stable_solution_found(std::size_t from, std::size_t to)
    {
      double error=0;
      for(std::size_t i=from; i!=to; ++i)
      {
        error = std::max(error,std::abs(m_new_solution[i]-m_previous_solution[i]));
      }
      mCRL2log(log::debug) << "Current solution: " << std::setprecision(m_options.precision) 
                           << detail::evaluate_directed(translated_pres_expressions[m_input_pres.initial_state()],m_new_solution) << "   " 
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
        m_previous_solution[j]= m_new_solution[j];
      }
      for(std::size_t j=base_equation_index ; j<to; ++j)
      { 
        m_new_solution[j] = detail::evaluate_directed(m_equations[j], m_new_solution);
      }
    }

    void apply_numerical_recursive_algorithm_directed(std::size_t base_equation_index)
    {
      if (base_equation_index<m_equations.size())
      {
// std::cerr << "RECURSIVE ALGORITHM " << base_equation_index << "---------------------------\n";
        std::size_t i=base_equation_index;
        std::size_t current_rank=m_rank[base_equation_index];
        const bool current_fixed_point_is_mu = (current_rank % 2)==0; // even ranks represent mu's.
        for( ; i<m_equations.size() && m_rank[i]==current_rank ; ++i)
        {
// std::cerr << "SET " << m_equations[i].variable().name() << "\n";
          const double sol = (current_fixed_point_is_mu?
                                             -1*std::numeric_limits<double>::infinity():
                                             std::numeric_limits<double>::infinity());
          m_new_solution[i] = sol;
//          m_previous_solution[m_equations[i].variable().name()] = sol;
                                                                  
// std::cerr << "INITIAL SOLUTION " << m_equations[i].variable().name() << " := " << m_new_solution[m_equations[i].variable().name()] << "\n";
        }

        apply_numerical_recursive_algorithm_directed(i);
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
          apply_numerical_recursive_algorithm_directed(i);
          calculate_new_solution(base_equation_index, i);
        } while (!stable_solution_found(base_equation_index, i));
// std::cerr << "RECURSIVE ALGORITHM TERMINATE " << base_equation_index << "---------------------------\n";
      }
    }; 

    const detail::internal_res_expression& translate_to_internal_res_expression(const pres_expression& p)
    {
      // First check whether this expression is already translated. 
      const pres_expressions_hash_map::const_iterator i=translated_pres_expressions.find(p);
      if (i!=translated_pres_expressions.end()) // found;
      {
        return *(i->second);
      }

      // Else the expression has not yet been translated. Note that variables, true and false are translated always initially.
      assert(!(is_propositional_variable_instantiation(p) || is_true(p) || is_false(p)));
      if (is_plus(p))
      {
        const plus& pp = atermpp::down_cast<plus>(p);
        const detail::internal_res_expression& left_argument=translate_to_internal_res_expression(pp.left());
        const detail::internal_res_expression& right_argument=translate_to_internal_res_expression(pp.right());
        plus_terms.emplace_back(left_argument,right_argument);
        translated_pres_expressions.emplace(p,&plus_terms.back());
        return plus_terms.back();
      }
      else if (is_and(p))
      {
        const and_& pp = atermpp::down_cast<and_>(p);
        const detail::internal_res_expression& left_argument=translate_to_internal_res_expression(pp.left());
        const detail::internal_res_expression& right_argument=translate_to_internal_res_expression(pp.right());
        and_terms.emplace_back(left_argument,right_argument);
        translated_pres_expressions.emplace(p,&and_terms.back());
        return and_terms.back();
      }
      else if (is_or(p))
      {
        const or_& pp = atermpp::down_cast<or_>(p);
        const detail::internal_res_expression& left_argument=translate_to_internal_res_expression(pp.left());
        const detail::internal_res_expression& right_argument=translate_to_internal_res_expression(pp.right());
        or_terms.emplace_back(left_argument,right_argument);
        translated_pres_expressions.emplace(p,&or_terms.back());
        return or_terms.back();
      }
      else if (is_const_multiply(p))
      {
        const const_multiply& pp = atermpp::down_cast<const_multiply>(p);
        const double left_argument=data::sort_real::value<double>(pp.left());
        assert(left_argument>0);
        const detail::internal_res_expression& right_argument=translate_to_internal_res_expression(pp.right());
        const_multiply_terms.emplace_back(left_argument,right_argument);
        translated_pres_expressions.emplace(p,&const_multiply_terms.back());
        return const_multiply_terms.back();
      }
      else if (data::is_data_expression(p))
      {
        const data::data_expression& pp = atermpp::down_cast<data::data_expression>(p);
        if (data::sort_real::real_() == pp.sort())
        {
          double r=data::sort_real::value<double>(pp);
          real_constants.emplace_back(r);
          translated_pres_expressions.emplace(p,&real_constants.back());
          return real_constants.back();
        }
      }
      throw runtime_error("Unknown term format in evaluate directed " + pp(p) + ".");
    }

    void register_variable_occurrences(const detail::internal_res_expression* p, const std::size_t current_equation_index)
    {
      switch (p->type())
      {                                          
        case detail::internal_res_expression_type::propositional_variable:
        {     
          variable_occurrences[current_equation_index].insert(p->index());
          return;
        }                                                             
        case detail::internal_res_expression_type::real_constant:
        {   
          return;
        }   
        case detail::internal_res_expression_type::plus:
        case detail::internal_res_expression_type::and_:
        case detail::internal_res_expression_type::or_:
        {   
          register_variable_occurrences(p->left(), current_equation_index);
          register_variable_occurrences(p->right(), current_equation_index);
          return;
        }   
        case detail::internal_res_expression_type::const_multiply:
        {     
          register_variable_occurrences(p->right(), current_equation_index);
          return;
        }
      }
    }


    // Translate the pres equations to internal data structures such that
    // numbers do not need to be evaluated repeatedly. The internal data structure is
    // maximally shared.  
    void translate_input_pres_to_internal_data_structures()
    {
      // First generate the translations for true and false.
      real_constants.emplace_back(std::numeric_limits<double>::infinity());
      translated_pres_expressions[true_()]=&real_constants.back();
      real_constants.emplace_back(-std::numeric_limits<double>::infinity());
      translated_pres_expressions[false_()]=&real_constants.back();
      // Next translate the variables.
      bool is_mu=true;
      std::size_t rank=0;
      for(const pres_equation& eq: m_input_pres.equations())
      {
        if (eq.symbol().is_mu() != is_mu)
        {
          rank++;
          is_mu=!is_mu;
        }
        m_rank.push_back(rank);
        propositional_variables.emplace_back(propositional_variables.size());
        translated_pres_expressions[propositional_variable_instantiation(eq.variable().name(),data::data_expression_list())]=&propositional_variables.back();
      }
      // Finally translate all rhs's of the equations. 
      for(const pres_equation& eq: m_input_pres.equations())
      {
        const detail::internal_res_expression& res_exp=translate_to_internal_res_expression(eq.formula());
        m_equations.emplace_back(&res_exp);
      }
    }

  public:
    ressolve_by_numerical_iteration_directed(
             const pressolve_options& options,
             const pres& input_pres
    ) 
     : m_options(options),
       m_input_pres(input_pres),
       variable_occurrences(m_input_pres.equations().size()),
       m_new_solution(m_input_pres.equations().size()),
       m_previous_solution(m_input_pres.equations().size())
    {}

    double run()
    {
      translate_input_pres_to_internal_data_structures();
      apply_numerical_recursive_algorithm_directed(0);

      double solution = detail::evaluate_directed(translated_pres_expressions[m_input_pres.initial_state()],m_new_solution);
      return solution;
    }
};

} // namespace mcrl2::pres_system



#endif // MCRL2_PRES_RESSOLVE_NUMERICAL_DIRECTED_H
