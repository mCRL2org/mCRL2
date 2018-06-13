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

    explicit vertex_set(std::size_t N)
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

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESSOLVE_VERTEX_SET_H
