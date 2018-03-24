// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/solve_structure_graph.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_SOLVE_STRUCTURE_GRAPH_H
#define MCRL2_PBES_SOLVE_STRUCTURE_GRAPH_H

#include <deque>
#include <limits>
#include <regex>
#include <sstream>
#include <unordered_set>
#include <tuple>
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/join.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/lps/print.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lts/lts_algorithm.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/pbes/pbes_equation_index.h"
#include "mcrl2/pbes/structure_graph.h"
//#include "mcrl2/utilities/chrono_timer.h"

namespace mcrl2 {

namespace pbes_system {

struct deque_vertex_set
{
  protected:
    std::deque<structure_graph::index_type> m_vertices;
    boost::dynamic_bitset<> m_include;

  public:
    deque_vertex_set() = default;

    deque_vertex_set(std::size_t N)
      : m_include(N)
    {}

    template <typename Iter>
    deque_vertex_set(std::size_t N, Iter first, Iter last)
      : m_include(N)
    {
      for (auto i = first; i != last; ++i)
      {
        insert(*i);
      }
    }

    bool is_empty() const
    {
      return m_vertices.empty();
    }

    bool contains(structure_graph::index_type u) const
    {
      return m_include[u];
    }

    void insert(structure_graph::index_type u)
    {
      assert(u < m_include.size());
      assert(!m_include[u]);
      m_vertices.push_back(u);
      m_include[u] = true;
    }

    void clear()
    {
      m_vertices.clear();
      m_include = boost::dynamic_bitset<>(m_include.size());
    }

    std::size_t extent() const
    {
      return m_include.size();
    }

    const std::deque<structure_graph::index_type>& vertices() const
    {
      return m_vertices;
    }

    std::size_t size() const
    {
      return m_vertices.size();
    }

    const boost::dynamic_bitset<>& include() const
    {
      return m_include;
    }

    structure_graph::index_type pop_front()
    {
      structure_graph::index_type u = m_vertices.front();
      m_vertices.pop_front();
      m_include[u] = false;
      return u;
    }

    structure_graph::index_type pop_back()
    {
      structure_graph::index_type u = m_vertices.back();
      m_vertices.pop_back();
      m_include[u] = false;
      return u;
    }

    bool operator==(const deque_vertex_set& other) const
    {
      return m_include == other.m_include;
    }

    bool operator!=(const deque_vertex_set& other) const
    {
      return !(*this == other);
    }
};

struct vertex_set
{
  protected:
    std::vector<structure_graph::index_type> m_vertices;
    boost::dynamic_bitset<> m_include;

  public:
    void self_check(const std::string& msg = "") const
    {
#ifndef NDEBUG
      if (!msg.empty())
      {
        std::cout << "--- " << msg << " ---" << std::endl;
      }
      for (structure_graph::index_type v: m_vertices)
      {
        if (v >= m_include.size())
        {
          throw mcrl2::runtime_error("The value " + std::to_string(v) + " is bigger than I.size() = " + std::to_string(m_include.size()));
        }
        if (!m_include[v])
        {
          throw mcrl2::runtime_error("Inconsistency detected with element " + std::to_string(v) + "!");
        }
      }
#endif
    }

    vertex_set() = default;

    vertex_set(std::size_t N)
      : m_include(N)
    {
      self_check();
    }

    template <typename Iter>
    vertex_set(std::size_t N, Iter first, Iter last)
      : m_include(N)
    {
      for (auto i = first; i != last; ++i)
      {
        insert(*i);
      }
      self_check();
    }

    bool is_empty() const
    {
      return m_vertices.empty();
    }

    bool contains(structure_graph::index_type u) const
    {
      return m_include[u];
    }

    void insert(structure_graph::index_type u)
    {
      assert(u < m_include.size());
      assert(!m_include[u]);
      m_vertices.push_back(u);
      m_include[u] = true;
      self_check();
    }

    void clear()
    {
      m_vertices.clear();
      m_include = boost::dynamic_bitset<>(m_include.size());
      self_check();
    }

    std::size_t extent() const
    {
      return m_include.size();
    }

    const std::vector<structure_graph::index_type>& vertices() const
    {
      return m_vertices;
    }

    std::size_t size() const
    {
      return m_vertices.size();
    }

    const boost::dynamic_bitset<>& include() const
    {
      return m_include;
    }

    structure_graph::index_type pop_back()
    {
      structure_graph::index_type u = m_vertices.back();
      m_vertices.pop_back();
      m_include[u] = false;
      return u;
    }

    bool operator==(const vertex_set& other) const
    {
      return m_include == other.m_include;
    }

    bool operator!=(const vertex_set& other) const
    {
      return !(*this == other);
    }
};

inline
std::ostream& operator<<(std::ostream& out, const vertex_set& V)
{
  return out << core::detail::print_set(V.vertices());
}

inline
vertex_set set_union(const vertex_set& V, const vertex_set& W)
{
  vertex_set result = V;
  for (structure_graph::index_type w: W.vertices())
  {
    if (!result.contains(w))
    {
      result.insert(w);
    }
  }
  return result;
}

inline
vertex_set set_difference(const vertex_set& V, const vertex_set& W)
{
  vertex_set result(V.extent());
  for (structure_graph::index_type v: V.vertices())
  {
    if (!W.contains(v))
    {
      result.insert(v);
    }
  }
  return result;
}

// inline
// vertex_set make_vertex_set(const structure_graph& G)
// {
//   std::size_t N = G.all_vertices().size();
//   vertex_set result(N);
//   for (std::size_t i = 0; i < N; i++)
//   {
//     if (G.contains(i))
//     {
//       result.insert(i);
//     }
//   }
//   return result;
// }

inline
void log_vertex_set(const structure_graph& G, const vertex_set& V, const std::string& name)
{
  mCRL2log(log::debug) << "--- " << name << " ---" << std::endl;
  for (structure_graph::index_type v: V.vertices())
  {
    mCRL2log(log::debug) << "  " << v << " " << G.find_vertex(v) << std::endl;
  }
  // mCRL2log(log::debug) << "\n";
  // mCRL2log(log::debug) << "exclude = " << G.exclude() << "\n";
}

// Returns true if the vertex u satisfies the conditions for being added to the attractor set A.
// alpha = 0: disjunctive
// alpha = 1: conjunctive
inline
bool is_attractor(const structure_graph& G, structure_graph::index_type u, const vertex_set& A, int alpha)
{
  if (G.decoration(u) != alpha)
  {
    return true;
  }
  if (G.decoration(u) != (1 - alpha))
  {
    for (structure_graph::index_type v: G.successors(u))
    {
      if (!(A.contains(v)))
      {
        return false;
      }
    }
    return true;
  }
  return false;
}

// Computes the conjunctive attractor set, by extending A.
// alpha = 0: disjunctive
// alpha = 1: conjunctive
inline
vertex_set compute_attractor_set(const structure_graph& G, vertex_set A, int alpha)
{
  // utilities::chrono_timer timer;
  // std::size_t A_size = A.size();

  // put all predecessors of elements in A in todo
  deque_vertex_set todo(G.all_vertices().size());
  for (structure_graph::index_type u: A.vertices())
  {
    for (structure_graph::index_type v: G.predecessors(u))
    {
      if (!(A.contains(v)) && !todo.contains(v))
      {
        todo.insert(v);
      }
    }
  }

  while (!todo.is_empty())
  {
    // N.B. Use a breadth first search, to minimize counter examples
    structure_graph::index_type u = todo.pop_front();

    if (is_attractor(G, u, A, 1 - alpha))
    {
      // set strategy
      if (G.decoration(u) != (1 - alpha))
      {
        for (structure_graph::index_type w: G.successors(u))
        {
          if ((A.contains(w)))
          {
            mCRL2log(log::debug) << "set strategy for node " << u << " to " << w << std::endl;
            G.find_vertex(u).strategy = w;
            break;
          }
        }
        if (G.strategy(u) == structure_graph::undefined_vertex)
        {
          mCRL2log(log::debug) << "Error: no strategy for node " << u << std::endl;
        }
      }

      A.insert(u);

      for (structure_graph::index_type v: G.predecessors(u))
      {
        if (!A.contains(v) && !todo.contains(v))
        {
          todo.insert(v);
        }
      }
    }
  }

  // mCRL2log(log::verbose) << "computed attractor set, alpha = " << alpha << ", size before = " << A_size << ", size after = " << A.size() << ", time = " << timer.elapsed() << std::endl;
  return A;
}

inline
std::tuple<std::size_t, std::size_t, vertex_set> get_minmax_rank(const structure_graph& G)
{
  std::size_t min_rank = (std::numeric_limits<std::size_t>::max)();
  std::size_t max_rank = 0;
  std::vector<structure_graph::index_type> M; // vertices with minimal rank
  std::size_t N = G.all_vertices().size();

  for (structure_graph::index_type vi = 0; vi < N; vi++)
  {
    if (!G.contains(vi))
    {
      continue;
    }
    const auto& v = G.find_vertex(vi);
    if (v.rank <= min_rank)
    {
      if (v.rank < min_rank)
      {
        M.clear();
        min_rank = v.rank;
      }
      M.push_back(vi);
    }
    if (v.rank > max_rank)
    {
      max_rank = v.rank;
    }
  }
  return std::make_tuple(min_rank, max_rank, vertex_set(N, M.begin(), M.end()));
}

class solve_structure_graph_algorithm
{
  protected:
    // do a sanity check on the computed strategy
    bool check_strategy = false;

    // find a successor of u
    structure_graph::index_type succ(const structure_graph& G, structure_graph::index_type u)
    {
      for (structure_graph::index_type v: G.successors(u))
      {
        return v;
      }
      return structure_graph::undefined_vertex;
    }

    // computes solve_recursive(G \ A)
    inline
    std::pair<vertex_set, vertex_set> solve_recursive(structure_graph& G, const vertex_set& A)
    {
      auto exclude = G.exclude() | A.include();
      std::swap(G.exclude(), exclude);
      auto result = solve_recursive(G);
      std::swap(G.exclude(), exclude);
      return result;
    }

    // pre: G does not contain nodes with decoration true or false.
    inline
    std::pair<vertex_set, vertex_set> solve_recursive(structure_graph& G)
    {
      mCRL2log(log::debug) << "\n--- solve_recursive input ---\n" << G << std::endl;
      std::size_t N = G.all_vertices().size();

      if (G.is_empty())
      {
        return { vertex_set(N), vertex_set(N) };
      }

      auto q = get_minmax_rank(G);
      std::size_t m = std::get<0>(q);
      const vertex_set& U = std::get<2>(q);

      int alpha = m % 2; // 0 = disjunctive, 1 = conjunctive

      // set strategy
      for (structure_graph::index_type ui: U.vertices())
      {
        const auto& u = G.find_vertex(ui);
        if (u.decoration == alpha)
        {
          auto v = succ(G, ui);
          if (v != structure_graph::undefined_vertex)
          {
            mCRL2log(log::debug) << "set initial strategy for node " << ui << " to " << v << std::endl;
            u.strategy = v;
          }
        }
      }

      // // optimization
      // std::size_t h = std::get<1>(q);
      // if (h == m)
      // {
      //   if (m % 2 == 0)
      //   {
      //     return { make_vertex_set(G), vertex_set(N) };
      //   }
      //   else
      //   {
      //     return { vertex_set(N), make_vertex_set(G) };
      //   }
      // }

      vertex_set W[2]   = { vertex_set(N), vertex_set(N) };
      vertex_set W_1[2];

// N.B. This code is wrong!
//
//      vertex_set A = compute_attractor_set(G, U, alpha);
//      std::tie(W_1[0], W_1[1]) = solve_recursive(G, A);
//      vertex_set B = compute_attractor_set(G, W_1[1 - alpha], 1 - alpha);
//      if (W_1[1 - alpha].size() == B.size())
//      {
//        W[alpha] = set_union(A, W_1[alpha]);
//        W[1 - alpha].clear();
//      }
//      else
//      {
//        std::tie(W[0], W[1]) = solve_recursive(G, B);
//        W[1 - alpha] = set_union(W[1 - alpha], B);
//      }

      vertex_set A = compute_attractor_set(G, U, alpha);
      std::tie(W_1[0], W_1[1]) = solve_recursive(G, A);
      if (W_1[1 - alpha].is_empty())
      {
        W[alpha] = set_union(A, W_1[alpha]);
        W[1 - alpha].clear();
      }
      else
      {
        vertex_set B = compute_attractor_set(G, W_1[1 - alpha], 1 - alpha);
        std::tie(W[0], W[1]) = solve_recursive(G, B);
        W[1 - alpha] = set_union(W[1 - alpha], B);
      }

      mCRL2log(log::debug) << "\n--- solution for solve_recursive input ---\n" << G;
      mCRL2log(log::debug) << "   W0 = " << W[0] << std::endl;
      mCRL2log(log::debug) << "   W1 = " << W[1] << std::endl;
      return { W[0], W[1] };
    }

    // Handles nodes with decoration true or false.
    inline
    std::pair<vertex_set, vertex_set> solve_recursive_extended(structure_graph& G)
    {
      mCRL2log(log::debug) << "\n--- solve_recursive_extended input ---\n" << G << std::endl;

      std::size_t N = G.all_vertices().size();
      vertex_set Vconj(N);
      vertex_set Vdisj(N);

      // find vertices Vconj with decoration false and Vdisj with decoration true
      for (structure_graph::index_type vi = 0; vi < N; vi++)
      {
        if (!G.contains(vi))
        {
          continue;
        }
        const auto& v = G.find_vertex(vi);
        if (v.decoration == structure_graph::d_false)
        {
          Vconj.insert(vi);
        }
        else if (v.decoration == structure_graph::d_true)
        {
          Vdisj.insert(vi);
        }
      }

      // extend Vconj and Vdisj
      if (!Vconj.is_empty())
      {
        Vconj = compute_attractor_set(G, Vconj, 1);
      }
      if (!Vdisj.is_empty())
      {
        Vdisj = compute_attractor_set(G, Vdisj, 0);
      }

      // default case
      if (Vconj.is_empty() && Vdisj.is_empty())
      {
        return solve_recursive(G);
      }
      else
      {
        vertex_set Wconj(N);
        vertex_set Wdisj(N);
        vertex_set Vunion = set_union(Vconj, Vdisj);
        std::tie(Wdisj, Wconj) = solve_recursive(G, Vunion);
        return std::make_pair(set_union(Wdisj, Vdisj), set_union(Wconj, Vconj));
      }
    }

    inline
    void check_solve_recursive_solution(const structure_graph& G, const vertex_set& Wdisj, const vertex_set& Wconj)
    {
      mCRL2log(log::debug) << "\n--- check_solve_recursive_solution ---" << std::endl;
      log_vertex_set(G, Wconj, "Wconj");
      log_vertex_set(G, Wdisj, "Wdisj");

      typedef structure_graph::vertex vertex;

      vertex_set Wconj1;
      vertex_set Wdisj1;
      structure_graph Gcopy;

      Gcopy = G;
      for (structure_graph::index_type ui: Wconj.vertices())
      {
        vertex& u = Gcopy.find_vertex(ui);
        if (u.decoration == structure_graph::d_conjunction)
        {
          if (u.strategy == structure_graph::undefined_vertex)
          {
            throw mcrl2::runtime_error("check_solve_recursive_solution failed: vertex " + std::to_string(ui) + " has no strategy");
          }
          u.decoration = structure_graph::d_none;

          for (structure_graph::index_type vi: Gcopy.successors(ui))
          {
            vertex& v = Gcopy.find_vertex(vi);
            v.remove_predecessor(ui);
          }
          u.successors.clear();

          // add the edge (u, u.strategy)
          structure_graph::index_type wi = u.strategy;
          u.successors.push_back(wi);
          vertex& w = Gcopy.find_vertex(wi);
          w.predecessors.push_back(ui);
        }
      }
      Gcopy.exclude() = ~(Wconj.include());
      log_vertex_set(Gcopy, Wconj, "Wconj after removal of edges");
      std::tie(Wdisj1, Wconj1) = solve_recursive_extended(Gcopy);
      if (!Wdisj1.is_empty() || Wconj1 != Wconj)
      {
        log_vertex_set(Gcopy, Wconj1, "Wconj1");
        log_vertex_set(Gcopy, Wdisj1, "Wdisj1");
        throw mcrl2::runtime_error("check_solve_recursive_solution failed!");
      }

      Gcopy = G;
      for (structure_graph::index_type ui: Wdisj.vertices())
      {
        vertex& u = Gcopy.find_vertex(ui);
        if (u.decoration == structure_graph::d_disjunction)
        {
          if (u.strategy == structure_graph::undefined_vertex)
          {
            throw mcrl2::runtime_error("check_solve_recursive_solution failed: vertex " + std::to_string(ui) + " has no strategy");
          }
          u.decoration = structure_graph::d_none;

          for (structure_graph::index_type vi: Gcopy.successors(ui))
          {
            vertex& v = Gcopy.find_vertex(vi);
            v.remove_predecessor(ui);
          }
          u.successors.clear();

          // add the edge (u, u.strategy)
          assert(u.strategy != structure_graph::undefined_vertex);
          structure_graph::index_type wi = u.strategy;
          u.successors.push_back(wi);
          vertex& w = Gcopy.find_vertex(wi);
          w.predecessors.push_back(ui);
        }
      }
      Gcopy.exclude() = ~(Wdisj.include());
      log_vertex_set(Gcopy, Wdisj, "Wdisj after removal of edges");
      std::tie(Wdisj1, Wconj1) = solve_recursive_extended(Gcopy);
      if (!Wconj1.is_empty() || Wdisj1 != Wdisj)
      {
        log_vertex_set(Gcopy, Wconj1, "Wconj1");
        log_vertex_set(Gcopy, Wdisj1, "Wdisj1");
        throw mcrl2::runtime_error("check_solve_recursive_solution failed!");
      }
    }

  public:
    solve_structure_graph_algorithm(bool check_strategy_ = false)
      : check_strategy(check_strategy_)
    {}

    inline
    bool solve(structure_graph& G)
    {
      mCRL2log(log::verbose) << "Solving parity game..." << std::endl;
      auto W = solve_recursive_extended(G);
      bool result;
      if (W.first.contains(G.initial_vertex()))
      {
        result = true;
      }
      else if (W.second.contains(G.initial_vertex()))
      {
        result = false;
      }
      else
      {
        throw mcrl2::runtime_error("No solution found!!!");
      }
      if (check_strategy)
      {
        check_solve_recursive_solution(G, W.first, W.second);
      }
      return result;
    }
};

inline
std::set<structure_graph::index_type> find_counter_example_nodes(structure_graph& G, structure_graph::index_type init, bool is_disjunctive)
{
  using utilities::detail::contains;

  typedef structure_graph::vertex vertex;
  std::vector<const vertex*> result;

  std::set<structure_graph::index_type> todo = { init };
  std::set<structure_graph::index_type> done;
  while (!todo.empty())
  {
    structure_graph::index_type u = *todo.begin();
    todo.erase(todo.begin());
    done.insert(u);
    if ((is_disjunctive && G.decoration(u) == structure_graph::d_disjunction) || (!is_disjunctive && G.decoration(u) == structure_graph::d_conjunction))
    {
      // explore only the strategy edge
      structure_graph::index_type v = G.strategy(u);
      if (!contains(done, v))
      {
        todo.insert(v);
      }
    }
    else
    {
      // explore all outgoing edges
      for (structure_graph::index_type v: G.successors(u))
      {
        if (!contains(done, v))
        {
          todo.insert(v);
        }
      }
    }
  }
  return done;
}

class lps_solve_structure_graph_algorithm: public solve_structure_graph_algorithm
{
  protected:
    lps::specification create_counter_example_lps(structure_graph& G, const std::set<structure_graph::index_type>& V, const lps::specification& lpsspec, const pbes& p, const pbes_equation_index& p_index)
    {
      lps::specification result = lpsspec;
      result.process().action_summands().clear();
      result.process().deadlock_summands().clear();
      auto& action_summands = result.process().action_summands();
      std::regex re("Z(neg|pos)_(\\d+)_.*");
      std::size_t n = lpsspec.process().process_parameters().size();

      for (structure_graph::index_type vi: V)
      {
        const auto& v = G.find_vertex(vi);
        const auto& Z = atermpp::down_cast<propositional_variable_instantiation>(v.formula);
        std::string Zname = Z.name();
        std::smatch match;
        if (std::regex_match(Zname, match, re))
        {
          std::size_t summand_index = std::stoul(match[2]);
          lps::action_summand summand = lpsspec.process().action_summands()[summand_index];

          std::size_t equation_index = p_index.index(Z.name());
          const pbes_equation& eqn = p.equations()[equation_index];
          const data::variable_list& d = eqn.variable().parameters();
          data::variable_vector d1(d.begin(), d.end());

          const data::data_expression_list& e = Z.parameters();
          data::data_expression_vector e1(e.begin(), e.end());

          data::data_expression_vector condition;
          data::assignment_vector next_state_assignments;
          std::size_t m = d.size() - 2 * n;

          for (std::size_t i = 0; i < n; i++)
          {
            condition.push_back(data::equal_to(d1[i], e1[i]));
            next_state_assignments.emplace_back(d1[i], e1[n + m + i]);
          }

          process::action_vector actions;
          std::size_t index = 0;
          for (const process::action& a: summand.multi_action().actions())
          {
            process::action a1(a.label(), data::data_expression_list(&e1[n + index], &e1[n + index + a.arguments().size()]));
            actions.push_back(a1);
            index = index + a.arguments().size();
          }

          summand.summation_variables() = data::variable_list();
          summand.condition() = data::join_and(condition.begin(), condition.end());
          summand.multi_action().actions() = process::action_list(actions.begin(), actions.end());
          summand.assignments() = data::assignment_list(next_state_assignments.begin(), next_state_assignments.end());

          action_summands.push_back(summand);
        }
      }
      return result;
    }

  public:
    lps_solve_structure_graph_algorithm() = default;

    /// \param lpsspec The original LPS that was used to create the PBES.
    std::pair<bool, lps::specification> solve_with_counter_example(structure_graph& G, const lps::specification& lpsspec, const pbes& p, const pbes_equation_index& p_index)
    {
      mCRL2log(log::verbose) << "Solving parity game..." << std::endl;
      vertex_set Wconj;
      vertex_set Wdisj;
      std::tie(Wdisj, Wconj) = solve_recursive_extended(G);
      structure_graph::index_type init = G.initial_vertex();

      if (Wdisj.contains(init))
      {
        mCRL2log(log::verbose) << "Extracting witness..." << std::endl;
        std::set<structure_graph::index_type> W = find_counter_example_nodes(G, init, true);
        return { true, create_counter_example_lps(G, W, lpsspec, p, p_index) };
      }
      else if (Wconj.contains(init))
      {
        mCRL2log(log::verbose) << "Extracting counter example..." << std::endl;
        std::set<structure_graph::index_type> W = find_counter_example_nodes(G, init, false);
        return { false, create_counter_example_lps(G, W, lpsspec, p, p_index) };
      }
      throw mcrl2::runtime_error("No solution found in solve_structure_graph!");
    }
};

class lts_solve_structure_graph_algorithm: public solve_structure_graph_algorithm
{
  protected:
    // Removes all transitions from ltsspec, except the ones in transition_indices.
    // After that, the unreachable parts of the LTS are removed.
    inline
    void filter_transitions(lts::lts_lts_t& ltsspec, const std::set<std::size_t>& transition_indices)
    {
      // remove transitions
      const auto& lts_transitions = ltsspec.get_transitions();
      std::vector<lts::transition> transitions;
      for (std::size_t i: transition_indices)
      {
        transitions.push_back(lts_transitions[i]);
      }
      ltsspec.get_transitions() = transitions;

      // remove unreachable states
      lts::reachability_check(ltsspec, true);
    }

    // modifies ltsspec
    inline
    void create_counter_example_lts(structure_graph& G, const std::set<structure_graph::index_type>& V, lts::lts_lts_t& ltsspec, const pbes& p)
    {
      std::regex re("Z(neg|pos)_(\\d+)_.*");

      std::set<std::size_t> transition_indices;
      for (structure_graph::index_type vi: V)
      {
        const auto& v = G.find_vertex(vi);
        const auto& Z = atermpp::down_cast<propositional_variable_instantiation>(v.formula);
        std::string Zname = Z.name();
        std::smatch match;
        if (std::regex_match(Zname, match, re))
        {
          std::size_t transition_index = std::stoul(match[2]);
          transition_indices.insert(transition_index);
        }
      }
      filter_transitions(ltsspec, transition_indices);
    }

  public:
    lts_solve_structure_graph_algorithm() = default;

    /// \param ltsspec The original LTS that was used to create the PBES.
    inline
    bool solve_with_counter_example(structure_graph& G, lts::lts_lts_t& ltsspec, const pbes& p)
    {
      mCRL2log(log::verbose) << "Solving parity game..." << std::endl;
      vertex_set Wconj;
      vertex_set Wdisj;
      std::tie(Wdisj, Wconj) = solve_recursive_extended(G);
      structure_graph::index_type init = G.initial_vertex();

      if (Wdisj.contains(init))
      {
        mCRL2log(log::verbose) << "Extracting witness..." << std::endl;
        std::set<structure_graph::index_type> W = find_counter_example_nodes(G, init, true);
        create_counter_example_lts(G, W, ltsspec, p);
        return true;
      }
      else if (Wconj.contains(init))
      {
        mCRL2log(log::verbose) << "Extracting counter example..." << std::endl;
        std::set<structure_graph::index_type> W = find_counter_example_nodes(G, init, false);
        create_counter_example_lts(G, W, ltsspec, p);
        return false;
      }
      throw mcrl2::runtime_error("No solution found in solve_structure_graph!");
    }
};

inline
bool solve_structure_graph(structure_graph& G, bool check_strategy = false)
{
  solve_structure_graph_algorithm algorithm(check_strategy);
  return algorithm.solve(G);
}

inline
std::pair<bool, lps::specification> solve_structure_graph_with_counter_example(structure_graph& G, const lps::specification& lpsspec, const pbes& p, const pbes_equation_index& p_index)
{
  lps_solve_structure_graph_algorithm algorithm;
  return algorithm.solve_with_counter_example(G, lpsspec, p, p_index);
}

/// \param ltsspec The original LTS that was used to create the PBES.
inline
bool solve_structure_graph_with_counter_example(structure_graph& G, lts::lts_lts_t& ltsspec, const pbes& p)
{
  lts_solve_structure_graph_algorithm algorithm;
  return algorithm.solve_with_counter_example(G, ltsspec, p);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_SOLVE_STRUCTURE_GRAPH_H
