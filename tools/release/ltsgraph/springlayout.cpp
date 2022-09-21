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
#ifdef DEV_DEBUG
// #define LOG_FORCE_MAGNITUDE
// #define LOG_FUNCTION_SETTER
#endif


namespace Graph
{

//
// Debug functions
//

#ifdef DEV_DEBUG
std::string getName(SpringLayout::AttractionCalculation c){
  std::string s = "UNKNOWN";
  #define PROC_VAL(p) case(p): s = #p; break;
  switch(c){
    PROC_VAL(SpringLayout::AttractionCalculation::ltsgraph_attr);
    PROC_VAL(SpringLayout::AttractionCalculation::linearsprings_attr);
    PROC_VAL(SpringLayout::AttractionCalculation::electricalsprings_attr);
  }
  #undef PROC_VAL
  return s;
}
std::string getName(SpringLayout::RepulsionCalculation c){
  std::string s = "UNKNOWN";
  #define PROC_VAL(p) case(p): s = #p; break;
  switch(c){
    PROC_VAL(SpringLayout::RepulsionCalculation::ltsgraph_rep);
    PROC_VAL(SpringLayout::RepulsionCalculation::none_rep);
    PROC_VAL(SpringLayout::RepulsionCalculation::electricalsprings_rep);
  }
  #undef PROC_VAL
  return s;
}
std::string getName(SpringLayout::ForceApplication c){
  std::string s = "UNKNOWN";
  #define PROC_VAL(p) case(p): s = #p; break;
  switch(c){
    PROC_VAL(SpringLayout::ForceApplication::ltsgraph_appl);
    PROC_VAL(SpringLayout::ForceApplication::force_directed_appl);
    PROC_VAL(SpringLayout::ForceApplication::force_directed_annealing_appl);
    PROC_VAL(SpringLayout::ForceApplication::force_cumulative_appl);
  }
  #undef PROC_VAL
  return s;
}
#endif

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

const float electricalSpringScaling = 1e-2f;


struct AttractionFunction{
  virtual QVector3D operator ()(const QVector3D& a, const QVector3D& b, const float ideal) = 0;
  void update(){};
  void reset(){};
};
namespace AttractionFunctions{
struct LTSGraph : AttractionFunction{
  QVector3D diff = {0, 0, 0};
  const float scaling = 1e3f;
  float dist = 0.0f;
  float factor = 0.0f;
  QVector3D operator ()(const QVector3D& a, const QVector3D& b, const float ideal) override{
    diff = (a - b);
    dist = (std::max)(diff.length(), 1.0f);
    factor = scaling * std::log(dist / (ideal + 1.0f)) / dist;
    return diff * factor;
  } 
};

struct LinearSprings : AttractionFunction{
  const float spring_constant = 1e-4f;
  const float scaling = 1.0f/10000;
  QVector3D diff = {0, 0, 0};
  float dist = 0.0f;
  float factor = 0.0f;
  QVector3D operator ()(const QVector3D& a, const QVector3D& b, const float ideal) override{
    diff = (a-b);
    dist = diff.length() - ideal;
    factor = spring_constant * std::max(dist, 0.0f);
    if (dist > 0.0f)
    {
      factor = std::max(factor, 100 / (std::max)(dist * dist / 10000.0f, 0.1f));
    }
    return diff * (factor * scaling);
  }
};

struct ElectricalSprings : AttractionFunction{
  QVector3D diff = {0, 0, 0};
  QVector3D operator ()(const QVector3D& a, const QVector3D& b, const float ideal) override{
    QVector3D diff = (a-b);
    return (electricalSpringScaling * diff.length() / std::max(0.01f, ideal)) * diff;
  }
};

struct SimpleSpring : AttractionFunction{
  QVector3D diff = {0, 0, 0};
  const float spring_constant = 1e-4f;
  QVector3D operator ()(const QVector3D& a, const QVector3D& b, const float ideal) override{
    diff = a-b;
    return spring_constant * (diff.length() - ideal) * diff;
  }
};
};

struct RepulsionFunction{
  virtual QVector3D operator ()(const QVector3D& a, const QVector3D& b, const float ideal) = 0;
  virtual void update(){};
  virtual void reset(){};
};

namespace RepulsionFunctions{
  struct LTSGraph : RepulsionFunction{
    QVector3D diff;
    float r;
    QVector3D operator()(const QVector3D& a, const QVector3D& b, const float natlength) override
    {
      diff = a - b;
      r = cube(natlength);
      r /= cube((std::max)(diff.length() * 0.5f, natlength * 0.1f));
      diff = diff * r + QVector3D(fast_frand(-0.01f, 0.01f), fast_frand(-0.01f, 0.01f), fast_frand(-0.01f, 0.01f));
      return diff;
    }
  };

  struct ElectricalSpring : RepulsionFunction{
    QVector3D diff;
    QVector3D operator()(const QVector3D& a, const QVector3D& b, const float K) override{
      diff = a - b;
      return ((electricalSpringScaling * K * K) / std::max(diff.lengthSquared(), 1.0f)) * diff;
    }
  };

  struct None : RepulsionFunction{
    QVector3D ZERO = {0, 0, 0};
    QVector3D operator()(const QVector3D& a, const QVector3D& b, const float natlength) override{
      return ZERO;
    }
  };
};

struct ApplicationFunction{
  virtual void operator() (QVector3D& pos, const QVector3D& f, const float speed) = 0;
  virtual void update(){};
  virtual void reset(){};
};

namespace ApplicationFunctions{
  struct LTSGraph : ApplicationFunction{
    const float scaling = 0.01f;
    const float limit = 1e4f;
    void operator() (QVector3D& pos, const QVector3D& f, const float speed) override {
      if (scaling*std::abs(f.x()) >= limit || scaling*std::abs(f.y()) >= limit || scaling*std::abs(f.z()) >= limit) return;
      pos += f * (speed * scaling);
    }
  };

  struct ForceDirected : ApplicationFunction{
    const float scaling = 2.0f;
    void operator() (QVector3D& pos, const QVector3D& f, const float speed) override {
      pos += (speed * scaling) * f.normalized();
    }
  };

  struct ForceDirectedAnnealing : ApplicationFunction{
    const float scaling = 2.0f;
    // Annealing parameters
    const int anneal_iterations = 1;
    const float anneal_cooling_factor = 0.98f;
    const float anneal_start_temperature = 100.0f;
    // Annealing variables
    float m_anneal_temperature = 1.0f;
    int iterations = 0;
    void operator() (QVector3D& pos, const QVector3D& f, const float speed) override {
      pos += (speed * scaling * m_anneal_temperature) * f.normalized();
    }
    void update() override {
      if( ++ iterations >= anneal_iterations){
        m_anneal_temperature *= anneal_cooling_factor;
        iterations = 0;
      }
    }
    void reset() override {
      m_anneal_temperature = anneal_start_temperature;
    }
  };

  struct ForceCumulative : ApplicationFunction{
    ForceCumulative(){
      mCRL2log(mcrl2::log::warning) << "Cumulative force not impemented yet." << std::endl;
    }
    void operator() (QVector3D& pos, const QVector3D& f, const float speed) override {
      pos += (speed) * f.normalized();
    }
  };
};
//
// SpringLayout
//

SpringLayout::SpringLayout(Graph& graph, GLWidget& glwidget)
    : m_speed(0.001f), m_attraction(0.13f), m_repulsion(50.0f),
      m_natLength(50.0f), m_controlPointWeight(0.001f), m_graph(graph),
      m_ui(nullptr), m_glwidget(glwidget), m_node_tree(0, {0, 0, 0}, {0, 0, 0}),
      m_trans_tree(0, {0, 0, 0}, {0, 0, 0}),
      m_handle_tree(0, {0, 0, 0}, {0, 0, 0}), m_node_tree2D(0, {0, 0}, {0, 0}),
      m_trans_tree2D(0, {0, 0}, {0, 0}), m_handle_tree2D(0, {0, 0}, {0, 0}),
      attrFuncMap({
          {AttractionCalculation::ltsgraph_attr,
           new AttractionFunctions::LTSGraph()},
          {AttractionCalculation::linearsprings_attr,
           new AttractionFunctions::LinearSprings()},
          {AttractionCalculation::electricalsprings_attr,
           new AttractionFunctions::ElectricalSprings()},
          {AttractionCalculation::simplespring_attr,
           new AttractionFunctions::SimpleSpring()},
      }),
      m_attrFunc(attrFuncMap[AttractionCalculation::ltsgraph_attr]),
      m_option_attractionCalculation(AttractionCalculation::ltsgraph_attr),
      repFuncMap({
          {RepulsionCalculation::ltsgraph_rep,
           new RepulsionFunctions::LTSGraph()},
          {RepulsionCalculation::electricalsprings_rep,
           new RepulsionFunctions::ElectricalSpring()},
          {RepulsionCalculation::none_rep, new RepulsionFunctions::None()},
      }),
      m_repFunc(repFuncMap[RepulsionCalculation::ltsgraph_rep]),
      m_option_repulsionCalculation(RepulsionCalculation::ltsgraph_rep),
      applFuncMap({
          {ForceApplication::ltsgraph_appl,
           new ApplicationFunctions::LTSGraph()},
          {ForceApplication::force_directed_appl,
           new ApplicationFunctions::ForceDirected()},
          {ForceApplication::force_directed_annealing_appl,
           new ApplicationFunctions::ForceDirectedAnnealing()},
           {ForceApplication::force_cumulative_appl,
           new ApplicationFunctions::ForceCumulative()},
      }),
      m_applFunc(applFuncMap[ForceApplication::ltsgraph_appl]),
      m_option_forceApplication(ForceApplication::ltsgraph_appl)

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
    m_ui->m_ui.dispSpeed->setText(QString::number(m_speed, 'g', 3));
    m_ui->m_ui.dispAttraction->setText(QString::number(m_attraction, 'g', 3));
    m_ui->m_ui.dispRepulsion->setText(QString::number(m_repulsion, 'g', 3));
    m_ui->m_ui.dispAccuracy->setText(QString::number(m_accuracy, 'g', 3));
    m_ui->m_ui.dispHandleWeight->setText(QString::number(m_controlPointWeight, 'g', 3));
    m_ui->m_ui.dispNatLength->setText(QString::number(m_natLength, 'g', 3));
  }
  return m_ui;
}

void SpringLayout::setAttractionCalculation(AttractionCalculation c)
{
  m_option_attractionCalculation = c;
  m_attrFunc = attrFuncMap[c];
  m_attrFunc->reset();
  #ifdef LOG_FUNCTION_SETTER
    mCRL2log(mcrl2::log::debug) << "Attraction set to: " << getName(c) << std::endl;
    mCRL2log(mcrl2::log::debug) << "Attr func: " << typeid(*m_attrFunc).name() << std::endl;
  #endif
}

SpringLayout::AttractionCalculation SpringLayout::attractionCalculation()
{
  return m_option_attractionCalculation;
}

void SpringLayout::setRepulsionCalculation(RepulsionCalculation c)
{
  m_option_repulsionCalculation = c;
  m_repFunc = repFuncMap[c];
  m_repFunc->reset();
  #ifdef LOG_FUNCTION_SETTER
    mCRL2log(mcrl2::log::debug) << "Repulsion set to: " << getName(c) << std::endl;
    mCRL2log(mcrl2::log::debug) << "Rep func: " <<  typeid(*m_repFunc) .name() << std::endl;
  #endif
}

SpringLayout::RepulsionCalculation SpringLayout::repulsionCalculation()
{
  return m_option_repulsionCalculation;
}

void SpringLayout::setForceApplication(SpringLayout::ForceApplication c){
  m_option_forceApplication = c;
  m_applFunc = applFuncMap[c];
  m_applFunc->reset();
  #ifdef LOG_FUNCTION_SETTER
    mCRL2log(mcrl2::log::debug) << "Application set to: " << getName(c) << std::endl;
    mCRL2log(mcrl2::log::debug) << "Appl func: " << typeid(*m_applFunc).name() << std::endl;
  #endif
}

SpringLayout::ForceApplication SpringLayout::forceApplication(){
  return m_option_forceApplication;
}

template<>
QVector3D SpringLayout::approxRepulsionForce<Octree>(const QVector3D& a, Octree& tree){
  QVector3D force(0, 0, 0);
  std::size_t num_nodes = 0;
  auto& super_nodes = tree.getSuperNodes(a);
  for (auto super_node : super_nodes){
    force += super_node->children * (*m_repFunc)(a, super_node->pos, m_natLength);
  }
  force *= m_repulsion;
  num_nodes = super_nodes.size();

  m_max_num_nodes = std::max(m_max_num_nodes, num_nodes);
  m_total_num_nodes += num_nodes;
  return force;
}

template<>
QVector3D SpringLayout::approxRepulsionForce<Quadtree>(const QVector3D& a, Quadtree& tree){
  QVector3D force(0, 0, 0);
  std::size_t num_nodes = 0;
  auto& super_nodes = tree.getSuperNodes({a.x(), a.y()});
  for (auto super_node : super_nodes){
    force += super_node->children * (*m_repFunc)(a, {super_node->pos.x(), super_node->pos.y(), 0}, m_natLength);
  }
  force *= m_repulsion;
  num_nodes = super_nodes.size();

  m_max_num_nodes = std::max(m_max_num_nodes, num_nodes);
  m_total_num_nodes += num_nodes;
  return force;
}



const std::size_t max_slice = 50;
/// @brief Takes average of at most @c max_slice values, or recursively computes average by splitting
/// @param i Start index
/// @param j End index (exclusive)
/// This method avoids precision loss due to adding up too much before division
///  Downside: more divisions. Upside: more accuracy.
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

int iterations = 0;


template<>
void SpringLayout::attractionAccumulation<SpringLayout::ThreadingMode::normal>(bool sel, std::size_t nodeCount, std::size_t edgeCount){
  
  for (std::size_t i = 0; i < nodeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationNode(i) : i;
    m_nforces[n] = {0, 0, 0};
    m_sforces[n] = (*m_attrFunc)(m_graph.node(n).pos(), m_graph.stateLabel(n).pos(), 0.0) * m_attraction;
  }

  QVector3D f;
  for (std::size_t i = 0; i < edgeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationEdge(i) : i;

    Edge e = m_graph.edge(n);
    // Variables for repulsion calculations

    m_hforces[n] = QVector3D(0, 0, 0);
    m_lforces[n] = QVector3D(0, 0, 0);

    if (e.is_selfloop())
    {
      m_hforces[n] += (*m_repFunc)(m_graph.handle(n).pos(), m_graph.node(e.from()).pos(), m_natLength) * m_repulsion;
    }

    f = (*m_attrFunc)(m_graph.node(e.to()).pos(), m_graph.node(e.from()).pos(), m_natLength) * m_attraction;
    m_nforces[e.from()] += f;
    m_nforces[e.to()] -= f;

    f = (*m_attrFunc)((m_graph.node(e.to()).pos() + m_graph.node(e.from()).pos()) / 2.0, m_graph.handle(n).pos(), 0.0) * m_attraction;
    m_hforces[n] += f;

    f = (*m_attrFunc)(m_graph.handle(n).pos(), m_graph.transitionLabel(n).pos(), 0.0) * m_attraction;
    m_lforces[n] += f;

  }
}

template<>
void SpringLayout::repulsionAccumulation<SpringLayout::TreeMode::quadtree>(bool sel, std::size_t nodeCount, std::size_t edgeCount){
  /// TODO: Fix blatant code duplication
  QVector2D node_min   = { INFINITY,  INFINITY}, 
            node_max   = {-INFINITY, -INFINITY}, 
            handle_min = { INFINITY,  INFINITY}, 
            handle_max = {-INFINITY, -INFINITY},
            trans_min  = { INFINITY,  INFINITY},
            trans_max  = {-INFINITY, -INFINITY};
  
  // calculate bounding volumes
  {
    for (int i = 0; i < nodeCount; i++)
    {
      std::size_t n = sel ? m_graph.explorationNode(i) : i;
      node_min.setX(std::min(node_min.x(), m_graph.node(n).pos().x())); 
      node_min.setY(std::min(node_min.y(), m_graph.node(n).pos().y())); 
      node_max.setX(std::max(node_max.x(), m_graph.node(n).pos().x())); 
      node_max.setY(std::max(node_max.y(), m_graph.node(n).pos().y())); 
    }

    // calculate bounding volume
    for (int i = 0; i < edgeCount; i++)
    {
      std::size_t n = sel ? m_graph.explorationEdge(i) : i;
      handle_min.setX(std::min(handle_min.x(), m_graph.handle(n).pos().x())); 
      handle_min.setY(std::min(handle_min.y(), m_graph.handle(n).pos().y())); 
      handle_max.setX(std::max(handle_max.x(), m_graph.handle(n).pos().x())); 
      handle_max.setY(std::max(handle_max.y(), m_graph.handle(n).pos().y())); 
    }

    // calculate bounding volume
    for (int i = 0; i < edgeCount; i++)
    {
      std::size_t n = sel ? m_graph.explorationEdge(i) : i;
      trans_min.setX(std::min(trans_min.x(), m_graph.transitionLabel(n).pos().x())); 
      trans_min.setY(std::min(trans_min.y(), m_graph.transitionLabel(n).pos().y())); 
      trans_max.setX(std::max(trans_max.x(), m_graph.transitionLabel(n).pos().x())); 
      trans_max.setY(std::max(trans_max.y(), m_graph.transitionLabel(n).pos().y())); 
    }
  }
  node_min -= QVector2D(1, 1);
  node_max += QVector2D(1, 1);
  handle_min -= QVector2D(1, 1);
  handle_max += QVector2D(1, 1);
  trans_min -= QVector2D(1, 1);
  trans_max += QVector2D(1, 1);

  QVector2D _extents, cubic_extents;
  float _width;

  _extents = node_max - node_min;
  _width = std::max({_extents.x(), _extents.y()});
  cubic_extents = QVector2D(_width, _width);
  node_min -= (cubic_extents - _extents)*0.5f;
  node_max = node_min + cubic_extents;

  _extents = handle_max - handle_min;
  _width = std::max({_extents.x(), _extents.y()});
  cubic_extents = QVector2D(_width, _width);
  handle_min -= (cubic_extents - _extents)*0.5f;
  handle_max = handle_min + cubic_extents;

  _extents = trans_max - trans_min;
  _width = std::max({_extents.x(), _extents.y()});
  cubic_extents = QVector2D(_width, _width);
  trans_min -= (cubic_extents - _extents)*0.5f;
  trans_max = trans_min + cubic_extents;
  
  m_node_tree2D.setMinBounds(node_min);
  m_node_tree2D.setMaxBounds(node_max);

  m_handle_tree2D.setMinBounds(handle_min);
  m_handle_tree2D.setMaxBounds(handle_max);

  m_trans_tree2D.setMinBounds(trans_min);
  m_trans_tree2D.setMaxBounds(trans_max);

  m_node_tree2D.setTheta(m_accuracy);
  m_handle_tree2D.setTheta(m_accuracy);
  m_trans_tree2D.setTheta(m_accuracy);

  m_node_tree2D.reset();
  m_handle_tree2D.reset();
  m_trans_tree2D.reset();
  
  for (std::size_t i = 0; i < nodeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationNode(i) : i;

    m_node_tree2D.insert((QVector2D)m_graph.node(n).pos());
  }

  m_node_tree2D.calculatePositions();

  // approx repulsive forces for nodes
  for (std::size_t i = 0; i < nodeCount; ++i){
    std::size_t n = sel ? m_graph.explorationNode(i) : i;
    m_nforces[n] += approxRepulsionForce<Quadtree>(m_graph.node(n).pos(), m_node_tree2D);
  }

  for (std::size_t i = 0; i < edgeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationEdge(i) : i;

    Edge e = m_graph.edge(n);
    m_handle_tree2D.insert((QVector2D)m_graph.handle(n).pos());
    m_trans_tree2D.insert((QVector2D)m_graph.transitionLabel(n).pos());
  }
  
  m_handle_tree2D.calculatePositions();
  m_trans_tree2D.calculatePositions();
  
  // approximate repulsive forces
  float temp = m_repulsion;
  m_repulsion *= m_controlPointWeight;
  for (std::size_t i = 0; i < edgeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationEdge(i) : i;
    m_hforces[n] += approxRepulsionForce<Quadtree>(m_graph.handle(n).pos(), m_handle_tree2D);
    m_lforces[n] += approxRepulsionForce<Quadtree>(m_graph.transitionLabel(n).pos(), m_trans_tree2D);
  }
  m_repulsion = temp;
}

template<>
void SpringLayout::repulsionAccumulation<SpringLayout::TreeMode::octree>(bool sel, std::size_t nodeCount, std::size_t edgeCount){
  /// TODO: Fix blatant code duplication
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
  
  
  for (std::size_t i = 0; i < nodeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationNode(i) : i;

    m_node_tree.insert(m_graph.node(n).pos());
  }

  m_node_tree.calculatePositions();

  // approx repulsive forces for nodes
  for (std::size_t i = 0; i < nodeCount; ++i){
    std::size_t n = sel ? m_graph.explorationNode(i) : i;
    m_nforces[n] += approxRepulsionForce<Octree>(m_graph.node(n).pos(), m_node_tree);
  }

  for (std::size_t i = 0; i < edgeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationEdge(i) : i;

    Edge e = m_graph.edge(n);
    m_handle_tree.insert(m_graph.handle(n).pos());
    m_trans_tree.insert(m_graph.transitionLabel(n).pos());

  }
  
  m_handle_tree.calculatePositions();
  m_trans_tree.calculatePositions();
  
  // approximate repulsive forces
  const float temp = m_repulsion;
  m_repulsion *= m_controlPointWeight;
  for (std::size_t i = 0; i < edgeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationEdge(i) : i;
    m_hforces[n] += approxRepulsionForce<Octree>(m_graph.handle(n).pos(), m_handle_tree);
    m_lforces[n] += approxRepulsionForce<Octree>(m_graph.transitionLabel(n).pos(), m_trans_tree);
  }
  m_repulsion = temp;
}

template<>
void SpringLayout::repulsionAccumulation<SpringLayout::TreeMode::none>(bool sel, std::size_t nodeCount, std::size_t edgeCount){
  // used for storing intermediate results
  QVector3D f;
  // repulsive forces for nodes
  for (std::size_t i = 0; i < nodeCount; ++i){
    std::size_t n = sel ? m_graph.explorationNode(i) : i;
    for (std::size_t j = i+1; j < nodeCount; ++j){
      std::size_t m = sel ? m_graph.explorationNode(j) : j;
      f = (*m_repFunc)(m_graph.node(n).pos(), m_graph.node(m).pos(), m_natLength) * m_repulsion;
      m_nforces[n] += f;
      m_nforces[m] -= f;
    }
  }

  // repulsive forces
  const float repulsion_force_control_point = m_repulsion * m_controlPointWeight;
  for (std::size_t i = 0; i < edgeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationEdge(i) : i;
    for (std::size_t j = i+1; j < edgeCount; ++j){
      std::size_t m = sel ? m_graph.explorationNode(j) : j;
      f = (*m_repFunc)(m_graph.handle(n).pos(), m_graph.handle(m).pos(), m_natLength) * repulsion_force_control_point;
      m_hforces[n] += f;
      m_hforces[m] -= f;

      f = (*m_repFunc)(m_graph.transitionLabel(n).pos(), m_graph.transitionLabel(m).pos(), m_natLength) * repulsion_force_control_point;
      m_lforces[n] += f;
      m_lforces[m] -= f;
    }
  }
}

void SpringLayout::forceAccumulation(bool sel, std::size_t nodeCount, std::size_t edgeCount, TreeMode treeMode, ThreadingMode threadingMode){
  switch(threadingMode){
    case ThreadingMode::normal:
      attractionAccumulation<ThreadingMode::normal>(sel, nodeCount, edgeCount);
      break;
  }
  if (m_option_repulsionCalculation != RepulsionCalculation::none_rep){
    switch(treeMode){
      case TreeMode::none:
        repulsionAccumulation<TreeMode::none>(sel, nodeCount, edgeCount);
        break;
      case TreeMode::quadtree:
        repulsionAccumulation<TreeMode::quadtree>(sel, nodeCount, edgeCount);
        break;
      case TreeMode::octree:
        repulsionAccumulation<TreeMode::octree>(sel, nodeCount, edgeCount);
        break;
    }
  }
}

void SpringLayout::apply()
{  
  assert(m_attrFunc);
  assert(m_repFunc);
  assert(m_attrFunc);
  if (!m_graph.stable())
  {

    bool sel = m_graph.hasExploration();
    std::size_t nodeCount = sel ? m_graph.explorationNodeCount() : m_graph.nodeCount();
    std::size_t edgeCount = sel ? m_graph.explorationEdgeCount() : m_graph.edgeCount();

    m_graph.lock(GRAPH_LOCK_TRACE); // enter critical section

    m_nforces.resize(m_graph.nodeCount()); // Todo: compact this
    m_hforces.resize(m_graph.edgeCount());
    m_lforces.resize(m_graph.edgeCount());
    m_sforces.resize(m_graph.nodeCount());
    if (m_tree_enabled){
      bool is_2D = (m_graph.getClipMax().z() - m_graph.getClipMin().z() < 0.001f);
      if (is_2D){
        forceAccumulation(sel, nodeCount, edgeCount, TreeMode::quadtree, ThreadingMode::normal);
      }else{
        forceAccumulation(sel, nodeCount, edgeCount, TreeMode::octree, ThreadingMode::normal);
      }
    }else{
      forceAccumulation(sel, nodeCount, edgeCount, TreeMode::none, ThreadingMode::normal);
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
        (*m_applFunc)(m_graph.node(n).pos_mutable(), m_nforces[n], m_speed);
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
        (*m_applFunc)(m_graph.stateLabel(n).pos_mutable(), m_sforces[n], m_speed);
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
        (*m_applFunc)(m_graph.handle(n).pos_mutable(), m_hforces[n], m_speed);
        m_graph.handle(n).pos_mutable() -= center_of_mass;
        edgeSumForces += m_hforces[n].lengthSquared();
        clipVector(m_graph.handle(n).pos_mutable(), clipmin, clipmax);
      }
      if (!m_graph.transitionLabel(n).anchored())
      {
        (*m_applFunc)(m_graph.transitionLabel(n).pos_mutable(), m_lforces[n], m_speed);
        m_graph.transitionLabel(n).pos_mutable() -= center_of_mass;
        edgeSumForces += m_lforces[n].lengthSquared();
        clipVector(m_graph.transitionLabel(n).pos_mutable(), clipmin, clipmax);
      }
    }
    
    // we already own the lock so we can directly set stable
    m_graph.stable() = std::sqrt(edgeSumForces) < m_graph.stabilityThreshold()*edgeCount && std::sqrt(nodeSumForces) < m_graph.stabilityThreshold()*nodeCount; 
    if (m_graph.stable()) mCRL2log(mcrl2::log::verbose) << "Graph is now stable." << std::endl;


    m_graph.unlock(GRAPH_LOCK_TRACE);

    m_max_num_nodes = 0;
    m_total_num_nodes = 0;

    m_applFunc->update();
    m_repFunc->update();
    m_attrFunc->update();

    notifyNewFrame();

    #ifdef LOG_FORCE_MAGNITUDE
    float maxForce = 0.0f;
    for (int i = 0; i < nodeCount; i++){
      std::size_t n = sel ? m_graph.explorationNode(i) : i;
      maxForce = std::max({m_sforces[n].length(), m_nforces[n].length(), maxForce});
    }
    for (int i = 0; i < edgeCount; i++){
      std::size_t n = sel ? m_graph.explorationEdge(i) : i;
      maxForce = std::max({m_hforces[n].length(), m_lforces[n].length(), maxForce});
    }
    mCRL2log(mcrl2::log::debug) << "[DEBUG] Force magnitude max: " << maxForce << std::endl;
    #endif
  }
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

void SpringLayout::notifyNewFrame(){
  m_has_new_frame = true;
  m_graph.hasNewFrame(true);
}

void SpringLayout::setTreeEnabled(bool b){
  m_tree_enabled = b;
  mCRL2log(mcrl2::log::verbose) << (b ? "Enabled" : "Disabled") << " tree acceleration." << std::endl;
}

void SpringLayout::setSpeed(int v){
  m_speed = lerp(v, m_min_speed, m_max_speed);
  mCRL2log(mcrl2::log::verbose) << "Set speed to: " << v << " corresponding to: " <<  m_speed << std::endl;
  if (this->m_ui) m_ui->m_ui.dispSpeed->setText(QString::number(m_speed, 'g', 3));
}

void SpringLayout::setAccuracy(int v){
  m_accuracy = lerp(v, m_min_accuracy, m_max_accuracy);
  mCRL2log(mcrl2::log::verbose) << "Set accuracy to: " << v << " corresponding to: " <<  m_accuracy << std::endl;
  if (this->m_ui)
    m_ui->m_ui.dispAccuracy->setText(QString::number(m_accuracy, 'g', 3));
}

void SpringLayout::setAttraction(int v) {
  m_attraction = lerp(v, m_min_attraction, m_max_attraction);
  mCRL2log(mcrl2::log::verbose) << "Set attraction scale to: " << v << " corresponding to: " << m_attraction << std::endl;
  if (this->m_ui)
    m_ui->m_ui.dispAttraction->setText(QString::number(m_attraction, 'g', 3));
}

void SpringLayout::setRepulsion(int v) {
  m_repulsion = lerp(v, m_min_repulsion, m_max_repulsion);
  mCRL2log(mcrl2::log::verbose) << "Set repulsion scale to: " << v << " corresponding to: " <<  m_repulsion << std::endl;
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
  m_natLength = lerp(v, m_min_natLength, m_max_natLength);
  mCRL2log(mcrl2::log::verbose) << "Set natural length to: " << v << " corresponding to: " <<  m_natLength << std::endl;
  if (this->m_ui){
    m_ui->m_ui.dispNatLength->setText(QString::number(m_natLength, 'g', 3));
  }
}

void SpringLayout::rulesChanged(){
   m_graph.setStable(false);
   setForceApplication(m_option_forceApplication);
   m_applFunc->reset();
   m_repFunc->reset();
   m_attrFunc->reset();
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
  m_ui.cmbForceApplication->setCurrentIndex(m_layout.forceApplication());
  m_layout.setTreeEnabled(m_ui.chkEnableTree->isChecked());
  m_ui.chkEnableTree->setChecked(false);
  onTreeToggled(false);

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
      quint32(m_ui.cmbRepulsionCalculation->currentIndex()) <<
      quint32(m_ui.cmbForceApplication->currentIndex()) <<
      quint32(m_ui.chkEnableTree->isChecked());
  layoutRulesChanged();
  return result;
}

void SpringLayoutUi::setSettings(QByteArray state)
{
  if (state.isEmpty()) {
    return;
  }

  QDataStream in(&state, QIODevice::ReadOnly);

  quint32 attraction, repulsion, speed, handleWeight, natLength, accuracy, attractionCalculation, repulsionCalculation, forceCalculation, treeEnabled;
  in >> attraction >> repulsion >> speed >> handleWeight >> natLength >> accuracy >> attractionCalculation >> repulsionCalculation >> forceCalculation >> treeEnabled;

  if (in.status() == QDataStream::Ok)
  {
    m_ui.sldAttraction->setValue(attraction);
    m_ui.sldRepulsion->setValue(repulsion);
    m_ui.sldSpeed->setValue(speed);
    m_ui.sldHandleWeight->setValue(handleWeight);
    m_ui.sldNatLength->setValue(natLength);
    m_ui.sldAccuracy->setValue(accuracy);
    m_ui.cmbAttractionCalculation->setCurrentIndex(attractionCalculation);
    m_ui.cmbRepulsionCalculation->setCurrentIndex(repulsionCalculation);
    m_ui.cmbForceApplication->setCurrentIndex(forceCalculation);
    m_ui.chkEnableTree->setChecked((bool)treeEnabled);
  }
  layoutRulesChanged();
}

void SpringLayoutUi::onAttractionChanged(int value)
{
  m_layout.setAttraction(value);
  layoutRulesChanged();
  update();
}

void SpringLayoutUi::onRepulsionChanged(int value)
{
  m_layout.setRepulsion(value);
  layoutRulesChanged();
  update();
}

void SpringLayoutUi::onSpeedChanged(int value)
{
  m_layout.setSpeed(value);
  update();
}

void SpringLayoutUi::onAccuracyChanged(int value)
{
  m_layout.setAccuracy(value);
  layoutRulesChanged();
  update();
}

void SpringLayoutUi::onHandleWeightChanged(int value)
{
  m_layout.setControlPointWeight(value);
  layoutRulesChanged();
  update();
}

void SpringLayoutUi::onNatLengthChanged(int value)
{
  m_layout.setNaturalTransitionLength(value);
  layoutRulesChanged();
  update();
}

void SpringLayoutUi::onAttractionCalculationChanged(int value)
{
  m_layout.setAttractionCalculation(static_cast<SpringLayout::AttractionCalculation>(value));
  layoutRulesChanged();
  update();
}

void SpringLayoutUi::onRepulsionCalculationChanged(int value)
{
  m_layout.setRepulsionCalculation(static_cast<SpringLayout::RepulsionCalculation>(value));
  layoutRulesChanged();
  update();
}

void SpringLayoutUi::onForceApplicationChanged(int value){
  m_layout.setForceApplication(static_cast<SpringLayout::ForceApplication>(value));
  layoutRulesChanged();
  update();
}

void SpringLayoutUi::onStarted()
{
  m_ui.btnStartStop->setText("Stop");
  m_ui.btnStartStop->setEnabled(true);
  update();
}

void SpringLayoutUi::onStopped()
{
  m_ui.btnStartStop->setText("Start");
  m_ui.btnStartStop->setEnabled(true);
  runningChanged(false);
  update();
}

void SpringLayoutUi::onTreeToggled(bool b){
  m_layout.setTreeEnabled(b);
  m_ui.sldAccuracy->setVisible(b);
  m_ui.dispAccuracy->setVisible(b);
  m_ui.lblAccuracy->setVisible(b);
  update();
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
  update();
}

void SpringLayoutUi::setActive(bool active)
{
  if (active && m_thread == nullptr) {
    onStartStop();
  }
  else if (!active && m_thread != nullptr) {
    onStartStop();
  }
  update();
}

}  // namespace Graph
