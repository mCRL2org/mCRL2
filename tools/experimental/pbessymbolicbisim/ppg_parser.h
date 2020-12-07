// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/ppg_visitor.h
/// \brief Visitor class for Parameterised Parity Games (PPG), PBES expressions of the form:
/// PPG :== /\_{i: I} f_i && /\_{j: J} forall v: D_j . ( g_j => X_j(e_j) )
///       | \/_{i: I} f_i || \/_{j: J} exists v: D_j . ( g_j && X_j(e_j) ).

#ifndef MCRL2_PBESSYMBOLICBISIM_PPG_PARSER_H
#define MCRL2_PBESSYMBOLICBISIM_PPG_PARSER_H

#include "mcrl2/data/fourier_motzkin.h"
#include "mcrl2/pbes/detail/ppg_rewriter.h"
#include "mcrl2/pbes/detail/ppg_traverser.h"
#include "mcrl2/pbes/replace.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/rewriters/pbes2data_rewriter.h"
#include "mcrl2/pbes/rewriters/quantifiers_inside_rewriter.h"

namespace mcrl2
{
namespace pbes_system
{
namespace detail
{

class ppg_summand;
class ppg_equation;
class ppg_pbes;
std::string pp(const ppg_summand& summ, bool is_conjunctive);
std::string pp(const ppg_equation& eq);
std::string pp(const ppg_pbes& x);
inline std::ostream& operator<<(std::ostream& out, const ppg_equation& x);
inline std::ostream& operator<<(std::ostream& out, const ppg_pbes& x);

class ppg_summand
{
protected:
  data::variable_list m_quantification_domain;
  data::data_expression m_condition;
  propositional_variable_instantiation m_new_state;

public:
  ppg_summand() = default;

  /**
   * \brief Create a summand in the shape true => X
   */
  explicit ppg_summand(const std::string& name)
  : m_condition(data::sort_bool::true_())
  , m_new_state(name, data::data_expression_list())
  {}

  explicit ppg_summand(const pbes_expression& e)
  {
    pbes_expression expr = e;
    if(is_exists(expr) || is_forall(expr))
    {
      m_quantification_domain = accessors::var(expr);
      expr = accessors::arg(expr);
    }
    if(is_or(expr))
    {
      pbes_expression cond = false_();
      for(const pbes_expression& disj: split_disjuncts(expr))
      {
        if(is_simple_expression(disj))
        {
          cond = optimized_or(cond, disj);
        }
        else
        {
          assert(is_or(expr));
          expr = disj;
        }
      }
      m_condition = data::sort_bool::not_(atermpp::down_cast<data::data_expression>(pbes2data(cond)));
    }
    else if(is_and(expr))
    {
      pbes_expression cond = true_();
      for(const pbes_expression& conj: split_conjuncts(expr))
      {
        if(is_simple_expression(conj))
        {
          cond = optimized_and(cond, conj);
        }
        else
        {
          assert(is_and(expr));
          expr = conj;
        }
      }
      m_condition = atermpp::down_cast<data::data_expression>(pbes2data(cond));
    }
    else
    {
      m_condition = data::sort_bool::true_();
    }
    // Quantifier may occur just before predicate variable instance due to use
    // of quantifier-inside rewriter.
    if(is_exists(expr) || is_forall(expr))
    {
      for(const data::variable& v: accessors::var(expr)) {
        m_quantification_domain.push_front(v);
      }
      expr = accessors::arg(expr);
    }
    assert(is_propositional_variable_instantiation(expr));
    m_new_state = atermpp::down_cast<propositional_variable_instantiation>(expr);
  }

  ppg_summand(const data::variable_list& quantification_domain, const data::data_expression& condition, const propositional_variable_instantiation& new_state)
  : m_quantification_domain(quantification_domain)
  , m_condition(condition)
  , m_new_state(new_state)
  {}

  const data::variable_list& quantification_domain() const
  {
    return m_quantification_domain;
  }

  const data::data_expression& condition() const
  {
    return m_condition;
  }

  const propositional_variable_instantiation& new_state() const
  {
    return m_new_state;
  }

  const ppg_summand simplify(const data::rewriter& rewr) const
  {
    ppg_summand result;
    result.m_quantification_domain = m_quantification_domain;
    result.m_condition = rewr(data::replace_data_expressions(m_condition, data::fourier_motzkin_sigma(rewr), true));
    result.m_new_state = m_new_state;
    return result;
  }

  bool operator<(const ppg_summand& summ) const
  {
    return m_quantification_domain <  summ.m_quantification_domain ||
           (m_quantification_domain == summ.m_quantification_domain && m_condition <  summ.m_condition) ||
           (m_quantification_domain == summ.m_quantification_domain && m_condition == summ.m_condition && m_new_state < summ.m_new_state);
  }
};

class ppg_equation
{
protected:
  mcrl2::pbes_system::fixpoint_symbol m_symbol;
  propositional_variable m_var;
  std::vector<ppg_summand> m_summands;
  bool m_is_conjunctive;

public:

  ppg_equation() = default;

  /**
   * \brief Constructor to create a ppg_equation in the shape \sigma X = X
   * \details Useful for equations such as nu X_true = X_true
   */
  ppg_equation(const std::string& name, const fixpoint_symbol& symbol, bool is_conjunctive)
  : m_symbol(symbol)
  , m_var(propositional_variable(name, data::variable_list()))
  , m_is_conjunctive(is_conjunctive)
  {
    m_summands.emplace_back(name);
  }

  ppg_equation(const pbes_equation& eq,
              const std::string& x_false_conj_name, const std::string& x_true_conj_name,
              const std::string& x_false_disj_name, const std::string& x_true_disj_name)
  : m_symbol(eq.symbol())
  , m_var(eq.variable())
  {
    const pbes_expression& rhs = eq.formula();
    data::data_expression simple_formula;
    if(is_simple_expression(rhs))
    {
      // Set the equation to disjunctive to prevent negation of simple_formula
      m_is_conjunctive = false;
      simple_formula = atermpp::down_cast<data::data_expression>(pbes2data(rhs));
    }
    else if(is_and(rhs))
    {
      m_is_conjunctive = true;
      std::set<pbes_expression> conjuncts = split_and(rhs);
      simple_formula = data::sort_bool::true_();
      for(const pbes_expression& expr: conjuncts)
      {
        if(is_simple_expression(expr))
        {
          data::data_expression data_expr = atermpp::down_cast<data::data_expression>(pbes2data(expr));
          simple_formula = optimized_and(simple_formula, data_expr);
        }
        else
        {
          m_summands.emplace_back(expr);
        }
      }
    }
    else if(is_forall(rhs))
    {
      m_is_conjunctive = true;
      simple_formula = data::sort_bool::true_();
      m_summands.emplace_back(rhs);
    }
    else if(is_or(rhs))
    {
      m_is_conjunctive = false;
      std::set<pbes_expression> disjuncts = split_or(rhs);
      simple_formula = data::sort_bool::false_();
      for(const pbes_expression& expr: disjuncts)
      {
        if(is_simple_expression(expr))
        {
          data::data_expression data_expr = atermpp::down_cast<data::data_expression>(pbes2data(expr));
          simple_formula = optimized_or(simple_formula, data_expr);
        }
        else
        {
          m_summands.emplace_back(expr);
        }
      }
    }
    else if(is_exists(rhs))
    {
      m_is_conjunctive = false;
      simple_formula = data::sort_bool::false_();
      m_summands.emplace_back(rhs);
    }
    else if(is_propositional_variable_instantiation(rhs))
    {
      m_is_conjunctive = false;
      m_summands.emplace_back(data::variable_list(), data::sort_bool::true_(), propositional_variable_instantiation(rhs));
      // We already have a summand that's always enabled, so we don't need a summand with X_true or X_false
      return;
    }

    // Build a summand using the simple formula
    if(!(( m_is_conjunctive && data::sort_bool::true_() == simple_formula) ||
         (!m_is_conjunctive && data::sort_bool::false_() == simple_formula) ||
         simple_formula == data::data_expression()))
    {
      if(m_is_conjunctive)
      {
        simple_formula = data::sort_bool::not_(simple_formula);
      }
      propositional_variable_instantiation next_state(m_is_conjunctive ? x_false_conj_name : x_true_disj_name, data::data_expression_list());
      m_summands.emplace_back(data::variable_list(), simple_formula, next_state);
    }
    // Add a summand for X_true or X_false to ensure the underlying BES is in SRF
    // This extra summand does not change the solution
    m_summands.emplace_back(
        data::variable_list(),
        data::sort_bool::true_(),
        propositional_variable_instantiation(m_is_conjunctive ? x_true_conj_name : x_false_disj_name, data::data_expression_list()));
  }

  const fixpoint_symbol& symbol() const
  {
    return m_symbol;
  }

  const propositional_variable& variable() const
  {
    return m_var;
  }

  const std::vector<ppg_summand>& summands() const
  {
    return m_summands;
  }

  const bool& is_conjunctive() const
  {
    return m_is_conjunctive;
  }

  const ppg_equation simplify(const data::rewriter& rewr) const
  {
    ppg_equation result;
    result.m_symbol = m_symbol;
    result.m_var = m_var;
    result.m_is_conjunctive = m_is_conjunctive;
    for(const ppg_summand& summ: m_summands)
    {
      ppg_summand new_summ = summ.simplify(rewr);
      if(!is_false(new_summ.condition()))
      {
        result.m_summands.push_back(new_summ);
      }
    }
    return result;
  }

  bool operator<(const ppg_equation& eq) const
  {
    return m_symbol <  eq.m_symbol ||
           (m_symbol == eq.m_symbol && m_var <  eq.m_var) ||
           (m_symbol == eq.m_symbol && m_var == eq.m_var && m_is_conjunctive < eq.m_is_conjunctive) ||
           (m_symbol == eq.m_symbol && m_var == eq.m_var && m_is_conjunctive == eq.m_is_conjunctive && m_summands < eq.m_summands);
  }
};

class ppg_pbes
{
protected:
  std::vector<ppg_equation> m_equations;
  propositional_variable_instantiation m_initial_state;
  data::data_specification m_data;
  std::set<data::variable> m_global_variables;

public:
  ppg_pbes() = default;

  explicit ppg_pbes(const pbes& p)
  : m_initial_state(p.initial_state())
  , m_data(p.data())
  , m_global_variables(p.global_variables())
  {
    pbes q = p;
    if(!is_ppg(p))
    {
      q = to_ppg(p);
      assert(is_ppg(q));
    }
    // Apply the quantifier inside rewriter since the ppg rewriter may
    // leave nested quantification, which this parser doesn't
    // deal with.
    replace_pbes_expressions(q, quantifiers_inside_rewriter(), false);

    data::set_identifier_generator id_gen;
    for(auto& eq: p.equations())
    {
      id_gen.add_identifier(eq.variable().name());
    }

    // Group equations by fixpoint symbol. Each mu group gets its
    // own instances of X_false. Each nu group gets its own instances
    // of X_true.
    fixpoint_symbol previous_symbol(q.equations()[0].symbol());
    int rank = previous_symbol == fixpoint_symbol::nu() ? 0 : 1;

    const std::string x_true_name = id_gen("X_true");
    const std::string x_false_name = id_gen("X_false");

    std::string x_false_conj_name;
    std::string x_true_conj_name;
    std::string x_false_disj_name;
    std::string x_true_disj_name;
    if(previous_symbol == fixpoint_symbol::mu())
    {
      // For a mu block we can only use local X_false
      x_false_conj_name = id_gen("X_false_conj");
      x_true_conj_name = x_true_name;
      x_false_disj_name = id_gen("X_false_disj");
      x_true_disj_name = x_true_name;
    }
    else
    {
      // For a nu block we can only use local X_true
      x_false_conj_name = x_false_name;
      x_true_conj_name = id_gen("X_true_conj");
      x_false_disj_name = x_false_name;
      x_true_disj_name = id_gen("X_true_disj");
    }
    for(const pbes_equation& eq: q.equations())
    {
      if(eq.symbol() != previous_symbol)
      {
        // Add the equations for the previous block
        // X_false for a mu block
        // X_true for a nu block
        m_equations.emplace_back(previous_symbol == fixpoint_symbol::mu() ? x_false_conj_name : x_true_conj_name, previous_symbol, true);
        m_equations.emplace_back(previous_symbol == fixpoint_symbol::mu() ? x_false_disj_name : x_true_disj_name, previous_symbol, false);

        // Update for the new block
        rank++;
        previous_symbol = eq.symbol();
        if(previous_symbol == fixpoint_symbol::mu())
        {
          // For a mu block we can only use local X_false
          x_false_conj_name = id_gen("X_false_conj");
          x_true_conj_name = x_true_name;
          x_false_disj_name = id_gen("X_false_disj");
          x_true_disj_name = x_true_name;
        }
        else
        {
          // For a nu block we can only use local X_true
          x_false_conj_name = x_false_name;
          x_true_conj_name = id_gen("X_true_conj");
          x_false_disj_name = x_false_name;
          x_true_disj_name = id_gen("X_true_disj");
        }
      }
      m_equations.emplace_back(eq, x_false_conj_name, x_true_conj_name, x_false_disj_name, x_true_disj_name);
    }
    // Add the final set of equations for X_true and X_false
    m_equations.emplace_back(previous_symbol == fixpoint_symbol::mu() ? x_false_conj_name : x_true_conj_name, previous_symbol, true);
    m_equations.emplace_back(previous_symbol == fixpoint_symbol::mu() ? x_false_disj_name : x_true_disj_name, previous_symbol, false);

    // Add global equations for X_true and X_false that are used throughout the
    // PBES
    m_equations.emplace_back(x_false_name, fixpoint_symbol::mu(), true);
    m_equations.emplace_back(x_true_name, fixpoint_symbol::nu(), true);
  }

  const std::vector<ppg_equation>& equations() const
  {
    return m_equations;
  }

  const propositional_variable_instantiation& initial_state() const
  {
    return m_initial_state;
  }

  const data::data_specification& data() const
  {
    return m_data;
  }

  const std::set<data::variable>& global_variables() const
  {
    return m_global_variables;
  }

  const ppg_pbes simplify(const data::rewriter& rewr) const
  {
    ppg_pbes result;
    result.m_initial_state = m_initial_state;
    result.m_data = m_data;
    result.m_global_variables = m_global_variables;
    for(const ppg_equation& eq: m_equations)
    {
      result.m_equations.push_back(eq.simplify(rewr));
    }
    return result;
  }
};

std::string pp(const ppg_summand& summ, bool is_conjunctive)
{
  std::string connecting_operator = is_conjunctive ? "=>" : "&&";
  std::string quantifier = is_conjunctive ? "forall" : "exists";
  std::ostringstream out;
  out << "       ";
  if(!summ.quantification_domain().empty())
  {
    out << quantifier << " ";
    bool print_delimiter = false;
    for(const data::variable& v: summ.quantification_domain())
    {
      if(print_delimiter)
      {
        out << ", ";
      }
      else
      {
        print_delimiter = true;
      }
      out << v.name() << ": " << v.sort();
    }
    out << " . ";
  }
  if(!is_true(summ.condition()))
  {
    out << "val(" << summ.condition() << ") " << connecting_operator << " ";
  }
  out << pp(summ.new_state());
  return out.str();
}

std::string pp(const ppg_equation& eq)
{
  std::string connecting_operator = eq.is_conjunctive() ? "&&" : "||";
  std::ostringstream out;
  out << "pbes " << pp(eq.symbol()) << " " << pp(eq.variable()) << " =\n";
  // if(!is_false(eq.simple_formula()) && !is_true(eq.simple_formula()))
  // {
  //   out << "     val(" << pp(eq.simple_formula()) << ") " << connecting_operator << "\n";
  // }
  bool print_operator = false;
  for(const ppg_summand& summ: eq.summands())
  {
    if(print_operator)
    {
      out << " " << connecting_operator << "\n";
    }
    else
    {
      print_operator = true;
    }
    out << pp(summ, eq.is_conjunctive());
  }
  out << ";\n";
  return out.str();
}

std::string pp(const ppg_pbes& x)
{
  std::ostringstream out;
  out << pp(x.data()) << std::endl;
  for(const ppg_equation& eq: x.equations())
  {
    out << pp(eq);
  }
  out << "init " << pp(x.initial_state()) << ";" << std::endl;
  return out.str();
}

inline
std::ostream& operator<<(std::ostream& out, const ppg_equation& x)
{
  return out << pp(x);
}
inline
std::ostream& operator<<(std::ostream& out, const ppg_pbes& x)
{
  return out << pp(x);
}



} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBESSYMBOLICBISIM_PPG_PARSER_H
