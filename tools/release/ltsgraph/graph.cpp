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
  return nodeCount() > 0 ? m_initialState : -1;
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
    m_stable = true;
    m_hasNewFrame = true;
    m_forceUpdate = true;
    m_resetPositions = true;

    unlockForWrite(m_lock, GRAPH_LOCK_TRACE);
    throw e;
  }
  if (m_exploration != nullptr)
  {
    delete m_exploration;
    m_exploration = nullptr;
  }
  m_stable = true;
  m_hasNewFrame = true;
  m_forceUpdate = true;
  m_resetPositions = true;
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

  // Position the ordinary states randomly
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

  // Position the probabilistic states randomly and make a mapping for the indices of non trivial probabilistic states.
  std::unordered_map<std::size_t, std::size_t> probabilistic_state_mapping;
  for (std::size_t i = 0; i < lts.num_probabilistic_states(); ++i)
  {
    probabilistic_state_mapping[i] = add_probabilistic_state<lts_t>(lts.probabilistic_state(i), min, max);
  }

  // Assign and position edge handles, position edge labels
  for (std::size_t i = 0; i < lts.num_transitions(); ++i)
  {
    const mcrl2::lts::transition& t = lts.get_transitions()[i];
    std::size_t new_probabilistic_state = probabilistic_state_mapping[t.to()];
    assert(m_edges.size() == m_handles.size());
    assert(m_transitionLabelnodes.size() == m_edges.size());
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
  if (probabilistic_state.size() <= 1)
  {
    return probabilistic_state.get();
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
  m_stable = false;
  unlockForWrite(m_lock, GRAPH_LOCK_TRACE);
  m_hasNewFrame = true;
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
  m_stable = false;
  m_hasNewFrame = true;

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
      setStable(false);
    }
    m_forceUpdate = true;
  }

  unlockForWrite(m_lock, GRAPH_LOCK_TRACE);
  
  m_hasNewFrame = true;
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
  for (std::size_t nodeId : m_exploration->nodes) 
  {
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
  m_stable = stable;
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
  assert(index<m_exploration->edges.size());
  return m_exploration->edges[index];
}

std::size_t Graph::explorationNode(std::size_t index) const
{
  assert(index<m_exploration->nodes.size());
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


DataView::DataView(std::list<double>& input_list){
  min = 1e12;
  max = -1e12;
  average = 0;
  std = 0;
  n = input_list.size();
  for (double d : input_list){
    min = std::min(d, min);
    min = std::max(d, max);
    average += d/n;
  }
  for (double d : input_list){
    std += (d-average)*(d-average)/n;
  }
}

DebugView::DebugView(std::size_t log_duration, std::size_t min_interval)
 : m_log_duration(log_duration), m_min_interval(min_interval){
    m_timer.restart();
  m_current_interval_start = m_timer.elapsed();
    m_lock = new QMutex();
}

void DebugView::push(double value){
  std::size_t current_time = m_timer.elapsed();
  //mCRL2log(mcrl2::log::debug) << "current_time: " << current_time << " current interval time: " << m_current_interval_start << " min interval time: " << m_min_interval << std::endl;

  bool changed = false;
  if (m_lock->tryLock(0))
  {
      // first we remove all from the front that are no longer relevant
      while (m_values.size() > 0 && current_time - m_values.front().first > m_log_duration){
        m_values.pop_front();
        changed = true;
      } 
      m_lock->unlock();
  }

  // Then we check whether the last interval has passed
  if (current_time - m_current_interval_start > m_min_interval){
    m_values.push_back({current_time, DataView(m_current_interval)});
    m_current_interval.clear();
    m_current_interval_start = current_time;
    changed = true;
  }
  
  // Add current log value to the current interval
  m_current_interval.push_back(value);

  if (changed) recalcMax();
}

double DebugView::recalcMax()
{
  double current_max = 0;

  m_lock->lock();
  for (auto& pair : m_values)
  {
    if (m_drawStd)
      current_max =
          std::max(pair.second.average + pair.second.std * 3, current_max);
    if (m_drawAvg)
      current_max = std::max(pair.second.average, current_max);
    if (m_drawMax)
      current_max = std::max(pair.second.max, current_max);
    if (m_drawMin)
      current_max = std::max(pair.second.min, current_max);
  }
  m_lock->unlock();

  // over time lower the max value
  if (current_max < m_scale_tolerance * m_max_value)
    m_max_value *= 0.99;

  // we cant raise the max slowly, because we lose information
  // we use *= 1.01 to slighly overshoot
  while (current_max > m_max_value)
    m_max_value *= 1.01; /// TODO: Solve m_max_value * x^n >= current_max
                         /// for given x, integer n

  return m_max_value;
}

void DebugView::drawLine(QPainter& painter, std::vector<QPointF>& line, double current_value, QBrush& brush, QPen& pen){
  painter.setBrush(brush);
  painter.setPen(pen);
  painter.drawPolyline(&line[0], static_cast<int>(line.size()));
  if (current_value >= 0)
    painter.drawText(line.back(), QString::number(current_value));
}

void DebugView::draw(QPainter& painter, QBrush& brush, QPen& pen){
  if (m_values.size() <= 1)
  {
    //mCRL2log(mcrl2::log::debug) << "Not enough values to draw: " << m_values.size() << std::endl;
      return;
  }
  //mCRL2log(mcrl2::log::debug) << "Drawing." << std::endl;
  std::vector<QPointF> pointsMin;
  std::vector<QPointF> pointsMax;
  std::vector<QPointF> pointsAvg;
  std::vector<QPointF> pointsStd;

  double t0 = static_cast<double>(m_values.front().first);
  auto getX = [&](std::size_t t) { return (t - t0) / m_log_duration; };
  auto getY = [&](double val) { return 1 - val / m_max_value; };
  auto createPoint = [&](std::size_t t, double val)
  {
    return QPointF(
        m_pad_width + getX(t) * m_width,
        m_pad_height + getY(val) * m_height);
  };
  m_lock->lock();
  for (auto& pair : m_values){
    pointsMin.push_back(createPoint(pair.first, pair.second.min));
    pointsMax.push_back(createPoint(pair.first, pair.second.max));
    pointsAvg.push_back(createPoint(pair.first, pair.second.average));
    pointsStd.push_back(createPoint(pair.first, pair.second.std));
  }
  m_lock->unlock();

  painter.setRenderHint(QPainter::RenderHint::Antialiasing, true);
  std::vector<QPointF> bounding_box = {
    QPointF(m_pad_width, m_pad_height),
    QPointF(m_pad_width + m_width, m_pad_height),
    QPointF(m_pad_width + m_width, m_pad_height + m_height),
    QPointF(m_pad_width, m_pad_height + m_height),
    QPointF(m_pad_width, m_pad_height),
  };
  QBrush qbrush{Qt::black, Qt::SolidPattern};
  QPen qpen{Qt::black, 1, Qt::SolidLine, Qt::FlatCap};
  drawLine(painter, bounding_box, -1e12, qbrush, qpen);
  if (m_drawMin) drawLine(painter, pointsMin, m_values.back().second.min, brush, pen);
  if (m_drawMax) drawLine(painter, pointsMax, m_values.back().second.max, brush, pen);
  if (m_drawAvg) drawLine(painter, pointsAvg, m_values.back().second.average, brush, pen);
  if (m_drawStd) drawLine(painter, pointsStd, m_values.back().second.std, brush, pen);
}

void DebugView::setDrawingArea(int width, int height, int offsetX, int offsetY)
{
  m_width = width;
  m_height = height;
  m_pad_width = offsetX;
  m_pad_height = offsetY;
}

GraphView::GraphView(int rows, int cols, QRect bounds, std::size_t log_duration, std::size_t min_interval)
   : m_rows(rows), 
     m_cols(cols), 
     m_log_duration(log_duration), 
     m_min_interval(min_interval),
     m_bounds(bounds)
{
    m_plots = std::vector<std::vector<PlotEntry>>(rows*cols);
}

void GraphView::addVar(std::string name)
{
  assert(m_vars.find(name) == m_vars.end()); // not yet inserted
  std::pair<std::string, DebugView> var = std::make_pair(name, DebugView(m_log_duration,m_min_interval));
  m_vars.emplace(var);
}

void GraphView::draw(QPainter& painter)
{
    
    for (std::size_t j = 0; j < m_plots.size(); j++)
    {
      auto& vec = m_plots[j];
      if (vec.size() < 1) continue;
      int row = j / m_cols;
      int col = j % m_cols;
      std::string graph_title;
      for (std::size_t i = 0; i < vec.size(); i++) {
        graph_title += vec[i].var;
        if (i < vec.size() - 1)
        {
          graph_title += ", ";
        }
        else
        {
          graph_title += ":";
        }
      }
      painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));
      painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap));

      double w = (m_bounds.width() - (m_cols - 1) * m_padx) /
                 static_cast<double>(m_cols);
      double h = (m_bounds.height() - (m_rows - 1) * m_pady) /
                 static_cast<double>(m_rows);
      double offsetX = m_bounds.left() + col * (w + m_padx);
      double offsetY = m_bounds.top() + row * (h + m_pady) - m_pady;
      painter.drawText(QPointF(offsetX, offsetY), QString::fromStdString(graph_title));
      double max = 0;
      for (auto& entry : vec)
      {
        max = std::max(max, m_vars[entry.var].recalcMax());
      }
      for (auto& entry : vec) {
        m_vars[entry.var].setMax(max);
        m_vars[entry.var].setDrawingArea(w, h, offsetX, offsetY);
        m_vars[entry.var].draw(painter, entry.brush, entry.pen);
      }
    }
}

void GraphView::addToPlot(int row, int col, PlotEntry entry){
  if (row >= m_rows || col >= m_cols)
  {
    mCRL2log(mcrl2::log::warning)
        << "Adding entry to graph of size (" << m_rows << ", " << m_cols
        << ") at position (" << row << ", " << col
        << "). Expanding number of plots to facilitate." << std::endl;
    int old_rows = m_rows;
    int old_cols = m_cols;
    m_rows = std::max(row + 1, m_rows);
    m_cols = std::max(col + 1, m_cols);
    std::vector<std::vector<PlotEntry>> new_plots(m_rows*m_cols);
    for (int r = 0; r < old_rows; r++)
    {
      for (int c = 0; c < old_cols; c++)
      {
        new_plots[r * m_cols + c] = m_plots[r * old_cols + c];
      }
    }
    m_plots = new_plots;
  }
  m_plots[row * m_cols + col].emplace_back(entry);
}

void GraphView::logVar(std::string name, double value)
{
  try
  {
    m_vars[name].push(value);
  }
  catch (int)
  {
    mCRL2log(mcrl2::log::error)
        << "Failed to log variable: " << name << std::endl;
    std::abort();
  }
}

} // namespace Graph
