// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbesinst_structure_graph2.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_PBESINST_STRUCTURE_GRAPH2_H
#define MCRL2_PBES_PBESINST_STRUCTURE_GRAPH2_H

#include <utility>
#include "mcrl2/data/undefined.h"
#include "mcrl2/pbes/pbesinst_fatal_attractors.h"
#include "mcrl2/pbes/pbesinst_find_loops.h"
#include "mcrl2/pbes/pbesinst_partial_solve.h"
#include "mcrl2/pbes/pbessolve_attractors.h"
#include "mcrl2/pbes/replace.h"
#include "mcrl2/pbes/simple_structure_graph.h"
#include "mcrl2/pbes/pbesinst_structure_graph.h"
#include "mcrl2/pbes/traverser.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

class computation_guard
{
  protected:
     std::size_t m_count = 64;

  public:
    explicit computation_guard(std::size_t initial_count = 64)
      : m_count(initial_count)
    {}

    bool operator()(std::size_t count)
    {
      bool result = count >= m_count;
      while (m_count <= count)
      {
        m_count *= 2;
      }
      return result;
    }
};

class periodic_guard
{
  protected:
    std::size_t count = 0;
    std::size_t regeneration_period = 100;

  public:
    bool operator()(std::size_t period)
    {
      if (++count == regeneration_period)
      {
        count = 0;
        regeneration_period = period;
        return true;
      }
      return false;
    }
};

} // namespace detail

/// \brief Adds an optimization to pbesinst_structure_graph.
class pbesinst_structure_graph_algorithm2: public pbesinst_structure_graph_algorithm
{
  protected:
    vertex_set S0;
    vertex_set S1;
    strategy_vector tau0;
    strategy_vector tau1;
    pbes_expression b; // to store the result of the Rplus computation
    detail::computation_guard S0_guard;
    detail::computation_guard S1_guard;
    detail::computation_guard find_loops_guard;
    detail::computation_guard fatal_attractors_guard;
    detail::periodic_guard reset_guard;

    template<typename T>
    pbes_expression expr(const T& x) const
    {
      return static_cast<const pbes_expression&>(x);
    }

    struct Rplus_traverser: public pbes_expression_traverser<Rplus_traverser>
    {
      typedef pbes_expression_traverser<Rplus_traverser> super;
      using super::enter;
      using super::leave;
      using super::apply;

      struct stack_element
      {
        pbes_expression b;
        pbes_expression f;
        pbes_expression g0;
        pbes_expression g1;

        stack_element(
          pbes_expression b_,
          pbes_expression f_,
          pbes_expression g0_,
          pbes_expression g1_
        )
         : b(std::move(b_)), f(std::move(f_)), g0(std::move(g0_)), g1(std::move(g1_))
        {}
      };

      vertex_set& S0;
      vertex_set& S1;
      detail::structure_graph_builder& graph_builder;
      std::vector<stack_element> stack;

      Rplus_traverser(vertex_set& S0_, vertex_set& S1_, detail::structure_graph_builder& graph_builder_)
       : S0(S0_), S1(S1_), graph_builder(graph_builder_)
      {}

      void unexpected(const pbes_expression& x) const
      {
        throw mcrl2::runtime_error("Unexpected term " + pbes_system::pp(x) + " encountered in Rplus");
      }

      void push(const stack_element& elem)
      {
        stack.push_back(elem);
      }

      stack_element pop()
      {
        auto result = stack.back();
        stack.pop_back();
        return result;
      }

      // Return the top element of result_stack
      stack_element& top()
      {
        return stack.back();
      }

      // Return the top element of result_stack
      const stack_element& top() const
      {
        return stack.back();
      }

      // TODO: use a heuristic for the smallest term
      bool less(const pbes_expression& /* x1 */, const pbes_expression& /* x2 */) const
      {
        return true;
      }

      void leave(const data::data_expression& x)
      {
        if (is_true(x) || is_false(x))
        {
          stack.emplace_back(x, x, true_(), false_());
        }
        else
        {
          unexpected(x);
        }
      }

      void leave(const propositional_variable_instantiation& x)
      {
        auto u = graph_builder.find_vertex(x);
        if (u == structure_graph::undefined_vertex)
        {
          // if x is not yet in the graph, then it certainly isn't in S0 or S1
          stack.emplace_back(data::undefined_data_expression(), x, true_(), false_());
        }
        else if (S0.contains(u))
        {
          stack.emplace_back(true_(), x, x, false_());
        }
        else if (S1.contains(u))
        {
          stack.emplace_back(false_(), x, true_(), x);
        }
        else
        {
          stack.emplace_back(data::undefined_data_expression(), x, true_(), false_());
        }
      }

      void leave(const and_& /* x */)
      {
        stack_element elem2 = pop();
        stack_element& elem1 = top();
        auto& b_1 = elem1.b;
        auto& f1_prime = elem1.f;
        auto& g0_1 = elem1.g0;
        auto& g1_1 = elem1.g1;
        auto& b_2 = elem2.b;
        auto& f2_prime = elem2.f;
        auto& g0_2 = elem2.g0;
        auto& g1_2 = elem2.g1;

        // put the result in (b1, f1_prime, g0, g1)
        if (is_true(b_1) && is_true(b_2))
        {
          b_1 = true_();
          f1_prime = and_(f1_prime, f2_prime);
          g0_1 = and_(g0_1, g0_2);
          g1_1 = false_();
        }
        else if (is_false(b_1) && !is_false(b_2))
        {
          b_1 = false_();
          // f1_prime = f1_prime;
          g0_1 = true_();
          // g1_1 = g1_1;
        }
        else if (!is_false(b_1) && is_false(b_2))
        {
          b_1 = false_();
          f1_prime = f2_prime;
          g0_1 = true_();
          g1_1 = g1_2;
        }
        else if (is_false(b_1) && is_false(b_2))
        {
          if (less(f1_prime, f2_prime))
          {
            b_1 = false_();
            // f1_prime = f1_prime;
            g0_1 = true_();
            // g1_1 = g1_1;
          }
          else
          {
            b_1 = false_();
            f1_prime = f2_prime;
            g0_1 = true_();
            g1_1 = g1_2;
          }
        }
        else // if (b1 == data::undefined_data_expression() && b2 == data::undefined_data_expression())
        {
          b_1 = data::undefined_data_expression();
          f1_prime = and_(f1_prime, f2_prime);
          g0_1 = true_();
          g1_1 = false_();
        }
      }

      void leave(const or_& /* x */)
      {
        stack_element elem2 = pop();
        stack_element& elem1 = top();
        auto& b_1 = elem1.b;
        auto& f1_prime = elem1.f;
        auto& g0_1 = elem1.g0;
        auto& g1_1 = elem1.g1;
        auto& b_2 = elem2.b;
        auto& f2_prime = elem2.f;
        auto& g0_2 = elem2.g0;
        auto& g1_2 = elem2.g1;

        // put the result in (b1, f1_prime, g0, g1)
        if (is_false(b_1) && is_false(b_2))
        {
          b_1 = false_();
          f1_prime = or_(f1_prime, f2_prime);
          g0_1 = true_();
          g1_1 = or_(g1_1, g1_2);
        }
        else if (is_true(b_1) && !is_true(b_2))
        {
          b_1 = true_();
          // f1_prime = f1_prime;
          // g0_1 = g0_1;
          g1_1 = false_();
        }
        else if (!is_true(b_1) && is_true(b_2))
        {
          b_1 = true_();
          f1_prime = f2_prime;
          g0_1 = g0_2;
          g1_1 = false_();
        }
        else if (is_true(b_1) && is_true(b_2))
        {
          if (less(f1_prime, f2_prime))
          {
            b_1 = true_();
            // f1_prime = f1_prime;
            // g0_1 = g0_1;
            g1_1 = false_();
          }
          else
          {
            b_1 = true_();
            f1_prime = f2_prime;
            g0_1 = g0_2;
            g1_1 = false_();
          }
        }
        else // if (b1 == data::undefined_data_expression() && b2 == data::undefined_data_expression())
        {
          b_1 = data::undefined_data_expression();
          f1_prime = or_(f1_prime, f2_prime);
          g0_1 = true_();
          g1_1 = false_();
        }
      }

      void leave(const imp& x)
      {
        unexpected(x);
      }

      void leave(const exists& x)
      {
        unexpected(x);
      }

      void leave(const forall& x)
      {
        unexpected(x);
      }
    };

    std::pair<pbes_expression, pbes_expression> Rplus_old(const pbes_expression& x)
    {
      if (is_true(x) || is_false(x))
      {
        return { x, x };
      }
      else if (is_propositional_variable_instantiation(x))
      {
        auto u = m_graph_builder.find_vertex(x);
        if (u != structure_graph::undefined_vertex)
        {
          if (S0.contains(u))
          {
            return { expr(true_()), x };
          }
          else if (S1.contains(u))
          {
            return { expr(false_()), x };
          }
        }
        return { expr(data::undefined_data_expression()), x };
      }
      else if (is_and(x))
      {
        const auto& x_ = atermpp::down_cast<and_>(x);
        // TODO: use structured bindings
        pbes_expression b1, b2, x1, x2;
        std::tie(b1, x1) = Rplus_old(x_.left());
        std::tie(b2, x2) = Rplus_old(x_.right());
        if (is_true(b1) && is_true(b2))
        {
          return { expr(true_()), x };
        }
        else if (is_false(b1) && !is_false(b2))
        {
          return { expr(false_()), x1 };
        }
        else if (!is_false(b1) && is_false(b2))
        {
          return { expr(false_()), x2 };
        }
        else if (is_false(b1) && is_false(b2))
        {
          // TODO: use a heuristic for the smallest term (x1 or x2)
          return { expr(false_()), x1 };
        }
        else // if (b1 == data::undefined_data_expression() && b2 == data::undefined_data_expression())
        {
          return { expr(data::undefined_data_expression()), x };
        }
      }
      else if (is_or(x))
      {
        const auto& x_ = atermpp::down_cast<or_>(x);
        // TODO: use structured bindings
        pbes_expression b1, b2, x1, x2;
        std::tie(b1, x1) = Rplus_old(x_.left());
        std::tie(b2, x2) = Rplus_old(x_.right());
        if (is_false(b1) && is_false(b2))
        {
          return { expr(false_()), x };
        }
        else if (is_true(b1) && !is_true(b2))
        {
          return { expr(true_()), x1 };
        }
        else if (!is_true(b1) && is_true(b2))
        {
          return { expr(true_()), x2 };
        }
        else if (is_true(b1) && is_true(b2))
        {
          // TODO: use a heuristic for the smallest term (x1 or x2)
          return { expr(true_()), x1 };
        }
        else // if (b1 == data::undefined_data_expression() && b2 == data::undefined_data_expression())
        {
          return { expr(data::undefined_data_expression()), x };
        }
      }
      else
      {
        throw mcrl2::runtime_error("Unexpected term " + pbes_system::pp(x) + " encountered in Rplus_old");
      }
    }

    Rplus_traverser::stack_element Rplus(const pbes_expression& x)
    {
      Rplus_traverser f(S0, S1, m_graph_builder);
      f.apply(x);
      const auto& top = f.top();

      // compare the result
      std::pair<pbes_expression, pbes_expression> old = Rplus_old(x);
      if (top.b != old.first || top.f != old.second)
      {
        std::cout << "x " << x << std::endl;
        std::cout << "b " << top.b << " " << old.first << std::endl;
        std::cout << "f " << top.f << " " << old.second << std::endl;
      }

      return f.top();
    }

    bool solution_found(const propositional_variable_instantiation& init) const override
    {
      auto u = m_graph_builder.find_vertex(init);
      return S0.contains(u) || S1.contains(u);
    }

    // Returns true if all nodes in the todo list are undefined (i.e. have not been processed yet)
    bool todo_has_only_undefined_nodes() const
    {
      for (const propositional_variable_instantiation& X: todo.all_elements())
      {
        structure_graph::index_type u = m_graph_builder.find_vertex(X);
        const structure_graph::vertex& u_ = m_graph_builder.vertex(u);
        if (u_.is_defined())
        {
          return false;
        }
      }
      return true;
    }

    void prune_todo_list(
      const propositional_variable_instantiation& init,
      pbesinst_lazy_todo& todo,
      std::size_t regeneration_period
    )
    {
      using utilities::detail::contains;

      if (!reset_guard(regeneration_period) && !m_options.aggressive && !todo.elements().empty())
      {
        return;
      }

      simple_structure_graph G(m_graph_builder.vertices());
      std::unordered_set<pbes_expression> todo1{init};
      std::unordered_set<pbes_expression> done1;
      std::unordered_set<propositional_variable_instantiation> new_todo;

      while (!todo1.empty())
      {
        using utilities::detail::contains;

        auto X = *todo1.begin();
        todo1.erase(todo1.begin());
        done1.insert(X);
        auto u = m_graph_builder.find_vertex(X);
        const auto& u_ = m_graph_builder.vertex(u);

        if (u_.decoration == structure_graph::d_none && u_.successors.empty())
        {
          assert(is_propositional_variable_instantiation(u_.formula));
          new_todo.insert(atermpp::down_cast<propositional_variable_instantiation>(u_.formula));
        }
        else
        {
          if (!S0.contains(u) && !S1.contains(u))
          {
            // todo' := todo' \cup (succ(u) \ done')
            for (auto v: G.successors(u))
            {
              const auto& v_ = m_graph_builder.vertex(v);
              const auto& Y = v_.formula;
              if (contains(done1, Y))
              {
                continue;
              }
              todo1.insert(Y);
            }
          }
        }
      }

      // new_todo_list := new_todo \cap (todo \cup irrelevant)
      // N.B. An attempt is made to preserve the order of the current todo list, to not
      // disturb breadth first and depth first search.
      std::deque<propositional_variable_instantiation> new_todo_list;
      for (const propositional_variable_instantiation& X: todo.irrelevant_elements())
      {
        if (contains(new_todo, X))
        {
          new_todo_list.push_back(X);
        }
      }
      for (const propositional_variable_instantiation& X: todo.elements())
      {
        if (contains(new_todo, X))
        {
          new_todo_list.push_back(X);
        }
      }
      todo.set_todo(new_todo_list);
      assert(todo_has_only_undefined_nodes());
    };

    bool strategies_are_set_in_solved_nodes() const
    {
      simple_structure_graph G(m_graph_builder.vertices());
      for (structure_graph::index_type u: S0.vertices())
      {
        if (G.decoration(u) == structure_graph::d_disjunction && G.strategy(u) == structure_graph::undefined_vertex)
        {
          mCRL2log(log::debug) << "Error: no strategy for node " << u << " in S0." << std::endl;
          mCRL2log(log::debug) << G << std::endl;
          mCRL2log(log::debug) << "S0 = " << S0 << std::endl;
          mCRL2log(log::debug) << "S1 = " << S1 << std::endl;
          return false;
        }
      }
      for (structure_graph::index_type u: S1.vertices())
      {
        if (G.decoration(u) == structure_graph::d_conjunction && G.strategy(u) == structure_graph::undefined_vertex)
        {
          mCRL2log(log::debug) << "Error: no strategy for node " << u << " in S1." << std::endl;
          mCRL2log(log::debug) << G << std::endl;
          mCRL2log(log::debug) << "S0 = " << S0 << std::endl;
          mCRL2log(log::debug) << "S1 = " << S1 << std::endl;
          return false;
        }
      }
      return true;
    }

  public:
    typedef pbesinst_structure_graph_algorithm super;

    pbesinst_structure_graph_algorithm2(
      const pbessolve_options& options,
      const pbes& p,
      structure_graph& G
    )
      : pbesinst_structure_graph_algorithm(options, p, G),
        find_loops_guard(2), fatal_attractors_guard(2)
    {}

    // Optimization 2 is implemented by overriding the function rewrite_psi.
    pbes_expression rewrite_psi(const fixpoint_symbol& symbol,
                                const propositional_variable_instantiation& X,
                                const pbes_expression& psi
                               ) override
    {
      auto result = Rplus(super::rewrite_psi(symbol, X, psi));
      b = result.b;
      if (is_true(b))
      {
        return result.g0;
      }
      else if (is_false(b))
      {
        return result.g1;
      }
      return result.f;
    }

    void on_report_equation(const propositional_variable_instantiation& X, const pbes_expression& psi, std::size_t k) override
    {
      super::on_report_equation(X, psi, k);

      // The structure graph has just been extended, so S0 and S1 need to be resized.
      S0.resize(m_graph_builder.extent());
      S1.resize(m_graph_builder.extent());

      auto u = m_graph_builder.find_vertex(X);
      if (is_true(b))
      {
        S0.insert(u);
      }
      else if (is_false(b))
      {
        S1.insert(u);
      }
    }

    void on_discovered_elements(const std::set<propositional_variable_instantiation>& elements) override
    {
      using utilities::detail::contains;

      if (m_options.optimization == 3)
      {
        if (S0_guard(S0.size()))
        {
          simple_structure_graph G(m_graph_builder.vertices());
          std::tie(S0, tau0) = attr_default_with_tau(G, S0, 0, tau0);
        }
        if (S1_guard(S1.size()))
        {
          simple_structure_graph G(m_graph_builder.vertices());
          std::tie(S1, tau1) = attr_default_with_tau(G, S1, 1, tau1);
        }
      }
      else if (m_options.optimization == 4 && (m_options.aggressive || find_loops_guard(m_iteration_count)))
      {
        simple_structure_graph G(m_graph_builder.vertices());
        detail::find_loops(G, discovered, todo, S0, S1, tau0, tau1, m_iteration_count, m_graph_builder); // modifies S0 and S1
        assert(strategies_are_set_in_solved_nodes());
      }
      else if ((5 <= m_options.optimization && m_options.optimization <= 7) && (m_options.aggressive || fatal_attractors_guard(m_iteration_count)))
      {
        simple_structure_graph G(m_graph_builder.vertices());
        if (m_options.optimization == 5)
        {
          detail::fatal_attractors(G, S0, S1, tau0, tau1, m_iteration_count); // modifies S0 and S1
          assert(strategies_are_set_in_solved_nodes());
        }
        else if (m_options.optimization == 6)
        {
          detail::fatal_attractors_original(G, S0, S1, tau0, tau1, m_iteration_count); // modifies S0 and S1
          assert(strategies_are_set_in_solved_nodes());
        }
        else // m_optimization == 7
        {
          m_graph_builder.finalize();
          detail::partial_solve(m_graph_builder.m_graph, todo, S0, S1, tau0, tau1, m_iteration_count, m_graph_builder); // modifies S0 and S1
        }
      }

      if (m_options.prune_todo_list)
      {
        for (const propositional_variable_instantiation& e: elements)
        {
          todo.irrelevant_elements().erase(e);
        }
        prune_todo_list(init, todo, (discovered.size() - todo.size()) / 2);
      }
    }

    void on_end_while_loop() override
    {
      using  utilities::detail::contains;

      simple_structure_graph G(m_graph_builder.vertices());

      structure_graph::index_type u = m_graph_builder.find_vertex(init);
      std::set<structure_graph::index_type> V = extract_minimal_structure_graph(G, u, S0, S1);

      std::size_t n = m_graph_builder.extent();
      vertex_set to_be_removed(n);
      for (std::size_t v = 0; v < n; v++)
      {
        if (!contains(V, v))
        {
          to_be_removed.insert(v);
        }
      }
      m_graph_builder.erase_vertices(to_be_removed);
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESINST_STRUCTURE_GRAPH2_H
