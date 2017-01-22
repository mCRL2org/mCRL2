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
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbesinst_algorithm.h"
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/fixpoint_symbol.h"
#include "mcrl2/pbes/detail/bes_equation_limit.h"
#include "mcrl2/pbes/detail/instantiate_global_variables.h"
#include "mcrl2/pbes/rewriters/enumerate_quantifiers_rewriter.h"
#include "mcrl2/pbes/rewriters/propositional_variable_rewriter.h"
#include "mcrl2/pbes/search_strategy.h"
#include "mcrl2/pbes/transformation_strategy.h"
#include "mcrl2/utilities/detail/container_utility.h"

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

/// \brief Returns true if map/set m has x as a key
template <typename Map>
bool has_key(const Map& m, const typename Map::key_type& x)
{
  return m.find(x) != m.end();
}

struct pbes_equation_index
{
  std::unordered_map<core::identifier_string, std::size_t> equation_index;

  pbes_equation_index()
  { }

  pbes_equation_index(const pbes& p)
  {
    auto const& equations = p.equations();
    for (auto i = equations.begin(); i != equations.end(); ++i)
    {
      equation_index[i->variable().name()] = i - equations.begin();
    }
  }

  std::size_t operator[](const core::identifier_string& x) const
  {
    auto i = equation_index.find(x);
    return i->second;
  }
};

struct find_loop_simplifier
{
  /// \brief A lookup map for PBES equations.
  const pbes_equation_index& equation_index;

  /// \brief Map a variable instantiation to its right hand side.
  const std::unordered_map<propositional_variable_instantiation, pbes_expression>& equation;

  /// \brief ranks[i] contains the rank of the i-th equation in the PBES.
  std::vector<std::size_t> ranks;

  std::size_t get_rank(const propositional_variable_instantiation& X) const
  {
    return ranks[equation_index[X.name()]];
  }

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
      else if (get_rank(Y) != rank)
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
    return find_loop_rec<is_mu>(expr, X, get_rank(X), visited);
  }

  find_loop_simplifier(const std::vector<pbes_equation>& equations,
                       const pbes_equation_index& equation_index_,
                       const std::unordered_map<propositional_variable_instantiation, pbes_expression>& equation_
                      )
    : equation_index(equation_index_),
      equation(equation_)
  {
    // initialize ranks
    std::size_t rank = 0;
    ranks.push_back(rank);
    for (std::size_t i = 1; i < equations.size(); i++)
    {
      if (equations[i - 1].symbol() == equations[i].symbol())
      {
        ranks.push_back(rank);
      }
      else
      {
        ranks.push_back(++rank);
      }
    }
  }

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
    }
  }
};

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

struct true_false_simplifier
{
  void operator()(transformation_strategy strategy,
                  const pbes_expression& psi_e,
                  const propositional_variable_instantiation& X_e,
                  std::unordered_map<propositional_variable_instantiation, std::vector<propositional_variable_instantiation> >& justification,
                  std::unordered_map<propositional_variable_instantiation, std::unordered_set<propositional_variable_instantiation> >& occurrence,
                  std::unordered_map<propositional_variable_instantiation, pbes_expression>& equation
                 )
  {
    if (strategy >= optimize && (is_true(psi_e) || is_false(psi_e)))
    {
      if (strategy >= on_the_fly)
      {
        // Substitute X_e to its value in all its occurrences, and
        // substitute all other variables to their values that are found
        // to be either true or false in all their occurrences.
        std::unordered_set<propositional_variable_instantiation> new_trivials;
        new_trivials.insert(X_e);
        while (!new_trivials.empty())
        {
          auto X = *new_trivials.begin();
          new_trivials.erase(new_trivials.begin());

          auto oc = occurrence[X];
          for (auto i = oc.begin(); i != oc.end(); i++)
          {
            auto Y = *i;
            pbes_expression& f = equation[Y];
            f = make_propositional_variable_rewriter(equation, justification[Y])(f);
            if (is_true(f) || is_false(f))
            {
              new_trivials.insert(Y);
            }
          }
          occurrence.erase(X);
        }
      }
    }
  }
};

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
    pbes_equation_index equation_index;

    /// \brief The rewriter.
    enumerate_quantifiers_rewriter R;

    /// \brief Propositional variable instantiations that need to be handled.
    todo_list todo;

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
    std::unordered_map<propositional_variable_instantiation, std::unordered_set<propositional_variable_instantiation> > occurrence;

    /// \brief Map a variable instantiation to its right hand side.
    std::unordered_map<propositional_variable_instantiation, pbes_expression> equation;

    /// \brief instantiations[i] contains all instantiations of the variable
    ///        of the i-th equation in the PBES.
    std::vector<std::vector<propositional_variable_instantiation> > instantiations;

    /// \brief The initial value.
    propositional_variable_instantiation init;

    /// \brief The search strategy to use when exploring the state space.
    search_strategy m_search_strategy;

    /// \brief Transformation strategy.
    transformation_strategy m_transformation_strategy;

    /// \brief Simplifies expressions based on a loop analysis.
    find_loop_simplifier m_find_loop_simplifier;

    /// \brief Simplifies some attributes based on variables that have the value true or false.
    true_false_simplifier m_true_false_simplifier;

    /// \brief Simplifies some attributes based on variables that have the value true or false.
    pbesinst_resetter m_pbesinst_resetter;

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
        eq.formula() = pbes_expression_order_quantified_variables(one_point_rule_rewriter(simplify_rewriter(eq.formula())), m_pbes.data());
      }
      return p;
    }

    pbes_expression substitute_trivial_values(const pbes_expression& psi_e,
                                              const propositional_variable_instantiation& X_e,
                                              const std::unordered_map<propositional_variable_instantiation, pbes_expression>& equation,
                                              std::unordered_map<propositional_variable_instantiation, std::vector<propositional_variable_instantiation> >& justification
                                             )
    {
      if (m_transformation_strategy >= optimize)
      {
        // Substitute all trivial variable instantiations by their values
        return make_propositional_variable_rewriter(equation, justification[X_e])(psi_e);
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

    void true_false_simplify(const pbes_expression& psi_e,
                             const propositional_variable_instantiation& X_e,
                             std::unordered_map<propositional_variable_instantiation, std::vector<propositional_variable_instantiation> >& justification,
                             std::unordered_map<propositional_variable_instantiation, std::unordered_set<propositional_variable_instantiation> >& occurrence,
                             std::unordered_map<propositional_variable_instantiation, pbes_expression>& equation
                            )
    {
      if (m_transformation_strategy >= optimize && (is_true(psi_e) || is_false(psi_e)))
      {
        m_true_false_simplifier(m_transformation_strategy, psi_e, X_e, justification, occurrence, equation);
      }
    }

    void reset(const propositional_variable_instantiation& init,
               todo_list& todo,
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
    /// \param rewriter_strategy A strategy for the data rewriter
    /// \param print_equations If true, the generated equations are printed
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
    /// \param p A PBES
    void run()
    {
      auto& pbes_equations = m_pbes.equations();
      std::size_t m_iteration_count = 0;
      instantiations.resize(m_pbes.equations().size());

      init = atermpp::down_cast<propositional_variable_instantiation>(R(m_pbes.initial_state()));
      todo.push_back(init);
      while (!todo.empty())
      {
        auto const& X_e = next_todo();
        std::size_t index = equation_index[X_e.name()];
        instantiations[index].push_back(X_e);

        const pbes_equation& eqn = pbes_equations[index];
        data::rewriter::substitution_type sigma;
        make_pbesinst_substitution(eqn.variable().parameters(), X_e.parameters(), sigma);
        auto const& phi = eqn.formula();
        pbes_expression psi_e = R(phi, sigma);

        // optional step
        psi_e = substitute_trivial_values(psi_e, X_e, equation, justification); // N.B. modifies justification

        // optional step
        psi_e = simplify_loop(psi_e, eqn.symbol(), X_e);

        // Add all variable instantiations in psi_e to todo and generated,
        // and augment the occurrence sets
        for (const propositional_variable_instantiation& v: find_propositional_variable_instantiations(psi_e))
        {
          if (!todo.contains(v) && !has_key(equation, v))
          {
            todo.push_back(v);
          }
          occurrence[v].insert(X_e);
        }

        // Store the result
        equation[X_e] = psi_e;

        // optional step
        true_false_simplify(psi_e, X_e, justification, occurrence, equation); // N.B. modifies equation, justification, occurrence

        // optional step
        reset(init, todo, equation); // N.B. modifies todo

        mCRL2log(log::verbose) << print_equation_count(++m_iteration_count);
        detail::check_bes_equation_limit(m_iteration_count);
      }
    }

    /// \brief Returns the computed bes in pbes format
    /// \return The computed bes in pbes format
    pbes get_result()
    {
      mCRL2log(log::verbose) << "Generated " << equation.size() << " BES equations in total, outputting BES" << std::endl;
      pbes result;
      std::size_t index = 0;
      for (auto i = instantiations.begin(); i != instantiations.end(); i++)
      {
        auto symbol = this->symbol(index++);
        for (auto j = i->begin(); j != i->end(); j++)
        {
          auto X_e = *j;
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
