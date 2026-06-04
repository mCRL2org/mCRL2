// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/explorer_control_flow.h
/// \brief add your file description here.


#ifndef MCRL2_EXPLORER_CONTROL_FLOW_H
#define MCRL2_EXPLORER_CONTROL_FLOW_H

#include <cassert>
#include <chrono>
#include <iostream>
#include <boost/container/small_vector.hpp>
#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include "mcrl2/data/variable.h"
#include "mcrl2/pbes/detail/lpsstategraph_algorithm.h"
#include "mcrl2/pbes/io.h"

namespace mcrl2::lps
{

struct cf_edge;

struct cf_vertex
{
  std::size_t j;                    // index of the process parameter
  data::data_expression value;      // value of the process parameter
  std::vector<cf_edge*> edges;      // set of outgoing edges
  boost::dynamic_bitset<> enabled;  // bitset of possibly enabled summands
};

struct cf_edge
{
  cf_vertex* source;             // the source of this edge
  cf_vertex* target;             // the target of this edge
  boost::dynamic_bitset<> mask;  // bitset of summands responsible for this transition
};

struct cf_graph
{
  std::vector<cf_vertex> V;                            // vertices of the graph
  std::vector<cf_edge> E;                              // edges of the graph
  std::size_t j;                                       // index of the control flow parameter of the graph
  std::map<data::data_expression, std::size_t> index;  // mapping from parameter values to indices in V
  boost::dynamic_bitset<> relevant;                    // bitset of summands relevant to this CFG
};

using edge_label_map = std::unordered_map<const cf_edge*, std::set<std::size_t>>;

inline
bool cfg_mentions_summand(const cf_graph& G, std::size_t s)
{
  for (const cf_edge& e : G.E)
  {
    if (e.mask.test(s))
    {
      return true;
    }
  }
  return false;
}

inline
void initialize_control_flow_bitsets(cf_graph& G, const edge_label_map& edge_labels, std::size_t number_of_summands)
{
  // Initialize vertex enabled bitsets
  for (cf_vertex& v : G.V)
  {
    v.enabled = boost::dynamic_bitset<>(number_of_summands);
  }

  // Initialize edge masks using the external map
  for (cf_edge& e: G.E)
  {
    e.mask = boost::dynamic_bitset<>(number_of_summands);

    auto it = edge_labels.find(&e);
    if (it != edge_labels.end())
    {
      for (std::size_t summand: it->second)
      {
        e.mask.set(summand);
      }
    }
  }

  // Compute enabled bitsets per vertex
  for (cf_vertex& v: G.V)
  {
    for (cf_edge* e: v.edges)
    {
      v.enabled |= e->mask;
    }
  }

  // Set G.relevant bitset
  G.relevant = boost::dynamic_bitset<>(number_of_summands);
  for (const cf_vertex& v: G.V)
  {
    G.relevant |= v.enabled; // union of all edge masks
  }
}

inline
void print_edge_label_map(const edge_label_map& m)
{
  std::cout << "\n=== Edge label map ===\n";

  for (const auto& [edge, labels] : m)
  {
    std::cout << "edge " << edge
              << "  [" << edge->source->value
              << " -> " << edge->target->value << "]"
              << "  labels = {";

    bool first = true;
    for (auto l : labels)
    {
      if (!first) std::cout << ", ";
      std::cout << l;
      first = false;
    }

    std::cout << "}\n";
  }

  std::cout << "======================\n";
}

inline
std::vector<cf_graph> extract_local_control_flow_graphs(const pbes_system::detail::lpsstategraph_algorithm& algorithm)
{
  using pbes_system::detail::local_control_flow_graph_vertex;

  std::size_t action_summand_count = algorithm.original_lps().process().action_summands().size();

  const auto& local_graphs = algorithm.local_graphs();
  std::vector<cf_graph> result;
  result.reserve(local_graphs.size());

  for (const auto& lg: local_graphs)
  {
    cf_graph g;

    // All vertices correspond to the same control-flow parameter
    if (!lg.vertices.empty())
    {
      g.j = lg.vertices.begin()->index();
    }

    // Reserve to keep pointers stable
    g.V.reserve(lg.vertices.size());

    // Map local CFG vertices to indices in g.V
    std::map<const local_control_flow_graph_vertex*, std::size_t> vertex_index;

    // First pass: create vertices
    for (const auto& v: lg.vertices)
    {
      cf_vertex cv;
      cv.j     = g.j;
      cv.value = v.value();
      // cv.enabled left empty on purpose

      g.V.push_back(std::move(cv));
      std::size_t i = g.V.size() - 1;

      g.index[g.V[i].value] = i;
      vertex_index[&v] = i;
    }

    // Estimate number of edges to reserve storage
    std::size_t edge_count = 0;
    for (const auto& v: lg.vertices)
    {
      for (const auto& e: v.outgoing_edges())
      {
        edge_count += e.second.size();
      }
    }
    g.E.reserve(edge_count);

    // Second pass: create edges
    edge_label_map edge_labels;

    for (const auto& v: lg.vertices)
    {
      cf_vertex* source = &g.V[vertex_index[&v]];

      for (const auto& entry: v.outgoing_edges())
      {
        const local_control_flow_graph_vertex* tgt = entry.first;
        const std::set<std::size_t>& labels = entry.second;

        cf_vertex* target = &g.V[vertex_index[tgt]];

        for (std::size_t label: labels)
        {
          if (label >= action_summand_count)
          {
            continue; // ignore deadlock summands
          }
          cf_edge e;
          e.source = source;
          e.target = target;
          // e.mask intentionally left empty

          g.E.push_back(std::move(e));

          cf_edge* eptr = &g.E.back();
          source->edges.push_back(eptr);
          edge_labels[eptr].insert(label);
        }
      }
    }

    initialize_control_flow_bitsets(g, edge_labels, action_summand_count);
    result.push_back(std::move(g));

    if (mCRL2logEnabled(log::debug))
    {
      print_edge_label_map(edge_labels);
    }
  }

  return result;
}

inline
std::vector<cf_graph> compute_control_flow_graphs(lps::specification& lpsspec, const pbes_system::pbesstategraph_options& stategraph_options, bool compute_marking = false)
{
  if (stategraph_options.use_global_variant)
  {
    throw mcrl2::runtime_error("The use global variant option is not supported in lpsstategraph!");
  }
  if (stategraph_options.print_influence_graph)
  {
    throw mcrl2::runtime_error("The print influence graph option is not supported in lpsstategraph!");
  }
  lps::detail::instantiate_global_variables(lpsspec);
  pbes_system::detail::lpsstategraph_algorithm algorithm(lpsspec, stategraph_options);

  if (compute_marking)
  {
    algorithm.run();
    algorithm.local_graphs().pop_back();
  }
  else
  {
    algorithm.execute_preprocessing();
    algorithm.execute_local_graphs_step();  // N.B. The 'extra' graph is not included
  }

  return extract_local_control_flow_graphs(algorithm);
}

template <typename Bitset>
void print_bitset(std::ostream& os, const Bitset& bs)
{
  os << "{";
  bool first = true;
  for (std::size_t i = 0; i < bs.size(); ++i)
  {
    if (bs[i])
    {
      if (!first)
      {
        os << ", ";
      }
      os << i;
      first = false;
    }
  }
  os << "}";
}

inline
void print_graph(const cf_graph& G)
{
  const bool is_extra_graph = G.j == std::numeric_limits<std::size_t>::max();

  if (is_extra_graph)
  {
    std::cout << "Extra control-flow graph (summands without control-flow parameter)\n";
  }
  else
  {
    std::cout << "Control-flow graph for parameter index j = " << G.j << "\n";
  }

  std::cout << "Number of vertices: " << G.V.size() << "\n";
  std::cout << "Number of edges:    " << G.E.size() << "\n\n";

  for (std::size_t i = 0; i < G.V.size(); ++i)
  {
    const cf_vertex& v = G.V[i];

    std::cout << "Vertex " << i << " (value = " << v.value << ")\n";
    std::cout << "  enabled summands: " << v.enabled << "\n";
    std::cout << "  outgoing edges:\n";

    for (const cf_edge* e: v.edges)
    {
      const cf_vertex* tgt = e->target;
      std::size_t tgt_index = G.index.at(tgt->value);

      std::cout << "    -> Vertex "
                << tgt_index
                << " (value = "
                << tgt->value
                << ")";

      if (!is_extra_graph)
      {
        std::cout << "  mask = " << e->mask;
      }

      std::cout << "\n";
    }

    std::cout << "\n";
  }
}

inline
bool cfg_allows_summand(std::size_t summand,
                        const data::mutable_indexed_substitution<>& sigma,
                        const std::vector<data::variable>& process_parameters,
                        const std::vector<cf_graph>& cfgs)
{
  for (const cf_graph& G: cfgs)
  {
    if (!G.relevant.test(summand))
    {
      continue; // CFG not relevant for this summand
    }
    const auto value = sigma(process_parameters[G.j]);
    const std::size_t v_index = G.index.at(value);
    const cf_vertex& v = G.V[v_index];
    if (!v.enabled.test(summand))
    {
      return false;
    }
  }
  return true;
}

struct update_active_cfg_vertices_timer
{
  using clock = std::chrono::steady_clock;

  static inline std::chrono::nanoseconds total_time{0};

  clock::time_point start;

  update_active_cfg_vertices_timer()
    : start(clock::now())
  {}

  ~update_active_cfg_vertices_timer()
  {
    total_time += clock::now() - start;
  }

  // Printed automatically at program shutdown
  static void report()
  {
    const double seconds =
      std::chrono::duration<double>(total_time).count();

    std::cerr << "[timing] update_active_cfg_vertices total time: "
              << seconds << " s\n";
  }
};

// Ensures report() is called at program exit
struct update_active_cfg_vertices_timer_reporter
{
  ~update_active_cfg_vertices_timer_reporter()
  {
    update_active_cfg_vertices_timer::report();
  }
};

// One instance per program
inline update_active_cfg_vertices_timer_reporter update_active_cfg_vertices_timer_reporter_instance;

template <typename Container>
void update_active_cfg_vertices(const data::mutable_indexed_substitution<>& sigma,
                                const std::vector<data::variable>& process_parameters,
                                const std::vector<cf_graph>& cfgs,
                                Container& active_cfg_vertices)
{
  active_cfg_vertices.resize(cfgs.size());
  for (std::size_t k = 0; k < cfgs.size(); ++k)
  {
    const cf_graph& G = cfgs[k];
    const auto value = sigma(process_parameters[G.j]);
    active_cfg_vertices[k] = G.index.at(value);
  }
}

template <typename Container = boost::container::small_vector<std::size_t, 64>>
Container compute_active_cfg_vertices(const data::mutable_indexed_substitution<>& sigma,
                                      const std::vector<data::variable>& process_parameters,
                                      const std::vector<cf_graph>& cfgs)
{
  Container result;
  update_active_cfg_vertices(sigma, process_parameters, cfgs, result);
  return result;
}

template <typename Container>
bool cfg_allows_summand(std::size_t summand,
                        const std::vector<cf_graph>& cfgs,
                        const Container& active_cfg_vertices)
{
  assert(cfgs.size() == active_cfg_vertices.size());
  for (std::size_t k = 0; k < cfgs.size(); ++k)
  {
    const cf_graph& G = cfgs[k];
    if (!G.relevant.test(summand))
    {
      continue; // CFG not relevant for this summand
    }
    const cf_vertex& v = G.V[active_cfg_vertices[k]];
    if (!v.enabled.test(summand))
    {
      return false;
    }
  }
  return true;
}

template <typename State>
void report_cfg_error(std::size_t summand, const State& state, const std::vector<cf_graph>& cfgs)
{
  std::ostringstream out;
  out << "CFG violation:\n";
  out << "  summand = " << summand << "\n";
  out << "  state   = " << state << "\n";

  for (const cf_graph& G: cfgs)
  {
    const auto value = state[G.j];
    const std::size_t v_index = G.index.at(value);
    const cf_vertex& v = G.V[v_index];

    if (!v.enabled.test(summand))
    {
      out << "  rejected by CFG with parameter " << G.j << "\n";
      out << "  active vertex = " << v.j << "\n";
    }
  }

  throw std::runtime_error(out.str());
}

} // namespace mcrl2::lps

#endif // MCRL2_EXPLORER_CONTROL_FLOW_H
