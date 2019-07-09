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

#include <algorithm>
#include <deque>
#include "mcrl2/pbes/structure_graph.h"

namespace mcrl2 {

namespace pbes_system {

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
bool is_subset_of(const vertex_set& V, const vertex_set& W)
{
  return V.include().is_subset_of(W.include());
}

template <typename StructureGraph>
void log_vertex_set(const StructureGraph& G, const vertex_set& V, const std::string& name)
{
  mCRL2log(log::debug) << "--- " << name << " ---" << std::endl;
  for (auto v: V.vertices())
  {
    mCRL2log(log::debug) << "  " << v << " " << G.find_vertex(v) << std::endl;
  }
  // mCRL2log(log::debug) << "\n";
  // mCRL2log(log::debug) << "exclude = " << G.exclude() << "\n";
}

namespace detail {

template <typename T>
std::vector<T> sorted(const std::vector<T>& x)
{
  std::vector<T> result = x;
  std::sort(result.begin(), result.end());
  return result;
}

inline
void log_vertex_set(const vertex_set& V, const std::string& name)
{
  mCRL2log(log::debug) << name << " = " << core::detail::print_set(sorted(V.vertices())) << std::endl;
}

} // namespace detail

template <typename StructureGraph>
std::set<structure_graph::index_type> extract_minimal_structure_graph(StructureGraph& G, typename StructureGraph::index_type init, const vertex_set& S0, const vertex_set& S1)
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
    if ((S0.contains(u) && G.decoration(u) == structure_graph::d_disjunction) || (S1.contains(u) && G.decoration(u) == structure_graph::d_conjunction))
    {
      // explore only the strategy edge
      structure_graph::index_type v = G.strategy(u);
      assert (v != structure_graph::undefined_vertex);
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

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESSOLVE_VERTEX_SET_H
