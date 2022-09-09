// Author(s): Rimco Boudewijns and Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "springlayout.h"
#include "utility.h"

#include <QThread>
#include <cstdlib>

namespace Graph
{

//
// Utility functions
//

inline float cube(float x)
{
  return x * x * x;
}

inline void clip(float& f, float min, float max)
{
  if (f < min) {
    f = min;
  }
  else if (f > max) {
    f = max;
  }
}

//
// SpringLayout
//

SpringLayout::SpringLayout(Graph& graph, GLWidget& glwidget)
  : m_speed(0.001f), m_attraction(0.13f), m_repulsion(50.0f), m_natLength(50.0f), m_controlPointWeight(0.001f),
    m_graph(graph), m_ui(nullptr), m_attractionCalculation(&SpringLayout::forceLTSGraph), m_repulsionCalculation(&SpringLayout::repulsionForce), m_glwidget(glwidget), m_node_tree(0, {0, 0, 0}, {0, 0, 0}), m_trans_tree(0, {0, 0, 0}, {0, 0, 0}), m_handle_tree(0, {0, 0, 0}, {0, 0, 0})
{
  srand(time(nullptr));
}

SpringLayout::~SpringLayout()
{
  delete m_ui;
}

SpringLayoutUi* SpringLayout::ui(QWidget* parent)
{
  if (m_ui == nullptr) {
    m_ui = new SpringLayoutUi(*this, parent);
  }
  return m_ui;
}

void SpringLayout::setAttractionCalculation(AttractionCalculation c)
{
  switch (c)
  {
    case ltsgraph:
      m_attractionCalculation = &SpringLayout::forceLTSGraph;
      break;
    case linearsprings:
      m_attractionCalculation = &SpringLayout::forceLinearSprings;
      break;
    case electricalsprings:
      m_attractionCalculation = &SpringLayout::forceSpringElectricalModel;
      break;
  }
}

SpringLayout::AttractionCalculation SpringLayout::attractionCalculation()
{
  if (m_attractionCalculation == &SpringLayout::forceLTSGraph) {
    return ltsgraph;
  }
  if (m_attractionCalculation == &SpringLayout::forceSpringElectricalModel){
    return electricalsprings;
  }
  return linearsprings;
}

void SpringLayout::setRepulsionCalculation(RepulsionCalculation c)
{
  switch (c)
  {
    case ltsgraph_repulsion:
      m_repulsionCalculation = &SpringLayout::repulsionForce;
      break;
    case electricalsprings_repulsion:
      m_repulsionCalculation = &SpringLayout::repulsionForceElectricalModel;
      break;
    case no_repulsion:
      m_repulsionCalculation = &SpringLayout::noRepulsionForce;
      break;
  }
}

SpringLayout::RepulsionCalculation SpringLayout::repulsionCalculation()
{
  if (m_repulsionCalculation == &SpringLayout::repulsionForce) {
    return ltsgraph_repulsion;
  }
  if (m_repulsionCalculation == &SpringLayout::repulsionForceElectricalModel){
    return electricalsprings_repulsion;
  }

  if (m_repulsionCalculation == &SpringLayout::noRepulsionForce){
    return no_repulsion;
  }
}

QVector3D SpringLayout::forceLTSGraph(const QVector3D& a, const QVector3D& b, float ideal)
{
  QVector3D diff = (a - b);
  float dist = (std::max)(diff.length(), 1.0f);
  float factor = m_attraction * 10000 * std::log(dist / (ideal + 1.0f)) / dist;
  return diff * factor;
}

QVector3D SpringLayout::forceLinearSprings(const QVector3D& a, const QVector3D& b, float ideal)
{
  QVector3D diff = (a-b);
  float dist = diff.length() - ideal;
  float factor = m_spring_constant * std::max(dist, 0.0f) * m_attraction;
  if (dist > 0.0f)
  {
    factor = std::max(factor, 100 * m_attraction / (std::max)(dist * dist / 10000.0f, 0.1f));
  }
  return diff *  factor;
}

/// Adapts from old ltsgraph repulsive force scale to new ltsgraph force scale
const float adaptiveconstant = 0.0001;

inline float attractionConstant(float attraction){
  return attraction * adaptiveconstant;
}

QVector3D SpringLayout::forceSpringElectricalModel(const QVector3D& a, const QVector3D& b, float natlength){
  QVector3D diff = (a-b);
  return attractionConstant(m_attraction) * (diff.length() / std::max(0.01f, natlength)) * diff;
}

inline float repulsionConstant(float repulsion, float natlength)
{
  return repulsion * natlength * natlength * adaptiveconstant;
}


QVector3D SpringLayout::repulsionForce(const QVector3D& a, const QVector3D& b, float repulsion, float natlength)
{
  QVector3D diff = a - b;
  float r = repulsion;
  r /= cube((std::max)(diff.length() * 0.5f, natlength * 0.1f));
  diff = diff * r + QVector3D(fast_frand(-0.01f, 0.01f), fast_frand(-0.01f, 0.01f), fast_frand(-0.01f, 0.01f));
  return diff;
}


QVector3D SpringLayout::repulsionForceElectricalModel(const QVector3D& a, const QVector3D& b, float repulsion, float natlength){
  QVector3D diff = a - b;
  return repulsionConstant(repulsion, natlength) * diff / std::max(diff.lengthSquared(), 0.001f);
}


QVector3D SpringLayout::approxRepulsionForce(const QVector3D& a, Octree& tree, float repulsion, float natlength){
  QVector3D force(0, 0, 0);
  std::size_t num_nodes = 0;
  /*
  for(auto it = tree.begin(a), end = tree.end(a); it != end; ++it){
    int K = ((it->children == 0) ? 1 : it->children);
    force += (this->*m_repulsionCalculation)(a, it->pos, repulsion*it->children, natlength);
    num_nodes += 1;
  }
  */
  auto super_nodes = tree.getSuperNodes(a);
  for (auto super_node : super_nodes){
    force += (this->*m_repulsionCalculation)(a, super_node.second, super_node.first * repulsion, natlength);
  }
  num_nodes = super_nodes.size();
  m_max_num_nodes = std::max(m_max_num_nodes, num_nodes);
  m_total_num_nodes += num_nodes;
  return force;
}

const float time_scale = 1;
static QVector3D applyForce(const QVector3D& pos, const QVector3D& force, float speed)
{
  return pos + speed * force * time_scale;
}

void SpringLayout::apply()
{
  bool sel = m_graph.hasExploration();
  std::size_t nodeCount = sel ? m_graph.explorationNodeCount() : m_graph.nodeCount();
  std::size_t edgeCount = sel ? m_graph.explorationEdgeCount() : m_graph.edgeCount();
  if (!m_graph.stable())
  {
    m_graph.lock(GRAPH_LOCK_TRACE); // enter critical section

    m_nforces.resize(m_graph.nodeCount()); // Todo: compact this
    m_hforces.resize(m_graph.edgeCount());
    m_lforces.resize(m_graph.edgeCount());
    m_sforces.resize(m_graph.nodeCount());
    QVector3D node_min   = { INFINITY,  INFINITY,  INFINITY}, 
              node_max   = {-INFINITY, -INFINITY, -INFINITY}, 
              handle_min = { INFINITY,  INFINITY,  INFINITY}, 
              handle_max = {-INFINITY, -INFINITY, -INFINITY},
              trans_min  = { INFINITY,  INFINITY,  INFINITY},
              trans_max  = {-INFINITY, -INFINITY, -INFINITY};
    
    // calculate bounding volumes
    {
      for (int i = 0; i < nodeCount; i++)
      {
        std::size_t n = sel ? m_graph.explorationNode(i) : i;
        node_min.setX(std::min(node_min.x(), m_graph.node(n).pos().x())); 
        node_min.setY(std::min(node_min.y(), m_graph.node(n).pos().y())); 
        node_min.setZ(std::min(node_min.z(), m_graph.node(n).pos().z()));
        node_max.setX(std::max(node_max.x(), m_graph.node(n).pos().x())); 
        node_max.setY(std::max(node_max.y(), m_graph.node(n).pos().y())); 
        node_max.setZ(std::max(node_max.z(), m_graph.node(n).pos().z())); 
      }

      // calculate bounding volume
      for (int i = 0; i < edgeCount; i++)
      {
        std::size_t n = sel ? m_graph.explorationEdge(i) : i;
        handle_min.setX(std::min(handle_min.x(), m_graph.handle(n).pos().x())); 
        handle_min.setY(std::min(handle_min.y(), m_graph.handle(n).pos().y())); 
        handle_min.setZ(std::min(handle_min.z(), m_graph.handle(n).pos().z()));
        handle_max.setX(std::max(handle_max.x(), m_graph.handle(n).pos().x())); 
        handle_max.setY(std::max(handle_max.y(), m_graph.handle(n).pos().y())); 
        handle_max.setZ(std::max(handle_max.z(), m_graph.handle(n).pos().z())); 
      }

      // calculate bounding volume
      for (int i = 0; i < edgeCount; i++)
      {
        std::size_t n = sel ? m_graph.explorationEdge(i) : i;
        trans_min.setX(std::min(trans_min.x(), m_graph.transitionLabel(n).pos().x())); 
        trans_min.setY(std::min(trans_min.y(), m_graph.transitionLabel(n).pos().y())); 
        trans_min.setZ(std::min(trans_min.z(), m_graph.transitionLabel(n).pos().z()));
        trans_max.setX(std::max(trans_max.x(), m_graph.transitionLabel(n).pos().x())); 
        trans_max.setY(std::max(trans_max.y(), m_graph.transitionLabel(n).pos().y())); 
        trans_max.setZ(std::max(trans_max.z(), m_graph.transitionLabel(n).pos().z())); 
      }
    }
    node_min -= QVector3D(1, 1, 1);
    node_max += QVector3D(1, 1, 1);
    handle_min -= QVector3D(1, 1, 1);
    handle_max += QVector3D(1, 1, 1);
    trans_min -= QVector3D(1, 1, 1);
    trans_max += QVector3D(1, 1, 1);

    m_node_tree.setMinBounds(node_min);
    m_node_tree.setMaxBounds(node_max);
    m_handle_tree.setMinBounds(handle_min);
    m_handle_tree.setMaxBounds(handle_max);
    m_trans_tree.setMinBounds(trans_min);
    m_trans_tree.setMaxBounds(trans_max);

    m_node_tree.setTheta(m_accuracy);
    m_handle_tree.setTheta(m_accuracy);
    m_trans_tree.setTheta(m_accuracy);

    m_node_tree.reset();
    m_handle_tree.reset();
    m_trans_tree.reset();


    for (std::size_t i = 0; i < nodeCount; ++i)
    {
      std::size_t n = sel ? m_graph.explorationNode(i) : i;

      m_node_tree.insert(m_graph.node(n).pos());
      m_nforces[n] = {0, 0, 0};
      m_sforces[n] = (this->*m_attractionCalculation)(m_graph.node(n).pos(), m_graph.stateLabel(n).pos(), 0.0);
    }


    // approx repulsive forces for nodes
    for (std::size_t i = 0; i < nodeCount; ++i){
      std::size_t n = sel ? m_graph.explorationNode(i) : i;
      m_nforces[n] += approxRepulsionForce(m_graph.node(n).pos(), m_node_tree, m_repulsion, m_natLength);
    }


    for (std::size_t i = 0; i < edgeCount; ++i)
    {
      std::size_t n = sel ? m_graph.explorationEdge(i) : i;

      Edge e = m_graph.edge(n);
      QVector3D f;
      // Variables for repulsion calculations

      m_hforces[n] = QVector3D(0, 0, 0);
      m_lforces[n] = QVector3D(0, 0, 0);

      if (e.is_selfloop())
      {
        m_hforces[n] += repulsionForce(m_graph.handle(n).pos(), m_graph.node(e.from()).pos(), m_repulsion, m_natLength);
      }

      f = (this->*m_attractionCalculation)(m_graph.node(e.to()).pos(), m_graph.node(e.from()).pos(), m_natLength);
      m_nforces[e.from()] += f;
      m_nforces[e.to()] -= f;

      f = (this->*m_attractionCalculation)((m_graph.node(e.to()).pos() + m_graph.node(e.from()).pos()) / 2.0, m_graph.handle(n).pos(), 0.0);
      m_hforces[n] += f;

      f = (this->*m_attractionCalculation)(m_graph.handle(n).pos(), m_graph.transitionLabel(n).pos(), 0.0);
      m_lforces[n] += f;

      m_handle_tree.insert(m_graph.handle(n).pos());
      m_trans_tree.insert(m_graph.transitionLabel(n).pos());
    }

    // approximate repulsive forces
    for (std::size_t i = 0; i < edgeCount; ++i)
    {
      std::size_t n = sel ? m_graph.explorationEdge(i) : i;
      m_hforces[n] += approxRepulsionForce(m_graph.handle(n).pos(), m_handle_tree, m_repulsion * m_controlPointWeight, m_natLength);
      m_lforces[n] += approxRepulsionForce(m_graph.transitionLabel(n).pos(), m_trans_tree, m_repulsion * m_controlPointWeight, m_natLength);
    }

    QVector3D clipmin = m_graph.getClipMin();
    QVector3D clipmax = m_graph.getClipMax();
    float nodeSumForces = 0;
    float edgeSumForces = 0;
    for (std::size_t i = 0; i < nodeCount; ++i)
    {
      std::size_t n = sel ? m_graph.explorationNode(i) : i;

      if (!m_graph.node(n).anchored())
      {
        m_graph.node(n).pos_mutable() = applyForce(m_graph.node(n).pos(), m_nforces[n], m_speed);
        nodeSumForces += m_nforces[n].lengthSquared();
        clipVector(m_graph.node(n).pos_mutable(), clipmin, clipmax);
      }
      if (!m_graph.stateLabel(n).anchored())
      {
        m_graph.stateLabel(n).pos_mutable() = applyForce(m_graph.stateLabel(n).pos(), m_sforces[n], m_speed);
        nodeSumForces += m_sforces[n].lengthSquared();
        clipVector(m_graph.stateLabel(n).pos_mutable(), clipmin, clipmax);
      }
    }

    for (std::size_t i = 0; i < edgeCount; ++i)
    {
      std::size_t n = sel ? m_graph.explorationEdge(i) : i;

      if (!m_graph.handle(n).anchored())
      {
        m_graph.handle(n).pos_mutable() = applyForce(m_graph.handle(n).pos(), m_hforces[n], m_speed);
        edgeSumForces += m_hforces[n].lengthSquared();
        clipVector(m_graph.handle(n).pos_mutable(), clipmin, clipmax);
      }
      if (!m_graph.transitionLabel(n).anchored())
      {
        m_graph.transitionLabel(n).pos_mutable() = applyForce(m_graph.transitionLabel(n).pos(), m_lforces[n], m_speed);
        edgeSumForces += m_lforces[n].lengthSquared();
        clipVector(m_graph.transitionLabel(n).pos_mutable(), clipmin, clipmax);
      }
    }
    
    // we already own the lock so we can directly set stable
    m_graph.stable() = std::sqrt(edgeSumForces) < m_graph.stabilityThreshold()*edgeCount && std::sqrt(nodeSumForces) < m_graph.stabilityThreshold()*nodeCount; 
    if (m_graph.stable()) mCRL2log(mcrl2::log::verbose) << "Graph is now stable." << std::endl;
    m_graph.unlock(GRAPH_LOCK_TRACE); // exit critical section
  }

  // mCRL2log(mcrl2::log::debug) << "Max number of super nodes: " << max_num_nodes << " and average number of super nodes: " << ((double)total_num_nodes)/(nodeCount + 2*edgeCount) << std::endl;
  // mCRL2log(mcrl2::log::debug) << "Tree sizes: " << std::endl;
  // mCRL2log(mcrl2::log::debug) << "\t- nodes  : " << m_node_tree.m_data.size()  << std::endl;
  // mCRL2log(mcrl2::log::debug) << "\t- handles: " << m_handle_tree.m_data.size() << std::endl;
  // mCRL2log(mcrl2::log::debug) << "\t- lables : " << m_trans_tree.m_data.size() << std::endl;
  
  m_max_num_nodes = 0;
  m_total_num_nodes = 0;
}

void SpringLayout::randomizeZ(float z)
{
  m_graph.lock(GRAPH_LOCK_TRACE);
  for (std::size_t n = 0; n < m_graph.nodeCount(); ++n)
  {
    if (!m_graph.node(n).anchored())
    {
      m_graph.node(n).pos_mutable().setZ(m_graph.node(n).pos().z() + fast_frand(-z, z));
    }
  }
  m_graph.unlock(GRAPH_LOCK_TRACE);
}

//
// SpringLayoutUi
//

class WorkerThread : public QThread
{
  private:
    bool m_stopped;
    QElapsedTimer m_time;
    SpringLayout& m_layout;
    int m_period;
  public:
    WorkerThread(SpringLayout& layout, int period, QObject* parent=nullptr)
      : QThread(parent), m_stopped(false), m_layout(layout), m_period(period)
    {
      mCRL2log(mcrl2::log::verbose) << "Created worker thread with a period of " << period << "ms." << std::endl;
    }

    void stop()
    {
      m_stopped = true;
    }

    void setPeriod(int period)
    {
      mCRL2log(mcrl2::log::verbose) << "Worker thread period adjusted to " << period << "ms." << std::endl;
      m_period = period;
    }

    int period() const
    {
      return m_period;
    }

    void run() override
    {
      m_time.start();
      int elapsed;
      while (!m_stopped)
      {
        elapsed = m_time.elapsed();
        int counter = 0;
        while(elapsed < m_period && !m_layout.isStable()){
          m_layout.apply();
          elapsed = m_time.elapsed();
          counter ++;
        }
        if (m_layout.isStable() && elapsed < 2) msleep(40);
        else mCRL2log(mcrl2::log::debug5) << "Worker thread performed " << counter << " cycles in " << elapsed << "ms. " << std::endl;
        if (elapsed > 200) mCRL2log(mcrl2::log::verbose) << "Worker thread took longer than expected; performed " << counter << " cycles in " << elapsed << "ms. " << std::endl;
        m_layout.m_glwidget.update();
        m_time.restart();
      }
    }
};

SpringLayoutUi::SpringLayoutUi(SpringLayout& layout, QWidget* parent)
  : QDockWidget(parent), m_layout(layout), m_thread(nullptr)
{
  m_ui.setupUi(this);
  m_ui.sldAttraction->setValue(m_layout.attraction());
  m_ui.sldRepulsion->setValue(m_layout.repulsion());
  m_ui.sldSpeed->setValue(m_layout.speed());
  m_ui.sldSpeed->setRange(1, 250);
  m_ui.sldHandleWeight->setValue(m_layout.controlPointWeight());
  m_ui.sldNatLength->setValue(m_layout.naturalTransitionLength());
  m_ui.cmbAttractionCalculation->setCurrentIndex(m_layout.attractionCalculation());
  m_ui.cmbRepulsionCalculation->setCurrentIndex(m_layout.repulsionCalculation());
}

SpringLayoutUi::~SpringLayoutUi()
{
  if (m_thread != nullptr)
  {
    dynamic_cast<WorkerThread*>(m_thread)->stop();
    m_thread->wait();
  }
}
/// @brief Notifies layout that rules have changed
void SpringLayoutUi::layoutRulesChanged(){
  m_layout.rulesChanged();
}

QByteArray SpringLayoutUi::settings()
{
  QByteArray result;
  QDataStream out(&result, QIODevice::WriteOnly);

  out << quint32(m_ui.sldAttraction->value()) <<
      quint32(m_ui.sldRepulsion->value()) <<
      quint32(m_ui.sldSpeed->value()) <<
      quint32(m_ui.sldHandleWeight->value()) <<
      quint32(m_ui.sldNatLength->value()) <<
      quint32(m_ui.sldAccuracy->value()) <<
      quint32(m_ui.cmbAttractionCalculation->currentIndex()) <<
      quint32(m_ui.cmbRepulsionCalculation->currentIndex());
  layoutRulesChanged();
  return result;
}

void SpringLayoutUi::setSettings(QByteArray state)
{
  if (state.isEmpty()) {
    return;
  }

  QDataStream in(&state, QIODevice::ReadOnly);

  quint32 attraction, repulsion, speed, handleWeight, NatLength, accuracy, attractionCalculation, repulsionCalculation;
  in >> attraction >> repulsion >> speed >> handleWeight >> NatLength >> accuracy >> attractionCalculation >> repulsionCalculation;

  if (in.status() == QDataStream::Ok)
  {
    m_ui.sldAttraction->setValue(attraction);
    m_ui.sldRepulsion->setValue(repulsion);
    m_ui.sldSpeed->setValue(speed);
    m_ui.sldHandleWeight->setValue(handleWeight);
    m_ui.sldNatLength->setValue(NatLength);
    m_ui.sldAccuracy->setValue(accuracy);
    m_ui.cmbAttractionCalculation->setCurrentIndex(attractionCalculation);
    m_ui.cmbRepulsionCalculation->setCurrentIndex(repulsionCalculation);
  }
  layoutRulesChanged();
}

void SpringLayoutUi::onAttractionChanged(int value)
{
  m_layout.setAttraction(value);
  layoutRulesChanged();
}

void SpringLayoutUi::onRepulsionChanged(int value)
{
  m_layout.setRepulsion(value);
  layoutRulesChanged();
}

void SpringLayoutUi::onSpeedChanged(int value)
{
  if (m_thread != nullptr) {
    dynamic_cast<WorkerThread*>(m_thread)->setPeriod(value);
  }
  m_layout.setSpeed(value);
}

void SpringLayoutUi::onAccuracyChanged(int value)
{
  m_layout.setAccuracy(value);
  layoutRulesChanged();
}

void SpringLayoutUi::onHandleWeightChanged(int value)
{
  m_layout.setControlPointWeight(value);
  layoutRulesChanged();
}

void SpringLayoutUi::onNatLengthChanged(int value)
{
  m_layout.setNaturalTransitionLength(value);
  layoutRulesChanged();
}

void SpringLayoutUi::onAttractionCalculationChanged(int value)
{
  m_layout.setAttractionCalculation(static_cast<SpringLayout::AttractionCalculation>(value));
  mCRL2log(mcrl2::log::debug) << "Attraction calculation changed to: " << value << std::endl;
  layoutRulesChanged();
}

void SpringLayoutUi::onRepulsionCalculationChanged(int value)
{
  m_layout.setRepulsionCalculation(static_cast<SpringLayout::RepulsionCalculation>(value));
  mCRL2log(mcrl2::log::debug) << "Repulsion calculation changed to: " << value << std::endl;
  layoutRulesChanged();
}

void SpringLayoutUi::onStarted()
{
  m_ui.btnStartStop->setText("Stop");
  m_ui.btnStartStop->setEnabled(true);
}

void SpringLayoutUi::onStopped()
{
  m_ui.btnStartStop->setText("Start");
  m_ui.btnStartStop->setEnabled(true);
  runningChanged(false);
}

void SpringLayoutUi::onStartStop()
{
  m_ui.btnStartStop->setEnabled(false);
  if (m_thread == nullptr)
  {
    emit runningChanged(true);
    layoutRulesChanged(); // force update
    m_thread = new WorkerThread(m_layout, m_ui.sldSpeed->value(), this);
    m_thread->connect(m_thread, SIGNAL(started()), this, SLOT(onStarted()));
    m_thread->connect(m_thread, SIGNAL(finished()), this, SLOT(onStopped()));
    m_thread->start();
  }
  else
  {
    dynamic_cast<WorkerThread*>(m_thread)->stop();
    m_thread->wait();
    m_thread = nullptr;
  }
}

void SpringLayoutUi::setActive(bool active)
{
  if (active && m_thread == nullptr) {
    onStartStop();
  }
  else if (!active && m_thread != nullptr) {
    onStartStop();
  }
}

}  // namespace Graph
