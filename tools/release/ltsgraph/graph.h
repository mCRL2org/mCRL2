// Author(s): Rimco Boudewijns and Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/**

  @file graph.h
  @author S. Cranen, R. Boudewijns

  This file contains an interface to the graph data structure used by LTSGraph.

*/

#ifndef GRAPH_H
#define GRAPH_H

#include "mcrl2/lts/lts.h"
#include "mcrl2/lts/state_label_empty.h"

#include <cmath>

#include <QReadWriteLock>
#include <utility>
#include <QVector3D>

namespace Graph
{
/**
 * @brief A class which contains the information of a single edge.
 */
class Edge
{
  protected:
    size_t m_from;      ///< The originating node.
    size_t m_to;        ///< The node pointed at.

  public:
    /// \brief Default constructor
    Edge()=default;

    /// \brief Constructor
    Edge(const size_t from, const size_t to)
      : m_from(from), m_to(to)
    {}

    /// \brief Obtain the value of from.
    size_t from() const
    {
      return m_from;
    }

    /// \brief Obtain a reference to the value of from.
    size_t& from()
    {
      return m_from;
    }

    /// \brief Obtain the value of to.
    size_t to() const
    {
      return m_to;
    }

    /// \brief Obtain a reference to the value of to.
    size_t& to()
    {
      return m_to;
    }
};


/**
 * @brief A class which contains the information of a single node (as in movable object).
 */
class Node
{
  protected:
    QVector3D m_pos;            ///< The position of the node.
    bool m_anchored;          ///< Indicates wether this node cannot be moved.
    bool m_locked;            ///< Indicates if anchored should be left true at all times.
    float m_selected;         ///< Indicates that this node is selected (pointed at). Range 0.0f .. 1.0f.

  public:

    /// \brief Default constructor.
    Node()=default;

    /// \brief Constructor
    Node(QVector3D  pos)
      : m_pos(pos),
        m_anchored(false),
        m_locked(false),
        m_selected(0.0f)
    {}

    /// \brief Constructor
    Node(QVector3D  pos,
         const bool anchored,
         const bool locked,
         const float& selected)
      : m_pos(pos),
        m_anchored(anchored),
        m_locked(locked),
        m_selected(selected)
    {}

    /// \brief Get the position of a node.
    const QVector3D& pos() const
    {
      return m_pos;
    }

    /// \brief Get a reference to the position of a node.
    QVector3D& pos_mutable()
    {
      return m_pos;
    }

    /// \brief Get the value of anchored.
    bool anchored() const
    {
      return m_anchored;
    }

    /// \brief Get a reference to anchored.
    void set_anchored(bool b)
    {
      m_anchored=b;
    }

    /// \brief Get the value of locked.
    bool locked() const
    {
      return m_locked;
    }

    /// \brief Get a reference to the value of locked.
    void set_locked(bool b)
    {
      m_locked=b;
    }

    /// \brief Get whether this node is selected.
    const float& selected() const
    {
      return m_selected;
    }

    /// \brief Get a reference to whether this node is selected.
    float& selected()
    {
      return m_selected;
    }
};

/**
 * @brief A class which contains a Node with additional color information.
 */
class NodeWithColor : public Node
{
  protected:
    GLfloat m_color[3];       ///< The (painted) color of the node.

  public:

    /// \brief Default constructor.
    NodeWithColor()=default;

    /// \brief Constructor
    NodeWithColor(const QVector3D& pos)
      : Node(pos)
    {
      m_color[0]=0.0f;
      m_color[1]=0.0f;
      m_color[2]=0.0f;
    }

    /// \brief Constructor
    NodeWithColor(
      const QVector3D& pos,
      const bool anchored,
      const bool locked,
      const float& selected,
      const GLfloat& color0,
      const GLfloat& color1,
      const GLfloat& color2)
      : Node(pos, anchored, locked, selected)
    {
      m_color[0]=color0;
      m_color[1]=color1;
      m_color[2]=color2;
    }

    /// \brief Get the color.
    GLfloat* color()
    {
      return m_color;
    }

    /// \brief Get the color.
    const GLfloat& color(size_t i) const
    {
      return m_color[i];
    }

    /// \brief Get a reference to the color.
    GLfloat& color(size_t i)
    {
      return m_color[i];
    }

};



/**
 * @brief A structure which contains the information of a single label (with tau indicator).
 */
struct LabelString
{
  protected:
    bool m_isTau;             ///< Indicates that the label is tau.
    QString m_label;          ///< The string representation of the label.

  public:
    /// \brief Default constructor.
    LabelString() = default;

    /// \brief Constructor. If isTau is true, set the label to the the greek symbol tau.
    LabelString(bool isTau, const QString& label)
      : m_isTau(isTau), m_label(isTau?QChar(0x03C4):label)
    {}

    /// \brief Get whether this label is equal to tau.
    bool is_tau() const
    {
      return m_isTau;
    }

    /// \brief Get the label in this string.
    const QString& label() const
    {
      return m_label;
    }
};


/**
 * @brief A structure which contains the information of a single edge.
 */
class LabelNode : public NodeWithColor
{
  protected:
    size_t m_labelindex;      ///< The index of the label (string).

  public:
    /// \default constructor
    LabelNode() = default;

    /// \brief Constructor
    LabelNode(const QVector3D& p, const size_t labelindex)
      : NodeWithColor(p), m_labelindex(labelindex)
    {}

    /// \brief Constructor
    LabelNode(const QVector3D& pos,
              bool anchored,
              bool locked,
              const float& selected,
              const GLfloat& color0,
              const GLfloat& color1,
              const GLfloat& color2,
              const size_t labelindex)
      : NodeWithColor(pos,anchored,locked,selected,color0,color1,color2), m_labelindex(labelindex)
    {
    }

    /// \brief Get the value of labelindex.
    size_t labelindex() const
    {
      return m_labelindex;
    }

    /// \brief Get a reference to the value of labelindex.
    size_t& labelindex()
    {
      return m_labelindex;
    }
};

class Graph;

/**
 * @brief A structure which contains the information of a single graph node.
 */
class NodeNode : public NodeWithColor
{
    friend Graph;

  protected:
    bool m_is_probabilistic;  ///< Indicates that this is a probabilistic state.
    bool m_active;            ///< Indicates that this node was activated (see toggleActive).

  public:
    /// \brief Default constructor.
    NodeNode()=default;

    /// \brief Constructor.
    NodeNode(const QVector3D& p, const bool is_probabilistic)
      : NodeWithColor(p), m_is_probabilistic(is_probabilistic), m_active(false)
    {
      if (!m_is_probabilistic) // Color action states white (probabilistic states remain black)
      {
        m_color[0]=1.0f;
        m_color[1]=1.0f;
        m_color[2]=1.0f;
      }
    }

    /// \brief Constructor
    NodeNode(const QVector3D& pos,
             bool anchored,
             bool locked,
             const float& selected,
             const GLfloat& color0,
             const GLfloat& color1,
             const GLfloat& color2,
             bool is_probabilistic)
      : NodeWithColor(pos,anchored,locked,selected,color0,color1,color2), m_is_probabilistic(is_probabilistic)
    {
    }

    /// \brief Get whether the node is probabilistic.
    bool is_probabilistic() const
    {
      return m_is_probabilistic;
    }

    /// \brief Get a reference to the boolean indicating that this NodeNode is probabilistic.
    bool& is_probabilistic()
    {
      return m_is_probabilistic;
    }

    /// \brief Get the value of active.
    bool active() const
    {
      return m_active;
    }
};

class Selection;
class Information;

/**
@brief: This is the internal data structure that LTSGraph operates on.

  In its implementation it uses the mCRL2 lts classes to represent the graphs,
  and augments it with further information. In particular, positions of labels
  and edge handles are stored as if they were nodes.
@attention When using the graph structure--especially when iterating--the graph should be locked and unlocked afterwards!
  See header file for details which operations are unguarded.
*/
// Todo: see if graph is locked as required throughout the application.
class Graph
{
    friend class Selection;

  private:
    Selection* m_sel;               ///< The selection of the current graph (or null).
    mcrl2::lts::lts_type m_type;    ///< The type of the current graph.
    QString m_empty;                ///< Empty string that is returned as label if none present.
    QReadWriteLock m_lock;          ///< Lock protecting the structure from being changed while rendering and simulating
    bool m_stable;                  ///< When true, the graph is considered stable, spring forces should not be applied.

    std::vector<NodeNode> m_nodes;                  ///< Vector containing all graph nodes.
    std::vector<Node> m_handles;                    ///< Vector containing all handles.
    std::vector<Edge> m_edges;                      ///< Vector containing all edges.
    std::vector<LabelString> m_transitionLabels;    ///< Vector containing all transition label strings.
    std::vector<LabelNode> m_transitionLabelnodes;  ///< Vector containing all transition label nodes.
    std::vector<QString> m_stateLabels;             ///< Vector containing all state label strings.
    std::vector<LabelNode> m_stateLabelnodes;       ///< Vector containing all state label nodes.

    /**
     * @brief Loads a graph of type ltsclass
     */
    template <class ltsclass>
    void templatedLoad(const QString& filename, const QVector3D& min, const QVector3D& max, float nodesize);

    // For each probability/state pair a new transition is generated labelled with the probability.
    // The index of the newly generated state is returned.
    // If there is only one state in the probabilistic state, then the index of this new state
    // is returned and no new transition is made.
    template <class ltsclass>
    size_t add_probabilistic_state(const typename ltsclass::probabilistic_state_t& probabilistic_state,
                                   const QVector3D& min, const QVector3D& max);
    size_t m_initialState;                          ///< Index of the initial state.
  protected:

  public:
    Graph();
    ~Graph();

    /**
     * @brief makes the graph structure read-only
     */
    void lock();
    /**
     * @brief makes the graph structure writable again after a lock
     */
    void unlock();

#ifndef DEBUG_GRAPH_LOCKS
#define GRAPH_LOCK_TRACE
#else
    void lock(const char* where);
    void unlock(const char* where);
#define GRAPH_LOCK_TRACE __func__
#endif

    /**
     * @brief Loads a graph with random positioning for the nodes.
     * @param filename The file which contains the graph.
     * @param min The minimum coordinates for any node.
     * @param max The maximum coordinates for any node.
     * @param nodesize The size of one node
     */
    void load(const QString& filename, const QVector3D& min, const QVector3D& max, float nodesize);

    /**
     * @brief Loads a graph with from a XML file exported using @fn saveXML.
     * @param filename The file which contains the graph.
     */
    void loadXML(const QString& filename);

    /**
     * @brief Saves the current graph with all neccesary information.
     * @param filename The file to which the graph is saved.
     */
    void saveXML(const QString& filename);

    /**
     * @brief Restrains all nodes of the graph between @e min and @e max.
     * @param min The minimum coordinates for any node.
     * @param max The maximum coordinates for any node.
     */
    void clip(const QVector3D& min, const QVector3D& max);

    void makeSelection(); ///< Creates a new empty selection (overwriting existing).
    void discardSelection(); ///< Discards the current selection (when present).

    /**
     * @brief Toggles the state of a node between active and inactive.
     *        Active nodes add their related nodes to the current selection.
     * @param index The index of the node.
     */
    void toggleActive(size_t index);
    /**
     * @brief Returns whether a given node should be toggled active or inactive.
     *        A node that leaves unconnected components or the selection empty should not be toggled inactive.
     * @param index The index of the node.
     */
    bool isToggleable(size_t index);

    void setStable(bool stable); ///< @brief Sets whether this graph is stable. (guarded)

    /*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*
     ! The operations below this note are unguarded, lock before use!  !
     *!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*/

    /**
     * @brief Returns the string representation of the transition label with index @e labelindex.
     *        If the index is not valid, an empty string is returned.
     * @param labelindex The index of the label.
     */
    const QString& transitionLabelstring(size_t labelindex) const;

    /**
     * @brief Returns the string representation of the state label with index @e labelindex.
     *        If the index is not valid, an empty string is returned.
     * @param labelindex The index of the label.
     */
    const QString& stateLabelstring(size_t labelindex) const;


    // Getters and setters
    Edge edge(size_t index) const;
    NodeNode& node(size_t index);
    Node& handle(size_t edge);
    LabelNode& transitionLabel(size_t edge);
    LabelNode& stateLabel(size_t edge);
    bool isTau(size_t labelindex) const;
    bool isBridge(size_t index) const; ///< Returns whether a given node forms a bridge in the selection

    size_t initialState() const;
    size_t edgeCount() const;
    size_t nodeCount() const;
    size_t transitionLabelCount() const;
    size_t stateLabelCount() const;

    bool hasSelection() const;                ///< Returns whether a portion of the graph is selected
    size_t selectionEdge(size_t index) const; ///< Returns the edge index for a certain edge in the selection
    size_t selectionNode(size_t index) const; ///< Returns the node index for a certain node in the selection
    size_t selectionEdgeCount() const;        ///< Returns the number of edges in the selection
    size_t selectionNodeCount() const;        ///< Returns the number of nodes in the selection

    const bool& stable() const ///< @brief Gets whether this graph is stable.
    {
      return m_stable;
    }
    bool& stable() ///< @brief Sets whether this graph is stable.
    {
      return m_stable;
    }
};
}  // namespace Graph

#endif // GRAPH_H
