// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/structure_graph_io.h
/// \brief Binary serialization of structure graphs, used to pass the structure
///        graph computed by the symbolic solver to pbescegps.

#ifndef MCRL2_PBES_STRUCTURE_GRAPH_IO_H
#define MCRL2_PBES_STRUCTURE_GRAPH_IO_H

#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/aterm_io_binary.h"
#include "mcrl2/pbes/structure_graph.h"
#include "mcrl2/pbes/structure_graph_builder.h"
#include "mcrl2/utilities/exception.h"
#include <boost/dynamic_bitset.hpp>
#include <cstddef>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

namespace mcrl2::pbes_system
{

/// \brief Writes a structure graph to a stream in binary aterm format.
/// \details A sequence of aterms: extent, initial vertex, excluded vertices, then
///          per vertex its formula, decoration, rank, strategy and successors.
inline void save_structure_graph(const structure_graph& G, std::ostream& out)
{
  atermpp::binary_aterm_ostream stream(out);

  stream << atermpp::aterm_int(G.extent());
  stream << atermpp::aterm_int(G.initial_vertex());

  const boost::dynamic_bitset<>& exclude = G.exclude();
  std::size_t excluded_count = exclude.count();
  stream << atermpp::aterm_int(excluded_count);
  for (std::size_t i = 0; i < exclude.size(); ++i)
  {
    if (exclude[i])
    {
      stream << atermpp::aterm_int(i);
    }
  }

  for (structure_graph::index_type i = 0; i < G.extent(); ++i)
  {
    const structure_graph::vertex& v = G.find_vertex(i);
    stream << v.formula();
    stream << atermpp::aterm_int(static_cast<std::size_t>(v.decoration));
    stream << atermpp::aterm_int(v.rank);
    stream << atermpp::aterm_int(v.strategy);
    stream << atermpp::aterm_int(v.successors.size());
    for (structure_graph::index_type successor: v.successors)
    {
      stream << atermpp::aterm_int(successor);
    }
  }
}

/// \brief Writes a structure graph to a file in binary aterm format.
inline void save_structure_graph(const structure_graph& G, const std::string& filename)
{
  std::ofstream out(filename, std::ios::binary);
  if (!out)
  {
    throw mcrl2::runtime_error("Could not open '" + filename + "' for writing a structure graph.");
  }
  save_structure_graph(G, out);
}

/// \brief Reads a structure graph from a stream in binary aterm format.
inline structure_graph load_structure_graph(std::istream& in)
{
  atermpp::binary_aterm_istream stream(in);

  auto read_size = [&stream]() -> std::size_t
  {
    atermpp::aterm_int value;
    stream >> value;
    return value.value();
  };

  structure_graph G;
  detail::manual_structure_graph_builder builder(G);

  const std::size_t extent = read_size();
  const std::size_t initial_vertex = read_size();

  const std::size_t excluded_count = read_size();
  std::vector<std::size_t> excluded;
  excluded.reserve(excluded_count);
  for (std::size_t i = 0; i < excluded_count; ++i)
  {
    excluded.push_back(read_size());
  }

  for (std::size_t i = 0; i < extent; ++i)
  {
    atermpp::aterm term;
    stream >> term;
    pbes_expression formula = atermpp::down_cast<pbes_expression>(term);
    const auto decoration = static_cast<structure_graph::decoration_type>(read_size());
    const std::size_t rank = read_size();
    const auto strategy = static_cast<structure_graph::index_type>(read_size());
    const std::size_t successor_count = read_size();
    std::vector<structure_graph::index_type> successors;
    successors.reserve(successor_count);
    for (std::size_t j = 0; j < successor_count; ++j)
    {
      successors.push_back(static_cast<structure_graph::index_type>(read_size()));
    }

    builder.m_vertices.emplace_back(formula, decoration, rank);
    structure_graph::vertex& v = builder.m_vertices.back();
    v.strategy = strategy;
    v.successors = std::move(successors);
  }

  // Reconstruct the predecessor lists from the successor lists.
  for (std::size_t i = 0; i < builder.m_vertices.size(); ++i)
  {
    const structure_graph::vertex& u = builder.m_vertices[i];
    for (structure_graph::index_type successor: u.successors)
    {
      structure_graph::vertex& v = builder.m_vertices[successor];
      v.predecessors.push_back(static_cast<structure_graph::index_type>(i));
    }
  }

  builder.set_initial_state(static_cast<structure_graph::index_type>(initial_vertex));
  builder.finalize();

  // finalize() resets the exclude set, so restore it afterwards.
  G.exclude() = boost::dynamic_bitset<>(extent);
  for (std::size_t i: excluded)
  {
    if (i < extent)
    {
      G.exclude()[i] = true;
    }
  }

  return G;
}

/// \brief Reads a structure graph from a file in binary aterm format.
inline structure_graph load_structure_graph(const std::string& filename)
{
  std::ifstream in(filename, std::ios::binary);
  if (!in)
  {
    throw mcrl2::runtime_error("Could not open '" + filename + "' for reading a structure graph.");
  }
  return load_structure_graph(in);
}

} // namespace mcrl2::pbes_system

#endif // MCRL2_PBES_STRUCTURE_GRAPH_IO_H
