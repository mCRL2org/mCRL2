// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/reachable_nodes.h
/// \brief Compute reachable nodes in a graph.

#ifndef MCRL2_CORE_REACHABLE_NODES_H
#define MCRL2_CORE_REACHABLE_NODES_H

#include <boost/graph/depth_first_search.hpp>
#include <boost/tuple/tuple.hpp>

#include <iterator>
#include <vector>

namespace mcrl2 {
  
namespace core {

/// \cond INTERNAL_DOCS
namespace detail {

  template <typename Graph>
  struct reachable_nodes_recorder: public boost::default_dfs_visitor
  {
    typedef typename Graph::vertex_descriptor vertex_descriptor;
    std::vector<int>& m_result;
  
    reachable_nodes_recorder(std::vector<int>& result)
      : m_result(result)
    {}
  
    /// \brief Is called whenever a new vertex is discovered
    /// \param u A vertex
    /// \param g A graph
    void discover_vertex(vertex_descriptor u, const Graph& g)
    {
      m_result.push_back(boost::get(boost::vertex_index, g)[u]);
    }
  };
} // namespace detail
  /// \endcond
  
  /// \brief Compute reachable nodes in a graph.
  /// \param g A graph.
  /// \param first Iterator to the first node.
  /// \param last Iterator to the last node.
  /// \return The indices of the nodes that are reachable from the nodes
  /// given by the range of vertex descriptors [first, last].
  template <typename Graph, typename Iter>
  std::vector<int> reachable_nodes(const Graph& g, Iter first, Iter last)
  {
    typedef boost::color_traits<boost::default_color_type> Color;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor vertex_descriptor;
    typedef typename boost::graph_traits<Graph>::edge_descriptor edge_descriptor;

    std::vector<int> result;
    detail::reachable_nodes_recorder<Graph> recorder(result);
    std::vector<boost::default_color_type> colormap(boost::num_vertices(g), Color::white());

    for (Iter i = first; i != last; ++i)
    {
      boost::default_color_type c = Color::white();
      boost::depth_first_visit(g, 
                               *i,
                               recorder,
                               boost::make_iterator_property_map(colormap.begin(), boost::get(boost::vertex_index, g), c)
                              );
    }

    return result;
  }
  
} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_REACHABLE_NODES_H
