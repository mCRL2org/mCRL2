// Author(s): Rimco Boudewijns and Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
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
#include "mcrl2/gui/glu.h"

#include <QReadWriteLock>
#include <QVector3D>
#include <QString>
#include <QElapsedTimer>
#include <QPainter>
#include <QMutex>

#include <utility>
#include <cmath>
#include <unordered_map>

namespace Graph
{
/**
 * @brief A class which contains the information of a single edge.
 */
class Edge
{
  protected:
    std::size_t m_from;      ///< The originating node.
    std::size_t m_to;        ///< The node pointed at.

  public:
    /// \brief Default constructor
    Edge()=default;

    /// \brief Constructor
    Edge(const std::size_t from, const std::size_t to)
      : m_from(from), m_to(to)
    {}

    /// \brief Obtain the value of from.
    constexpr std::size_t from() const
    {
      return m_from;
    }

    /// \brief Obtain a reference to the value of from.
    constexpr std::size_t& from()
    {
      return m_from;
    }

    /// \brief Obtain the value of to.
    constexpr std::size_t to() const
    {
      return m_to;
    }

    /// \brief Obtain a reference to the value of to.
    constexpr std::size_t& to()
    {
      return m_to;
    }

    constexpr bool is_selfloop() const
    {
      return m_from == m_to;
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
         const float selected)
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
    QVector3D m_color;       ///< The (painted) color of the node.

  public:

    /// \brief Default constructor.
    NodeWithColor()=default;

    /// \brief Constructor
    NodeWithColor(QVector3D pos)
      : Node(pos)
    {}

    /// \brief Constructor
    NodeWithColor(
      QVector3D pos,
      bool anchored,
      bool locked,
      float selected,
      QVector3D color)
      : Node(pos, anchored, locked, selected),
        m_color(color)
    {}

    /// \brief Get the color.
    QVector3D& color() { return m_color; }
    const QVector3D& color() const { return m_color; }
};



/**
 * @brief A structure which contains the information of a single label (with tau indicator).
 */
struct LabelString
{
  protected:
    QString m_label;          ///< The string representation of the label.

  public:
    /// \brief Default constructor.
    LabelString() = default;

    /// \brief Constructor. If isTau is true, set the label to the the greek symbol tau.
    LabelString(bool isTau, const QString& label)
      : m_label(isTau ? QChar(0x03C4) : label)
    {}

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
    std::size_t m_labelindex;      ///< The index of the label (string).

  public:
    /// \default constructor
    LabelNode() = default;

    /// \brief Constructor
    LabelNode(QVector3D p, std::size_t labelindex)
      : NodeWithColor(p), m_labelindex(labelindex)
    {}

    /// \brief Constructor
    LabelNode(QVector3D pos,
              bool anchored,
              bool locked,
              float selected,
              QVector3D color,
              std::size_t labelindex)
      : NodeWithColor(pos, anchored, locked, selected, color), m_labelindex(labelindex)
    {}

    /// \brief Get the value of labelindex.
    std::size_t labelindex() const
    {
      return m_labelindex;
    }

    /// \brief Get a reference to the value of labelindex.
    std::size_t& labelindex()
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
    bool m_active;            ///< Indicates that this node was activated in exploration mode (see toggleActive).

  public:
    /// \brief Default constructor.
    NodeNode()=default;

    /// \brief Constructor.
    NodeNode(const QVector3D& p, const bool is_probabilistic)
      : NodeWithColor(p), m_is_probabilistic(is_probabilistic), m_active(false)
    {
      if (m_is_probabilistic) // Color probabilistic states light blue
      {
        m_color = QVector3D(0.35f, 0.7f, 1.0f);
      }
      else // Color normal states white
      {
        m_color = QVector3D(1.0f, 1.0f, 1.0f);
      }
    }

    /// \brief Constructor
    NodeNode(QVector3D pos,
             bool anchored,
             bool locked,
             float selected,
             QVector3D color,
             bool is_probabilistic)
      : NodeWithColor(pos, anchored, locked, selected, color), m_is_probabilistic(is_probabilistic)
    {}

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

class Exploration;
class Information;

#define DEBUG_GRAPHS
#ifdef DEBUG_GRAPHS



struct DataView{
  double min, max, average, std;
  std::size_t n;
  DataView(std::list<double>& input_list);
};
class DebugView{

  public:
    DebugView(std::size_t log_duration = 0, std::size_t min_interval = 0);

    void push(double value);

    /// @brief if more than 1 sample is compiled into 1 point on the line
    ///        we can draw either of the following statistics
    bool m_drawStd = false;

    bool m_drawAvg = true;

    bool m_drawMin = false;

    bool m_drawMax = false;

    void draw(QPainter& painter, QBrush& brush, QPen& pen);
    
    void setDrawingArea(int width, int height, int offsetX, int offsetY);
    double getMax(){ return m_max_value; }
    double recalcMax();
    void setMax(double max)
    {
      m_max_value = max;
    }
    QPointF getOffset() { return QPointF(m_pad_width, m_pad_height); }

    void determineScale(double max = -1);
  private:
    QMutex* m_lock;
    void drawLine(QPainter& painter, std::vector<QPointF>& line,
                  double current_value, QBrush& brush,
                  QPen& pen);
    
  private:
      std::list<std::pair<std::size_t, DataView>> m_values = std::list<std::pair<std::size_t, DataView>>();

      std::list<double> m_current_interval = std::list<double>();
      std::size_t m_current_interval_start;

      QElapsedTimer m_timer = QElapsedTimer();

      std::size_t m_log_duration; // The amount of time represented by a line
      std::size_t m_min_interval; // The amount of time represented by a single point

      int m_width = 200;
      int m_height = 100;
      int m_pad_height = 20;
      int m_pad_width = 20;
      double m_max_value = 50;

      const double m_scale_tolerance = 0.7; // If the max value in m_values is less than m_scale_tolerance * m_max_value, we rescale m_max_value
};

struct PlotEntry
{
  std::string var;
  QBrush brush;
  QPen pen;
};
class GraphView
{
  public:
  GraphView(int rows, int cols, QRect bounds, std::size_t log_duration, std::size_t min_interval);

  void logVar(std::string name, double value);

  void addVar(std::string name);

  void addToPlot(int row, int col, PlotEntry entry);

  void draw(QPainter& painter);

  private:
  int m_rows = 2;
  int m_cols = 2;
  int m_padx = 20;
  int m_pady = 10;
  std::size_t m_log_duration;
  std::size_t m_min_interval;
  QRect m_bounds;

  std::vector<std::vector<PlotEntry>> m_plots;

  std::unordered_map<std::string, DebugView> m_vars;
};
#endif

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
    friend class Exploration;

  private:
    Exploration* m_exploration;                 ///< The exploration of the current graph (or null).
    mcrl2::lts::lts_type m_type;                ///< The type of the current graph.
    QString m_empty;                            ///< Empty string that is returned as label if none present.
    mutable QReadWriteLock m_lock;              ///< Lock protecting the structure from being changed while rendering and simulating
    bool m_stable;                              ///< When true, the graph is considered stable, spring forces should not be applied.
    bool m_hasNewFrame = false;                 ///< Set to false when frame is rendered. Whenever new layout is applied set to true.
    bool m_forceUpdate = false;                 ///< Some actions require forcing update -> toggleOpen for instance
    bool m_resetPositions = false;              ///< Resets all positions on the next worker iteration
    bool m_scrambleZPositions = false;          ///< Scrambles Z-positions on the next worker iteration
    QVector3D m_clip_min;
    QVector3D m_clip_max;

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
    void templatedLoad(const QString& filename, const QVector3D& min, const QVector3D& max);

    // For each probability/state pair a new transition is generated labelled with the probability.
    // The index of the newly generated state is returned.
    // If there is only one state in the probabilistic state, then the index of this new state
    // is returned and no new transition is made.
    template <class ltsclass>
    std::size_t add_probabilistic_state(const typename ltsclass::probabilistic_state_t& probabilistic_state,
                                   const QVector3D& min, const QVector3D& max);
    std::size_t m_initialState = 0;                          ///< Index of the initial state.
  protected:

  public:

    #define DEBUG_LOG_TEMPERATURE
    #ifdef DEBUG_LOG_TEMPERATURE
      GraphView gv_debug = GraphView(4, 1, {20, 20, 200, 600}, 1000, 1);
    #endif
    Graph();
    ~Graph();

    /**
     * @brief makes the graph structure read-only
     */
    void lock() const;
    /**
     * @brief makes the graph structure writable again after a lock
     */
    void unlock() const;

#ifndef DEBUG_GRAPH_LOCKS
#define GRAPH_LOCK_TRACE
#else
    void lock(const char* where);
    void unlock(const char* where);
#define GRAPH_LOCK_TRACE __func__
#endif

    /**
     * @brief Helper class that allows RAII-style locking for graph access
     */
    class Guard
    {
      public:
        Graph& graph;
        Guard(Graph& graph) : graph(graph) { graph.lock(GRAPH_LOCK_TRACE); }
        ~Guard() { graph.unlock(GRAPH_LOCK_TRACE); }
    };

    /**
     * @brief Deletes all nodes, handles, edges and labels stored in this graph
     */
    void clear();

    /**
     * @brief Loads a graph with random positioning for the nodes.
     * @param filename The file which contains the graph.
     * @param min The minimum coordinates for any node.
     * @param max The maximum coordinates for any node.
     */
    void load(const QString& filename, const QVector3D& min, const QVector3D& max);

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
    const QVector3D& getClipMin() const;
    const QVector3D& getClipMax() const;

    void makeExploration(); ///< Creates a new empty exploration (overwriting existing).
    void discardExploration(); ///< Discards the current exploration (when present).

    /**
     * @brief Toggles the state of a node between open and closed.
     *        For open nodes, all successors are added to the current exploration.
     * @param index The index of the node.
     */
    void toggleOpen(std::size_t index);
    /**
     * @brief Returns whether a given node can be closed.
     * @detail When closing the node would leave disconnected components in the
     * exploration, it is not allowed to close that node.
     * @param index The index of the node.
     */
    bool isClosable(std::size_t index);

    void setStable(bool stable); ///< @brief Sets whether this graph is stable. (guarded)

    /*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*
     ! The operations below this note are unguarded, lock before use!  !
     *!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*/

    /**
     * @brief Returns the string representation of the transition label with index @e labelindex.
     *        If the index is not valid, an empty string is returned.
     * @param labelindex The index of the label.
     */
    inline const QString& transitionLabelstring(std::size_t labelindex) const
    {
      if (labelindex >= m_transitionLabels.size())
      {
        return m_empty;
      }
      return m_transitionLabels[labelindex].label();
    }
    

    /**
     * @brief Returns the string representation of the state label with index @e labelindex.
     *        If the index is not valid, an empty string is returned.
     * @param labelindex The index of the label.
     */
    inline const QString& stateLabelstring(std::size_t labelindex) const
    {
      if (labelindex >= m_stateLabels.size())
      {
        return m_empty;
      }
      return m_stateLabels[labelindex];
    }
    

    bool userIsDragging = false;

    /// Accessors

    inline Node& handle(std::size_t edge)
    { 
      assert(edge<m_handles.size());
      return m_handles[edge];
    }   

    inline NodeNode& node(std::size_t index)
    { 
      assert(index<m_nodes.size());
      return m_nodes[index];
    } 
      
    inline LabelNode& transitionLabel(std::size_t edge)
    { 
      assert(edge<m_transitionLabelnodes.size());
      return m_transitionLabelnodes[edge];
    }
    
    inline LabelNode& stateLabel(std::size_t index)
    { 
      assert(index<m_stateLabelnodes.size());
      return m_stateLabelnodes[index];
    }
    
    /// Getters

    inline const Edge& edge(std::size_t index) const
    { 
      assert(index<m_edges.size());
      return m_edges[index]; 
    } 
      
    inline const NodeNode& node(std::size_t index) const
    {
      assert(index<m_nodes.size());
      return m_nodes[index];
    } 
    
    inline const Node& handle(std::size_t edge) const
    {
      assert(edge<m_handles.size());
      return m_handles[edge];
    }
    
    inline const LabelNode& transitionLabel(std::size_t edge) const
    {
      assert(edge<m_transitionLabelnodes.size());
      return m_transitionLabelnodes[edge];
    }
    
    inline const LabelNode& stateLabel(std::size_t index) const
    {
      assert(index<m_stateLabelnodes.size());
      return m_stateLabelnodes[index];
    }
    
    bool isBridge(std::size_t index) const; ///< Returns whether a given node forms a bridge in the exploration

    std::size_t initialState() const;
    std::size_t edgeCount() const;
    std::size_t nodeCount() const;
    std::size_t transitionLabelCount() const;
    std::size_t stateLabelCount() const;

    bool hasExploration() const;                ///< Returns whether a portion of the graph is selected
    std::size_t explorationEdge(std::size_t index) const; ///< Returns the edge index for a certain edge in the exploration
    std::size_t explorationNode(std::size_t index) const; ///< Returns the node index for a certain node in the exploration
    std::size_t explorationEdgeCount() const;        ///< Returns the number of edges in the exploration
    std::size_t explorationNodeCount() const;        ///< Returns the number of nodes in the exploration

    bool& stable() ///< @brief Sets whether this graph is stable.
    {
      return m_stable;
    }

    void hasNewFrame(bool b) 
    { 
      m_hasNewFrame = b; 
    }

    bool hasNewFrame() const 
    { 
      return m_hasNewFrame; 
    }

    bool& hasForcedUpdate()
    {
      return m_forceUpdate;
    }

    bool& resetPositions()
    {
      return m_resetPositions;
    }

    bool& scrambleZ()
    {
      return m_scrambleZPositions;
    }
};
}  // namespace Graph

#endif // GRAPH_H
