// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/symbolic_exploration.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_SYMBOLIC_EXPLORATION_H
#define MCRL2_PBES_DETAIL_SYMBOLIC_EXPLORATION_H

#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/pbes_functions.h"
#include "mcrl2/pbes/pbesinst_symbolic.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

class symbolic_exploration_algorithm
{
  protected:
    pbes& m_pbes;
    std::vector<data::variable> m_variables; // the parameters of the current equation (used for clustering)
    data::set_identifier_generator m_generator; // used for generating cluster variables
    bool m_optimized;
    bool m_clustered;
    std::vector<pbes_equation> m_cluster_equations;

    static std::string pp(const std::vector<data::variable>& v)
    {
      return data::pp(data::variable_list(v.begin(), v.end()));
    }

    void push_variables(const data::variable_list& v)
    {
      if (v.empty())
      {
        return;
      }
      m_variables.insert(m_variables.end(), v.begin(), v.end());
    }

    void pop_variables(const data::variable_list& v)
    {
      if (v.empty())
      {
        return;
      }
      assert (m_variables.size() >= v.size());
      m_variables.erase(m_variables.end() - v.size(), m_variables.end());
    }

    /// \brief Concatenates two variable lists
    /// \param x A variable list
    /// \param y A variable list
    /// \return The concatenation of x and y
    static data::variable_list concat(const data::variable_list& x, const data::variable_list& y)
    {
      std::vector<data::variable> v(x.begin(), x.end());
      v.insert(v.end(), y.begin(), y.end());
      return data::variable_list(v.begin(), v.end());
    }

    bool is_disjunctive(const pbes_expression& x) const
    {
      return is_or(x) || is_exists(x);
    }

    bool is_conjunctive(const pbes_expression& x) const
    {
      return is_and(x) || is_forall(x);
    }

    void check_equation(const pbes_equation& eqn) const
    {
      std::set<data::variable> v = pbes_system::find_free_variables(eqn);
      if (!v.empty())
      {
        throw std::runtime_error("CORRUPT CLUSTER");
      }
    }

    pbes_expression make_cluster(const pbes_expression& x)
    {
      core::identifier_string X = m_generator("Cluster");
      data::variable_list parameters(m_variables.begin(), m_variables.end());
      pbes_equation eqn(fixpoint_symbol::mu(), propositional_variable(X, parameters), x);
      m_cluster_equations.push_back(eqn);
      mCRL2log(log::debug) << "\nadding cluster " << pbes_system::pp(eqn);
      check_equation(eqn);
      data::data_expression_list e = data::make_data_expression_list(parameters);
      return propositional_variable_instantiation(X, e);
    }

    pbes_expression not_(const pbes_expression& x)
    {
      if (m_optimized)
      {
        pbes_expression result;
        optimized_not(result, x);
        return result;
      }
      else
      {
        return pbes_system::not_(x);
      }
    }

    pbes_expression and_(const pbes_expression& x, const pbes_expression& y)
    {
      const pbes_expression& left = x;
      const pbes_expression& right = y;
      if (m_optimized)
      {
        pbes_expression result;
        optimized_and(result, left, right);
        return result;
      }
      else
      {
        return pbes_system::and_(left, right);
      }
    }

    pbes_expression or_(const pbes_expression& x, const pbes_expression& y)
    {
      const pbes_expression& left = x;
      const pbes_expression& right = y;
      if (m_optimized)
      {
        pbes_expression result;
        optimized_or(result, left, right);
        return result;
      }
      else
      {
        return pbes_system::or_(left, right);
      }
    }

    pbes_expression imp(const pbes_expression& x, const pbes_expression& y)
    {
      const pbes_expression& left = x;
      const pbes_expression& right = y;
      if (m_optimized)
      {
        pbes_expression result;
        optimized_imp(result, left, right);
        return result;
      }
      else
      {
        return pbes_system::imp(left, right);
      }
    }

    pbes_expression forall(const data::variable_list& d, const pbes_expression& x)
    {
      const pbes_expression& body = x;
      if (m_optimized)
      {
        pbes_expression result;
        optimized_forall(result, d, body);
        return result;
      }
      else
      {
        return pbes_system::forall(d, body);
      }
    }

    pbes_expression exists(const data::variable_list& d, const pbes_expression& x)
    {
      const pbes_expression& body = x;
      if (m_optimized)
      {
        pbes_expression result;
        optimized_exists(result, d, body);
        return result;
      }
      else
      {
        return pbes_system::exists(d, body);
      }
    }

    pbes_expression expr_or(const pbes_expression& x, const data::variable_list& v = data::variable_list())
    {
      push_variables(v);
      pbes_expression result;
      if (is_propositional_variable_instantiation(x))
      {
        result = false_();
      }
      else if (is_or(x))
      {
        const auto& y = atermpp::down_cast<pbes_system::or_>(x);
        result = or_(expr_or(y.left()), expr_or(y.right()));
      }
      else if (is_forall(x))
      {
        const auto& y = atermpp::down_cast<pbes_system::forall>(x);
        result = forall(y.variables(), expr_or(y.body(), y.variables()));
      }
      else if (is_exists(x))
      {
        const auto& y = atermpp::down_cast<pbes_system::exists>(x);
        result = exists(y.variables(), expr_or(y.body(), y.variables()));
      }
      else
      {
        result = x;
      }
      //else
      //{
      //  throw mcrl2::runtime_error("unknown pbes expression encountered in expr_or: " + pbes_system::pp(x));
      //}
      mCRL2log(log::debug) << "\n<expr_or> " << pbes_system::pp(x) << " -> " << pbes_system::pp(result);
      pop_variables(v);
      return result;
    }

    pbes_expression expr_and(const pbes_expression& x, const data::variable_list& v = data::variable_list())
    {
      push_variables(v);
      pbes_expression result;
      if (is_propositional_variable_instantiation(x))
      {
        result = true_();
      }
      else if (is_and(x))
      {
        const auto& y = atermpp::down_cast<pbes_system::and_>(x);
        result = and_(expr_and(y.left()), expr_and(y.right()));
      }
      else if (is_forall(x))
      {
        const auto& y = atermpp::down_cast<pbes_system::forall>(x);
        result = forall(y.variables(), expr_and(y.body(), y.variables()));
      }
      else if (is_exists(x))
      {
        const auto& y = atermpp::down_cast<pbes_system::exists>(x);
        result = exists(y.variables(), expr_and(y.body(), y.variables()));
      }
      else
      {
        result = x;
      }
      //else
      //{
      //  throw mcrl2::runtime_error("unknown pbes expression encountered in expr_and: " + pbes_system::pp(x));
      //}
      mCRL2log(log::debug) << "\n<expr_and> " << pbes_system::pp(x) << " -> " << pbes_system::pp(result);
      pop_variables(v);
      return result;
    }

    pbes_expression F_or(const pbes_expression& x)
    {
      pbes_expression result;
      if (is_simple_expression(x, false))
      {
        result = false_();
      }
      else if (is_propositional_variable_instantiation(x))
      {
        result = x;
      }
      else if (is_and(x))
      {
        result = and_(expr_and(x), make_cluster(F_and(x)));
      }
      else if (is_or(x))
      {
        pbes_expression phi = pbes_system::or_(x).left();
        pbes_expression psi = pbes_system::or_(x).right();
        result = or_(F_or(phi), F_or(psi));
      }
      else if (is_forall(x))
      {
        result = and_(expr_and(x), make_cluster(F_and(x)));
      }
      else if (is_exists(x))
      {
        data::variable_list d = pbes_system::exists(x).variables();
        pbes_expression phi = pbes_system::exists(x).body();
        pbes_expression Fphi = F(phi, d);
        if (is_conjunctive(Fphi))
        {
          push_variables(d);
          Fphi = make_cluster(Fphi);
          pop_variables(d);
        }
        result = exists(d, and_(expr_and(phi, d), Fphi));
      }
      else
      {
        throw mcrl2::runtime_error("unknown pbes expression encountered in F_or: " + pbes_system::pp(x));
      }
      mCRL2log(log::debug) << "\n<F_or> " << pbes_system::pp(x) << " -> " << pbes_system::pp(result);
      return result;
    }

    pbes_expression F_and(const pbes_expression& x)
    {
      pbes_expression result;
      if (is_simple_expression(x, false))
      {
        result = true_();
      }
      else if (is_propositional_variable_instantiation(x))
      {
        result = x;
      }
      else if (is_and(x))
      {
        pbes_expression phi = pbes_system::and_(x).left();
        pbes_expression psi = pbes_system::and_(x).right();
        result = and_(F_and(phi), F_and(psi));
      }
      else if (is_or(x))
      {
        result = imp(not_(expr_or(x)), make_cluster(F_or(x)));
      }
      else if (is_exists(x))
      {
        result = imp(not_(expr_or(x)), make_cluster(F_or(x)));
      }
      else if (is_forall(x))
      {
        data::variable_list d = pbes_system::forall(x).variables();
        pbes_expression phi = pbes_system::forall(x).body();
        pbes_expression Fphi = F(phi, d);
        if (is_disjunctive(Fphi))
        {
          push_variables(d);
          Fphi = make_cluster(Fphi);
          pop_variables(d);
        }
        result = forall(d, imp(not_(expr_or(phi, d)), Fphi));
      }
      else
      {
        throw mcrl2::runtime_error("unknown pbes expression encountered in F_and: " + pbes_system::pp(x));
      }
      mCRL2log(log::debug) << "\n<F_and> " << pbes_system::pp(x) << " -> " << pbes_system::pp(result);
      return result;
    }

    pbes_expression F(const pbes_expression& x, const data::variable_list& v = data::variable_list())
    {
      push_variables(v);
      pbes_expression result;
      if (is_conjunctive(x))
      {
        result = F_and(x);
      }
      else
      {
        result = F_or(x);
      }
      mCRL2log(log::debug) << "\n<F> " << pbes_system::pp(x) << " -> " << pbes_system::pp(result);
      pop_variables(v);
      return result;
    }

  public:
    symbolic_exploration_algorithm(pbes& p, bool optimized, bool clustered)
      : m_pbes(p), m_optimized(optimized), m_clustered(clustered)
    {
      std::vector<pbes_equation>& equations = m_pbes.equations();
      for (auto& equation: equations)
      {
        m_generator.add_identifier(equation.variable().name());
      }
    }

    void run()
    {
      std::vector<pbes_equation>& equations = m_pbes.equations();
      for (auto& equation : equations)
      {
        push_variables(equation.variable().parameters());
        pbes_expression phi = equation.formula();
        if (is_conjunctive(phi))
        {
          equation.formula() = and_(expr_and(phi), make_cluster(F_and(phi)));
        }
        else
        {
          equation.formula() = imp(not_(expr_or(phi)), make_cluster(F_or(phi)));
        }
        pop_variables(equation.variable().parameters());
      }

      // add the cluster equations to the PBES
      equations.insert(equations.end(), m_cluster_equations.begin(), m_cluster_equations.end());
    }
};

inline
void symbolic_exploration(const std::string& input_filename,
                          const std::string& output_filename,
                          const utilities::file_format& input_format,
                          const utilities::file_format& output_format,
                          bool optimized = true,
                          bool clustered = false,
                          bool instantiate = false
                         )
{
  // load the pbes
  pbes p;
  load_pbes(p, input_filename, input_format);

  // apply the algorithm
  symbolic_exploration_algorithm algorithm(p, optimized, clustered);
  algorithm.run();

  // save the result
  save_pbes(p, output_filename, output_format);

  if (instantiate)
  {
    // instantiate the PBES
    pbesinst_symbolic_algorithm symbolic_algorithm(p);
    symbolic_algorithm.run();
  }
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_SYMBOLIC_EXPLORATION_H
