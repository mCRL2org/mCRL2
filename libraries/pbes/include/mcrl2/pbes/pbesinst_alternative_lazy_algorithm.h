// Author(s): XIAO Qi
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbesinst_alternative_lazy_algorithm.h
/// \brief An alternative lazy algorithm for instantiating a PBES, ported from
///        bes_deprecated.h.

#include <cassert>
#include <set>
#include <deque>
#include <stack>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbesinst_algorithm.h"
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/fixpoint_symbol.h"
#include "mcrl2/pbes/detail/bes_equation_limit.h"
#include "mcrl2/pbes/detail/instantiate_global_variables.h"
#include "mcrl2/pbes/rewriters/enumerate_quantifiers_rewriter.h"
#include "mcrl2/pbes/rewriters/propositional_variable_rewriter.h"
#include "mcrl2/bes/search_strategy.h"
#include "mcrl2/bes/transformation_strategy.h"
#include "mcrl2/utilities/detail/container_utility.h"

#ifndef MCRL2_PBES_PBESINST_ALTERNATIVE_LAZY_ALGORITHM_H
#define MCRL2_PBES_PBESINST_ALTERNATIVE_LAZY_ALGORITHM_H

namespace std
{
template <>
struct hash<mcrl2::pbes_system::propositional_variable_instantiation>
{
  size_t operator()(const mcrl2::pbes_system::propositional_variable_instantiation& x) const
  {
    return hash<atermpp::aterm>()(x);
  }
};
}

namespace mcrl2
{

namespace pbes_system
{

using mcrl2::bes::search_strategy;
using mcrl2::bes::breadth_first;
using mcrl2::bes::depth_first;
using mcrl2::bes::breadth_first_short;
using mcrl2::bes::depth_first_short;

using mcrl2::bes::transformation_strategy;
using mcrl2::bes::lazy;
using mcrl2::bes::optimize;
using mcrl2::bes::on_the_fly;
using mcrl2::bes::on_the_fly_with_fixed_points;

// Used for printing justifications.
// Maybe should be moved to a more generic place.
template <typename T>
inline
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
  os << "[";
  for (auto i = v.begin(); i != v.end(); i++)
  {
    if (i != v.begin())
    {
      os << " ";
    }
    os << *i;
  }
  os << "]";
  return os;
}

inline mcrl2::pbes_system::pbes_expression pbes_expression_order_quantified_variables(
              const mcrl2::pbes_system::pbes_expression& p, const mcrl2::data::data_specification& data_spec)
{
  using namespace mcrl2;
  using namespace mcrl2::pbes_system;
  using namespace mcrl2::pbes_system::pbes_expr;
  using namespace mcrl2::pbes_system::accessors;

  if (is_pbes_and(p))
  {
    return pbes_expr::and_(pbes_expression_order_quantified_variables(left(p),data_spec),pbes_expression_order_quantified_variables(right(p),data_spec));
  }
  else if (is_pbes_or(p))
  {
    return pbes_expr::or_(pbes_expression_order_quantified_variables(left(p),data_spec),pbes_expression_order_quantified_variables(right(p),data_spec));
  }
  else if (is_pbes_imp(p))
  {
    return pbes_expr::imp(pbes_expression_order_quantified_variables(left(p),data_spec),pbes_expression_order_quantified_variables(right(p),data_spec));
  }
  else if (is_pbes_not(p))
  {
    return pbes_expr::not_(pbes_expression_order_quantified_variables(arg(p),data_spec));
  }
  else if (is_pbes_forall(p))
  {
    const pbes_expression expr = pbes_expression_order_quantified_variables(arg(p),data_spec);
    return pbes_expr::forall(mcrl2::data::order_variables_to_optimise_enumeration(var(p),data_spec),expr);
  }
  else if (is_pbes_exists(p))
  {
    const pbes_expression expr = pbes_expression_order_quantified_variables(arg(p),data_spec);
    return pbes_expr::exists(mcrl2::data::order_variables_to_optimise_enumeration(var(p),data_spec),expr);
  }
  else 
  {
    return p;
  }
}

/// \brief An alternative lazy algorithm for instantiating a PBES, ported from
///         bes_deprecated.h.
class pbesinst_alternative_lazy_algorithm
{
  typedef core::term_traits<pbes_expression> tr;

  protected:
    const data::data_specification& m_data_spec;

    /// \brief Data rewriter.
    data::rewriter datar;

    /// \brief The rewriter.
    enumerate_quantifiers_rewriter R;

    /// \brief The number of generated equations.
    int m_equation_count;

    /// \brief The number of equations generated since last state space
    ///        regeneration.
    int regeneration_count;

    /// \brief The number of equations to generate before regenerating the
    ///        state space.
    int regeneration_period;

    /// \brief Initial value for regeneration_period.
    enum { regeneration_period_init = 100 };

    /// \brief Propositional variable instantiations that need to be handled.
    std::deque<propositional_variable_instantiation> todo;

    /// \brief The content of todo as a set.
    std::set<propositional_variable_instantiation> todo_set;

    /// \brief Propositional variable instantiations that have been handled.
    std::set<propositional_variable_instantiation> done;

    /// \brief Propositional variable instantiations that are reachable from
    ///        init.
    std::set<propositional_variable_instantiation> reachable;

    /// \brief Map an instantiation X to a list of other instantiations that
    ///        are found true of false and thus may justify the ultimate
    ///        outcome of the value of X.
    ///
    ///        This can be used later to build counter-examples. Only
    ///        relevant when optimization level >= 1.
    ///
    ///        Note: Currently this map only exists in memory and is
    ///        ouputted as debug logs in result(). Later the PBES and BES
    ///        equations should be able to store justification maps.
    std::unordered_map<propositional_variable_instantiation, std::vector<propositional_variable_instantiation> > justification;

    /// \brief Map a variable instantiation to a set of other variable
    ///        instantiations on whose right hand sides it appears.
    std::unordered_map<propositional_variable_instantiation, std::set<propositional_variable_instantiation> > occurrence;

    /// \brief Map a variable instantiation to its right hand side.
    std::unordered_map<propositional_variable_instantiation, pbes_expression> equation;

    /// \brief Map a variable instantiations to its right hand side
    ///        when the latter is trivial (either true or false).
    std::unordered_map<propositional_variable_instantiation, pbes_expression> trivial;

    /// \brief instantiations[i] contains all instantiations of the variable
    ///        of the i-th equation in the PBES.
    std::vector<std::vector<propositional_variable_instantiation> > instantiations;

    /// \brief symbols[i] contains the fixedpoint symbol of the i-th equation
    ///        in the PBES.
    std::vector<fixpoint_symbol> symbols;

    /// \brief ranks[i] contains the rank of the i-th equation in the PBES.
    std::vector<int> ranks;

    /// \brief The initial value.
    propositional_variable_instantiation init;

    /// \brief A lookup map for PBES equations.
    std::unordered_map<core::identifier_string, int> equation_index;

    /// \brief Print the equations to standard out.
    bool m_print_equations;

    /// \brief The search strategy to use when exploring the state space.
    search_strategy m_search_strategy;

    /// \brief Transformation strategy.
    transformation_strategy m_transformation_strategy;

    /// \brief Prints a log message for every 1000-th equation
    std::string print_equation_count(size_t size) const
    {
      if (size > 0 && size % 1000 == 0)
      {
        std::ostringstream out;
        out << "Generated " << size << " BES equations" << std::endl;
        return out.str();
      }
      return "";
    }

    // renames propositional variables in x
    pbes_expression rho(const pbes_expression& x) const
    {
      return replace_propositional_variables(x, pbesinst_rename());
    }

  public:

    /// \brief Constructor.
    /// \param data_spec A data specification
    /// \param rewriter_strategy A strategy for the data rewriter
    /// \param print_equations If true, the generated equations are printed
    pbesinst_alternative_lazy_algorithm(
        data::data_specification const& data_spec,
        data::rewriter::strategy rewrite_strategy = data::jitty,
        bool print_equations = false,
        search_strategy search_strategy = breadth_first,
        transformation_strategy transformation_strategy = lazy
        )
      :
        m_data_spec(data_spec),
        datar(data_spec, rewrite_strategy),
        R(datar, data_spec),
        m_equation_count(0),
        regeneration_count(0),
        regeneration_period(regeneration_period_init),
        m_print_equations(print_equations),
        m_search_strategy(search_strategy),
        m_transformation_strategy(transformation_strategy)
    {
      if (m_search_strategy == breadth_first_short)
        m_search_strategy = breadth_first;
      else if (m_search_strategy == depth_first_short)
        m_search_strategy = depth_first;
    }

    inline propositional_variable_instantiation next_todo()
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

    inline void add_todo(const propositional_variable_instantiation &X)
    {
      todo.push_back(X);
      todo_set.insert(X);
      reachable.insert(X);
    }

    int get_rank(propositional_variable_instantiation X)
    {
      return ranks[equation_index[X.name()]];
    }

    template <bool is_mu>
    bool find_loop_rec(
        pbes_expression expr,
        propositional_variable_instantiation X,
        int rank,
        std::unordered_map<propositional_variable_instantiation, bool>& visited)
    {
      if (is_false(expr) || is_true(expr))
      {
        return false;
      }
      if (is_propositional_variable_instantiation(expr))
      {
        auto Y = atermpp::vertical_cast<propositional_variable_instantiation>(expr);
        if (Y == X)
        {
          return true;
        }
        else if (get_rank(Y) != rank)
        {
          return false;
        }
        if (visited.count(Y))
        {
          return visited[Y];
        }
        if (equation.count(Y) == 0)
        {
          return false;
        }
        visited[Y] = false;
        return visited[Y] = find_loop_rec<is_mu>(equation[Y], X, rank, visited);
      }

      using accessors::left;
      using accessors::right;

      if (is_mu)
      {
        if (is_and(expr))
        {
          return find_loop_rec<is_mu>(left(expr), X, rank, visited) ||
                 find_loop_rec<is_mu>(right(expr), X, rank, visited);
        }
        if (is_or(expr))
        {
          return find_loop_rec<is_mu>(left(expr), X, rank, visited) &&
                 find_loop_rec<is_mu>(right(expr), X, rank, visited);
        }
      }
      else
      {
        if (is_and(expr))
        {
          return find_loop_rec<is_mu>(left(expr), X, rank, visited) &&
                 find_loop_rec<is_mu>(right(expr), X, rank, visited);
        }
        if (is_or(expr))
        {
          return find_loop_rec<is_mu>(left(expr), X, rank, visited) ||
                 find_loop_rec<is_mu>(right(expr), X, rank, visited);
        }
      }
      return false;
    }

    template <bool is_mu>
    bool find_loop(pbes_expression expr, propositional_variable_instantiation X)
    {
      std::unordered_map<propositional_variable_instantiation, bool> visited;
      return find_loop_rec<is_mu>(expr, X, get_rank(X), visited);
    }

    void regenerate_states()
    {
      todo.clear();
      todo_set.clear();
      reachable.clear();

      std::stack<pbes_expression> stack;
      stack.push(init);

      while (!stack.empty())
      {
        const pbes_expression expr = stack.top();
        stack.pop();

        if (is_propositional_variable_instantiation(expr))
        {
          auto X = atermpp::vertical_cast<propositional_variable_instantiation>(expr);
          if (reachable.count(X) == 0)
          {
            if (done.count(X))
            {
              stack.push(equation[X]);
              reachable.insert(X);
            }
            else
            {
              add_todo(X);
            }
          }
        }
        else if (is_and(expr) || is_or(expr))
        {
          using accessors::left;
          using accessors::right;
          stack.push(left(expr));
          stack.push(right(expr));
        }
      }
    }

    /// \brief Runs the algorithm. The result is obtained by calling the function \p get_result.
    /// \param p A PBES
    void run(pbes& p)
    {
      using utilities::detail::pick_element;
      using utilities::detail::contains;

      pbes_system::detail::instantiate_global_variables(p);

      auto& pbes_equations = p.equations();

      // simplify all right hand sides of p
      //
      // NOTE: This is not just an optimization. There are certain PBES
      // equations for which applying enumerate_quantifiers_rewriter directly
      // won't terminate, like:
      //
      // forall m: Nat . exists k: Nat . val(m == k)
      pbes_system::one_point_rule_rewriter one_point_rule_rewriter;
      pbes_system::simplify_quantifiers_data_rewriter<mcrl2::data::rewriter> simplify_rewriter(datar);
      for (auto eqi = pbes_equations.begin(); eqi != pbes_equations.end(); eqi++)
      {
        eqi->formula() = pbes_expression_order_quantified_variables(one_point_rule_rewriter(simplify_rewriter(eqi->formula())), m_data_spec);
      }

      // initialize equation_index, instantiations, symbols and ranks
      int eqn_index = 0;
      ranks.resize(pbes_equations.size());
      instantiations.resize(pbes_equations.size());
      for (auto i = pbes_equations.begin(); i != pbes_equations.end(); ++i, ++eqn_index)
      {
        auto const& eqn = *i;
        equation_index[eqn.variable().name()] = eqn_index;
        symbols.push_back(eqn.symbol());
        if (eqn_index > 0)
        {
          ranks[eqn_index] = ranks[eqn_index-1] + (symbols[eqn_index] == symbols[eqn_index-1] ? 0 : 1);
        }
      }

      init = atermpp::down_cast<propositional_variable_instantiation>(R(p.initial_state()));
      add_todo(init);
      while (!todo.empty())
      {
        auto const X_e = next_todo();
        int index = equation_index[X_e.name()];
        done.insert(X_e);
        instantiations[index].push_back(X_e);

        const pbes_equation& eqn = pbes_equations[index];
        data::rewriter::substitution_type sigma;
        make_pbesinst_substitution(eqn.variable().parameters(), X_e.parameters(), sigma);
        auto const& phi = eqn.formula();
        pbes_expression psi_e = R(phi, sigma);

        if (m_transformation_strategy >= optimize)
        {
          // Substitute all trivial variable instantiations by their values
          psi_e = make_propositional_variable_rewriter(trivial, justification[X_e])(psi_e);
        }

        if (m_transformation_strategy >= on_the_fly_with_fixed_points)
        {
          // Find mu or nu loop
          if (eqn.symbol() == fixpoint_symbol::mu())
          {
            if (find_loop<true>(psi_e, X_e))
            {
              psi_e = false_();
            }
          }
          else
          {
            if (find_loop<false>(psi_e, X_e))
            {
              psi_e = true_();
            }
          }
        }

        // Add all variable instantiations in psi_e to todo and generated,
        // and augment the occurrence sets
        std::set<propositional_variable_instantiation> psi_variables = find_propositional_variable_instantiations(psi_e);
        for (auto i = psi_variables.begin(); i != psi_variables.end(); ++i)
        {
          if (todo_set.count(*i) == 0 && done.count(*i) == 0)
          {
            add_todo(*i);
          }
          occurrence[*i].insert(X_e);
        }

        // Store the result
        equation[X_e] = psi_e;

        if (m_transformation_strategy >= optimize && (is_true(psi_e) || is_false(psi_e)))
        {
          trivial[X_e] = psi_e;
          if (m_transformation_strategy >= on_the_fly)
          {
            // Substitute X_e to its value in all its occurrences, and
            // substitute all other variables to their values that are found
            // to be either true or false in all their occurrences.
            std::set<propositional_variable_instantiation> new_trivials;
            new_trivials.insert(X_e);
            while (!new_trivials.empty())
            {
              auto X = *new_trivials.begin();
              new_trivials.erase(new_trivials.begin());

              auto oc = occurrence[X];
              // TODO Instead using a map of a single element, we should
              // probably generalize propositional_variable_rewriter to take a
              // function instead of a set
              std::unordered_map<propositional_variable_instantiation, pbes_expression> trivial_X;
              trivial_X[X] = psi_e;
              for (auto i = oc.begin(); i != oc.end(); i++)
              {
                auto Y = *i;
                pbes_expression &f = equation[Y];
                f = make_propositional_variable_rewriter(trivial_X, justification[Y])(f);
                if (is_true(f) || is_false(f))
                {
                  trivial[Y] = f;
                  new_trivials.insert(Y);
                }
              }
              occurrence.erase(X);
            }
          }
        }

        if (m_transformation_strategy >= on_the_fly)
        {
          if (++regeneration_count == regeneration_period)
          {
            regeneration_count = 0;
            regeneration_period = equation.size() / 2;
            regenerate_states();
          }
        }

        if (m_print_equations)
        {
          mCRL2log(log::info) << eqn.symbol() << " " << X_e << " = " << psi_e << std::endl;
        }

        mCRL2log(log::verbose) << print_equation_count(++m_equation_count);
        detail::check_bes_equation_limit(m_equation_count);
      }
    }

    /// \brief Returns the computed bes in pbes format
    /// \return The computed bes in pbes format
    pbes get_result()
    {
      mCRL2log(log::verbose) << "Generated " << m_equation_count << " BES equations in total, outputting BES" << std::endl;
      pbes result;
      int index = 0;
      for (auto i = instantiations.begin(); i != instantiations.end(); i++)
      {
        auto symbol = symbols[index++];
        for (auto j = i->begin(); j != i->end(); j++)
        {
          auto X_e = *j;
          if (reachable.count(X_e) == 0)
          {
            continue;
          }
          auto lhs = propositional_variable(pbesinst_rename()(X_e).name(), data::variable_list());
          auto rhs = rho(equation[X_e]);
          result.equations().push_back(pbes_equation(symbol, lhs, rhs));
          mCRL2log(log::debug) << "Equation: " << symbol << " " << X_e << " = " << equation[X_e] << std::endl;
          mCRL2log(log::debug) << "Obtained justification of " << X_e << ": " << justification[X_e] << std::endl;
        }
      }

      result.initial_state() = pbesinst_rename()(init);
      return result;
    }

    /// \brief Returns the flag for printing the generated bes equations
    /// \return The flag for printing the generated bes equations
    bool& print_equations()
    {
      return m_print_equations;
    }

    enumerate_quantifiers_rewriter& rewriter()
    {
      return R;
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESINST_ALTERNATIVE_LAZY_ALGORITHM_H
