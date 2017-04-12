// Author(s): Rimco Boudewijns and Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <unordered_map>
#include <unordered_set>

#include <QDomDocument>
#include <QTextStream>
#include <QFile>
#include <QtOpenGL>

#include "graph.h"
#include "mcrl2/lts/probabilistic_lts.h"
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/lts/lts_aut.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/lts/lts_fsm.h"
#include "mcrl2/lts/state_label_empty.h"
#include "mcrl2/lts/action_label_string.h"

namespace Graph
{
    inline float frand(float min, float max)
    {
      return ((float)qrand() / RAND_MAX) * (max - min) + min;
    }

    QString stateLabelToQString(const mcrl2::lts::state_label_empty& label)
    {
        return QString("");
    }
    QString stateLabelToQString(const mcrl2::lts::state_label_lts& label)
    {
        return QString::fromStdString(mcrl2::lts::pp(label));
    }
    QString stateLabelToQString(const mcrl2::lts::state_label_fsm& label)
    {
        return QString::fromStdString(mcrl2::lts::pp(label));
    }
    QString transitionLabelToQString(const mcrl2::lts::action_label_lts& label)
    {
        return QString::fromStdString(mcrl2::lts::pp(label));
    }
    QString transitionLabelToQString(const mcrl2::lts::action_label_string& label)
    {
        return QString::fromStdString(label);
    }

  class Selection
  {
    public:
      const std::vector<size_t>& nodes;
      const std::vector<size_t>& edges;
    private:
      struct Node
      {
        size_t id;    // index in the complete node list
        size_t index; // index in the selected node list
        std::vector<size_t> inEdges, outEdges; // by edge id
        size_t count;
        bool bridge;
        Node()=default;
      };
      struct Edge
      {
        size_t id;    // index in the complete edge list
        size_t index; // index in the selected edge list
        size_t count;
        Edge()=default;
      };

      Graph &m_graph;

      // maps node/edge indices to selection Node/Edge objects
      std::unordered_map<size_t,Node> m_nodes;
      std::unordered_map<size_t,Edge> m_edges;
      // keeps track of node/edge indices in selection
      std::vector<size_t> m_nodeIndices;
      std::vector<size_t> m_edgeIndices;

      void repositionNode(const Node& node)
      {
        // Center the first node placed
        if (m_nodeIndices.size() == 1)
        {
          m_graph.m_nodes[node.id].pos() = Coord3D(0.0, 0.0, 0.0);
          return;
        }

        Coord3D centroid;
        size_t count = 0;
        for (size_t i = 0; i < node.inEdges.size(); ++i)
        {
          ::Graph::Edge& edge = m_graph.m_edges[node.inEdges[i]];
          if (m_nodes.count(edge.from()))
          {
            centroid += m_graph.m_nodes[edge.from()].pos();
            ++count;
          }
        }
        for (size_t i = 0; i < node.outEdges.size(); ++i)
        {
          ::Graph::Edge& edge = m_graph.m_edges[node.outEdges[i]];
          if (m_nodes.count(edge.to()))
          {
            centroid += m_graph.m_nodes[edge.to()].pos();
            ++count;
          }
        }

        if (count)
        {
          Coord3D rvec = Coord3D(frand(-1.0, 1.0),
            frand(-1.0, 1.0), frand(-1.0, 1.0));
          rvec *= 50.0 / rvec.size();
          m_graph.m_nodes[node.id].pos() = centroid / ((GLfloat) count) + rvec;
        }
      }

      void repositionEdge(size_t edgeId)
      {
        Coord3D pos1 = m_graph.m_nodes[m_graph.m_edges[edgeId].from()].pos();
        Coord3D pos2 = m_graph.m_nodes[m_graph.m_edges[edgeId].to()].pos();
        Coord3D center = (pos1 + pos2) / 2.0;
        m_graph.m_handles[edgeId].pos() = center;
        m_graph.m_transitionLabelnodes[edgeId].pos() = center;
      }

      // creates, and increases count for node
      Node& increaseNode(size_t nodeId)
      {
        if (m_nodes.count(nodeId))
        {
          Node& node = m_nodes[nodeId];
          ++node.count;
          return node;
        }

        Node& node = m_nodes[nodeId];
        node.id = nodeId;
        node.index = m_nodeIndices.size();
        m_nodeIndices.push_back(nodeId);

        for (size_t i = 0; i < m_graph.m_edges.size(); ++i)
        {
          ::Graph::Edge& edge = m_graph.m_edges[i];
          if (edge.from() == nodeId)
            node.outEdges.push_back(i);
          if (edge.to() == nodeId)
            node.inEdges.push_back(i);
        }

        node.count = 1;

        repositionNode(node);
        return node;
      }

      // creates, and increases count for edge
      Edge& increaseEdge(size_t edgeId)
      {
        if (m_edges.count(edgeId))
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

      // decreases selection count for node, and purges
      void decreaseNode(size_t nodeId)
      {
        if (!m_nodes.count(nodeId))
          return;

        Node& node = m_nodes[nodeId];
        if (--node.count < 1)
        {
          size_t last = m_nodeIndices.size() - 1;
          if (node.index < last)
          {
            size_t lastId = m_nodeIndices[last];
            m_nodeIndices[node.index] = lastId;
            m_nodes[lastId].index = node.index;
          }
          m_nodeIndices.pop_back();
          m_nodes.erase(nodeId);
        }
      }

      // decreases selection count for edge, and purges
      void decreaseEdge(size_t edgeId)
      {
        if (!m_edges.count(edgeId))
          return;

        Edge& edge = m_edges[edgeId];
        if (--edge.count < 1)
        {
          size_t last = m_edgeIndices.size() - 1;
          if (edge.index < last)
          {
            size_t lastId = m_edgeIndices[last];
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
        if (m_nodeIndices.size() < 1) return;

        struct NodeInfo
        {
          size_t id;
          std::unordered_set<size_t> neighbors;
          bool searched;
          size_t parent;
          std::unordered_set<size_t> backEdges;
          bool visited;
          bool leaf;
          NodeInfo(size_t id = 0)
            : id(id), searched(false), parent(id), visited(false), leaf(false) {}
        };
        struct EdgeInfo
        {
          size_t u, v;
          EdgeInfo(size_t f, size_t t) : u(f < t ? f : t), v(f < t ? t : f) {}
          bool operator ==(const EdgeInfo& e) const { return e.u == u && e.v == v; }
        };
        struct EdgeInfoHash
        {
          size_t operator()(const EdgeInfo& e) const
          {
            return std::hash<size_t>()(e.u) ^ (std::hash<size_t>()(e.v) << 1);
          };
        };
        std::unordered_map<size_t,NodeInfo> nodes;
        std::vector<size_t> order;
        std::unordered_set<EdgeInfo,EdgeInfoHash> chains;

        // generate simplified graph
        {
          std::stack<size_t> progress;
          progress.push(m_nodeIndices[0]);
          while (!progress.empty())
          {
            size_t nodeId = progress.top();
            progress.pop();
            if (nodes.count(nodeId)) continue;

            nodes[nodeId] = NodeInfo(nodeId);
            NodeInfo& nodeinfo = nodes[nodeId];
            Node& node = m_nodes[nodeId];
            for (size_t i = 0; i < node.outEdges.size(); ++i)
            {
              size_t edgeId = node.outEdges[i];
              if (!m_edges.count(edgeId)) continue;
              size_t otherId = m_graph.m_edges[edgeId].to();
              if (nodeId == otherId || !m_nodes.count(otherId)) continue;
              nodeinfo.neighbors.insert(otherId);
              progress.push(otherId);
            }
            for (size_t i = 0; i < node.inEdges.size(); ++i)
            {
              size_t edgeId = node.inEdges[i];
              if (!m_edges.count(edgeId)) continue;
              size_t otherId = m_graph.m_edges[edgeId].from();
              if (nodeId == otherId || !m_nodes.count(otherId)) continue;
              nodeinfo.neighbors.insert(otherId);
              progress.push(otherId);
            }
          }
        }

        // generate depth first search tree, find backedges
        {
          std::stack<size_t> progress;
          progress.push(m_nodeIndices[0]);
          while (!progress.empty())
          {
            size_t nodeId = progress.top();
            progress.pop();
            NodeInfo& node = nodes[nodeId];

            if (node.searched) continue;
            node.searched = true;
            order.push_back(nodeId);

            for (std::unordered_set<size_t>::iterator
              it = node.neighbors.begin(); it != node.neighbors.end(); ++it)
            {
              size_t otherId = *it;
              if (!nodes[otherId].searched) // node was not yet processed:
              {
                nodes[otherId].parent = nodeId;
                progress.push(otherId);
              }
              else if (node.parent != otherId) // node was processed, backedge
              {
                nodes[otherId].backEdges.insert(nodeId);
              }
            }
          }
        }

        // find chains
        {
          for (size_t i = 0; i < order.size(); ++i)
          {
            size_t nodeId = order[i];
            NodeInfo& node = nodes[nodeId];
            for (std::unordered_set<size_t>::iterator
              it = node.backEdges.begin(); it != node.backEdges.end(); ++it)
            {
              size_t id = *it;
              node.visited = true;
              chains.insert(EdgeInfo(nodeId, id));
              while (!nodes[id].visited)
              {
                nodes[id].visited = true;
                chains.insert(EdgeInfo(id, nodes[id].parent));
                id = nodes[id].parent;
              }
            }
          }
        }

        // find leafs
        {
          for (size_t i = 0; i < order.size(); ++i)
          {
            size_t nodeId = order[i];
            Node& node = m_nodes[nodeId];
            NodeInfo& nodeinfo = nodes[nodeId];
            if (m_graph.m_nodes[nodeId].active())
            {
              nodeinfo.leaf = false;
              continue;
            }
            size_t count = 0;
            for (size_t j = 0; j < node.inEdges.size(); ++j)
              count += m_edges.count(node.inEdges[j]) ? 1 : 0;
            for (size_t j = 0; j < node.outEdges.size(); ++j)
              count += m_edges.count(node.outEdges[j]) ? 1 : 0;
            nodeinfo.leaf = (count <= 1);
          }
        }

        // find bridges
        {
          for (size_t i = 0; i < order.size(); ++i)
          {
            size_t nodeId = order[i];
            Node& node = m_nodes[nodeId];
            NodeInfo& nodeinfo = nodes[nodeId];
            node.bridge = false;

            std::unordered_set<size_t> connected;
            size_t connections = 0;
            for (size_t j = 0; j < node.inEdges.size(); ++j)
            {
              size_t edgeId = node.inEdges[j];
              if (m_edges.count(edgeId))
                connected.insert( m_graph.m_edges[edgeId].from());
            }
            for (size_t j = 0; j < node.outEdges.size(); ++j)
            {
              size_t edgeId = node.outEdges[j];
              if (m_edges.count(edgeId) && m_edges[edgeId].count > 1)
                connected.insert(m_graph.m_edges[edgeId].to());
            }
            
            for (std::unordered_set<size_t>::iterator
              it = nodeinfo.neighbors.begin(); it != nodeinfo.neighbors.end(); ++it)
            {
              size_t otherId = *it;
              bool isLeaf = nodes[otherId].leaf;
              bool isConnected = connected.count(otherId);
              bool isChain = chains.count(EdgeInfo(nodeId,otherId));
              if (!isLeaf)
                ++connections;
              if (!isLeaf && !isConnected && !isChain)
                node.bridge = true;
            }
            if (connections <= 1)
              node.bridge = false;
          }
        }

        // debug tree
        /*{
          FILE* fp = fopen("selection.dot", "wt");
          fputs("digraph {\n\tnode [shape=circle];\n", fp);
          for (size_t i = 0; i < order.size(); ++i)
          {
            NodeInfo& node = nodes[order[i]];
            const char *color = chains.count(EdgeInfo(node.id, node.parent)) ? "red" : "black";
            fprintf(fp, "\t\"%d\"->\"%d\" [color=%s];\n", node.id, node.parent, color);
            for (std::unordered_set<size_t>::iterator
              it = node.backEdges.begin(); it != node.backEdges.end(); ++it)
            {
              const char *color = chains.count(EdgeInfo(node.id, *it)) ? "red" : "black";
              fprintf(fp, "\t\"%d\"->\"%d\" [style=dashed,constraint=false,color=%s];\n", node.id, *it, color);
            }
            if (node.leaf)
              fprintf(fp, "\t\"%d\" [style=dashed]\n", node.id);
          }
          fputs("}\n", fp);
          fclose(fp);
        }*/
      }

      // returns whether contracting this node will not leave orphans
      // standard depth-first search algorithm
      bool contractable(size_t nodeId)
      {
        Node& node = m_nodes[nodeId];
        std::unordered_set<size_t> nedges; // removed edges
        std::unordered_set<size_t> neighbors;
        for (size_t i = 0; i < node.inEdges.size(); ++i)
        {
          size_t edgeId = node.inEdges[i];
          if (m_edges.count(edgeId))
          {
            size_t otherId = m_graph.m_edges[edgeId].from();
            if (m_nodes.count(otherId))
              neighbors.insert(otherId);
          }
        }
        for (size_t i = 0; i < node.outEdges.size(); ++i)
        {
          size_t edgeId = node.outEdges[i];
          if (m_edges.count(edgeId))
          {
            if (m_edges[edgeId].count <= 1)
              nedges.insert(edgeId);
            size_t otherId = m_graph.m_edges[edgeId].to();
            if (m_nodes.count(otherId) && m_nodes[otherId].count > 1)
              neighbors.insert(otherId);
          }
        }
        if (neighbors.size() <= 1)
          return true;

        std::stack<size_t> progress;
        std::unordered_set<size_t> status;
        progress.push(*neighbors.begin());
        while (!progress.empty())
        {
          size_t nodeId = progress.top();
          progress.pop();
          if (status.count(nodeId)) continue;
          status.insert(nodeId);

          neighbors.erase(nodeId);
          if (!neighbors.size())
            return true;

          Node& node = m_nodes[nodeId];
          for (size_t i = 0; i < node.inEdges.size(); ++i)
          {
            size_t edgeId = node.inEdges[i];
            if (m_edges.count(edgeId) && !nedges.count(edgeId))
              progress.push(m_graph.m_edges[edgeId].from());
          }
          for (size_t i = 0; i < node.outEdges.size(); ++i)
          {
            size_t edgeId = node.outEdges[i];
            if (m_edges.count(edgeId) && !nedges.count(edgeId))
              progress.push(m_graph.m_edges[edgeId].to());
          }
        }
        return false;
      }

    public:
      Selection(Graph& graph)
        : nodes(m_nodeIndices), edges(m_edgeIndices), m_graph(graph) {}

      // expand outgoing transitions and states for specified node
      void expand(size_t nodeId)
      {
        Node& node = increaseNode(nodeId);
        for (size_t i = 0; i < node.outEdges.size(); ++i)
        {
          size_t edgeId = node.outEdges[i];
          increaseNode(m_graph.m_edges[edgeId].to());
          increaseEdge(edgeId);
        }
        updateBridges();
      }

      // contract outgoing transitions and states for specified node
      void contract(size_t nodeId)
      {
        if (m_nodes.count(nodeId) != 0)
        {
          Node& node = m_nodes[nodeId];
          for (size_t i = 0; i < node.outEdges.size(); ++i)
          {
            size_t edgeId = node.outEdges[i];
            decreaseEdge(edgeId);
            decreaseNode(m_graph.m_edges[edgeId].to());
          }
        }
        decreaseNode(nodeId);
        updateBridges();
      }

      // return true when contracting given node would leave unconnected parts
      bool isContractable(size_t nodeId)
      {
        if (!m_nodes.count(nodeId))
          return false;
        bool value = contractable(nodeId);
        if (!value)
          m_nodes[nodeId].bridge = true;
        return value;
      }

      // tells whether a node is part of a bridge (is a cut edge)
      // slightly more lenient as isContractable but fast (false positives)
      bool isBridge(size_t nodeId)
      {
        if (!m_nodes.count(nodeId))
          return false;
        return m_nodes[nodeId].bridge;
      }
  };

  #ifndef DEBUG_GRAPH_LOCKS
    #define GRAPH_LOCK(type, where, x) x
  #else
    #define GRAPH_LOCK(type, where, x) do { debug_lock(type, where); x; \
                                            debug_lock(type"ed", where); } while (false)
    void debug_lock(const char* type, const char* func)
    {
      static int count = 0;
      static std::unordered_map<Qt::HANDLE,int> ids;
      Qt::HANDLE id = QThread::currentThreadId();
      if (!ids.count(id))
        ids[id] = ++count;
      printf("[%d] %s at %s\n", ids[id], type, func);
      fflush(stdout);
    }
  #endif

  #define lockForRead(lock, where)    GRAPH_LOCK("lock", where, lock.lockForRead())
  #define unlockForRead(lock, where)  GRAPH_LOCK("unlock", where, lock.unlock())
  #define lockForWrite(lock, where)   GRAPH_LOCK("W lock", where, lock.lockForWrite())
  #define unlockForWrite(lock, where) GRAPH_LOCK("W unlock", where, lock.unlock())

  Graph::Graph() : m_sel(nullptr), m_lock(), m_stable(true)
  {
    m_type = mcrl2::lts::lts_lts;
    m_empty = QString("");
  }

  Graph::~Graph()
  {
    if (m_sel != nullptr)
    {
      delete m_sel;
      m_sel = nullptr;
    }
  }

  size_t Graph::edgeCount() const
  {
    return m_edges.size();
  }

  size_t Graph::nodeCount() const
  {
    return m_nodes.size();
  }

  size_t Graph::transitionLabelCount() const
  {
    return m_transitionLabels.size();
  }

  size_t Graph::stateLabelCount() const
  {
    return m_stateLabels.size();
  }

  size_t Graph::initialState() const
  {
    return m_initialState;
  }

  bool Graph::isTau(size_t labelindex) const
  {
    return m_transitionLabels[labelindex].is_tau();
  }

  void Graph::load(const QString& filename, const Coord3D& min, const Coord3D& max)
  {
    lockForWrite(m_lock, GRAPH_LOCK_TRACE);

    m_type = mcrl2::lts::detail::guess_format(filename.toUtf8().constData());
    try
    {
        switch (m_type)
        {
            case mcrl2::lts::lts_aut:
                templatedLoad<mcrl2::lts::probabilistic_lts_aut_t>(filename, min, max);
                break;
            case mcrl2::lts::lts_dot:
                throw mcrl2::runtime_error("Cannot read a .dot file anymore.");
                break;
            case mcrl2::lts::lts_fsm:
                templatedLoad<mcrl2::lts::probabilistic_lts_fsm_t>(filename, min, max);
                break;
            case mcrl2::lts::lts_lts:
            default:
                m_type = mcrl2::lts::lts_lts;
                templatedLoad<mcrl2::lts::probabilistic_lts_lts_t>(filename, min, max);
                break;
        }
    }
    catch(mcrl2::runtime_error e)
    {
        unlockForWrite(m_lock, GRAPH_LOCK_TRACE);
        throw e;
    }
    if (m_sel != nullptr)
    {
      delete m_sel;
      m_sel = nullptr;
    }
    m_stable = true;

    unlockForWrite(m_lock, GRAPH_LOCK_TRACE);
  }

  template <class ltsclass>
  void Graph::templatedLoad(const QString& filename,
          const Coord3D& min, const Coord3D& max)
  {
      ltsclass lts;
      lts.load(filename.toUtf8().constData());

      // Reserve all auxiliary data vectors
      m_nodes.reserve(lts.num_states());
      m_edges.reserve(lts.num_transitions());
      m_handles.reserve(lts.num_transitions());

      m_transitionLabels.reserve(lts.num_action_labels());
      m_transitionLabelnodes.reserve(lts.num_transitions());

      m_stateLabels.reserve(lts.num_state_labels());
      m_stateLabelnodes.reserve(lts.num_states());

      for (size_t i = 0; i < lts.num_state_labels(); ++i)
      {
          m_stateLabels.push_back(stateLabelToQString(lts.state_label(i)));
      }

      // Position nodes randomly
      for (size_t i = 0; i < lts.num_states(); ++i)
      {
          const bool is_not_probabilistic=false;
          m_nodes.push_back(NodeNode(Coord3D(frand(min.x, max.x), frand(min.y, max.y), frand(min.z, max.z)),is_not_probabilistic));
          m_stateLabelnodes.push_back(LabelNode(m_nodes[i].pos(),i));
      }

      // Store string representations of labels
      for (size_t i = 0; i < lts.num_action_labels(); ++i)
      {
          /* auto label = lts.action_label(i); */
          /* QString str = QString::fromStdString(label); */
          // FIXME
          m_transitionLabels.push_back(LabelString(lts.is_tau(i), transitionLabelToQString(lts.action_label(i))));
      }

      // Assign and position edge handles, position edge labels
      for (size_t i = 0; i < lts.num_transitions(); ++i)
      {
          mcrl2::lts::transition& t = lts.get_transitions()[i];
          size_t new_probabilistic_state = add_probabilistic_state<ltsclass>(lts.probabilistic_state(t.to()),min,max);
          m_edges.push_back(Edge(t.from(),new_probabilistic_state));
          m_handles.push_back(Node((m_nodes[t.from()].pos() + m_nodes[new_probabilistic_state].pos()) / 2.0));
          m_transitionLabelnodes.push_back(LabelNode((m_nodes[t.from()].pos() + m_nodes[new_probabilistic_state].pos()) / 2.0,t.label()));
      }

      m_initialState = add_probabilistic_state<ltsclass>(lts.initial_probabilistic_state(), min, max);
  }

  template <class ltsclass>
  size_t Graph::add_probabilistic_state(const typename ltsclass::probabilistic_state_t& probabilistic_state,
          const Coord3D& min, const Coord3D& max)
  {
      if (probabilistic_state.size()==1)
      {
          return probabilistic_state.begin()->state();
      }
      else
      {
          // There are multiple probabilistic states. Make a new state
          // with outgoing probabilistic transitions to all states. 
          size_t index_of_the_new_probabilistic_state=m_nodes.size();
          const bool is_probabilistic=true;
          m_nodes.push_back(NodeNode(Coord3D(frand(min.x, max.x), frand(min.y, max.y), frand(min.z, max.z)),is_probabilistic));
          m_stateLabelnodes.push_back(LabelNode(m_nodes[index_of_the_new_probabilistic_state].pos(),index_of_the_new_probabilistic_state));

          // The following map recalls where probabilities are stored in transitionLabels.
          typedef std::map<typename ltsclass::probabilistic_state_t::probability_t, size_t> probability_map_t;
          probability_map_t probability_label_indices;
          for(const typename ltsclass::probabilistic_state_t::state_probability_pair& p: probabilistic_state)
          {
              // Find an index for the probabilistic label of the outgoing transition of the probabilistic state.
              size_t label_index;
              const typename probability_map_t::const_iterator i=probability_label_indices.find(p.probability());
              if (i==probability_label_indices.end()) // not found
              {
                  label_index=m_transitionLabels.size();
                  probability_label_indices[p.probability()]=label_index;
                  m_transitionLabels.push_back(LabelString(false,QString::fromStdString(pp(p.probability()))));
              }
              else
              {
                  label_index=i->second;
              }

              m_edges.push_back(Edge(index_of_the_new_probabilistic_state,p.state()));
              m_handles.push_back(Node((m_nodes[index_of_the_new_probabilistic_state].pos() + m_nodes[p.state()].pos()) / 2.0));
              m_transitionLabelnodes.push_back(LabelNode((m_nodes[index_of_the_new_probabilistic_state].pos() + m_nodes[p.state()].pos()) / 2.0,label_index));
          } 
          return index_of_the_new_probabilistic_state;
      }
  }

  void Graph::loadXML(const QString& filename)
  {
    lockForWrite(m_lock, GRAPH_LOCK_TRACE);

    QDomDocument xml;
    QFile file(filename);
    if(!file.open( QFile::ReadOnly ))
    {
      mCRL2log(mcrl2::log::error) << "Could not open XML file: " << filename.toStdString() << std::endl;
      return;
    }
    QString errorMsg;
    if(!xml.setContent(&file, false, &errorMsg))
    {
      file.close();
      mCRL2log(mcrl2::log::error) << "Could not parse XML file: " << errorMsg.toStdString() << std::endl;
      return;
    }
    file.close();

    QDomElement root = xml.documentElement();
    if(root.tagName() != "Graph")
    {
      mCRL2log(mcrl2::log::error) << "XML contains no valid graph" << std::endl;
      return;
    }

    m_type = (mcrl2::lts::lts_type) root.attribute("type").toInt();

    m_nodes.resize(root.attribute("states").toInt());
    m_edges.resize(root.attribute("transitions").toInt());
    m_handles.resize(root.attribute("transitions").toInt());

    m_transitionLabels.resize(root.attribute("transitionlabels").toInt());
    m_transitionLabelnodes.resize(root.attribute("transitions").toInt());

    m_stateLabels.resize(root.attribute("statelabels").toInt());
    m_stateLabelnodes.resize(root.attribute("states").toInt());

    QDomNode node = root.firstChild();
    while (!node.isNull()) 
    {
      QDomElement e = node.toElement();

      if (e.tagName() == "StateLabel") {
        m_stateLabels[e.attribute("value").toInt()] = e.attribute("label");
      }
      if (e.tagName() == "State") 
      {
        m_nodes[e.attribute("value").toInt()]=
           NodeNode(
              Coord3D(e.attribute("x").toFloat(), e.attribute("y").toFloat(), e.attribute("z").toFloat()),
              e.attribute("locked").toInt(),   // anchored is equal to locked.
              e.attribute("locked").toInt(),
              0.0f,                            // selected
              e.attribute("red").toFloat(),
              e.attribute("green").toFloat(),
              e.attribute("blue").toFloat(),
              e.attribute("is_probabilistic").toInt());

        if (e.attribute("isInitial").toInt())
        {  
          m_initialState = e.attribute("value").toInt();
        }
      }
      if (e.tagName() == "StateLabelNode") 
      {
        m_stateLabelnodes[e.attribute("value").toInt()]=
          LabelNode(
              Coord3D(e.attribute("x").toFloat(),e.attribute("y").toFloat(),e.attribute("z").toFloat()),
              e.attribute("locked").toInt(),   // anchored is equal to locked.
              e.attribute("locked").toInt(),
              0.0f,                            // selected
              e.attribute("red").toFloat(),
              e.attribute("green").toFloat(),
              e.attribute("blue").toFloat(),
              e.attribute("labelindex").toInt());

      }

      if (e.tagName() == "TransitionLabel") 
      {
        m_transitionLabels[e.attribute("value").toInt()]=LabelString(e.attribute("isTau").toInt(),e.attribute("label"));
      }
      if (e.tagName() == "Transition") 
      {
        m_edges[e.attribute("value").toInt()]=Edge(e.attribute("from").toInt(),e.attribute("to").toInt());
        m_handles[e.attribute("value").toInt()]=
          Node(Coord3D(e.attribute("x").toFloat(),e.attribute("y").toFloat(),e.attribute("z").toFloat()),
               e.attribute("locked").toInt(),  // anchored is equal to locked.
               e.attribute("locked").toInt(),
               0.0f);                          // selected
      }
      if (e.tagName() == "TransitionLabelNode") 
      {
        m_transitionLabelnodes[e.attribute("value").toInt()]=
          LabelNode(
              Coord3D(e.attribute("x").toFloat(),e.attribute("y").toFloat(),e.attribute("z").toFloat()),
              e.attribute("locked").toInt(),   // anchored is equal to locked.
              e.attribute("locked").toInt(),
              0.0f,                            // selected
              e.attribute("red").toFloat(),
              e.attribute("green").toFloat(),
              e.attribute("blue").toFloat(),
              e.attribute("labelindex").toInt());
      }

      node = node.nextSibling();
    }

    if (m_sel != nullptr)
    {
      delete m_sel;
      m_sel = nullptr;
    }
    m_stable = true;

    unlockForWrite(m_lock, GRAPH_LOCK_TRACE);
  }

  void Graph::saveXML(const QString& filename)
  {
    lockForRead(m_lock, GRAPH_LOCK_TRACE);

    QDomDocument xml;
    QDomElement root = xml.createElement("Graph");
    root.setAttribute("type", (int)m_type);
    root.setAttribute("states", (int)nodeCount());
    root.setAttribute("transitions", (int)edgeCount());
    root.setAttribute("statelabels", (int)stateLabelCount());
    root.setAttribute("transitionlabels", (int)transitionLabelCount());
    xml.appendChild(root);

    for (size_t i = 0; i < stateLabelCount(); ++i)
    {
      QDomElement stateL = xml.createElement("StateLabel");
      stateL.setAttribute("value", (int)i);
      stateL.setAttribute("label", stateLabelstring(i));
      root.appendChild(stateL);
    }

    for (size_t i = 0; i < nodeCount(); ++i)
    {
      QDomElement state = xml.createElement("State");
      state.setAttribute("value", (int)i);
      state.setAttribute("x", node(i).pos().x);
      state.setAttribute("y", node(i).pos().y);
      state.setAttribute("z", node(i).pos().z);
      state.setAttribute("locked", node(i).locked());
      state.setAttribute("isInitial", (int)(i == initialState()));
      state.setAttribute("red", node(i).color(0));
      state.setAttribute("green", node(i).color(1));
      state.setAttribute("blue", node(i).color(2));
      state.setAttribute("is_probabilistic", node(i).is_probabilistic());
      root.appendChild(state);

      QDomElement stateL = xml.createElement("StateLabelNode");
      stateL.setAttribute("value", (int)i);
      stateL.setAttribute("labelindex", (int)stateLabel(i).labelindex());
      stateL.setAttribute("x", stateLabel(i).pos().x);
      stateL.setAttribute("y", stateLabel(i).pos().y);
      stateL.setAttribute("z", stateLabel(i).pos().z);
      stateL.setAttribute("locked", stateLabel(i).locked());
      stateL.setAttribute("red", stateLabel(i).color(0));
      stateL.setAttribute("green", stateLabel(i).color(1));
      stateL.setAttribute("blue", stateLabel(i).color(2));
      root.appendChild(stateL);
    }

    for (size_t i = 0; i < transitionLabelCount(); ++i)
    {
      QDomElement edgL = xml.createElement("TransitionLabel");
      edgL.setAttribute("value", (int)i);
      edgL.setAttribute("label", transitionLabelstring(i));
      root.appendChild(edgL);
    }

    for (size_t i = 0; i < edgeCount(); ++i)
    {
      QDomElement edg = xml.createElement("Transition");
      edg.setAttribute("value", (int)i);
      edg.setAttribute("from", (int)edge(i).from());
      edg.setAttribute("to", (int)edge(i).to());
      edg.setAttribute("x", handle(i).pos().x);
      edg.setAttribute("y", handle(i).pos().y);
      edg.setAttribute("z", handle(i).pos().z);
      edg.setAttribute("locked", handle(i).locked());
      root.appendChild(edg);

      QDomElement edgL = xml.createElement("TransitionLabelNode");
      edgL.setAttribute("value", (int)i);
      edgL.setAttribute("labelindex", (int)transitionLabel(i).labelindex());
      edgL.setAttribute("x", transitionLabel(i).pos().x);
      edgL.setAttribute("y", transitionLabel(i).pos().y);
      edgL.setAttribute("z", transitionLabel(i).pos().z);
      edgL.setAttribute("locked", transitionLabel(i).locked());
      edgL.setAttribute("red", transitionLabel(i).color(0));
      edgL.setAttribute("green", transitionLabel(i).color(1));
      edgL.setAttribute("blue", transitionLabel(i).color(2));
      root.appendChild(edgL);
    }

    QFile data(filename);
    if (data.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&data);
        xml.save(out, 2);
    }

    // Todo: Perhaps save selection too

    unlockForRead(m_lock, GRAPH_LOCK_TRACE);
  }

  Edge Graph::edge(size_t index) const
  {
    return m_edges[index];
  }

  NodeNode& Graph::node(size_t index)
  {
    return m_nodes[index];
  }

  Node& Graph::handle(size_t edge)
  {
    return m_handles[edge];
  }

  LabelNode& Graph::transitionLabel(size_t edge)
  {
    return m_transitionLabelnodes[edge];
  }

  LabelNode& Graph::stateLabel(size_t edge)
  {
    return m_stateLabelnodes[edge];
  }

  const QString& Graph::transitionLabelstring(size_t labelindex) const
  {
    if (labelindex >= m_transitionLabels.size())
      return m_empty;
    return m_transitionLabels[labelindex].label();
  }

  const QString& Graph::stateLabelstring(size_t labelindex) const
  {
    if (labelindex >= m_stateLabels.size())
      return m_empty;
    return m_stateLabels[labelindex];
  }

  void Graph::clip(const Coord3D& min, const Coord3D& max)
  {
    lockForRead(m_lock, GRAPH_LOCK_TRACE); // read lock because indices are not invalidated

    for (auto& nodeNode : m_nodes)
        nodeNode.pos().clip(min, max);
    for (auto& labelNode : m_transitionLabelnodes)
        labelNode.pos().clip(min, max);
    for (auto& node : m_handles)
        node.pos().clip(min, max);

    unlockForRead(m_lock, GRAPH_LOCK_TRACE);
  }

  void Graph::lock()
  {
    lockForRead(m_lock, GRAPH_LOCK_TRACE);
  }

  void Graph::unlock()
  {
    unlockForRead(m_lock, GRAPH_LOCK_TRACE);
  }

  #ifdef DEBUG_GRAPH_LOCKS
    void Graph::lock(const char* where)
    {
      lockForRead(m_lock, where);
    }

    void Graph::unlock(const char* where)
    {
      unlockForRead(m_lock, where);
    }
  #endif

  void Graph::makeSelection()
  {
    lockForWrite(m_lock, GRAPH_LOCK_TRACE);

    if (m_sel != nullptr)
    {
      delete m_sel;
    }
    m_sel = new Selection(*this);

    unlockForWrite(m_lock, GRAPH_LOCK_TRACE);
  }

  void Graph::discardSelection()
  {
    lockForWrite(m_lock, GRAPH_LOCK_TRACE);

    if (m_sel != nullptr)
    {
      delete m_sel;
      m_sel = nullptr;
    }

    // Deactive all nodes
    for (std::vector<NodeNode>::iterator it = m_nodes.begin(); it != m_nodes.end(); ++it)
      it->m_active = false;

    unlockForWrite(m_lock, GRAPH_LOCK_TRACE);
  }

  void Graph::toggleActive(size_t index)
  {
    lockForWrite(m_lock, GRAPH_LOCK_TRACE);

    if (m_sel != nullptr && index < m_nodes.size())
    {
      NodeNode& node = m_nodes[index];
      bool active = node.m_active;
      node.m_active = !node.m_active;
      if (active)
      {
        m_sel->contract(index);
      }
      else
      {
        m_sel->expand(index);
      }
    }

    unlockForWrite(m_lock, GRAPH_LOCK_TRACE);
  }

  bool Graph::isToggleable(size_t index)
  {
    if (m_sel == nullptr || index >= m_nodes.size())
      return false;

    lockForRead(m_lock, GRAPH_LOCK_TRACE);

    // active node count:
    // Todo: improve this
    size_t count = 0;
    for (size_t i = 0; i < m_sel->nodes.size(); ++i)
      if (m_nodes[m_sel->nodes[i]].m_active)
        ++count;
    
    NodeNode& node = m_nodes[index];
    bool toggleable = !node.m_active || (m_sel->isContractable(index) && count > 1);

    unlockForRead(m_lock, GRAPH_LOCK_TRACE);

    return toggleable;
  }

  void Graph::setStable(bool stable)
  {
    lockForRead(m_lock, GRAPH_LOCK_TRACE);

    m_stable = stable;

    unlockForRead(m_lock, GRAPH_LOCK_TRACE);
  }

  bool Graph::isBridge(size_t index) const
  {
    return m_sel->isBridge(index);
  }

  bool Graph::hasSelection() const
  {
    return m_sel != nullptr;
  }

  size_t Graph::selectionEdge(size_t index) const
  {
    return m_sel->edges[index];
  }

  size_t Graph::selectionNode(size_t index) const
  {
    return m_sel->nodes[index];
  }

  size_t Graph::selectionEdgeCount() const
  {
    if (m_sel == nullptr)
      return 0;
    return m_sel->edges.size();
  }

  size_t Graph::selectionNodeCount() const
  {
    if (m_sel == nullptr)
      return 0;
    return m_sel->nodes.size();
  }
}
