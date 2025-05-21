// Author(s): Rimco Boudewijns and Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "springlayout.h"
#include "settingsmanager.h"
#include "utility.h"

#include <QThread>
#include <QWidget>
#include <cmath>
#include <cstdlib>

namespace Graph
{

//
// Debug functions
//

#define PROC_VAL(p) \
  case (p):         \
    s = #p;         \
    break;

std::string getName(AttractionFunctionID c)
{
  std::string s = "UNKNOWN";
  switch (c)
  {
    PROC_VAL(AttractionFunctionID::ltsgraph_attr);
    PROC_VAL(AttractionFunctionID::electricalsprings_attr);
    PROC_VAL(AttractionFunctionID::linearsprings_attr);
  }
  return s;
}

std::string getName(RepulsionFunctionID c)
{
  std::string s = "UNKNOWN";
  switch (c)
  {
    PROC_VAL(RepulsionFunctionID::ltsgraph_rep);
    PROC_VAL(RepulsionFunctionID::electricalsprings_rep);
    PROC_VAL(RepulsionFunctionID::none_rep);
  }
  return s;
}

#undef PROC_VAL

//
// SpringLayout
//

SpringLayout::SpringLayout(Graph& graph, GLWidget& glwidget)
    : m_node_tree(0, {0, 0, 0}, {0, 0, 0}),
      m_handle_tree(0, {0, 0, 0}, {0, 0, 0}),
      m_trans_tree(0, {0, 0, 0}, {0, 0, 0}),
      m_node_tree2D(0, {0, 0}, {0, 0}),
      m_handle_tree2D(0, {0, 0}, {0, 0}),
      m_trans_tree2D(0, {0, 0}, {0, 0}),
      m_speed(0.001f),
      m_attraction(0.13f),
      m_repulsion(50.0f),
      m_natLength(50.0f),
      m_handleDeviation(10.0f),
      m_graph(graph),
      m_ui(nullptr),
      attrFuncMap({
          {AttractionFunctionID::ltsgraph_attr, new AttractionFunctions::LTSGraph()},
          {AttractionFunctionID::electricalsprings_attr, new AttractionFunctions::ElectricalSprings()},
          {AttractionFunctionID::linearsprings_attr, new AttractionFunctions::LinearSprings()},
      }),
      m_attrFunc(attrFuncMap[AttractionFunctionID::ltsgraph_attr]),
      m_option_attractionCalculation(AttractionFunctionID::ltsgraph_attr),
      repFuncMap({
          {RepulsionFunctionID::ltsgraph_rep, new RepulsionFunctions::LTSGraph()},
          {RepulsionFunctionID::electricalsprings_rep, new RepulsionFunctions::ElectricalSpring()},
          {RepulsionFunctionID::none_rep, new RepulsionFunctions::None()},
      }),
      m_repFunc(repFuncMap[RepulsionFunctionID::ltsgraph_rep]),
      m_option_repulsionCalculation(RepulsionFunctionID::ltsgraph_rep),
      m_glwidget(glwidget)
{
  m_graph.gv_debug.addVar("Stability");
  m_graph.gv_debug.addVar("Energy");
  m_graph.gv_debug.addVar("min energy");
  m_graph.gv_debug.addVar("max energy");
  m_graph.gv_debug.addToPlot(0, 0, {"Stability", QBrush(Qt::red, Qt::SolidPattern), QPen(Qt::red, 1)});

  m_graph.gv_debug.addToPlot(1,
      0,
      {"Energy", QBrush(QColor::fromRgbF(0, 0.5, 1), Qt::SolidPattern), QPen(QColor::fromRgbF(0, 0.5, 1), 1)});

  m_graph.gv_debug.addToPlot(1,
      0,
      {"min energy",
          QBrush(QColor::fromRgbF(0, 0.25, 0.5), Qt::SolidPattern),
          QPen(QColor::fromRgbF(0, 0.5, 1), 1, Qt::DashLine, Qt::FlatCap)});

  m_graph.gv_debug.addToPlot(1,
      0,
      {"max energy",
          QBrush(QColor::fromRgbF(0, 0.25, 0.5), Qt::SolidPattern),
          QPen(QColor::fromRgbF(0, 0.5, 1), 1, Qt::DashLine, Qt::FlatCap)});

  srand(time(nullptr));
  drift_timer.restart();
}

SpringLayout::~SpringLayout()
{
  delete m_ui;
}

SpringLayoutUi* SpringLayout::ui(QAction* /* advancedDialogAction */, CustomQWidget* advancedWidget, QWidget* parent)
{
  if (m_ui == nullptr)
  {
    m_ui = new SpringLayoutUi(*this, advancedWidget, parent);

    SettingsManager::addSettings("SpringLayoutUi");
    Settings* settings = SettingsManager::getSettings("SpringLayoutUi");
    Ui::AdvancedSpringLayoutDialog& advanced_ui = m_ui->m_ui_advanced;
    Ui::DockWidgetLayout& ui = m_ui->m_ui;
    settings->registerVar(ui.sldBalance, unlerp(0.5f, 0, 1));
    settings->registerVar(ui.sldHandleWeight, unlerp(5.0, m_min_handleDeviation, m_max_handleDeviation));
    settings->registerVar(ui.sldNatLength, unlerp(20, m_min_natLength, m_max_natLength));

    settings->registerVar(advanced_ui.sld_acc, unlerp(1.2f, m_min_accuracy, m_max_accuracy), true);
    settings->registerVar(advanced_ui.sld_spd,
        unlerp(m_speed_inverse_scale_func(1.0f),
            m_speed_inverse_scale_func(m_min_speed),
            m_speed_inverse_scale_func(m_max_speed)),
        true);

    settings->registerVar(advanced_ui.chk_debugDraw, false, true);
    settings->registerVar(advanced_ui.chk_enableTree, true, true);

    settings->registerVar(advanced_ui.cmb_attr, (int)AttractionFunctionID::ltsgraph_attr, true);
    settings->registerVar(advanced_ui.cmb_rep, (int)RepulsionFunctionID::ltsgraph_rep, true);

    settings->registerVar(advanced_ui.txt_stab_thres, QString::number(m_stabilityThreshold), true);
    settings->registerVar(advanced_ui.txt_stab_iters, QString::number(m_stabilityMaxCount), true);

    m_ui->m_ui.dispHandleWeight->setText(QString::number(m_handleDeviation, 'g', 3));
    m_ui->m_ui.dispNatLength->setText(QString::number(m_natLength, 'g', 3));
  }
  return m_ui;
}

void SpringLayout::setAttractionCalculation(AttractionFunctionID c)
{
  if (attrFuncMap.find(c) == attrFuncMap.end())
  {
    mCRL2log(mcrl2::log::debug) << "Unkown attraction calculation selected \"" << getName(c)
                                << "\". Cause may be invalid settings were loaded or selected function "
                                   "is not implemented."
                                << std::endl;
    c = AttractionFunctionID::electricalsprings_attr;
    mCRL2log(mcrl2::log::debug) << "Setting default attraction calculation \"" << getName(c) << "\"." << std::endl;
  }

  m_option_attractionCalculation = c;
  m_attrFunc = attrFuncMap[c];
}

AttractionFunctionID SpringLayout::attractionCalculation()
{
  return m_option_attractionCalculation;
}

void SpringLayout::setRepulsionCalculation(RepulsionFunctionID c)
{
  if (repFuncMap.find(c) == repFuncMap.end())
  {
    mCRL2log(mcrl2::log::debug) << "Unkown repulsion calculation selected \"" << getName(c)
                                << "\". Cause may be invalid settings were loaded or selected function "
                                   "is not implemented."
                                << std::endl;
    c = RepulsionFunctionID::electricalsprings_rep;
    mCRL2log(mcrl2::log::debug) << "Setting default repulsion calculation \"" << getName(c) << "\"." << std::endl;
  }

  m_option_repulsionCalculation = c;
  m_repFunc = repFuncMap[c];
}

RepulsionFunctionID SpringLayout::repulsionCalculation()
{
  return m_option_repulsionCalculation;
}

template <>
QVector3D SpringLayout::approxRepulsionForce<Octree>(const QVector3D& a, Octree& tree, const float ideal_distance)
{
  QVector3D force(0, 0, 0);
  std::size_t num_nodes = 0;
  auto& super_nodes = tree.getSuperNodes(a);
  for (auto super_node : super_nodes)
  {
    force += super_node->children * (*m_repFunc)(a, super_node->pos, ideal_distance);
  }
  force *= m_repulsion;
  num_nodes = super_nodes.size();

  m_max_num_nodes = std::max(m_max_num_nodes, num_nodes);
  m_total_num_nodes += num_nodes;
  return force;
}

template <>
QVector3D SpringLayout::approxRepulsionForce<Quadtree>(const QVector3D& a, Quadtree& tree,  const float ideal_distance)
{
  QVector3D force(0, 0, 0);
  std::size_t num_nodes = 0;
  auto& super_nodes = tree.getSuperNodes({a.x(), a.y()});
  for (auto super_node : super_nodes)
  {
    force += super_node->children * (*m_repFunc)(a, {super_node->pos.x(), super_node->pos.y(), 0}, ideal_distance);
  }
  force *= m_repulsion;
  num_nodes = super_nodes.size();

  m_max_num_nodes = std::max(m_max_num_nodes, num_nodes);
  m_total_num_nodes += num_nodes;
  return force;
}

template <>
void SpringLayout::attractionAccumulation<SpringLayout::ThreadingMode::normal>(bool sel,
    std::size_t nodeCount,
    std::size_t edgeCount)
{
  std::vector<std::size_t> nodeLocations(m_graph.nodeCount());
  for (std::size_t i = 0; i < nodeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationNode(i) : i;
    nodeLocations[n] = i;
    m_nforces[i] = {0, 0, 0};
    m_sforces[i] = (*m_attrFunc)(m_graph.node(n).pos(), m_graph.stateLabel(n).pos(), m_labelDistance) * m_attraction;
  }

  QVector3D f;
  for (std::size_t i = 0; i < edgeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationEdge(i) : i;

    Edge e = m_graph.edge(n);
    // Variables for repulsion calculations
    std::size_t from = e.from();
    std::size_t to = e.to();

    m_hforces[i] = QVector3D(0, 0, 0);
    m_lforces[i] = QVector3D(0, 0, 0);

    if (e.is_selfloop())
    {
      m_hforces[i] += (*m_repFunc)(m_graph.handle(n).pos(), m_graph.node(from).pos(), 10*m_handleDeviation) * m_repulsion;
    }

    f = (*m_attrFunc)(m_graph.node(to).pos(), m_graph.node(from).pos(), m_natLength) * m_attraction;
    m_nforces[nodeLocations[from]] += f;
    m_nforces[nodeLocations[to]] -= f;

    f = (*m_attrFunc)((m_graph.node(to).pos() + m_graph.node(from).pos()) / 2.0, m_graph.handle(n).pos(), m_handleDeviation) * m_attraction;
    m_hforces[i] += f;

    f = (*m_attrFunc)(m_graph.handle(n).pos(), m_graph.transitionLabel(n).pos(), m_labelDistance) * m_attraction;
    m_lforces[i] += f;
  }
}

template <>
void SpringLayout::repulsionAccumulation<SpringLayout::TreeMode::quadtree>(bool sel,
    std::size_t nodeCount,
    std::size_t edgeCount)
{
  /// TODO: Fix blatant code duplication
  QVector2D node_min = {INFINITY, INFINITY}, node_max = {-INFINITY, -INFINITY}, handle_min = {INFINITY, INFINITY},
            handle_max = {-INFINITY, -INFINITY}, trans_min = {INFINITY, INFINITY}, trans_max = {-INFINITY, -INFINITY};

  // calculate bounding volumes
  {
    for (std::size_t i = 0; i < nodeCount; i++)
    {
      std::size_t n = sel ? m_graph.explorationNode(i) : i;
      node_min.setX(std::min(node_min.x(), m_graph.node(n).pos().x()));
      node_min.setY(std::min(node_min.y(), m_graph.node(n).pos().y()));
      node_max.setX(std::max(node_max.x(), m_graph.node(n).pos().x()));
      node_max.setY(std::max(node_max.y(), m_graph.node(n).pos().y()));
    }

    // calculate bounding volume
    for (std::size_t i = 0; i < edgeCount; i++)
    {
      std::size_t n = sel ? m_graph.explorationEdge(i) : i;
      handle_min.setX(std::min(handle_min.x(), m_graph.handle(n).pos().x()));
      handle_min.setY(std::min(handle_min.y(), m_graph.handle(n).pos().y()));
      handle_max.setX(std::max(handle_max.x(), m_graph.handle(n).pos().x()));
      handle_max.setY(std::max(handle_max.y(), m_graph.handle(n).pos().y()));
    }

    // calculate bounding volume
    for (std::size_t i = 0; i < edgeCount; i++)
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

  // The code commented out below calculates a cube around all points to have a 'perfect' octree

  // QVector2D _extents, cubic_extents;
  // float _width;

  // _extents = node_max - node_min;
  // _width = std::max({_extents.x(), _extents.y()});
  // cubic_extents = QVector2D(_width, _width);
  // node_min -= (cubic_extents - _extents)*0.5f;
  // node_max = node_min + cubic_extents;

  // _extents = handle_max - handle_min;
  // _width = std::max({_extents.x(), _extents.y()});
  // cubic_extents = QVector2D(_width, _width);
  // handle_min -= (cubic_extents - _extents)*0.5f;
  // handle_max = handle_min + cubic_extents;

  // _extents = trans_max - trans_min;
  // _width = std::max({_extents.x(), _extents.y()});
  // cubic_extents = QVector2D(_width, _width);
  // trans_min -= (cubic_extents - _extents)*0.5f;
  // trans_max = trans_min + cubic_extents;

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
  for (std::size_t i = 0; i < nodeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationNode(i) : i;
    m_nforces[i] += approxRepulsionForce<Quadtree>(m_graph.node(n).pos(), m_node_tree2D, m_natLength);
  }

  for (std::size_t i = 0; i < edgeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationEdge(i) : i;

    m_handle_tree2D.insert((QVector2D)m_graph.handle(n).pos());
    m_trans_tree2D.insert((QVector2D)m_graph.transitionLabel(n).pos());
  }

  m_handle_tree2D.calculatePositions();
  m_trans_tree2D.calculatePositions();

  // approximate repulsive forces
  // float temp = m_repulsion;
  // m_repulsion *= m_handleDeviation; 
  for (std::size_t i = 0; i < edgeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationEdge(i) : i;
    m_hforces[i] += approxRepulsionForce<Quadtree>(m_graph.handle(n).pos(), m_handle_tree2D, m_handleDeviation);
    m_lforces[i] += approxRepulsionForce<Quadtree>(m_graph.transitionLabel(n).pos(), m_trans_tree2D, m_labelDistance);
  }
  // m_repulsion = temp;
}

template <>
void SpringLayout::repulsionAccumulation<SpringLayout::TreeMode::octree>(bool sel,
    std::size_t nodeCount,
    std::size_t edgeCount)
{
  /// TODO: Fix blatant code duplication
  QVector3D node_min = {INFINITY, INFINITY, INFINITY}, node_max = {-INFINITY, -INFINITY, -INFINITY},
            handle_min = {INFINITY, INFINITY, INFINITY}, handle_max = {-INFINITY, -INFINITY, -INFINITY},
            trans_min = {INFINITY, INFINITY, INFINITY}, trans_max = {-INFINITY, -INFINITY, -INFINITY};

  // calculate bounding volumes
  {
    for (std::size_t i = 0; i < nodeCount; i++)
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
    for (std::size_t i = 0; i < edgeCount; i++)
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
    for (std::size_t i = 0; i < edgeCount; i++)
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

  // QVector3D _extents, cubic_extents;
  // float _width;

  // _extents = node_max - node_min;
  // _width = std::max({_extents.x(), _extents.y(), _extents.z()});
  // cubic_extents = QVector3D(_width, _width, _width);
  // node_min -= (cubic_extents - _extents)*0.5f;
  // node_max = node_min + cubic_extents;

  // _extents = handle_max - handle_min;
  // _width = std::max({_extents.x(), _extents.y(), _extents.z()});
  // cubic_extents = QVector3D(_width, _width, _width);
  // handle_min -= (cubic_extents - _extents)*0.5f;
  // handle_max = handle_min + cubic_extents;

  // _extents = trans_max - trans_min;
  // _width = std::max({_extents.x(), _extents.y(), _extents.z()});
  // cubic_extents = QVector3D(_width, _width, _width);
  // trans_min -= (cubic_extents - _extents)*0.5f;
  // trans_max = trans_min + cubic_extents;

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
  for (std::size_t i = 0; i < nodeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationNode(i) : i;
    m_nforces[i] += approxRepulsionForce<Octree>(m_graph.node(n).pos(), m_node_tree, m_natLength);
  }

  for (std::size_t i = 0; i < edgeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationEdge(i) : i;
    m_handle_tree.insert(m_graph.handle(n).pos());
    m_trans_tree.insert(m_graph.transitionLabel(n).pos());
  }

  m_handle_tree.calculatePositions();
  m_trans_tree.calculatePositions();

  for (std::size_t i = 0; i < edgeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationEdge(i) : i;
    m_hforces[i] += approxRepulsionForce<Octree>(m_graph.handle(n).pos(), m_handle_tree, m_handleDeviation);
    m_lforces[i] += approxRepulsionForce<Octree>(m_graph.transitionLabel(n).pos(), m_trans_tree, m_labelDistance);
  }
}

template <>
void SpringLayout::repulsionAccumulation<SpringLayout::TreeMode::none>(bool sel, std::size_t nodeCount, std::size_t edgeCount)
{
  // used for storing intermediate results
  QVector3D f;
  // repulsive forces for nodes
  for (std::size_t i = 0; i < nodeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationNode(i) : i;
    for (std::size_t j = i + 1; j < nodeCount; ++j)
    {
      std::size_t m = sel ? m_graph.explorationNode(j) : j;
      f = (*m_repFunc)(m_graph.node(n).pos(), m_graph.node(m).pos(), m_natLength) * m_repulsion;
      m_nforces[i] += f;
      m_nforces[j] -= f;
    }
  }

  // repulsive forces
  for (std::size_t i = 0; i < edgeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationEdge(i) : i;
    for (std::size_t j = i + 1; j < edgeCount; ++j)
    {
      std::size_t m = sel ? m_graph.explorationEdge(j) : j;
      f = (*m_repFunc)(m_graph.handle(n).pos(), m_graph.handle(m).pos(), m_handleDeviation) * m_repulsion;
      m_hforces[i] += f;
      m_hforces[j] -= f;

      f = (*m_repFunc)(m_graph.transitionLabel(n).pos(), m_graph.transitionLabel(m).pos(), m_labelDistance) * m_repulsion;
      m_lforces[i] += f;
      m_lforces[j] -= f;
    }
  }
}

void SpringLayout::forceAccumulation(bool sel,
    std::size_t nodeCount,
    std::size_t edgeCount,
    TreeMode treeMode,
    ThreadingMode threadingMode)
{
  switch (threadingMode)
  {
  case ThreadingMode::normal:
    attractionAccumulation<ThreadingMode::normal>(sel, nodeCount, edgeCount);
    break;
  }
  if (m_option_repulsionCalculation != RepulsionFunctionID::none_rep)
  {
    switch (treeMode)
    {
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
  if (!m_graph.stable() || m_graph.hasForcedUpdate())
  {
    m_graph.lock(GRAPH_LOCK_TRACE); // enter critical section
    bool sel = m_graph.hasExploration();
    std::size_t nodeCount = sel ? m_graph.explorationNodeCount() : m_graph.nodeCount();
    std::size_t edgeCount = sel ? m_graph.explorationEdgeCount() : m_graph.edgeCount();

    bool m_tree_enabled=m_tree_enable_for_large_graphs && (nodeCount+edgeCount)>200;

    if (m_graph.hasForcedUpdate())
    {
      m_graph.hasForcedUpdate() = false;
    }

    if (m_graph.scrambleZ())
    {
      m_graph.scrambleZ() = false;
      float min_x = INFINITY;
      float max_x = -INFINITY;
      float min_y = INFINITY;
      float max_y = -INFINITY;
      for (std::size_t i = 0; i < m_graph.nodeCount(); i++)
      {
        min_x = std::min(min_x, m_graph.node(i).pos().x());
        max_x = std::max(max_x, m_graph.node(i).pos().x());
        min_y = std::min(min_y, m_graph.node(i).pos().y());
        max_y = std::max(max_y, m_graph.node(i).pos().y());
      }
      randomizeZ(0.1f * std::sqrt((max_x - min_x) * (max_x - min_x) + (max_y - min_y) * (max_y - min_y)));
    }

    if (m_graph.resetPositions())
    {
      m_graph.resetPositions() = false;
      resetPositions();
    }

    m_nforces.resize(nodeCount);
    m_sforces.resize(nodeCount);
    m_hforces.resize(edgeCount);
    m_lforces.resize(edgeCount);
    std::fill(m_nforces.begin(), m_nforces.end(), QVector3D(0, 0, 0));
    std::fill(m_sforces.begin(), m_sforces.end(), QVector3D(0, 0, 0));
    std::fill(m_hforces.begin(), m_hforces.end(), QVector3D(0, 0, 0));
    std::fill(m_lforces.begin(), m_lforces.end(), QVector3D(0, 0, 0));

    if (m_tree_enabled)
    {
      bool is_2D = (m_graph.getClipMax().z() - m_graph.getClipMin().z() < 0.001f);
      if (is_2D)
      {
        forceAccumulation(sel, nodeCount, edgeCount, TreeMode::quadtree, ThreadingMode::normal);
      }
      else
      {
        forceAccumulation(sel, nodeCount, edgeCount, TreeMode::octree, ThreadingMode::normal);
      }
    }
    else
    {
      forceAccumulation(sel, nodeCount, edgeCount, TreeMode::none, ThreadingMode::normal);
    }

    QVector3D clipmin = m_graph.getClipMin();
    QVector3D clipmax = m_graph.getClipMax();
    bool new_anchored = false;

    // Offset the nodeCount to avoid multiplying by zero
    float use_speed = m_speed * std::log2f(nodeCount+2) * 0.25f;

    for (std::size_t i = 0; i < nodeCount; ++i)
    {
      std::size_t n = sel ? m_graph.explorationNode(i) : i;

      if (!m_graph.node(n).anchored())
      {
        ApplicationFunctions::apply_forces(m_graph.node(n).pos_mutable(), m_nforces[i], use_speed);
        clipVector(m_graph.node(n).pos_mutable(), clipmin, clipmax);
      }
      else
      {
        new_anchored = true;
      }
    }

    float drift_secs = drift_timer.elapsed() * 0.001f; // seconds
    QVector3D center_of_mass = slicedAverage(m_graph);
    if (new_anchored ^ any_anchored)
    {
      // changed
      center_of_mass_offset = center_of_mass;
      any_anchored = new_anchored;
      mCRL2log(mcrl2::log::debug) << "Setting new center of mass offset: " << center_of_mass_offset.x() << ", "
                                  << center_of_mass_offset.y() << ", " << center_of_mass_offset.z() << std::endl;
      drift_timer.restart();
      drift_secs = 0;
    }
    if (!any_anchored)
    {
      center_of_mass -= smoothstep(0, time_to_center, time_to_center - drift_secs) * center_of_mass_offset;
    }
    else
    {
      center_of_mass = QVector3D(0, 0, 0);
    }

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
        ApplicationFunctions::apply_forces(m_graph.stateLabel(n).pos_mutable(), m_sforces[i], use_speed);
        m_graph.stateLabel(n).pos_mutable() -= center_of_mass;
        clipVector(m_graph.stateLabel(n).pos_mutable(), clipmin, clipmax);
      }
    }

    for (std::size_t i = 0; i < edgeCount; ++i)
    {
      std::size_t n = sel ? m_graph.explorationEdge(i) : i;

      if (!m_graph.handle(n).anchored())
      {
        ApplicationFunctions::apply_forces(m_graph.handle(n).pos_mutable(), m_hforces[i], use_speed);
        m_graph.handle(n).pos_mutable() -= center_of_mass;
        clipVector(m_graph.handle(n).pos_mutable(), clipmin, clipmax);
      }
      if (!m_graph.transitionLabel(n).anchored())
      {
        ApplicationFunctions::apply_forces(m_graph.transitionLabel(n).pos_mutable(), m_lforces[i], use_speed);
        m_graph.transitionLabel(n).pos_mutable() -= center_of_mass;
        clipVector(m_graph.transitionLabel(n).pos_mutable(), clipmin, clipmax);
      }
    }
    double energy = slicedAverageSqrMagnitude(m_nforces); //+slicedAverageSqrMagnitude(m_hforces);
    double min = 1e15;
    double max = -1e15;
    for (const QVector3D& f: m_nforces)
    {
      double mag = f.lengthSquared();
      min = std::min(min, mag);
      max = std::max(max, mag);
    }
    if (m_graph.userIsDragging)
    {
      m_graph.userIsDragging=false;
    }

    m_max_num_nodes = 0;
    m_total_num_nodes = 0;

    // float stability = std::abs((m_previous_energy - energy) / m_previous_energy);
    float stability = std::abs(m_previous_energy - energy)/(edgeCount+nodeCount);

    if (m_glwidget.getDebugDrawGraphs())
    {
      m_graph.gv_debug.logVar("Stability", stability);
      m_graph.gv_debug.logVar("Energy", energy);

      m_graph.gv_debug.logVar("min energy", min);
      m_graph.gv_debug.logVar("max energy", max);
    }

    // The graph becomes stable if the center of mass is sufficiently close to (0,0,0) or is anchored
    // and the energy of the graph does not fluctuate too much.
    if (stability <= m_stabilityThreshold && (center_of_mass.length()<0.01 || any_anchored))
    {
      m_stabilityCounter++;
      if (m_stabilityCounter >= m_stabilityMaxCount)
      {
        m_graph.setStable(true);
        mCRL2log(mcrl2::log::debug) << "The graph is now stable." << std::endl;
        m_ui->m_ui.lblStable->setText("Stable");
      }
    }
    else
    {
      m_stabilityCounter = 0;
      m_ui->m_ui.lblStable->setText("");
    }
    m_previous_energy = energy;

    notifyNewFrame();
    m_graph.unlock(GRAPH_LOCK_TRACE);
  }
}

void SpringLayout::randomizeZ(float z)
{
  m_graph.lock(GRAPH_LOCK_TRACE);
  bool exploration = m_graph.hasExploration();
  std::size_t nodeCount = exploration ? m_graph.explorationNodeCount() : m_graph.nodeCount();
  std::size_t edgeCount = exploration ? m_graph.explorationEdgeCount() : m_graph.edgeCount();
  for (std::size_t i = 0; i < nodeCount; ++i)
  {
    std::size_t n = exploration ? m_graph.explorationNode(i) : i;
    if (!m_graph.node(n).anchored())
    {
      float z_offset = frand(-z, z);
      m_graph.node(n).pos_mutable().setZ(m_graph.node(n).pos().z() + z_offset);
      m_graph.stateLabel(n).pos_mutable().setZ(m_graph.stateLabel(n).pos().z() + z_offset);
    }
  }
  for (std::size_t i = 0; i < edgeCount; ++i)
  {
    std::size_t n = exploration ? m_graph.explorationEdge(i) : i;
    if (!m_graph.transitionLabel(n).anchored())
    {
      m_graph.transitionLabel(n).pos_mutable()
          = 0.5 * (m_graph.node(m_graph.edge(n).from()).pos() + m_graph.node(m_graph.edge(n).to()).pos());
      m_graph.handle(n).pos_mutable() = m_graph.transitionLabel(n).pos();
    }
  }
  m_graph.unlock(GRAPH_LOCK_TRACE);
}

void SpringLayout::notifyNewFrame()
{
  if (!m_graph.hasNewFrame())
  {
    m_graph.hasNewFrame(true);
    m_ui->m_thread->signal_draw_new_frame();
  }
}

void SpringLayout::setTreeEnabled(bool b)
{
  m_tree_enable_for_large_graphs = b;
  mCRL2log(mcrl2::log::verbose) << (b ? "Enabled" : "Disabled") << " tree acceleration for large graphs." << std::endl;
}

void SpringLayout::setSpeed(int v)
{
  m_speed
      = m_speed_scale_func(lerp(v, m_speed_inverse_scale_func(m_min_speed), m_speed_inverse_scale_func(m_max_speed)));
  mCRL2log(mcrl2::log::verbose) << "Set speed to: " << v << " corresponding to: " << m_speed << std::endl;
  if (this->m_ui)
  {
    m_ui->m_ui_advanced.disp_spd->setText(QString::number(m_speed, 'g', 3));
  }
}

void SpringLayout::setAccuracy(int v)
{
  m_accuracy = lerp(v, m_min_accuracy, m_max_accuracy);
  mCRL2log(mcrl2::log::verbose) << "Set accuracy to: " << v << " corresponding to: " << m_accuracy << std::endl;
  if (this->m_ui)
  {
    m_ui->m_ui_advanced.disp_acc->setText(QString::number(m_accuracy, 'g', 3));
  }
}

void SpringLayout::setAttraction(int v)
{
  m_attraction = lerp(v, 0.99, 0.01);
  if (this->m_ui)
  {
    m_ui->m_ui.lbl_attractRepulse->setText(QString::number(1 - m_attraction, 'g', 2));
  }
  mCRL2log(mcrl2::log::verbose) << "Set attraction scale to: " << v << " corresponding to: " << m_attraction
                                << std::endl;
}

void SpringLayout::setRepulsion(int v)
{
  m_repulsion = lerp(v, 0.01, 0.99);
  mCRL2log(mcrl2::log::verbose) << "Set repulsion scale to: " << v << " corresponding to: " << m_repulsion << std::endl;
}

void SpringLayout::setControlPointWeight(int v)
{
  m_handleDeviation = lerp(v, m_min_handleDeviation, m_max_handleDeviation);
  mCRL2log(mcrl2::log::verbose) << "Set the handle deviation to: " << m_handleDeviation << std::endl;
  if (this->m_ui)
  {
    m_ui->m_ui.dispHandleWeight->setText(QString::number(m_handleDeviation, 'g', 3));
  }
}

void SpringLayout::setNaturalTransitionLength(int v)
{
  m_natLength = lerp(v, m_min_natLength, m_max_natLength);
  mCRL2log(mcrl2::log::verbose) << "Set natural length to: " << v << " corresponding to: " << m_natLength << std::endl;
  if (this->m_ui)
  {
    m_ui->m_ui.dispNatLength->setText(QString::number(m_natLength, 'g', 3));
  }
  m_glwidget.update();
}

void SpringLayout::rulesChanged()
{
  m_graph.setStable(false);
  m_stabilityCounter=0;
}

void SpringLayout::resetPositions()
{
  mCRL2log(mcrl2::log::debug) << "Resetting positions" << std::endl;
  m_graph.lock(GRAPH_LOCK_TRACE);
  bool exploration = m_graph.hasExploration();
  std::size_t n_nodes = exploration ? m_graph.explorationNodeCount() : m_graph.nodeCount();
  std::size_t n_edges = exploration ? m_graph.explorationEdgeCount() : m_graph.edgeCount();
  bool is3D = m_glwidget.get3D();
  float hwidth = 5 * std::pow(m_natLength * n_nodes, 1.0f / (is3D ? 3 : 2));
  for (std::size_t i = 0; i < n_nodes; i++)
  {
    std::size_t n = exploration ? m_graph.explorationNode(i) : i;
    if (!m_graph.node(n).anchored())
    {
      m_graph.node(n).pos_mutable().setX(frand(-hwidth, hwidth));
      m_graph.node(n).pos_mutable().setY(frand(-hwidth, hwidth));
      m_graph.node(n).pos_mutable().setZ(is3D ? frand(-hwidth, hwidth) : 0);
      m_graph.stateLabel(n).pos_mutable() = m_graph.node(n).pos();
    }
  }
  for (std::size_t i = 0; i < n_edges; i++)
  {
    std::size_t n = exploration ? m_graph.explorationEdge(i) : i;
    if (!m_graph.transitionLabel(n).anchored())
    {
      m_graph.transitionLabel(n).pos_mutable()
          = 0.5 * (m_graph.node(m_graph.edge(n).from()).pos() + m_graph.node(m_graph.edge(n).to()).pos());
      m_graph.handle(n).pos_mutable() = m_graph.transitionLabel(n).pos();
    }
  }
  rulesChanged();
  m_graph.unlock(GRAPH_LOCK_TRACE);
}

//
// SpringLayoutUi
//

SpringLayoutUi::SpringLayoutUi(SpringLayout& layout, CustomQWidget* advancedDialogWidget, QWidget* parent)
    : QDockWidget(parent),
      m_layout(layout),
      m_thread(nullptr),
      m_ui_advanced_dialog(advancedDialogWidget)
{
  m_ui.setupUi(this);
  m_ui_advanced.setupUi(m_ui_advanced_dialog);
  m_ui_advanced_dialog->hide();

  m_ui.sldBalance->setValue(m_layout.repulsion());
  m_ui.sldHandleWeight->setValue(m_layout.handleDeviation());
  m_ui.sldNatLength->setValue(m_layout.naturalTransitionLength());
  // m_ui_advanced.chk_enableTree->setChecked(false);
  m_layout.setTreeEnabled(m_ui_advanced.chk_enableTree->isChecked());

  m_ui_advanced.sld_spd->setValue(m_layout.speed());
  connect(m_ui_advanced.sld_spd, SIGNAL(valueChanged(int)), this, SLOT(onSpeedChanged(int)));

  m_ui_advanced.sld_acc->setValue(50);
  connect(m_ui_advanced.sld_acc, SIGNAL(valueChanged(int)), this, SLOT(onAccuracyChanged(int)));

  m_ui_advanced.cmb_attr->setCurrentIndex(m_layout.attractionCalculation());
  connect(m_ui_advanced.cmb_attr, SIGNAL(currentIndexChanged(int)), this, SLOT(onAttractionCalculationChanged(int)));

  m_ui_advanced.cmb_rep->setCurrentIndex(m_layout.repulsionCalculation());
  connect(m_ui_advanced.cmb_rep, SIGNAL(currentIndexChanged(int)), this, SLOT(onRepulsionCalculationChanged(int)));

  connect(m_ui_advanced.chk_debugDraw, SIGNAL(toggled(bool)), &m_layout.m_glwidget, SLOT(toggleDebugDrawGraphs(bool)));

  connect(m_ui_advanced.chk_enableTree, SIGNAL(toggled(bool)), this, SLOT(onTreeToggled(bool)));

  connect(m_ui_advanced.txt_stab_thres, &QLineEdit::textChanged, this, &SpringLayoutUi::onStabilityThresholdChanged);
  connect(m_ui_advanced.txt_stab_iters, &QLineEdit::textChanged, this, &SpringLayoutUi::onStabilityIterationsChanged);

  connect(m_ui_advanced.cmd_reset_positions, &QPushButton::pressed, this, &SpringLayoutUi::onResetPositionsPressed);

  // connect(m_ui_advanced_dialog, SIGNAL(finished(int)), this,
  //         SLOT(onAdvancedDialogShow(false)));
  // connect(m_ui_advanced_dialog, SIGNAL(accepted()), this,
  //         SLOT(onAdvancedDialogShow(false)));
  // connect(m_ui_advanced_dialog, SIGNAL(rejected()), this,
  //         SLOT(onAdvancedDialogShow(false)));
}

SpringLayoutUi::~SpringLayoutUi()
{
  if (m_thread != nullptr)
  {
    dynamic_cast<WorkerThread*>(m_thread)->stop();
    m_thread->wait();
  }
}
/// @brief Notifies layout that rules have changed and redraw the graph.
void SpringLayoutUi::layoutChanged()
{
  m_layout.rulesChanged();
  m_layout.m_graph.hasNewFrame(true);
  m_layout.m_glwidget.update(); // Redraw the graph. */
}

QByteArray SpringLayoutUi::settings()
{
  return SettingsManager::getSettings("SpringLayoutUi")->save();
}

void SpringLayoutUi::setSettings(QByteArray state)
{
  SettingsManager::getSettings("SpringLayoutUi")->load(state);
  layoutChanged();
}

void SpringLayoutUi::onStabilityThresholdChanged(const QString& text)
{
  bool success;
  float num = text.toFloat(&success);
  if (success && num > 0)
  {
    m_layout.m_stabilityThreshold = num;
    mCRL2log(mcrl2::log::debug) << "Setting stability threshold to: " << num << std::endl;
  }
}

void SpringLayoutUi::onStabilityIterationsChanged(const QString& text)
{
  bool success;
  float num = text.toInt(&success);
  if (success && num > 0)
  {
    m_layout.m_stabilityMaxCount = num;
    mCRL2log(mcrl2::log::debug) << "Setting stability iterations to: " << num << std::endl;
  }
}

void SpringLayoutUi::onResetPositionsPressed()
{
  m_layout.resetPositions();
  layoutChanged();
}

void SpringLayoutUi::onAttractionChanged(int value)
{
  m_layout.setAttraction(value);
  layoutChanged();
}

void SpringLayoutUi::onRepulsionChanged(int value)
{
  m_layout.setRepulsion(value);
  layoutChanged();
}

void SpringLayoutUi::onSpeedChanged(int value)
{
  m_layout.setSpeed(value);
  layoutChanged();
}

void SpringLayoutUi::onAccuracyChanged(int value)
{
  m_layout.setAccuracy(value);
  layoutChanged();
}

void SpringLayoutUi::onHandleWeightChanged(int value)
{
  m_layout.setControlPointWeight(value);
  layoutChanged();
}

void SpringLayoutUi::onNatLengthChanged(int value)
{
  m_layout.setNaturalTransitionLength(value);
  layoutChanged();
}

void SpringLayoutUi::onAttractionCalculationChanged(int value)
{
  m_layout.setAttractionCalculation(static_cast<AttractionFunctionID>(value));
  m_ui_advanced.cmb_attr->setCurrentIndex((int)m_layout.attractionCalculation());
  layoutChanged();
}

void SpringLayoutUi::onRepulsionCalculationChanged(int value)
{
  m_layout.setRepulsionCalculation(static_cast<RepulsionFunctionID>(value));
  m_ui_advanced.cmb_rep->setCurrentIndex((int)m_layout.repulsionCalculation());
  layoutChanged();
}

void SpringLayoutUi::onStarted()
{
  m_ui.btnStartStop->setText("Stop shaping");
  m_ui.btnStartStop->setEnabled(true);
  layoutChanged();
}

void SpringLayoutUi::onStopped()
{
  m_ui.btnStartStop->setText("Start shaping");
  m_ui.btnStartStop->setEnabled(true);
  runningChanged(false);
  update();
}

void SpringLayoutUi::onDrawNewFrame()
{
  m_layout.m_glwidget.update();
}

void SpringLayoutUi::onTreeToggled(bool b)
{
  m_layout.setTreeEnabled(b);
  update();
}

void SpringLayoutUi::onStartStop()
{
  m_ui.btnStartStop->setEnabled(false);
  if (m_thread == nullptr)
  {
    emit runningChanged(true);
    layoutChanged(); // force update
    m_thread = new WorkerThread(m_layout, this);
    m_thread->connect(m_thread, SIGNAL(started()), this, SLOT(onStarted()));
    m_thread->connect(m_thread, SIGNAL(finished()), this, SLOT(onStopped()));
    m_thread->connect(m_thread, &WorkerThread::draw_new_frame, this, &SpringLayoutUi::onDrawNewFrame);
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
  if (active && m_thread == nullptr)
  {
    onStartStop();
  }
  else if (!active && m_thread != nullptr)
  {
    onStartStop();
  }
  update();
}

void SpringLayoutUi::onAdvancedDialogShow(bool b)
{
  mCRL2log(mcrl2::log::debug) << "OnAdvancedDialogShow called." << std::endl;
  if (b)
  {
    mCRL2log(mcrl2::log::debug) << "Opening advanced dialog." << std::endl;
    m_ui_advanced_dialog->show();
  }
  else
  {
    mCRL2log(mcrl2::log::debug) << "Closing advanced dialog." << std::endl;
    m_ui_advanced_dialog->hide();
  }
}

} // namespace Graph
