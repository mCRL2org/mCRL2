// Author(s): Xiao Qi
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
#include <unordered_set>
#include <unordered_map>
#include "mcrl2/utilities/detail/container_utility.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbesinst_algorithm.h"
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/fixpoint_symbol.h"
#include "mcrl2/pbes/detail/bes_equation_limit.h"
#include "mcrl2/pbes/detail/instantiate_global_variables.h"
#include "mcrl2/pbes/rewriters/enumerate_quantifiers_rewriter.h"
#include "mcrl2/pbes/search_strategy.h"
#include "mcrl2/pbes/transformation_strategy.h"

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

namespace detail
{
  class rename_pbesinst_consecutively: public std::unary_function<propositional_variable_instantiation, propositional_variable_instantiation>
  {
    protected:
      std::map<propositional_variable_instantiation,propositional_variable_instantiation> pv_renaming;

    public:
      rename_pbesinst_consecutively(std::vector<std::vector<propositional_variable_instantiation> >& instantiations)
      {
        size_t index=0;
        for(const std::vector<propositional_variable_instantiation>& vec: instantiations)
        {
          for(const propositional_variable_instantiation inst:vec)
          {
            std::stringstream ss;
            ss << "X" << index;
            pv_renaming[inst]=propositional_variable_instantiation(ss.str(),data::data_expression_list());
            index++;
          }
        }
      }

      propositional_variable_instantiation operator()(const propositional_variable_instantiation& v)
      {
        assert(pv_renaming.count(v)>0);
        return pv_renaming[v];
      }
  };
} // end namespace detail


inline mcrl2::pbes_system::pbes_expression pbes_expression_order_quantified_variables(
              const mcrl2::pbes_system::pbes_expression& p, const mcrl2::data::data_specification& data_spec)
{
  using namespace mcrl2;
  using namespace mcrl2::pbes_system;

  if (is_pbes_and(p))
  {
    const and_& pa=atermpp::down_cast<and_>(p);
    return pbes_expr::and_(pbes_expression_order_quantified_variables(pa.left(),data_spec),
                           pbes_expression_order_quantified_variables(pa.right(),data_spec));
  }
  else if (is_pbes_or(p))
  {
    const or_& po=atermpp::down_cast<or_>(p);
    return pbes_expr::or_(pbes_expression_order_quantified_variables(po.left(),data_spec),
                          pbes_expression_order_quantified_variables(po.right(),data_spec));
  }
  else if (is_pbes_imp(p))
  {
    const imp& pi=atermpp::down_cast<imp>(p);
    return pbes_expr::imp(pbes_expression_order_quantified_variables(pi.left(),data_spec),
                          pbes_expression_order_quantified_variables(pi.right(),data_spec));
  }
  else if (is_pbes_not(p))
  {
    return pbes_expr::not_(pbes_expression_order_quantified_variables(atermpp::down_cast<not_>(p).operand(),data_spec));
  }
  else if (is_pbes_forall(p))
  {
    const forall& pf=atermpp::down_cast<forall>(p);
    const pbes_expression expr = pbes_expression_order_quantified_variables(pf.body(),data_spec);
    return pbes_expr::forall(mcrl2::data::order_variables_to_optimise_enumeration(pf.variables(),data_spec),expr);
  }
  else if (is_pbes_exists(p))
  {
    const exists& pe=atermpp::down_cast<exists>(p);
    const pbes_expression expr = pbes_expression_order_quantified_variables(pe.body(),data_spec);
    return pbes_expr::exists(mcrl2::data::order_variables_to_optimise_enumeration(pe.variables(),data_spec),expr);
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
    const data::rewriter& m_datar;

    /// \brief The rewriter.
    enumerate_quantifiers_rewriter R;

    /// \brief The number of generated equations.
    size_t m_equation_count;

    /// \brief Initial value for regeneration_period.
    static const size_t regeneration_count_init = 100;

    /// \brief Propositional variable instantiations that need to be handled.
    std::deque<propositional_variable_instantiation> todo;

    /// \brief The content of todo as a set.
    std::unordered_set<propositional_variable_instantiation> todo_set;

    /// \brief Propositional variable instantiations that have been handled.
    std::unordered_set<propositional_variable_instantiation> done;

    /// \brief Propositional variable instantiations that are reachable from
    ///        init.
    std::unordered_set<propositional_variable_instantiation> reachable;

    /// \brief Map a variable instantiation to a set of other variable
    ///        instantiations on whose right hand sides it appears.
    std::unordered_map<propositional_variable_instantiation, std::unordered_set<propositional_variable_instantiation> > occurrence;

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
    std::vector<size_t> ranks;

    /// \brief The initial value.
    propositional_variable_instantiation init;

    /// \brief A lookup map for PBES equations.
    std::unordered_map<core::identifier_string, size_t> equation_index;

    /// \brief The search strategy to use when exploring the state space.
    search_strategy m_search_strategy;

    /// \brief Transformation strategy.
    transformation_strategy m_transformation_strategy;

    /// \brief Prints a log message for every 1000-th equation
    void print_equation_count(size_t size) const
    {
      if (size > 0 && size % 1000 == 0)
      {
        mCRL2log(log::verbose) << "Generated " << size << " BES equations" << std::endl;
      }
    }

    // renames propositional variables in x
    template <class RENAMER>
    pbes_expression rho(const pbes_expression& x, const RENAMER renamer) const
    {
      // return replace_propositional_variables(x, pbesinst_rename());
      
      return replace_propositional_variables(x, renamer);
    }

  public:

    /// \brief Constructor.
    /// \param data_spec A data specification
    /// \param rewriter_strategy A strategy for the data rewriter
    /// \param print_equations If true, the generated equations are printed
    pbesinst_alternative_lazy_algorithm(
        const data::data_specification& data_spec,
        const data::rewriter& datar,
        search_strategy search_strategy = breadth_first,
        transformation_strategy transformation_strategy = lazy
        )
      :
        m_data_spec(data_spec),
        m_datar(datar),
        R(datar, data_spec),
        m_equation_count(0),
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
        const propositional_variable_instantiation X_e = todo.front();
        todo.pop_front();
        return X_e;
      }
      else
      {
        const propositional_variable_instantiation X_e = todo.back();
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

    size_t get_rank(propositional_variable_instantiation X)
    {
      return ranks[equation_index[X.name()]];
    }

    template <bool is_mu>
    bool find_loop_rec(
        pbes_expression expr,
        propositional_variable_instantiation X,
        size_t rank,
        std::unordered_map<propositional_variable_instantiation, bool>& visited)
    {
      if (is_false(expr) || is_true(expr))
      {
        return false;
      }
      if (is_propositional_variable_instantiation(expr))
      {
        const propositional_variable_instantiation Y = atermpp::vertical_cast<propositional_variable_instantiation>(expr);
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

      if (is_mu)
      {
        if (is_and(expr))
        {
          const and_& expra=atermpp::down_cast<and_>(expr);
          return find_loop_rec<is_mu>(expra.left(), X, rank, visited) ||
                 find_loop_rec<is_mu>(expra.right(), X, rank, visited);
        }
        if (is_or(expr))
        {
          const or_& expro=atermpp::down_cast<or_>(expr);
          return find_loop_rec<is_mu>(expro.left(), X, rank, visited) &&
                 find_loop_rec<is_mu>(expro.right(), X, rank, visited);
        }
      }
      else
      {
        if (is_and(expr))
        {
          const and_& expra=atermpp::down_cast<and_>(expr);
          return find_loop_rec<is_mu>(expra.left(), X, rank, visited) &&
                 find_loop_rec<is_mu>(expra.right(), X, rank, visited);
        }
        if (is_or(expr))
        {
          const or_& expro=atermpp::down_cast<or_>(expr);
          return find_loop_rec<is_mu>(expro.left(), X, rank, visited) ||
                 find_loop_rec<is_mu>(expro.right(), X, rank, visited);
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
          const propositional_variable_instantiation X = atermpp::vertical_cast<propositional_variable_instantiation>(expr);
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
        else if (is_and(expr))
        {
          const and_& expra=atermpp::down_cast<and_>(expr);
          stack.push(expra.left());
          stack.push(expra.right());
        }
        else if (is_or(expr))
        {
          const or_& expro=atermpp::down_cast<or_>(expr);
          stack.push(expro.left());
          stack.push(expro.right());
        }
      }
    }

    // The function below simplifies an boolean_expression, given the knowledge that some propositional variables in trivial
    // are known to be true or false. The idea is that variables that are redundant can be removed. If p = p1 && p2, and p1 is
    // false, then p2 can be removed, as its value does not influence the rewrite system. 
    // The result of the function is a pair, with the simplified expression as first term, and the expression that is rewritten under the 
    // simplifications in trivial as the second term.
    typedef std::pair < pbes_expression, pbes_expression > pbes_expression_pair;
    pbes_expression_pair simplify_pbes_expression(const pbes_expression& p, const std::unordered_map<propositional_variable_instantiation, pbes_expression>& trivial)
    {
      if (is_propositional_variable_instantiation(p))
      {
        const std::unordered_map<propositional_variable_instantiation, pbes_expression>::const_iterator i=
                                            trivial.find(atermpp::down_cast<propositional_variable_instantiation>(p));
        if (i!=trivial.end() && (is_true(i->second) || is_false(i->second)))
        {
          return pbes_expression_pair(p,i->second);
        }
        return pbes_expression_pair(p,p);
      }
      else if (is_true(p)||is_false(p))
      {
        return pbes_expression_pair(p,p);
      }
      else if (is_and(p))
      {
        const and_& pa=atermpp::down_cast<and_>(p);
        const pbes_expression_pair lhs=simplify_pbes_expression(pa.left(),trivial);
        const pbes_expression_pair rhs=simplify_pbes_expression(pa.right(),trivial);
        if (is_false(lhs.second))
        {
          return lhs;
        }
        if (is_false(rhs.second))
        {
          return rhs;
        }
        if (is_true(lhs.second))
        {
          return pbes_expression_pair(and_(lhs.first,rhs.first),rhs.second);
        }
        if (is_true(rhs.second))
        {
          return pbes_expression_pair(and_(lhs.first,rhs.first),lhs.second);
        }
        return pbes_expression_pair(and_(lhs.first,rhs.first),and_(lhs.first,rhs.first));
      }
      assert(is_or(p));
      const or_& po=atermpp::down_cast<or_>(p);
      const pbes_expression_pair lhs=simplify_pbes_expression(po.left(),trivial);
      const pbes_expression_pair rhs=simplify_pbes_expression(po.right(),trivial);
      if (is_true(lhs.second))
      {
        return lhs;
      }
      if (is_true(rhs.second))
      {
        return rhs;
      }
      if (is_false(lhs.second))
      {
        return pbes_expression_pair(or_(lhs.first,rhs.first),rhs.second);
      }
      if (is_false(rhs.second))
      {
        return pbes_expression_pair(or_(lhs.first,rhs.first),lhs.second);;
      }
      return pbes_expression_pair(or_(lhs.first,rhs.first),or_(lhs.first,rhs.first));
    }
 


    /// \brief Runs the algorithm. The result is obtained by calling the function \p get_result.
    /// \param p A PBES
    void run(pbes& p)
    {
      using utilities::detail::pick_element;
      using utilities::detail::contains;

      size_t regeneration_count=regeneration_count_init;
      pbes_system::detail::instantiate_global_variables(p);

      std::vector<pbes_equation>& pbes_equations = p.equations();

      // simplify all right hand sides of p
      //
      // NOTE: This is not just an optimization. There are certain PBES
      // equations for which applying enumerate_quantifiers_rewriter directly
      // won't terminate, like:
      //
      // forall m: Nat . exists k: Nat . val(m == k)
      pbes_system::one_point_rule_rewriter one_point_rule_rewriter;
      pbes_system::simplify_quantifiers_data_rewriter<mcrl2::data::rewriter> simplify_rewriter(m_datar);
      for (pbes_equation eq: pbes_equations)
      {
        eq.formula() = pbes_expression_order_quantified_variables(one_point_rule_rewriter(simplify_rewriter(eq.formula())), m_data_spec);
      }

      // initialize equation_index, instantiations, symbols and ranks
      size_t eqn_index = 0;
      ranks.resize(pbes_equations.size());
      instantiations.resize(pbes_equations.size());
      for (std::vector<pbes_equation>::const_iterator i = pbes_equations.begin(); i != pbes_equations.end(); ++i, ++eqn_index)
      {
        const pbes_equation& eqn = *i;
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
        const propositional_variable_instantiation X_e = next_todo();
        size_t index = equation_index[X_e.name()];
        done.insert(X_e);
        instantiations[index].push_back(X_e);

        const pbes_equation& eqn = pbes_equations[index];
        data::rewriter::substitution_type sigma;
        make_pbesinst_substitution(eqn.variable().parameters(), X_e.parameters(), sigma);
        const pbes_expression& phi = eqn.formula();
        pbes_expression psi_e = R(phi, sigma);
        pbes_expression rewritten_psi_e;

        if (m_transformation_strategy >= optimize)
        {
          // Substitute all trivial variable instantiations by their values
          pbes_expression_pair p=simplify_pbes_expression(psi_e,trivial);
          psi_e=p.first;
          rewritten_psi_e=p.second;
        }
        else
        {
          rewritten_psi_e=psi_e;
        }
        // Store the result
        equation[X_e] = psi_e;

        if (m_transformation_strategy >= on_the_fly_with_fixed_points)
        {
          // Find mu or nu loop
          if (eqn.symbol() == fixpoint_symbol::mu())
          {
            if (find_loop<true>(psi_e, X_e))
            {
              rewritten_psi_e = false_();
            }
          }
          else
          {
            if (find_loop<false>(psi_e, X_e))
            {
              rewritten_psi_e = true_();
            }
          }
        }

        // Add all variable instantiations in psi_e to todo and generated,
        // and augment the occurrence sets
        std::set<propositional_variable_instantiation> psi_variables = find_propositional_variable_instantiations(psi_e);
        for (propositional_variable_instantiation v: psi_variables)
        {
          if (todo_set.count(v) == 0 && done.count(v) == 0)
          {
            add_todo(v);
          }
          occurrence[v].insert(X_e);
        }

        if (m_transformation_strategy >= optimize && (is_true(rewritten_psi_e) || is_false(rewritten_psi_e)))
        {
          trivial[X_e] = rewritten_psi_e;
          if (m_transformation_strategy >= on_the_fly)
          {
            // Substitute X_e to its value in all its occurrences, and
            // substitute all other variables to their values that are found
            // to be either true or false in all their occurrences.
            std::stack<propositional_variable_instantiation> new_trivials;
            new_trivials.push(X_e);
            while (!new_trivials.empty())
            {
              const propositional_variable_instantiation X = new_trivials.top();
              new_trivials.pop();

              const std::unordered_set<propositional_variable_instantiation> oc = occurrence[X];
              std::unordered_map<propositional_variable_instantiation, pbes_expression> trivial_X;
              trivial_X[X] = psi_e;
              for (const propositional_variable_instantiation Y: oc)
              {
                pbes_expression_pair p=simplify_pbes_expression(equation[Y],trivial);
                equation[Y]=p.first;
                const pbes_expression f=p.second;
                if (is_true(f) || is_false(f))
                {
                  trivial[Y] = f;
                  new_trivials.push(Y);
                }
              }
              occurrence.erase(X);
            }
          }
        }

        if (m_transformation_strategy >= on_the_fly)
        {
          if (--regeneration_count == 0 || trivial.count(init)>0 )
          {
            regeneration_count = equation.size() / 2;
            regenerate_states();
          }
        }

        m_equation_count++;
        print_equation_count(m_equation_count); // Print the number of equations in verbose mode once for every thousand equations. 
        detail::check_bes_equation_limit(m_equation_count);
      }
    }

    /// \brief Returns the computed bes in pbes format
    /// \return The computed bes in pbes format
    pbes get_result(bool short_rename_scheme=true)
    {
      mCRL2log(log::verbose) << "Generated " << m_equation_count << " BES equations in total, generating BES" << std::endl;
      pbes result;
      size_t index = 0;
      pbesinst_rename long_renamer;
      detail::rename_pbesinst_consecutively short_renamer(instantiations);
      for (const std::vector<propositional_variable_instantiation> vec: instantiations)
      {
        const fixpoint_symbol symbol = symbols[index++];
        for (propositional_variable_instantiation X_e: vec)
        {
          if (reachable.count(X_e) == 0)
          {
            continue;
          }
          const propositional_variable lhs = 
                            (short_rename_scheme?
                                   propositional_variable(short_renamer(X_e).name(), data::variable_list()):
                                   propositional_variable(long_renamer(X_e).name(), data::variable_list()));
          const pbes_expression rhs = 
                            (short_rename_scheme?
                                    rho(equation[X_e],short_renamer):
                                    rho(equation[X_e],long_renamer));
          result.equations().push_back(pbes_equation(symbol, lhs, rhs));
          mCRL2log(log::debug) << "Equation: " << atermpp::aterm(symbol) << " " << X_e << " = " << equation[X_e] << std::endl;
        }
      }

      result.initial_state() = (short_rename_scheme?short_renamer(init):long_renamer(init));
      return result;
    }

    enumerate_quantifiers_rewriter& rewriter()
    {
      return R;
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESINST_ALTERNATIVE_LAZY_ALGORITHM_H
