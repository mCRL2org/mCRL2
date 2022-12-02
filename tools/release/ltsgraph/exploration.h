// Author(s): Ferry Timmers
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_LTSGRAPH_EXPLORATION_H
#define MCRL2_LTSGRAPH_EXPLORATION_H

#include <stack>
#include <unordered_map>
#include <unordered_set>

#include "graph.h"
#include "utility.h"

namespace Graph
{

/**
 * \brief Class that stores information about 'exploration mode' of ltsgraph
 * \details In exploration mode, only a part of the graph is shown. This part
 * should be reachable from the initial state.
 */
class Exploration
{
public:
  const std::vector<std::size_t>& nodes;
  const std::vector<std::size_t>& edges;

private:
  struct ExplorationNode
  {
    std::size_t id;                             // index in the complete node list
    std::size_t index;                          // index in the selected node list
    std::vector<std::size_t> inEdges, outEdges; // by edge id
    std::size_t count;                          ///< A counter for the number of references to this node.
    bool bridge;

    ExplorationNode() = default;
  };

  struct Edge
  {
    std::size_t id;    // index in the complete edge list
    std::size_t index; // index in the selected edge list
    std::size_t count;

    Edge() = default;
  };

  Graph& m_graph;

  // Maps indices of node and edge to exploration Node or Edge objects
  std::unordered_map<std::size_t, ExplorationNode> m_explorationNodes;
  std::unordered_map<std::size_t, Edge> m_edges;

  // Keeps track of indices for nodes and edges in exploration
  std::vector<std::size_t> m_nodeIndices;
  std::vector<std::size_t> m_edgeIndices;

  void repositionNode(const ExplorationNode& node)
  {
    // Center the first node placed
    if (m_nodeIndices.size() == 1)
    {
      m_graph.m_nodes[node.id].pos_mutable() = QVector3D(0.0, 0.0, 0.0);
      return;
    }

    QVector3D centroid;
    std::size_t count = 0;
    for (std::size_t inEdge : node.inEdges)
    {
      ::Graph::Edge& edge = m_graph.m_edges[inEdge];
      if (m_explorationNodes.count(edge.from()) != 0u)
      {
        centroid += m_graph.m_nodes[edge.from()].pos();
        ++count;
      }
    }
    for (std::size_t outEdge : node.outEdges)
    {
      ::Graph::Edge& edge = m_graph.m_edges[outEdge];
      if (m_explorationNodes.count(edge.to()) != 0u)
      {
        centroid += m_graph.m_nodes[edge.to()].pos();
        ++count;
      }
    }

    if (count != 0u)
    {
      QVector3D random_vec = QVector3D(frand(-1.0, 1.0), frand(-1.0, 1.0), frand(-1.0, 1.0));
      random_vec.normalize();
      random_vec *= 50.0f;

      // Take the average of the centroid vector.
      centroid = centroid / count;

      m_graph.m_nodes[node.id].pos_mutable() = centroid + random_vec;
      clipVector(m_graph.m_nodes[node.id].pos_mutable(), m_graph.getClipMin(), m_graph.getClipMax());
    }
  }

  void repositionEdge(std::size_t edgeId)
  {
    QVector3D pos1 = m_graph.m_nodes[m_graph.m_edges[edgeId].from()].pos();
    QVector3D pos2 = m_graph.m_nodes[m_graph.m_edges[edgeId].to()].pos();
    QVector3D center = (pos1 + pos2) / 2.0;
    m_graph.m_handles[edgeId].pos_mutable() = center;
    m_graph.m_transitionLabelnodes[edgeId].pos_mutable() = center;
  }

  // creates, and increases count for node
  ExplorationNode& increaseNode(std::size_t nodeId)
  {
    if (m_explorationNodes.count(nodeId) != 0u)
    {
      ExplorationNode& node = m_explorationNodes[nodeId];
      ++node.count;
      return node;
    }

    ExplorationNode& node = m_explorationNodes[nodeId];
    node.id = nodeId;
    node.index = m_nodeIndices.size();
    m_nodeIndices.push_back(nodeId);

    for (std::size_t i = 0; i < m_graph.m_edges.size(); ++i)
    {
      ::Graph::Edge& edge = m_graph.m_edges[i];
      if (edge.from() == nodeId)
      {
        node.outEdges.push_back(i);
      }
      if (edge.to() == nodeId)
      {
        node.inEdges.push_back(i);
      }
    }

    node.count = 1;

    repositionNode(node);
    return node;
  }

  // creates, and increases count for edge
  Edge& increaseEdge(std::size_t edgeId)
  {
    if (m_edges.count(edgeId) != 0u)
    {
      Edge& edge = m_edges[edgeId];
      ++edge.count;
      return edge;
    }

    Edge& edge = m_edges[edgeId];
    edge.id = edgeId;
    edge.index = m_edgeIndices.size();
    m_edgeIndices.push_back(edgeId);

    edge.count = 1;

    repositionEdge(edgeId);
    return edge;
  }

  // decreases exploration count for node, and purges
  void decreaseNode(std::size_t nodeId)
  {
    if (m_explorationNodes.count(nodeId) == 0u)
    {
      return;
    }

    ExplorationNode& node = m_explorationNodes[nodeId];
    if (--node.count < 1)
    {
      std::size_t last = m_nodeIndices.size() - 1;
      if (node.index < last)
      {
        std::size_t lastId = m_nodeIndices[last];
        m_nodeIndices[node.index] = lastId;
        m_explorationNodes[lastId].index = node.index;
      }
      m_nodeIndices.pop_back();
      m_explorationNodes.erase(nodeId);
    }
  }

  // decreases exploration count for edge, and purges
  void decreaseEdge(std::size_t edgeId)
  {
    if (m_edges.count(edgeId) == 0u)
    {
      return;
    }

    Edge& edge = m_edges[edgeId];
    if (--edge.count < 1)
    {
      std::size_t last = m_edgeIndices.size() - 1;
      if (edge.index < last)
      {
        std::size_t lastId = m_edgeIndices[last];
        m_edgeIndices[edge.index] = lastId;
        m_edges[lastId].index = edge.index;
      }
      m_edgeIndices.pop_back();
      m_edges.erase(edgeId);
    }
  }

  // updates the bridge values of the edges
  // algorithm based on http://dx.doi.org/10.1016/j.ipl.2013.01.016
  void updateBridges()
  {
    if (m_nodeIndices.empty())
    {
      return;
    }

    struct NodeInfo
    {
      std::size_t id;
      std::unordered_set<std::size_t> neighbors;

      std::size_t parent;
      std::unordered_set<std::size_t> backEdges;

      bool visited  = false;
      bool leaf     = false;
      bool searched = false;

      NodeInfo(std::size_t id = 0)
          : id(id),  parent(id)
      {
      }
    };
    struct EdgeInfo
    {
      std::size_t u, v;
      EdgeInfo(std::size_t f, std::size_t t) : u(f < t ? f : t), v(f < t ? t : f)
      {
      }
      bool operator==(const EdgeInfo& e) const
      {
        return e.u == u && e.v == v;
      }
    };
    struct EdgeInfoHash
    {
      std::size_t operator()(const EdgeInfo& e) const
      {
        return std::hash<std::size_t>()(e.u) ^ (std::hash<std::size_t>()(e.v) << 1);
      }
    };
    
    std::unordered_map<std::size_t, NodeInfo> _nodes;
    std::vector<std::size_t> order;
    std::unordered_set<EdgeInfo, EdgeInfoHash> chains;

    // generate simplified graph
    {
      std::stack<std::size_t> progress;
      progress.push(m_nodeIndices[0]);
      while (!progress.empty())
      {
        std::size_t nodeId = progress.top();
        progress.pop();
        if (_nodes.count(nodeId) != 0u)
        {
          continue;
        }

        _nodes[nodeId] = NodeInfo(nodeId);
        NodeInfo& nodeinfo = _nodes[nodeId];
        ExplorationNode& node = m_explorationNodes[nodeId];
        for (std::size_t outEdge : node.outEdges)
        {
          if (m_edges.count(outEdge) == 0u)
          {
            continue;
          }
          std::size_t otherId = m_graph.m_edges[outEdge].to();
          if (nodeId == otherId || (m_explorationNodes.count(otherId) == 0u))
          {
            continue;
          }
          nodeinfo.neighbors.insert(otherId);
          progress.push(otherId);
        }
        for (std::size_t inEdge : node.inEdges)
        {
          if (m_edges.count(inEdge) == 0u)
          {
            continue;
          }
          std::size_t otherId = m_graph.m_edges[inEdge].from();
          if (nodeId == otherId || (m_explorationNodes.count(otherId) == 0u))
          {
            continue;
          }
          nodeinfo.neighbors.insert(otherId);
          progress.push(otherId);
        }
      }
    }

    // generate depth first search tree, find backedges
    {
      std::stack<std::size_t> progress;
      progress.push(m_nodeIndices[0]);
      while (!progress.empty())
      {
        std::size_t nodeId = progress.top();
        progress.pop();
        NodeInfo& node = _nodes[nodeId];

        if (node.searched)
        {
          continue;
        }
        node.searched = true;
        order.push_back(nodeId);

        for (std::size_t otherId : node.neighbors)
        {
          if (!_nodes[otherId].searched) // node was not yet processed:
          {
            _nodes[otherId].parent = nodeId;
            progress.push(otherId);
          }
          else if (node.parent != otherId) // node was processed, backedge
          {
            _nodes[otherId].backEdges.insert(nodeId);
          }
        }
      }
    }

    // find chains
    {
      for (std::size_t nodeId : order)
      {
        NodeInfo& node = _nodes[nodeId];
        for (std::size_t id : node.backEdges)
        {
          node.visited = true;
          chains.insert(EdgeInfo(nodeId, id));
          while (!_nodes[id].visited)
          {
            _nodes[id].visited = true;
            chains.insert(EdgeInfo(id, _nodes[id].parent));
            id = _nodes[id].parent;
          }
        }
      }
    }

    // find leafs
    {
      for (std::size_t nodeId : order)
      {
        ExplorationNode& node = m_explorationNodes[nodeId];
        NodeInfo& nodeinfo = _nodes[nodeId];
        if (m_graph.m_nodes[nodeId].active())
        {
          nodeinfo.leaf = false;
          continue;
        }
        std::size_t count = 0;
        for (std::size_t inEdge : node.inEdges)
        {
          count += m_edges.count(inEdge) != 0u ? 1 : 0;
        }
        for (std::size_t outEdge : node.outEdges)
        {
          count += m_edges.count(outEdge) != 0u ? 1 : 0;
        }
        nodeinfo.leaf = (count <= 1);
      }
    }

    // find bridges
    {
      for (std::size_t nodeId : order)
      {
        ExplorationNode& node = m_explorationNodes[nodeId];
        NodeInfo& nodeinfo = _nodes[nodeId];
        node.bridge = false;

        std::unordered_set<std::size_t> connected;
        std::size_t connections = 0;
        for (std::size_t inEdge : node.inEdges)
        {
          if (m_edges.count(inEdge) != 0u)
          {
            connected.insert(m_graph.m_edges[inEdge].from());
          }
        }
        for (std::size_t outEdge : node.outEdges)
        {
          if ((m_edges.count(outEdge) != 0u) && m_edges[outEdge].count > 1)
          {
            connected.insert(m_graph.m_edges[outEdge].to());
          }
        }

        for (std::size_t otherId : nodeinfo.neighbors)
        {
          bool isLeaf = _nodes[otherId].leaf;
          bool isConnected = connected.count(otherId) != 0u;
          bool isChain = chains.count(EdgeInfo(nodeId, otherId)) != 0u;
          if (!isLeaf)
          {
            ++connections;
          }
          if (!isLeaf && !isConnected && !isChain)
          {
            node.bridge = true;
          }
        }
        if (connections <= 1)
        {
          node.bridge = false;
        }
      }
    }
  }

  // returns whether contracting this node will not leave orphans
  // standard depth-first search algorithm
  bool contractable(std::size_t nodeId)
  {
    ExplorationNode& node = m_explorationNodes[nodeId];
    std::unordered_set<std::size_t> nedges; // removed edges
    std::unordered_set<std::size_t> neighbors;
    for (std::size_t edgeId : node.inEdges)
    {
      if (m_edges.count(edgeId) != 0u)
      {
        std::size_t otherId = m_graph.m_edges[edgeId].from();
        if (m_explorationNodes.count(otherId) != 0u)
        {
          neighbors.insert(otherId);
        }
      }
    }
    for (std::size_t edgeId : node.outEdges)
    {
      if (m_edges.count(edgeId) != 0u)
      {
        if (m_edges[edgeId].count <= 1)
        {
          nedges.insert(edgeId);
        }
        std::size_t otherId = m_graph.m_edges[edgeId].to();
        if ((m_explorationNodes.count(otherId) != 0u) && m_explorationNodes[otherId].count > 1)
        {
          neighbors.insert(otherId);
        }
      }
    }
    if (neighbors.size() <= 1)
    {
      return true;
    }

    std::stack<std::size_t> progress;
    std::unordered_set<std::size_t> status;
    progress.push(*neighbors.begin());
    while (!progress.empty())
    {
      std::size_t _nodeId = progress.top();
      progress.pop();
      if (status.count(_nodeId) != 0u)
      {
        continue;
      }
      status.insert(_nodeId);

      neighbors.erase(_nodeId);
      if (neighbors.empty())
      {
        return true;
      }

      ExplorationNode& _node = m_explorationNodes[_nodeId];
      for (std::size_t edgeId : _node.inEdges)
      {
        if ((m_edges.count(edgeId) != 0u) && (nedges.count(edgeId) == 0u))
        {
          progress.push(m_graph.m_edges[edgeId].from());
        }
      }
      for (std::size_t edgeId : _node.outEdges)
      {
        if ((m_edges.count(edgeId) != 0u) && (nedges.count(edgeId) == 0u))
        {
          progress.push(m_graph.m_edges[edgeId].to());
        }
      }
    }
    return false;
  }

  public:
  Exploration(Graph& graph)
      : nodes(m_nodeIndices), edges(m_edgeIndices), m_graph(graph)
  {
  }

  // expand outgoing transitions and states for specified node
  void expand(std::size_t nodeId)
  {
    ExplorationNode& node = increaseNode(nodeId);
    for (std::size_t edgeId : node.outEdges)
    {
      increaseNode(m_graph.m_edges[edgeId].to());
      increaseEdge(edgeId);
    }
    updateBridges();
  }

  // contract outgoing transitions and states for specified node
  void contract(std::size_t nodeId)
  {
    if (m_explorationNodes.count(nodeId) != 0)
    {
      ExplorationNode& node = m_explorationNodes[nodeId];
      for (std::size_t edgeId : node.outEdges)
      {
        decreaseEdge(edgeId);
        decreaseNode(m_graph.m_edges[edgeId].to());
      }
    }
    decreaseNode(nodeId);
    updateBridges();
  }

  // return true when contracting given node would leave unconnected parts
  bool isContractable(std::size_t nodeId)
  {
    if (m_explorationNodes.count(nodeId) == 0u)
    {
      return false;
    }
    bool value = contractable(nodeId);
    if (!value)
    {
      m_explorationNodes[nodeId].bridge = true;
    }
    return value;
  }

  // tells whether a node is part of a bridge (is a cut edge)
  // slightly more lenient as isContractable but fast (false positives)
  bool isBridge(std::size_t nodeId)
  {
    if (m_explorationNodes.count(nodeId) == 0u)
    {
      return false;
    }
    return m_explorationNodes[nodeId].bridge;
  }
};

} // namespace Graph

#endif // MCRL2_LTSGRAPH_EXPLORATION_H
