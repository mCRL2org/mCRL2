// Author(s): Jan Friso Groote
//            Xiao Qi
//            Wieger Wesselink 2017-2018
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
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
#include "mcrl2/pbes/pbesinst_algorithm.h"
#include "mcrl2/pbes/pbes_equation_index.h"
#include "mcrl2/pbes/remove_equations.h"
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

namespace detail
{

// attempts to move the equation with the initial variable to the top
inline
bool move_initial_variable_equation_up(pbes& p)
{
  auto& equations = p.equations();

  const core::identifier_string& X = p.initial_state().name();
  if (equations.front().variable().name() == X)
  {
    return true;
  }

  const fixpoint_symbol& first_symbol = equations.front().symbol();
  for (auto i = ++equations.begin(); i != equations.end(); ++i)
  {
    const pbes_equation& eqn = *i;
    if (eqn.symbol() != first_symbol)
    {
      return false;
    }
    if (eqn.variable().name() == X)
    {
      std::swap(*equations.begin(), *i);
      return true;
    }
  }
  throw mcrl2::runtime_error("move_initial_variable_equation_up: could not find an equation corresponding to the initial state");
}

struct pbesinst_rename_short
{
  std::unordered_map<propositional_variable_instantiation, core::identifier_string>& name_map;
  std::size_t index = 0;

  explicit pbesinst_rename_short(std::unordered_map<propositional_variable_instantiation, core::identifier_string>& name_map_)
    : name_map(name_map_)
  {}

  core::identifier_string operator()(const propositional_variable_instantiation& Ye)
  {
    auto i = name_map.find(Ye);
    if (i != name_map.end())
    {
      return i->second;
    }
    core::identifier_string name("X" + utilities::number2string(++index));
    name_map[Ye] = name;
    return name;
  }
};

template <template <class> class Builder, class Derived, class Map>
struct add_forward_substitute_rewriter: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::apply;

  typedef core::term_traits<pbes_expression> tr;

  const Map &map;

  explicit add_forward_substitute_rewriter(const Map& m)
    : map(m)
  {}

  bool is_true_or_false(const pbes_expression& x) const
  {
    return is_true(x) || is_false(x);
  }

  pbes_expression apply(const propositional_variable_instantiation& x)
  {
    auto p = map.find(x);
    if (p == map.end() || !is_true_or_false(p->second))
    {
      return x;
    }
    else
    {
      return p->second;
    }
  }
};

template <typename Map>
struct forward_substitute_rewriter_builder: public add_forward_substitute_rewriter<pbes_system::detail::simplify_builder, forward_substitute_rewriter_builder<Map>, Map>
{
  typedef add_forward_substitute_rewriter<pbes_system::detail::simplify_builder, forward_substitute_rewriter_builder<Map>, Map> super;
  explicit forward_substitute_rewriter_builder(const Map& m)
    : super(m)
  {}
};

template <typename Map>
struct forward_substitute_rewriter
{
  const Map& map;

  explicit forward_substitute_rewriter(const Map& m)
    : map(m)
  {}

  pbes_expression operator()(const pbes_expression& x) const
  {
    return forward_substitute_rewriter_builder<Map>(map).apply(x);
  }
};

template <typename Map>
forward_substitute_rewriter<Map>
make_forward_substitute_rewriter(const Map &m)
{
  return forward_substitute_rewriter<Map>(m);
}

/// \brief Returns true if map/set m has x as a key
template <typename Map>
bool has_key(const Map& m, const typename Map::key_type& x)
{
  return m.find(x) != m.end();
}

struct find_loop_simplifier
{
  /// \brief A lookup map for PBES equations.
  const pbes_equation_index& equation_index;

  /// \brief Map a variable instantiation to its right hand side.
  const std::unordered_map<propositional_variable_instantiation, pbes_expression>& equation;

  template <bool is_mu>
  bool find_loop_rec(const pbes_expression& x,
                     propositional_variable_instantiation X,
                     std::size_t rank,
                     std::unordered_map<propositional_variable_instantiation, bool>& visited
                    ) const
  {
    if (is_false(x) || is_true(x))
    {
      return false;
    }
    if (is_propositional_variable_instantiation(x))
    {
      auto const& Y = atermpp::down_cast<propositional_variable_instantiation>(x);
      if (Y == X)
      {
        return true;
      }
      else if (equation_index.rank(Y.name()) != rank)
      {
        return false;
      }
      if (has_key(visited, Y))
      {
        return visited[Y];
      }
      if (!has_key(equation, Y))
      {
        return false;
      }
      visited[Y] = false;
      bool b = find_loop_rec<is_mu>(equation.at(Y), X, rank, visited);
      visited[Y] = b;
      return b;
    }

    if (is_mu)
    {
      if (is_and(x))
      {
        const auto& x_ = atermpp::down_cast<and_>(x);
        return find_loop_rec<is_mu>(x_.left(), X, rank, visited) ||
               find_loop_rec<is_mu>(x_.right(), X, rank, visited);
      }
      if (is_or(x))
      {
        const auto& x_ = atermpp::down_cast<or_>(x);
        return find_loop_rec<is_mu>(x_.left(), X, rank, visited) &&
               find_loop_rec<is_mu>(x_.right(), X, rank, visited);
      }
    }
    else
    {
      if (is_and(x))
      {
        const auto& x_ = atermpp::down_cast<and_>(x);
        return find_loop_rec<is_mu>(x_.left(), X, rank, visited) &&
               find_loop_rec<is_mu>(x_.right(), X, rank, visited);
      }
      if (is_or(x))
      {
        const auto& x_ = atermpp::down_cast<or_>(x);
        return find_loop_rec<is_mu>(x_.left(), X, rank, visited) ||
               find_loop_rec<is_mu>(x_.right(), X, rank, visited);
      }
    }
    return false;
  }

  template <bool is_mu>
  bool find_loop(pbes_expression expr, propositional_variable_instantiation X) const
  {
    std::unordered_map<propositional_variable_instantiation, bool> visited;
    return find_loop_rec<is_mu>(expr, X, equation_index.rank(X.name()), visited);
  }

  find_loop_simplifier(const pbes_equation_index& equation_index_,
                       const std::unordered_map<propositional_variable_instantiation, pbes_expression>& equation_
                      )
    : equation_index(equation_index_),
      equation(equation_)
  {}

  pbes_expression operator()(const pbes_expression& psi_e, const fixpoint_symbol& symbol, const propositional_variable_instantiation& X_e) const
  {
    // Find mu or nu loop
    if (symbol == fixpoint_symbol::mu())
    {
      if (find_loop<true>(psi_e, X_e))
      {
        return false_();
      }
    }
    else
    {
      if (find_loop<false>(psi_e, X_e))
      {
        return true_();
      }
    }
    return psi_e;
  }
};

// This class is used to periodically reset some attributes of the pbesinst algorithm, using a reachability analysis.
struct pbesinst_resetter
{
  /// \brief The number of equations generated since last state space
  ///        regeneration.
  std::size_t regeneration_count;

  /// \brief The number of equations to generate before regenerating the
  ///        state space.
  std::size_t regeneration_period;

  /// \brief Initial value for regeneration_period.
  enum { regeneration_period_init = 100 };

  pbesinst_resetter()
    : regeneration_count(0),
      regeneration_period(regeneration_period_init)
  {}

  void operator()(const propositional_variable_instantiation& init,
                  std::deque<propositional_variable_instantiation>& todo,
                  std::unordered_set<propositional_variable_instantiation>& discovered,
                  const std::unordered_map<propositional_variable_instantiation, pbes_expression>& equation
                 )
  {
    using utilities::detail::contains;

    if (++regeneration_count == regeneration_period)
    {
      regeneration_count = 0;
      regeneration_period = equation.size() / 2;
    }
    else
    {
      return;
    }

    todo.clear();

    std::stack<propositional_variable_instantiation> todo1;
    std::unordered_set<propositional_variable_instantiation> done1;
    todo1.push(init);

    while (!todo1.empty())
    {
      propositional_variable_instantiation X_e = todo1.top();
      todo1.pop();
      done1.insert(X_e);
      if (equation.find(X_e) != equation.end())
      {
        for (const propositional_variable_instantiation& Y_f: find_propositional_variable_instantiations(equation.at(X_e)))
        {
          if (!contains(done1, Y_f))
          {
            todo1.push(Y_f);
          }
        }
      }
      else
      {
        todo.push_back(X_e);
        discovered.insert(X_e);
      }
    }
  }
};

struct pbesinst_backward_substitute
{
  /// \brief Map a variable instantiation to a set of other variable instantiations on whose right hand sides it appears.
  std::unordered_map<propositional_variable_instantiation, std::unordered_set<propositional_variable_instantiation>> occurrence;

  // Substitute X_e to its value in all its occurrences, and substitute all other variables to their values that are found
  // to be either true or false in all their occurrences.
  std::unordered_set<propositional_variable_instantiation> trivials;

  void add_dependency(const propositional_variable_instantiation& v, const propositional_variable_instantiation& X_e)
  {
    occurrence[v].insert(X_e);
  }

  void operator()(transformation_strategy strategy,
                  const pbes_expression& psi_e,
                  const propositional_variable_instantiation& X_e,
                  std::unordered_map<propositional_variable_instantiation, pbes_expression>& equation
                 )
  {
    if (strategy >= optimize && (is_true(psi_e) || is_false(psi_e)))
    {
      if (strategy >= on_the_fly)
      {
        trivials.clear();
        trivials.insert(X_e);
        while (!trivials.empty())
        {
          auto X = *trivials.begin();
          trivials.erase(trivials.begin());

          for (const auto& Y: occurrence[X]) {
            pbes_expression& f = equation[Y];
            f = make_forward_substitute_rewriter(equation)(f);
            if (is_true(f) || is_false(f))
            {
              trivials.insert(Y);
            }
          }
          occurrence.erase(X);
        }
      }
    }
  }
};

} // namespace detail

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

    /// \brief Map a variable instantiation to its right hand side.
    std::unordered_map<propositional_variable_instantiation, pbes_expression> equation;

    /// \brief The initial value (after rewriting).
    propositional_variable_instantiation init;

    /// \brief The search strategy to use when exploring the state space.
    search_strategy m_search_strategy;

    /// \brief Transformation strategy.
    transformation_strategy m_transformation_strategy;

    /// \brief Simplifies expressions based on a loop analysis.
    detail::find_loop_simplifier m_find_loop_simplifier;

    /// \brief Simplifies some attributes based on variables that have the value true or false.
    detail::pbesinst_backward_substitute m_pbesinst_backward_substitute;

    /// \brief Simplifies some attributes based on variables that have the value true or false.
    detail::pbesinst_resetter m_pbesinst_resetter;

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

    // instantiates global variables
    // simplifies the pbes
    pbes preprocess(const pbes& x) const
    {
      pbes p = x;
      pbes_system::detail::instantiate_global_variables(p);

      // simplify all right hand sides of p
      //
      // NOTE: This is not just an optimization. There are certain PBES
      // equations for which applying enumerate_quantifiers_rewriter directly
      // won't terminate, like:
      //
      // forall m: Nat . exists k: Nat . val(m == k)
      pbes_system::one_point_rule_rewriter one_point_rule_rewriter;
      pbes_system::simplify_quantifiers_data_rewriter<mcrl2::data::rewriter> simplify_rewriter(datar);
      for (pbes_equation& eq: p.equations())
      {
        eq.formula() = order_quantified_variables(one_point_rule_rewriter(simplify_rewriter(eq.formula())), p.data());
      }
      return p;
    }

    pbes_expression forward_substitute(const pbes_expression& psi_e,
                                       const std::unordered_map<propositional_variable_instantiation, pbes_expression>& equation
                                      )
    {
      if (m_transformation_strategy >= optimize)
      {
        // Substitute all trivial variable instantiations by their values
        return detail::make_forward_substitute_rewriter(equation)(psi_e);
      }
      else
      {
        return psi_e;
      }
    }

    pbes_expression simplify_loop(const pbes_expression& psi_e, const fixpoint_symbol& symbol, const propositional_variable_instantiation& X_e) const
    {
      if (m_transformation_strategy >= on_the_fly_with_fixed_points)
      {
        return m_find_loop_simplifier(psi_e, symbol, X_e);
      }
      else
      {
        return psi_e;
      }
    }

    void backward_substitute(const pbes_expression& psi_e,
                             const propositional_variable_instantiation& X_e,
                             std::unordered_map<propositional_variable_instantiation, pbes_expression>& equation
                            )
    {
      if (m_transformation_strategy >= optimize && (is_true(psi_e) || is_false(psi_e)))
      {
        m_pbesinst_backward_substitute(m_transformation_strategy, psi_e, X_e, equation);
      }
    }

    void reset(const propositional_variable_instantiation& init,
               std::deque<propositional_variable_instantiation>& todo,
               std::unordered_set<propositional_variable_instantiation>& done,
               const std::unordered_map<propositional_variable_instantiation, pbes_expression>& equation
              )
    {
      if (m_transformation_strategy >= on_the_fly)
      {
        m_pbesinst_resetter(init, todo, done, equation);
      }
    }

  public:

    /// \brief Constructor.
    /// \param p The pbes used in the exploration algorithm.
    /// \param rewrite_strategy A strategy for the data rewriter.
    /// \param search_strategy The search strategy used to explore the pbes, typically depth or breadth first.
    /// \param transformation_strategy The strategy that determines to which extent the generated bes is simplified while being generated.
    explicit pbesinst_lazy_algorithm(
         const pbes& p,
         data::rewriter::strategy rewrite_strategy = data::jitty,
         search_strategy search_strategy = breadth_first,
         transformation_strategy transformation_strategy = lazy
        )
      :
        datar(p.data(), data::used_data_equation_selector(p.data(), pbes_system::find_function_symbols(p), p.global_variables()), rewrite_strategy),
        m_pbes(preprocess(p)),
        m_equation_index(p),
        R(datar, p.data()),
        m_search_strategy(search_strategy),
        m_transformation_strategy(transformation_strategy),
        m_find_loop_simplifier(m_equation_index, equation)
    {}

    /// \brief Reports BES equations that are produced by the algorithm.
    /// This function is called for every BES equation X = psi with rank k that is produced. By default it does nothing.
    virtual void report_equation(const propositional_variable_instantiation& /* X */, const pbes_expression& /* psi */, std::size_t /* k */)
    {}

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

    /// \brief Runs the algorithm. The result is obtained by calling the function \p get_result.
    virtual void run()
    {
      using utilities::detail::contains;

      std::size_t m_iteration_count = 0;

      init = atermpp::down_cast<propositional_variable_instantiation>(R(m_pbes.initial_state()));
      todo.push_back(init);
      discovered.insert(init);
      while (!todo.empty())
      {
        auto const& X_e = next_todo();

        std::size_t index = m_equation_index.index(X_e.name());
        const pbes_equation& eqn = m_pbes.equations()[index];
        data::rewriter::substitution_type sigma;
        make_pbesinst_substitution(eqn.variable().parameters(), X_e.parameters(), sigma);
        auto const& phi = eqn.formula();
        pbes_expression psi_e = R(phi, sigma);

        // optional step
        psi_e = forward_substitute(psi_e, equation);

        // optional step
        psi_e = simplify_loop(psi_e, eqn.symbol(), X_e);

        // Store and report the new equation
        equation[X_e] = psi_e;
        report_equation(X_e, psi_e, m_equation_index.rank(X_e.name()));

        for (const propositional_variable_instantiation& Y_f: find_propositional_variable_instantiations(psi_e))
        {
          if (!contains(discovered, Y_f))
          {
            todo.push_back(Y_f);
            discovered.insert(Y_f);
          }
          m_pbesinst_backward_substitute.add_dependency(Y_f, X_e);
        }

        // optional step (backward substitution)
        backward_substitute(psi_e, X_e, equation); // N.B. modifies equation

        // optional step
        reset(init, todo, discovered, equation); // N.B. modifies todo and done

        mCRL2log(log::status) << print_equation_count(++m_iteration_count);
        detail::check_bes_equation_limit(m_iteration_count);
      }
    }

    const pbes_equation_index& equation_index() const
    {
      return m_equation_index;
    }

    template <typename Rename>
    pbes compose_result(Rename& rename)
    {
      mCRL2log(log::verbose) << "Generated " << equation.size() << " BES equations in total, outputting BES" << std::endl;
      pbes result;

      // collect the equations in blocks, since they need to be put in the right order
      std::vector<std::vector<pbes_equation> > blocks(m_pbes.equations().size());
      for (auto const& p: equation)
      {
        auto const& X_e = p.first;
        std::size_t index = m_equation_index.index(X_e.name());
        auto const& symbol = this->symbol(index);
        auto lhs = propositional_variable(rename(X_e), data::variable_list());
        auto rhs = replace_propositional_variables(equation[X_e], [&](const propositional_variable_instantiation& x)
          {
            return propositional_variable_instantiation(rename(x), data::data_expression_list());
          }
        );
        blocks[index].push_back(pbes_equation(symbol, lhs, rhs));
        mCRL2log(log::debug) << "Equation: " << symbol << " " << X_e << " = " << equation[X_e] << std::endl;
      }

      // concatenate the blocks
      for (const std::vector<pbes_equation>& block: blocks)
      {
        for (const pbes_equation& eqn: block)
        {
          result.equations().push_back(eqn);
        }
      }

      result.initial_state() = propositional_variable_instantiation(rename(init), data::data_expression_list());
      remove_unreachable_variables(result);
      detail::move_initial_variable_equation_up(result);
      return result;
    }

    /// \brief Returns the computed bes in pbes format
    /// \return The computed bes in pbes format
    pbes get_result(bool short_rename_scheme = true)
    {
      if (short_rename_scheme)
      {
        std::unordered_map<propositional_variable_instantiation, core::identifier_string> rename_map;
        detail::pbesinst_rename_short rename(rename_map);
        return compose_result(rename);
      }
      else
      {
        pbesinst_rename_long rename;
        return compose_result(rename);
      }
    }

    enumerate_quantifiers_rewriter& rewriter()
    {
      return R;
    }
};

inline
pbes pbesinst_lazy(const pbes& p,
                   data::rewriter::strategy rewrite_strategy = data::jitty,
                   search_strategy search_strategy = breadth_first,
                   transformation_strategy transformation_strategy = lazy
                  )
{
  if (search_strategy == breadth_first_short)
  {
    throw mcrl2::runtime_error("The breadth_first_short option is not supported!");
  }
  if (search_strategy == depth_first_short)
  {
    throw mcrl2::runtime_error("The depth_first_short option is not supported!");
  }
  pbesinst_lazy_algorithm algorithm(p, rewrite_strategy, search_strategy, transformation_strategy);
  algorithm.run();
  return algorithm.get_result();
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESINST_LAZY_H
