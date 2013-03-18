// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/stategraph_dependency_graph.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_STATEGRAPH_DEPENDENCY_GRAPH_H
#define MCRL2_PBES_DETAIL_STATEGRAPH_DEPENDENCY_GRAPH_H

#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/sequence.h"
#include "mcrl2/utilities/text_utility.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

struct dependency_vertex
{
  core::identifier_string X;
  std::size_t p;
  std::set<dependency_vertex*> outgoing_edges;
  std::set<dependency_vertex*> incoming_edges;

  dependency_vertex(const core::identifier_string& X_, std::size_t p_)
    : X(X_), p(p_)
  {}

  std::string print() const
  {
    std::ostringstream out;
    out << "(" << std::string(X) << ", " << p << ")";
    return out.str();
  }

  bool operator<(const dependency_vertex& other)
  {
    if (X != other.X)
    {
      return X < other.X;
    }
    return (p < other.p);
  }
};

struct dependency_graph
{
  typedef std::map<std::size_t, dependency_vertex*> vertex_map;
  typedef std::map<core::identifier_string, const dependency_vertex*> constraint_map;

  std::vector<dependency_vertex> m_vertices;

  // needs to be initialized after inserting vertices!
  std::map<core::identifier_string, vertex_map> m_index;

  bool has_vertex(const dependency_vertex* u)
  {
    for (std::vector<dependency_vertex>::iterator i = m_vertices.begin(); i != m_vertices.end(); ++i)
    {
      dependency_vertex& v = *i;
      if (&v == u)
      {
        return true;
      }
    }
    return false;
  }

  void self_check()
  {
    for (std::vector<dependency_vertex>::iterator i = m_vertices.begin(); i != m_vertices.end(); ++i)
    {
      dependency_vertex& u = *i;
      for (std::set<dependency_vertex*>::iterator j = u.outgoing_edges.begin(); j != u.outgoing_edges.end(); ++j)
      {
        if (!has_vertex(*j))
        {
          std::cout << "error: vertex not found!" << std::endl;
        }
      }
    }
  }

  void set_index()
  {
    for (std::vector<dependency_vertex>::iterator i = m_vertices.begin(); i != m_vertices.end(); ++i)
    {
      m_index[i->X][i->p] = &(*i);
    }
  }

  dependency_graph()
  {}

  dependency_graph(const dependency_graph& other)
    : m_vertices(other.m_vertices)
  {
    const dependency_vertex* old_first_vertex = &other.m_vertices[0];
    dependency_vertex* new_first_vertex = &m_vertices[0];

    // reset the pointers
    for (std::size_t i = 0; i < m_vertices.size(); i++)
    {
      const std::set<dependency_vertex*>& old_edges = m_vertices[i].outgoing_edges;
      std::set<dependency_vertex*>& new_edges = m_vertices[i].outgoing_edges;
      new_edges.clear();
      for (std::set<dependency_vertex*>::iterator j = old_edges.begin(); j != old_edges.end(); ++j)
      {
        std::size_t index = *j - old_first_vertex;
        new_edges.insert(new_first_vertex + index);
      }
    }
    set_index();
  
    // reset incoming edges
    for (auto i = m_vertices.begin(); i != m_vertices.end(); ++i)
    {
      dependency_vertex& u = *i;
      u.incoming_edges.clear();
    }
    for (auto i = m_vertices.begin(); i != m_vertices.end(); ++i)
    {
      dependency_vertex& u = *i;
      std::set<dependency_vertex*>& S = u.outgoing_edges;
      for (auto j = S.begin(); j != S.end(); ++j)
      {
        dependency_vertex& v = **j;
        v.incoming_edges.insert(&u);
      }
    }

  //self_check();
  }

  // @pre: (X, p) is in the graph
  dependency_vertex& find_vertex(const core::identifier_string& X, std::size_t p)
  {
    if (m_index.find(X) == m_index.end())
    {
      std::cerr << "error: entry " << std::string(X) << " not found in the graph!" << std::endl;
      assert(false);
    }
    vertex_map& m = m_index[X];
    if (m.find(p) == m.end())
    {
      std::cerr << "vertex not found: (X, p) = (" << std::string(X) << ", " << p << ")" << std::endl;
      assert(false);
    }
    return *(m_index[X][p]);
  }

  // @pre: (X, p) is not in the graph
  void insert_vertex(const core::identifier_string& X, std::size_t p)
  {
    mCRL2log(log::debug1, "stategraph") << "insert vertex (" << std::string(X) << ", " << p << ")" << std::endl;
    m_vertices.push_back(dependency_vertex(X, p));
    //self_check();
  }

  // insert edge between (X, i) and (Y, j)
  void insert_edge(const core::identifier_string& X, std::size_t i, const core::identifier_string& Y, std::size_t j)
  {
    mCRL2log(log::debug1, "stategraph") << "insert edge (" << std::string(X) << ", " << i << ") (" << std::string(Y) << ", " << j << ")" << std::endl;
    dependency_vertex& u = find_vertex(X, i);
    dependency_vertex& v = find_vertex(Y, j);
    u.outgoing_edges.insert(&v);
    v.incoming_edges.insert(&u);
  }

  std::string print()
  {
    std::ostringstream out;
    for (std::vector<dependency_vertex>::const_iterator i = m_vertices.begin(); i != m_vertices.end(); ++i)
    {
      const dependency_vertex& u = *i;
      out << u.print() << " edges = ";
      for (std::set<dependency_vertex*>::const_iterator j = u.outgoing_edges.begin(); j != u.outgoing_edges.end(); ++j)
      {
        const dependency_vertex& v = **j;
        out << " (" << u.print() << ", " << v.print() << ")";
      }
      out << std::endl;
    }
    return out.str();
  }

  const std::vector<dependency_vertex>& vertices() const
  {
    return m_vertices;
  }

  std::vector<dependency_vertex>& vertices()
  {
    return m_vertices;
  }

  bool check_constraint2(const dependency_vertex& u, constraint_map& m) const
  {
    constraint_map::iterator i = m.find(u.X);
    if (i == m.end())
    {
      m[u.X] = &u;
    }
    else
    {
      // if we end up in a vertex with known X, then these vertices must coincide
      const dependency_vertex& v = *(i->second);
      return &u == &v;
    }
    const std::set<dependency_vertex*>& S = u.outgoing_edges;
    for (std::set<dependency_vertex*>::const_iterator j = S.begin(); j != S.end(); ++j)
    {
      const dependency_vertex& v = **j;
      if (!check_constraint2(v, m))
      {
        return false;
      }
    }
    return true;
  }

  bool check_constraint3(const dependency_vertex& u, constraint_map& m) const
  {
    constraint_map::iterator i = m.find(u.X);
    if (i == m.end())
    {
      m[u.X] = &u;
    }
    else
    {
      // if we end up in a vertex with known X, then these vertices must coincide
      const dependency_vertex& v = *(i->second);
      return &u == &v;
    }
    const std::set<dependency_vertex*>& S = u.incoming_edges;
    for (std::set<dependency_vertex*>::const_iterator j = S.begin(); j != S.end(); ++j)
    {
      const dependency_vertex& v = **j;
      if (!check_constraint3(v, m))
      {
        return false;
      }
    }
    return true;
  }

  // checks two constraints for all vertices (X, p):
  // 1) if (X, p) -> (Y, q) and (X, p) -> (Y, r) then q = r
  // 2) if (X, p) ->* (X, q) then p = q
  // 3) if (X, p) ->* (Y, r) and (X, q) ->* (Y, r) then p = q
  bool check_constraints() const
  {
    // check 1)
    for (std::vector<dependency_vertex>::const_iterator i = m_vertices.begin(); i != m_vertices.end(); ++i)
    {
      const dependency_vertex& u = *i;
      std::map<core::identifier_string, std::size_t> index;
      const std::set<dependency_vertex*>& S = u.outgoing_edges;
      for (std::set<dependency_vertex*>::const_iterator j = S.begin(); j != S.end(); ++j)
      {
        const dependency_vertex& v = **j;
        std::map<core::identifier_string, std::size_t>::iterator k = index.find(v.X);
        if (k == index.end())
        {
          index[v.X] = v.p;
        }
        else
        {
          if (v.p != k->second)
          {
            return false;
          }
        }
      }
    }

    // check 2)
    std::set<const dependency_vertex*> done;
    for (std::size_t i = 0; i < m_vertices.size(); i++)
    {
      const dependency_vertex& u = m_vertices[i];
      if (done.find(&u) != done.end())
      {
        continue;
      }
      constraint_map m;
      if (!check_constraint2(u, m))
      {
        return false;
      }
      for (constraint_map::const_iterator j = m.begin(); j != m.end(); ++j)
      {
        done.insert(j->second);
      }
    }

    // check 3)
    done.clear();
    for (std::size_t i = 0; i < m_vertices.size(); i++)
    {
      const dependency_vertex& u = m_vertices[i];
      if (done.find(&u) != done.end())
      {
        continue;
      }
      constraint_map m;
      if (!check_constraint3(u, m))
      {
        return false;
      }
      for (constraint_map::const_iterator j = m.begin(); j != m.end(); ++j)
      {
        done.insert(j->second);
      }
    }

    return true;
  }

  void compute_dependency_map(const dependency_vertex& u, std::map<core::identifier_string, std::size_t>& result) const
  {
    if (result.find(u.X) != result.end())
    {
      return;
    }
    result[u.X] = u.p;
    const std::set<dependency_vertex*>& S = u.outgoing_edges;
    for (std::set<dependency_vertex*>::const_iterator j = S.begin(); j != S.end(); ++j)
    {
      const dependency_vertex& v = **j;
      compute_dependency_map(v, result);
    }
  }

  // returns a mapping of names to indices, induced by the graph, and starting in (X, p)
  std::map<core::identifier_string, std::size_t> dependency_map(const core::identifier_string& X, std::size_t p)
  {
    std::map<core::identifier_string, std::size_t> result;
    dependency_vertex& u = find_vertex(X, p);
    compute_dependency_map(u, result);
    return result;
  }
};

inline
std::pair<core::identifier_string, std::size_t> parse_dependency_vertex(const std::string& text)
{
  std::istringstream from(text);
  std::string X;
  std::size_t p;
  from >> X >> p;
  if (!from)
  {
    throw mcrl2::runtime_error("error: could not parse local vertex " + text);
  }
  return std::make_pair(core::identifier_string(X), p);
}

// The first line contains a ';'-separated sequence of vertices.
// Each subsequent line contains a ';'-separated sequence of 2 vertices, which is interpreted as an edge.
inline
dependency_graph parse_dependency_graph(const std::string& text)
{
  dependency_graph result;

  std::vector<std::string> lines = utilities::regex_split(text, "\n");

  // insert vertices
  std::vector<std::string> vertices = utilities::split(lines[0], ";");
  for (std::vector<std::string>::const_iterator j = vertices.begin(); j != vertices.end(); ++j)
  {
    std::pair<core::identifier_string, std::size_t> q = parse_dependency_vertex(*j);
    result.insert_vertex(q.first, q.second);
  }

  result.set_index();

  // insert edges
  for (std::vector<std::string>::const_iterator i = ++lines.begin(); i != lines.end(); ++i)
  {
    std::vector<std::string> vertices = utilities::split(boost::trim_copy(*i), ";");
    if (vertices.size() != 2)
    {
      throw mcrl2::runtime_error("error: could not parse local edge " + *i);
    }
    std::pair<core::identifier_string, std::size_t> u = parse_dependency_vertex(vertices[0]);
    std::pair<core::identifier_string, std::size_t> v = parse_dependency_vertex(vertices[1]);
    result.insert_edge(u.first, u.second, v.first, v.second);
  }

  return result;
}

struct remove_may_transitions_finished
{};

struct remove_may_transitions_helper
{
  // N.B. sources and targets are in the may-graph
  const std::vector<dependency_vertex*>& sources;
  std::vector<dependency_vertex*>& targets;

  // G is the must-graph
  dependency_graph& G;

  remove_may_transitions_helper(const std::vector<dependency_vertex*>& sources_, std::vector<dependency_vertex*>& targets_, dependency_graph& G_)
    : sources(sources_),
      targets(targets_),
      G(G_)
  {}

  // checks if G satisfies the constraints after adding edges between sources[i] and targets[i], for i in [0, ..., sources[i].size())
  // throws remove_may_transitions_finished() if the constraints are satisfied; the edges of the satisfying assignment are added to G
  void operator()()
  {
    assert(sources.size() == targets.size());

    // add edges to G
    for (std::size_t i = 0; i < sources.size(); i++)
    {
      dependency_vertex& u = *(sources[i]);
      dependency_vertex& v = *(targets[i]);
      dependency_vertex& u1 = G.find_vertex(u.X, u.p);
      dependency_vertex& v1 = G.find_vertex(v.X, v.p);
      u1.outgoing_edges.insert(&v1);
      v1.incoming_edges.insert(&u1);
    }

    bool result = G.check_constraints();
    if (result)
    {
      throw remove_may_transitions_finished();
    }

    // remove edges from G
    for (std::size_t i = 0; i < sources.size(); i++)
    {
      dependency_vertex& u = *(sources[i]);
      dependency_vertex& v = *(targets[i]);
      dependency_vertex& u1 = G.find_vertex(u.X, u.p);
      dependency_vertex& v1 = G.find_vertex(v.X, v.p);
      u1.outgoing_edges.erase(&v1);
      v1.incoming_edges.erase(&u1);
    }
  }
};

// returns all unique targets of outgoing edges, i.e. with the property
// (Y, p) and (Y, q) \in targets => p = q
inline
std::set<dependency_vertex*> single_target_edges(const dependency_vertex& u)
{
  const std::set<dependency_vertex*>& S = u.outgoing_edges;
  std::map<core::identifier_string, std::vector<dependency_vertex*> > m;
  for (std::set<dependency_vertex*>::const_iterator i = S.begin(); i != S.end(); ++i)
  {
    dependency_vertex& v = **i;
    m[v.X].push_back(*i);
  }
  std::set<dependency_vertex*> result;
  for (std::map<core::identifier_string, std::vector<dependency_vertex*> >::iterator j = m.begin(); j != m.end(); ++j)
  {
    if (j->second.size() == 1)
    {
      result.insert(j->second.front());
    }
  }
  return result;
}

// Returns true if a control flow graph is found that satisfies the constraints.
template <typename VertexCompare>
bool remove_may_transitions(dependency_graph& must_graph, dependency_graph& may_graph, VertexCompare comp)
{
  // pass 1: handle all edges for which there is no choice
  std::vector<dependency_vertex>& V = may_graph.vertices();
  for (std::vector<dependency_vertex>::iterator i = V.begin(); i != V.end(); ++i)
  {
    dependency_vertex& u = *i;
    std::set<dependency_vertex*> E = single_target_edges(u);
    for (std::set<dependency_vertex*>::const_iterator i = E.begin(); i != E.end(); ++i)
    {
      dependency_vertex& v = **i;
      u.outgoing_edges.erase(*i);
      must_graph.insert_edge(u.X, u.p, v.X, v.p);
    }
  }

  // pass 2: handle all edges for which there is a choice
  std::vector<dependency_vertex*> sources;         // the sources of the edges for which there is a choice
  std::vector<std::vector<dependency_vertex*> > T; // T[i] contains the possible targets for sources[i]
  std::vector<dependency_vertex*> targets;         // targets[i] will hold an element of T[i]

  // initialization of sources, T and targets
  for (std::vector<dependency_vertex>::iterator i = V.begin(); i != V.end(); ++i)
  {
    dependency_vertex& u = *i;
    const std::set<dependency_vertex*>& S = u.outgoing_edges;
    std::map<core::identifier_string, std::vector<dependency_vertex*> > m;
    for (std::set<dependency_vertex*>::const_iterator i = S.begin(); i != S.end(); ++i)
    {
      dependency_vertex& v = **i;
      m[v.X].push_back(*i);
    }
    for (std::map<core::identifier_string, std::vector<dependency_vertex*> >::iterator j = m.begin(); j != m.end(); ++j)
    {
      sources.push_back(&u);
      T.push_back(j->second);
      targets.push_back(*(j->second.begin()));
    }
  }

  // sort the sequences T[i] using the supplied function sort_vertices
  for (std::size_t i = 0; i < T.size(); i++)
  {
    comp.source = sources[i];
    std::sort(T[i].begin(), T[i].end(), comp);
  }

  // for each possible sequence of targets, check if the must_graph fulfills all constraints
  try
  {
    utilities::foreach_sequence(T, targets.begin(), remove_may_transitions_helper(sources, targets, must_graph));
  }
  catch(remove_may_transitions_finished&)
  {
    return true;
  }
  return false;
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_DEPENDENCY_GRAPH_H
