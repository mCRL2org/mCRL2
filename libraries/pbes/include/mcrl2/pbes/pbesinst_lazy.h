// Author(s): Jan Friso Groote
//            Xiao Qi
//            Wieger Wesselink 2017
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
#include "mcrl2/pbes/remove_equations.h"
#include "mcrl2/pbes/pbesinst_algorithm.h"
#include "mcrl2/pbes/detail/bes_equation_limit.h"
#include "mcrl2/pbes/detail/instantiate_global_variables.h"
#include "mcrl2/pbes/rewriters/enumerate_quantifiers_rewriter.h"
#include "mcrl2/pbes/rewriters/one_point_rule_rewriter.h"
#include "mcrl2/pbes/rewriters/simplify_quantifiers_rewriter.h"
#include "mcrl2/pbes/rewriters/simplify_rewriter.h"
#include "mcrl2/pbes/search_strategy.h"
#include "mcrl2/pbes/transformation_strategy.h"
#include "mcrl2/utilities/detail/container_utility.h"
#include "mcrl2/utilities/text_utility.h"

#ifndef MCRL2_PBES_PBESINST_LAZY_H
#define MCRL2_PBES_PBESINST_LAZY_H

namespace std
{
template <>
struct hash<mcrl2::pbes_system::propositional_variable_instantiation>
{
  std::size_t operator()(const mcrl2::pbes_system::propositional_variable_instantiation& x) const
  {
    return hash<atermpp::aterm>()(x);
  }
};
}

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

  pbesinst_rename_short(std::unordered_map<propositional_variable_instantiation, core::identifier_string>& name_map_)
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

template <template <class> class Builder, class Derived, class Map, class Justification>
struct add_forward_substitute_rewriter: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::apply;

  typedef core::term_traits<pbes_expression> tr;

  const Map &map;
  Justification &justification;

  add_forward_substitute_rewriter(const Map& m, Justification &j)
    : map(m), justification(j)
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
      justification.push_back(x);
      return p->second;
    }
  }
};

template <typename Map, typename Justification>
struct forward_substitute_rewriter_builder: public add_forward_substitute_rewriter<pbes_system::detail::simplify_builder, forward_substitute_rewriter_builder<Map, Justification>, Map, Justification>
{
  typedef add_forward_substitute_rewriter<pbes_system::detail::simplify_builder, forward_substitute_rewriter_builder<Map, Justification>, Map, Justification> super;
  forward_substitute_rewriter_builder(const Map& m, Justification &j)
    : super(m, j)
  {}
};

template <typename Map, typename Justification>
struct forward_substitute_rewriter
{
  const Map& map;
  Justification &justification;

  forward_substitute_rewriter(const Map& m, Justification &j)
    : map(m), justification(j)
  {}

  pbes_expression operator()(const pbes_expression& x) const
  {
    return forward_substitute_rewriter_builder<Map, Justification>(map, justification).apply(x);
  }
};

template <typename Map, typename Justification>
forward_substitute_rewriter<Map, Justification>
make_forward_substitute_rewriter(const Map &m, Justification &j)
{
  return forward_substitute_rewriter<Map, Justification>(m, j);
}

/// \brief Returns true if map/set m has x as a key
template <typename Map>
bool has_key(const Map& m, const typename Map::key_type& x)
{
  return m.find(x) != m.end();
}

struct pbes_equation_index
{
  // maps the name of an equation to the pair (i, k) with i the corresponding index of the equation, and k the rank
  std::unordered_map<core::identifier_string, std::pair<std::size_t, std::size_t> > equation_index;

  pbes_equation_index()
  { }

  pbes_equation_index(const pbes& p)
  {
    std::size_t rank = 0;
    auto const& equations = p.equations();
    for (std::size_t i = 0; i < equations.size(); i++)
    {
      const auto& eqn = equations[i];
      std::size_t k;
      if (i == 0)
      {
        k = 0;
      }
      else
      {
        k = (equations[i - 1].symbol() == equations[i].symbol()) ? rank : ++rank;
      }
      equation_index.insert({eqn.variable().name(), std::make_pair(i, k)});
    }
  }

  /// \brief Returns the index of the equation of the variable with the given name
  std::size_t index(const core::identifier_string& name) const
  {
    auto i = equation_index.find(name);
    assert (i != equation_index.end());
    return i->second.first;
  }

  /// \brief Returns the rank of the equation of the variable with the given name
  std::size_t rank(const core::identifier_string& name) const
  {
    auto i = equation_index.find(name);
    assert (i != equation_index.end());
    return i->second.second;
  }
};

struct find_loop_simplifier
{
  /// \brief A lookup map for PBES equations.
  const pbes_equation_index& equation_index;

  /// \brief Map a variable instantiation to its right hand side.
  const std::unordered_map<propositional_variable_instantiation, pbes_expression>& equation;

  template <bool is_mu>
  bool find_loop_rec(const pbes_expression& expr,
                     propositional_variable_instantiation X,
                     std::size_t rank,
                     std::unordered_map<propositional_variable_instantiation, bool>& visited
                    ) const
  {
    if (is_false(expr) || is_true(expr))
    {
      return false;
    }
    if (is_propositional_variable_instantiation(expr))
    {
      auto const& Y = atermpp::down_cast<propositional_variable_instantiation>(expr);
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
      return visited[Y] = find_loop_rec<is_mu>(equation.at(Y), X, rank, visited);
    }

    if (is_mu)
    {
      if (is_and(expr))
      {
        const and_& expra = atermpp::down_cast<and_>(expr);
        return find_loop_rec<is_mu>(expra.left(), X, rank, visited) ||
               find_loop_rec<is_mu>(expra.right(), X, rank, visited);
      }
      if (is_or(expr))
      {
        const or_& expro = atermpp::down_cast<or_>(expr);
        return find_loop_rec<is_mu>(expro.left(), X, rank, visited) &&
               find_loop_rec<is_mu>(expro.right(), X, rank, visited);
      }
    }
    else
    {
      if (is_and(expr))
      {
        const and_& expra = atermpp::down_cast<and_>(expr);
        return find_loop_rec<is_mu>(expra.left(), X, rank, visited) &&
               find_loop_rec<is_mu>(expra.right(), X, rank, visited);
      }
      if (is_or(expr))
      {
        const or_& expro = atermpp::down_cast<or_>(expr);
        return find_loop_rec<is_mu>(expro.left(), X, rank, visited) ||
               find_loop_rec<is_mu>(expro.right(), X, rank, visited);
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

  find_loop_simplifier(const std::vector<pbes_equation>& equations,
                       const pbes_equation_index& equation_index_,
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

struct todo_list
{
  /// \brief Propositional variable instantiations that need to be handled.
  std::deque<propositional_variable_instantiation> todo;

  /// \brief The content of todo as a set.
  std::unordered_set<propositional_variable_instantiation> todo_set;

  bool empty() const
  {
    return todo.empty();
  }

  const propositional_variable_instantiation& front() const
  {
    return todo.front();
  }

  const propositional_variable_instantiation& back() const
  {
    return todo.back();
  }

  void pop_front()
  {
    todo.pop_front();
  }

  void pop_back()
  {
    todo.pop_back();
  }

  void push_back(const propositional_variable_instantiation& x)
  {
    todo.push_back(x);
    todo_set.insert(x);
  }

  bool contains(const propositional_variable_instantiation& x) const
  {
    return todo_set.find(x) != todo_set.end();
  }

  void clear()
  {
    todo.clear();
    todo_set.clear();
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
                  todo_list& todo,
                  const std::unordered_map<propositional_variable_instantiation, pbes_expression>& equation
                 )
  {
    if (++regeneration_count == regeneration_period)
    {
      regeneration_count = 0;
      regeneration_period = equation.size() / 2;
    }
    else
    {
      return;
    }

    // Propositional variable instantiations that are reachable from init.
    std::unordered_set<propositional_variable_instantiation> reachable;
    todo.clear();

    std::stack<pbes_expression> stack;
    stack.push(init);

    while (!stack.empty())
    {
      const pbes_expression expr = stack.top();
      stack.pop();

      if (is_propositional_variable_instantiation(expr))
      {
        auto X = atermpp::down_cast<propositional_variable_instantiation>(expr);
        if (!has_key(reachable, X))
        {
          if (has_key(equation, X))
          {
            stack.push(equation.at(X));
            reachable.insert(X);
          }
          else
          {
            todo.push_back(X);
            reachable.insert(X);
          }
        }
      }
      else if (is_and(expr))
      {
        const and_& expra = atermpp::down_cast<and_>(expr);
        stack.push(expra.left());
        stack.push(expra.right());
      }
      else if (is_or(expr))
      {
        const or_& expro = atermpp::down_cast<or_>(expr);
        stack.push(expro.left());
        stack.push(expro.right());
      }
      else if (is_imp(expr))
      {
        const or_& expro = atermpp::down_cast<or_>(expr);
        stack.push(expro.left());
        stack.push(expro.right());
      }
      else if (is_not(expr))
      {
        const not_& y = atermpp::down_cast<not_>(expr);
        stack.push(y.operand());
      }
    }
  }
};

inline
pbes_expression pbes_expression_order_quantified_variables(const mcrl2::pbes_system::pbes_expression& p, const mcrl2::data::data_specification& data_spec)
{
  if (is_pbes_and(p))
  {
    const and_& pa=atermpp::down_cast<and_>(p);
    return and_(pbes_expression_order_quantified_variables(pa.left(),data_spec),
                pbes_expression_order_quantified_variables(pa.right(),data_spec));
  }
  else if (is_pbes_or(p))
  {
    const or_& po=atermpp::down_cast<or_>(p);
    return or_(pbes_expression_order_quantified_variables(po.left(),data_spec),
               pbes_expression_order_quantified_variables(po.right(),data_spec));
  }
  else if (is_pbes_imp(p))
  {
    const imp& pi=atermpp::down_cast<imp>(p);
    return imp(pbes_expression_order_quantified_variables(pi.left(),data_spec),
               pbes_expression_order_quantified_variables(pi.right(),data_spec));
  }
  else if (is_pbes_not(p))
  {
    return not_(pbes_expression_order_quantified_variables(atermpp::down_cast<not_>(p).operand(),data_spec));
  }
  else if (is_pbes_forall(p))
  {
    const forall& pf=atermpp::down_cast<forall>(p);
    const pbes_expression expr = pbes_expression_order_quantified_variables(pf.body(),data_spec);
    return make_forall(mcrl2::data::order_variables_to_optimise_enumeration(pf.variables(),data_spec),expr);
  }
  else if (is_pbes_exists(p))
  {
    const exists& pe=atermpp::down_cast<exists>(p);
    const pbes_expression expr = pbes_expression_order_quantified_variables(pe.body(),data_spec);
    return make_exists(mcrl2::data::order_variables_to_optimise_enumeration(pe.variables(),data_spec),expr);
  }
  else
  {
    return p;
  }
}

struct pbesinst_backward_substitute
{
  /// \brief Map a variable instantiation to a set of other variable instantiations on whose right hand sides it appears.
  std::unordered_map<propositional_variable_instantiation, std::unordered_set<propositional_variable_instantiation> > occurrence;

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
                  std::unordered_map<propositional_variable_instantiation, std::vector<propositional_variable_instantiation> >& justification,
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

          auto oc = occurrence[X];
          for (auto i = oc.begin(); i != oc.end(); i++)
          {
            auto Y = *i;
            pbes_expression& f = equation[Y];
            f = make_forward_substitute_rewriter(equation, justification[Y])(f);
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
    detail::pbes_equation_index equation_index;

    /// \brief The rewriter.
    enumerate_quantifiers_rewriter R;

    /// \brief Propositional variable instantiations that need to be handled.
    detail::todo_list todo;

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
        eq.formula() = detail::pbes_expression_order_quantified_variables(one_point_rule_rewriter(simplify_rewriter(eq.formula())), p.data());
      }
      return p;
    }

    pbes_expression forward_substitute(const pbes_expression& psi_e,
                                       const propositional_variable_instantiation& X_e,
                                       const std::unordered_map<propositional_variable_instantiation, pbes_expression>& equation,
                                       std::unordered_map<propositional_variable_instantiation, std::vector<propositional_variable_instantiation> >& justification
                                      )
    {
      if (m_transformation_strategy >= optimize)
      {
        // Substitute all trivial variable instantiations by their values
        return detail::make_forward_substitute_rewriter(equation, justification[X_e])(psi_e);
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
                             std::unordered_map<propositional_variable_instantiation, std::vector<propositional_variable_instantiation> >& justification,
                             std::unordered_map<propositional_variable_instantiation, pbes_expression>& equation
                            )
    {
      if (m_transformation_strategy >= optimize && (is_true(psi_e) || is_false(psi_e)))
      {
        m_pbesinst_backward_substitute(m_transformation_strategy, psi_e, X_e, justification, equation);
      }
    }

    void reset(const propositional_variable_instantiation& init,
               detail::todo_list& todo,
               const std::unordered_map<propositional_variable_instantiation, pbes_expression>& equation
              )
    {
      if (m_transformation_strategy >= on_the_fly)
      {
        m_pbesinst_resetter(init, todo, equation);
      }
    }

  public:

    /// \brief Constructor.
    /// \param p The pbes used in the exploration algorithm.
    /// \param rewrite_strategy A strategy for the data rewriter.
    /// \param search_strategy The search strategy used to explore the pbes, typically depth or breadth first.
    /// \param transformation_strategy The strategy that determines to which extent the generated bes is simplified while being generated.
    pbesinst_lazy_algorithm(
         const pbes& p,
         data::rewriter::strategy rewrite_strategy = data::jitty,
         search_strategy search_strategy = breadth_first,
         transformation_strategy transformation_strategy = lazy
        )
      :
        datar(p.data(), rewrite_strategy),
        m_pbes(preprocess(p)),
        equation_index(p),
        R(datar, p.data()),
        m_search_strategy(search_strategy),
        m_transformation_strategy(transformation_strategy),
        m_find_loop_simplifier(p.equations(), equation_index, equation)
    {}

    /// \brief Reports BES equations that are produced by the algorithm.
    /// This function is called for every BES equation X = psi with rank k that is produced. By default it does nothing.
    virtual void report_equation(const propositional_variable_instantiation& /* X */, const pbes_expression& /* psi */, std::size_t /* k */) const
    {}

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

    const fixpoint_symbol& symbol(std::size_t i) const
    {
      return m_pbes.equations()[i].symbol();
    }

    /// \brief Runs the algorithm. The result is obtained by calling the function \p get_result.
    void run()
    {
      std::size_t m_iteration_count = 0;

      init = atermpp::down_cast<propositional_variable_instantiation>(R(m_pbes.initial_state()));
      todo.push_back(init);
      while (!todo.empty())
      {
        auto const& X_e = next_todo();
        std::size_t index = equation_index.index(X_e.name());

        const pbes_equation& eqn = m_pbes.equations()[index];
        data::rewriter::substitution_type sigma;
        make_pbesinst_substitution(eqn.variable().parameters(), X_e.parameters(), sigma);
        auto const& phi = eqn.formula();
        pbes_expression psi_e = R(phi, sigma);

        // optional step
        psi_e = forward_substitute(psi_e, X_e, equation, justification); // N.B. modifies justification

        // optional step
        psi_e = simplify_loop(psi_e, eqn.symbol(), X_e);

        for (const propositional_variable_instantiation& v: find_propositional_variable_instantiations(psi_e))
        {
          if (!todo.contains(v) && !detail::has_key(equation, v))
          {
            todo.push_back(v);
          }
          m_pbesinst_backward_substitute.add_dependency(v, X_e);
        }

        // Store the result
        equation[X_e] = psi_e;
        report_equation(X_e, psi_e, equation_index.rank(X_e.name()));

        // optional step (backward substitution)
        backward_substitute(psi_e, X_e, justification, equation); // N.B. modifies equation, justification

        // optional step
        reset(init, todo, equation); // N.B. modifies todo

        mCRL2log(log::status) << print_equation_count(++m_iteration_count);
        detail::check_bes_equation_limit(m_iteration_count);
      }
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
        std::size_t index = equation_index.index(X_e.name());
        auto const& symbol = this->symbol(index);
        auto lhs = propositional_variable(rename(X_e), data::variable_list());
        auto rhs = replace_propositional_variables(equation[X_e], [&](const propositional_variable_instantiation& x)
          {
            return propositional_variable_instantiation(rename(x), data::data_expression_list());
          }
        );
        blocks[index].push_back(pbes_equation(symbol, lhs, rhs));
        mCRL2log(log::debug) << "Equation: " << symbol << " " << X_e << " = " << equation[X_e] << std::endl;
        mCRL2log(log::debug) << "Obtained justification of " << X_e << ": " << core::detail::print_container(justification[X_e]) << std::endl;
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
