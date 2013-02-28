// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/stategraph_local_graph.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_STATEGRAPH_LOCAL_GRAPH_H
#define MCRL2_PBES_DETAIL_STATEGRAPH_LOCAL_GRAPH_H

#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/text_utility.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

struct local_vertex
{
  core::identifier_string X;
  std::size_t p;
  std::set<local_vertex*> outgoing_edges;

  local_vertex(const core::identifier_string& X_, std::size_t p_)
    : X(X_), p(p_)
  {}

  std::string print() const
  {
    std::ostringstream out;
    out << "(" << std::string(X) << ", " << p << ")";
    return out.str();
  }
};

struct local_graph
{
  typedef std::map<std::size_t, local_vertex*> vertex_map;
  typedef std::map<core::identifier_string, const local_vertex*> constraint_map;

  std::vector<local_vertex> m_vertices;

  // needs to be initialized after inserting vertices!
  std::map<core::identifier_string, vertex_map> m_index;

  // @pre: (X, p) is in the graph
  local_vertex& find_vertex(const core::identifier_string& X, std::size_t p)
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
    m_vertices.push_back(local_vertex(X, p));
  }

  // insert edge between (X, i) and (Y, j)
  void insert_edge(const core::identifier_string& X, std::size_t i, const core::identifier_string& Y, std::size_t j)
  {
    mCRL2log(log::debug, "stategraph") << "insert edge (" << std::string(X) << ", " << i << ") (" << std::string(Y) << ", " << j << ")" << std::endl;
    local_vertex& u = find_vertex(X, i);
    local_vertex& v = find_vertex(Y, j);
    u.outgoing_edges.insert(&v);
  }

  std::string print()
  {
    std::ostringstream out;
    for (std::vector<local_vertex>::const_iterator i = m_vertices.begin(); i != m_vertices.end(); ++i)
    {
      const local_vertex& u = *i;
      out << u.print() << " connected with";
      for (std::set<local_vertex*>::const_iterator j = u.outgoing_edges.begin(); j != u.outgoing_edges.end(); ++j)
      {
        const local_vertex& v = **j;
        out << " " << v.print();
      }
      out << std::endl;
    }
    return out.str();
  }

  const std::vector<local_vertex>& vertices() const
  {
    return m_vertices;
  }

  std::vector<local_vertex>& vertices()
  {
    return m_vertices;
  }

  void set_index()
  {
    for (std::vector<local_vertex>::iterator i = m_vertices.begin(); i != m_vertices.end(); ++i)
    {
      m_index[i->X][i->p] = &(*i);
    }
  }

  bool check_constraints(const local_vertex& u, constraint_map& m) const
  {
    constraint_map::iterator i = m.find(u.X);
    if (i == m.end())
    {
      m[u.X] = &u;
    }
    else
    {
      // if we end up in a vertex with known X, then these vertices must coincide
      const local_vertex& v = *(i->second);
      return &u == &v;
    }
    const std::set<local_vertex*>& S = u.outgoing_edges;
    for (std::set<local_vertex*>::const_iterator j = S.begin(); j != S.end(); ++j)
    {
      const local_vertex& v = **j;
      if (!check_constraints(v, m))
      {
        return false;
      }
    }
    return true;
  }

  // checks two constraints for all vertices (X, p):
  // 1) if (X, p) -> (Y, l) and (X, p) -> (Y, m) then l = m
  // 2) if (X, p) ->* (X, l) then p = l
  bool check_constraints() const
  {
    // check 1)
    for (std::vector<local_vertex>::const_iterator i = m_vertices.begin(); i != m_vertices.end(); ++i)
    {
      const local_vertex& u = *i;
      std::map<core::identifier_string, std::size_t> index;
      const std::set<local_vertex*>& S = u.outgoing_edges;
      for (std::set<local_vertex*>::const_iterator j = S.begin(); j != S.end(); ++j)
      {
        const local_vertex& v = **j;
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
    std::set<const local_vertex*> done;
    for (std::size_t i = 0; i < m_vertices.size(); i++)
    {
      const local_vertex& u = m_vertices[i];
      if (done.find(&u) != done.end())
      {
        continue;
      }
      constraint_map m;
      if (!check_constraints(u, m))
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
};

inline
std::pair<core::identifier_string, std::size_t> parse_local_vertex(const std::string& text)
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
local_graph parse_local_graph(const std::string& text)
{
  local_graph result;

  std::vector<std::string> lines = utilities::regex_split(text, "\n");

  // insert vertices
  std::vector<std::string> vertices = utilities::split(lines[0], ";");
  for (std::vector<std::string>::const_iterator j = vertices.begin(); j != vertices.end(); ++j)
  {
    std::pair<core::identifier_string, std::size_t> q = parse_local_vertex(*j);
    result.insert_vertex(q.first, q.second);
  }

  result.set_index();

  // insert edges
  for (std::vector<std::string>::const_iterator i = ++lines.begin(); i != lines.end(); ++i)
  {
    std::vector<std::string> vertices = utilities::split(boost::trim_copy(*i), ";");
    if (!vertices.size() == 2)
    {
      throw mcrl2::runtime_error("error: could not parse local edge " + *i);
    }
    std::pair<core::identifier_string, std::size_t> u = parse_local_vertex(vertices[0]);
    std::pair<core::identifier_string, std::size_t> v = parse_local_vertex(vertices[1]);
    result.insert_edge(u.first, u.second, v.first, v.second);
  }

  return result;
}

struct remove_may_transitions_finished
{};

struct remove_may_transitions_helper
{
  // N.B. sources and targets are in the may-graph
  const std::vector<local_vertex*>& sources;
  std::vector<local_vertex*>& targets;

  // G is the must-graph
  local_graph& G;

  remove_may_transitions_helper(const std::vector<local_vertex*>& sources_, std::vector<local_vertex*>& targets_, local_graph& G_)
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
      local_vertex& u = *(sources[i]);
      local_vertex& v = *(targets[i]);
      local_vertex& u1 = G.find_vertex(u.X, u.p);
      local_vertex& v1 = G.find_vertex(v.X, v.p);
      u1.outgoing_edges.insert(&v1);
    }

std::cerr << "--- checking constraints on graph ---\n" << G.print() << std::endl;
    bool result = G.check_constraints();
std::cerr << "result = " << std::boolalpha << result << std::endl;
    if (result)
    {
      throw remove_may_transitions_finished();
    }

    // remove edges from G
    for (std::size_t i = 0; i < sources.size(); i++)
    {
      local_vertex& u = *(sources[i]);
      local_vertex& v = *(targets[i]);
      local_vertex& u1 = G.find_vertex(u.X, u.p);
      local_vertex& v1 = G.find_vertex(v.X, v.p);
      u1.outgoing_edges.erase(&v1);
    }
  }
};

// Returns true if a control flow graph is found that satisfies the constraints.
inline
bool remove_may_transitions(local_graph& must_graph, local_graph& may_graph)
{
  // pass 1: handle all vertices for which the number of outgoing may transitions is equal to 1
  std::vector<local_vertex>& V = may_graph.vertices();
  for (std::vector<local_vertex>::iterator i = V.begin(); i != V.end(); ++i)
  {
    local_vertex& u = *i;
    if (u.outgoing_edges.size() == 1)
    {
      local_vertex v = **(u.outgoing_edges.begin());
      u.outgoing_edges.clear();
      must_graph.insert_edge(u.X, u.p, v.X, v.p);
    }
  }

  // pass 2: handle all vertices for which the number of outgoing may transitions is greater than 1
  std::vector<local_vertex*> sources;         // the vertices for which the number of outgoing may transitions is greater than 1
  std::vector<std::vector<local_vertex*> > T; // T[i] contains the targets of the outgoing edges of vertex sources
  std::vector<local_vertex*> targets;         // targets[i] will hold an element of T[i]

  // initialization of sources, T and targets
  for (std::vector<local_vertex>::iterator i = V.begin(); i != V.end(); ++i)
  {
    local_vertex& u = *i;
    if (u.outgoing_edges.size() > 1)
    {
      sources.push_back(&u);
      std::set<local_vertex*>& S = u.outgoing_edges;
      T.push_back(std::vector<local_vertex*>(S.begin(), S.end()));
    }
  }

  // sort the sequences T[i] according to some heuristic
  // TODO

  // initialize targets
  for (std::vector<std::vector<local_vertex*> >::const_iterator i = T.begin(); i != T.end(); ++i)
  {
    targets.push_back(i->front());
  }

  // for each possible sequence of targets, check if the must_graph fulfills all constraints
  try
  {
    utilities::foreach_sequence(T, targets.begin(), remove_may_transitions_helper(sources, targets, must_graph));
  }
  catch(remove_may_transitions_finished&)
  {
    mCRL2log(log::debug, "stategraph") << "=== must graph ===\n" << must_graph.print() << std::endl;
    return true;
  }
  return false;
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_LOCAL_GRAPH_H
