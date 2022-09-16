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

#define DEV_DEBUG

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
    m_graph(graph), m_ui(nullptr), m_forceApplication(&SpringLayout::applyForceLTSGraph), m_attractionCalculation(&SpringLayout::forceLTSGraph), m_repulsionCalculation(&SpringLayout::repulsionForceLTSGraph), m_glwidget(glwidget), m_node_tree(0, {0, 0, 0}, {0, 0, 0}), m_trans_tree(0, {0, 0, 0}, {0, 0, 0}), m_handle_tree(0, {0, 0, 0}, {0, 0, 0})
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
      m_repulsionCalculation = &SpringLayout::repulsionForceLTSGraph;
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
  if (m_repulsionCalculation == &SpringLayout::repulsionForceLTSGraph) {
    return ltsgraph_repulsion;
  }
  if (m_repulsionCalculation == &SpringLayout::repulsionForceElectricalModel){
    return electricalsprings_repulsion;
  }

  if (m_repulsionCalculation == &SpringLayout::noRepulsionForce){
    return no_repulsion;
  }

  return ltsgraph_repulsion;
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


QVector3D SpringLayout::repulsionForceLTSGraph(const QVector3D& a, const QVector3D& b, float repulsion, float natlength)
{
  QVector3D diff = a - b;
  float r = repulsion;
  r /= cube((std::max)(diff.length() * 0.5f, natlength * 0.1f));
  diff = diff * r + QVector3D(fast_frand(-0.01f, 0.01f), fast_frand(-0.01f, 0.01f), fast_frand(-0.01f, 0.01f));
  return diff;
}

inline float repulsionConstant(float repulsion, float natlength)
{
  return repulsion * natlength * natlength * adaptiveconstant;
}

QVector3D SpringLayout::repulsionForceElectricalModel(const QVector3D& a, const QVector3D& b, float repulsion, float natlength){
  QVector3D diff = a - b;
  return repulsionConstant(repulsion, natlength) * diff / std::max(diff.lengthSquared(), 0.001f);
}

QVector3D SpringLayout::approxRepulsionForce(const QVector3D& a, Octree& tree, float repulsion, float natlength){
  QVector3D force(0, 0, 0);
  std::size_t num_nodes = 0;
  auto super_nodes = tree.getSuperNodes(a);
  for (auto super_node : super_nodes){
    force += (this->*m_repulsionCalculation)(a, super_node->pos, super_node->children * repulsion, natlength);
  }
  num_nodes = super_nodes.size();

  m_max_num_nodes = std::max(m_max_num_nodes, num_nodes);
  m_total_num_nodes += num_nodes;
  return force;
}

constexpr float consolidation_constant = 0.01f;
void SpringLayout::applyForceLTSGraph(QVector3D& pos, const QVector3D& force, float speed)
{
  pos += force * (speed * consolidation_constant);
}

const std::size_t max_slice = 100;
/// @brief Takes average of at most @c max_slice values, or recursively computes average by splitting
/// @param i Start index
/// @param j End index (exclusive)
static QVector3D slicedAverage(Graph& graph, std::size_t i, std::size_t j){
  std::size_t n = j-i;
  if (n > max_slice){
    // split
    std::size_t m = i + n/2;
    double recip = 1.0/n;
    return (m-i)*recip*slicedAverage(graph, i, m) + (j-m)*recip*slicedAverage(graph, m, j);
  }else{
    double x = 0, y = 0, z = 0;
    for (std::size_t k = i; k < j; ++k){
      x += graph.node(k).pos().x();
      y += graph.node(k).pos().y();
      z += graph.node(k).pos().z();
    }
    return QVector3D(x/n, y/n, z/n);
  }
}

void SpringLayout::apply()
{
  bool sel = m_graph.hasExploration();
  std::size_t nodeCount = sel ? m_graph.explorationNodeCount() : m_graph.nodeCount();
  std::size_t edgeCount = sel ? m_graph.explorationEdgeCount() : m_graph.edgeCount();
  bool use_tree = m_tree_enabled; // This can be updated mid-calculation and then there will be problems
  if (!m_graph.stable())
  {
    m_graph.lock(GRAPH_LOCK_TRACE); // enter critical section

    m_nforces.resize(m_graph.nodeCount()); // Todo: compact this
    m_hforces.resize(m_graph.edgeCount());
    m_lforces.resize(m_graph.edgeCount());
    m_sforces.resize(m_graph.nodeCount());
    /// TODO: Fix blatant code duplication
    if (use_tree){
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

      QVector3D _extents, cubic_extents;
      float _width;

      _extents = node_max - node_min;
      _width = std::max({_extents.x(), _extents.y(), _extents.z()});
      cubic_extents = QVector3D(_width, _width, _width);
      node_min -= (cubic_extents - _extents)*0.5f;
      node_max = node_min + cubic_extents;

      _extents = handle_max - handle_min;
      _width = std::max({_extents.x(), _extents.y(), _extents.z()});
      cubic_extents = QVector3D(_width, _width, _width);
      handle_min -= (cubic_extents - _extents)*0.5f;
      handle_max = handle_min + cubic_extents;

      _extents = trans_max - trans_min;
      _width = std::max({_extents.x(), _extents.y(), _extents.z()});
      cubic_extents = QVector3D(_width, _width, _width);
      trans_min -= (cubic_extents - _extents)*0.5f;
      trans_max = trans_min + cubic_extents;
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
    
    }




    for (std::size_t i = 0; i < nodeCount; ++i)
    {
      std::size_t n = sel ? m_graph.explorationNode(i) : i;

      if(use_tree) m_node_tree.insert(m_graph.node(n).pos());
      m_nforces[n] = {0, 0, 0};
      m_sforces[n] = (this->*m_attractionCalculation)(m_graph.node(n).pos(), m_graph.stateLabel(n).pos(), 0.0);
    }

    m_node_tree.calculatePositions();
    // approx repulsive forces for nodes
    for (std::size_t i = 0; i < nodeCount; ++i){
      std::size_t n = sel ? m_graph.explorationNode(i) : i;
      if(use_tree){
        m_nforces[n] += approxRepulsionForce(m_graph.node(n).pos(), m_node_tree, m_repulsion, m_natLength);
      }else{
        QVector3D f;
        for (std::size_t j = i+1; j < nodeCount; ++j){
          std::size_t m = sel ? m_graph.explorationNode(j) : j;
          f = (this->*m_repulsionCalculation)(m_graph.node(n).pos(), m_graph.node(m).pos(), m_repulsion, m_natLength);
          m_nforces[n] += f;
          m_nforces[m] -= f;
        }
      }
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
        m_hforces[n] += (this->*m_repulsionCalculation)(m_graph.handle(n).pos(), m_graph.node(e.from()).pos(), m_repulsion, m_natLength);
      }

      f = (this->*m_attractionCalculation)(m_graph.node(e.to()).pos(), m_graph.node(e.from()).pos(), m_natLength);
      m_nforces[e.from()] += f;
      m_nforces[e.to()] -= f;

      f = (this->*m_attractionCalculation)((m_graph.node(e.to()).pos() + m_graph.node(e.from()).pos()) / 2.0, m_graph.handle(n).pos(), 0.0);
      m_hforces[n] += f;

      f = (this->*m_attractionCalculation)(m_graph.handle(n).pos(), m_graph.transitionLabel(n).pos(), 0.0);
      m_lforces[n] += f;

      if (use_tree){
        m_handle_tree.insert(m_graph.handle(n).pos());
        m_trans_tree.insert(m_graph.transitionLabel(n).pos());
      }
    }
    if (use_tree){
      m_handle_tree.calculatePositions();
      m_trans_tree.calculatePositions();
    }
    // approximate repulsive forces
    const float repulsion_force_control_point = m_repulsion * m_controlPointWeight;
    for (std::size_t i = 0; i < edgeCount; ++i)
    {
      std::size_t n = sel ? m_graph.explorationEdge(i) : i;
      if (use_tree){
        m_hforces[n] += approxRepulsionForce(m_graph.handle(n).pos(), m_handle_tree, repulsion_force_control_point, m_natLength);
        m_lforces[n] += approxRepulsionForce(m_graph.transitionLabel(n).pos(), m_trans_tree, repulsion_force_control_point, m_natLength);
      }else{
        QVector3D f;
        for (std::size_t j = i+1; j < edgeCount; ++j){
          std::size_t m = sel ? m_graph.explorationNode(j) : j;
          f = (this->*m_repulsionCalculation)(m_graph.handle(n).pos(), m_graph.handle(m).pos(), repulsion_force_control_point, m_natLength);
          m_hforces[n] += f;
          m_hforces[m] -= f;

          f = (this->*m_repulsionCalculation)(m_graph.transitionLabel(n).pos(), m_graph.transitionLabel(m).pos(), repulsion_force_control_point, m_natLength);
          m_lforces[n] += f;
          m_lforces[m] -= f;
        }
      }
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
        (this->*m_forceApplication)(m_graph.node(n).pos_mutable(), m_nforces[n], m_speed);
        nodeSumForces += m_nforces[n].lengthSquared();
        clipVector(m_graph.node(n).pos_mutable(), clipmin, clipmax);
      }
    }

    QVector3D center_of_mass = slicedAverage(m_graph, 0, m_graph.nodeCount());

    for (std::size_t i = 0; i < nodeCount; ++i)
    {
      std::size_t n = sel ? m_graph.explorationNode(i) : i;
      m_graph.node(n).pos_mutable() -= center_of_mass;
    }
  
    for (std::size_t i = 0; i < nodeCount; ++i)
    {
      std::size_t n = sel ? m_graph.explorationNode(i) : i;

      if (!m_graph.stateLabel(n).anchored())
      {
        (this->*m_forceApplication)(m_graph.stateLabel(n).pos_mutable(), m_sforces[n], m_speed);
        m_graph.stateLabel(n).pos_mutable() -= center_of_mass;
        nodeSumForces += m_sforces[n].lengthSquared();
        clipVector(m_graph.stateLabel(n).pos_mutable(), clipmin, clipmax);
      }
    }

    for (std::size_t i = 0; i < edgeCount; ++i)
    {
      std::size_t n = sel ? m_graph.explorationEdge(i) : i;

      if (!m_graph.handle(n).anchored())
      {
        (this->*m_forceApplication)(m_graph.handle(n).pos_mutable(), m_hforces[n], m_speed);
        m_graph.handle(n).pos_mutable() -= center_of_mass;
        edgeSumForces += m_hforces[n].lengthSquared();
        clipVector(m_graph.handle(n).pos_mutable(), clipmin, clipmax);
      }
      if (!m_graph.transitionLabel(n).anchored())
      {
        (this->*m_forceApplication)(m_graph.transitionLabel(n).pos_mutable(), m_lforces[n], m_speed);
        m_graph.transitionLabel(n).pos_mutable() -= center_of_mass;
        edgeSumForces += m_lforces[n].lengthSquared();
        clipVector(m_graph.transitionLabel(n).pos_mutable(), clipmin, clipmax);
      }
    }
    
    // we already own the lock so we can directly set stable
    m_graph.stable() = std::sqrt(edgeSumForces) < m_graph.stabilityThreshold()*edgeCount && std::sqrt(nodeSumForces) < m_graph.stabilityThreshold()*nodeCount; 
    if (m_graph.stable()) mCRL2log(mcrl2::log::verbose) << "Graph is now stable." << std::endl;
    m_graph.unlock(GRAPH_LOCK_TRACE); // exit critical section
  }

  // mCRL2log(mcrl2::log::debug) << "Max number of super nodes: " << m_max_num_nodes << " and average number of super nodes: " << ((double)m_total_num_nodes)/(nodeCount + 2*edgeCount) << std::endl;
  // mCRL2log(mcrl2::log::debug) << "Tree sizes: " << std::endl;
  // mCRL2log(mcrl2::log::debug) << "\t- nodes  : " << m_node_tree.size()  << std::endl;
  // mCRL2log(mcrl2::log::debug) << "\t- handles: " << m_handle_tree.size() << std::endl;
  // mCRL2log(mcrl2::log::debug) << "\t- lables : " << m_trans_tree.size() << std::endl;
  
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

void SpringLayout::setTreeEnabled(bool b){
  m_tree_enabled = b;
}

void SpringLayout::setSpeed(int v){
  m_speed = lerp(v, m_min_speed, m_max_speed);
  mCRL2log(mcrl2::log::verbose) << "Set speed to: " << m_speed << std::endl;
  if (this->m_ui) m_ui->m_ui.dispSpeed->setText(QString::number(m_speed, 'g', 3));
}

void SpringLayout::setAccuracy(int v){
  m_accuracy = lerp(v, m_min_accuracy, m_max_accuracy);
  mCRL2log(mcrl2::log::verbose) << "Set theta to: " << m_accuracy << std::endl;
  if (this->m_ui)
    m_ui->m_ui.dispAccuracy->setText(QString::number(m_accuracy, 'g', 3));
}

void SpringLayout::setAttraction(int v) {
  m_attraction = lerp(v, m_min_attraction, m_max_attraction);
  mCRL2log(mcrl2::log::verbose) << "Set attraction scale to: " << m_attraction << std::endl;
  if (this->m_ui)
    m_ui->m_ui.dispAttraction->setText(QString::number(m_attraction, 'g', 3));
}
void SpringLayout::setRepulsion(int v) {
  float cube = m_natLength * m_natLength * m_natLength;
  m_repulsion = lerp(v, m_min_repulsion*cube, m_max_repulsion*cube);
  mCRL2log(mcrl2::log::verbose) << "Set repulsion scale to: " << m_repulsion << std::endl;
  if (this->m_ui)
    m_ui->m_ui.dispRepulsion->setText(QString::number(m_repulsion, 'g', 3));
}
void SpringLayout::setControlPointWeight(int v) {
  m_controlPointWeight = lerp(v, m_min_controlPointWeight, m_max_controlPointWeight);
  mCRL2log(mcrl2::log::verbose) << "Set control point weight to: " << m_controlPointWeight << std::endl;
  if (this->m_ui)
    m_ui->m_ui.dispHandleWeight->setText(
        QString::number(m_controlPointWeight, 'g', 3));
}
void SpringLayout::setNaturalTransitionLength(int v) {
  m_repulsion /= m_natLength * m_natLength * m_natLength;
  m_natLength = lerp(v, m_min_natLength, m_max_natLength);
  m_repulsion *= m_natLength * m_natLength * m_natLength;
  mCRL2log(mcrl2::log::verbose) << "Set natural length to: " << m_natLength << std::endl;
  if (this->m_ui){
    m_ui->m_ui.dispNatLength->setText(QString::number(m_natLength, 'g', 3));
    m_ui->m_ui.dispRepulsion->setText(QString::number(m_repulsion, 'g', 3));
  }
}

//
// SpringLayoutUi
//

class WorkerThread : public QThread
{
  private:
    bool m_stopped;
    SpringLayout& m_layout;
    int m_counter;
#ifdef DEV_DEBUG
    // By default log ever second
    const int m_debug_log_interval = 1000;
    // if we have a cycle time longer than 100ms we want an extra message
    const int m_debug_max_cycle_time = 100;
    QElapsedTimer m_debug_log_timer;
#endif
  public:
    WorkerThread(SpringLayout& layout, QObject* parent=nullptr)
      : QThread(parent), m_stopped(false), m_layout(layout)
    {
#ifdef DEV_DEBUG
      mCRL2log(mcrl2::log::debug) << "Workerthread will output debug messages to this stream." << std::endl;
      m_debug_log_timer.start();
#endif
    }

    void stop()
    {
      m_stopped = true;
    }

    void run() override
    {
      if (m_layout.isStable()) msleep(50);// We don't want to keep looping if the layout is stable
      while (!m_stopped && !m_layout.isStable())
      {
        m_layout.apply();
        m_counter ++;
        debugLogging();
      }
    }

#ifdef DEV_DEBUG
    /// @brief Only called when a debug configuration is ran. 
    void debugLogging(){
      int elapsed = m_debug_log_timer.elapsed();
      if (elapsed > m_debug_log_interval){
        mCRL2log(mcrl2::log::debug) << "Worker thread performed " << m_counter << " cycles in " << elapsed << "ms. ";
        if (m_counter/(float)elapsed > 50) mCRL2log(mcrl2::log::debug) << " - NB: This is longer than the set expected maximum " << m_debug_max_cycle_time << "ms per cycle. ";
        mCRL2log(mcrl2::log::debug) << std::endl;
        // reset debugging
        m_debug_log_timer.restart();
        m_counter = 0;
      }
    }
#else
    // This structure is required to be optimised out by every "Release" flagged compiler at least, and most compilers will optimise always
    void debugLogging(){
      do {} while(0);
    }
#endif
};

SpringLayoutUi::SpringLayoutUi(SpringLayout& layout, QWidget* parent)
  : QDockWidget(parent), m_layout(layout), m_thread(nullptr)
{
  m_ui.setupUi(this);
  m_ui.sldAttraction->setValue(m_layout.attraction());
  m_ui.sldRepulsion->setValue(m_layout.repulsion());
  m_ui.sldSpeed->setValue(m_layout.speed());
  m_ui.sldHandleWeight->setValue(m_layout.controlPointWeight());
  m_ui.sldNatLength->setValue(m_layout.naturalTransitionLength());
  m_ui.cmbAttractionCalculation->setCurrentIndex(m_layout.attractionCalculation());
  m_ui.cmbRepulsionCalculation->setCurrentIndex(m_layout.repulsionCalculation());
  m_layout.setTreeEnabled(m_ui.chkEnableTree->isChecked());
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

void SpringLayoutUi::onTreeToggled(bool b){
  m_layout.setTreeEnabled(b);
}

void SpringLayoutUi::onStartStop()
{
  m_ui.btnStartStop->setEnabled(false);
  if (m_thread == nullptr)
  {
    emit runningChanged(true);
    layoutRulesChanged(); // force update
    m_thread = new WorkerThread(m_layout, this);
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
