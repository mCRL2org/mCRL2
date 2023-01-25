// Author(s): Rimco Boudewijns and Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <QDomDocument>
#include <QFile>
#include <QTextStream>
#include <QtOpenGL>

#include "exploration.h"
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/gui/arcball.h"

namespace Graph
{

static QString stateLabelToQString(const mcrl2::lts::state_label_empty& /*unused*/)
{
  return QString("");
}
static QString stateLabelToQString(const mcrl2::lts::state_label_lts& label)
{
  return QString::fromStdString(mcrl2::lts::pp(label));
}
static QString stateLabelToQString(const mcrl2::lts::state_label_fsm& label)
{
  return QString::fromStdString(mcrl2::lts::pp(label));
}
static QString
transitionLabelToQString(const mcrl2::lts::action_label_lts& label)
{
  return QString::fromStdString(mcrl2::lts::pp(label));
}
static QString
transitionLabelToQString(const mcrl2::lts::action_label_string& label)
{
  return QString::fromStdString(label);
}


#ifndef DEBUG_GRAPH_LOCKS
#define GRAPH_LOCK(type, where, x) x
#else
#define GRAPH_LOCK(type, where, x)                                             \
  do                                                                           \
  {                                                                            \
    debug_lock(type, where);                                                   \
    x;                                                                         \
    debug_lock(type "ed", where);                                              \
  } while (false)
void debug_lock(const char *type, const char *func)
{
  static int count = 0;
  static std::unordered_map<Qt::HANDLE, int> ids;
  Qt::HANDLE id = QThread::currentThreadId();
  if (!ids.count(id))
  {
    ids[id] = ++count;
  }
  printf("[%d] %s at %s\n", ids[id], type, func);
  fflush(stdout);
}
#endif

#define lockForRead(lock, where) GRAPH_LOCK("lock", where, (lock).lockForRead())
#define unlockForRead(lock, where) GRAPH_LOCK("unlock", where, (lock).unlock())
#define lockForWrite(lock, where)                                              \
  GRAPH_LOCK("W lock", where, (lock).lockForWrite())
#define unlockForWrite(lock, where) GRAPH_LOCK("W unlock", where, (lock).unlock())

Graph::Graph()
    : m_exploration(nullptr), m_type(mcrl2::lts::lts_lts), m_empty(""),
      m_stable(true)
{
}

Graph::~Graph()
{
  if (m_exploration != nullptr)
  {
    delete m_exploration;
    m_exploration = nullptr;
  }
}

std::size_t Graph::edgeCount() const
{
  return m_edges.size();
}

std::size_t Graph::nodeCount() const
{
  return m_nodes.size();
}

std::size_t Graph::transitionLabelCount() const
{
  return m_transitionLabels.size();
}

std::size_t Graph::stateLabelCount() const
{
  return m_stateLabels.size();
}

std::size_t Graph::initialState() const
{
  return m_initialState;
}

void Graph::clear()
{
  m_nodes.clear();
  m_handles.clear();
  m_edges.clear();
  m_transitionLabels.clear();
  m_transitionLabelnodes.clear();
  m_stateLabels.clear();
  m_stateLabelnodes.clear();
}

void Graph::load(const QString& filename, const QVector3D& min,
                 const QVector3D& max)
{
  lockForWrite(m_lock, GRAPH_LOCK_TRACE);

  clear();
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
  catch (mcrl2::runtime_error& e)
  {
    unlockForWrite(m_lock, GRAPH_LOCK_TRACE);
    throw e;
  }
  if (m_exploration != nullptr)
  {
    delete m_exploration;
    m_exploration = nullptr;
  }
  m_stable = true;

  unlockForWrite(m_lock, GRAPH_LOCK_TRACE);
}

template <class lts_t>
void Graph::templatedLoad(const QString& filename, const QVector3D& min,
                          const QVector3D& max)
{
  lts_t lts;
  lts.load(filename.toUtf8().constData());

  // Reserve all auxiliary data vectors
  m_nodes.reserve(lts.num_states());
  m_edges.reserve(lts.num_transitions());
  m_handles.reserve(lts.num_transitions());

  m_transitionLabels.reserve(lts.num_action_labels());
  m_transitionLabelnodes.reserve(lts.num_transitions());

  m_stateLabels.reserve(lts.num_state_labels());
  m_stateLabelnodes.reserve(lts.num_states());

  for (std::size_t i = 0; i < lts.num_state_labels(); ++i)
  {
    m_stateLabels.push_back(stateLabelToQString(lts.state_label(i)));
  }

  // Position nodes randomly
  for (std::size_t i = 0; i < lts.num_states(); ++i)
  {
    const bool is_not_probabilistic = false;
    m_nodes.emplace_back(
        QVector3D(frand(min.x(), max.x()), frand(min.y(), max.y()), frand(min.z(), max.z())),
        is_not_probabilistic);
    m_stateLabelnodes.emplace_back(m_nodes[i].pos(), i);
  }

  // Store string representations of labels
  for (std::size_t i = 0; i < lts.num_action_labels(); ++i)
  {
    QString label = transitionLabelToQString(lts.action_label(i));
    m_transitionLabels.push_back(LabelString(lts.is_tau(i), label));
  }

  // Assign and position edge handles, position edge labels
  for (std::size_t i = 0; i < lts.num_transitions(); ++i)
  {
    mcrl2::lts::transition& t = lts.get_transitions()[i];
    std::size_t new_probabilistic_state = add_probabilistic_state<lts_t>(
        lts.probabilistic_state(t.to()), min, max);
    m_edges.emplace_back(t.from(), new_probabilistic_state);
    m_handles.push_back(Node(
        (m_nodes[t.from()].pos() + m_nodes[new_probabilistic_state].pos()) /
        2.0));
    m_transitionLabelnodes.emplace_back(
        (m_nodes[t.from()].pos() + m_nodes[new_probabilistic_state].pos()) /
            2.0,
        t.label());
  }

  m_initialState = add_probabilistic_state<lts_t>(
      lts.initial_probabilistic_state(), min, max);
}

template <class lts_t>
std::size_t Graph::add_probabilistic_state(
    const typename lts_t::probabilistic_state_t& probabilistic_state,
    const QVector3D& min, const QVector3D& max)
{
  if (probabilistic_state.size() == 1)
  {
    return probabilistic_state.begin()->state();
  }
  else
  {
    // There are multiple probabilistic states. Make a new state
    // with outgoing probabilistic transitions to all states.
    std::size_t index_of_the_new_probabilistic_state = m_nodes.size();
    const bool is_probabilistic = true;
    m_nodes.emplace_back(
        QVector3D(frand(min.x(), max.x()), frand(min.y(), max.y()), frand(min.z(), max.z())),
        is_probabilistic);
    m_stateLabelnodes.emplace_back(m_nodes[index_of_the_new_probabilistic_state].pos(),
                  index_of_the_new_probabilistic_state);

    // The following map recalls where probabilities are stored in
    // transitionLabels.
    typedef std::map<typename lts_t::probabilistic_state_t::probability_t,
                     std::size_t>
        probability_map_t;
    probability_map_t probability_label_indices;
    for (const typename lts_t::probabilistic_state_t::state_probability_pair&
            p : probabilistic_state)
    {
      // Find an index for the probabilistic label of the outgoing transition of
      // the probabilistic state.
      std::size_t label_index;
      const typename probability_map_t::const_iterator i =
          probability_label_indices.find(p.probability());
      if (i == probability_label_indices.end()) // not found
      {
        label_index = m_transitionLabels.size();
        probability_label_indices[p.probability()] = label_index;
        m_transitionLabels.push_back(
            LabelString(false, QString::fromStdString(pp(p.probability()))));
      }
      else
      {
        label_index = i->second;
      }

      m_edges.push_back(Edge(index_of_the_new_probabilistic_state, p.state()));
      m_handles.push_back(
          Node((m_nodes[index_of_the_new_probabilistic_state].pos() +
                m_nodes[p.state()].pos()) /
               2.0));
      m_transitionLabelnodes.push_back(
          LabelNode((m_nodes[index_of_the_new_probabilistic_state].pos() +
                     m_nodes[p.state()].pos()) /
                        2.0,
                    label_index));
    }
    return index_of_the_new_probabilistic_state;
  }
}

void Graph::loadXML(const QString& filename)
{
  lockForWrite(m_lock, GRAPH_LOCK_TRACE);

  QDomDocument xml;
  QFile file(filename);
  if (!file.open(QFile::ReadOnly))
  {
    mCRL2log(mcrl2::log::error)
        << "Could not open XML file: " << filename.toStdString() << std::endl;
    return;
  }
  QString errorMsg;
  if (!xml.setContent(&file, false, &errorMsg))
  {
    file.close();
    mCRL2log(mcrl2::log::error)
        << "Could not parse XML file: " << errorMsg.toStdString() << std::endl;
    return;
  }
  file.close();

  QDomElement root = xml.documentElement();
  if (root.tagName() != "Graph")
  {
    mCRL2log(mcrl2::log::error) << "XML contains no valid graph" << std::endl;
    return;
  }

  m_type = (mcrl2::lts::lts_type)root.attribute("type").toInt();

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

    if (e.tagName() == "StateLabel")
    {
      m_stateLabels[e.attribute("value").toInt()] = e.attribute("label");
    }
    if (e.tagName() == "State")
    {
      m_nodes[e.attribute("value").toInt()] = NodeNode(
          QVector3D(e.attribute("x").toFloat(), e.attribute("y").toFloat(),
                  e.attribute("z").toFloat()),
          e.attribute("locked").toInt() != 0, // anchored is equal to locked.
          e.attribute("locked").toInt() != 0,
          0.0f, // selected
          QVector3D(e.attribute("red").toFloat(), e.attribute("green").toFloat(), e.attribute("blue").toFloat()),
          e.attribute("is_probabilistic").toInt() != 0);

      if (e.attribute("isInitial").toInt() != 0)
      {
        m_initialState = e.attribute("value").toInt();
      }
    }
    if (e.tagName() == "StateLabelNode")
    {
      m_stateLabelnodes[e.attribute("value").toInt()] = LabelNode(
          QVector3D(e.attribute("x").toFloat(), e.attribute("y").toFloat(),
                  e.attribute("z").toFloat()),
          e.attribute("locked").toInt() != 0, // anchored is equal to locked.
          e.attribute("locked").toInt() != 0,
          0.0f, // selected
          QVector3D(e.attribute("red").toFloat(), e.attribute("green").toFloat(), e.attribute("blue").toFloat()),
          e.attribute("labelindex").toInt());
    }

    if (e.tagName() == "TransitionLabel")
    {
      m_transitionLabels[e.attribute("value").toInt()] =
          LabelString(e.attribute("isTau").toInt() != 0, e.attribute("label"));
    }
    if (e.tagName() == "Transition")
    {
      m_edges[e.attribute("value").toInt()] =
          Edge(e.attribute("from").toInt(), e.attribute("to").toInt());
      m_handles[e.attribute("value").toInt()] =
          Node(QVector3D(e.attribute("x").toFloat(), e.attribute("y").toFloat(),
                       e.attribute("z").toFloat()),
               e.attribute("locked").toInt() != 0, // anchored is equal to locked.
               e.attribute("locked").toInt() != 0,
               0.0f); // selected
    }
    if (e.tagName() == "TransitionLabelNode")
    {
      m_transitionLabelnodes[e.attribute("value").toInt()] = LabelNode(
          QVector3D(e.attribute("x").toFloat(), e.attribute("y").toFloat(),
                  e.attribute("z").toFloat()),
          e.attribute("locked").toInt() != 0, // anchored is equal to locked.
          e.attribute("locked").toInt() != 0,
          0.0f, // selected
          QVector3D(e.attribute("red").toFloat(), e.attribute("green").toFloat(), e.attribute("blue").toFloat()),
          e.attribute("labelindex").toInt());
    }

    node = node.nextSibling();
  }

  if (m_exploration != nullptr)
  {
    delete m_exploration;
    m_exploration = nullptr;
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

  for (std::size_t i = 0; i < stateLabelCount(); ++i)
  {
    QDomElement stateL = xml.createElement("StateLabel");
    stateL.setAttribute("value", (int)i);
    stateL.setAttribute("label", stateLabelstring(i));
    root.appendChild(stateL);
  }

  for (std::size_t i = 0; i < nodeCount(); ++i)
  {
    QDomElement state = xml.createElement("State");
    state.setAttribute("value", (int)i);
    state.setAttribute("x", node(i).pos().x());
    state.setAttribute("y", node(i).pos().y());
    state.setAttribute("z", node(i).pos().z());
    state.setAttribute("locked", static_cast<int>(node(i).locked()));
    state.setAttribute("isInitial", (int)(i == initialState()));
    state.setAttribute("red", node(i).color().x());
    state.setAttribute("green", node(i).color().y());
    state.setAttribute("blue", node(i).color().z());
    state.setAttribute("is_probabilistic", static_cast<int>(node(i).is_probabilistic()));
    root.appendChild(state);

    QDomElement stateL = xml.createElement("StateLabelNode");
    stateL.setAttribute("value", (int)i);
    stateL.setAttribute("labelindex", (int)stateLabel(i).labelindex());
    stateL.setAttribute("x", stateLabel(i).pos().x());
    stateL.setAttribute("y", stateLabel(i).pos().y());
    stateL.setAttribute("z", stateLabel(i).pos().z());
    stateL.setAttribute("locked", static_cast<int>(stateLabel(i).locked()));
    stateL.setAttribute("red", stateLabel(i).color().x());
    stateL.setAttribute("green", stateLabel(i).color().y());
    stateL.setAttribute("blue", stateLabel(i).color().z());
    root.appendChild(stateL);
  }

  for (std::size_t i = 0; i < transitionLabelCount(); ++i)
  {
    QDomElement edgL = xml.createElement("TransitionLabel");
    edgL.setAttribute("value", (int)i);
    edgL.setAttribute("label", transitionLabelstring(i));
    root.appendChild(edgL);
  }

  for (std::size_t i = 0; i < edgeCount(); ++i)
  {
    QDomElement edg = xml.createElement("Transition");
    edg.setAttribute("value", (int)i);
    edg.setAttribute("from", (int)edge(i).from());
    edg.setAttribute("to", (int)edge(i).to());
    edg.setAttribute("x", handle(i).pos().x());
    edg.setAttribute("y", handle(i).pos().y());
    edg.setAttribute("z", handle(i).pos().z());
    edg.setAttribute("locked", static_cast<int>(handle(i).locked()));
    root.appendChild(edg);

    QDomElement edgL = xml.createElement("TransitionLabelNode");
    edgL.setAttribute("value", (int)i);
    edgL.setAttribute("labelindex", (int)transitionLabel(i).labelindex());
    edgL.setAttribute("x", transitionLabel(i).pos().x());
    edgL.setAttribute("y", transitionLabel(i).pos().y());
    edgL.setAttribute("z", transitionLabel(i).pos().z());
    edgL.setAttribute("locked", static_cast<int>(transitionLabel(i).locked()));
    edgL.setAttribute("red", transitionLabel(i).color().x());
    edgL.setAttribute("green", transitionLabel(i).color().y());
    edgL.setAttribute("blue", transitionLabel(i).color().z());
    root.appendChild(edgL);
  }

  QFile data(filename);
  if (data.open(QFile::WriteOnly | QFile::Truncate))
  {
    QTextStream out(&data);
    xml.save(out, 2);
  }

  // Todo: Perhaps save exploration too

  unlockForRead(m_lock, GRAPH_LOCK_TRACE);
}

NodeNode& Graph::node(std::size_t index)
{
  return m_nodes[index];
}

Node& Graph::handle(std::size_t edge)
{
  return m_handles[edge];
}

LabelNode& Graph::transitionLabel(std::size_t edge)
{
  return m_transitionLabelnodes[edge];
}

LabelNode& Graph::stateLabel(std::size_t index)
{
  return m_stateLabelnodes[index];
}

const Edge& Graph::edge(std::size_t index) const
{
  return m_edges[index];
}

const NodeNode& Graph::node(std::size_t index) const
{
  return m_nodes[index];
}

const Node& Graph::handle(std::size_t edge) const
{
  return m_handles[edge];
}

const LabelNode& Graph::transitionLabel(std::size_t edge) const
{
  return m_transitionLabelnodes[edge];
}

const LabelNode& Graph::stateLabel(std::size_t index) const
{
  return m_stateLabelnodes[index];
}

const QString& Graph::transitionLabelstring(std::size_t labelindex) const
{
  if (labelindex >= m_transitionLabels.size())
  {
    return m_empty;
  }
  return m_transitionLabels[labelindex].label();
}

const QString& Graph::stateLabelstring(std::size_t labelindex) const
{
  if (labelindex >= m_stateLabels.size())
  {
    return m_empty;
  }
  return m_stateLabels[labelindex];
}

void Graph::clip(const QVector3D& min, const QVector3D& max)
{
  lockForRead(
      m_lock,
      GRAPH_LOCK_TRACE); // read lock because indices are not invalidated

  m_clip_min = min;
  m_clip_max = max;
  for (NodeNode& node : m_nodes)
  {
    clipVector(node.pos_mutable(), min, max);
  }
  for (LabelNode& node : m_transitionLabelnodes)
  {
    clipVector(node.pos_mutable(), min, max);
  }
  for (Node& node : m_handles)
  {
    clipVector(node.pos_mutable(), min, max);
  }

  unlockForRead(m_lock, GRAPH_LOCK_TRACE);
}

const QVector3D& Graph::getClipMin() const
{
  return m_clip_min;
}

const QVector3D& Graph::getClipMax() const
{
  return m_clip_max;
}

void Graph::lock() const
{
  lockForRead(m_lock, GRAPH_LOCK_TRACE);
}

void Graph::unlock() const
{
  unlockForRead(m_lock, GRAPH_LOCK_TRACE);
}

#ifdef DEBUG_GRAPH_LOCKS
void Graph::lock(const char *where)
{
  lockForRead(m_lock, where);
}

void Graph::unlock(const char *where)
{
  unlockForRead(m_lock, where);
}
#endif

void Graph::makeExploration()
{
  lockForWrite(m_lock, GRAPH_LOCK_TRACE);
  delete m_exploration;
  m_exploration = new Exploration(*this);

  unlockForWrite(m_lock, GRAPH_LOCK_TRACE);
}

void Graph::discardExploration()
{
  lockForWrite(m_lock, GRAPH_LOCK_TRACE);

  if (m_exploration != nullptr)
  {
    delete m_exploration;
    m_exploration = nullptr;
  }

  // Deactive all nodes
  for (NodeNode& node : m_nodes)
  {
    node.m_active = false;
  }

  unlockForWrite(m_lock, GRAPH_LOCK_TRACE);
}

void Graph::toggleOpen(std::size_t index)
{
  lockForWrite(m_lock, GRAPH_LOCK_TRACE);

  if (m_exploration != nullptr && index < m_nodes.size())
  {
    NodeNode& node = m_nodes[index];
    bool active = node.m_active;
    node.m_active = !node.m_active;
    if (active)
    {
      m_exploration->contract(index);
    }
    else
    {
      m_exploration->expand(index);
    }
  }

  unlockForWrite(m_lock, GRAPH_LOCK_TRACE);
}

bool Graph::isClosable(std::size_t index)
{
  if (m_exploration == nullptr || index >= m_nodes.size())
  {
    return false;
  }

  lockForRead(m_lock, GRAPH_LOCK_TRACE);

  // active node count:
  // Todo: improve this
  std::size_t count = 0;
  for (std::size_t nodeId : m_exploration->nodes) {
    if (m_nodes[nodeId].m_active)
    {
      ++count;
    }
}

  NodeNode& node = m_nodes[index];
  bool toggleable =
      !node.m_active || (m_exploration->isContractable(index) && count > 1);

  toggleable = toggleable && index != m_initialState;

  unlockForRead(m_lock, GRAPH_LOCK_TRACE);

  return toggleable;
}

void Graph::setStable(bool stable)
{
  lockForRead(m_lock, GRAPH_LOCK_TRACE);

  m_stable = stable;

  unlockForRead(m_lock, GRAPH_LOCK_TRACE);
}

bool Graph::isBridge(std::size_t index) const
{
  return m_exploration->isBridge(index);
}

bool Graph::hasExploration() const
{
  return m_exploration != nullptr;
}

std::size_t Graph::explorationEdge(std::size_t index) const
{
  return m_exploration->edges[index];
}

std::size_t Graph::explorationNode(std::size_t index) const
{
  return m_exploration->nodes[index];
}

std::size_t Graph::explorationEdgeCount() const
{
  if (m_exploration == nullptr)
  {
    return 0;
  }
  return m_exploration->edges.size();
}

std::size_t Graph::explorationNodeCount() const
{
  if (m_exploration == nullptr)
  {
    return 0;
  }
  return m_exploration->nodes.size();
}
}  // namespace Graph
