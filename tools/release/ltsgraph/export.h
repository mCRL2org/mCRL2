// Author(s): Ferry Timmers
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_LTSGRAPH_EXPORT_H
#define MCRL2_LTSGRAPH_EXPORT_H

#include "graph.h"

#include <cstdlib>
#include <QFile>

/** @brief Graph file export utilities */
namespace Export
{

class Node;
class Edge;
class Exporter;

/** @brief base class for graph node and edge descriptors */
class Entity
{
  protected:
    const std::size_t m_id;
    const Graph::Graph& m_graph;

    constexpr Entity(const Graph::Graph& graph, std::size_t id)
      : m_graph(graph), m_id(id) {}
  public:
    /**
     * @brief A number that uniquely describes this entity
     * @note The Ids are not unique among different entity types.
     */
    constexpr const std::size_t& id() const
    {
      return m_id;
    }
};

/** @brief A descriptor class for graph nodes beging exported */
class Node : public Entity
{
  private:
    using Entity::Entity;
    friend class Edge;
    friend class Exporter;
  public:
    constexpr operator const Graph::NodeNode&() const
    {
      return m_graph.node(m_id);
    }
    constexpr operator const Graph::LabelNode&() const
    {
      return m_graph.stateLabel(m_id);
    }
    /** @brief Returns true when this node represents the initial state */
    constexpr bool initial() const
    {
      return m_graph.initialState() == m_id;
    }
    /** @brief Returns false when the node is not being explored in exploration mode */
    constexpr bool active() const
    {
      return !m_graph.hasExploration() || m_graph.node(m_id).active();
    }
    constexpr const QString& label() const
    {
      return m_graph.stateLabelstring(m_graph.stateLabel(m_id).labelindex());
    }
    constexpr const QVector3D& pos() const
    {
      return m_graph.node(m_id).pos();
    }
    constexpr const QVector3D& color() const
    {
      return m_graph.node(m_id).color();
    }
};

/** @brief A descriptor class for graph edges beging exported */
class Edge : public Entity
{
  private:
    const Node m_from;
    const Node m_to;

    constexpr Edge(const Graph::Graph& graph, std::size_t id)
      : Entity(graph, id),
      m_from(graph, graph.edge(id).from()),
      m_to(graph, graph.edge(id).to()) {}

    friend class Exporter;
  public:
    constexpr operator const Graph::LabelNode&() const
    {
      return m_graph.transitionLabel(m_id);
    }
    constexpr const Node& from() const
    {
      return m_from;
    }
    constexpr const Node& to() const
    {
      return m_to;
    }
    constexpr const Graph::Node& handle() const
    {
      return m_graph.handle(m_id);
    }
    /** @brief Returns true when the edge has the same source and destination node */
    constexpr bool selfLoop() const
    {
      return m_from.m_id == m_to.m_id;
    }
    constexpr const QString& label() const
    {
      return m_graph.transitionLabelstring(m_graph.transitionLabel(m_id).labelindex());
    }
};

/**
 * @brief Helper class for exporting graphs to a file.
 * This class provides access to the underlying graph, efficiently and thread-safe.
 * It abstract away details that are not relevant for exporting and makes sure
 * the graph remains locked until the export is ready.
 */
class Exporter
{
  protected:
    Graph::Graph::Guard m_guard;
    QFile m_file;
    bool m_valid;

    const bool m_exploring;
    const std::size_t m_nodeCount;
    const std::size_t m_edgeCount;

    constexpr std::size_t nodeId(std::size_t index) const
    {
      return m_exploring ? m_guard.graph.explorationNode(index) : index;
    }
    constexpr std::size_t edgeId(std::size_t index) const
    {
      return m_exploring ? m_guard.graph.explorationEdge(index) : index;
    }

  public:
    /**
     * @brief Construct a new graph export
     * @param graph The graph that should be exported
     * @param filename The file the export should be written to
     * During the lifetime of this object the graph remains locked, and the file remains open.
     */
    Exporter(Graph::Graph& graph, const QString& fileName)
      : m_guard(graph), m_file(fileName), m_valid(false),
      m_exploring(m_guard.graph.hasExploration()),
      m_nodeCount(m_exploring ? m_guard.graph.explorationNodeCount() : m_guard.graph.nodeCount()),
      m_edgeCount(m_exploring ? m_guard.graph.explorationEdgeCount() : m_guard.graph.edgeCount())
    {
      m_valid = m_file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    }

    /** @brief Writes a string to the output file */
    inline void operator +=(const QString& str)
    {
      if (m_valid && m_file.write(str.toLatin1()) == -1)
      {
        m_valid = false;
      }
    }
    /** @brief Returns true when operations on the output file were, so far, successful */
    constexpr operator bool() const { return m_valid; }

    /** @brief Returns the number of nodes in the exported graph */
    constexpr const std::size_t& nodeCount() const
    {
      return m_nodeCount;
    }
    /** @brief Returns the number of edges in the exported graph */
    constexpr const std::size_t& edgeCount() const
    {
      return m_edgeCount;
    }
    /**
     * @brief Returns a descriptor of the node at the specified index
     * @pre 0 <= index < nodeCount()
     */
    constexpr Node node(std::size_t index) const
    {
      return Node(m_guard.graph, nodeId(index));
    }
    /**
     * @brief Returns a descriptor of the edge at the specified index
     * @pre 0 <= index < edgeCount()
     */
    constexpr Edge edge(std::size_t index) const
    {
      return Edge(m_guard.graph, edgeId(index));
    }
};

}

#endif // MCRL2_LTSGRAPH_EXPORT_H
