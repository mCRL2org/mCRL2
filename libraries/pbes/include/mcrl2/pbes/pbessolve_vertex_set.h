// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbessolve_vertex_set.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_PBESSOLVE_VERTEX_SET_H
#define MCRL2_PBES_PBESSOLVE_VERTEX_SET_H

#include <deque>
#include "mcrl2/pbes/structure_graph.h"

namespace mcrl2::pbes_system {

struct deque_vertex_set
{
  protected:
    std::deque<structure_graph::index_type> m_vertices;
    boost::dynamic_bitset<> m_include;

  public:
    deque_vertex_set() = default;

    explicit deque_vertex_set(std::size_t N)
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
      if (m_include[u])
      {
        return;
      }
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
    bool self_check(const std::string& msg = "") const
    {
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
      return true;
    }

    vertex_set() = default;

    vertex_set(const vertex_set& other) = default;
    vertex_set& operator=(const vertex_set& other) = default;

    explicit vertex_set(std::size_t N)
            : m_include(N)
    {
      assert(self_check());
    }

    template <typename Iter>
    vertex_set(std::size_t N, Iter first, Iter last)
            : m_include(N)
    {
      for (auto i = first; i != last; ++i)
      {
        insert(*i);
      }
      assert(self_check());
    }

    // resize to at least n elements
    void resize(std::size_t n)
    {
      std::size_t m = m_include.size();
      if (m < 1024)
      {
        m = 1024;
      }
      while (m < n)
      {
        m *= 2;
      }
      m_include.resize(m);
    }

    // truncate the size to n
    void truncate(std::size_t n)
    {
      if (m_include.size() > n)
      {
        m_include = boost::dynamic_bitset<>(n);
        for (auto u: m_vertices)
        {
          m_include[u] = true;
        }
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
      if (m_include[u])
      {
        return;
      }
      m_vertices.push_back(u);
      m_include[u] = true;
      assert(self_check());
    }

    /// @brief Sorts the set of vertices, but resets the include set.
    void sort()
    {
      std::sort(m_vertices.begin(), m_vertices.end());
      assert(self_check());
    }

    void clear()
    {
      m_vertices.clear();
      m_include = boost::dynamic_bitset<>(m_include.size());
      assert(self_check());
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
    result.insert(w);
  }
  return result;
}

// VertexSet can be either vertex_set or deque_vertex_set
template <typename VertexSet>
vertex_set set_intersection(const VertexSet& V, const vertex_set& W)
{
  vertex_set result(V.extent());
  for (structure_graph::index_type v: V.vertices())
  {
    if (W.contains(v))
    {
      result.insert(v);
    }
  }
  return result;
}

inline
vertex_set set_minus(const vertex_set& V, const vertex_set& W)
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

struct lazy_union
{
  const vertex_set& A;
  const vertex_set& B;

  lazy_union(const vertex_set& A_, const vertex_set& B_)
    : A(A_), B(B_)
  {}

  bool contains(structure_graph::index_type u) const
  {
    return A.contains(u) || B.contains(u);
  }
};

inline
std::ostream& operator<<(std::ostream& out, const lazy_union& V)
{
  return out << "(" << V.A << " U " << V.B << ")";
}

inline
bool is_subset_of(const vertex_set& V, const vertex_set& W)
{
  return V.include().is_subset_of(W.include());
}

// returns a successor of u that is in A, or undefined_index if not found
template <typename StructureGraph, typename VertexSet>
structure_graph::index_type find_successor_in(const StructureGraph& G, structure_graph::index_type u, const VertexSet& A)
{
  for (auto v: G.successors(u))
  {
    if (A.contains(v))
    {
      return v;
    }
  }
  mCRL2log(log::debug) << "No successor found for node " << u << " in " << A << std::endl;
  return undefined_vertex();
}

template <typename StructureGraph>
void log_vertex_set(const StructureGraph& G, const vertex_set& V, const std::string& name)
{
  mCRL2log(log::debug) << "--- " << name << " ---" << std::endl;
  for (auto v: V.vertices())
  {
    mCRL2log(log::debug) << "  " << v << " " << G.find_vertex(v) << std::endl;
  }
}

// strategy vector that resizes automatically
class strategy_vector
{
  protected:
    mutable std::vector<structure_graph::index_type> m_strategy;

    // resize to at least n elements
    void resize(std::size_t n) const
    {
      std::size_t m = m_strategy.size();
      if (m < 16)
      {
        m = 16;
      }
      while (m < n)
      {
        m *= 2;
      }
      m_strategy.resize(m, undefined_vertex());
    }

  public:
    structure_graph::index_type operator[](std::size_t i) const
    {
      if (i >= m_strategy.size())
      {
        resize(i + 1);
      }
      return m_strategy[i];
    }

    structure_graph::index_type& operator[](std::size_t i)
    {
      if (i >= m_strategy.size())
      {
        resize(i + 1);
      }
      return m_strategy[i];
    }

    std::size_t size() const
    {
      return m_strategy.size();
    }

    // truncate the size to n
    void truncate(std::size_t n)
    {
      if (m_strategy.size() > n)
      {
        m_strategy.erase(m_strategy.begin() + n, m_strategy.end());
        m_strategy.shrink_to_fit();
      }
    }

    const std::vector<structure_graph::index_type>& strategy() const
    {
      return m_strategy;
    }
};

inline
std::ostream& operator<<(std::ostream& out, const strategy_vector& tau_alpha)
{
  return out << core::detail::print_list(tau_alpha.strategy());
}

inline
std::string print_strategy_vector(const vertex_set& S_alpha, const strategy_vector& tau_alpha)
{
  std::ostringstream out;
  bool first = true;
  for (structure_graph::index_type u: S_alpha.vertices())
  {
    if (tau_alpha[u] != undefined_vertex())
    {
      if (!first)
      {
        out << ", ";
      }
      out << u << " -> " << tau_alpha[u];
      first = false;
    }
  }
  return out.str();
}

template <typename StructureGraph>
std::set<structure_graph::index_type> extract_minimal_structure_graph(StructureGraph& G, typename StructureGraph::index_type init, const vertex_set& S0, const vertex_set& S1)
{
  using utilities::detail::contains;

  using vertex = structure_graph::vertex;
  std::vector<const vertex*> result;

  std::set<structure_graph::index_type> todo = { init };
  std::set<structure_graph::index_type> done;
  while (!todo.empty())
  {
    structure_graph::index_type u = *todo.begin();
    todo.erase(todo.begin());
    done.insert(u);
    if ((S0.contains(u) && G.decoration(u) == structure_graph::d_disjunction) || (S1.contains(u) && G.decoration(u) == structure_graph::d_conjunction))
    {
      // explore only the strategy edge
      structure_graph::index_type v = G.strategy(u);
      assert (v != undefined_vertex());
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
  mCRL2log(log::debug) << "Extracted minimal structure graph " << core::detail::print_set(done) << std::endl;
  for (const auto& index : done) {
    mCRL2log(log::debug) << std::setw(4) << index << " " << G.find_vertex(index) << std::endl;
  }

  return done;
}

template <typename StructureGraph>
std::set<structure_graph::index_type> extract_minimal_structure_graph(
  StructureGraph& G,
  typename StructureGraph::index_type init,
  const vertex_set& S0,
  const vertex_set& S1,
  const strategy_vector& tau0,
  const strategy_vector& tau1
)
{
  using utilities::detail::contains;

  using vertex = structure_graph::vertex;
  std::vector<const vertex*> result;

  std::set<structure_graph::index_type> todo = { init };
  std::set<structure_graph::index_type> done;
  while (!todo.empty())
  {
    structure_graph::index_type u = *todo.begin();
    todo.erase(todo.begin());
    done.insert(u);
    if (S0.contains(u) && G.decoration(u) == structure_graph::d_disjunction)
    {
      // explore only the strategy edge
      structure_graph::index_type v = tau0[u];
      assert (v != undefined_vertex());
      if (!contains(done, v))
      {
        todo.insert(v);
      }
    }
    else if (S1.contains(u) && G.decoration(u) == structure_graph::d_conjunction)
    {
      // explore only the strategy edge
      structure_graph::index_type v = tau1[u];
      assert (v != undefined_vertex());
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
  mCRL2log(log::debug) << "\nExtracted minimal structure graph " << core::detail::print_set(done) << std::endl;
  for (const auto& index : done) {
    mCRL2log(log::debug) << std::setw(4) << index << " " << G.find_vertex(index) << std::endl;
  }

  return done;
}

template <typename StructureGraph>
std::set<structure_graph::index_type> extract_minimal_structure_graph(StructureGraph& G, typename StructureGraph::index_type init, bool is_disjunctive)
{
  std::size_t n = G.extent();

  vertex_set all(n);
  for (std::size_t i = 0; i < n; i++)
  {
    all.insert(i);
  }
  vertex_set empty(n);

  if (is_disjunctive)
  {
    return extract_minimal_structure_graph(G, init, all, empty);
  }
  else
  {
    return extract_minimal_structure_graph(G, init, empty, all);
  }
}

} // namespace mcrl2::pbes_system



#endif // MCRL2_PBES_PBESSOLVE_VERTEX_SET_H
