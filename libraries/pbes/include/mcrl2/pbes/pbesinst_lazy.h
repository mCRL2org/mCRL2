// Author(s): Jan Friso Groote
//            Xiao Qi
//            Wieger Wesselink 2017-2018
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbesinst_lazy_algorithm.h
/// \brief A lazy algorithm for instantiating a PBES, ported from bes_deprecated.h.

#include <cassert>
#include <set>
#include <deque>
#include <stack>
#include <iostream>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/detail/bes_equation_limit.h"
#include "mcrl2/pbes/detail/instantiate_global_variables.h"
#include "mcrl2/pbes/pbes_equation_index.h"
#include "mcrl2/pbes/remove_equations.h"
#include "mcrl2/pbes/replace.h"
#include "mcrl2/pbes/rewriters/enumerate_quantifiers_rewriter.h"
#include "mcrl2/pbes/rewriters/one_point_rule_rewriter.h"
#include "mcrl2/pbes/rewriters/simplify_quantifiers_rewriter.h"
#include "mcrl2/pbes/rewriters/simplify_rewriter.h"
#include "mcrl2/pbes/search_strategy.h"
#include "mcrl2/pbes/transformation_strategy.h"
#include "mcrl2/pbes/transformations.h"
#include "mcrl2/utilities/detail/container_utility.h"
#include "mcrl2/utilities/text_utility.h"

#ifndef MCRL2_PBES_PBESINST_LAZY_H
#define MCRL2_PBES_PBESINST_LAZY_H

namespace mcrl2
{

namespace pbes_system
{

/// \brief An alternative lazy algorithm for instantiating a PBES, ported from
///         bes_deprecated.h.
class pbesinst_lazy_algorithm
{
  protected:
    /// \brief Data rewriter.
    data::rewriter datar;

    /// \brief A PBES.
    pbes m_pbes;

    /// \brief A lookup map for PBES equations.
    pbes_equation_index m_equation_index;

    /// \brief The rewriter.
    enumerate_quantifiers_rewriter R;

    /// \brief The propositional variable instantiations that need to be handled.
    std::deque<propositional_variable_instantiation> todo;

    /// \brief The propositional variable instantiations that have been discovered (not necessarily handled).
    std::unordered_set<propositional_variable_instantiation> discovered;

    /// \brief The initial value (after rewriting).
    propositional_variable_instantiation init;

    /// \brief The search strategy to use when exploring the state space.
    search_strategy m_search_strategy;

    // \brief The number of iterations
    std::size_t m_iteration_count = 0;

    int m_optimization = 0;

    /// \brief Prints a log message for every 1000-th equation
    std::string print_equation_count(std::size_t size) const
    {
      if (size > 0 && size % 1000 == 0)
      {
        std::ostringstream out;
        out << "Generated " << size << " BES equations" << std::endl;
        return out.str();
      }
      return "";
    }

    /// \brief Creates a substitution function for the pbesinst rewriter.
    /// \param v A sequence of data variables
    /// \param e A sequence of data expressions
    /// \param sigma The substitution that maps the i-th element of \p v to the i-th element of \p e
    inline
    void make_substitution(const data::variable_list& v, const data::data_expression_list& e, data::rewriter::substitution_type& sigma)
    {
      assert(v.size() == e.size());
      auto vi = v.begin();
      auto ei = e.begin();
      for (; vi != v.end(); ++vi, ++ei)
      {
        sigma[*vi] = *ei;
      }
    }

    // instantiates global variables
    // simplifies the pbes
    pbes preprocess(const pbes& x) const
    {
      pbes p = x;
      pbes_system::detail::instantiate_global_variables(p);
      pbes_system::one_point_rule_rewriter one_point_rule_rewriter;
      pbes_system::simplify_quantifiers_data_rewriter<mcrl2::data::rewriter> simplify_rewriter(datar);
      for (pbes_equation& eqn: p.equations())
      {
        eqn.formula() = order_quantified_variables(one_point_rule_rewriter(simplify_rewriter(eqn.formula())), p.data());
      }
      return p;
    }

    pbes_expression rewrite_true_false(const fixpoint_symbol& symbol,
                                       const propositional_variable_instantiation& X,
                                       const pbes_expression& psi
                                      ) const
    {
      bool changed = false;
      pbes_expression value;
      if (symbol.is_mu())
      {
        value = false_();
      }
      else
      {
        value = true_();
      }
      pbes_expression result = replace_propositional_variables(psi, [&](const propositional_variable_instantiation& Y) -> pbes_expression {
                                                                   if (Y == X)
                                                                   {
                                                                     changed = true;
                                                                     return value;
                                                                   }
                                                                   return Y;
                                                               }
      );
      if (changed)
      {
        simplify_rewriter R;
        return R(result);
      }
      else
      {
        return result;
      }
    }

  public:

    /// \brief Constructor.
    /// \param p The pbes used in the exploration algorithm.
    /// \param rewrite_strategy A strategy for the data rewriter.
    /// \param search_strategy The search strategy used to explore the pbes, typically depth or breadth first.
    /// \param optimization An indication of the optimisation level. 
    explicit pbesinst_lazy_algorithm(
            const pbes& p,
            data::rewriter::strategy rewrite_strategy = data::jitty,
            search_strategy search_strategy = breadth_first,
            int optimization = 0
    )
     : datar(p.data(), data::used_data_equation_selector(p.data(), pbes_system::find_function_symbols(p), p.global_variables()), rewrite_strategy),
       m_pbes(preprocess(p)),
       m_equation_index(p),
       R(datar, p.data()),
       m_search_strategy(search_strategy),
       m_optimization(optimization)
    { }

    /// \brief Reports BES equations that are produced by the algorithm.
    /// This function is called for every BES equation X = psi with rank k that is produced. By default it does nothing.
    virtual void on_report_equation(const propositional_variable_instantiation& /* X */, const pbes_expression& /* psi */, std::size_t /* k */)
    { }

    /// \brief This function is called at the end of every iteration.
    virtual void on_end_iteration()
    { }

    propositional_variable_instantiation next_todo()
    {
      if (m_search_strategy == breadth_first)
      {
        auto X_e = todo.front();
        todo.pop_front();
        return X_e;
      }
      else
      {
        auto X_e = todo.back();
        todo.pop_back();
        return X_e;
      }
    }

    const fixpoint_symbol& symbol(std::size_t i) const
    {
      return m_pbes.equations()[i].symbol();
    }

    // rewrite the right hand side of the equation X = psi
    virtual pbes_expression rewrite_psi(const fixpoint_symbol& symbol,
                                        const propositional_variable_instantiation& X,
                                        const pbes_expression& psi
                                       )
    {
      return m_optimization >= 1 ? rewrite_true_false(symbol, X, psi) : psi;
    }

    virtual bool solution_found(const propositional_variable_instantiation& /* init */) const
    {
      return false;
    }

    // recreates todo
    virtual void reset(const propositional_variable_instantiation& /* init */,
                       std::deque<propositional_variable_instantiation>& /* todo */,
                       std::size_t /* regeneration_period */
                      )
    { }

    /// \brief Runs the algorithm. The result is obtained by calling the function \p get_result.
    virtual void run()
    {
      using utilities::detail::contains;

      m_iteration_count = 0;

      init = atermpp::down_cast<propositional_variable_instantiation>(R(m_pbes.initial_state()));
      todo.push_back(init);
      discovered.insert(init);
      while (!todo.empty())
      {
        ++m_iteration_count;
        mCRL2log(log::status) << print_equation_count(m_iteration_count);
        detail::check_bes_equation_limit(m_iteration_count);

        auto const& X_e = next_todo();

        std::size_t index = m_equation_index.index(X_e.name());
        const pbes_equation& eqn = m_pbes.equations()[index];
        data::rewriter::substitution_type sigma;
        make_substitution(eqn.variable().parameters(), X_e.parameters(), sigma);
        const auto& phi = eqn.formula();
        pbes_expression psi_e = R(phi, sigma);

        // optional step
        psi_e = rewrite_psi(eqn.symbol(), X_e, psi_e);

        // report the generated equation
        std::size_t k = m_equation_index.rank(X_e.name());
        mCRL2log(log::debug) << "generated equation " << X_e << " = " << psi_e << " with rank " << k << std::endl;
        on_report_equation(X_e, psi_e, k);

        for (const propositional_variable_instantiation& Y_f: find_propositional_variable_instantiations(psi_e))
        {
          if (!contains(discovered, Y_f))
          {
            todo.push_back(Y_f);
            discovered.insert(Y_f);
          }
        }

        on_end_iteration();

        if (solution_found(init))
        {
          break;
        }
        reset(init, todo, (discovered.size() - todo.size()) / 2);
      }
    }

    const pbes_equation_index& equation_index() const
    {
      return m_equation_index;
    }

    enumerate_quantifiers_rewriter& rewriter()
    {
      return R;
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESINST_LAZY_H
