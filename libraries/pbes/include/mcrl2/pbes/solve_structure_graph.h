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
#include "mcrl2/utilities/chrono_timer.h"

namespace mcrl2 {

namespace pbes_system {

struct vertex_set
{
  protected:
    std::vector<int> m_vertices;
    std::vector<bool> m_I;

  public:

    void self_check(const std::string& msg = "") const
    {
#ifndef NDEBUG
      if (!msg.empty())
      {
        std::cout << "--- " << msg << " ---" << std::endl;
      }
      for (int v: m_vertices)
      {
        if (v >= m_I.size())
        {
          throw mcrl2::runtime_error("The value " + std::to_string(v) + " is bigger than I.size() = " + std::to_string(m_I.size()));
        }
        if (!m_I[v])
        {
          throw mcrl2::runtime_error("Inconsistency detected with element " + std::to_string(v) + "!");
        }
      }
#endif
    }

    vertex_set()
    {}

    vertex_set(std::size_t N)
      : m_I(N, false)
    {
      self_check();
    }

    template <typename Iter>
    vertex_set(std::size_t N, Iter first, Iter last)
      : m_I(N, false)
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

    bool contains(int u) const
    {
      return m_I[u];
    }

    void insert(int u)
    {
      if (u >= m_I.size())
      {
        throw mcrl2::runtime_error("The value " + std::to_string(u) + " is bigger than I.size() = " + std::to_string(m_I.size()));
      }
      if (m_I[u])
      {
        throw mcrl2::runtime_error("The value " + std::to_string(u) + " is already contained!");
      }
      m_vertices.push_back(u);
      m_I[u] = true;
      self_check();
    }

    void clear()
    {
      m_vertices.clear();
      m_I = std::vector<bool>(m_I.size(), false);
      self_check();
    }

    std::size_t extent() const
    {
      return m_I.size();
    }

    const std::vector<int>& vertices() const
    {
      return m_vertices;
    }

    std::size_t size() const
    {
      return m_vertices.size();
    }

    const std::vector<bool>& I() const
    {
      return m_I;
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
  for (int w: W.vertices())
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
  for (int v: V.vertices())
  {
    if (!W.contains(v))
    {
      result.insert(v);
    }
  }
  return result;
}

inline
void log_vertex_set(const structure_graph& G, const vertex_set& V, const std::string& name)
{
  mCRL2log(log::debug) << "--- " << name << " ---" << std::endl;
  for (const structure_graph::vertex& v: G.vertices())
  {
    mCRL2log(log::debug) << "  " << v << std::endl;
  }
  mCRL2log(log::debug) << "\n";
  mCRL2log(log::debug) << "exclude = " << core::detail::print_list(G.exclude()) << "\n";
}

// Returns true if the vertex u satisfies the conditions for being added to the attractor set A.
// alpha = 0: disjunctive
// alpha = 1: conjunctive
inline
bool is_attractor(const structure_graph& G, int u, const vertex_set& A, int alpha)
{
  if (G.decoration(u) != alpha)
  {
    return true;
  }
  if (G.decoration(u) != (1 - alpha))
  {
    for (int v: G.successors(u))
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
  // put all predecessors of elements in A in todo
  std::set<int> todo;
  for (int u: A.vertices())
  {
    for (int v: G.predecessors(u))
    {
      if (!(A.contains(v)))
      {
        todo.insert(v);
      }
    }
  }

  while (!todo.empty())
  {
    int u = *todo.begin();
    todo.erase(todo.begin());

    if (is_attractor(G, u, A, 1 - alpha))
    {
      // set strategy
      if (G.decoration(u) != (1 - alpha))
      {
        for (int w: G.successors(u))
        {
          if ((A.contains(w)))
          {
            G.find_vertex(u).strategy = w;
            break;
          }
        }
        if (G.strategy(u) == -1)
        {
          mCRL2log(log::debug) << "Error: no strategy for node " << u << std::endl;
        }
      }

      A.insert(u);

      for (int v: G.predecessors(u))
      {
        if (!(A.contains(v)))
        {
          todo.insert(v);
        }
      }
    }
  }
  return A;
}

inline
std::tuple<std::size_t, std::size_t, vertex_set> get_minmax_rank(const structure_graph& G)
{
  std::size_t min_rank = (std::numeric_limits<std::size_t>::max)();
  std::size_t max_rank = 0;
  std::vector<int> M; // vertices with minimal rank
  std::size_t N = G.all_vertices().size();

  for (int vi = 0; vi < N; vi++)
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

// find a successor of u in U, or a random one if no successor in U exists
inline
int succ(const structure_graph& G, int u, const vertex_set& U)
{
  int result = -1;

  for (int v: G.successors(u))
  {
    if (U.contains(v))
    {
      result = v;
      break;
    }
  }
  return result;
}

inline
std::pair<vertex_set, vertex_set> solve_recursive(structure_graph& G);

// computes solve_recursive(G \ A)
inline
std::pair<vertex_set, vertex_set> solve_recursive(structure_graph& G, const vertex_set& A)
{
  auto exclude = G.exclude();
  for (int u: A.vertices())
  {
    exclude[u] = true;
  }
  std::swap(G.exclude(), exclude);
  auto result = solve_recursive(G);
  G.exclude() = exclude;
  return result;
}

// pre: G does not contain nodes with decoration true or false.
inline
std::pair<vertex_set, vertex_set> solve_recursive(structure_graph& G)
{
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
  for (int ui: U.vertices())
  {
    const auto& u = G.find_vertex(ui);
    if (u.decoration == alpha)
    {
      auto v = succ(G, ui, U);
      if (v != -1)
      {
        u.strategy = v;
      }
    }
  }

  vertex_set W[2]   = { vertex_set(N), vertex_set(N) };
  vertex_set W_1[2] = { vertex_set(N), vertex_set(N) };

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

  return { W[0], W[1] };
}

// Handles nodes with decoration true or false.
inline
std::pair<vertex_set, vertex_set> solve_recursive_extended(structure_graph& G)
{
  std::size_t N = G.all_vertices().size();
  vertex_set Vconj(N);
  vertex_set Vdisj(N);

  log_vertex_set(G, Vconj, "Vconj---");
  log_vertex_set(G, Vdisj, "Vdisj---");

  // find vertices Vconj with decoration false and Vdisj with decoration true
  for (int vi = 0; vi < N; vi++)
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

  mCRL2log(log::debug) << "\n--- solve_recursive_extended ---" << std::endl;
  log_vertex_set(G, Vconj, "Vconj");
  log_vertex_set(G, Vdisj, "Vdisj");

  // extend Vconj and Vdisj
  if (!Vconj.is_empty())
  {
    Vconj = compute_attractor_set(G, Vconj, 1);
  }
  if (!Vdisj.is_empty())
  {
    Vdisj = compute_attractor_set(G, Vdisj, 0);
  }

  log_vertex_set(G, Vconj, "Vconj after extension");
  log_vertex_set(G, Vdisj, "Vdisj after extension");

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
bool solve_structure_graph(structure_graph& G, bool check_strategy = false)
{
  mCRL2log(log::verbose) << "Solving parity game..." << std::endl;
  // utilities::chrono_timer timer;
  auto W = solve_recursive_extended(G);
  // std::cout << "time: " << timer.elapsed() << std::endl;
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
  return result;

  // const vertex& init = G.initial_vertex();
  // if (check_strategy)
  // {
  //   check_solve_recursive_solution(Wconj, Wdisj);
  // }
}

inline
std::set<int> find_counter_example_nodes(structure_graph& G, const vertex_set& V, int init, bool is_disjunctive)
{
  using utilities::detail::contains;

  typedef structure_graph::vertex vertex;
  std::vector<const vertex*> result;

  std::set<int> todo = { init };
  std::set<int> done;
  while (!todo.empty())
  {
    int u = *todo.begin();
    todo.erase(todo.begin());
    done.insert(u);
    if ((is_disjunctive && G.decoration(u) == structure_graph::d_disjunction) || (!is_disjunctive && G.decoration(u) == structure_graph::d_conjunction))
    {
      // explore only the strategy edge
      int v = G.strategy(u);
      if (!contains(done, v))
      {
        todo.insert(v);
      }
    }
    else
    {
      // explore all outgoing edges
      for (int v: G.successors(u))
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

inline
lps::specification create_counter_example_lps(structure_graph& G, const std::set<int>& V, const lps::specification& lpsspec, const pbes& p, const pbes_equation_index& p_index)
{
  lps::specification result = lpsspec;
  result.process().action_summands().clear();
  result.process().deadlock_summands().clear();
  auto& action_summands = result.process().action_summands();
  std::regex re("Z(neg|pos)_(\\d+)_.*");
  std::size_t n = lpsspec.process().process_parameters().size();

  for (int vi: V)
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

      summand.condition() = data::join_and(condition.begin(), condition.end());
      summand.multi_action().actions() = process::action_list(actions.begin(), actions.end());
      summand.assignments() = data::assignment_list(next_state_assignments.begin(), next_state_assignments.end());

      action_summands.push_back(summand);
    }
  }
  return result;
}

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
void create_counter_example_lts(structure_graph& G, const std::set<int>& V, lts::lts_lts_t& ltsspec, const pbes& p, const pbes_equation_index& p_index)
{
  std::regex re("Z(neg|pos)_(\\d+)_.*");

  std::set<std::size_t> transition_indices;
  for (int vi: V)
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

/// \param lpsspec The original LPS that was used to create the PBES.
inline
std::pair<bool, lps::specification> solve_structure_graph_with_counter_example(structure_graph& G, const lps::specification& lpsspec, const pbes& p, const pbes_equation_index& p_index)
{
  mCRL2log(log::verbose) << "Solving parity game..." << std::endl;
  vertex_set Wconj;
  vertex_set Wdisj;
  std::tie(Wdisj, Wconj) = solve_recursive_extended(G);
  int init = G.initial_vertex();

  if (Wdisj.contains(init))
  {
    mCRL2log(log::verbose) << "Extracting witness..." << std::endl;
    std::set<int> W = find_counter_example_nodes(G, Wdisj, init, true);
    return { true, create_counter_example_lps(G, W, lpsspec, p, p_index) };
  }
  else if (Wconj.contains(init))
  {
    mCRL2log(log::verbose) << "Extracting counter example..." << std::endl;
    std::set<int>  W = find_counter_example_nodes(G, Wconj, init, false);
    return { false, create_counter_example_lps(G, W, lpsspec, p, p_index) };
  }
  throw mcrl2::runtime_error("No solution found in solve_structure_graph!");
}

/// \param ltsspec The original LTS that was used to create the PBES.
inline
bool solve_structure_graph_with_counter_example(structure_graph& G, lts::lts_lts_t& ltsspec, const pbes& p, const pbes_equation_index& p_index)
{
  mCRL2log(log::verbose) << "Solving parity game..." << std::endl;
  vertex_set Wconj;
  vertex_set Wdisj;
  std::tie(Wdisj, Wconj) = solve_recursive_extended(G);
  int init = G.initial_vertex();

  if (Wdisj.contains(init))
  {
    mCRL2log(log::verbose) << "Extracting witness..." << std::endl;
    std::set<int>  W = find_counter_example_nodes(G, Wdisj, init, true);
    create_counter_example_lts(G, W, ltsspec, p, p_index);
    return true;
  }
  else if (Wconj.contains(init))
  {
    mCRL2log(log::verbose) << "Extracting counter example..." << std::endl;
    std::set<int>  W = find_counter_example_nodes(G, Wconj, init, false);
    create_counter_example_lts(G, W, ltsspec, p, p_index);
    return false;
  }
  throw mcrl2::runtime_error("No solution found in solve_structure_graph!");
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_SOLVE_STRUCTURE_GRAPH_H
